/****************************************************************************
    Implementation of QXEmbed class
    
   Copyright (C) 1999-2000 Troll Tech AS

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
*****************************************************************************/

#include "qxembed.h"
#include <qapplication.h>
#include <qlist.h>
#include <qptrdict.h>
#include <qguardedptr.h>
#include <qfocusdata.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>

#ifndef XK_ISO_Left_Tab
#define	XK_ISO_Left_Tab					0xFE20
#endif
const int XFocusOut = FocusOut;
const int XFocusIn = FocusIn;
#undef FocusOut
#undef FocusIn

// defined in qapplication_x11.cpp
extern Atom qt_wm_protocols;
extern Atom qt_wm_delete_window;
extern Time qt_x_time;

static Atom xembed = 0;

// xembed messages
#define XEMBED_EMBEDDED_NOTIFY 	0
#define XEMBED_PROCESS_NEXT_EVENT  	1
#define XEMBED_WINDOW_ACTIVATE  	2
#define XEMBED_WINDOW_DEACTIVATE  	3
#define XEMBED_REQUEST_FOCUS	 	4
#define XEMBED_FOCUS_IN 	 	5
#define XEMBED_FOCUS_OUT  		6
#define XEMBED_FOCUS_NEXT 		7
#define XEMBED_FOCUS_PREV 		8

// xembed details

// XEMBED_FOCUS_IN:
#define XEMBED_FOCUS_CURRENT 	0
#define XEMBED_FOCUS_FIRST 		1
#define XEMBED_FOCUS_LAST		2



class QXEmbedData
{
public:
    QXEmbedData(){};
    ~QXEmbedData(){};
};

class QXEmbedAppFilter : public QObject
{
public:
    QXEmbedAppFilter(){ qApp->installEventFilter( this ); }
    ~QXEmbedAppFilter(){};
    bool eventFilter( QObject *, QEvent * );
};


static QXEmbedAppFilter* filter = 0;
static QPtrDict<QGuardedPtr<QWidget> > *focusMap = 0;

static XKeyEvent last_key_event;
static bool tabForward = TRUE;
static bool process_next_event = FALSE;


class QPublicWidget : public QWidget
{
public:
    QTLWExtra* topData() { return QWidget::topData(); };
    QFocusData *focusData(){ return QWidget::focusData(); };
};

typedef int (*QX11EventFilter) (XEvent*);
extern QX11EventFilter qt_set_x11_event_filter (QX11EventFilter filter);
static QX11EventFilter oldFilter = 0;


static void send_xembed_message( WId window, int message, int detail = 0 )
{
    XEvent ev;
    memset(&ev, 0, sizeof(ev));
    ev.xclient.type = ClientMessage;
    ev.xclient.window = window;
    ev.xclient.message_type = xembed;
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = qt_x_time;
    ev.xclient.data.l[1] = message;
    ev.xclient.data.l[2] = detail;
    XSendEvent(qt_xdisplay(), window, FALSE, NoEventMask, &ev);
}


bool QXEmbedAppFilter::eventFilter( QObject *o, QEvent * e)
{
    switch ( e->type() ) {
    case QEvent::FocusIn:
	if ( qApp->focusWidget() == o &&
	     ((QPublicWidget*)qApp->focusWidget()->topLevelWidget())->topData()->embedded ) {
	    QFocusEvent* fe = (QFocusEvent*) e; 
		
	    if (fe->reason() == QFocusEvent::Mouse ||
		fe->reason() == QFocusEvent::Shortcut ) {
		WId window = ((QPublicWidget*)qApp->focusWidget()->topLevelWidget())->topData()->parentWinId;
		focusMap->remove( qApp->focusWidget()->topLevelWidget() );
		send_xembed_message( window, XEMBED_REQUEST_FOCUS );
	    } else if ( fe->reason() == QFocusEvent::ActiveWindow ) {
		focusMap->remove( qApp->focusWidget()->topLevelWidget() );
		focusMap->insert( qApp->focusWidget()->topLevelWidget(),
				  new QGuardedPtr<QWidget>(qApp->focusWidget()->topLevelWidget()->focusWidget() ) );
		qApp->focusWidget()->clearFocus();
	    }
	}
	break;
    default:
	break;
    }
    return FALSE;
}


