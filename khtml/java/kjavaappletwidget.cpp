#include "kjavaappletwidget.h"

#include <kwinmodule.h>
#include <kwin.h>
#include <kdebug.h>
#include <klocale.h>

#include <qtimer.h>
#include <qapplication.h>
#include <qlabel.h>

// For future expansion
struct KJavaAppletWidgetPrivate
{
    QLabel* tmplabel;
};

static unsigned int count = 0;

KJavaAppletWidget::KJavaAppletWidget( KJavaAppletContext* context,
                                      QWidget* parent, const char* name )
   : JavaEmbed( parent, name )
{
    applet = new KJavaApplet( this, context );
    init();
}

KJavaAppletWidget::KJavaAppletWidget( KJavaApplet* _applet,
                                      QWidget* parent, const char* name )
   : JavaEmbed( parent, name )
{
    applet = _applet;
    init();
}

KJavaAppletWidget::KJavaAppletWidget( QWidget* parent, const char* name )
   : JavaEmbed( parent, name )
{
    KJavaAppletContext* context = KJavaAppletContext::getDefaultContext();
    applet = new KJavaApplet( this, context );
    init();
}

KJavaAppletWidget::~KJavaAppletWidget()
{
    delete applet;
    delete d;
}

void KJavaAppletWidget::init()
{
    d   = new KJavaAppletWidgetPrivate;
    kwm = new KWinModule( this );

    d->tmplabel = new QLabel( i18n("Loading Applet"), this );
    d->tmplabel->setAlignment( Qt::AlignCenter | Qt::WordBreak );
    d->tmplabel->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );

    uniqueTitle();
    shown = false;
}

void KJavaAppletWidget::create()
{
}

void KJavaAppletWidget::setAppletClass( const QString &clazzName )
{
   applet->setAppletClass( clazzName );
}

QString &KJavaAppletWidget::appletClass()
{
   return applet->appletClass();
}

void KJavaAppletWidget::setAppletName( const QString &name )
{
    applet->setAppletName( name );
}

QString &KJavaAppletWidget::appletName()
{
    return applet->appletName();
}

void KJavaAppletWidget::setJARFile( const QString &jar )
{
   applet->setJARFile( jar );
}

QString &KJavaAppletWidget::jarFile()
{
   return applet->jarFile();
}

void KJavaAppletWidget::setParameter( const QString &name, const QString &value )
{
   applet->setParameter( name, value );
}

QString &KJavaAppletWidget::parameter( const QString &name )
{
    return applet->parameter( name );
}

void KJavaAppletWidget::setBaseURL( const QString &base )
{
   applet->setBaseURL( base );
}

QString &KJavaAppletWidget::baseURL()
{
   return applet->baseURL();
}

void KJavaAppletWidget::setCodeBase( const QString &codeBase )
{
   applet->setCodeBase( codeBase );
}

QString &KJavaAppletWidget::codeBase()
{
   return applet->codeBase();
}

void KJavaAppletWidget::uniqueTitle()
{
   swallowTitle.sprintf( "KJAS Applet - Ticket number %u", count );
   count++;
}

void KJavaAppletWidget::showApplet()
{
    //Now we send applet info to the applet server
    if ( !applet->isCreated() )
        applet->create();

    connect( kwm, SIGNAL( windowAdded( WId ) ),
	         this, SLOT( setWindow( WId ) ) );
   
    kwm->doNotManage(swallowTitle);
    applet->show( swallowTitle );
   
    shown = true;
}

void KJavaAppletWidget::start()
{
    applet->start();
}

void KJavaAppletWidget::stop()
{
    applet->stop();
}

void KJavaAppletWidget::setWindow( WId w )
{
    //make sure that this window has the right name, if so, embed it...
    KWin::Info w_info = KWin::info( w );

    if ( swallowTitle == w_info.name ||
         swallowTitle == w_info.visibleName )
    {
        kdDebug(6100) << "swallowing our window: " << swallowTitle
                      << ", window id = " << w << endl;

        // disconnect from KWM events
        disconnect( kwm, SIGNAL( windowAdded( WId ) ),
                    this, SLOT( setWindow( WId ) ) );

        delete d->tmplabel;
        d->tmplabel = 0;

        embed( w );
    }
}

QSize KJavaAppletWidget::sizeHint()
{
    kdDebug(6100) << "KJavaAppletWidget::sizeHint()" << endl;
    QSize rval = JavaEmbed::sizeHint();

    if( rval.width() == 0 || rval.height() == 0 )
        if( width() != 0 && height() != 0 )
            rval = QSize( width(), height() );

    kdDebug(6100) << "returning: (" << rval.width() << ", " << rval.height() << ")" << endl;

    return rval;
}

void KJavaAppletWidget::resize( int w, int h)
{
    kdDebug(6100) << "KJavaAppletWidget::resize " << w << ", " << h << endl;

    if( d->tmplabel )
    {
        d->tmplabel->resize( w, h );
    }

    JavaEmbed::resize( w, h );
    if( !embedded() )
        applet->setSize( QSize(w, h) );
}

#include "kjavaappletwidget.moc"
