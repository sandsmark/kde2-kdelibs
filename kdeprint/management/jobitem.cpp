/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2001 Michael Goffioul <goffioul@imec.be>
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include "jobitem.h"
#include "kmjob.h"

#include <kiconloader.h>

JobItem::JobItem(QListView *parent, KMJob *job)
: QListViewItem(parent)
{
	init(job);
}

void JobItem::init(KMJob *job)
{
	m_job = job;
	if (m_job)
	{
		setPixmap(0,SmallIcon(m_job->pixmap()));
		setText(0,QString::number(m_job->id()));
		setText(1,m_job->printer());
		setText(2,m_job->name());
		setText(3,m_job->owner());
		setText(4,m_job->stateString());
		setText(5,QString::number(m_job->size()));
		m_ID = m_job->id();
	}
	widthChanged();
}
