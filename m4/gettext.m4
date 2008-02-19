# Select gettext and choose translations to install.	-*- shell-script -*-
# François Pinard <pinard@iro.umontreal.ca>, 1998.

# Here is a check list about how one could use this macro.
# - In the build directory, execute `make distclean'.
# - Rename `po/' to `i18n/'.
# - Recursively delete directory `intl/'.
# - Add the following files:
#    . `i18n/Makefile.am'
#    . `m4/gettext.m4'
#    . `lib/gettext.c'
#    . `lib/gettext.h'.
# - In top level `configure.in':
#    . use `fp_WITH_GETTEXT'.
#    . delete `AM_GNU_GETTEXT' and any `AC_LINK_FILES' for $nls variables.
#    . add `i18n/Makefile' to `AC_OUTPUT'.
#    . delete `intl/Makefile' and `po/Makefile.in' from `AC_OUTPUT'.
# - In top level `Makefile.am':
#    . add `i18n' in SUBDIRS, and remove `intl' and `po'.
#    . define `POTFILES' (peek at previous `i18n/POTFILES.in).
#    . add `$(srcdir)/stamp-pot' to `all-local'.
#    . add rule for `$(srcdir)/stamp-pot'.
# - In top level `acconfig.h':
#    . document `HAVE_DCGETTEXT' and `LOCALEDIR'.
#    . delete documentation for `HAVE_CATGETS'.
# - In `lib/Makefile.am':
#    . add `gettext.c' to `EXTRA_DIST'.
#    . add `gettext.h' to `noinstl_HEADERS'.
#    . add definitions for `localdir' and `aliaspath'.
#    . add special rules for `gettext.o' and `gettext._o'.
# - In `m4/Makefile.am':
#    . add `gettext.m4' to `EXTRA_DIST'.
# - In `src/Makefile.am':
#    . delete `-I/..intl' from `INCLUDES'.
#    . delete `@INTLLIBS@' from `LDADD'.
#    . delete `localedir' and its references.
# - Clean up directory `i18n/':
#    . delete `Makefile.in.in' and `POTFILES.in'.
#    . delete all `*.gmo' files, `cat-id-tbl.c' and `stamp-cat-id'.
# - In the top level distribution directory:
#    . run `aclocal -I m4'.
#    . run `autoreconf'.
# - In the build directory:
#    . run `$top_srcdir/configure'.
#    . run `make check'.

AC_DEFUN(fp_WITH_GETTEXT, [

  AC_MSG_CHECKING(whether NLS is wanted)
  AC_ARG_ENABLE(nls,
    [  --disable-nls           disallow Native Language Support],
    enable_nls=$enableval, enable_nls=yes)
  AC_MSG_RESULT($enable_nls)
  use_nls=$enable_nls
  AM_CONDITIONAL(USE_NLS, test $use_nls = yes)

  if test $enable_nls = yes; then
    AC_DEFINE(ENABLE_NLS)

    AC_ARG_WITH(catgets,
      [  --with-catgets          say that catgets is not supported],
      [AC_MSG_WARN([catgets not supported, --with-catgets ignored])])

    AC_CHECK_FUNCS(gettext)
    AC_CHECK_LIB(intl, gettext, :)
    if test $ac_cv_lib_intl_gettext$ac_cv_func_gettext != nono; then
      AC_MSG_CHECKING(whether the included gettext is preferred)
      AC_ARG_WITH(included-gettext,
	[  --without-included-gettext avoid our provided version of gettext],
	with_included_gettext=$withval, with_included_gettext=yes)
      AC_MSG_RESULT($with_included_gettext)
      if test $with_included_gettext$ac_cv_func_gettext = nono; then
        LIBS="$LIBS -lintl"
      fi
    else
      with_included_gettext=yes
    fi
    if test $with_included_gettext = yes; then
      LIBOBJS="$LIBOBJS gettext.o"
      AC_DEFINE(HAVE_GETTEXT)
      AC_DEFINE(HAVE_DCGETTEXT)
    else
      AC_CHECK_HEADERS(libintl.h)
      AC_CHECK_FUNCS(dcgettext gettext)
    fi

    AC_CHECK_HEADERS(argz.h limits.h locale.h malloc.h string.h unistd.h sys/param.h)
    AC_FUNC_MMAP
    AC_CHECK_FUNCS(getcwd mempcpy munmap setlocale stpcpy strcasecmp strchr strdup __argz_count __argz_stringify __argz_next)
    AM_LC_MESSAGES

    if test -z "$ALL_LINGUAS"; then
      AC_MSG_WARN(This package does not install translations yet.)
    else
      ac_items="$ALL_LINGUAS"
      for ac_item in $ac_items; do
	ALL_POFILES="$ALL_POFILES $ac_item.po"
	ALL_MOFILES="$ALL_MOFILES $ac_item.mo"
      done
    fi
    AC_SUBST(ALL_LINGUAS)
    AC_SUBST(ALL_POFILES)
    AC_SUBST(ALL_MOFILES)

    AC_MSG_CHECKING(which translations to install)
    if test -z "$LINGUAS"; then
      ac_print="$ALL_LINGUAS"
      MOFILES="$ALL_MOFILES"
    else
      ac_items="$LINGUAS"
      for ac_item in $ac_items; do
	case "$ALL_LINGUAS" in
	  *$ac_item*)
	    ac_print="$ac_print $ac_item"
	    MOFILES="$MOFILES $ac_item.mo"
	    ;;
	esac
      done
    fi
    AC_SUBST(MOFILES)
    if test -z "$ac_print"; then
      AC_MSG_RESULT(none)
    else
      AC_MSG_RESULT($ac_print)
    fi

    if test "x$prefix" = xNONE; then
      AC_DEFINE_UNQUOTED(LOCALEDIR, "$ac_default_prefix/share/locale")
    else
      AC_DEFINE_UNQUOTED(LOCALEDIR, "$prefix/share/locale")
    fi
  fi])
