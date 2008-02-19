%define rcsid      $Id: recode.spec,v 1.5 2000/06/29 13:52:54 lebel Exp $
%define nam	   recode
%define ver	   3.5c
%define rel  	   1
%define sysconfdir /etc
%define prefix	   /usr

Summary: The `recode' library converts files between character sets and usages.
Name:		%nam
Version:	%ver
Release:	%rel
Copyright:	GPL
Vendor:		François Pinard et al.
Group:		Applications/Text
Source:		http://www.iro.umontreal.ca/contrib/recode/%{nam}-%{ver}.tar.gz
URL:		http://www.iro.umontreal.ca/~pinard/
BuildRoot:	/var/tmp/%{nam}-%{ver}-root
Docdir:		%{prefix}/doc

%description
The `recode' library converts files between character sets and usages.
The library recognises or produces nearly 150 different character sets
and is able to transliterate files between almost any pair.  When
exact transliteration are not possible, it may get rid of the
offending characters or fall back on approximations.  Most RFC 1345
character sets are supported.  The `recode' program is a handy
front-end to the library.

The `recode' program and library have been written by François Pinard.
It is an evolving package, and specifications might change in future
releases.  Option `-f' is now fairly implemented, yet not fully.

%package devel
Summary:	Libraries and include files for developing applications using the `recode' library.
Group:		Development/Libraries

%description devel
This package provides the necessary development libraries and include
files to allow you to develop applications using the `recode' libraries.

%changelog 
* Thu Jun 29 2000 David Lebel <lebel@lebel.org>
- Initial public release of this SPEC file.

%prep
%setup -q

%build
if [ ! -f configure ]; then
  CFLAGS="$RPM_OPT_FLAGS" ./autogen.sh	\
	--prefix=%{prefix} --sysconfdir=%{sysconfdir}
else
  CFLAGS="$RPM_OPT_FLAGS" ./configure	\
	--prefix=%{prefix} --sysconfdir=%{sysconfdir}
fi

make

%install
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

make install-strip \
	prefix=$RPM_BUILD_ROOT%{prefix} \
	sysconfdir=$RPM_BUILD_ROOT%{sysconfdir}

%clean
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

%post
if ! grep %{prefix}/lib /etc/ld.so.conf > /dev/null ; then
  echo "%{prefix}/lib" >> /etc/ld.so.conf
fi

[ -x /sbin/ldconfig ] && /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-, bin, bin)

%doc ABOUT-NLS AUTHORS BACKLOG COPYING COPYING-LIB ChangeLog INSTALL NEWS README THANKS TODO
%{prefix}/bin/*
%{prefix}/lib/*.so*
%{prefix}/info/recode*
%{prefix}/man/man1/*
%{prefix}/share/locale/*

%files devel
%defattr(0555, bin, bin)

%{prefix}/include/*
%{prefix}/lib/*.a
%{prefix}/lib/*.la
