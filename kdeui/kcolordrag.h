/* This file is part of the KDE libraries
   Copyright (C) 1999 Steffen Hansen (hansen@kde.org)

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
/*
 * $Id$
 *
 * $Log$
 * Revision 1.2  1999/12/13 18:17:36  kulow
 * removed ; after Q_OBJECT
 *
 * Revision 1.1  1999/05/06 02:46:13  steffen
 * Drag&drop for colors. Qt drag&drop is really easy to use. We should have stuff like this all over KDE.
 *
 *
 */
#ifndef _KCOLORDRAG_H
#define _KCOLORDRAG_H

#include <qdragobject.h>
#include <qcolor.h>

/**
 * KColorDrag for XDnd'ing objects of type application/x-color.
 *
 * See the Qt drag'n'drop documentation.
 */
class KColorDrag : public QStoredDrag {
     Q_OBJECT

public:
     KColorDrag( const QColor&, QWidget *dragsource = 0, const char *name = 0);
     KColorDrag( QWidget *dragsource = 0, const char *name = 0);
     virtual ~KColorDrag() {};
     void setColor( const QColor&);

     static bool canDecode( QMimeSource *);
     static bool decode( QMimeSource *, QColor&);
     
     /**
      * Convenience function for making a dragobject with an
      * associated pixmap
      */
     static KColorDrag* makeDrag( const QColor&,QWidget *dragsource);
protected:
     QColor m_color;
};


#endif // _KCOLORDRAG_H
