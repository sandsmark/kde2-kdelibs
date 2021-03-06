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

#ifndef KMCUPSMANAGER_H
#define KMCUPSMANAGER_H

#include "kmmanager.h"

class IppRequest;
class KLibrary;

class KMCupsManager : public KMManager
{
public:
	KMCupsManager(QObject *parent = 0, const char *name = 0);
	virtual ~KMCupsManager();

	// printer management functions
	bool createPrinter(KMPrinter *p);
	bool removePrinter(KMPrinter *p);
	bool enablePrinter(KMPrinter *p);
	bool disablePrinter(KMPrinter *p);
	bool completePrinter(KMPrinter *p);
	bool completePrinterShort(KMPrinter *p);
	bool setDefaultPrinter(KMPrinter *p);
	bool testPrinter(KMPrinter *p);

	// printer listing functions
	// driver DB functions
	QString driverDbCreationProgram();
	QString driverDirectory();

	DrMain* loadPrinterDriver(KMPrinter *p, bool config = false);
	DrMain* loadFileDriver(const QString& filename);
	bool savePrinterDriver(KMPrinter *p, DrMain *d);

	bool configure(QWidget *parent = 0);

	bool restartServer();
	bool configureServer(QWidget *parent = 0);
	QStringList detectLocalPrinters();

protected:
	// the real printer listing job is done here
	void listPrinters();
	void loadServerPrinters();
	void processRequest(IppRequest*);
	bool setPrinterState(KMPrinter *p, int st);
	DrMain* loadDriverFile(const QString& filename);
	void saveDriverFile(DrMain *driver, const QString& filename);
	void reportIppError(IppRequest*);
	void* loadCupsdConfFunction(const char*);
	void unloadCupsdConf();
	QString cupsInstallDir();

private:
	KLibrary	*m_cupsdconf;
};

#endif
