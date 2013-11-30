# Configure template for Recode.
# Copyright (C) 1994-1999, 2000, 2001 Free Software Foundation, Inc.
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT(recode, 3.7-beta2)
AC_CONFIG_SRCDIR(src/recode.c)
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR(m4)
LT_INIT
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AC_FUNC_ALLOCA
AC_ISC_POSIX
AC_C_CONST
AC_C_INLINE
AC_PROG_LEX

AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h locale.h memory.h stdbool.h stddef.h stdlib.h string.h unistd.h utime.h)
AC_TYPE_SIGNAL
jm_STRUCT_UTIMBUF

AM_ICONV
LIBS="$LIBICONV $LIBS"

AC_CHECK_FUNCS(dup2 mbrtowc rename setlocale strerror strndup wcrtomb)
AC_REPLACE_FUNCS(strtol strtoul)
AC_FUNC_ALLOCA
AC_FUNC_VPRINTF

jm_FUNC_MALLOC
jm_FUNC_REALLOC

AC_CANONICAL_HOST
jm_LANGINFO_CODESET
jm_GLIBC21

AM_WITH_DMALLOC

AM_GNU_GETTEXT_VERSION(0.18)
AM_GNU_GETTEXT(external)

AH_BOTTOM([
#include "cleaner.h"
])

AC_OUTPUT(Makefile cleaner.h contrib/Makefile doc/Makefile lib/Makefile
          po/Makefile.in src/Makefile tests/Makefile tests/setup.py)
