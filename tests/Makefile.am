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

SUITE = african.m4 base64.m4 combine.m4 dumps.m4 lists.m4 methods.m4 \
names.m4 quoted.m4 testdump.m4 utf7.m4 utf8.m4

EXTRA_DIST = atgeneral.m4 suite.m4 testsuite $(SUITE) \
bigauto.py dumps-data quoted-data utf7-data

PYTHON = python

all-local: atconfig testsuite

# I have been told that the `libtool' bug who warranted the following
# diagnostic has been corrected by now, so I silenced it out of `check-local'.
#	@if test -f $(libdir)/librecode.so; then \
#	  echo; \
#	  echo "WARNING: A recode library is already installed.  Sadly, it"; \
#	  echo "         _might_ interfere with checking.  You might want to"; \
#	  echo "         delete it first, or rename it out of the way."; \
#	  echo; \
#	fi
check-local: atconfig testsuite
	$(SHELL) $(srcdir)/testsuite

$(srcdir)/testsuite: atgeneral.m4 suite.m4 $(SUITE)
	m4 -I $(srcdir) atgeneral.m4 suite.m4 | cat -s > $@-tmp
	mv $@-tmp $@

bigtest: warning
	PATH=../src:$$PATH $(PYTHON) $(srcdir)/bigauto.py

bigtest-strict: warning
	PATH=../src:$$PATH $(PYTHON) $(srcdir)/bigauto.py -s

warning:
	echo 1>&2 '==============================================='
	echo 1>&2 "The \`bigauto' test may take hours.  Be patient."
	echo 1>&2 '==============================================='

clean-local:
	rm -f at-check-line debug-*.sh stderr stdout
	rm -f input output big-s-off.res big-s-on.res
