/* This file is part of the KDE libraries
    Copyright (C) 1998 Stephan Kulow <coolo@kde.org>
                  1998 Daniel Grana <grana@ie.iwi.unibe.ch>

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

// $Id$

#include "kfileviewitem.h"
#include "kcombiview.h"
#include "kdirlistbox.h"
#include "kfileiconview.h"
#include "kfiledetailview.h"
#include "config-kfile.h"

#include <qpainter.h>
#include <qlistbox.h>

#include <qdir.h>

#include <kapp.h>
#include <kconfig.h>
#include <kglobal.h>

#include <qvaluelist.h>

KCombiView::KCombiView( QWidget *parent, const char *name)
  : QSplitter( parent, name), KFileView(), right(0)
{
    KFileIconView *dirs = new KFileIconView( (QSplitter*)this, "left" );
    dirs->KFileView::setViewMode( Directories );
    dirs->setArrangement( QIconView::LeftToRight );
    left = dirs;
    dirs->setOperator(this);
}

KCombiView::~KCombiView()
{
    delete right;
}

void KCombiView::setRight(KFileView *view)
{
    right = view;
    right->KFileView::setViewMode( Files );
    setViewName( right->viewName() );

    QValueList<int> lst;
    lst << left->gridX() + 2 * left->spacing();
    setSizes( lst );
    setResizeMode( left, QSplitter::KeepSize );

    right->setOperator(this);
}

void KCombiView::insertSorted(KFileViewItem *first, uint )
{
    KFileViewItem *f_first = 0, *d_first = 0;
    uint dirs = 0, files = 0;

    KFileViewItem *tmp;

    for (KFileViewItem *it = first; it;) {
	tmp = it->next();

	if (it->isDir()) {
	    if (!d_first) {
		d_first = it;
		d_first->setNext(0);
	    } else {
		it->setNext(d_first);
		d_first = it;
	    }
	    dirs++;
	} else {
	    if (!f_first) {
		f_first = it;
		f_first->setNext(0);
	    } else {
		it->setNext(f_first);
		f_first = it;
	    }
	    files++;
	}
	it = tmp;
    }

    if (dirs)
	left->insertSorted(d_first, dirs);
    if (files)
	right->insertSorted(f_first, files);
}

void KCombiView::insertItem( KFileViewItem * )
{
    debug("KCombiView::insertItem not implemented (as not needed :)");
}

void KCombiView::clearView()
{
    left->clearView();
    right->clearView();
}

void KCombiView::updateView( bool b )
{
    left->updateView( b );
    right->updateView( b );
}

void KCombiView::updateView( const KFileViewItem *i )
{
    left->updateView( i );
    right->updateView( i );
}

void KCombiView::clear()
{
    KFileView::clear();
    left->KFileView::clear();
    right->clear();
}

void KCombiView::clearSelection()
{
    left->clearSelection();
    right->clearSelection();
}

void KCombiView::setSelectMode( KFileView::SelectionMode sm )
{
    left->setSelectMode( sm );
    right->setSelectMode( sm );
}

void KCombiView::highlightItem( const KFileViewItem *)
{
    debug("TODO");
}

void KCombiView::selectDir(const KFileViewItem* item)
{
    sig->activateDir(item);
}

void KCombiView::highlightFile(const KFileViewItem* item)
{
    sig->highlightFile(item);
}

void KCombiView::selectFile(const KFileViewItem* item)
{
    sig->activateFile(item);
}

void KCombiView::activatedMenu(const KFileViewItem *item)
{
    sig->activateMenu(item);
}

// *****************************************************************************

#include "kcombiview.moc"

