/* xstring.h - string declarations wrapper wrapper
   Copyright (C) 1986, 1995, 1996 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#ifndef _xstring_h_
#define _xstring_h_

#define dirname sidestep_dirname_declaration
#define basename sidestep_basename_declaration
#ifdef HAVE_STRING_H
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#else
# include <strings.h>
char *memchr ();
#endif
#undef dirname
#undef basename

#define strequ(s1, s2)		(strcmp ((s1), (s2)) == 0)
#define strnequ(s1, s2, n)	(strncmp ((s1), (s2), (n)) == 0)

#if !HAVE_STRNDUP
extern char *strndup __P((char const *, size_t n));
#endif
extern char *basename __P((char const *));
extern char *dirname __P((char const *));

#endif /* not _xstring_h_ */
