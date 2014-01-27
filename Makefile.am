# Main Makefile for Recode.
# Copyright © 1992,93,94,95,96,97,98,99,00 Free Software Foundation, Inc.
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

EXTRA_DIST = COPYING-LIB aclocal.sh after-patch.py after-patch.sh \
m4/ChangeLog tables.py \
keld/NomsSeulsfinal.lst keld/charsets.def keld/control.def \
keld/iso10646.def keld/mnemonics,ds keld/other.def keld/rfc1345.txt
SUBDIRS = doc lib po src tests contrib

# All object files should depend on cleaner.h as well on as config.h.
# For now at least, if you modify cleaner.h.in, make twice!
all-local: cleaner.h

get-po-files:
	cd $(top_srcdir) && \
	  rsync -Lrtvz translationproject.org::tp/latest/recode/ po

# The following goals for the maintainer of the Recode Web site.

local:
	$(MAKE) -C contrib
	$(MAKE) manual

manual:
	cd $(top_srcdir) \
	  && rm -rf web/manual web/manual-tmp \
	  && mkdir web/manual web/manual-tmp \
	  && makeinfo --html --output=web/manual-tmp doc/recode.texi \
	  && cd web/manual-tmp \
	  && for file in *.html; do \
	       sed 's,href=",href="/manual/,g' $$file > ../manual/$$file; \
	     done
	rm -rf $(top_srcdir)/web/manual-tmp

publish:
	(cd $(top_srcdir) && \
	  git archive --format=tar --prefix=recode-$(VERSION)/ HEAD .) \
	  | gzip > $(top_srcdir)/web/archives/recode-$(VERSION).tar.gz

official: publish
	rm -f $(top_srcdir)/web/archives/recode.tar.gz
	ln -s recode-$(VERSION).tar.gz $(top_srcdir)/web/archives/recode.tar.gz

synchro: local
	du -s --si $(top_srcdir)/.git
	cd $(top_srcdir) && git gc --auto --prune
	du -s --si $(top_srcdir)/.git
	find $(top_srcdir) -name '*~' | xargs rm -fv
	synchro -PD alcyon entretien

ifneq "$(wildcard ~/etc/mes-sites/site.mk)" ""

site: site-all

package_name = recode
mgp_base = http://pinard.progiciels-bpi.ca/fp-etc/dist/mgp
margin_color = "\#ff896c"
caption_color = "\#ffffb7"

LOGOURL = "/logo.gif"
GOALS = $(wildcard manual/*.html)

include ~/etc/mes-sites/site.mk

$(htmldir)/manual/%.html: manual/%.html
	@[ -d $(htmldir)/manual ] || mkdir -v $(htmldir)/manual
	@$(xsl-html)

endif
