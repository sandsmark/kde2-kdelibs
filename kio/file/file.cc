// $Id$

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <iostream.h>

#include <qvaluelist.h>

#include <kshred.h>
#include <kdebug.h>
#include <kurl.h>
#include <kprotocolmanager.h>
#include <kinstance.h>
#include <qfile.h>
#include "file.h"
#include <limits.h>

using namespace KIO;

QString testLogFile( const QString& _filename );

FileProtocol::FileProtocol( Connection *_conn ) : SlaveBase( "file", _conn )
{
    usercache.setAutoDelete( TRUE );
    groupcache.setAutoDelete( TRUE );
}

void FileProtocol::chmod( const QString& path, int permissions )
{
    if ( ::chmod( path, permissions ) == -1 )
        error( KIO::ERR_CANNOT_CHMOD, path );
    else
        finished();
}

void FileProtocol::mkdir( const QString& path, int permissions )
{
    struct stat buff;
    if ( ::stat( path, &buff ) == -1 ) {
	if ( ::mkdir( path, S_IRWXU ) != 0 ) {
	    if ( errno == EACCES ) {
		error( KIO::ERR_ACCESS_DENIED, path );
		return;
	    } else {
		error( KIO::ERR_COULD_NOT_MKDIR, path );
		return;
	    }
	} else {
	    if ( permissions != -1 )
	        chmod( path, permissions );
	    else
	        finished();
	    return;
	}
    }

    if ( S_ISDIR( buff.st_mode ) ) {
	debug("ERR_DIR_ALREADY_EXIST");
	error( KIO::ERR_DIR_ALREADY_EXIST, path );
	return;
    }
    error( KIO::ERR_FILE_ALREADY_EXIST, path );
    return;
}

void FileProtocol::get( const QString& path, const QString& /*query*/, bool /* reload */)
{
    struct stat buff;
    if ( ::stat( path, &buff ) == -1 ) {
        if ( errno == EACCES )
           error( KIO::ERR_ACCESS_DENIED, path );
        else
           error( KIO::ERR_DOES_NOT_EXIST, path );
	return;
    }

    if ( S_ISDIR( buff.st_mode ) ) {
	error( KIO::ERR_IS_DIRECTORY, path );
	return;
    }

    FILE *f = fopen( path, "rb" );
    if ( f == 0L ) {
	error( KIO::ERR_CANNOT_OPEN_FOR_READING, path );
	return;
    }

    ready();

    // TODO gettingFile( usrc );

    totalSize( buff.st_size );
    int processed_size = 0;
    time_t t_start = time( 0L );
    time_t t_last = t_start;

    char buffer[ 4090 ];
    QByteArray array;

    while( !feof( f ) )
	{
	    int n = fread( buffer, 1, 2048, f );
            if (n == -1)
            {
               error( KIO::ERR_COULD_NOT_READ, path);
               fclose(f);
               return;
            }

	    array.setRawData(buffer, n);
	    data( array );
            array.resetRawData(buffer, n);

	    processed_size += n;
	    time_t t = time( 0L );
	    if ( t - t_last >= 1 )
		{
		    processedSize( processed_size );
		    speed( processed_size / ( t - t_start ) );
		    t_last = t;
		}
	}

    data( QByteArray() );

    fclose( f );

    processedSize( buff.st_size );
    time_t t = time( 0L );
    if ( t - t_start >= 1 )
	speed( processed_size / ( t - t_start ) );

    finished();
}


