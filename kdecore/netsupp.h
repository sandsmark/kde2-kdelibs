/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2000,2001 Thiago Macieira <thiagom@mail.com>
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
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

#include <sys/socket.h>
#include "config.h"

/*
 * Seems some systems don't know about AF_LOCAL
 */
#ifndef AF_LOCAL
#define AF_LOCAL	AF_UNIX
#define PF_LOCAL	PF_UNIX
#endif

#ifdef CLOBBER_IN6
#define kde_in6_addr in6_addr
#define kde_sockaddr_in6 sockaddr_in6
#endif

#ifdef s6_addr
#undef s6_addr
#endif

/*
 * This is taken from RFC 2553
 */
struct kde_in6_addr
{
  unsigned char s6_addr[16];
};

struct kde_sockaddr_in6
{
#ifdef HAVE_SOCKADDR_SA_LEN
  Q_UINT8		sin6_len;
#endif
  sa_family_t		sin6_family;
  unsigned short       	sin6_port;	/* RFC says in_port_t */
  Q_UINT32		sin6_flowinfo;
  struct kde_in6_addr	sin6_addr;
  Q_UINT32		sin6_scope_id;
};

#ifdef NEED_IN6_TESTS

#define IN6_IS_ADDR_UNSPECIFIED(a) \
	(((Q_UINT32 *) (a))[0] == 0 && ((Q_UINT32 *) (a))[1] == 0 && \
	 ((Q_UINT32 *) (a))[2] == 0 && ((Q_UINT32 *) (a))[3] == 0)

#define IN6_IS_ADDR_LOOPBACK(a) \
	(((Q_UINT32 *) (a))[0] == 0 && ((Q_UINT32 *) (a))[1] == 0 && \
	 ((Q_UINT32 *) (a))[2] == 0 && ((Q_UINT32 *) (a))[3] == htonl (1))

#define IN6_IS_ADDR_MULTICAST(a) (((u_int8_t *) (a))[0] == 0xff)

#define IN6_IS_ADDR_LINKLOCAL(a) \
	((((Q_UINT32 *) (a))[0] & htonl (0xffc00000)) == htonl (0xfe800000))

#define IN6_IS_ADDR_SITELOCAL(a) \
	((((Q_UINT32 *) (a))[0] & htonl (0xffc00000)) == htonl (0xfec00000))

#define IN6_IS_ADDR_V4MAPPED(a) \
	((((Q_UINT32 *) (a))[0] == 0) && (((Q_UINT32 *) (a))[1] == 0) && \
	 (((Q_UINT32 *) (a))[2] == htonl (0xffff)))

#define IN6_IS_ADDR_V4COMPAT(a) \
	((((Q_UINT32 *) (a))[0] == 0) && (((Q_UINT32 *) (a))[1] == 0) && \
	 (((Q_UINT32 *) (a))[2] == 0) && (ntohl (((Q_UINT32 *) (a))[3]) > 1))

#define IN6_ARE_ADDR_EQUAL(a,b) \
	((((Q_UINT32 *) (a))[0] == ((Q_UINT32 *) (b))[0]) && \
	 (((Q_UINT32 *) (a))[1] == ((Q_UINT32 *) (b))[1]) && \
	 (((Q_UINT32 *) (a))[2] == ((Q_UINT32 *) (b))[2]) && \
	 (((Q_UINT32 *) (a))[3] == ((Q_UINT32 *) (b))[3]))

#define IN6_IS_ADDR_MC_NODELOCAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((Q_UINT8 *) (a))[1] & 0xf) == 0x1))

#define IN6_IS_ADDR_MC_LINKLOCAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((Q_UINT8 *) (a))[1] & 0xf) == 0x2))

#define IN6_IS_ADDR_MC_SITELOCAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((Q_UINT8 *) (a))[1] & 0xf) == 0x5))

#define IN6_IS_ADDR_MC_ORGLOCAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((Q_UINT8 *) (a))[1] & 0xf) == 0x8))

#define IN6_IS_ADDR_MC_GLOBAL(a) \
	(IN6_IS_ADDR_MULTICAST(a) && ((((Q_UINT8 *) (a))[1] & 0xf) == 0xe))

#endif

