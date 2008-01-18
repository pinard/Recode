## ---------------------------------------- ##
## See if compiling for Microsoft systems.  ##
## ---------------------------------------- ##

AC_DEFUN(fp_OS_MICROSOFT,
[AC_CACHE_CHECK([for MSDOS, Win95 or WinNT], fp_cv_os_microsoft,
[if test -n "$COMSPEC"; then
  # MSDOS or Win95
   fp_cv_os_microsoft=yes
elif test -n "$ComSpec"; then
  # WinNT
  fp_cv_os_microsoft=yes
else
  fp_cv_os_microsoft=no
fi
])])
