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
#ifndef RENDER_LIST_H
#define RENDER_LIST_H

#include "render_flow.h"

// ### list-style-position, list-style-image is still missing

namespace khtml
{
    /* used to render the lists marker.
       This class always has to be a direct child of a RenderListItem!
     */
    class RenderListMarker : public RenderBox
    {
    public:
	RenderListMarker( RenderStyle *style );
	
	virtual const char *renderName() const { return "RenderListMarker"; }
	virtual bool isInline() const { return true; }
	// so the marker gets to layout itself. Only needed for
	// list-style-position: inside
	virtual bool isReplaced() const { return true; }

	virtual void print(QPainter *p, int x, int y, int w, int h,
			   int xoff, int yoff);
	virtual void printObject(QPainter *p, int x, int y, int w, int h,
				 int xoff, int yoff);
	virtual void layout( bool );
	virtual void calcMinMaxWidth();
	
	long int val;
	QString item;
    };


class RenderListItem : public RenderFlow
{
public:
    RenderListItem(RenderStyle *style);
    virtual ~RenderListItem();

    virtual const char *renderName() const { return "RenderListItem"; }

    virtual bool isListItem() const { return true; }
    virtual bool isInline() const { return false; }

    long value() const { return m_marker->val; }
    void setValue( long v ) { predefVal = v; }
    void calcListValue();

    virtual void print(QPainter *p, int x, int y, int w, int h,
		       int xoff, int yoff);
    virtual void printObject(QPainter *p, int x, int y, int w, int h,
		       int xoff, int yoff);

    virtual void layout( bool deep = false );

protected:
    long int predefVal;
    RenderListMarker *m_marker;
};

}; //namespace

#endif
