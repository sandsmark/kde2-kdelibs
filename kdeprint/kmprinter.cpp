#include "kmprinter.h"
#include "driver.h"

KMPrinter::KMPrinter()
: KMObject()
{
	m_type = KMPrinter::Printer;
	m_dbentry = 0;
	m_harddefault = m_softdefault = m_ownsoftdefault = false;
	m_driver = 0;
	m_isedited = false;
}

KMPrinter::KMPrinter(const KMPrinter& p)
: KMObject()
{
	m_driver = 0;	// don't copy driver structure
	m_harddefault = m_softdefault = m_ownsoftdefault = false;
	m_isedited = false;
	copy(p);
}

KMPrinter::~KMPrinter()
{
	delete m_driver;
}

void KMPrinter::copy(const KMPrinter& p)
{
	m_name = p.m_name;
	m_printername = p.m_printername;
	m_instancename = p.m_instancename;
	m_type = p.m_type;
	m_state = p.m_state;
	m_device = p.m_device;
	m_members = p.m_members;
	m_description = p.m_description;
	m_location = p.m_location;
	m_manufacturer = p.m_manufacturer;
	m_model = p.m_model;
	m_uri = p.m_uri;
	m_driverinfo = p.m_driverinfo;
	m_dbentry = p.m_dbentry;
	//m_harddefault = p.m_harddefault;
	//m_softdefault = p.m_softdefault;
	m_options = p.m_options;
	setDiscarded(false);
}

void KMPrinter::setDriver(DrMain *driver)
{
	delete m_driver;
	m_driver = driver;
}

QString KMPrinter::pixmap()
{
	QString	str("kdeprint_printer");
	if (!isValid()) str.append("_defect");
	else
	{
		//if (isHardDefault()) str.append("_default");
		if (isClass(true)) str.append("_class");
		else if (isRemote()) str.append("_remote");
		switch (state())
		{
			case KMPrinter::Stopped:
				str.append("_stopped");
				break;
			case KMPrinter::Processing:
				str.append("_process");
				break;
			default:
				break;
		}
	}
	return str;
}

int KMPrinter::compare(KMPrinter *p1, KMPrinter *p2)
{
	if (p1 && p2)
	{
		bool	c1(p1->isClass(false)), c2(p2->isClass(false));
		if (c1 == c2) return QString::compare(p1->name(),p2->name());
		else if (c1 && !c2) return -1;
		else if (!c1 && c2) return 1;
	}
	return 0;
}