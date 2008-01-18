#!/bin/sh
#                                                      -*- coding: latin-1 -*-
# Validation suite for the Free `recode' program and library.
# Copyright © 1998, 1999, 2000 Progiciels Bourbeau-Pinard inc.
# François Pinard <pinard@iro.umontreal.ca>, 1998.

# Still many parts of `recode' are not exercised by the test suite.  A few
# FIXME's, below, are used to list tests that we would need.  Do you feel
# like contributing new tests?  If you do, you may tell your intent to
# `recode-forum@iro.umontreal.ca', so no two people work at the same thing.

AT_INIT(recode)

# Tell the installer that `make bigtest' exists.
echo
echo "WARNING: The \`bigauto' test will be skipped, as it takes a long time to"
echo "         complete.  To launch it, get into the build \`tests/' directory"
echo "         and do either \`make bigtest' or \`make bigtest-strict'.  The"
echo "         later forces \`-s' on all \`recode' calls."

echo
echo 'Charset listings.'
echo

AT_INCLUDE(names.m4)
AT_INCLUDE(lists.m4)

echo
echo 'Individual surfaces.'
echo

# FIXME: tests for endline and permut.

AT_INCLUDE(dumps.m4)
AT_INCLUDE(base64.m4)
AT_INCLUDE(quoted.m4)

echo
echo 'Individual charsets.'
echo

# FIXME: tests for atarist, next, ebcdic, tables, applemac, ibmpc, iconqnx,
# cdcnos, bangbang, ascii, flat, html, latex, texinfo, texte and utf16.

AT_INCLUDE(african.m4)
AT_INCLUDE(combine.m4)
AT_INCLUDE(testdump.m4)
AT_INCLUDE(utf7.m4)
AT_INCLUDE(utf8.m4)

echo
echo 'Inter-step mechanics.'
echo

AT_INCLUDE(methods.m4)
