# Main Makefile for GNU recode.
# Copyright © 1992-1999, 2000, 2001 Free Software Foundation, Inc.
# François Pinard <pinard@iro.umontreal.ca>, 1992.

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
ACLOCAL = ./aclocal.sh @ACLOCAL@
ACLOCAL_AMFLAGS = -I m4

BABYL = ../rmail/* ../*/rmail/* ../*/*/RMAIL ../*/*/rmail/*
EXTRA_DIST = COPYING-LIB PATCHES-AC rebox.el recode.spec aclocal.sh
SUBDIRS = m4 doc intl lib libiconv src po tests contrib

install-stow:
	$(MAKE) install \
	  prefix=$(exec_prefix)/stow/$(PACKAGE)-$(VERSION) \
	  exec_prefix=$(exec_prefix)/stow/$(PACKAGE)-$(VERSION)
	rm -f $(exec_prefix)/stow/$(PACKAGE)-$(VERSION)/info/dir
	cd $(exec_prefix)/stow && stow $(PACKAGE)-$(VERSION)
