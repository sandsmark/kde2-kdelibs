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

#include "kmmanager.h"
#include "kmprinter.h"
#include "kmdbentry.h"
#include "kmfactory.h"
#include "kmvirtualmanager.h"
#include "kmspecialmanager.h"

#include <zlib.h>
#include <qfile.h>
#include <kstddirs.h>
#include <kconfig.h>
#include <klocale.h>
#include <kdebug.h>
#include <kapp.h>
#include <klibloader.h>
#include <unistd.h>

KMManager::KMManager(QObject *parent, const char *name)
: QObject(parent,name)
{
	m_printers.setAutoDelete(true);
	m_hasmanagement = false;
	m_printeroperationmask = 0;
	m_serveroperationmask = 0;

	m_specialmgr = new KMSpecialManager(this);
	CHECK_PTR(m_specialmgr);
	m_virtualmgr = new KMVirtualManager(this);
	CHECK_PTR(m_virtualmgr);
}

KMManager::~KMManager()
{
}

bool KMManager::notImplemented()
{
	setErrorMsg(i18n("This operation is not implemented."));
	return false;
}

bool KMManager::modifyPrinter(KMPrinter *oldp, KMPrinter *newp)
{
	if (oldp->printerName() != newp->printerName())
	{
		// remove the old one, then create the new one. But before removing
		// the old one, retrieve its driver so that the new printer has the
		// same.
		DrMain	*driver = loadPrinterDriver(oldp, true);
		newp->setDriver(driver);
		if (!removePrinter(oldp))
			return false;
	}
	return createPrinter(newp);
}

bool KMManager::createPrinter(KMPrinter*)
{
	return notImplemented();
}

bool KMManager::removePrinter(KMPrinter*)
{
	return notImplemented();
}

bool KMManager::removePrinter(const QString& name)
{
	KMPrinter	*p = findPrinter(name);
	return (p ? removePrinter(p) : false);
}

bool KMManager::enablePrinter(KMPrinter*)
{
	return notImplemented();
}

bool KMManager::enablePrinter(const QString& name)
{
	KMPrinter	*p = findPrinter(name);
	return (p ? enablePrinter(p) : false);
}

bool KMManager::disablePrinter(KMPrinter*)
{
	return notImplemented();
}

bool KMManager::disablePrinter(const QString& name)
{
	KMPrinter	*p = findPrinter(name);
	return (p ? disablePrinter(p) : false);
}

bool KMManager::completePrinter(KMPrinter*)
{
	notImplemented();
	return true;	// not false to allow instance config/creation
}

bool KMManager::completePrinterShort(KMPrinter *p)
{
	return completePrinter(p);
}

bool KMManager::completePrinter(const QString& name)
{
	KMPrinter	*p = findPrinter(name);
	return (p ? completePrinter(p) : false);
}

bool KMManager::setDefaultPrinter(KMPrinter*)
{
	return notImplemented();
}

bool KMManager::setDefaultPrinter(const QString& name)
{
	KMPrinter	*p = findPrinter(name);
	return (p ? setDefaultPrinter(p) : false);
}

bool KMManager::testPrinter(KMPrinter*)
{
	return notImplemented();
}

KMPrinter* KMManager::findPrinter(const QString& name)
{
	QListIterator<KMPrinter>	it(m_printers);
	for (;it.current();++it)
		if (it.current()->name() == name) return it.current();
	setErrorMsg(i18n("%1: printer not found.").arg(name));
	return 0;
}

KMPrinter* KMManager::softDefault() const
{
	QListIterator<KMPrinter>	it(m_printers);
	for (;it.current();++it)
		if (it.current()->isSoftDefault()) return it.current();
	return 0;
}

QList<KMPrinter>* KMManager::printerList(bool reload)
{
	if (reload || m_printers.count() == 0)
	{
		// first discard all printers
		discardAllPrinters(true);

		// make sure virtual printers will be reloaded if we don't have
		// any printer (for example if settings are wrong)
		if (m_printers.count() == 0)
			m_virtualmgr->reset();

		// List real printers (in subclasses)
		listPrinters();
                // list virtual printers (and undiscard virtual printers if necessary)
		m_virtualmgr->refresh();
		m_specialmgr->refresh();

		// remove discarded printers
		for (uint i=0; i<m_printers.count(); i++)
			if (m_printers.at(i)->isDiscarded())
			{
				m_printers.remove(i);
				i--;
			}

		// try to find the default printer from these situations:
		//   - it already exists from .lpoptions file
		//   - use the PRINTER variable
		if (!softDefault())
		{
			KMPrinter	*defprinter = findPrinter(QString::fromLatin1(getenv("PRINTER")));
			if (defprinter)
				setSoftDefault(defprinter);
		}
	}

	return &m_printers;
}

void KMManager::listPrinters()
{
	// do nothing
}