static int qxembed_x11_event_filter( XEvent* e)
{
    switch ( e->type ) {
    case KeyPress: {
	int kc = XKeycodeToKeysym(qt_xdisplay(), e->xkey.keycode, 0);
	if ( kc == XK_Tab || kc == XK_ISO_Left_Tab ) {
	    tabForward = (e->xkey.state & ShiftMask) == 0;
	    QWidget* w = QWidget::find( e->xkey.window );
	    if ( w && w->isActiveWindow() && qApp->focusWidget() && 
		 qApp->focusWidget()->topLevelWidget() == w->topLevelWidget() && 
		 ((QPublicWidget*)w->topLevelWidget())->topData()->embedded ) {
		WId window = ((QPublicWidget*)w->topLevelWidget())->topData()->parentWinId;
		QFocusData *fd = ((QPublicWidget*)w)->focusData();
		while ( fd->next() != w->topLevelWidget() )
		    ;
		QWidget* first = fd->next();
		QWidget* last = fd->prev(); last = fd->prev();
		if ( !tabForward && fd->focusWidget() == first ) {
		    send_xembed_message( window, XEMBED_FOCUS_PREV );
		    return TRUE;
		} else if ( tabForward && fd->focusWidget() == last ) {
		    send_xembed_message( window, XEMBED_FOCUS_NEXT );
		    return TRUE;
		}
	    }
	}
    }
    // FALL THROUGH
    case KeyRelease: {
	last_key_event = e->xkey;
	if ( process_next_event ) {
	    process_next_event = FALSE;
	} else {
	    QWidget* w = QWidget::find( e->xkey.window );
	    if ( w && ((QPublicWidget*)w->topLevelWidget())->topData()->embedded ) {
		e->xkey.window = ((QPublicWidget*)w->topLevelWidget())->topData()->parentWinId;
		XSendEvent(qt_xdisplay(), e->xkey.window, FALSE, NoEventMask, e);
		return FALSE;
	    }
	}
	break;
    }
    case ClientMessage:
	if ( e->xclient.message_type == xembed ) {
	    Time msgtime = (Time) e->xclient.data.l[0];
	    int message = e->xclient.data.l[1];
	    int detail = e->xclient.data.l[2];
	    if ( msgtime > qt_x_time )
		qt_x_time = msgtime;
	    QWidget* w = QWidget::find( e->xclient.window );
	    if ( !w )
		break;
	    switch ( message) {
	    case XEMBED_EMBEDDED_NOTIFY:
		((QPublicWidget*)w->topLevelWidget())->topData()->embedded = 1;
		break;
	    case XEMBED_PROCESS_NEXT_EVENT:
		process_next_event = TRUE;
		break;
	    case XEMBED_WINDOW_ACTIVATE: {
		// fake focus in
		XEvent ev;
		memset(&ev, 0, sizeof(ev));
		ev.xfocus.display = qt_xdisplay();
		ev.xfocus.type = XFocusIn;
		ev.xfocus.window = w->topLevelWidget()->winId();
		ev.xfocus.mode = NotifyNormal;
		ev.xfocus.detail = NotifyAncestor;
		qApp->x11ProcessEvent( &ev );
	    }
	    break;
	    case XEMBED_WINDOW_DEACTIVATE: {
		// fake focus out
		XEvent ev;
		memset(&ev, 0, sizeof(ev));
		ev.xfocus.display = qt_xdisplay();
		ev.xfocus.type = XFocusOut;
		ev.xfocus.window = w->topLevelWidget()->winId();
		ev.xfocus.mode = NotifyNormal;
		ev.xfocus.detail = NotifyAncestor;
		qApp->x11ProcessEvent( &ev );
	    }
	    break;
	    case XEMBED_FOCUS_IN: 
		{
		    QWidget* focusCurrent = 0;
		    QGuardedPtr<QWidget>* fw = focusMap->find( w->topLevelWidget() );
		    if ( fw ) {
			focusCurrent = *fw;
			focusMap->remove( w->topLevelWidget() );
		    }
		    switch ( detail ) {
		    case XEMBED_FOCUS_CURRENT:
			if ( focusCurrent )
			    focusCurrent->setFocus();
			else if ( !w->topLevelWidget()->focusWidget() )
			    w->topLevelWidget()->setFocus();
			break;
		    case XEMBED_FOCUS_FIRST: 
			{
			    QFocusData *fd = ((QPublicWidget*)w)->focusData();
			    while ( fd->next() != w->topLevelWidget() )
				;
			    QWidget* fw = fd->next();
			    QFocusEvent::setReason( QFocusEvent::Tab );
			    if ( w )
				fw->setFocus();
			    else
				w->topLevelWidget()->setFocus();
			    QFocusEvent::resetReason();
			}
			break;
		    case XEMBED_FOCUS_LAST: 
			{
			    QFocusData *fd = ((QPublicWidget*)w)->focusData();
			    while ( fd->next() != w->topLevelWidget() )
				;
			    QWidget* fw = fd->prev();
			    QFocusEvent::setReason( QFocusEvent::Tab );
			    if ( w )
				fw->setFocus();
			    else
				w->topLevelWidget()->setFocus();
			    QFocusEvent::resetReason();
			}
			break;
		    default:
			break;
		    }
		}
		break;
	    case XEMBED_FOCUS_OUT:
		if ( w->topLevelWidget()->focusWidget() ) {
		    focusMap->insert( w->topLevelWidget(),
				      new QGuardedPtr<QWidget>(w->topLevelWidget()->focusWidget() ) );
		    w->topLevelWidget()->focusWidget()->clearFocus();
		}
	    break;
	    default:
		break;
	    }
	}
	break;
    default:
	break;
    }

    if ( oldFilter )
	return oldFilter( e );
    return FALSE;
}

