/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Michael Goffioul <goffioul@imec.be>
 *
 *  $Id$
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

#include "kmlpdunixmanager.h"
#include "kmfactory.h"
#include "kmprinter.h"

#include <qfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <klocale.h>

/*****************************
 * Various parsing functions *
 *****************************/

// extract a line from a QTextStream ('#' -> comments, '\' -> line continue)
QString readLine(QTextStream& t)
{
	QString	line, buffer;
	while (!t.eof())
	{
		buffer = t.readLine().stripWhiteSpace();
		if (buffer.isEmpty() || buffer[0] == '#')
			continue;
		line.append(buffer);
		if (line.right(1) == "\\")
		{
			line.truncate(line.length()-1);
			line = line.stripWhiteSpace();
		}
		else break;
	}
	return line;
}

// extact an entry (printcap-like)
QMap<QString,QString> readEntry(QTextStream& t)
{
	QString	line = readLine(t);
	QMap<QString,QString>	entry;

	if (!line.isEmpty())
	{
		QStringList	l = QStringList::split(':',line,false);
		if (l.count() > 0)
		{
			int 	p(-1);
			if ((p=l[0].find('|')) != -1)
				entry["printer-name"] = l[0].left(p);	// only keep first name (discard aliases
			else
				entry["printer-name"] = l[0];
			for (uint i=1; i<l.count(); i++)
				if ((p=l[i].find('=')) != -1)
					entry[l[i].left(p).stripWhiteSpace()] = l[i].right(l[i].length()-p-1).stripWhiteSpace();
				else
					entry[l[i].stripWhiteSpace()] = QString::null;
		}
	}
	return entry;
}

// create basic printer from entry
KMPrinter* createPrinter(const QMap<QString,QString>& entry)
{
	KMPrinter	*printer = new KMPrinter();
	printer->setName(entry["printer-name"]);
	printer->setPrinterName(entry["printer-name"]);
	printer->setType(KMPrinter::Printer);
	printer->setState(KMPrinter::Idle);
	return printer;
}

// "/etc/printcap" file parsing (Linux/LPR)
void KMLpdUnixManager::parseEtcPrintcap()
{
	QFile	f("/etc/printcap");
	if (f.exists() && f.open(IO_ReadOnly))
	{
		QTextStream	t(&f);
		QMap<QString,QString>	entry;

		while (!t.eof())
		{
			entry = readEntry(t);
			if (entry.isEmpty() || !entry.contains("printer-name"))
				continue;
			KMPrinter	*printer = ::createPrinter(entry);
			if (entry.contains("rm"))
				printer->setDescription(i18n("Remote printer queue on %1").arg(entry["rm"]));
			else
				printer->setDescription(i18n("Local printer"));
			addPrinter(printer);
		}
	}
}

// "/etc/printers.conf" file parsing (Solaris 2.6)
void KMLpdUnixManager::parseEtcPrintersConf()
{
	QFile	f("/etc/printers.conf");
	if (f.exists() && f.open(IO_ReadOnly))
	{
		QTextStream	t(&f);
		QMap<QString,QString>	entry;
		QString		default_printer;

		while (!t.eof())
		{
			entry = readEntry(t);
			if (entry.isEmpty() || !entry.contains("printer-name"))
				continue;
			QString	prname = entry["printer-name"];
			if (prname == "_default")
			{
				if (entry.contains("use"))
					default_printer = entry["use"];
			}
			else if (prname != "_all")
			{
				KMPrinter	*printer = ::createPrinter(entry);
				if (entry.contains("bsdaddr"))
				{
					QStringList	l = QStringList::split(',',entry["bsdaddr"],false);
					printer->setDescription(i18n("Remote printer queue on %1").arg(l[0]));
				}
				else
					printer->setDescription(i18n("Local printer"));
				addPrinter(printer);
			}
		}

		if (!default_printer.isEmpty())
			setSoftDefault(findPrinter(default_printer));
	}
}