void FileProtocol::put( const QString& dest_orig, int _mode, bool _overwrite, bool _resume )
{
    kdDebug( 7101 ) << "Put " << dest_orig << endl;
    QString dest_part( dest_orig );
    dest_part += ".part";

    bool bMarkPartial = KProtocolManager::self().markPartial();

    struct stat buff_orig;
    bool orig_exists = ( ::stat( dest_orig, &buff_orig ) != -1 );
    if ( orig_exists &&  !_overwrite && !_resume)
    {
        if (S_ISDIR(buff_orig.st_mode))
           error( KIO::ERR_DIR_ALREADY_EXIST, dest_orig );
        else
           error( KIO::ERR_FILE_ALREADY_EXIST, dest_orig );
        return;
    }

    QString dest;
    if (bMarkPartial)
    {
        kDebugInfo( 7101, "Appending .part extension to %s", debugString(dest_orig) );
        dest = dest_part;

        struct stat buff_part;
        bool part_exists = ( ::stat( dest_part, &buff_part ) != -1 );
        if ( part_exists && !_resume )
        {
             kDebugInfo( 7101, "Deleting partial file %s", debugString(dest_part) );
             if ( ! remove( dest_part ) ) {
                 part_exists = false;
             } else {
                 error( KIO::ERR_CANNOT_DELETE_PARTIAL, dest_part );
                 return;
             }
        }
    }
    else
    {
       dest = dest_orig;
       if ( orig_exists && !_resume )
        {
             kDebugInfo( 7101, "Deleting destination file %s", debugString(dest_part) );
             remove( dest_orig );
             // Catch errors when we try to open the file.
        }
    }

    if ( _resume ) {
        m_fPut = fopen( dest, "ab" );  // append if resuming
    } else {
        // WABA: Make sure that we keep writing permissions ourselves,
        // otherwise we can be in for a surprise on NFS.
        mode_t initialMode;
        if (_mode != -1)
           initialMode = _mode | S_IWUSR;
        else
           initialMode = 0666;

        int fd = open(dest, O_CREAT | O_TRUNC | O_WRONLY, initialMode);
	m_fPut = fdopen( fd, "wb" );
    }

    if ( m_fPut == 0L ) {
	kDebugInfo( 7101, "####################### COULD NOT WRITE %s", debugString(dest));
        if ( errno == EACCES ) {
            error( KIO::ERR_WRITE_ACCESS_DENIED, dest );
        } else {
            error( KIO::ERR_CANNOT_OPEN_FOR_WRITING, dest );
        }
        return;
    }

    // We are ready for receiving data
    ready();

    kDebugInfo( 7101, "Put: Ready" );


    int result;
    // Loop until we got 0 (end of data)
    do
    {
      QByteArray buffer;
      dataReq(); // Request for data
      result = readData( buffer );
      if (result > 0)
      {
         fwrite( buffer.data(), buffer.size(), 1, m_fPut);
      }
    }
    while ( result > 0 );


    if (result != 0)
    {
        fclose(m_fPut);
	kDebugInfo( 7101, "Error during 'put'. Aborting.");
        if (bMarkPartial)
        {
           struct stat buff;
           if (( ::stat( dest, &buff ) == -1 ) ||
               ( buff.st_size < KProtocolManager::self().minimumKeepSize() ))
           {
	       remove(dest);
           }
        }
        ::exit(255);
    }

    if (fclose( m_fPut))
    {
        error( KIO::ERR_COULD_NOT_WRITE, dest_orig);
        return;
    }

    // after full download rename the file back to original name
    if ( bMarkPartial )
    {
       if ( ::rename( dest, dest_orig ) )
       {
           error( KIO::ERR_CANNOT_RENAME_PARTIAL, dest_orig );
           return;
       }
    }

    // set final permissions
    if ( _mode != -1 )
    {
       if (::chmod(dest_orig, _mode) != 0)
       {
           error( KIO::ERR_CANNOT_CHMOD, dest_orig );
           return;
       }
    }

    // We have done our job => finish
    finished();
}

