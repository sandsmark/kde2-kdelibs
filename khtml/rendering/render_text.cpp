/**
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
//#define DEBUG_LAYOUT
//#define BIDI_DEBUG

#include "render_text.h"
#include "dom_stringimpl.h"

#include "render_style.h"

#include "misc/loader.h"

using namespace khtml;
using namespace DOM;


void TextSlave::print( QPainter *p, int _tx, int _ty)
{

    _ty += m_baseline;

    //p->setPen(QColor("#000000"));
    QConstString s(m_text , len);
    //printf("textSlave::printing(%s) at(%d/%d)\n", s.string().ascii(), x+_tx, y+_ty);
    p->drawText(x + _tx, y + _ty, s.string());
}

// no blink at the moment...
void TextSlave::printDecoration( QPainter *p, int _tx, int _ty, int deco)
{
    _tx += x;
    _ty += y;

    if(deco & UNDERLINE)
	p->drawLine(_tx, _ty + m_height, _tx + m_width, _ty + m_height );
    if(deco & OVERLINE)
	p->drawLine(_tx, _ty, _tx + m_width, _ty );
    if(deco & LINE_THROUGH)
	p->drawLine(_tx, _ty + 2*m_baseline/3, _tx + m_width, _ty + 2*m_baseline/3 );
// ### add BLINK
}

void TextSlave::printBoxDecorations(QPainter *pt, RenderText *p, int _tx, int _ty, bool begin, bool end)
{
    _tx += x;
    _ty += y - p->paddingTop() - p->borderTop();

    //printf("renderBox::printDecorations()\n");
    RenderStyle *style = p->style();

    int width = m_width;
    if(begin)
	_tx -= p->paddingLeft() + p->borderLeft();

    QColor c = style->backgroundColor();
    if(c.isValid())
	pt->fillRect(_tx, _ty, width, m_height, c);
    CachedImage *i = style->backgroundImage();
    if(i)
    {
	// ### might need to add some correct offsets
	// ### use paddingX/Y
	pt->drawTiledPixmap(_tx + p->borderLeft(), _ty + p->borderTop(),
			   m_width + p->paddingLeft() + p->paddingRight(),
			   m_height + p->paddingTop() + p->paddingBottom(), i->pixmap());
    }

    if(style->hasBorder())
    {
	int h = m_height + p->paddingTop() + p->paddingBottom() + p->borderTop() + p->borderBottom();

	if(style->borderTopStyle() != BNONE)
	{
	    c = style->borderTopColor();
	    if(!c.isValid()) c = style->color();
	    p->drawBorder(pt, _tx, _ty, _tx + width, _ty, style->borderTopWidth(),
		       RenderObject::BSTop, c, style->borderTopStyle());
	}
	if(style->borderBottomStyle() != BNONE)
	{
	    c = style->borderBottomColor();
	    if(!c.isValid()) c = style->color();
	    p->drawBorder(pt, _tx, _ty + h, _tx + width, _ty + h, style->borderBottomWidth(),
		       RenderObject::BSBottom, c, style->borderBottomStyle());
	}
	// ### has to be changed for RTL
	if(style->borderLeftStyle() != BNONE && (begin))
	{
	    c = style->borderLeftColor();
	    if(!c.isValid()) c = style->color();
	    p->drawBorder(pt, _tx, _ty, _tx, _ty + h, style->borderLeftWidth(),
		       RenderObject::BSLeft, c, style->borderLeftStyle());
	}
	if(style->borderRightStyle() != BNONE && end)
	{
	    c = style->borderRightColor();
	    if(!c.isValid()) c = style->color();
	    p->drawBorder(pt, _tx + width, _ty, _tx + width, _ty + h, style->borderRightWidth(),
		       RenderObject::BSRight, c, style->borderRightStyle());
	}
    }

#ifdef BIDI_DEBUG
    int h = m_height + p->paddingTop() + p->paddingBottom() + p->borderTop() + p->borderBottom();
    c = QColor("#ff0000");
    p->drawBorder(pt, _tx, _ty, _tx, _ty + h, 1,
		  RenderObject::BSLeft, c, SOLID);
    p->drawBorder(pt, _tx + width, _ty, _tx + width, _ty + h, style->borderRightWidth(),
		  RenderObject::BSRight, c, SOLID);
#endif
}


bool TextSlave::checkPoint(int _x, int _y, int _tx, int _ty)
{
    if((_ty + y > _y) || (_ty + y + m_height < _y) ||
       (_tx + x > _x) || (_tx + x + m_width < _x))
	return false;
    return true;
}

// -------------------------------------------------------------------------------------

RenderText::RenderText(RenderStyle *style, DOMStringImpl *_str)
    : RenderObject(style)
{
    m_first = 0;
    m_last = 0;
    m_minWidth = -1;
    m_maxWidth = -1;
    fm = new QFontMetrics(m_style->font());
    str = _str;
    if(str) str->ref();

#ifdef DEBUG_LAYOUT
    QConstString cstr(str->s, str->l);
    printf("RenderText::setText '%s'\n", (const char *)cstr.string().utf8());
#endif

    // ### add line-height property
    m_contentHeight = m_style->lineHeight().width(fm->height());
}

RenderText::~RenderText()
{
    deleteSlaves();
    delete fm;
    if(str) str->deref();
}

void RenderText::deleteSlaves()
{
    // delete all slaves
    TextSlave *s = m_first;
    while(s)
    {
	TextSlave *next = s->next();
	delete s;
	s = next;
    }
    m_first = m_last = 0;
}

bool RenderText::checkPoint(int _x, int _y, int _tx, int _ty, int &offset)
{
    int off = 0;
    TextSlave *s = m_first;
    while(s)
    {
	if( s->checkPoint(_x, _y, _tx, _ty) )
	{
            // now we need to get the exact position
	    int delta = _x - _tx - s->x;
	    int pos = 0;
	    while(1)
	    {
		// ### this will produce wrong results for RTL text!!!
		int w = fm->width(*(s->m_text+pos));
		int w2 = w/2;
		w = w - w2;
		delta -= w2;
		if(delta <= 0) break;
		pos++;
		delta -= w;
	    }
	    offset = off + pos;
	    //printf(" Text  --> inside at position %d\n", offset);

	    return true;
	}
	// ### this might be wrong, if combining chars are used ( eg arabic )
	off += s->len;
	s=s->next();
    }
    return false;
}

void RenderText::printObject( QPainter *p, int /*x*/, int y, int /*w*/, int h,
		      int tx, int ty)
{
    //printf("Text::printObject(long)\n");

    TextSlave *s = m_first;

    //printf("Text::printObject(2)\n");

    bool start = true;
#ifndef BIDI_DEBUG
    if(m_printSpecial && m_parent->isInline())
    {
#endif
	while(s)
	{
	    bool end = false;
	    if(!s->next()) end = true;
	    if(s->checkVerticalPoint(y, ty, h))
		s->printBoxDecorations(p, this, tx, ty, start, end);
	    s=s->next();
	    start = false;
	}
	s = m_first;
#ifndef BIDI_DEBUG
    }	
#endif

    p->setFont( m_style->font() );
    p->setPen( m_style->color() );
    while(s)
    {
	if(s->checkVerticalPoint(y, ty, h))
	    s->print(p, tx, ty);
	s=s->next();
    }

    int d = m_style->textDecoration();
    if(d != TDNONE)
    {
	p->setPen( m_style->textDecorationColor() );
	s = m_first;
	while(s)
	{
	    if(s->checkVerticalPoint(y, ty, h))
		s->printDecoration(p, tx, ty, d);
	    s=s->next();
	}
    }

}

