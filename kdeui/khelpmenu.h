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
class KBugReport;


/**
 * This class provides the standard KDE help menu with the default "about"
 * dialog boxes and help entry. This class is used in @ref KTMainWindow so
 * normally you don't need to use this class yourself. However, if you
 * need the help menu or any of its dialog boxes in your code that is
 * not subclassed from @ref KTMainWindow you should use this class.
 *
 * The usage is simple:
 *
 * mHelpMenu = new KHelpMenu( this, <someText> );
 * kmenubar->insertItem(i18n("&Help"), mHelpMenu->menu() );
 *
 * or if you just want to open a dialog box:
 *
 * mHelpMenu = new KHelpMenu( this, <someText> );
 * connect( this, SIGNAL(someSignal()), mHelpMenu,SLOT(mHelpMenu->aboutKDE()));
 *
 * IMPORTANT:
 * The first time you use mHelpMenu->menu(), a QPopupMenu object is
 * allocated. Only one object is created by the class so if you call
 * mHelpMenu->menu() twice or more, the same pointer is returned. The class
 * will destroy the popupmenu in the destructor so do not delete this
 * pointer yourself.
 *
 * The KHelpMenu object will be deleted when its parent is destroyed but you
 * can delete it yourself if you want. The code below will always work.
 *
 * MyClass::~MyClass( void )
 * {
 *   delete mHelpMenu;
 * }
 *
 *
 * Using your own "about application" dialog box:
 *
 * The standard "about application" dialog box is quite simple. If you
 * need a dialog box with more functionality you must design that one
 * yourself. When you want to display the dialog you can choose one of
 * two methods. Common for both is that you must make a help menu object 
 * with no text argument If the text is missing the default dialog box 
 * will not be displayed:
 *
 * Example 1 Using @ref showAboutApplication signal (preferred)
 * <pre>
 *
 * void MyClass::myFunc( void )
 * {
 *   ..
 *   KHelpMenu *helpMenu = new KHelpMenu( this );
 *   connect( helpMenu, SIGNAL(showAboutApplication()), 
 *          this, SLOT(myDialogSlot()));
 *   ..
 * }
 *
 * void MyClass::myDialogSlot( void )
 * {
 *   <activate your custom dialog>
 * }
 * </pre>
 *
 *
 * Example 2 Old style - connecting directly to the menu entry.
 * <pre>
 *
 * void MyClass::myFunc( void )
 * {
 *   KHelpMenu *helpMenu = new KHelpMenu( this );
 *   QPopupMenu *help = mHelpMenu->menu();
 *   help->connectItem( KHelpMenu::menuAboutApp, this, SLOT(myDialogSlot()) );
 * }
 *
 * void MyClass::myDialogSlot( void )
 * {
 *   <activate your custom dialog>
 * }
 * </pre>
 *
 * @short Standard KDE help menu with dialog boxes.
 * @author Espen Sand (espen@kde.org)
 * @version $Id$
 */

class KHelpMenu : public QObject
{
  Q_OBJECT

  public:
    enum MenuId
    {
      menuHelpContents = 0,
      menuWhatsThis = 1,
      menuAboutApp = 2,
      menuAboutKDE = 3,
      menuReportBug = 4
    };

    /**
     * Constructor.
     *
     * @param parent The parent of the dialog boxes. The boxes are modeless
     *        and will be centered with respect to the parent.
     * @param aboutAppText User definable string that is used in the
     *        application specific dialog box. Note: The help menu will
     *        not open this dialog box if you don't define a string. See
     *        @ref showAboutApplication for more information.
     * @param showWhatsThis Decides whether a "Whats this" entry will be
     *        added to the dialog.
     * 
     */
    KHelpMenu( QWidget *parent=0, const QString &aboutAppText=QString::null,
	       bool showWhatsThis=true );

    /**
     * Destructor
     *
     * Destroys dialogs and the menu pointer retuned by @ref menu
     */
    ~KHelpMenu( void );

    /**
     * Returns a popup menu you can use in the menu bar or where you
     * need it.
     *
     * Note: This method will only create one instance of the menu. If
     * you call this method twice or more the same pointer is returned
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
     * Activates What's This help for the application.
     */
    void contextHelpActivated( void );

    /**
     * Opens an application specific dialog box. The dialog box will display
     * the string that was defined in the constructor. If that string was 
     * empty the @ref showAboutApplication is emitted instead.
     */
    void aboutApplication( void );

    /**
     * Opens the standard "About KDE" dialog box.
     */
    void aboutKDE( void );

    /**
     * Opens the standard "Report Bugs" dialog box.
     */
    void reportBug( void );
    
  private slots:
    /**
     * Connected to the menu pointer (if created) to detect a delete
     * operation on the pointer. You should not delete the pointer in your
     * code yourself. Let the KHelpMenu destructor do the job.
     */
    void menuDestroyed( void );
    
    /**
     * Connected to the dialogs (about kde and bug report) to detect 
     * when they are closed (becomes hidden).
     */
    void dialogHidden( void );

    /**
     * This slot will delete a dialog (about kde or bug report) if the
     * dialog pointer is not zero and the the dialog is not visible. This
     * slot is activated by a one shot timer started in @ref dialogHidden
     */
    void timerExpired( void );

  signals:
    /**
     * This signal is emitted from the @ref aboutApplication if no 
     * "about application" string has been defined. The standard 
     * application specific dialog box that is normally activated in 
     * @ref aboutApp will not be displayed when this signal is emitted.
     */
    void showAboutApplication( void );

  private:
    QPopupMenu   *mMenu;
    QMessageBox  *mAboutApp;
    KAboutDialog *mAboutKDE;
    KBugReport   *mBugReport;

    QString      mAboutAppText;
    QWidget      *mParent;

    bool         mShowWhatsThis;
};


#endif


