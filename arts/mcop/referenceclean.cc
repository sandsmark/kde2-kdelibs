    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de

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

#include "referenceclean.h"
#include "time.h"

using namespace std;
using namespace Arts;

ReferenceClean::ReferenceClean(Pool<Object_skel>& objectPool)
	:objectPool(objectPool)
{
	Dispatcher::the()->ioManager()->addTimer(5000, this);
}

void ReferenceClean::notifyTime()
{
	/*
	 * This last_notify and now check is because IOManager accumulates
	 * notifyTime() calls, so it may happen that we don't get one for ten
	 * seconds, and then two. However, this breaks the "second-chance"
	 * algorithm referenceClean is using, which depends on the fact that
	 * there is some significant time delay between two calls. So we'll
	 * verify it by hand.
	 */

	static time_t last_notify = 0;
	time_t now;

	time(&now);
	if(last_notify-now > 4)
	{
		list<Object_skel *> items = objectPool.enumerate();
		list<Object_skel *>::iterator i;

		for(i=items.begin();i != items.end();i++) (*i)->_referenceClean();
		last_notify = now;
	}
}

ReferenceClean::~ReferenceClean()
{
	Dispatcher::the()->ioManager()->removeTimer(this);
}
