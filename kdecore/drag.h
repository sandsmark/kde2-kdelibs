/* This file is part of the KDE libraries
    Copyright (C) 1997 Torben Weis (weis@kde.org)

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
/* $Id$
 *
 * $Log$
 * Revision 1.30  1999/05/21 19:24:17  dmuell
 * Adding obsolete warnings to KDND* classes (both at compile & run time)
 *
 * Revision 1.29  1999/05/07 15:42:26  kulow
 * making some changes to the code and partly to the API to make it
 * -DQT_NO_ASCII_CAST compatible.
 * The job is quite boring, but triggers some abuses of QString. BTW:
 * I added some TODOs to the code where I was too lazy to continue.
 * Someone should start a grep for TODO in the code on a regular base ;)
 *
 * Revision 1.28  1999/05/03 07:21:27  kulow
 * some more warnings removed. Wouldn't it be cool to compile KDE
 * with -ansi -pedantic -Werror? I put that into my egcs's spec file
 * and I won't take it back ;)
 *
 * Revision 1.27  1999/04/18 09:14:59  kulow
 * taking out config.h from Header files. I don't know if I haven't noticed
 * before, but this is even very dangerous
 *
 * Revision 1.26  1999/04/17 19:15:38  kulow
 * cleaning up kapp.h, so that only needed headers are included. Guess how
 * many files include kapp.h because it includes almost anything they need ;)
 * If you find problems after this, please use
 *
 *   make -k 2>&1 | perl ..../kdesdk/script/fixheaders
 *
 * And if you find a problem, fixheaders doesn't fix, fix fixheaders
 *
 * Revision 1.25  1999/03/01 23:33:05  kulow
 * CVS_SILENT ported to Qt 2.0
 *
 * Revision 1.24.2.1  1999/02/14 02:05:26  granroth
 * Converted a lot of 'const char*' to 'QString'.  This compiles... but
 * it's entirely possible that nothing will run linked to it :-P
 *
 * Revision 1.24  1998/10/22 12:22:37  ettrich
 * grbmll... anyways, now it should compile again.
 *
 * Revision 1.23  1998/10/22 12:18:12  ettrich
 * sorry....
 *
 * Revision 1.22  1998/10/22 11:44:54  ettrich
 * Matthias: simplified and documented the do-not-include-x.h fix
 *
 * Revision 1.21  1998/10/20 18:58:11  ettrich
 * ugly hack to get rid of X11 includes, small fix
 *
 * Revision 1.20  1998/09/01 20:21:10  kulow
 * I renamed all old qt header files to the new versions. I think, this looks
 * nicer (and gives the change in configure a sense :)
 *
 * Revision 1.19  1998/08/23 15:58:30  kulow
 * fixed some more advanced warnings
 *
 * Revision 1.18  1998/08/22 20:02:34  kulow
 * make kdecore have nicer output, when compiled with -Weffc++ :)
 *
 * Revision 1.17  1998/07/16 14:52:32  ssk
 * Removed stub inline implementations for disallowed copy constructor and
 * operator= (not a good idea since it still allows KApp to call them).
 * Possibly BINARY INCOMPATIBLE (I'm not sure about this), but it also fixes
 * a couple of egcs warnings.
 *
 * Removed extra KDNDWidget:: qualification that caused an egcs warning.
 *
 * Revision 1.16  1998/06/16 06:03:14  kalle
 * Implemented copy constructors and assignment operators or disabled them
 *
 * Revision 1.15  1998/05/08 20:45:08  kulow
 * took out the #undef I added
 *
 * Revision 1.14  1998/05/08 16:09:59  kulow
 * undef Color and GrayScale after including X11/X.h. This stupid header breaks
 * everything!
 * How about typedef Window unsigned long instead of X11/X.h?
 *
 * Drag and Drop for KDE
 * Torben Weis <weis@stud.uni-frankfurt.de>
 *
 */

#ifndef DRAG_H
#define DRAG_H

