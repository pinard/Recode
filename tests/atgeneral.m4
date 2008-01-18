divert(-1)						-*- shell-script -*-
# `m4' macros used in building test suites.
# Copyright © 1998, 1999 Progiciels Bourbeau-Pinard inc.
# François Pinard <pinard@iro.umontreal.ca>, 1998.

changequote([, ])

define(AT_DEFINE, defn([define]))
define(AT_EVAL, defn([eval]))
define(AT_FORMAT, defn([format]))
define(AT_INCLUDE, defn([include]))
define(AT_SHIFT, defn([shift]))
define(AT_UNDEFINE, defn([undefine]))

undefine([define])
undefine([eval])
undefine([format])
undefine([include])
undefine([shift])
undefine([undefine])

# AT_LINE

# Return the current file sans directory, a colon, and the current line.

AT_DEFINE(AT_LINE,
[patsubst(__file__, ^.*/\(.*\), \1):__line__])

# AT_INIT(PROGRAM, RELPATH)

# Begin testing suite, using PROGRAM to check version, and RELPATH as a
# relative path (usually `../src') to find executable binaries to test.
# RELPATH may be omitted; `.' is always added in front of the search path.

AT_DEFINE(AT_INIT,
[AT_DEFINE(AT_ordinal, 0)
. ./atconfig
# Snippet (3

at_usage="Usage: [$]0 [OPTION]...

  -e  Stop and inhibit normal clean up if a test of the full test suite fails
  -n  Do not redirect stdout and stderr and do not test their contents
  -s  Inhibit verbosity in debugging scripts, at generation or at execution
  -v  Force more detailed output, default for debugging scripts unless -s
  -x  Have the shell to trace command execution; implies options -a"

while test [$][#] -gt 0; do
  case "[$]1" in
    -e) at_stop_on_error=1; shift ;;
    -n) at_no_redirs=1; shift ;;
    -s) at_verbose=; at_silent=1; shift ;;
    -v) at_verbose=1; at_silent=; shift ;;
    -x) at_tracex=1; at_no_redirs=1; shift ;;
    *) echo 1>&2 "$at_usage"; exit 1 ;;
  esac
done

# In the testing suite, we only want to know if the test succeeded or failed.
# But in debugging scripts, we want more information, so we prefer `diff -u'
# to silent `cmp', even if it may happen that we compare binary files.
# Option `-u' might be less portable, so either change it or use GNU `diff'.

if test -n "$at_verbose"; then
  at_diff='diff -u'
else
  at_diff='cmp -s'
fi

# Each generated debugging script, containing a single test group, cleans
# up files at the beginning only, not at the end.  This is so we can repeat
# the script many times and browse left over files.  To cope with such left
# over files, the full test suite cleans up both before and after test groups.
# Snippet )3

if test -n "`$1 --version | sed -n s/$at_package.*$at_version/OK/p`"; then
  at_banner="Testing suite for $at_package, version $at_version"
  at_dashes=`echo $at_banner | sed s/./=/g`
  echo "$at_dashes"
  echo "$at_banner"
  echo "$at_dashes"
else
  echo '======================================================='
  echo 'ERROR: Not using the proper version, no tests performed'
  echo '======================================================='
  exit 1
fi

# Remove any debugging script resulting from a previous run.
rm -f debug-*.sh

at_failed_list=
at_ignore_count=0
divert(2)[]dnl

# Wrap up the testing suite with summary statistics.

rm -f at-check-line
at_fail_count=0
if test -z "$at_failed_list"; then
  if test "$at_ignore_count" = 0; then
    at_banner="All $at_test_count tests were successful"
  else
    at_banner="All $at_test_count tests were successful ($at_ignore_count ignored)"
  fi
else
  echo
  echo $at_n "Writing \`debug-NN.sh' scripts, NN =$at_c"
  for at_group in $at_failed_list; do
    echo $at_n " $at_group$at_c"
    ( echo '#!/bin/sh'
      sed -n "/^[#] Snippet (1/,/^[#] Snippet )1/p" atconfig
      test -z "$at_silent" && echo 'at_verbose=1'
      sed -n "/^[#] Snippet (2/,/^[#] Snippet )2/p" atconfig
      sed -n "/^[#] Snippet (3/,/^[#] Snippet )3/p" [$]0
      sed -n "/^[#] Snippet (c$at_group(/,/^[#] Snippet )c$at_group)/p" [$]0
      echo 'test -n "$at_verbose" \
        && echo "[$]0:1: =================================================="'
      sed -n "/^[#] Snippet (s$at_group(/,/^[#] Snippet )s$at_group)/p" [$]0
      echo 'exit 0'
    ) | grep -v '^[#] Snippet' > debug-$at_group.sh
    chmod +x debug-$at_group.sh
    at_fail_count=`expr $at_fail_count + 1`
  done
  echo ', done'
  if test -n "$at_stop_on_error"; then
    at_banner="ERROR: One of the tests failed, inhibiting subsequent tests"
  else
    at_banner="ERROR: Suite unsuccessful, $at_fail_count of $at_test_count tests failed"
  fi