/*!  
  Initializes the xembed system. 
  
  This function is called automatically when using
  embedClientIntoWindow() or creating an instance of QXEmbed You may
  have to call it manually for a client when using embedder-side
  embedding, though.
 */
void QXEmbed::initialize()
{
    static bool is_initialized = FALSE;
    if ( is_initialized )
	return;
    xembed = XInternAtom( qt_xdisplay(), "_XEMBED", FALSE );
    oldFilter = qt_set_x11_event_filter( qxembed_x11_event_filter );

    focusMap = new QPtrDict<QGuardedPtr<QWidget> >;
    focusMap->setAutoDelete( TRUE );
    
    filter = new QXEmbedAppFilter;

    is_initialized = TRUE;
}


/*!
  \class QXEmbed qxembed.h

  \brief The QXEmbed widget is a graphical socket that can embed an
  external X-Window.

  \extension XEmbed

  A QXEmbed widget serves as an embedder that can manage one single
  embedded X-window. These so-called client windows can be arbitrary
  QWidgets.

  There are two different ways of using QXembed, from the embedder's
  or from the client's side.  When using it from the embedder's side,
  you already know the window identifier of the window that should be
  embedded. Simply call embed() with this identifier as parameter.

  Embedding from the client's side requires that the client knows the
  window identifier of the respective embedder widget. Use either
  embedClientIntoWindow() or the high-level wrapper
  processClientCmdline().

  If a window has been embedded succesfully, embeddedWinId() returns
  its id.

  Reimplement the change handler windowChanged() to catch embedding or
  the destruction of embedded windows. In the latter case, the
  embedder also emits a signal embeddedWindowDestroyed() for
  convenience.

*/

/*!
  Constructs a xembed widget.

  The \e parent, \e name and \e f arguments are passed to the QFrame
  constructor.
 */
QXEmbed::QXEmbed(QWidget *parent, const char *name, WFlags f)
  : QWidget(parent, name, f)
{
    initialize();
    window = 0;
    setFocusPolicy(StrongFocus);
    setKeyCompression( FALSE );

    //trick to create extraData();
    (void) topData();

    // we are interested in SubstructureNotify
    XSelectInput(qt_xdisplay(), winId(),
  		 KeyPressMask | KeyReleaseMask |
  		 ButtonPressMask | ButtonReleaseMask |
  		 KeymapStateMask |
   		 ButtonMotionMask |
   		 PointerMotionMask | // may need this, too
  		 EnterWindowMask | LeaveWindowMask |
  		 FocusChangeMask |
  		 ExposureMask |
		 StructureNotifyMask |
		 SubstructureRedirectMask |
		 SubstructureNotifyMask
  		 );

    topLevelWidget()->installEventFilter( this );
    qApp->installEventFilter( this );

    setAcceptDrops( TRUE );
}

/*!
  Destructor. Cleans up the focus if necessary.
 */