#ifndef _KAPP_H
#warning KDNDWidget, KDNDDropZone & KDNDIcon are obsolete and will disappear soon. Use Qt DnD instead.
#endif

#include <qwidget.h>
#include <qpixmap.h>
#include <qstrlist.h>

/* Usually we drag URLs around. Every type of data must have
  a unique ID so that the receiver can decide wether he wants to
  accept the drop or not.
  */
// ???
#define DndNotDnd       -1
// ???
#define DndUnknown      0
// Raw data
#define DndRawData      1
// Dont use that any more
#define DndFile         2
// Dont use that any more
#define DndFiles        3
// ASCII Text
#define DndText         4
// Dont use that any more
#define DndDir          5
// Dont use that any more
#define DndLink         6
// Dont use that any more
#define DndExe          7
// For Offix internal use only
#define DndEND          8
// An URL
#define DndURL          128

#define Dnd_X_Precision 2
#define Dnd_Y_Precision 2

// we need Window and Atom but do not want to include X.h since it
// #defines way too many constants
typedef unsigned long XID;
typedef unsigned long Atom;
typedef XID Window;

/**
* Icon for KDE Drag 'n Drop. This is the widget that is moved
* around during DND.
*
* @short Icon for KDE Drag 'n Drop
* @author Torben Weis (weis@kde.org)
* @version $Id$
*/
class KDNDIcon : public QWidget
{
  Q_OBJECT
public:

/**
* Creates an Icon with the specified pixmap. _x and _y are the upper
* left corner in global coordinates.
*/
  KDNDIcon( QPixmap &pixmap , int _x, int _y );

  /**
   * Copy constructor
   */
  KDNDIcon( const KDNDIcon& icon );

  /**
   * Assignment operator
   */
  KDNDIcon& operator= ( const KDNDIcon& icon );

/**
* Destructor
*/
  virtual ~KDNDIcon();

protected:
  virtual void paintEvent( QPaintEvent * );
  virtual void resizeEvent( QResizeEvent * );

/**
* The pixmap displayed.
*/
  QPixmap pixmap;
};