void FileProtocol::copy( const QString &src, const QString &dest,
                         int _mode, bool _overwrite )
{
    struct stat buff_src;
    if ( ::stat( src, &buff_src ) == -1 ) {
        if ( errno == EACCES )
           error( KIO::ERR_ACCESS_DENIED, src );
        else
           error( KIO::ERR_DOES_NOT_EXIST, src );
	return;
    }

    if ( S_ISDIR( buff_src.st_mode ) ) {
	error( KIO::ERR_IS_DIRECTORY, src );
	return;
    }

    struct stat buff_dest;
    bool dest_exists = ( ::stat( dest, &buff_dest ) != -1 );
    if ( dest_exists )
    {
        if (S_ISDIR(buff_dest.st_mode))
        {
           error( KIO::ERR_DIR_ALREADY_EXIST, dest );
           return;
        }

        if (!_overwrite)
        {
           error( KIO::ERR_FILE_ALREADY_EXIST, dest );
           return;
        }
    }

    FILE *f = fopen( src, "rb" );
    if ( f == 0L ) {
	error( KIO::ERR_CANNOT_OPEN_FOR_READING, src );
	return;
    }

    // WABA: Make sure that we keep writing permissions ourselves,
    // otherwise we can be in for a surprise on NFS.
    mode_t initialMode;
    if (_mode != -1)
       initialMode = _mode | S_IWUSR;
    else
       initialMode = 0666;

    int fd = open(dest, O_CREAT | O_TRUNC | O_WRONLY, initialMode);
    m_fPut = fdopen( fd, "wb" );
    if ( m_fPut == 0L ) {
	kDebugInfo( 7101, "####################### COULD NOT WRITE %s", debugString(dest));
        if ( errno == EACCES ) {
            error( KIO::ERR_WRITE_ACCESS_DENIED, dest );
        } else {
            error( KIO::ERR_CANNOT_OPEN_FOR_WRITING, dest );
        }
        fclose(f);
        return;
    }

    ready();

    totalSize( buff_src.st_size );
    int processed_size = 0;
    time_t t_start = time( 0L );
    time_t t_last = t_start;

    char buffer[ 4090 ];
    QByteArray array;

    while( !feof( f ) )
	{
	    int n = fread( buffer, 1, 2048, f );

            if (n == -1)
            {
               error( KIO::ERR_COULD_NOT_READ, src);
               fclose(m_fPut);
               fclose(f);
               return;
            }

            fwrite( buffer, n, 1, m_fPut );

	    processed_size += n;
	    time_t t = time( 0L );
	    if ( t - t_last >= 1 )
		{
		    processedSize( processed_size );
		    speed( processed_size / ( t - t_start ) );
		    t_last = t;
		}
	}

    fclose( f );

    if (fclose( m_fPut))
    {
        error( KIO::ERR_COULD_NOT_WRITE, dest);
        return;
    }

    // set final permissions
    if ( _mode != -1 )
    {
       if (::chmod(dest, _mode) != 0)
       {
           error( KIO::ERR_CANNOT_CHMOD, dest );
           return;
       }
    }

    processedSize( buff_src.st_size );
    time_t t = time( 0L );
    if ( t - t_start >= 1 )
	speed( processed_size / ( t - t_start ) );

    finished();
}

void FileProtocol::rename( const QString &src, const QString &dest,
                           bool _overwrite )
{
    struct stat buff_src;
    if ( ::stat( src, &buff_src ) == -1 ) {
        if ( errno == EACCES )
           error( KIO::ERR_ACCESS_DENIED, src );
        else
           error( KIO::ERR_DOES_NOT_EXIST, src );
	return;
    }

    struct stat buff_dest;
    bool dest_exists = ( ::stat( dest, &buff_dest ) != -1 );
    if ( dest_exists )
    {
        if (S_ISDIR(buff_dest.st_mode))
        {
           error( KIO::ERR_DIR_ALREADY_EXIST, dest );
           return;
        }

        if (!_overwrite)
        {
           error( KIO::ERR_FILE_ALREADY_EXIST, dest );
           return;
        }
    }

    ready();

    if ( ::rename( src, dest))
    {
        if (( errno == EACCES ) || (errno == EPERM)) {
            error( KIO::ERR_ACCESS_DENIED, dest );
        }
        else if (errno == EXDEV) {
           error( KIO::ERR_UNSUPPORTED_ACTION, "rename");
        }
        else {
           error( KIO::ERR_CANNOT_RENAME, src );
        }
        return;
    }

    finished();
}


