/*
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 1999 Lars Knoll (knoll@kde.org)
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
#ifndef RENDERTEXT_H
#define RENDERTEXT_H

#include "dom_string.h"
#include "dom_stringimpl.h"
#include "render_object.h"

#include <assert.h>

class QPainter;
class QFontMetrics;

namespace khtml
{
    class RenderText;
    class RenderStyle;

class TextSlave
{
public:
    TextSlave(int _x, int _y, QChar *text, int _len,
              int height, int baseline, int width, bool _deleteText = false, bool _firstLine = false)
    {
        x = _x;
        y = _y;
        n = 0;
        m_text = text;
        len = _len;
        m_height = height;
        m_baseline = baseline;
        m_width = width;
        deleteText = _deleteText;
        firstLine = _firstLine;
    }
    ~TextSlave();
    void print( QPainter *p, int _tx, int _ty);
    void printDecoration( QPainter *p, int _tx, int _ty, int decoration);
    void printBoxDecorations(QPainter *p, RenderText *parent, int _tx, int _ty, bool begin, bool end);
    void printActivation( QPainter *, int, int );

    bool checkPoint(int _x, int _y, int _tx, int _ty);

    /**
     * if this textslave was rendered @ref _ty pixels below the upper edge
     * of a view, would the @ref _y -coordinate be inside the vertical range
     * of this object's representation?
     */
    bool checkVerticalPoint(int _y, int _ty, int _h)
    { if((_ty + y > _y + _h) || (_ty + y + m_height < _y)) return false; return true; }
    void printSelection(QPainter *p, RenderText* rt, int tx, int ty, int startPos, int endPos);

    void setNext(TextSlave *_n) { n = _n; }
    TextSlave *next() { return n; }

    int x;
    int y;
    QChar* m_text;
    int len;

    TextSlave *n;
    unsigned short m_height;
    unsigned short m_baseline;
    unsigned short m_width;

    // this is needed for right to left text. In this case, m_text will point to a QChar array which
    // holds the already reversed string. The slave has to delete this string by itself.
    bool deleteText;
    bool firstLine;
};

class RenderText : public RenderObject
{
public:
    RenderText(DOM::DOMStringImpl *_str);
    virtual ~RenderText();

    virtual const char *renderName() const { return "RenderText"; }

    virtual void setStyle(RenderStyle *style);

    virtual bool isRendered() const { return true; }

    virtual void print( QPainter *, int x, int y, int w, int h,
                        int tx, int ty);
    virtual void printObject( QPainter *, int x, int y, int w, int h,
                        int tx, int ty);

    void deleteSlaves();

    DOM::DOMString data() const { return str; }
    DOM::DOMStringImpl *string() const { return str; }

    virtual void layout() {assert(false);}

    bool checkPoint(int _x, int _y, int _tx, int _ty, int &off);

    virtual unsigned int length() const { return str->l; }
    // no need for this to be virtual, however length needs to be!
    inline QChar *text() const { return str->s; }
    virtual void position(int x, int y, int from, int len, int width, bool reverse, bool firstLine);
    virtual unsigned int width( int from, int len) const;

    virtual int height() const;

    // from BiDiObject
    // height of the contents (without paddings, margins and borders)
    virtual int bidiHeight() const;

    // overrides
    virtual void calcMinMaxWidth();
    virtual short minWidth() const { return m_minWidth; }
    virtual short maxWidth() const { return m_maxWidth; }

    virtual int xPos() const;
    virtual int yPos() const;

    virtual short baselineOffset() const;
    virtual short verticalPositionHint() const;

    virtual const QFont &font();
    const QFontMetrics *metrics() const { return fm; }

    bool isFixedWidthFont() const;

    void setText(DOM::DOMStringImpl *text);

    TextSlave *first() { return m_first; }
    TextSlave *last() { return m_last; }

    virtual SelectionState selectionState() const {return m_selectionState;}
    virtual void setSelectionState(SelectionState s) {m_selectionState = s; }
    virtual void cursorPos(int offset, int &_x, int &_y, int &height);
    virtual void absolutePosition(int &/*xPos*/, int &/*yPos*/, bool f = false);
    void posOfChar(int ch, int &x, int &y);

    virtual short marginLeft() const { return m_style->marginLeft().minWidth(0); }
    virtual short marginRight() const { return m_style->marginRight().minWidth(0); }

    virtual void repaint();
protected:

    QFontMetrics *fm;
    DOM::DOMStringImpl *str;
    TextSlave *m_first;
    TextSlave *m_last;

    int m_contentHeight;
    short m_minWidth;
    short m_maxWidth;

    SelectionState m_selectionState : 3 ;
};


inline bool isBreakable( const QChar *c )
{
    char ch = c->latin1();
    if ( ! ch ) {
	// not latin1, need to do more sophisticated checks for asian fonts
	unsigned char row = c->row();
	if ( row < 0x11 ) // no asian font
	    return false;
	if ( row > 0x2d && row < 0xfb || row == 0x11 )
	    return true;
    } else {
	if ( ch == ' ' || ch == '\n' )
	    return true;
    }
    return false;
}



};
#endif
