/**
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * $Id$
 */
#include "html_documentimpl.h"

#include "khtmlview.h"
#include "khtml_part.h"
#include "khtml_settings.h"
#include "misc/htmlattrs.h"

#include "htmltokenizer.h"
#include "xml_tokenizer.h"
#include "htmlhashes.h"
#include "html_imageimpl.h"
#include "html_headimpl.h"
#include "html/html_baseimpl.h"

#include "khtml_factory.h"
#include "html_miscimpl.h"
#include "rendering/render_object.h"

#include <kdebug.h>
#include <kurl.h>
#include <kglobal.h>
#include <kcharsets.h>
#include <kglobalsettings.h>

#include "css/cssstyleselector.h"
#include "css/css_stylesheetimpl.h"
#include <stdlib.h>
#include <qstack.h>

template class QStack<DOM::NodeImpl>;

using namespace DOM;
using namespace khtml;


HTMLDocumentImpl::HTMLDocumentImpl() : DocumentImpl()
{
//    kdDebug( 6090 ) << "HTMLDocumentImpl constructor this = " << this << endl;
    bodyElement = 0;
    htmlElement = 0;

    connect( KHTMLFactory::vLinks(), SIGNAL( inserted( const QString& )),
	     SLOT( slotHistoryChanged() ));
    connect( KHTMLFactory::vLinks(), SIGNAL( removed( const QString& )),
	     SLOT( slotHistoryChanged() ));
    connect( KHTMLFactory::vLinks(), SIGNAL( cleared()),
	     SLOT( slotHistoryChanged() ));
}

HTMLDocumentImpl::HTMLDocumentImpl(KHTMLView *v)
    : DocumentImpl(v)
{
//    kdDebug( 6090 ) << "HTMLDocumentImpl constructor2 this = " << this << endl;
    bodyElement = 0;
    htmlElement = 0;

    m_styleSelector = new CSSStyleSelector(this);

    connect( KHTMLFactory::vLinks(), SIGNAL( inserted( const QString& )),
	     SLOT( slotHistoryChanged() ));
    connect( KHTMLFactory::vLinks(), SIGNAL( removed( const QString& )),
	     SLOT( slotHistoryChanged() ));
    connect( KHTMLFactory::vLinks(), SIGNAL( cleared()),
	     SLOT( slotHistoryChanged() ));
}

HTMLDocumentImpl::~HTMLDocumentImpl()
{
}

DOMString HTMLDocumentImpl::referrer() const
{
    // ### should we fix that? I vote against for privacy reasons
    return DOMString();
}

DOMString HTMLDocumentImpl::domain() const
{
    // ### do they want the host or the domain????
    KURL u(url.string());
    return u.host();
}

HTMLElementImpl *HTMLDocumentImpl::body()
{
    NodeImpl *de = documentElement();
    if (!de)
	return 0;
    NodeImpl *b = de->firstChild();
    while (b && b->id() != ID_BODY && b->id() != ID_FRAMESET)
	b = b->nextSibling();
    return static_cast<HTMLElementImpl *>(b);
}

void HTMLDocumentImpl::setBody(const HTMLElement &/*_body*/)
{
    // ###
}

Tokenizer *HTMLDocumentImpl::createTokenizer()
{
    return new HTMLTokenizer(this,m_view);
}

NodeListImpl *HTMLDocumentImpl::getElementsByName( const DOMString &elementName )
{
    return new NameNodeListImpl( documentElement(), elementName );
}

StyleSheetListImpl *HTMLDocumentImpl::styleSheets()
{
    // ### implement for html
    return 0;
}


// --------------------------------------------------------------------------
// not part of the DOM
// --------------------------------------------------------------------------

void HTMLDocumentImpl::detach()
{
    // onunload script...
    if(m_view && m_view->part() && m_view->part()->jScriptEnabled() && body()) {
        DOMString script = body()->getAttribute(ATTR_ONUNLOAD);
        if(script.length()) {
            //kdDebug( 6030 ) << "emit executeScript( " << script.string() << " )" << endl;
            m_view->part()->executeScript( Node(this), script.string() );
        }
    }
    kdDebug( 6090 ) << "HTMLDocumentImpl::detach()" << endl;
    m_view = 0;

    DocumentImpl::detach();
}

void HTMLDocumentImpl::slotFinishedParsing()
{
    // first propagate that we finished parsing
    DocumentImpl::slotFinishedParsing();

    // and then execute the onload script... as the part
    // blocks submitting while we're still parsing
    if(m_view && m_view->part()->jScriptEnabled() && body()) {
        DOMString script = body()->getAttribute(ATTR_ONLOAD);
        if(script.length()) {
            kdDebug( 6030 ) << "emit executeScript( " << script.string() << " )" << endl;
            m_view->part()->executeScript( Node(this), script.string() );
        }
    }
    if ( !onloadScript.isEmpty() )
	m_view->part()->executeScript( Node(this), onloadScript );
}

bool HTMLDocumentImpl::childAllowed( NodeImpl *newChild )
{
    return (newChild->id() == ID_HTML || newChild->id() == ID_COMMENT);
}

ElementImpl *HTMLDocumentImpl::createElement( const DOMString &name )
{
    return createHTMLElement(name);
}

QList<StyleSheetImpl> HTMLDocumentImpl::authorStyleSheets()
{
    return htmlAuthorStyleSheets();
}

void HTMLDocumentImpl::slotHistoryChanged()
{
    if ( !m_render )
	return;
    
    recalcStyle();
    m_render->updateSize();
    m_render->repaint();
}

//-----------------------------------------------------------------------------


XHTMLDocumentImpl::XHTMLDocumentImpl() : HTMLDocumentImpl()
{
}

XHTMLDocumentImpl::XHTMLDocumentImpl(KHTMLView *v) : HTMLDocumentImpl(v)
{
}

XHTMLDocumentImpl::~XHTMLDocumentImpl()
{
}

Tokenizer *XHTMLDocumentImpl::createTokenizer()
{
    return new XMLTokenizer(this,m_view);
}



#include "html_documentimpl.moc"
