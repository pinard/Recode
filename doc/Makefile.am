# Makefile for `recode' documentation.
# Copyright © 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
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
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

AUTOMAKE_OPTIONS = gnits
info_TEXINFOS = recode.texi
man_MANS = recode.1

EXTRA_DIST = charset.texi File-Latin1 help2man $(MANS) Makemore	\
tables.pl

PERL = perl

recode.info: charset.texi
recode.dvi: charset.texi

recode.1: help2man $(top_srcdir)/src/main.c
	@if test -f ../src/recode; then \
	  echo "Updating the \`man' page for \`recode'"; \
	  LANGUAGE=C $(PERL) $(srcdir)/help2man ../src/recode > recode.1-tmp; \
	  mv recode.1-tmp $(srcdir)/recode.1; \
	else \
	  echo "WARNING: The \`man' page for \`recode' cannot be updated yet."; \
	  echo "         Retry once the program executable will be ready."; \
	fi