/**
* Drop zone for KDE Drag n Drop.
*
* You can create a DropZone for every widget. When the user makes a
* drop over this widget, the KDNDDropZone takes over control.
*
* The KDE drag and drop classes are based on the Offix Drag and Drop
* protocol, but are not compatible with it yet.
*
* Currently used types are: DndText, DndURL.
*
* @short Drop zone for KDE Drag n Drop.
* @author Torben Weis (weis@kde.org)
* @version $Id$
*/
class KDNDDropZone : public QObject
{
  Q_OBJECT

public:
/**
* Create a DropZone for the widget _parent.
*
* @param _parent	The parent of this dropzone, usually the widget
*			for which you wish to receive drop events.
* @param _type		The type of Drop events to accept, eg DndURL.
*/
  KDNDDropZone( QWidget* _parent, int _type );

/**
* Destructor
*/
  virtual ~KDNDDropZone();

/** When a drop occurs, this function is called. _data is the
   dropped data, _size its size, _type for example DndURL and
   _x and _y are the global coordinates of the drop.
   */

/**
* This function is called when a drop event occurs.
*
* @param _data		A pointer to the dropped data.
* @param _size		The length of the data dropped.
* @param _type		The type of the data, eg DndURL means a URL was
*			dropped.
* @param _x,_y		The global coordinates of the drop.
*/
  virtual void drop( char *_data, int _size, int _type, int _x, int _y );

/**
* This function is called when an icon is being dragged over this
* drop zone.
*
* Note that the icon may never have left the drop zone; the user may
* be dragging it around withing this zone and this function would still
* be called.
*
* @param _data		A pointer to the dragged data.
* @param _size		The length of the data dragged.
* @param _type		The type of the data, eg DndURL means a URL is
*			being dragged.
* @param _x,_y		The global coordinates of the drag.
*/
  virtual void enter( char *_data, int _size, int _type, int _x, int _y );

/**
* This function is called when the icon being dragged has left
* this drop zone.
*/
  virtual void leave();

/**
* Decode the dropped data into a list of URLs. This should only be called
* if the dropped data was of type DndURL.
*
* Note that the contents of this list are only valid till the next
* drop event.
*/
  virtual QStrList & getURLList();

/**
* Get dropped data.
*
* @return A pointer to the dropped data.
*/
  virtual QString getData() { return dndData; }

/**
* Get dropped data length.
*
* @return the length of the data that was dropped.
*/
  virtual int getDataSize() { return dndSize; }

/**
* Get drop data type.
*
* @return the type of the data dropped.
*/
  virtual int getDataType() { return dndType; }

/**
* Get the mouse position at which the item was dropped.
*
* @return the X coordinate at which the item was dropped.
* @see #getMouseY
*/
  virtual int getMouseX() { return dndX; }
/**
* Get the mouse position at which the item was dropped.
*
* @return the Y coordinate at which the item was dropped.
* @see #getMouseX
*/
  virtual int getMouseY() { return dndY; }

/**
* The types of dropped data this drop zone will accept.
*
* @return the types of drops accepted.
*/
  virtual int getAcceptType() { return acceptType; }

/**
* Tests whether this data type will be accepted.
*
* @param _type	the data type to be tested.
* @return true if this type will be accepted, false otherwise.
*/
  virtual bool accepts( int _type )
  { return (( acceptType & _type ) == _type); }

/**
* Get the parent widget.
*
* @return the parent widget for which this object is monitoring drops.
*/
  QWidget* getWidget();

signals:

/**
* Emitted when a drop has occurred.
*
* The zone into which the drop has occurred is passed as a parameter.
*/
  void dropAction( KDNDDropZone* );

/**
* Emitted when an icon is dragged into and inside this drop zone.
*/
  void dropEnter( KDNDDropZone* );

/**
* Emitted when an icon is dragged out of this drop zone.
*/
  void dropLeave( KDNDDropZone* );

protected:
/**
* Fills 'urlList' with the URLs in 'dndData'.
* Works only if 'dndType' is DndURL.
*/
  void parseURLList();

/**
* The parent widget being monitored.
*/
  QWidget *widget;

/**
* Last drop data.
* Only valid during a call to 'dropAction'
*/
  char *dndData;
/**
*/
  int dndSize;
/**
*/
  int dndType;
/**
* Drop/drag X position.
*/
  int dndX;
/**
* Drop/drag Y position.
*/
  int dndY;
/**
* Data type mask.
*/
  int acceptType;

/**
* If 'dndType' is URL, then all URLs in 'dndData' are stored here.
*/
  QStrList urlList;

 private:
  KDNDDropZone(const KDNDDropZone&);
  KDNDDropZone& operator=(const KDNDDropZone&);
};

