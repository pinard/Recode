# Makefile for `libiconv' part of `recode'.
# Copyright © 1999, 2000 Free Software Foundation, Inc.
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
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

AUTOMAKE_OPTIONS = gnits

noinst_HEADERS = iconv.h aliases.h armscii_8.h ascii.h big5.h big5hkscs.h \
ces_big5.h ces_gbk.h cjk_variants.h cns11643.h cns11643_1.h cns11643_2.h \
cns11643_3.h cns11643_inv.h converters.h cp1133.h cp1250.h cp1251.h cp1252.h \
cp1253.h cp1254.h cp1255.h cp1256.h cp1257.h cp1258.h cp850.h cp866.h cp874.h \
cp932.h cp932ext.h cp936ext.h cp949.h cp950.h cp950ext.h euc_cn.h euc_jp.h \
euc_kr.h euc_tw.h flags.h gb12345.h gb12345ext.h gb18030.h gb18030ext.h \
gb18030uni.h gb2312.h gbk.h gbkext1.h gbkext2.h gbkext_inv.h \
georgian_academy.h georgian_ps.h hkscs.h hp_roman8.h hz.h iso2022_cn.h \
iso2022_cnext.h iso2022_jp.h iso2022_jp1.h iso2022_jp2.h iso2022_kr.h \
iso646_cn.h iso646_jp.h iso8859_1.h iso8859_10.h iso8859_13.h iso8859_14.h \
iso8859_15.h iso8859_16.h iso8859_2.h iso8859_3.h iso8859_4.h iso8859_5.h \
iso8859_6.h iso8859_7.h iso8859_8.h iso8859_9.h isoir165.h isoir165ext.h java.h jisx0201.h jisx0208.h jisx0212.h johab.h johab_hangul.h koi8_r.h \
koi8_ru.h koi8_u.h ksc5601.h loops.h loop_unicode.h loop_wchar.h mac_arabic.h \
mac_centraleurope.h mac_croatian.h mac_cyrillic.h mac_greek.h mac_hebrew.h \
mac_iceland.h mac_roman.h mac_romania.h mac_thai.h mac_turkish.h \
mac_ukraine.h mulelao.h nextstep.h sjis.h tcvn.h tis620.h translit.h \
ucs2.h ucs2be.h ucs2le.h ucs2internal.h ucs2swapped.h \
ucs4.h ucs4be.h ucs4le.h ucs4internal.h ucs4swapped.h \
uhc_1.h uhc_2.h utf16.h utf16be.h utf16le.h utf7.h \
utf8.h viscii.h \
libcharset.h

EXTRA_DIST = encodings.def encodings_local.def \
config.charset ref-add.sin ref-del.sin

all-local: charset.alias ref-add.sed ref-del.sed

charset_alias = $(DESTDIR)$(libdir)/charset.alias
charset_tmp = $(DESTDIR)$(libdir)/charset.tmp
install-exec-local: all-local
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	if test -f $(charset_alias); then \
	  sed -f ref-add.sed $(charset_alias) > $(charset_tmp) ; \
	  $(INSTALL_DATA) $(charset_tmp) $(charset_alias) ; \
	  rm -f $(charset_tmp) ; \
	else \
	  if test @GLIBC21@ = no; then \
	    sed -f ref-add.sed charset.alias > $(charset_tmp) ; \
	    $(INSTALL_DATA) $(charset_tmp) $(charset_alias) ; \
	    rm -f $(charset_tmp) ; \
	  fi ; \
	fi

uninstall-local: all-local
	if test -f $(charset_alias); then \
	  sed -f ref-del.sed $(charset_alias) > $(charset_tmp); \
	  if grep '^# Packages using this file: $$' $(charset_tmp) \
	      > /dev/null; then \
	    rm -f $(charset_alias); \
	  else \
	    $(INSTALL_DATA) $(charset_tmp) $(charset_alias); \
	  fi; \
	  rm -f $(charset_tmp); \
	fi

charset.alias: config.charset
	$(SHELL) $(srcdir)/config.charset '@host@' > t-$@
	mv t-$@ $@

SUFFIXES = .sed .sin
.sin.sed:
	sed -e '/^#/d' -e 's/@''PACKAGE''@/@PACKAGE@/g' $< > $@-t
	mv $@-t $@

CLEANFILES = charset.alias ref-add.sed ref-del.sed