fi
at_dashes=`echo $at_banner | sed s/./=/g`
echo
echo "$at_dashes"
echo "$at_banner"
echo "$at_dashes"

if test -n "$at_failed_list"; then
  if test -z "$at_silent"; then
    echo
    echo "Now, failed tests will be executed again, with more details..."
    echo
    for at_group in $at_failed_list; do
      ./debug-$at_group.sh
    done
  fi
  exit 1
fi

exit 0
divert[]dnl
])

# AT_SETUP(DESCRIPTION)

# Start a group of related tests, all to be executed in the same subshell.
# The group is testing what DESCRIPTION says.

AT_DEFINE(AT_SETUP,
[AT_DEFINE([AT_ordinal], AT_EVAL(AT_ordinal + 1))
pushdef([AT_group_description], [$1])
pushdef([AT_data_files], )
pushdef([AT_data_expout], )
pushdef([AT_data_experr], )
if test -z "$at_stop_on_error" || test -z "$at_failed_list"; then
divert(1)[]dnl
  echo AT_LINE > at-check-line
  if test -n "$at_verbose"; then
    echo 'testing AT_group_description'
    echo $at_n "     $at_c"
  fi
  echo $at_n "substr(AT_ordinal. $srcdir/AT_LINE                            , 0, 30)[]$at_c"
  (
[#] Snippet (s[]AT_ordinal[](

[#] The test group starts at `AT_LINE'.  An error occurred while
[#] testing AT_group_description.
])

# AT_CLEANUP(FILES)

# Complete a group of related tests, recursively remove those FILES
# created within the test.  There is no need to list stdout, stderr,
# nor files created with AT_DATA.

AT_DEFINE(AT_CLEANUP,
[[#] Snippet )s[]AT_ordinal[])
  )
  case [$]? in
    0) echo ok
       ;;
    77) echo "ignored near \``cat at-check-line`'"
        at_ignore_count=`expr $at_ignore_count + 1`
	;;
    *) echo "FAILED near \``cat at-check-line`'"
       at_failed_list="$at_failed_list AT_ordinal"
       ;;
  esac
  at_test_count=AT_ordinal
  if test -z "$at_stop_on_error" || test -z "$at_failed_list"; then
divert(0)[]dnl
[#] Snippet (c[]AT_ordinal[](

rm ifelse([AT_data_files$1], , [-f], [-rf[]AT_data_files[]ifelse($1, , , [ $1])]) stdout stderr[]AT_data_expout[]AT_data_experr
[#] Snippet )c[]AT_ordinal[])
undivert(1)[]dnl
    rm ifelse([AT_data_files$1], , [-f], [-rf[]AT_data_files[]ifelse($1, , , [ $1])]) stdout stderr[]AT_data_expout[]AT_data_experr
  fi
fi
popdef([AT_data_experr])
popdef([AT_data_expout])
popdef([AT_data_files])
popdef([AT_group_description])])

# AT_DATA(FILE, CONTENTS)

# Initialize an input data FILE with given CONTENTS, which should end with
# an end of line.

AT_DEFINE(AT_DATA,
[AT_DEFINE([AT_data_files], AT_data_files[ ]$1)
cat > $1 <<'EOF'
$2[]EOF
])

# AT_CHECK(COMMANDS, STATUS, STDOUT, STDERR)

# Execute a test by performing given shell COMMANDS.  These commands
# should normally exit with STATUS, while producing expected STDOUT and
# STDERR contents.  The special word `expout' for STDOUT means that file
# `expout' contents has been set to the expected stdout.  The special word
# `experr' for STDERR means that file `experr' contents has been set to
# the expected stderr.  STATUS is not checked if it is empty.

AT_DEFINE(AT_CHECK,
[test -n "$at_verbose" \
  && echo $srcdir'/AT_LINE: Testing AT_group_description'
echo AT_LINE > at-check-line
test -z "$at_no_redirs" && exec 5>&1 6>&2 1>stdout 2>stderr
test -n "$at_tracex" && set -x
$1
ifelse([$2], , , [test $? = $2 || exit 1])
test -n "$at_tracex" && set +x
if test -z "$at_no_redirs"; then
  exec 1>&5 2>&6
  ifelse([$3], , [test ! -s stdout || exit 1
], [$3], expout, [AT_DEFINE([AT_data_expout], [ expout])dnl
$at_diff expout stdout || exit 1
], [changequote({{, }})dnl
echo $at_n "patsubst({{$3}}, \([\"`$]\), \\\1)$at_c" | $at_diff - stdout || exit 1
changequote([, ])])dnl
  ifelse([$4], , [test ! -s stderr || exit 1
], [$4], experr, [AT_DEFINE([AT_data_experr], [ experr])dnl
$at_diff experr stderr || exit 1
], [changequote({{, }})dnl
echo $at_n "patsubst({{$4}}, \([\"`$]\), \\\1)$at_c" | $at_diff - stderr || exit 1
changequote([, ])])dnl
fi
])

divert[]dnl
