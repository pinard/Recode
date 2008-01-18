#							-*- shell-script -*-

AT_SETUP(that combine does not crash)
dnl      ---------------------------

AT_CHECK(recode co..l1 < /dev/null, 0)

AT_CLEANUP()
