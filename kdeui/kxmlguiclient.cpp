/* This file is part of the KDE libraries
   Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2000 Kurt Granroth <granroth@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kxmlguiclient.h"
#include "kxmlgui.h"

#include <qdom.h>

#include <kinstance.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kaction.h>

static void dump_xml(const QDomElement& elem)
{
  qDebug("%s", KXMLGUIFactory::elementToXML(elem).latin1());
}

class KXMLGUIClientPrivate
{
public:
  KXMLGUIClientPrivate()
  {
    m_instance = KGlobal::instance();
    m_factory = 0L;
    m_parent = 0L;
    m_builder = 0L;
    m_xmlFile = QString::null;
  }
  ~KXMLGUIClientPrivate()
  {
  }

  KInstance *m_instance;

  QDomDocument m_doc;
  KActionCollection m_actionCollection;
  QMap<QString,QByteArray> m_containerStates;
  KXMLGUIFactory *m_factory;
  KXMLGUIClient *m_parent;
  QList<KXMLGUIClient> m_children;
  KXMLGUIBuilder *m_builder;
  QString m_xmlFile;
};

KXMLGUIClient::KXMLGUIClient()
{
  d = new KXMLGUIClientPrivate;
}

KXMLGUIClient::KXMLGUIClient( KXMLGUIClient *parent )
{
  d = new KXMLGUIClientPrivate;
  d->m_parent = parent;
}

KXMLGUIClient::~KXMLGUIClient()
{
  if ( d->m_parent )
    d->m_parent->removeChildClient( this );
  /*
  QListIterator<KXMLGUIClient> childIt( d->m_children );
  for (; childIt.current(); ++childIt )
    childIt.current()->d->m_parent = 0L;

  d->m_children.setAutoDelete( true );
  d->m_children.clear();
  */
  delete d;
}

KAction *KXMLGUIClient::action( const char *name )
{
  return d->m_actionCollection.action( name );
}

KActionCollection *KXMLGUIClient::actionCollection() const
{
  return &d->m_actionCollection;
}

KAction *KXMLGUIClient::action( const QDomElement &element )
{
  static QString attrName = QString::fromLatin1( "name" );
  return action( element.attribute( attrName ).ascii() );
}

KInstance *KXMLGUIClient::instance() const
{
  return d->m_instance;
}

QDomDocument KXMLGUIClient::document() const
{
  return d->m_doc;
}

QString KXMLGUIClient::xmlFile() const
{
  return d->m_xmlFile;
}

void KXMLGUIClient::setInstance( KInstance *instance )
{
  d->m_instance = instance;
  d->m_actionCollection.setInstance( instance );
}

void KXMLGUIClient::setXMLFile( const QString& _file, bool merge )
{
  // store our xml file name
  if ( !_file.isNull() )
    d->m_xmlFile = _file;

  QString file = _file;
  if ( file[0] != '/' )
  {
    file = locate( "data", QString(instance()->instanceName())+"/"+file );
    if ( file.isEmpty() )
    {
      // this might or might not be an error.  for the time being,
      // let's treat this as if it isn't a problem and the user just
      // wants the global standards file
      setXML( QString::null, true );
      return;
    }
  }

  QString xml = KXMLGUIFactory::readConfigFile( file );
  setXML( xml, merge );
}

void KXMLGUIClient::setXML( const QString &document, bool merge )
{
  if ( merge )
  {
    QDomElement base = d->m_doc.documentElement();
    QDomDocument doc;

    if ( !document.isNull() )
      doc.setContent( document );

    // merge our original (global) xml with our new one
    mergeXML(base, doc.documentElement(), actionCollection());

    // reassign our pointer as mergeXML might have done something
    // strange to it
    base = d->m_doc.documentElement();
    dump_xml(base.toElement());

    // we want some sort of failsafe.. just in case
    if ( base.isNull() )
      d->m_doc.setContent( document );
  }
  else
    d->m_doc.setContent( document );
}

