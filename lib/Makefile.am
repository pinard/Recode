# Makefile for `recode' (own internal) library.
# Copyright © 1995, 1996, 1997, 1998, 1999 Free Software Foundation, Inc.
# François Pinard <pinard@iro.umontreal.ca>

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

AUTOMAKE_OPTIONS = gnits

noinst_LIBRARIES = libreco.a
noinst_HEADERS = error.h getopt.h gettext.h pathmax.h xstring.h
libreco_a_SOURCES = error.c getopt.c getopt1.c xstrdup.c

EXTRA_DIST = alloca.c gettext.c malloc.c realloc.c strtol.c strtoul.c

INCLUDES = -I.. -I$(srcdir)

libreco_a_LIBADD = @ALLOCA@ @LIBOBJS@
## libreco_a_DEPENDENCIES = $(libtar_a_LIBADD)
localedir = $(prefix)/share/locale
aliaspath = $(localedir):.

gettext.o: gettext.c
	$(COMPILE) -DALIASPATH=\"$(aliaspath)\" -DLOCALEDIR=\"$(localedir)\" \
	  -c $(srcdir)/gettext.c

gettext._o: gettext._c
	@rm -f _gettext.c
	@ln gettext._c _gettext.c
	$(COMPILE) -DALIASPATH=\"$(aliaspath)\" -DLOCALEDIR=\"$(localedir)\" \
	  -c _gettext.c
	@mv _gettext.o $@
	@rm _gettext.c
