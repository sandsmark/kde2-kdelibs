/* This file is part of the KDE libraries
 *  Copyright (C) 2000 Yves Arrouye <yves@realnames.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
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

#include <config.h>
#include <kmimetype.h>
#include <klibloader.h>
#include <ktrader.h>
#include <kdebug.h>

#include "kurifilter.h"

template class QList<KURIFilterPlugin>;

KURIFilterPlugin::KURIFilterPlugin( QObject *parent, const char *name, double pri )
                 :QObject( parent, name )
{
    m_strName = QString::fromLatin1( name );
    m_dblPriority = pri;
}

void KURIFilterPlugin::setFilteredURI( KURIFilterData& data, const KURL& uri ) const
{
    if ( data.uri() != uri )
    {
        data.m_pURI = uri;
        data.m_bChanged = true;
    }
    // data.m_bFiltered = true; deprecated!!!
}

KURIFilterData::KURIFilterData( const KURIFilterData& data )
{
    m_iType = data.m_iType;
    m_pURI = data.m_pURI;
    m_strErrMsg = data.m_strErrMsg;
    m_strIconName = data.m_strIconName;
    // m_bFiltered = data.m_bFiltered;  deprecated!!!
    m_bChanged = data.m_bChanged;

}

void KURIFilterData::init( const KURL& url )
{
    m_iType = KURIFilterData::UNKNOWN;
    m_pURI = url;
    m_strErrMsg = QString::null;
    m_strIconName = QString::null;
    m_bFiltered = true;  //deprecated!!! Always returns true!
    m_bChanged = true;
}

QString KURIFilterData::iconName()
{
    if( m_bChanged )
    {
        switch ( m_iType )
        {
            case KURIFilterData::LOCAL_FILE:
            case KURIFilterData::LOCAL_DIR:
            case KURIFilterData::NET_PROTOCOL:
            {
                KMimeType::Ptr mimetype = KMimeType::findByURL( m_pURI );
                if (mimetype)
                    m_strIconName = mimetype->icon( m_pURI, false );
                break;
            }
            case KURIFilterData::EXECUTABLE:
            {
                KService::Ptr service = KService::serviceByDesktopName( m_pURI.url() );
                if (service)
                    m_strIconName = service->icon();
                else
                    m_strIconName = QString::fromLatin1("exec");
                break;
            }
            case KURIFilterData::HELP:
            {
                m_strIconName = QString::fromLatin1("khelpcenter");
                break;
            }
            case KURIFilterData::SHELL:
            {
                m_strIconName = QString::fromLatin1("konsole");
                break;
            }
            case KURIFilterData::ERROR:
            case KURIFilterData::BLOCKED:
            {
                m_strIconName = QString::fromLatin1("error");
                break;
            }
            default:
                m_strIconName = QString::null;
                break;
        }
        m_bChanged = false;
    }
    return m_strIconName;
}

//********************************************  KURIFilter **********************************************
KURIFilter *KURIFilter::ms_pFilter = 0;

KURIFilter::KURIFilter()
{
    m_lstPlugins.setAutoDelete(true);
    loadPlugins();
}

KURIFilter *KURIFilter::self()
{
    if (!ms_pFilter)
        ms_pFilter = new KURIFilter();

    return ms_pFilter;
}

bool KURIFilter::filterURI( KURIFilterData& data, const QStringList& filters )
{
    bool filtered = false;
    KURIFilterPluginList use_plugins;

    // If we have don't have a filter list, only include the
    // once that were specified.  Otherwise use all available
    // filters...
    if( filters.isEmpty() )
        use_plugins = m_lstPlugins;  // Use everything that is loaded...
    else
    {
        for( QStringList::ConstIterator lst = filters.begin(); lst != filters.end(); ++lst )
        {
            QListIterator<KURIFilterPlugin> it( m_lstPlugins );
            for( ; it.current() ; ++it )
            {
                kdDebug() << "Requested Filter: " << (*lst) << "    Current Filter: " << it.current()->name() << endl;
                if( (*lst) == it.current()->name() )
                {
                    kdDebug() << "Adding filter to \"OK-TO-USE\" list..." << endl;
                    use_plugins.append( it.current() );
                    break;  // We already found it ; so lets test the next named filter...
                }
            }
        }
    }

    QListIterator<KURIFilterPlugin> it( use_plugins );
    kdDebug() << "Using " << use_plugins.count() << " out of " << m_lstPlugins.count() << endl;
    for (; it.current() && !filtered; ++it)
    {
        kdDebug() << "Using the named filter plugin: " << it.current()->name() << endl;
        filtered |= it.current()->filterURI( data );
    }
    return filtered;
}

bool KURIFilter::filterURI( KURL& uri, const QStringList& filters )
{
    KURIFilterData data = uri;
    bool filtered = filterURI( data, filters );
    if( filtered ) uri = data.uri();
    return filtered;
}

bool KURIFilter::filterURI( QString& uri, const QStringList& filters )
{
    KURIFilterData data = uri;
    bool filtered = filterURI( data, filters );
    if( filtered )  uri = data.uri().url();
    return filtered;

}

KURL KURIFilter::filteredURI( const KURL &uri, const QStringList& filters )
{
    KURIFilterData data = uri;
    filterURI( data, filters );
    return data.uri();
}

QString KURIFilter::filteredURI( const QString &uri, const QStringList& filters )
{
    KURIFilterData data = uri;
    filterURI( data, filters );
    return data.uri().url();
}

QListIterator<KURIFilterPlugin> KURIFilter::pluginsIterator() const
{
    return QListIterator<KURIFilterPlugin>(m_lstPlugins);
};

void KURIFilter::loadPlugins()
{
    KTrader::OfferList offers = KTrader::self()->query( "KURIFilter/Plugin" );
    KTrader::OfferList::ConstIterator it = offers.begin();
    KTrader::OfferList::ConstIterator end = offers.end();

    for (; it != end; ++it )
    {
        if ((*it)->library().isEmpty()) { continue; }
        KLibFactory *factory = KLibLoader::self()->factory((*it)->library().latin1());
        if (!factory) { continue; }
        KURIFilterPlugin *plugin = (KURIFilterPlugin *) factory->create(0, (*it)->desktopEntryName().latin1(), "KURIFilterPlugin");
        if ( plugin ) { m_lstPlugins.append( plugin ); }
    }
     // NOTE: Plugin priority is now determined by
     // the entry in the .desktop files...
     // TODO: Config dialog to differentiate "system"
     // plugins from "user-defined" ones...
     // m_lstPlugins.sort();
}

#include "kurifilter.moc"

