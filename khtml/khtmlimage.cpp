/* This file is part of the KDE project
   Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "khtmlimage.h"
#include "khtml_part.h"
#include "misc/loader.h"

#include <qvbox.h>

#include <kparts/factory.h>
#include <kio/job.h>
#include <kglobal.h>
#include <kinstance.h>
#include <kaction.h>

extern "C"
{
    void *init_libkhtmlimage()
    {
        return new KHTMLImageFactory();
    }
};

KInstance *KHTMLImageFactory::s_instance = 0;

KHTMLImageFactory::KHTMLImageFactory()
{
    s_instance = new KInstance( "khtmlimagepart" );
}

KHTMLImageFactory::~KHTMLImageFactory()
{
    delete s_instance;
}

KParts::Part *KHTMLImageFactory::createPartObject( QWidget *parentWidget, const char *widgetName,
                                                   QObject *parent, const char *name,
                                                   const char *, const QStringList & )
{
    return new KHTMLImage( parentWidget, widgetName, parent, name );
}

KHTMLImage::KHTMLImage( QWidget *parentWidget, const char *widgetName,
                        QObject *parent, const char *name )
    : KParts::ReadOnlyPart( parent, name )
{
    setInstance( KHTMLImageFactory::instance() );

    QVBox *box = new QVBox( parentWidget, widgetName );

    m_khtml = new KHTMLPart( box, widgetName, this, "htmlimagepart" );

    setWidget( box );

    setXMLFile( m_khtml->xmlFile() );

    m_ext = new KParts::BrowserExtension( this, "be" );

    connect( KParts::BrowserExtension::childObject( m_khtml ), SIGNAL( popupMenu( KXMLGUIClient *, const QPoint &, const KURL &, const QString &, mode_t ) ),
             this, SLOT( slotPopupMenu( KXMLGUIClient *, const QPoint &, const KURL &, const QString &, mode_t ) ) );

    m_ext->setURLDropHandlingEnabled( true );
}

KHTMLImage::~KHTMLImage()
{
    // important: delete the html part before the part or qobject destructor runs.
    // we now delete the htmlpart which deletes the part's widget which makes
    // _OUR_ m_widget 0 which in turn avoids our part destructor to delete the
    // widget ;-)
    delete m_khtml;
}

bool KHTMLImage::openURL( const KURL &url )
{
    static const QString &html = KGlobal::staticQString( "<html><body><img src=\"%1\"></body></html>" );

    m_url = url;

    emit started( 0 );

    m_mimeType = m_ext->urlArgs().serviceType;

    m_khtml->begin( m_url );
    m_khtml->write( html.arg( m_url.url() ) );
    m_khtml->end();

    KIO::Job *job = khtml::Cache::loader()->jobForRequest( m_url.url() );

    if ( job )
    {
        emit started( job );

        connect( job, SIGNAL( result( KIO::Job * ) ),
                 this, SLOT( slotImageJobFinished( KIO::Job * ) ) );
    }
    else
    {
        emit started( 0 );
        emit completed();
    }

    return true;
}

bool KHTMLImage::closeURL()
{
    return true;
}

void KHTMLImage::guiActivateEvent( KParts::GUIActivateEvent *e )
{
    if ( e->activated() )
        emit setWindowCaption( m_url.prettyURL() );
}

void KHTMLImage::slotPopupMenu( KXMLGUIClient *cl, const QPoint &pos, const KURL &u,
                                const QString &, mode_t mode )
{
    KAction *encodingAction = cl->actionCollection()->action( "setEncoding" );
    if ( encodingAction )
        cl->actionCollection()->take( encodingAction );
    emit m_ext->popupMenu( cl, pos, u, m_mimeType, mode );
}

void KHTMLImage::slotImageJobFinished( KIO::Job *job )
{
    if ( job->error() )
    {
        job->showErrorDialog();
        emit canceled( job->errorString() );
    }
    else
        emit completed();
}

#include "khtmlimage.moc"
