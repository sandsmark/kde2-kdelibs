/* This file is part of the KDE libraries
   Copyright (C) 2000 Matej Koss <koss@miesto.sk>

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

#include <qtooltip.h>
#include <qlayout.h>
#include <qwidgetstack.h>

#include <kapp.h>
#include <klocale.h>

#include "jobclasses.h"
#include "statusbarprogress.h"

StatusbarProgress::StatusbarProgress( QWidget* parent, bool button )
  : ProgressBase( parent ) {

  m_bShowButton = button;

  int w = fontMetrics().width( " 999.9 kB/s 00:00:01 " ) + 8;
  box = new QHBoxLayout( this, 0, 0 );

  m_pButton = new QPushButton( "X", this );
  box->addWidget( m_pButton  );
  stack = new QWidgetStack( this );
  box->addWidget( stack );

  m_pProgressBar = new KProgress( 0, 100, 0, KProgress::Horizontal, this );
  m_pProgressBar->setFrameStyle( QFrame::Box | QFrame::Raised );
  m_pProgressBar->setLineWidth( 1 );
  m_pProgressBar->setBackgroundMode( QWidget::PaletteBackground );
  m_pProgressBar->setBarColor( Qt::blue );
  m_pProgressBar->installEventFilter( this );
  m_pProgressBar->setMinimumWidth( w );
  stack->addWidget( m_pProgressBar, 1 );

  m_pLabel = new QLabel( "", this );
  m_pLabel->setAlignment( AlignHCenter | AlignVCenter );
  m_pLabel->installEventFilter( this );
  m_pLabel->setMinimumWidth( w );
  stack->addWidget( m_pLabel, 2 );
  setMinimumSize( sizeHint() );

  mode = None;
  setMode();
}


void StatusbarProgress::setJob( KIO::Job *job )
{
  // we don't want to delete this widget, only clean
  ProgressBase::setJob( job, true, false );

  connect( m_pButton, SIGNAL( clicked() ), this, SLOT( stop() ) );
  mode = Progress;
  setMode();
}


void StatusbarProgress::setMode() {
  switch ( mode ) {
  case None:
    if ( m_bShowButton ) {
      m_pButton->hide();
    }
    stack->hide();
    break;

  case Label:
    if ( m_bShowButton ) {
      m_pButton->show();
    }
    stack->show();
    stack->raiseWidget( m_pLabel );
    break;

  case Progress:
    if ( m_bShowButton ) {
      m_pButton->show();
    }
    stack->show();
    stack->raiseWidget( m_pProgressBar );
    break;
  }
}


void StatusbarProgress::clean() {
  m_pJob = 0L;
  m_pProgressBar->setValue( 0 );
  m_pLabel->clear();

  mode = None;
  setMode();
}


void StatusbarProgress::slotTotalSize( KIO::Job*, unsigned long _size ) {
  m_iTotalSize = _size;
}

void StatusbarProgress::slotPercent( KIO::Job*, unsigned long _percent ) {
  m_pProgressBar->setValue( _percent );
}


void StatusbarProgress::slotSpeed( KIO::Job*, unsigned long _bytes_per_second ) {
  if ( _bytes_per_second == 0 ) {
    m_pLabel->setText( i18n( " Stalled ") );
  } else {
    m_pLabel->setText( i18n( " %1/s ").arg( KIO::convertSize( _bytes_per_second )) );
  }
}


bool StatusbarProgress::eventFilter( QObject *, QEvent *ev ) {
  if ( ! m_pJob ) { // don't react when there isn't any job doing IO
    return true;
  }

  if ( ev->type() == QEvent::MouseButtonPress ) {
    QMouseEvent *e = (QMouseEvent*)ev;

    if ( e->button() == LeftButton ) {    // toggle view on left mouse button
      if ( mode == Label ) {
	mode = Progress;
      } else if ( mode == Progress ) {
	mode = Label;
      }
      setMode();
      return true;

    }
  }

  return false;
}

#include "statusbarprogress.moc"
