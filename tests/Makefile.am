# Makefile for `recode' regression tests.
# Copyright © 1996, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.
# François Pinard <pinard@iro.umontreal.ca>, 1988.

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

SUITE = t21_names.py t22_lists.py t25_subsets.py t30_base64.py \
t30_dumps.py t30_quoted.py t40_african.py t40_combine.py t40_testdump.py \
t40_utf7.py t40_utf8.py t50_methods.py t90_bigauto.py

EXTRA_DIST = NOTES pytest common.py $(SUITE)

PYTHON = python

check-local:
	RECODE=../src/recode PYTHONPATH=$(srcdir) \
	  $(PYTHON) $(srcdir)/pytest $(srcdir)/t*.py