void KMManager::addPrinter(KMPrinter *p)
{
	if (p)
	{
		if (p->name().isEmpty())
			// discard printer with empty name
			delete p;
		else
		{
			KMPrinter	*other = findPrinter(p->name());
			if (other)
			{
				other->copy(*p);
				delete p;
			}
			else
			{
				p->setDiscarded(false);
				m_printers.inSort(p);
			}
		}
	}
}

QString KMManager::driverDbCreationProgram()
{
	return QString::null;
}

QString KMManager::driverDirectory()
{
	return QString::null;
}

DrMain* KMManager::loadPrinterDriver(KMPrinter*, bool)
{
	return NULL;
}

DrMain* KMManager::loadDbDriver(KMDBEntry *entry)
{
	return loadFileDriver(entry->file);
}

DrMain* KMManager::loadFileDriver(const QString&)
{
	return NULL;
}

bool KMManager::savePrinterDriver(KMPrinter*,DrMain*)
{
	return false;
}

bool KMManager::uncompressFile(const QString& filename, QString& destname)
{
	QFile	f(filename);
	bool	result(true);
	destname = QString::null;
	if (f.exists() && f.open(IO_ReadOnly))
	{
		char	buf[1024] = {0};
		f.readBlock(buf,2);
		if ((uchar)(buf[0]) == 037 && (uchar)(buf[1]) == 0213)
		{
			f.close();
			destname = locateLocal("tmp","kdeprint_") + KApplication::randomString(8);
			f.setName(destname);

			if (f.open(IO_WriteOnly))
			{
				gzFile	in = gzopen(filename.latin1(),"r");
				int	n(0);
				while ((n=gzread(in,buf,1024)) > 0)
					if (f.writeBlock(buf,n) != n)
						break;
				if (n != 0) result = false;
				gzclose(in);
				f.close();
			}
		}
	}
	return result;
}

void KMManager::setHardDefault(KMPrinter *p)
{
	QListIterator<KMPrinter>	it(m_printers);
	for (;it.current();++it)
		it.current()->setHardDefault(false);
	if (p) p->setHardDefault(true);
}

void KMManager::setSoftDefault(KMPrinter *p)
{
	QListIterator<KMPrinter>	it(m_printers);
	for (;it.current();++it)
        {
		it.current()->setSoftDefault(false);
		it.current()->setOwnSoftDefault(false);
        }
	if (p)
        {
                p->setSoftDefault(true);
                KMPrinter       *pp = findPrinter(p->printerName());
                if (pp)
                        pp->setOwnSoftDefault(true);
        }
}

bool KMManager::configure(QWidget*)
{
	return notImplemented();
}

bool KMManager::restartServer()
{
	return notImplemented();
}

bool KMManager::configureServer(QWidget*)
{
	return notImplemented();
}

QString KMManager::testPage()
{
	KConfig	*conf = KMFactory::self()->printConfig();
	conf->setGroup("General");
	QString	tpage = conf->readEntry("TestPage",QString::null);
	if (tpage.isEmpty())
		tpage = locate("data","kdeprint/testprint.ps");
	return tpage;
}

void KMManager::discardAllPrinters(bool on)
{
	QListIterator<KMPrinter>	it(m_printers);
	for (;it.current();++it)
		if (!on || !it.current()->isSpecial())
			it.current()->setDiscarded(on);
}

bool KMManager::validateDbDriver(KMDBEntry*)
{
	return true;
}

bool KMManager::createSpecialPrinter(KMPrinter *p)
{
	if (p && p->isSpecial())
	{
		KMPrinter	*old = findPrinter(p->name());
		if (old && !old->isSpecial())
		{
			setErrorMsg(i18n("Can't overwrite regular printer with special printer settings."));
			return false;
		}
		// if the special printer already exists, it will be overwritten
		addPrinter(p);
		return m_specialmgr->savePrinters();
	}
	return false;
}

bool KMManager::removeSpecialPrinter(KMPrinter *p)
{
	if (p && p->isSpecial() && m_printers.findRef(p) != -1)
	{
		m_printers.removeRef(p);
		return m_specialmgr->savePrinters();
	}
	return false;
}

/* format of output:
	URI0, Desc0, Printer0, URI1, Desc1, Printer1, ...

  Defaults to 3 parallel printers
*/
QStringList KMManager::detectLocalPrinters()
{
	QStringList	list;
	for (int i=0; i<3; i++)
		list << QString::fromLatin1("parallel:/dev/lp%1").arg(i) << i18n("Parallel Port #%1").arg(i+1) << QString::null;
	return list;
}

int KMManager::addPrinterWizard(QWidget *parent)
{
	KLibrary	*lib = KLibLoader::self()->library("libkdeprint_management");
	if (!lib)
		setErrorMsg(i18n("Unable to load KDE print management library:<br>%1").arg(KLibLoader::self()->lastErrorMessage()));
	else
	{
		int (*func)(QWidget*) = (int(*)(QWidget*))lib->symbol("add_printer_wizard");
		if (!func)
			setErrorMsg(i18n("Unable to find wizard object in management library."));
		else
			return func(parent);
	}
	return (-1);
}
