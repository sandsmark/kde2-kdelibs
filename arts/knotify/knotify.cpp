/*
   Copyright (c) 1997 Christian Esken (esken@kde.org)
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <qfile.h>

#include <kstddirs.h>
#include <kapp.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <dcopclient.h>

#include <qmessagebox.h>

#include "knotify.h"
#include "knotify.moc"

#include <iostream.h>

#include <qiomanager.h>
#include <dispatcher.h>
#include <qtextstream.h>
#include <soundserver.h>

int main(int argc, char **argv)
{
	KAboutData aboutdata("knotify", I18N_NOOP("KNotify"),
	                     "2.0pre", I18N_NOOP("KDE Notification Server"),
	                     KAboutData::License_GPL, "(C) 1997-2000, KDE Developers");
	aboutdata.addAuthor("Christian Esken",0,"esken@kde.org");
	aboutdata.addAuthor("Stefan Westerfeld",I18N_NOOP("Sound support"),"stefan@space.twc.de");
	aboutdata.addAuthor("Charles Samuels",I18N_NOOP("Current Maintainer"),"charles@altair.dhs.org");
	
	KCmdLineArgs::init( argc, argv, &aboutdata );
//	KCmdLineArgs::addCmdLineOptions( options );
	KApplication app;
	
	// setup dcop communication
	if (!app.dcopClient()->isAttached())
		app.dcopClient()->registerAs("knotify",false);
	if (!app.dcopClient()->isAttached())
		return 1;
	
	// setup mcop communication
	QIOManager qiomanager;
	Dispatcher dispatcher(&qiomanager);


	(void) new KNotify;

	return app.exec();
}

KNotify::KNotify() : QObject(), DCOPObject("Notify")
{
	// obtain an object reference to the soundserver
	server = SimpleSoundServer_base::_fromString("global:Arts_SimpleSoundServer");
	if(!server)
		cerr << "artsd is not running, there will be no sound notifications.\n";
}

bool KNotify::process(const QCString &fun, const QByteArray &data,
                      QCString& /*replyType*/, QByteArray& /*replyData*/ )
{
	if (fun == "notify(QString,QString,QString,QString,QString, Presentation)")
	{
		QDataStream dataStream( data, IO_ReadOnly );
		QString event;
		QString fromApp;
		QString text;
		QString sound;
		QString file;
	 	int present;
		dataStream >> event >>fromApp >> text >> sound >> file>> present;
		processNotification(event, fromApp, text, sound, file, (Presentation)present);
 
		return true;
	}
	return false;
}

void KNotify::processNotification(const QString &event, const QString &fromApp,
                                  const QString &text, QString sound, QString file,
                                  Presentation present)
{
	static bool eventRunning=true;
	
	if (event.length())
	{
		KConfig eventsfile(locate("data", fromApp+"/eventsrc"));
		eventsfile.setGroup(event);
	
		if (present==-1)
			present=(Presentation)eventsfile.readNumEntry("presentation", -1);
		if (present==-1)
			present=(Presentation)eventsfile.readNumEntry("default_presentation", 0);
		sound=eventsfile.readEntry("sound", 0);
		if (sound.isNull())
			sound=eventsfile.readEntry("default_logfile", "");
		file=eventsfile.readEntry("sound", 0);
		if (file.isNull())
			sound=eventsfile.readEntry("default_logfile", "");
		
	}
	
	eventRunning=true;
	if ((present & KNotifyClient::Sound) && (QFile(sound).isReadable()))
		notifyBySound(sound);
	if (present & KNotifyClient::Messagebox)
		notifyByMessagebox(text);
#warning Logfile doesn t exist in knotifyclient.h
	//if (present & KNotifyClient::Logfile && (QFile(file).isReadable()))
		//notifyByLogfile(text, file);
	if (present & KNotifyClient::Stderr)
		notifyByStderr(text);
	eventRunning=false;
}

bool KNotify::notifyBySound(const QString &sound)
{
	if(server) server->play((const char *)sound);
	return true;
}

bool KNotify::notifyByMessagebox(const QString &text)
{
	QMessageBox *notification;
	notification = new QMessageBox(i18n("Notification"),
	                               text,
	                               QMessageBox::Information,
	                               QMessageBox::Ok | QMessageBox::Default,
	                               0, 0, 0, 0, false);

	notification->show();
	return true;
}

bool KNotify::notifyByLogfile(const QString &text, const QString &file)
{
	QFile f(file);
	if (!f.open(IO_WriteOnly)) return false;
	QTextStream t(&f);

	t<< "=======================================\n";
	t<< "KNotify: " << QDateTime::currentDateTime().toString() << '\n';
	t<< text<< "\n\n";
	f.close();
	return true;
}

bool KNotify::notifyByStderr(const QString &text)
{
	QTextStream t(stderr, IO_WriteOnly);

	t<< "KNotify: " << QDateTime::currentDateTime().toString() << '\n';
	t<< text<< "\n\n";
	return true;
}


