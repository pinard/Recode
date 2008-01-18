/* Conversion of files between different charsets and surfaces.
   Copyright © 1990, 93, 94, 96, 97, 98, 99, 00 Free Software Foundation, Inc.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1988.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the `recode' Library; see the file `COPYING.LIB'.
   If not, write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#if MSDOS || WIN32 || _WIN32
# define DOSWIN 1
# define DOSWIN_OR_OS2 1
#endif

#if OS2
# define DOSWIN_OR_OS2 1
#endif

#include <assert.h>
#include <stdio.h>

#if STDC_HEADERS
# include <stdlib.h>
#endif

#if HAVE_STDBOOL_H
# include <stdbool.h>
#else
typedef enum {false = 0, true = 1} bool;
#endif

#if HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
# define strchr index
# define strrchr rindex
#endif

#include <signal.h>
#ifndef RETSIGTYPE
# define RETSIGTYPE void
#endif

#if DIFF_HASH
# ifdef HAVE_LIMITS_H
#  include <limits.h>
# endif
# ifndef CHAR_BIT
#  define CHAR_BIT 8
# endif
#endif

/* Some systems do not define EXIT_*, even with STDC_HEADERS.  */
#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
# define EXIT_FAILURE 1
#endif
/* The following test is to work around the gross typo in systems like Sony
   NEWS-OS Release 4.0C, whereby EXIT_FAILURE is defined to 0, not 1.  */
#if !EXIT_FAILURE
# undef EXIT_FAILURE
# define EXIT_FAILURE 1
#endif

#include "xalloc.h"

#if HAVE_LOCALE_H
# include <locale.h>
#endif
#if !HAVE_SETLOCALE
# define setlocale(Category, Locale)
#endif

#if ENABLE_NLS
# include <libintl.h>
# define _(Text) gettext (Text)
#else
# define bindtextdomain(Domain, Directory)
# define textdomain(Domain)
# define _(Text) Text
#endif
#define N_(Text) Text

#ifndef PARAMS
# if __STDC__
#  define PARAMS(Args) Args
# else
#  define PARAMS(Args) ()
# endif
#endif

#include <argmatch.h>

#include <errno.h>
#ifndef errno
extern int errno;
#endif
#include "error.h"

/* Generate a mask of LENGTH one-bits, right justified in a word.  */
#define MASK(Length) ((unsigned) ~(~0 << (Length)))

/* Indicate if CHARACTER holds into 7 bits.  */
#define IS_ASCII(Character) \
  (!((Character) & ~MASK (7)))

/* This is a bit ridiculous.  DJGPP offers a non-working `pipe' function,
   for the only sake of being POSIX compliant (I'm told so).  So, `pipe'
   cannot be autoconfigured without AC_TRY_RUN, which is better avoided.  */
#if !DOSWIN
# if !_AMIGA
#  define HAVE_PIPE 1
# endif
#endif

/* Debugging the memory allocator.  */

#if WITH_DMALLOC
# define DMALLOC_FUNC_CHECK
# include <dmalloc.h>
#endif

#include "recodext.h"