bool KXMLGUIClient::mergeXML( QDomElement &base, const QDomElement &additive, KActionCollection *actionCollection )
{
  static QString tagAction = QString::fromLatin1( "Action" );
  static QString tagMerge = QString::fromLatin1( "Merge" );
  static QString tagSeparator = QString::fromLatin1( "Separator" );
  static QString attrName = QString::fromLatin1( "name" );
  static QString attrAppend = QString::fromLatin1( "append" );
  static QString attrWeakSeparator = QString::fromLatin1( "weakSeparator" );
  static QString tagMergeLocal = QString::fromLatin1( "MergeLocal" );
  static QString tagText = QString::fromLatin1( "text" );
  static QString attrAlreadyVisited = QString::fromLatin1( "alreadyVisited" );
  static QString attrNoMerge = QString::fromLatin1( "noMerge" );

  // there is a possibility that we don't want to merge in the
  // additive.. rather, we might want to *replace* the base with the
  // additive.  this can be for any container.. either at a file wide
  // level or a simple container level.  we look for the 'noMerge'
  // tag, in any event and just replace the old with the new
  if ( additive.attribute(attrNoMerge) == "1" )
  {
    base.parentNode().replaceChild(additive, base);
    return true;
  }

  QString tag;

  QDomElement e = base.firstChild().toElement();
  // iterate over all elements in the container (of the global DOM tree)
  while ( !e.isNull() )
  {
    tag = e.tagName();

    // if there's an action tag in the global tree and the action is
    // not implemented, then we remove the element
    if ( tag == tagAction )
    {
      if ( !actionCollection->action( e.attribute( attrName ).utf8() ) )
      {
        // remove this child as we aren't using it
        QDomElement oldChild = e;
        e = e.nextSibling().toElement();
        base.removeChild( oldChild );
        continue;
      }
    }

    // if there's a separator defined in the global tree, then add an
    // attribute, specifying that this is a "weak" separator
    else if ( tag == tagSeparator )
    {
      e.setAttribute( attrWeakSeparator, (uint)1 );

      // okay, hack time. if the last item was a weak separator OR
      // this is the first item in a container, then we nuke the
      // current one
      QDomElement prev = e.previousSibling().toElement();
      if ( prev.isNull() || ( prev.tagName() == tagSeparator && !prev.attribute( attrWeakSeparator ).isNull() ) )
      {
        // the previous element was a weak separator or didn't exist
        QDomElement oldChild = e;
        e = e.nextSibling().toElement();
        base.removeChild( oldChild );
        continue;
      }
    }

    // the MergeLocal tag lets us specify where non-standard elements
    // of the local tree shall be merged in.  After inserting the
    // elements we delete this element
    else if ( tag == tagMergeLocal )
    {
      QDomElement currElement = e;

      // switch our iterator "e" to the next sibling, so that we don't
      // process the local tree's inserted items!
      e = e.nextSibling().toElement();

      QDomElement it = additive.firstChild().toElement();
      while ( !it.isNull() )
      {
        QDomElement newChild = it;

        it = it.nextSibling().toElement();

        if ( newChild.tagName() == tagText )
          continue;

        if ( newChild.attribute( attrAlreadyVisited ) == "1" )
          continue;

        QString itAppend( newChild.attribute( attrAppend ) );
        QString elemName( currElement.attribute( attrName ) );

        if ( ( itAppend.isNull() && elemName.isNull() ) ||
             ( itAppend == elemName ) )
        {
          // first, see if this new element matches a standard one in
          // the global file.  if it does, then we skip it as it will
          // be merged in, later
          QDomElement matchingElement = findMatchingElement( newChild, base );
          if ( matchingElement.isNull() )
            base.insertBefore( newChild, currElement );
        }
      }

      base.removeChild( currElement );
      continue;
    }

    // in this last case we check for a separator tag and, if not, we
    // can be sure that its a container --> proceed with child nodes
    // recursively and delete the just proceeded container item in
    // case its empty (if the recursive call returns true)
    else if ( tag != tagMerge )
    {
      // handle the text tag
      if ( tag == tagText )
      {
        e = e.nextSibling().toElement();
        continue;
      }

      QDomElement matchingElement = findMatchingElement( e, additive );

      QDomElement currElement = e;
      e = e.nextSibling().toElement();

      if ( !matchingElement.isNull() )
      {
        matchingElement.setAttribute( attrAlreadyVisited, (uint)1 );

        if ( mergeXML( currElement, matchingElement, actionCollection ) )
          base.removeChild( currElement );
        continue;
      }
      else
      {
        // this is an important case here! We reach this point if the
        // "local" tree does not contain a container definition for
        // this container. However we have to call mergeXML recursively
        // and make it check if there are actions implemented for this
        // container. *If* none, then we can remove this container now
        if ( mergeXML( currElement, QDomElement(), actionCollection ) )
          base.removeChild( currElement );
        continue;
      }
    }

    //I think this can be removed ;-)
    e = e.nextSibling().toElement();
  }

  //here we append all child elements which were not inserted
  //previously via the LocalMerge tag
  e = additive.firstChild().toElement();
  while ( !e.isNull() )
  {
    QDomElement matchingElement = findMatchingElement( e, base );

    if ( matchingElement.isNull() )
    {
      QDomElement newChild = e;
      e = e.nextSibling().toElement();
      base.appendChild( newChild );
    }
    else
      e = e.nextSibling().toElement();
  }

  // do one quick check to make sure that the last element was not
  // a weak separator
  QDomElement last = base.lastChild().toElement();
  if ( (last.tagName() == tagSeparator) && (!last.attribute( attrWeakSeparator ).isNull()) )
  {
    base.removeChild( base.lastChild() );
  }

  // now we check if we are empty (in which case we return "true", to
  // indicate the caller that it can delete "us" (the base element
  // argument of "this" call)
  bool deleteMe = true;
  e = base.firstChild().toElement();
  for ( ; !e.isNull(); e = e.nextSibling().toElement() )
  {
    tag = e.tagName();

    if ( tag == tagAction )
    {
      // if base contains an implemented action, then we must not get
      // deleted (note that the actionCollection contains both,
      // "global" and "local" actions
      if ( actionCollection->action( e.attribute( attrName ).utf8() ) )
      {
        deleteMe = false;
        break;
      }
    }
    else if ( tag == tagSeparator )
    {
      // if we have a separator which has *not* the weak attribute
      // set, then it must be owned by the "local" tree in which case
      // we must not get deleted either
      QString weakAttr = e.attribute( attrWeakSeparator );
      if ( weakAttr.isEmpty() || weakAttr.toInt() != 1 )
      {
        deleteMe = false;
        break;
      }
    }

    // in case of a merge tag we have unlimited lives, too ;-)
    else if ( tag == tagMerge )
    {
      deleteMe = false;
      break;
    }

    // a text tag is NOT enough to spare this container
    else if ( tag == tagText )
    {
      continue;
    }

    // what's left are non-empty containers! *don't* delete us in this
    // case (at this position we can be *sure* that the container is
    // *not* empty, as the recursive call for it was in the first loop
    // which deleted the element in case the call returned "true"
    else
    {
      deleteMe = false;
      break;
    }
  }

  return deleteMe;
}