QXEmbed::~QXEmbed()
{

    if ( window != 0 ) {
 	XUnmapWindow(qt_xdisplay(), window );
 	XReparentWindow(qt_xdisplay(), window, qt_xrootwin(), 0, 0);

 	XEvent ev;
 	memset(&ev, 0, sizeof(ev));
 	ev.xclient.type = ClientMessage;
 	ev.xclient.window = window;
 	ev.xclient.message_type = qt_wm_protocols;
 	ev.xclient.format = 32;
 	ev.xclient.data.s[0] = qt_wm_delete_window;
 	XSendEvent(qt_xdisplay(), window, FALSE, NoEventMask, &ev);
     }
    window = 0;
}


/*!\reimp
 */
void QXEmbed::resizeEvent(QResizeEvent*)
{
    if (window != 0)
	XResizeWindow(qt_xdisplay(), window, width(), height());
}

/*!\reimp
 */
void QXEmbed::showEvent(QShowEvent*)
{
    if (window != 0)
	XMapRaised(qt_xdisplay(), window);

}


/*!\reimp
 */
bool QXEmbed::eventFilter( QObject *o, QEvent * e)
{

    switch ( e->type() ) {
    case QEvent::WindowActivate:
	if ( o == topLevelWidget() )
	    send_xembed_message( window, XEMBED_WINDOW_ACTIVATE );
	break;
    case QEvent::WindowDeactivate:
	if ( o == topLevelWidget() )
	    send_xembed_message( window, XEMBED_WINDOW_DEACTIVATE );
	break;
    default:
	break;
    }
    return FALSE;
}


bool  QXEmbed::event( QEvent * e)
{
    return QWidget::event( e );
}

/*!\reimp
 */
void QXEmbed::keyPressEvent( QKeyEvent *)
{
    if (!window)
	return;
    send_xembed_message( window, XEMBED_PROCESS_NEXT_EVENT );
    last_key_event.window = window;
    XSendEvent(qt_xdisplay(), window, FALSE, NoEventMask, (XEvent*)&last_key_event);

}

/*!\reimp
 */
void QXEmbed::keyReleaseEvent( QKeyEvent *)
{
    if (!window)
	return;
    send_xembed_message( window, XEMBED_PROCESS_NEXT_EVENT );
    last_key_event.window = window;
    XSendEvent(qt_xdisplay(), window, FALSE, NoEventMask, (XEvent*)&last_key_event);
}

/*!\reimp
 */
void QXEmbed::focusInEvent( QFocusEvent * e ){
    if (!window)
	return;
    int detail = XEMBED_FOCUS_CURRENT;
    if ( e->reason() == QFocusEvent::Tab )
	detail = tabForward?XEMBED_FOCUS_FIRST:XEMBED_FOCUS_LAST;
    send_xembed_message( window, XEMBED_FOCUS_IN, detail);
}

/*!\reimp
 */
void QXEmbed::focusOutEvent( QFocusEvent * ){
    if (!window)
	return;
    send_xembed_message( window, XEMBED_FOCUS_OUT );
}


/*!

  Embeds the window with the identifier \a w into this xembed widget.

  This function is useful if the server knows about the client window
  that should be embedded.  Often it is vice versa: the client knows
  about its target embedder. In that case, it is not necessary to call
  embed(). Instead, the client will call the static function
  embedClientIntoWindow().

  \sa embeddedWinId()
 */
void QXEmbed::embed(WId w)
{
    if (!w)
	return;

    bool has_window =  w == window;
    window = w;
    if ( !has_window )
	XReparentWindow(qt_xdisplay(), w, winId(), 0, 0);
    QApplication::syncX();
    XResizeWindow(qt_xdisplay(), w, width(), height());
    XMapRaised(qt_xdisplay(), window);
    XAddToSaveSet( qt_xdisplay(), w );
    extraData()->xDndProxy = w;

    if ( parent() ) {
	QEvent * layoutHint = new QEvent( QEvent::LayoutHint );
	QApplication::postEvent( parent(), layoutHint );
    }
    windowChanged( window );
    send_xembed_message( window, XEMBED_EMBEDDED_NOTIFY );
    send_xembed_message( window, this == qApp->focusWidget()?XEMBED_FOCUS_IN :XEMBED_FOCUS_OUT);
}


/*!
  Returns the window identifier of the embedded window, or 0 if no
  window is embedded yet.
 */
WId QXEmbed::embeddedWinId() const
{
    return window;
}


/*!\reimp
 */
