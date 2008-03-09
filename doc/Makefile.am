# Makefile for Recode documentation.
# Copyright © 1994, 95, 96, 97, 98, 99, 00 Free Software Foundation, Inc.
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
MAKEINFO = LANG= LANGUAGE= @MAKEINFO@

EXTRA_DIST = rfc1345.texi File-Latin1 help2man
MOSTLYCLEANFILES = recode.ops

# Relative to $(srcdir).
KELD = ../keld
CHARSETS_DEF = $(KELD)/charsets.def
MNEMONICS_DS = $(KELD)/iso10646.def $(KELD)/other.def $(KELD)/control.def

PYTHON = python
TABLES_PY = $(PYTHON) $(top_srcdir)/tables.py

$(srcdir)/recode.info: rfc1345.texi
recode.dvi: rfc1345.texi

# There is duplication of work with making src/strip-{pool,data}.c.
# This used to be combined, but it seems clearer to keep both separate.
rfc1345.texi: ../tables.py $(MNEMONICS_DS) $(CHARSETS_DEF)
	$(TABLES_PY) -C $(srcdir) -t $(MNEMONICS_DS) $(CHARSETS_DEF)

fr-rfc1345.texi: ../tables.py $(MNEMONICS_DS) $(CHARSETS_DEF)
	$(TABLES_PY) -C $(srcdir) -Ft $(MNEMONICS_DS) $(CHARSETS_DEF)
