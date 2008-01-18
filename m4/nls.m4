# Select gettext and choose translations to install.
# François Pinard <pinard@iro.umontreal.ca>, 1998.

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

    AC_CHECK_HEADERS(locale.h)
    AC_CHECK_FUNCS(setlocale)
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

  fi])
