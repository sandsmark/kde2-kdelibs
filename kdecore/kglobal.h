/*
* kglobal.h -- Declaration of class KGlobal.
* Generated by newclass on Sat May  1 02:08:43 EST 1999.
*/
#ifndef SSK_KGLOBAL_H
#define SSK_KGLOBAL_H

class QPopupMenu;

class KApplication;
class KStandardDirs;
class KConfig;
class KLocale;
class KIconLoader;
class KCharsets;

/**
* Accessors to KDE global objects.
* 
* This class is never instantiated. Instead, it is initialized by
* @ref KApplication.
*
* WARNING: Do NOT access these functions without first constructing a 
* @ref KApplication object.
* 
* @author Sirtaj Singh Kang (taj@kde.org)
* @version $Id$
*/
class KGlobal
{
public:

	static KApplication	*kapp();
	static KStandardDirs	*dirs();
	
	enum ConfigState {
		APPCONFIG_NONE, APPCONFIG_READONLY, APPCONFIG_READWRITE 
	};

	static ConfigState	configState();
	static KConfig		*config();
	static KConfig		*instanceConfig();
	
	static KIconLoader	*iconLoader();

	static bool		localeConstructed();
	static KLocale		*locale();
	static KCharsets	*charsets();
		
	static QPopupMenu	*helpMenu();

private:
	KGlobal();
	KGlobal( const KGlobal& );

	static 	KApplication	*_kapp;
	static 	KStandardDirs	*_dirs;

	static 	ConfigState	_configState;
	static 	KConfig		*_config;
	static 	KConfig		*_instanceConfig;
	static 	KIconLoader	*_iconLoader;

	static 	KLocale		*_locale;
	static 	KCharsets	*_charsets;

	static 	QPopupMenu	*_helpMenu;
	
	friend class KApplication;
	static void cleanup();
};

#endif // SSK_KGLOBAL_H
