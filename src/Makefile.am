# Makefile for `recode' sources.
# Copyright © 1991,92,93,94,95,96,97,98,99, 00 Free Software Foundation, Inc.
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

AUTOMAKE_OPTIONS = gnits ansi2knr
bin_PROGRAMS = recode
lib_LTLIBRARIES = librecode.la
man_MANS = recode.1
include_HEADERS = recode.h recodext.h

noinst_HEADERS = argmatch.h charname.h common.h decsteps.h \
fr-charname.h hash.h inisteps.h libiconv.h quotearg.h rfc1345.h	tersteps.h \
xalloc.h $(H_FALLBACKS) $(H_SURFACES)
EXTRA_DIST = stamp-steps $(L_STEPS) mergelex.py $(MANS)

C_STEPS = african.c afrtran.c applemac.c atarist.c bangbang.c cdcnos.c \
ebcdic.c ibmpc.c iconqnx.c lat1asci.c mule.c strip-data.c testdump.c \
ucs.c utf16.c utf7.c utf8.c varia.c vn.c $(C_FALLBACKS) $(C_SURFACES)
OLD_C_STEPS = next.c
L_STEPS = ascilat1.l $(L_FALLBACKS) $(L_SURFACES)

H_FALLBACKS =
C_FALLBACKS = flat.c html.c lat1ltex.c lat1txte.c rfc1345.c texinfo.c
L_FALLBACKS = ltexlat1.l txtelat1.l

H_SURFACES = base64.h
C_SURFACES = base64.c dump.c endline.c permut.c quoted.c
L_SURFACES =

recode_SOURCES = main.c freeze.c mixed.c
recode_LDADD = librecode.la ../lib/libreco.a

librecode_la_SOURCES = argmatch.c charname.c combine.c fr-charname.c \
hash.c iconv.c libiconv.c localcharset.c merged.c names.c outer.c quotearg.c \
recode.c request.c strip-pool.c task.c xmalloc.c $(C_STEPS)
librecode_la_LDFLAGS = -version-info 0:0:0

INCLUDES = -I.. -I$(srcdir) -I$(top_srcdir)/lib -I$(top_srcdir)/libiconv
DEFS = -DLIBDIR=\"$(libdir)\" @DEFS@

ETAGS_ARGS = $(L_STEPS)
TAGS_DEPENDENCIES = $(L_STEPS)

# These CFLAGS values are not auto-configured:
# -DDIFF_HASH		For using GNU diff's hash algorithm instead.
# -DHASH_STATS		For printing a one-liner hash statistics.

LEX = @LEX@
PERL = perl
PYTHON = python

$(srcdir)/decsteps.h: stamp-steps
$(srcdir)/inisteps.h: stamp-steps
$(srcdir)/tersteps.h: stamp-steps
stamp-steps: $(C_STEPS) merged.c
	@echo "Updating {dec,ini,ter}steps.h"
	@(cd $(srcdir) && grep -h '^module_' $(C_STEPS) merged.c ) \
	    | sed 's/ .*//' > liste.tmp
	@sed 's/\(.*\)/bool \1 PARAMS ((struct recode_outer *));/' \
	    liste.tmp > decsteps.tmp
	@sed 's/\(.*\)/void del\1 PARAMS ((struct recode_outer *));/' \
	    liste.tmp >> decsteps.tmp
	@sed 's/\(.*\)/  if (!\1 (outer)) return false;/' \
	    liste.tmp > inisteps.tmp
	@sed 's/\(.*\)/  del\1 (outer);/' \
	    liste.tmp > tersteps.tmp
	@rm liste.tmp
	@if cmp -s $(srcdir)/decsteps.h decsteps.tmp; then rm decsteps.tmp; \
	else mv decsteps.tmp $(srcdir)/decsteps.h; fi
	@if cmp -s $(srcdir)/inisteps.h inisteps.tmp; then rm inisteps.tmp; \
	else mv inisteps.tmp $(srcdir)/inisteps.h; fi
	@if cmp -s $(srcdir)/tersteps.h tersteps.tmp; then rm tersteps.tmp; \
	else mv tersteps.tmp $(srcdir)/tersteps.h; fi
	@echo timestamp > $(srcdir)/stamp-steps

main.o: main.c ../config.status
#main.o: common.h recode.h (should be automatic?)
outer.o: stamp-steps

$(srcdir)/merged.c: mergelex.py $(L_STEPS)
	(cd $(srcdir) && cat $(L_STEPS)) \
	| $(PYTHON) $(srcdir)/mergelex.py > merged.tm1
	$(LEX) -t8 merged.tm1 > merged.tm2
	grep -v '^# *line [0-9]' merged.tm2 > $(srcdir)/merged.c
	rm merged.tm1 merged.tm2

$(srcdir)/recode.1: recode
	@if test -r $@ && test ! -w $@; then \
	  echo "WARNING: Page \`$@' read only, not updated"; \
	elif $(PERL) $(top_srcdir)/doc/help2man --output=$@ ./recode; then \
	  echo "Page \`$@' has been updated"; \
	else \
	  echo "WARNING: Page \`$@' has *not* been updated."; \
	  echo "         It might be that \`Perl' is missing on your system,"; \
	  echo "         but you may safely and merely ignore this error."; \
	fi
