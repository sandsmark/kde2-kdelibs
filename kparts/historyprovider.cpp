/* This file is part of the KDE project
   Copyright (C) 2001 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qdict.h>

#include "historyprovider.h"

using namespace KParts;

class HistoryProvider::HistoryProviderPrivate
{
public:
    HistoryProviderPrivate() : dict( 3001 ) {}

    QDict<void> dict;
};

HistoryProvider::HistoryProvider( QObject *parent, const char *name )
    : QObject( parent, name )
{
    d = new HistoryProviderPrivate;
}

HistoryProvider::~HistoryProvider()
{
    delete d;
}

bool HistoryProvider::contains( const QString& item ) const
{
    return (bool) d->dict.find( item );
}

void HistoryProvider::insert( const QString& item )
{
    if ( !d->dict.find( item ) ) {
	// no need to allocate memory, we only want to have fast lookup,
	// no mapping
	d->dict.insert( item, (void*) 1 );
	emit inserted( item );
    }
}

void HistoryProvider::remove( const QString& item )
{
    (void) d->dict.remove( item );
}

void HistoryProvider::clear()
{
    d->dict.clear();
}	

#include "historyprovider.moc"