/**
* A widget for drag support.
*
* If you require only drop support you dont need this widget, you just need
* KDndDropZone.
*
* @short A widget for drag support.
* @author Torben Weis (weis@kde.org)
* @version $Id$
*/
class KDNDWidget : public QWidget
{
  Q_OBJECT
public:
/**
* Constructor.
*/
  KDNDWidget( QWidget *_parent=0, const char *_name=0, WFlags f=0 ) :
      QWidget( _parent, _name, f ) , drag(false), dndData(0L),
      dndSize(0), dndType(0), dndOffsetX(0), dndOffsetY(0), dndIcon(0L),
      dndLastWindow(0)
    {}

/**
* Destructor.
*/
  virtual ~KDNDWidget();

/**
* Start a drag.
*
* Call this function when you notice that the user wants to drag something
* around, usually from a dndMouseMoveEvent.
*
* @param _icon	The icon that the user can drag around.
* @param _data	A pointer to the data being dragged. A deep copy is
*		made of this data, so you don't need to maintain its
*		value after you call this function.
* @param _size	The length of the data pointed to by _data.
* @param _type	The type of the data that is being dragged, eg DndURL.
* @param _dx,_dy The difference between the icons upper left corner and
*		the mouse pointer. For example when the user clicks the
*		mouse over the middle of a pixmap, _dx and _dy would be
*		' - pixmap.width() / 2 ' and ' - pixmap.height() / 2 '.
*		This is just provided for look and feel.
*/
    virtual void startDrag( KDNDIcon *_icon, const char *_data, int _size,
    			int _type, int _dx, int _dy );

protected:
/**
* Finds the root window belonging to the global point p.
*/
  virtual Window findRootWindow( QPoint & p );

/**
* This function MUST be called by your implementation if you overload it.
*
* In nearly all cases, you probably mean to call dndMouseMoveEvent().
*
* @see #dndMouseMoveEvent
*/
  virtual void mouseMoveEvent( QMouseEvent * );

/**
* This function MUST be called by your implementation if you overload it.
*
* In nearly all cases, you probably mean to call dndMouseReleaseEvent().
*
* @see #dndMouseReleaseEvent
*/
  virtual void mouseReleaseEvent( QMouseEvent * );

/**
* A root drop occurred.
*
* At the point (_x,_y) the user dropped the icon. If there is now window
* below this point, this function is called.  Usually it emits a XEvent,
* so that every application gets informed about this. This function is
* only called if the drag started in this widget.
*
* See KApplication for details on receiving root drop events.
*/
  virtual void rootDropEvent( int _x, int _y );

/**
* Perform internal housekeeping after a root drop event.
*
* If you must overload rootDropEvent(...), call this function at the
* end to do some clean up.
*/
  virtual void rootDropEvent();

/**
* Called when a drag is ended.
*
* This function is only called if the drag started in this widget.
* Overload it to do your own clean up.
*/
  virtual void dragEndEvent() { }

/**
* Overload this instead of mouseMoveEvent.
* Ususally drags are started in
*    this functions. A implementation might look like this:
*
* <pre>
* void KFileView::dndMouseMoveEvent( QMouseEvent * _mouse )
*    {
*    // 'pressed' is set in mousePressedEvent(...)
*    if ( !pressed )
*	return;
*
*    int x = _mouse->pos().x();
*    int y = _mouse->pos().y();
*
*    if ( abs( x - press_x ) > Dnd_X_Precision || abs( y - press_y ) > Dnd_Y_Precision )
*    {
*        QString data = "Transfer me";
*	QPoint p = mapToGlobal( _mouse->pos() );
*	QPixmap pixmap = typ->getPixmap( filename );
*	int dx = - pixmap.width() / 2;
*	int dy = - pixmap.height() / 2;
*
*	startDrag( new KDNDIcon( pixmap, p.x() + dx, p.y() + dy ), data.data(), data.length(), DndText, dx, dy );
*    }
*    else
*    {
*      Do something different
*    }
* </pre>
*
*    The function is only called if the mouse movement was not part of a
*    drag process.
*/
  virtual void dndMouseMoveEvent( QMouseEvent * ) { }
/**
* Your mouse release event function.
*
* Usually you will only set 'pressed' ( see dndMouseMoveEvent) to false here.
* The function is only called if the release event had nothing to do with
* DND.
*/
  virtual void dndMouseReleaseEvent( QMouseEvent * ) { }

/**
* Are we just doing DND ?
*/
  bool drag;
/**
* The data that is currently dragged.
*/
  char *dndData;
/**
* data size
*/
  int dndSize;
/**
* data type
*/
  int dndType;
/**
* The offset we got from 'startDrag'
*/
  int dndOffsetX;
/**
* The offset we got from 'startDrag'
*/
  int dndOffsetY;
/**
* The icon we are moving around
*/
  KDNDIcon *dndIcon;
/**
* The last window we entered with the mouse pointer.
*/
  Window dndLastWindow;

private:
  // Disallow assignment and copy-construction
  KDNDWidget( const KDNDWidget& );
  KDNDWidget& operator= ( const KDNDWidget& );
};

#endif


