/*
    This file is part of the KDE File Manager

    Copyright (C) 1998 Waldo Bastian (bastian@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License 
    as published by the Free Software Foundation; either version 2 
    of the License, or (at your option) any later version.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; see the file COPYING. If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
//----------------------------------------------------------------------------
//
// KDE File Manager -- HTTP Cookie Dialogs
// $Id$

#ifndef _KCOOKIEWIN_H_
#define _KCOOKIEWIN_H_

#include "kdialogbase.h"
#include "kcookiejar.h"                                                
#include "qlayout.h"

class KHttpCookie;
class QRadioButton;

class KCookieWin : public KDialogBase
{
public:
    
    KCookieWin( QWidget *parent, KHttpCookie *_cookie, KCookieJar *cookiejar );
    ~KCookieWin();
 
    /**
     * The following function executes the dialog.
     */
    KCookieAdvice advice(KCookieJar *cookiejar); 
    
protected:
    KHttpCookie *cookie;
    QRadioButton *rb1;
    QRadioButton *rb2;
    QRadioButton *rb3;
    QGridLayout *layout;

private slots:
    void slotCancel();

};

#endif

