/*
 * This file is part of the KDE Libraries
 * Copyright (C) 1999 Espen Sand (espen@kde.org)
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
 */

#ifndef _KHELPMENU_H_
#define _KHELPMENU_H_

#include <qobject.h>
#include <qstring.h>

class QMessageBox;
class QPopupMenu;
class QWidget;
class KAboutDialog;

class KHelpMenu : public QObject
{
  Q_OBJECT
  
  public:
    /**
     * Constructor.
     * 
     * @param parent The parent of the dialog boxes. The boxes are modeless
     *        and will be centered with respect to the parent.
     * @param aboutAppText User definable string that is used in the 
     *        application specific dialog box. Note: The help menu will
     *        not open this dialog box if you don't define a string.
     */
    KHelpMenu( QWidget *parent=0, const QString &aboutAppText=QString::null );

    /**
     * Destructor
     *
     * Destroys dialogs. NOTE: The menu is not destroyed.
     */
    ~KHelpMenu( void );

    /**
     * Returns a popup menu you can use in the menu bar or where you 
     * need it. 
     *
     * Note 1: You must delete the returned object when you do not
     * need it anymore. It is not destroyed by the destructor.
     *
     * Note 2: This method will only create one instance of the menu. If
     * you call this method twice or more the same menu pointer is returned
     */
    QPopupMenu *menu( void );

  public slots:
    /**
     * Opens the help page for the application. The application name is 
     * used as a key to determine what to display and the system will attempt
     * to open <appName>/index.html.
     */
    void appHelpActivated( void );
  
    /**
     * Opens an application specific dialog box. The dialog box will display
     * the string that was defined in the constructor.
     */
    void aboutApp( void );

    /**
     * Opens the standard "About KDE" dialog box.
     */
    void aboutKDE( void );

  private:
    QPopupMenu   *mMenu;
    QMessageBox  *mAboutApp;
    KAboutDialog *mAboutKDE;

    QString      mAboutAppText;
    QWidget      *mParent;
};


#endif