void RenderText::print( QPainter *p, int x, int y, int w, int h,
		      int tx, int ty)
{
    printObject(p, x, y, w, h, tx, ty);
}

void RenderText::calcMinMaxWidth()
{
    //printf("Text::calcMinMaxWidth(): known=%d\n", minMaxKnown());

    if(minMaxKnown()) return;

    // ### calc Min and Max width...
    m_minWidth = 0;
    m_maxWidth = 0;

    int currMinWidth = 0;
    int currMaxWidth = 0;

    int len = str->l;
    for(int i = 0; i < len; i++)
    {
	const QChar c = *(str->s+i);
	if( c == QChar(' '))
	{
	    if(currMinWidth > m_minWidth) m_minWidth = currMinWidth;
	    currMinWidth = 0;
	    currMaxWidth += fm->width(c);
	}
	else if( c == QChar('-'))
	{
	    currMinWidth += fm->width(c);
	    if(currMinWidth > m_minWidth) m_minWidth = currMinWidth;
	    currMinWidth = 0;
    	    currMaxWidth += fm->width(c);	
	}
	else if( c == QChar('\n'))
	{
	    if(currMinWidth > m_minWidth) m_minWidth = currMinWidth;
	    currMinWidth = 0;
	    if(currMaxWidth > m_maxWidth) m_maxWidth = currMaxWidth;
	    currMaxWidth = 0;
	}
	else
	{
	    int w = fm->width(c);
	    currMinWidth += w;
	    currMaxWidth += w;
	}
    }
    if(currMinWidth > m_minWidth) m_minWidth = currMinWidth;
    currMinWidth = 0;
    if(currMaxWidth > m_maxWidth) m_maxWidth = currMaxWidth;
    currMaxWidth = 0;

    setMinMaxKnown();
}