// "/etc/lp/printers/" directory parsing (Solaris non-2.6)
void KMLpdUnixManager::parseEtcLpPrinters()
{
	QDir	d("/etc/lp/printers");
	const QFileInfoList	*prlist = d.entryInfoList(QDir::Dirs);
	if (!prlist)
		return;

	QFileInfoListIterator	it(*prlist);
	for (;it.current();++it)
	{
		if (it.current()->fileName() == "." || it.current()->fileName() == "..")
			continue;
		QFile	f(it.current()->absFilePath() + "/configuration");
		if (f.exists() && f.open(IO_ReadOnly))
		{
			QTextStream	t(&f);
			QString		line, remote;
			while (!t.eof())
			{
				line = readLine(t);
				if (line.isEmpty()) continue;
				if (line.startsWith("Remote:"))
				{
					QStringList	l = QStringList::split(':',line,false);
					if (l.count() > 1) remote = l[1];
				}
			}
			KMPrinter	*printer = new KMPrinter;
			printer->setName(it.current()->fileName());
			printer->setPrinterName(it.current()->fileName());
			printer->setType(KMPrinter::Printer);
			printer->setState(KMPrinter::Idle);
			if (!remote.isEmpty())
				printer->setDescription(i18n("Remote printer queue on %1").arg(remote));
			else
				printer->setDescription(i18n("Local printer"));
			addPrinter(printer);
		}
	}
}

// "/etc/lp/member/" directory parsing (HP-UX)
void KMLpdUnixManager::parseEtcLpMember()
{
	QDir	d("/etc/lp/member");
	const QFileInfoList	*prlist = d.entryInfoList(QDir::Files);
	if (!prlist)
		return;

	QFileInfoListIterator	it(*prlist);
	for (;it.current();++it)
	{
		KMPrinter	*printer = new KMPrinter;
		printer->setName(it.current()->fileName());
		printer->setPrinterName(it.current()->fileName());
		printer->setType(KMPrinter::Printer);
		printer->setState(KMPrinter::Idle);
		printer->setDescription(i18n("Local printer"));
		addPrinter(printer);
	}
}

// "/usr/spool/lp/interfaces/" directory parsing (IRIX 6.x)
void KMLpdUnixManager::parseSpoolInterface()
{
	QDir	d("/usr/spool/interfaces/lp");
	const QFileInfoList	*prlist = d.entryInfoList(QDir::Files);
	if (!prlist)
		return;

	QFileInfoListIterator	it(*prlist);
	for (;it.current();++it)
	{
		QFile	f(it.current()->absFilePath());
		if (f.exists() && f.open(IO_ReadOnly))
		{
			QTextStream	t(&f);
			QString		line, remote;

			while (!t.eof())
			{
				line = t.readLine().stripWhiteSpace();
				if (line.startsWith("HOSTNAME"))
				{
					QStringList	l = QStringList::split('=',line,false);
					if (l.count() > 1) remote = l[1];
				}
			}

			KMPrinter	*printer = new KMPrinter;
			printer->setName(it.current()->fileName());
			printer->setPrinterName(it.current()->fileName());
			printer->setType(KMPrinter::Printer);
			printer->setState(KMPrinter::Idle);
			if (!remote.isEmpty())
				printer->setDescription(i18n("Remote printer queue on %1").arg(remote));
			else
				printer->setDescription(i18n("Local printer"));
			addPrinter(printer);
		}
	}
}

//*********************************************************************************************************

KMLpdUnixManager::KMLpdUnixManager(QObject *parent, const char *name)
: KMManager(parent,name)
{
}

void KMLpdUnixManager::listPrinters()
{
	// load only once, if already loaded, just keep them (remove discard flag)
	if (m_printers.count() == 0)
	{
		parseEtcPrintcap();
		parseEtcPrintersConf();
		parseEtcLpPrinters();
		parseEtcLpMember();
		parseSpoolInterface();
	}
	else
	{
		QListIterator<KMPrinter>	it(m_printers);
		for (;it.current();++it)
			it.current()->setDiscarded(false);
	}
}
