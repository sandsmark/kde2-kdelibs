/*
* kstddirs.cpp -- Implementation of class KStandardDirs.
* Author:	Sirtaj Singh Kang <taj@kde.org>
* Version:	$Id$
* Generated:	Thu Mar  5 16:05:28 EST 1998
*/

#include"kstddirs.h"
#include"config.h"

#include<stdlib.h>

#include<qdict.h>
#include<qdir.h>
#include<qfileinfo.h>
#include<qstring.h>
#include<qstrlist.h>

#define KDEDIR "/opt/kde"
#define KDEDIR_LEN 6

static const QString& tokenize( QString& token, const QString& str, 
		const QString& delim );
static const QString& selectStr( const QString& env, 
		const QString& builtin );

KStandardDirs::KStandardDirs( const QString& appName ) :
	UserDir (	QDir::homeDirPath() ),
	KDEDir	(	selectStr( "KDEDIR", KDEDIR ) ),
	_appPath(	0	),
	_appName(	new QString( appName ) ),
	_app	(	new QDict<QString> ),
	_sysapp	(	new QDict<QString> ),
	_sys	(	new QDict<QString> ),
	_user	(	new QDict<QString> )

{
	if( _app ) _app->setAutoDelete( true );
	if( _sysapp ) _sysapp->setAutoDelete( true );
	if( _sys ) _sys->setAutoDelete( true );
	if( _user) _user->setAutoDelete( true );
}

KStandardDirs::~KStandardDirs()
{
	delete _app;
	delete _sysapp;
	delete _sys;
	delete _user;

	delete _appName;
}

QString KStandardDirs::findExe( const QString& appname, 
		const QString& pathstr, bool ignore)
{
	if( pathstr == 0 ) {
		pathstr = getenv( "PATH" );
	}

	QString onepath;
	QFileInfo info;

	QString p(pathstr);

	// split path using : or \b as delimiters
        while( (p = tokenize( onepath, p, ":\b") ) != 0 ) {
		onepath += "/";
		onepath += appname;

		// Check for executable in this tokenized path
		info.setFile( onepath );

		if( info.exists() && ( ignore || info.isExecutable() )
			       	&& info.isFile() ) {
			return onepath;
		}
        }

	// If we reach here, the executable wasn't found.
	// So return empty string.

	onepath = (const char*)0;

	return onepath;
}

int KStandardDirs::findAllExe( QStrList& list, const QString& appname,
			const QString& pathstr=QString::null, bool ignore )
{
	if( pathstr == 0 ) {
		pathstr = getenv( "PATH" );
	}

	QString onepath;
	QFileInfo info;

	QString p(pathstr);

	list.clear();

	// split path using : or \b as delimiters
        while( (p = tokenize( onepath, p, ":\b") ) != 0 ) {
		onepath += "/";
		onepath += appname;

		// Check for executable in this tokenized path
		info.setFile( onepath );

		if( info.exists() && (ignore || info.isExecutable())
			       	&& info.isFile() ) {
			list.append( onepath );
		}
        }

	return list.count();
}


const QString KStandardDirs::closest( DirScope method, 
		const QString& suffix ) const
{
	CHECK_PTR( suffix );
	CHECK_PTR( _app );
	CHECK_PTR( _sys );
	CHECK_PTR( _user );

	QString sys, sysapp, user, app;
	QString *found = _user->find( suffix );
       	

	// check dict for previous full-string insertion
	if( found == 0 ) {
		// not already inserted, so insert them now.
		QString realsuffix(suffix);
		if( !strncmp( suffix, "KDEDIR", 
				KDEDIR_LEN ) ) {
			// remove prefix KDEDIR from path
			realsuffix += KDEDIR_LEN;
		}

		// system dir
		QString *name = new QString( KDEDir );
		*name += realsuffix;
		sys = name->data();

		const_cast<KStandardDirs*>(this)->_sys->insert( 
				suffix, name );

		// system app dir
		name = new QString( KDEDir );

		*name += "/share/apps/";
		*name += realsuffix;

		sysapp = name->data();

		const_cast<KStandardDirs*>(this)->_sysapp->insert( 
				suffix, name );

		// user dir
		name = new QString( UserDir );

		*name += "/.kde";
		*name += realsuffix;


		user = name->data();

		const_cast<KStandardDirs*>(this)->_user->insert( 
				suffix, name );


		// app dir
		name = new QString( UserDir );

		*name += "/.kde/share/apps/";
		*name += *_appName;
		*name += realsuffix;

		app = name->data();

		const_cast<KStandardDirs*>(this)->_app->insert( 
				suffix, name );

	}
	else {
		// string's already in there
		user = found->data(); 
		sys = _sys->find( suffix )->data();
		sysapp = _sysapp->find( suffix )->data();
		app = _app->find( suffix )->data();
	}
			
	// return specific dirs

	switch( method ) {
		case User:	return user;
		case System:	return sys;
		case SysApp:	return sysapp;
		case App:	return app;
		default:	break;
	}

	// find closest

	QFileInfo info( app );	// app

	if( info.isDir() ) {
		return app;
	}

	info.setFile( user );	// user

	if( info.isDir() ) {
		return user;
	}

	info.setFile( sysapp );	// sysapp

	if( info.isDir() ) {
		return sysapp;
	}

	return sys;		// sys
}

const QString KStandardDirs::app( KStandardDirs::DirScope s ) const
{
	if( _appPath == 0 ) {
		QString app( "/share/apps/" );
		app += _appName->data();

		const_cast<KStandardDirs*>(this)->_appPath
			= closest( s, app.data() );
	}

	return _appPath;
}

const QString KStandardDirs::bin( DirScope s ) const
{
	return closest( s, KDE_BINDIR );
}

const QString KStandardDirs::cgi( DirScope s ) const
{

	return closest( s, KDE_CGIDIR );
}

const QString KStandardDirs::config( DirScope s ) const
{
	return closest( s, KDE_CONFIGDIR );
}

const QString KStandardDirs::apps( DirScope s ) const
{
	return closest( s, KDE_DATADIR );
}

const QString KStandardDirs::html( DirScope s ) const
{
	return closest( s, KDE_HTMLDIR );
}

const QString KStandardDirs::icon( DirScope s ) const
{
	return closest( s, KDE_ICONDIR );
}

const QString KStandardDirs::locale( DirScope s ) const
{
	return closest( s, KDE_LOCALE );
}

const QString KStandardDirs::mime( DirScope s ) const
{
	return closest( s, KDE_MIMEDIR );
}

const QString KStandardDirs::parts( DirScope s ) const
{
	return closest( s, KDE_PARTSDIR );
}

const QString KStandardDirs::toolbar( DirScope s ) const
{
	return closest( s, KDE_TOOLBARDIR );
}

const QString KStandardDirs::wallpaper( DirScope s ) const
{
	return closest( s, KDE_WALLPAPERDIR );
}

const QString KStandardDirs::sound( DirScope s ) const
{
	return closest( s, KDE_SOUNDDIR );
}

static const QString& tokenize( QString& token, const QString& str, 
		const QString& delim )
{
        token = "";

        if( !str || !*str ) {
                return 0;
        }

        // find first non-delimiter character
        while( *str && strchr( delim, *str ) ) {
                str++;
        }

        if( !*str ) {
                return 0;
        }

        // find first delimiter or end, storing each non-qualifier into token
        while( *str && !strchr( delim, *str ) ) {
                token += *str;
                str++;
        }

        return str;
}

static const QString& selectStr( const QString& env, const QString& builtin )
{
	QString result(getenv( env ));

	if( result == 0 ) {
		result = builtin;
	}

	return result;
}
