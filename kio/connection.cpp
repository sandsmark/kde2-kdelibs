/* This file is part of the KDE libraries
    Copyright (C) 2000 Stephan Kulow <coolo@kde.org>
                       David Faure <faure@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

// $Id$

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ksock.h>
#include <qtimer.h>

#include <sys/types.h>
#include <sys/signal.h>
#include <sys/time.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "kio/connection.h"

#include <kdebug.h>

using namespace KIO;

template class QList<Task>;


Connection::Connection()
{
    f_out = 0;
    fd_in = 0;
    socket = 0;
    notifier = 0;
    receiver = 0;
    member = 0;
    queueonly = false;
    m_suspended = false;
    unqueuedTasks = 0;
}

Connection::~Connection()
{
    close();
}

void Connection::suspend()
{
    m_suspended = true;
    if (notifier)
       notifier->setEnabled(false);
}

void Connection::resume()
{
    m_suspended = false;
    if (notifier)
       notifier->setEnabled(true);
}

void Connection::close()
{
    delete notifier;
    notifier = 0;
    delete socket;
    socket = 0;
    if (f_out)
       fclose(f_out);
    f_out = 0;
    fd_in = 0;
}

void Connection::send(int cmd, const QByteArray& data)
{
    if (!inited() || queueonly || tasks.count() > 0) {
	kDebugInfo( 7017, "pending queue %d", cmd);
	Task *task = new Task();
	task->cmd = cmd;
	task->data = data;
	tasks.append(task);
    } else {
	sendnow( cmd, data );
    }

    if (inited() && tasks.count() > 0 && !queueonly)
	QTimer::singleShot(100, this, SLOT(dequeue()));
}

void Connection::queueOnly(bool queue) {
    unqueuedTasks = tasks.count();
    kDebugInfo( 7017, "setting queueOnly to %d", queue);
    queueonly = queue;
    dequeue();
}

void Connection::dequeue()
{
    if (tasks.count() == 0  || !inited() || (queueonly && unqueuedTasks == 0))
	return;

    kDebugInfo(7017, "dequeue");

    tasks.first();
    Task *task = tasks.take();
    sendnow( task->cmd, task->data );
    delete task;

    if (queueonly)
	unqueuedTasks--;

    if (tasks.count() > 0 && (!queueonly || unqueuedTasks > 0))
	QTimer::singleShot(100, this, SLOT(dequeue()));
}


void Connection::init(int _fd_in, int fd_out)
{
    fd_in = _fd_in;
    f_out = fdopen( fd_out, "wb" );
}

void Connection::init(KSocket *sock)
{
    delete notifier;
    notifier = 0;
    delete socket;
    socket = sock;
    fd_in = socket->socket();
    f_out = fdopen( socket->socket(), "wb" );
    if (receiver && fd_in) {
	notifier = new QSocketNotifier(fd_in, QSocketNotifier::Read);
        if ( m_suspended )
            suspend();
	QObject::connect(notifier, SIGNAL(activated(int)), receiver, member);
    }
    dequeue();
}

void Connection::connect(QObject *_receiver, const char *_member)
{
    receiver = _receiver;
    member = _member;
    delete notifier;
    notifier = 0;
    if (receiver && fd_in) {
	notifier = new QSocketNotifier(fd_in, QSocketNotifier::Read);
        if ( m_suspended )
            suspend();
	QObject::connect(notifier, SIGNAL(activated(int)), receiver, member);
    }
}

bool Connection::sendnow( int _cmd, const QByteArray &data )
{
    if (f_out == 0) {
	kDebugInfo(7017, "write: not yet inited.");
	return false;
    }

    kDebugInfo(7017, "sendnow %d", _cmd);

    static char buffer[ 64 ];
    sprintf( buffer, "%6x_%2x_", data.size(), _cmd );

    size_t n = fwrite( buffer, 1, 10, f_out );

    if ( n != 10 ) {
	kDebugError( 7017, "Could not send header");
	return false;
    }

    n = fwrite( data.data(), 1, data.size(), f_out );

    if ( n != data.size() ) {
	kDebugError( 7017, "Could not write data");
	return false;
    }

    fflush( f_out );

    return true;
}

int Connection::read( int* _cmd, QByteArray &data )
{
    kdDebug(7017) << "read\n";

    if (!fd_in) {
	kDebugInfo(7017, "read: not yet inited");
	return -1;
    }

    static char buffer[ 10 ];

 again1:
    ssize_t n = ::read( fd_in, buffer, 10);
    if ( n == -1 && errno == EINTR )
	goto again1;

    if ( n == -1) {
	kDebugError( 7017, "Header read failed, errno=%d", errno);
    }

    if ( n != 10 ) {
      if ( n ) // 0 indicates end of file
        kDebugError( 7017, "Header has invalid size (%d)", n);
      return -1;
    }

    buffer[ 6 ] = 0;
    buffer[ 9 ] = 0;

    char *p = buffer;
    while( *p == ' ' ) p++;
    long int len = strtol( p, 0L, 16 );

    p = buffer + 7;
    while( *p == ' ' ) p++;
    long int cmd = strtol( p, 0L, 16 );
    kdDebug(7017) << "read cmd " << cmd << endl;

    data.resize( len );

    if ( len > 0L ) {
	int bytesToGo = len;
	int bytesRead = 0;
	do {
	    n = ::read(fd_in, data.data()+bytesRead, bytesToGo);
	    if (n == -1) {
		if (errno == EINTR)
		    continue;
		
		kDebugError( 7017, "Data read failed, errno=%d", errno);
		return -1;
	    }
	    if (n != bytesToGo) {
		kDebugInfo( 7017, "Not enough data read (%d instead of %d) cmd=%ld", n, bytesToGo, cmd);
	    }
	
	    bytesRead += n;
	    bytesToGo -= n;
	}
	while(bytesToGo);
    }

    *_cmd = cmd;
    kdDebug(7017) << "finished reading cmd " << cmd << endl;

    return len;
}

#include "connection.moc"
