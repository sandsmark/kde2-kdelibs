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
#ifndef _KXMLGUICLIENT_H
#define _KXMLGUICLIENT_H

#include <qdom.h>
#include <qlist.h>
#include <qmap.h>

class KAction;
class KActionCollection;
class KInstance;
class KXMLGUIClientPrivate;
class KXMLGUIFactory;
class KXMLGUIBuilder;

class KXMLGUIClient
{
public:
  KXMLGUIClient();
  KXMLGUIClient( KXMLGUIClient *parent );
  virtual ~KXMLGUIClient();

  KAction* action( const char* name );
  virtual KAction *action( const QDomElement &element );
  virtual KActionCollection* actionCollection() const;

  /**
   * @return The instance (@ref KInstance) for this part.
   */
  virtual KInstance *instance() const;

  /**
   * @return The parsed XML in a @ref QDomDocument, set by @ref
   * setXMLFile() or @ref setXML()
   */
  virtual QDomDocument document() const;

  /**
   * This will return the XML file as set by @ref setXMLFile.  If
   * @ref setXML is used directly, then this will return NULL.
   *
   * The filename that this returns is obvious for components as each
   * component has exactly one XML file.  In non-components, however,
   * there are usually two: the global file and the local file.  This
   * function doesn't really care about that, though.  It will always
   * return the last XML file set.  This, in almost all cases, will
   * be the local XML file.
   *
   * @return The name of the XML file or QString::null
   */
  virtual QString xmlFile() const;

  /**
   * default implementation, storing the given data in an internal
   * map. Called from KKXMLGUIFactory when removing containers which
   * were owned by the servant.
   */
  virtual void storeContainerStateBuffer( const QString &key, const QByteArray &data );

  /**
   * default implementation, returning a previously via
   * @ref storeContainerStateBuffer saved data. Called from
   * KKXMLGUIFactory when creating a new container.
   */
  virtual QByteArray takeContainerStateBuffer( const QString &key );

  /**
   * <<INTERNAL>>
   */
  void setContainerStates( const QMap<QString,QByteArray> &states );
  /**
   * <<INTERNAL>>
   */
  QMap<QString,QByteArray> containerStates() const;

  void setFactory( KXMLGUIFactory *factory );
  KXMLGUIFactory *factory() const;

  KXMLGUIClient *parentClient() const;

  void insertChildClient( KXMLGUIClient *child );
  void removeChildClient( KXMLGUIClient *child );
  const QList<KXMLGUIClient> *childClients();

  void setClientBuilder( KXMLGUIBuilder *builder );
  KXMLGUIBuilder *clientBuilder() const;

protected:
  /**
   * Set the instance (@ref KInstance) for this part.
   *
   * Call this first in the inherited class constructor.
   * (At least before @ref setXMLFile().)
   */
  virtual void setInstance( KInstance *instance );

  /**
   * Set the name of the rc file containing the XML for the part.
   *
   * Call this in the Part-inherited class constructor.
   *
   * @param file Either an absolute path for the file, or simply the
   *             filename, which will then be assumed to be installed
   *             in the "data" resource, under a directory named like
   *             the instance.
   **/
  virtual void setXMLFile( const QString& file, bool merge = false );

  /**
   * Set the XML for the part.
   *
   * Call this in the Part-inherited class constructor if you
   *  don't call @ref setXMLFile().
   **/
  virtual void setXML( const QString &document, bool merge = false );

  /**
   * This function will attempt to give up some memory after the GUI
   * is built.  It should never be used in apps where the GUI may be
   * rebuilt at some later time (components, for instance).
   */
  virtual void conserveMemory();

private:
  bool mergeXML( QDomElement &base, const QDomElement &additive,
                 KActionCollection *actionCollection );
  QDomElement findMatchingElement( const QDomElement &base,
                                   const QDomElement &additive );

  KXMLGUIClientPrivate *d;
};

#endif