QDomElement KXMLGUIClient::findMatchingElement( const QDomElement &base, const QDomElement &additive )
{
  static QString tagAction = QString::fromLatin1( "Action" );
  static QString tagMergeLocal = QString::fromLatin1( "MergeLocal" );
  static QString attrName = QString::fromLatin1( "name" );

  QDomElement e = additive.firstChild().toElement();
  for ( ; !e.isNull(); e = e.nextSibling().toElement() )
  {
    // skip all action and merge tags as we will never use them
    if ( ( e.tagName() == tagAction ) || ( e.tagName() == tagMergeLocal ) )
    {
      continue;
    }

    // now see if our tags are equivalent
    if ( ( e.tagName() == base.tagName() ) &&
         ( e.attribute( attrName ) == base.attribute( attrName ) ) )
    {
        return e;
    }
  }

  // nope, return a (now) null element
  return e;
}

void KXMLGUIClient::conserveMemory()
{
  d->m_doc = QDomDocument();
}

void KXMLGUIClient::storeContainerStateBuffer( const QString &key, const QByteArray &data )
{
  if ( !key.isEmpty() )
    d->m_containerStates.replace( key, data );
}

QByteArray KXMLGUIClient::takeContainerStateBuffer( const QString &key )
{
  QByteArray res;

  if ( key.isEmpty() )
    return res;

  QMap<QString,QByteArray>::Iterator it = d->m_containerStates.find( key );
  if ( it != d->m_containerStates.end() )
  {
    res = it.data();
    d->m_containerStates.remove( it );
  }

  return res;
}

void KXMLGUIClient::setContainerStates( const QMap<QString,QByteArray> &states )
{
  d->m_containerStates = states;
}

QMap<QString,QByteArray> KXMLGUIClient::containerStates() const
{
  return d->m_containerStates;
}

void KXMLGUIClient::setFactory( KXMLGUIFactory *factory )
{
  d->m_factory = factory;
}

KXMLGUIFactory *KXMLGUIClient::factory() const
{
  return d->m_factory;
}

KXMLGUIClient *KXMLGUIClient::parentClient() const
{
  return d->m_parent;
}

void KXMLGUIClient::insertChildClient( KXMLGUIClient *child )
{
  if ( child->parentClient() )
    child->parentClient()->removeChildClient( child );

  d->m_children.append( child );
}

void KXMLGUIClient::removeChildClient( KXMLGUIClient *child )
{
  d->m_children.removeRef( child );
}

const QList<KXMLGUIClient> *KXMLGUIClient::childClients()
{
  return &d->m_children;
}

void KXMLGUIClient::setClientBuilder( KXMLGUIBuilder *builder )
{
  d->m_builder = builder;
}

KXMLGUIBuilder *KXMLGUIClient::clientBuilder() const
{
  return d->m_builder;
}