bool QXEmbed::focusNextPrevChild( bool next )
{
    if ( window )
	return FALSE;
    else
	return QWidget::focusNextPrevChild( next );
}


/*!\reimp
 */
bool QXEmbed::x11Event( XEvent* e)
{
    switch ( e->type ) {
    case DestroyNotify:
	if ( e->xdestroywindow.window == window ) {
	    window = 0;
	    windowChanged( window );
	    emit embeddedWindowDestroyed();
	}
	break;
    case ReparentNotify:
	if ( window && e->xreparent.window == window &&
	     e->xreparent.parent != winId() ) {
	    // we lost the window
	    window = 0;
	    windowChanged( window );
	} else if ( e->xreparent.parent == winId() ){
	    // we got a window
	    window = e->xreparent.window;
	    embed( window );
	}
	break;
    case MapRequest:
	if ( window && e->xmaprequest.window == window )
	    XMapRaised(qt_xdisplay(), window );
	break;
    case ClientMessage:
	if ( e->xclient.format == 32 && e->xclient.message_type == xembed ) {
	    int message = e->xclient.data.l[1];
// 	    int detail = e->xclient.data.l[2];
	    switch ( message ) {
	    case XEMBED_FOCUS_NEXT:
		QWidget::focusNextPrevChild( TRUE );
		break;
	    case XEMBED_FOCUS_PREV:
		QWidget::focusNextPrevChild( FALSE );
		break;
	    case XEMBED_REQUEST_FOCUS:
		setFocus();
		break;
	    default:
		break;
	    }
	}
    default:
	break;
    }
    return FALSE;
}


/*!
  A change handler that indicates that the embedded window has been
  changed.  The window handle can also be retrieved with
  embeddedWinId().
 */
void QXEmbed::windowChanged( WId )
{
}


/*!
  A utility function for clients that embed theirselves. The widget \a
  client will be embedded in the window that is passed as
  \c -embed command line argument.

  The function returns TRUE on sucess or FALSE if no such command line
  parameter is specified.

  \sa embedClientIntoWindow()
 */
bool QXEmbed::processClientCmdline( QWidget* client, int& argc, char ** argv )
{
    int myargc = argc;
    WId window = 0;
    int i, j;

    j = 1;
    for ( i=1; i<myargc; i++ ) {
	if ( argv[i] && *argv[i] != '-' ) {
	    argv[j++] = argv[i];
	    continue;
	}
	QCString arg = argv[i];
	if ( strcmp(arg,"-embed") == 0 && i < myargc-1 ) {
	    QCString s = argv[++i];
	    window = s.toInt();
	} else
	    argv[j++] = argv[i];
    }
    argc = j;

    if ( window != 0 ) {
	embedClientIntoWindow( client, window );
	return TRUE;
    }

    return FALSE;
}


/*!
  A  function for clients that embed themselves. The widget \a
  client will be embedded in the window \a window. The application has
  to ensure that \a window is the handle of the window identifier of
  an QXEmbed widget.

  \sa processClientCmdline()
 */
void QXEmbed::embedClientIntoWindow(QWidget* client, WId window)
{
    initialize();
    XReparentWindow(qt_xdisplay(), client->winId(), window, 0, 0);
    ((QXEmbed*)client)->topData()->embedded = TRUE;
    ((QXEmbed*)client)->topData()->parentWinId = window;
    client->show();
}



/*!
  Specifies that this widget can use additional space, and that it can
  survive on less than sizeHint().
*/

QSizePolicy QXEmbed::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}


/*!
  Returns a size sufficient for the embedded window
*/
QSize QXEmbed::sizeHint() const
{
    return minimumSizeHint();
}

/*!
  \fn void QXEmbed::embeddedWindowDestroyed()

  This signal is emitted when the embedded window has been destroyed.

  \sa embeddedWinId()
*/

/*!
  Returns a size sufficient for one character, and scroll bars.
*/

QSize QXEmbed::minimumSizeHint() const
{
    int minw = 0;
    int minh = 0;
    if ( window ) {
	XSizeHints size;
	long msize;
	if (XGetWMNormalHints(qt_xdisplay(), window, &size, &msize)
	    && ( size.flags & PMinSize) ) {
	    minw = size.min_width;
	    minh = size.min_height;
	}
    }

    return QSize( minw, minh );
}


// for KDE
#include "qxembed.moc"
