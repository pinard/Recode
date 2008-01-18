# RPM spec file for Free recode.
# Copyright (C) 2000 Progiciels Bourbeau-Pinard inc.
# Franc,ois Pinard <pinard@iro.umontreal.ca>, 2000.

Name:    recode
Version: 3.5d
Release: 1
Summary: Converter between charsets and surfaces
Group:   Utilities/Text

Vendor:    Progiciels Bourbeau-Pinard inc.
Copyright: GPL, LGPL
Packager:  Franc,ois Pinard <pinard@iro.umontreal.ca>

Source: http://www.iro.umontreal.ca/contrib/recode/recode-3.5d.tar.gz
URL:    http://www.iro.umontreal.ca/contrib/recode/HTML

Buildroot: /bpi/titan/home/pinard/.rpm

%description
The recode library converts files between character sets and usages.
It recognises or produces more than 300 different character sets
and transliterates files between almost any pair.  When exact
transliteration are not possible, it gets rid of offending characters
or fall back on approximations.  The recode program is a handy
front-end to the library.

%prep
%setup
%patch

%build

%install

%files
%doc /usr/doc/packages/recode/README
%doc /usr/doc
/usr/bin/recode
/usr/include/recode.h
/usr/include/recodext.h
/usr/info/recode.info*.gz
/usr/lib/librecode.a
/usr/lib/librecode.la
/usr/man/man1/recode.1.gz
/usr/share/locale/*/LC_MESSAGES/recode.mo