void FileProtocol::del( const QString& path, bool isfile)
{
    /*****
     * Delete files
     *****/

    if (isfile) {
	kdDebug( 7101 ) <<  "Deleting file "<< path << endl;
	
	// TODO deletingFile( source );
	
	if ( unlink( path ) == -1 ) {
            if ((errno == EACCES) || (errno == EPERM))
               error( KIO::ERR_ACCESS_DENIED, path);
            else if (errno == EISDIR)
               error( KIO::ERR_IS_DIRECTORY, path);
            else
               error( KIO::ERR_CANNOT_DELETE, path );
	    return;
	}
    } else {
	
	/*****
	 * Delete empty directory
	 *****/
	
	kDebugInfo( 7101, "Deleting directory %s", debugString(path) );

	// TODO deletingFile( source );
	

      /*****
       * Delete empty directory
       *****/
	
      kdDebug( 7101 ) << "Deleting directory " << debugString(path) << endl;

      if ( ::rmdir( path ) == -1 ) {
	if ((errno == EACCES) || (errno == EPERM))
	  error( KIO::ERR_ACCESS_DENIED, path);
	else {
	  kdDebug( 7101 ) << "could not rmdir " << perror << endl;
	  error( KIO::ERR_COULD_NOT_RMDIR, path );
	  return;
	}
      }
    }

    finished();
}

void FileProtocol::createUDSEntry( const QString & filename, const QString & path, UDSEntry & entry  )
{
    assert(entry.count() == 0); // by contract :-)
    UDSAtom atom;
    atom.m_uds = KIO::UDS_NAME;
    atom.m_str = filename;
    entry.append( atom );
	
    mode_t type;
    mode_t access;
    struct stat buff;

	if ( lstat( path, &buff ) == 0 )  {
	
	    if (S_ISLNK(buff.st_mode)) {

		// A link poiting to nowhere ?
		if ( ::stat( path, &buff ) == -1 ) {
		    // It is a link pointing to nowhere
		    type = S_IFMT - 1;
		    access = S_IRWXU | S_IRWXG | S_IRWXO;

		    atom.m_uds = KIO::UDS_FILE_TYPE;
		    atom.m_long = type;
		    entry.append( atom );
		
		    atom.m_uds = KIO::UDS_ACCESS;
		    atom.m_long = access;
		    entry.append( atom );
		
		    goto notype;

		} else {
		    char buffer2[ 1000 ];
		    int n = readlink( path, buffer2, 1000 );
		    if ( n != -1 ) {
			buffer2[ n ] = 0;
		    }

		    atom.m_uds = KIO::UDS_LINK_DEST;
		    atom.m_str = buffer2;
		    entry.append( atom );
		}
	    }
	}
	
	type = buff.st_mode & S_IFMT; // extract file type
	access = buff.st_mode & 0x1FF; // extract permissions

	atom.m_uds = KIO::UDS_FILE_TYPE;
	atom.m_long = type;
	entry.append( atom );
	
	atom.m_uds = KIO::UDS_ACCESS;
	atom.m_long = access;
	entry.append( atom );

	atom.m_uds = KIO::UDS_SIZE;
	atom.m_long = buff.st_size;
	entry.append( atom );

    notype:
	atom.m_uds = KIO::UDS_MODIFICATION_TIME;
	atom.m_long = buff.st_mtime;
	entry.append( atom );

	atom.m_uds = KIO::UDS_USER;
	uid_t uid = buff.st_uid;
	QString *temp = usercache.find( uid );

	if ( !temp ) {
	    struct passwd *user = getpwuid( uid );
	    if ( user ) {
		usercache.insert( uid, new QString(user->pw_name) );
		atom.m_str = user->pw_name;
	    }
	    else
		atom.m_str = "???";
	}
	else
	    atom.m_str = *temp;
	entry.append( atom );

	atom.m_uds = KIO::UDS_GROUP;
	gid_t gid = buff.st_gid;
	temp = groupcache.find( gid );
	if ( !temp ) {
	    struct group *grp = getgrgid( gid );
	    if ( grp ) {
		groupcache.insert( gid, new QString(grp->gr_name) );
		atom.m_str = grp->gr_name;
	    }
	    else
		atom.m_str = "???";
	}
	else
	    atom.m_str = *temp;
	entry.append( atom );

	atom.m_uds = KIO::UDS_ACCESS_TIME;
	atom.m_long = buff.st_atime;
	entry.append( atom );

	atom.m_uds = KIO::UDS_CREATION_TIME;
	atom.m_long = buff.st_ctime;
	entry.append( atom );
}

