# Makefile for Recode related contributions.
# Copyright © 1997, 1998, 2000 Free Software Foundation, Inc.
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

AMIGA_PORT = Amiga/lib-Makefile Amiga/src-Makefile
DJGPP_PORT = DJGPP/README.in DJGPP/config.site DJGPP/configdj.bat \
DJGPP/configdj.sed DJGPP/DJGPP/README DJGPP/recodepo.sh
ELISP_FILES = recode.el utf8.el
SPEC_FILES = recode.spec1 recode.spec2

EXTRA_DIST = $(AMIGA_PORT) $(DJGPP_PORT) $(ELISP_FILES) $(SPEC_FILES)

MAINTAINERCLEANFILES = DJGPP/README DJGPP/fnchange.lst

all-local: DJGPP/README DJGPP/fnchange.lst

$(srcdir)/DJGPP/README: DJGPP/README.in $(top_srcdir)/configure.ac
	packageversion=`echo "$(VERSION)" | sed 's/\.//g'`; \
	sed \
	    -e 's/@V@/$(PACKAGE)-$(VERSION)/g' \
	    -e 's/@VER@/$(VERSION)/g' \
	    -e "s/@packageversion@/$$packageversion/g" \
	    -e "s/@treeversion@/$(VERSION)/g" \
	  $(srcdir)/DJGPP/README.in > $@-tmp
	mv $@-tmp $@

$(srcdir)/DJGPP/fnchange.lst: DJGPP/fnchange.in $(top_srcdir)/configure.ac
	sed \
	    -e '/^#/d' \
	    -e 's/@V@/$(PACKAGE)-$(VERSION)/g' \
	  $(srcdir)/DJGPP/fnchange.in > $@-tmp
	mv $@-tmp $@
