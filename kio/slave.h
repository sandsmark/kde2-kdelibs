// -*- c++ -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2000 Waldo Bastian <bastian@kde.org>
 *                2000 Stephan Kulow <coolo@kde.org>
 *
 * $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#ifndef KIO_SLAVE_H
#define KIO_SLAVE_H

#include <time.h>
#include <unistd.h>

#include <qobject.h>

#include <kurl.h>

#include "kio/slaveinterface.h"
#include "kio/connection.h"

class KServerSocket;
class KSocket;

namespace KIO {

    class Slave : public KIO::SlaveInterface
    {
	Q_OBJECT
	
    public:
	Slave(KServerSocket *unixdomain,
	      const QString &protocol, const QString &socketname);

	void setPID(pid_t);

        int slave_pid() { return m_pid; }

	/**
	 * Force termination
	 */
	void kill();

        /**
         * @return true if the slave survided the last mission.
         */
        bool isAlive() { return !dead; }

        /**
         * Set host for url
         * @param host to connect to.
         * @param port to connect to.
         * @param user to login as
         * @param passwd to login with
         */
        void setHost( const QString &host, int port,
                      const QString &user, const QString &passwd);

        /**
         * Open connection (forced).
         */
        void openConnection();

        /**
         * Close connection (forced).
         */
        void closeConnection();

        /**
         * @return Protocol handled by this slave
         */
        QString protocol() { return m_protocol; }

        /**
         * @return Host this slave is (was?) connected to
         */
        QString host() { return m_host; }

        /**
         * @return port this slave is (was?) connected to
         */
        int port() { return m_port; }

        /**
         * @return User this slave is (was?) logged in as
         */
        QString user() { return m_user; }

        /**
         * @return Passwd used to log in
         */
        QString passwd() { return m_passwd; }

	/**
	 * Creates a new slave if the @ref KIOSlavePool has no matching one.
	 * @ref m_pSlave and @ref m_strSlaveProtocol are set accordingly on success.
	 *
	 * @param _error is the error code on failure and undefined else.
	 * @param _error_text is the error text on failure and undefined else.
	 *
	 * @return @ref m_pSlave on success or 0L on failure.
	 */
	static Slave* createSlave( const KURL& url, int& error, QString& error_text );

	void queueOnly(bool queue) { slaveconn.queueOnly(queue); }
	
	/**
	 * @return The time this slave has been idle.
	 */
	time_t idleTime();
	
	/**
	 * Marks this slave as idle.
	 */
	void setIdle();

    public slots:
        void accept(KSocket *socket);
	void gotInput();
	void gotAnswer();
    signals:
        void slaveDied(KIO::Slave *slave);
	
    private:
        QString m_protocol;
        QString m_host;
        int m_port;
        QString m_user;
        QString m_passwd;
	KServerSocket *serv;
	QString m_socket;
	pid_t m_pid;
	bool contacted;
	bool dead;
	time_t contact_started;
	time_t idle_since;
	KIO::Connection slaveconn;
    };

};

#endif