void FileProtocol::stat( const QString & path )
{
    struct stat buff;
    if ( ::stat( path, &buff ) == -1 ) {
	error( KIO::ERR_DOES_NOT_EXIST, path );
	return;
    }

    // Extract filename out of path
    QString filename = KURL( path ).filename();

    UDSEntry entry;
    createUDSEntry( filename, path, entry );
///////// debug code

    KIO::UDSEntry::ConstIterator it = entry.begin();
    for( ; it != entry.end(); it++ ) {
        switch ((*it).m_uds) {
            case KIO::UDS_FILE_TYPE:
                kDebugInfo("File Type : %d", (mode_t)((*it).m_long) );
                break;
            case KIO::UDS_ACCESS:
                kDebugInfo("Access permissions : %d", (mode_t)((*it).m_long) );
                break;
            case KIO::UDS_USER:
                kDebugInfo("User : %s", ((*it).m_str.ascii() ) );
                break;
            case KIO::UDS_GROUP:
                kDebugInfo("Group : %s", ((*it).m_str.ascii() ) );
                break;
            case KIO::UDS_NAME:
                kDebugInfo("Name : %s", ((*it).m_str.ascii() ) );
                //m_strText = decodeFileName( (*it).m_str );
                break;
            case KIO::UDS_URL:
                kDebugInfo("URL : %s", ((*it).m_str.ascii() ) );
                break;
            case KIO::UDS_MIME_TYPE:
                kDebugInfo("MimeType : %s", ((*it).m_str.ascii() ) );
                break;
            case KIO::UDS_LINK_DEST:
                kDebugInfo("LinkDest : %s", ((*it).m_str.ascii() ) );
                break;
        }
    }

/////////
    statEntry( entry );

    finished();
}

void FileProtocol::listDir( const QString& path )
{
    kDebugInfo( 7101, "=============== LIST %s ===============", debugString(path) );

    struct stat buff;
    if ( ::stat( path, &buff ) == -1 ) {
	error( KIO::ERR_DOES_NOT_EXIST, path );
	return;
    }

    if ( !S_ISDIR( buff.st_mode ) ) {
	error( KIO::ERR_IS_FILE, path );
	return;
    }

    DIR *dp = 0L;
    struct dirent *ep;

    dp = opendir( path );
    if ( dp == 0 ) {
	error( KIO::ERR_CANNOT_ENTER_DIRECTORY, path );
	return;
    }

    QStringList entryNames;

    while ( ( ep = readdir( dp ) ) != 0L )
	entryNames.append( ep->d_name );

    closedir( dp );
    totalFiles( entryNames.count() );

    // set the current dir
    char path_buffer[PATH_MAX];
    getcwd(path_buffer, PATH_MAX - 1);
    chdir( path );

    UDSEntry entry;
    QStringList::Iterator it (entryNames.begin());

    for (; it != entryNames.end(); ++it) {
	entry.clear();
        createUDSEntry( *it, *it /* we can use the filename as relative path*/, entry );
	listEntry( entry, false);
    }

    listEntry( entry, true ); // ready

    kDebugInfo(7101, "============= COMPLETED LIST ============" );

    chdir(path_buffer);

    finished();

    kDebugInfo(7101, "=============== BYE ===========" );
}