#ifndef HAVE_GETADDRINFO
struct addrinfo
{
  int ai_flags;			/* Input flags.  */
  int ai_family;		/* Protocol family for socket.  */
  int ai_socktype;		/* Socket type.  */
  int ai_protocol;		/* Protocol for socket.  */
  int ai_addrlen;		/* Length of socket address.  */
  struct sockaddr *ai_addr;	/* Socket address for socket.  */
  char *ai_canonname;		/* Canonical name for service location.  */
  struct addrinfo *ai_next;	/* Pointer to next in list.  */
};

# ifdef AI_PASSIVE
#  undef AI_PASSIVE
#  undef AI_CANONNAME
#  undef AI_NUMERICHOST
# endif

/* Possible values for `ai_flags' field in `addrinfo' structure.  */
# define AI_PASSIVE	1	/* Socket address is intended for `bind'.  */
# define AI_CANONNAME	2	/* Request for canonical name.  */
# define AI_NUMERICHOST	4	/* Don't use name resolution.  */

# ifdef EAI_ADDRFAMILY
#  warning "Your system already defines some getaddrinfo constants!"
#  undef EAI_ADDRFAMILY
#  undef EAI_AGAIN
#  undef EAI_BADFLAGS
#  undef EAI_FAIL
#  undef EAI_FAMILY
#  undef EAI_MEMORY
#  undef EAI_NODATA
#  undef EAI_NONAME
#  undef EAI_SERVICE
#  undef EAI_SOCKTYPE
#  undef EAI_SYSTEM
# endif

/* Error values for `getaddrinfo' function.  */
# define EAI_ADDRFAMILY	1	/* Address family for NAME not supported.  */
# define EAI_AGAIN	2	/* Temporary failure in name resolution.  */
# define EAI_BADFLAGS	3	/* Invalid value for `ai_flags' field.  */
# define EAI_FAIL	4	/* Non-recoverable failure in name res.  */
# define EAI_FAMILY	5	/* `ai_family' not supported.  */
# define EAI_MEMORY	6 	/* Memory allocation failure.  */
# define EAI_NODATA	7	/* No address associated with NAME.  */
# define EAI_NONAME	8	/* NAME or SERVICE is unknown.  */
# define EAI_SERVICE	9	/* SERVICE not supported for `ai_socktype'.  */
# define EAI_SOCKTYPE	10	/* `ai_socktype' not supported.  */
# define EAI_SYSTEM	11	/* System error returned in `errno'.  */

/*
 * These are specified in the RFC
 * We won't undefine them. If someone defined them to a different value
 * the preprocessor will generate an error
 */
# define NI_MAXHOST	1025
# define NI_MAXSERV	32

# ifdef NI_NUMERICHOST
#  undef NI_NUMERICHOST
#  undef NI_NUMERICSERV
#  undef NI_NOFQDN
#  undef NI_NAMEREQD
#  undef NI_DGRAM
# endif

# define NI_NUMERICHOST	1	/* Don't try to look up hostname.  */
# define NI_NUMERICSERV 2	/* Don't convert port number to name.  */
# define NI_NOFQDN	4	/* Only return nodename portion.  */
# define NI_NAMEREQD	8	/* Don't return numeric addresses.  */
# define NI_DGRAM	16	/* Look up UDP service rather than TCP.  */

namespace KDE
{
  extern int getaddrinfo(const char *name, const char *service,
			 const struct addrinfo* hint,
			 struct addrinfo** result);
  extern void freeaddrinfo(struct addrinfo* ai);
  extern char *gai_strerror(int errorcode);
  extern int getnameinfo(const struct sockaddr *sa,
			 socklen_t salen,
			 char *host, size_t hostlen,
			 char *serv, size_t servlen,
			 int flags);
}

# define getaddrinfo	KDE::getaddrinfo
# define freeaddrinfo	KDE::freeaddrinfo
# define gai_strerror	KDE::gai_strerror
# define getnameinfo	KDE::getnameinfo

#endif

#ifndef HAVE_INET_PTON

namespace KDE
{
  extern int inet_pton(int af, const char *cp, void* buf);
}

# define inet_pton	KDE::inet_pton
#endif

#ifndef HAVE_INET_NTOP

namespace KDE
{
  extern const char* inet_ntop(int af, const void *cp, char *buf, size_t len);
}

# define inet_ntop	KDE::inet_ntop
#endif