## Replacement for AC_PROG_LEX and AC_DECL_YYTEXT
## by Alexandre Oliva <oliva@dcc.unicamp.br>
## Modified by Akim Demaille so that only flex is legal

# serial 2

dnl ad_AC_PROG_FLEX
dnl Look for flex or missing, then run AC_PROG_LEX and AC_DECL_YYTEXT
AC_DEFUN(ad_AC_PROG_FLEX,
[AC_CHECK_PROGS(LEX, flex, missing)
if test "$LEX" = missing; then
  LEX="\$(top_srcdir)/$ac_aux_dir/missing flex"
  LEX_OUTPUT_ROOT=lex.yy
  AC_SUBST(LEX_OUTPUT_ROOT)dnl
else
  AC_PROG_LEX
  AC_DECL_YYTEXT
fi])
