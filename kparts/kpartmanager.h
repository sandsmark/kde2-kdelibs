
#ifndef __kpartmanager_h__
#define __kpartmanager_h__

#include <qobject.h>

class KXMLGUIBuilder;

class KPart;

class KPartManager : public QObject
{
  Q_OBJECT
public:
  // The partmanager know about the builder (interface) so that
  // when the active part changes, the builder slot (createGUI) gets
  // automatically called. (That's not docu, it's internal stuff !)
  KPartManager( KXMLGUIBuilder * builder );
  virtual ~KPartManager();
  
  virtual bool eventFilter( QObject *obj, QEvent *ev );
  
  virtual void addPart( KPart *part );
  virtual void removePart( KPart *part );

  KPart *activePart() { return m_activePart; }

signals:
  void activePartChanged( KPart *newPart, KPart *oldPart );

protected slots:
  void slotObjectDestroyed();
  
private:
  KPart * findPartFromWidget( QWidget * widget );
  KPart *m_activePart;
  QList<KPart> m_parts;
};

#endif