/*
void FileProtocol::testDir( const QString& path )
{
    struct stat buff;
    if ( ::stat( path, &buff ) == -1 ) {
	error( KIO::ERR_DOES_NOT_EXIST, path );
	return;
    }

    if ( S_ISDIR( buff.st_mode ) )
	isDirectory();
    else
	isFile();

    finished();
}
*/

void FileProtocol::special( const QByteArray &data)
{
    int tmp;
    QDataStream stream(data, IO_ReadOnly);

    stream >> tmp;
    switch (tmp) {
    case 1:
      {
		QString fstype, dev, point;
	Q_INT8 iRo;
	
	stream >> iRo >> fstype >> dev >> point;
	
	bool ro = ( iRo != 0 );
	
	kDebugInfo(7006 ,"!!!!!!!!! MOUNTING %s %s %s",
		   debugString(fstype), debugString(dev), debugString(point) );
	mount( ro, fstype, dev, point );
	
      }
      break;
    case 2:
      {
	QString point;
	stream >> point;
	unmount( point );
      }
      break;
      
    case 3:
    {
      QString filename;
      stream >> filename;
      if (!KShred::shred(filename))
        error( KIO::ERR_CANNOT_DELETE, filename );
      finished();
      break;
    }
    default:
      assert(0);
    }
}
void FileProtocol::mount( bool _ro, const char *_fstype, const QString& _dev, const QString& _point )
{
    QString buffer;

    int t = (int)time( 0L );

    // Look in /etc/fstab ?
    if ( !_fstype || *_fstype == 0 || _point.isEmpty() )
	buffer.sprintf( "mount %s 2>/tmp/mnt%i", QFile::encodeName(_dev).data(), t );
    else if ( _ro )
	buffer.sprintf( "mount -rt %s %s %s 2>/tmp/mnt%i",_fstype,
			QFile::encodeName(_dev).data(),
			QFile::encodeName(_point).data(), t );
    else
	buffer.sprintf( "mount -t %s %s %s 2>/tmp/mnt%i", _fstype,
			QFile::encodeName(_dev).data(),
			QFile::encodeName(_point).data(), t );

    system( buffer );

    buffer.sprintf( "/tmp/mnt%i", t );

    QString err = testLogFile( buffer );
    if ( err.isEmpty() )
	finished();
    else
        error( KIO::ERR_COULD_NOT_MOUNT, err );
}


void FileProtocol::unmount( const QString& _point )
{
    QString buffer;

    int t = (int)time( 0L );

    buffer.sprintf( "umount %s 2>/tmp/mnt%i", QFile::encodeName(_point).data(), t );
    system( buffer );

    buffer.sprintf( "/tmp/mnt%i", t );

    QString err = testLogFile( buffer );
    if ( err.isEmpty() )
	finished();
    else
        error( KIO::ERR_COULD_NOT_MOUNT, err );
}

/*************************************
 *
 * Utilities
 *
 *************************************/

QString testLogFile( const QString& _filename )
{
    char buffer[ 1024 ];
    struct stat buff;

    QString result;

    stat( _filename.local8Bit(), &buff );
    int size = buff.st_size;
    if ( size == 0 ) {
	unlink( _filename.local8Bit() );
	return result;
    }

    FILE * f = fopen( _filename, "rb" );
    if ( f == 0L ) {
	unlink( _filename );
	result = "Could not read ";
	result += _filename;
	return result;
    }

    result = "";
    const char *p = "";
    while ( p != 0L ) {
	p = fgets( buffer, sizeof(buffer)-1, f );
	if ( p != 0L )
	    result += buffer;
    }

    fclose( f );

    unlink( _filename );

    return result;
}

extern "C" {
    SlaveBase *init_file() {
	return new FileProtocol();
    }
}

