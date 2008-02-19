# Makefile for `recode' documentation.
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

EXTRA_DIST = libiconv.texi rfc1345.texi File-Latin1 help2man Makemore \
tables.py

MOSTLYCLEANFILES = recode.ops

recode.info: libiconv.texi rfc1345.texi
recode.dvi: libiconv.texi rfc1345.texi
