package org.kde.kjas.server;

import java.net.*;
import java.io.*;
import java.util.*;
import java.util.zip.*;
import java.security.*;

/**
 * ClassLoader used to download and instantiate Applets.
 * <P>
 * NOTE: The class loader extends Java 1.2 specific class.
 */
public class KJASAppletClassLoader
    extends URLClassLoader
{
    public static KJASAppletClassLoader createLoader( URL codeBase )
    {
        Main.kjas_debug( "createLoader with: " + codeBase );

        URL[] urls = new URL[1];
        urls[0] = codeBase;

        return new KJASAppletClassLoader( urls );
    }

    public KJASAppletClassLoader( URL[] urls )
    {
        super( urls );
    }

    public void addJar( URL baseURL, String jarname )
    {
        try
        {
            URL newurl = new URL( baseURL, jarname );
            addURL( newurl );
        }
        catch ( MalformedURLException e )
        {
            Main.kjas_err( "bad url creation: " + e, e );
            throw new IllegalArgumentException( jarname );
        }
    }

    public Class loadClass( String name )
        throws ClassNotFoundException
    {
        //We need to be able to handle foo.class, so strip off the suffix
        if( name.endsWith( ".class" ) )
        {
            name = name.substring( 0, name.length() - 6 );
        }

        //try to load it with the parent first...
        try
        {
            return super.loadClass( name );
        }
        catch( ClassNotFoundException e )
        {
            Main.kjas_debug( "super couldn't load class: " + name + ", exception = " + e );
            throw e;
        }
        catch( ClassFormatError e )
        {
            Main.kjas_debug( "Class format error: " + e );
            return null;
        }
    }


    /**
     *  Emergency class loading function- the last resort
     */
    public Class findClass(String name)
        throws ClassNotFoundException
    {
        //All we need to worry about here are classes
        //with kde url's other than those handled by
        //the URLClassLoader

        try
        {
            return super.findClass( name );
        }
        catch( ClassNotFoundException e )
        {
            Main.kjas_debug( "could not find the class: " + name + ", exception = " + e );
            throw e;
        }
    }

    public void addCodeBase( URL url )
    {
        URL[] urls = getURLs();

        boolean inthere = false;
        for( int i = 0; i < urls.length; i++ )
        {
            if( urls[i].equals( url ) )
            {
                inthere = true;
                break;
            }
        }

        if( !inthere )
        {
            Main.kjas_debug( "KJASAppletClassLoader::addCodeBase, just added: " + url );
            addURL( url );
        }
    }

    protected PermissionCollection getPermissions( CodeSource cs )
    {
        //get the permissions from the SecureClassLoader
        final PermissionCollection perms = super.getPermissions( cs );
        final URL url = cs.getLocation();

        //first add permission to connect back to originating host
        perms.add(new SocketPermission(url.getHost(), "connect,accept"));

        //add ability to read from it's own directory...
        if ( url.getProtocol().equals("file") )
        {
            String path = url.getFile().replace('/', File.separatorChar);

	        if (!path.endsWith(File.separator))
            {
                int endIndex = path.lastIndexOf(File.separatorChar);
		        if (endIndex != -1)
                {
			        path = path.substring(0, endIndex+1) + "-";
			        perms.add(new FilePermission(path, "read"));
                }
	        }

            AccessController.doPrivileged(
                new PrivilegedAction()
                {
                    public Object run()
                    {
                        try
                        {
                            if (InetAddress.getLocalHost().equals(InetAddress.getByName(url.getHost())))
                            {
                                perms.add(new SocketPermission("localhost", "connect,accept"));
                            }
                        } catch (UnknownHostException uhe)
                        {}
                        return null;
                    }
                }
            );
        }

        return perms;
    }

    public static void main( String[] args )
    {
        System.out.println( "num args = " + args.length );
        System.out.println( "args[0] = " + args[0] );
        System.out.println( "args[1] = " + args[1] );

        try
        {
            URL location = new URL( args[0] );
            if( location.getFile() == null )
            {
                System.out.println( "getFile returned null" );
            }

            KJASAppletClassLoader loader = KJASAppletClassLoader.createLoader( new URL(args[0]) );
            Class foo = loader.loadClass( args[1] );

            System.out.println( "loaded class: " + foo );
        }
            catch( Exception e )
        {
            System.out.println( "Couldn't load class " + args[1] );
            e.printStackTrace();
        }
    }
}
