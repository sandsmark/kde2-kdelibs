    /*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Permission is also granted to link this program with the Qt
    library, treating Qt like a library that normally accompanies the
    operating system kernel, whether or not that is in fact the case.

    */

#ifndef MCOPUTILS_H
#define MCOPUTILS_H

#include <string>

class MCOPUtils {
public:
	/**
	 * Returns the full pathname to a file in the mcop directory which
	 * is called "name". It will also care that no other characters than
	 * A-Z,a-z,0-9,-,_ occur.
	 * 
	 * The result is something like /tmp/mcop-<username>/name, the directory
	 * will be created when necessary.
	 */
	static std::string createFilePath(std::string name);

	/**
	 * Returns the fully qualified hostname, such as "www.kde.org" (of course
	 * this may fail due to misconfiguration).
	 *
	 * The result is "localhost" if nothing at all can be found out.
	 */
	static string getFullHostname();
};

#endif /* MCOPUTILS_H */
