#include "kregexpeditor.h"
#include <ktrader.h>
#include <kservice.h>
#include <klibloader.h>
#include <kdebug.h>

KRegExpEditor::KRegExpEditor( QWidget* parent, const char* name )
  : QWidget( parent, name )
{
}


KRegExpEditor* KRegExpEditor::createEditor( QWidget* parent, const char* name )
{
  KTrader::OfferList offers = KTrader::self()->query( "KRegExpEditor/KRegExpEditor" );
  if ( offers.count() < 1 ) {
    qDebug("Couldn't find library");
    return 0;
  }
  
  KService::Ptr service = *offers.begin();
  
  KLibFactory *factory = KLibLoader::self()->factory( service->library().latin1() );
  if ( ! factory ) {
    kdWarning() << "Couldn't find a factory" << endl;
    return 0;
  }
  
  QObject *obj = factory->create( parent, name, "KRegExpEditor" );
  return dynamic_cast<KRegExpEditor *>( obj );
}

#include "kregexpeditor.moc"