int RenderText::xPos() const
{
    if (m_first)
	return m_first->x;
    else
	return 0;
}

int RenderText::yPos() const
{
    if (m_first)
	return m_first->y;
    else
	return 0;
}

const QFont &RenderText::font()
{
    return parent()->style()->font();
}

void RenderText::setText(DOMStringImpl *text)
{
    if(str) str->deref();
    str = text;
    if(str) str->ref();

    containingBlock()->layout();
#ifdef DEBUG_LAYOUT
    QConstString cstr(str->s, str->l);
    printf("RenderText::setText '%s'\n", (const char *)cstr.string().utf8());
#endif
}

int RenderText::height() const
{
    return m_contentHeight
	+ m_style->borderTopWidth() + m_style->borderBottomWidth();
   // ### padding is relative to the _width_ of the containing block
    //+ m_style->paddingTop() + m_style->paddingBottom()
 }

int RenderText::bidiHeight() const
{
    return m_contentHeight;
}

short RenderText::baselineOffset() const
{
    return fm->ascent();
}

short RenderText::verticalPositionHint() const
{
    return fm->ascent();
}

void RenderText::position(int x, int y, int from, int len, int width, bool reverse)
{
    QChar *ch;
    bool deleteChar = false;
    if(reverse)
    {
	deleteChar = true;
	// reverse String
	QString aStr = QConstString(str->s+from, len).string();
#ifdef DEBUG_LAYOUT
	printf("reversing '%s' len=%d\n", (const char *)aStr.utf8(), len);
#endif
	aStr.compose();
	len = aStr.length();
	ch = new QChar[len];
	int half =  len/2;
	for(int i = 0; i <= half; i++)
	{
	    ch[len-1-i] = str->s[from+i];
	    ch[i] = str->s[from+len-1-i];
	    if(ch[i].mirrored())
		ch[i] = ch[i].mirroredChar();
	    if(ch[len-i].mirrored())
		ch[len-i] = ch[len-i].mirroredChar();
	}
    }
    else
	ch = str->s+from;

    // ### margins and RTL
    if(from == 0 && m_parent->isInline())
	x += paddingLeft() + borderLeft();

#ifdef DEBUG_LAYOUT
    QConstString cstr(ch, len);
    printf("setting slave text to '%s' len=%d width=%d at (%d/%d)\n", (const char *)cstr.string().utf8(), len, width, x, y);
#endif

    TextSlave *s = new TextSlave(x, y, ch, len,
				 fm->ascent() + fm->descent(), fm->ascent(), width, deleteChar);

    if(!m_first)
	m_first = m_last = s;
    else
    {
	m_last->setNext(s);
	m_last = s;
    }
}

unsigned int RenderText::width( int from, int len) const
{
    QConstString s(str->s+from, len);
    int w = fm->width(s.string());
    // ### add margins and support for RTL

    if(m_parent->isInline())
    {
	if(from == 0)
	    w += borderLeft() + paddingLeft();
	if(from + len == str->l)
	    w += borderRight() + paddingRight();
    }

    //printf("RenderText::width(%d, %d) = %d\n", from, len, w);
    return w;
}

