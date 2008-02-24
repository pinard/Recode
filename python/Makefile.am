# Makefile for `recode' Python interface (used for tests).
# Copyright © 2008 Free Software Foundation, Inc.
# François Pinard <pinard@iro.umontreal.ca>, 2008.

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

EXTRA_DIST = Recode.c Recode.pyx

PYTHON = python

all-local: Recode.c

install-data-local: all-local
	$(PYTHON) $(srcdir)/setup.py install --skip-build

clean-local:
	rm -f Recode.c
	rm -rf build

Recode.c: Recode.pyx
	rm -f Recode.pyx
	cp $(srcdir)/Recode.pyx .
	$(PYTHON) $(srcdir)/setup.py build_ext \
	  --include-dir=$(top_builddir):$(top_srcdir)/src \
	  --libraries=recode
	rm -f Recode.pyx
