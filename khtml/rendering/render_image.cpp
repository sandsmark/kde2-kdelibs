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
//#define DEBUG_LAYOUT

#include "render_image.h"

#include <qpixmap.h>
#include <qdrawutil.h>
#include <qpalette.h>

#include <stdio.h>

#include "rendering/render_style.h"

using namespace DOM;
using namespace khtml;

// -------------------------------------------------------------------------

RenderImage::RenderImage(RenderStyle *style)
    : RenderReplaced(style)
{
    bComplete = true;
    setLayouted(false);
}

RenderImage::~RenderImage()
{
}

void RenderImage::setPixmap( const QPixmap &p )
{

    // Image dimensions have been changed, recalculate layout
    //printf("Image: recalculating layout\n");
    if(p.width() != pixmap.width() || p.height() != pixmap.height())
    {
	pixmap = p;
	setLayouted(false);
	layout();
	// the updateSize() call should trigger a repaint too
	if(m_parent) m_parent->updateSize();	
    }
    else
    {
    	pixmap = p;
    	repaintObject(this, 0, 0);
    }
}

void RenderImage::printReplaced(QPainter *p, int _tx, int _ty)
{
    //printf("Image::printObject (%d/%d)\n", width(), height());

    int contentWidth = m_width;
    int contentHeight = m_height;
    int leftBorder = 0;
    int rightBorder = 0;
    int topBorder = 0;
    int bottomBorder = 0;
    if(m_style->hasBorder())
    {
	leftBorder = borderLeft();
	rightBorder = borderRight();
	topBorder =  borderTop();
	bottomBorder = borderBottom();
    }
    if(m_style->hasPadding())
    {
	leftBorder += paddingLeft();
	rightBorder += paddingRight();
	topBorder +=  paddingTop();
	bottomBorder += paddingBottom();
    }
    contentWidth -= leftBorder + rightBorder;
    contentHeight -= topBorder + bottomBorder;

    //printf("    contents (%d/%d) border=%d padding=%d\n", contentWidth, contentHeight, borderLeft(), paddingLeft());

    QRect rect( 0, 0, contentWidth, contentHeight );

    if ( pixmap.isNull() )
    {
	QColorGroup colorGrp( Qt::black, Qt::lightGray, Qt::white, Qt::darkGray, Qt::gray,
			      Qt::black, Qt::white );
	qDrawShadePanel( p, _tx + leftBorder, _ty + topBorder, contentWidth, contentHeight,
			 colorGrp, true, 1 );
	if(!alt.isEmpty())
	{
	    QString text = alt.string();  	
	    p->setFont(style()->font());
	    p->setPen( style()->color() );
	    int ax = _tx + leftBorder + 5;
	    int ay = _ty + topBorder + 5;
	    int ah = contentHeight - 10;
	    int aw = contentWidth - 10;
	    QFontMetrics fm(style()->font());
	    if (aw>15 && ah>fm.height())
    	    	p->drawText(ax, ay, aw, ah , Qt::WordBreak, text );
	}
    }
    else
    {
	if ( (contentWidth != pixmap.width() ||
	    contentHeight != pixmap.height() ) &&
	    pixmap.width() && pixmap.height() )
	{
	  //printf("have to scale: width:%d<-->%d height %d<-->%d\n",
	  //   width - border*2, pixmap.width(),
	  //   getHeight() - border, pixmap.height() );
	    if (resizeCache.isNull())
	    {
		QWMatrix matrix;
		matrix.scale( (float)(contentWidth)/pixmap.width(),
			(float)(contentHeight)/pixmap.height() );
		resizeCache = pixmap.xForm( matrix );
	    }
	    p->drawPixmap( QPoint( _tx + leftBorder, _ty + topBorder ), resizeCache, rect );
	
	}
	else
	    p->drawPixmap( QPoint( _tx + leftBorder, _ty + topBorder ), pixmap, rect );
    }
}

void RenderImage::calcMinMaxWidth()
{
    if(minMaxKnown()) return;
#ifdef DEBUG_LAYOUT
    printf("Image::calcMinMaxWidth() known=%d\n", minMaxKnown());
#endif
    setMinMaxKnown();

    // contentWidth
    Length w = m_style->width();

    switch(w.type)
    {
    case Fixed:
	m_width = w.value;
	m_minWidth = m_width;
	break;
    case Percent:
    	{
	    int nwidth = w.value*containingBlockWidth()/100;
	    if (nwidth != m_width)
		resizeCache.resize(0,0);
	    m_width = nwidth;
	    m_minWidth = 1;
	}
	break;
    default:
	// we still don't know the width...
	if(pixmap.isNull())
	{
	    m_width = 32;
	    setMinMaxKnown(false);
	}
	else if (m_width != pixmap.width())
	{
	    m_width = pixmap.width();
	    setLayouted(false);
	    // if it doesn't fit... make it fit
	    // NO! Images don't scale unless told to. Ever.  -AKo
	    //if(availableWidth < width) width = availableWidth;
	    m_minWidth = m_width;
	    //printf("IMG Width changed, width=%d\n",width);
	}
    }
    m_maxWidth = m_minWidth;

    int toAdd = 0;
    if(m_style->hasBorder())
	toAdd = borderLeft() + borderRight();
    if(m_style->hasPadding())
	toAdd += paddingLeft() + paddingRight();
    m_width += toAdd;
    m_minWidth += toAdd;
    m_maxWidth += toAdd;
}

void RenderImage::layout(bool)
{
    if(layouted()) return;
#ifdef DEBUG_LAYOUT
    printf("Image::layout(?) width=%d, layouted=%d\n", m_width, layouted());
#endif

    calcMinMaxWidth(); // ### just to be sure here...

    Length h = m_style->height();

    switch(h.type)
    {
    case Fixed:
	m_height = h.value;
	break;
    case Percent:
	{
	int hh = m_height;
	hh = h.value*containingBlock()->width()/100;	
	if (m_height != hh)
	{
	    resizeCache.resize(0,0);
	    m_height = hh;	
	}
	}
	break;
    default:
	// we still don't know the height...
	if(pixmap.isNull())
	    m_height = 32;
	else
	{
	    if(m_width == pixmap.width())
		m_height = pixmap.height();
	    else
		m_height = pixmap.height()*m_width/pixmap.width();
	}
    }

    int toAdd = 0;
    if(m_style->hasBorder())
	toAdd = borderTop() + borderBottom();
    if(m_style->hasPadding())
	toAdd += paddingTop() + paddingBottom();
    m_height += toAdd;

    setLayouted();
}

void RenderImage::setImageUrl(DOMString url, DOMString baseUrl)
{
    if(m_bgImage) m_bgImage->deref(this);
    m_bgImage = Cache::requestImage(url, baseUrl);
    m_bgImage->ref(this);
}

void RenderImage::setAlt(DOM::DOMString text)
{
    alt = text;
}

short RenderImage::baselineOffset() const
{
    switch(vAlign())
    {
    case BASELINE:
    case SUB:
    case SUPER:
    case BOTTOM:
	return contentHeight();
    case TOP:
	return 0;
    case TEXT_TOP:
	return QFontMetrics(m_style->font()).ascent();
    case MIDDLE:
	return contentHeight()/2;
    case TEXT_BOTTOM:
	return contentHeight()-QFontMetrics(m_style->font()).descent();
    }
    return 0;
}
