# Makefile for Recode sources.
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

noinst_HEADERS = argmatch.h charname.h common.h decsteps.h exit.h exitfail.h \
fr-charname.h hash.h inisteps.h iconvdecl.h quotearg.h rfc1345.h tersteps.h \
xalloc.h $(H_FALLBACKS) $(H_SURFACES)

EXTRA_DIST = recodext.h.in stamp-steps stamp-strip \
$(L_STEPS) mergelex.py $(MANS)

CLEANFILES = iconvdecl.h

C_STEPS = african.c afrtran.c applemac.c atarist.c bangbang.c cdcnos.c \
ebcdic.c ibmpc.c iconqnx.c lat1asci.c java.c mule.c strip-data.c \
testdump.c ucs.c utf16.c utf7.c utf8.c varia.c vn.c \
$(C_FALLBACKS) $(C_SURFACES) merged.c

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

librecode_la_SOURCES = argmatch.c charname.c combine.c exitfail.c \
fr-charname.c hash.c iconv.c localcharset.c names.c outer.c quotearg.c \
recode.c request.c strip-pool.c task.c xalloc-die.c xmalloc.c $(C_STEPS)
librecode_la_LDFLAGS = -version-info 0:0:0

# Relative to $(srcdir).
ALAIN = $(KELD)
KELD = ../keld
CHARSETS_DEF = $(KELD)/charsets.def
MNEMONICS_DS = $(KELD)/iso10646.def $(KELD)/other.def $(KELD)/control.def
RFC1345_TXT = $(KELD)/rfc1345.txt
NOMS_CARACS = $(ALAIN)/NomsSeulsfinal.lst

INCLUDES = -I.. -I$(srcdir) -I$(top_srcdir)/lib
DEFS = -DLIBDIR=\"$(libdir)\" @DEFS@

ETAGS_ARGS = $(L_STEPS)
TAGS_DEPENDENCIES = $(L_STEPS)

# These CFLAGS values are not auto-configured:
# -DDIFF_HASH		For using GNU diff's hash algorithm instead.
# -DHASH_STATS		For printing a one-liner hash statistics.

LEX = @LEX@
PERL = perl
PYTHON = python
TABLES_PY = $(PYTHON) $(top_srcdir)/tables.py

$(srcdir)/merged.c: mergelex.py $(L_STEPS)
	(cd $(srcdir) && cat $(L_STEPS)) \
	| $(PYTHON) $(srcdir)/mergelex.py > merged.tm1
	$(LEX) -t -8 -Plibrecode_yy merged.tm1 > merged.tm2
	grep -v '^# *line [0-9]' merged.tm2 > $(srcdir)/merged.c
	rm merged.tm1 merged.tm2

$(srcdir)/recode.1: recode
	@if test -r $@ && test ! -w $@; then \
	  echo "WARNING: Page \`$@' read only, not updated"; \
	elif $(PERL) $(top_srcdir)/doc/help2man \
	    --name="converts files between character sets" \
	    --output=$@ ./recode; then \
	  echo "Page \`$@' has been updated"; \
	else \
	  echo "WARNING: Page \`$@' has *not* been updated."; \
	  echo "         It might be that \`Perl' is missing on your system,"; \
	  echo "         but you may safely and merely ignore this error."; \
	fi

$(srcdir)/recodext.h: $(top_srcdir)/cleaner.h.in $(srcdir)/recodext.h.in
	cat $(top_srcdir)/cleaner.h.in $(srcdir)/recodext.h.in > $@

main.o: main.c ../config.status

charname$U.lo: rfc1345.h
charname.h: ../tables.py $(MNEMONICS_DS) $(RFC1345_TXT)
	$(TABLES_PY) -C $(srcdir) -n $(MNEMONICS_DS) $(RFC1345_TXT)

fr-charname$U.lo: fr-charname.h
fr-charname.h: ../tables.py $(NOMS_CARACS)
	$(TABLES_PY) -C $(srcdir) -Fn $(NOMS_CARACS)

iconv$U.lo: iconvdecl.h
iconvdecl.h: ../tables.py
	$(TABLES_PY) -i

outer$U.lo: stamp-steps
stamp-steps: $(C_STEPS)
	@echo "Updating {dec,ini,ter}steps.h"
	@(cd $(srcdir) && grep -h '^module_' $(C_STEPS)) \
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
	@echo timestamp > $(srcdir)/$@

rfc1345$U.lo: rfc1345.h
rfc1345.h: ../tables.py $(MNEMONICS_DS)
	$(TABLES_PY) -C $(srcdir) -m $(MNEMONICS_DS)

# There is duplication of work with making doc/rfc1345.texi.
# This used to be combined, but it seems clearer to keep both separate.
strip-pool.c strip-data.c: stamp-strip
stamp-strip: ../tables.py $(MNEMONICS_DS) $(CHARSETS_DEF)
	$(TABLES_PY) -C $(srcdir) -p $(MNEMONICS_DS) $(CHARSETS_DEF)
	echo timestamp > $(srcdir)/$@
