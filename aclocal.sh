#! /bin/sh

# This script is a work-alike of `aclocal' contained in the GNU automake
# package. It is needed because our aclocal.m4 must be generated from the
# non-gettext .m4 files in /usr/share/aclocal and from the gettext specific
# .m4 files in the local m4 directory.
# With   "aclocal --acdir=m4"
# we get an error: macro `AM_INIT_AUTOMAKE' not found in library
# With   "aclocal -I m4"
# we get an error: duplicated macro `AM_GNU_GETTEXT'
# The solution is to put all the .m4 files into a temporary directory.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

set -e

if test $# = 0; then
  echo "Usage: $0 aclocal-program [aclocal-options]" 1>&2
  exit 1
fi

ACLOCAL="$1"
shift

# Prepare temporary directory.
mkdir aclocal.tmp
trap "rm -rf aclocal.tmp; exit 1" 1 2 15

# First, copy the standard m4 files.
for f in `"$ACLOCAL" --print-ac-dir`/*.m4; do
  cp $f aclocal.tmp
done

# Then, copy the contents of any -I directories, overriding previously
# copied files of the same name.
options=""
last_was_I=no
for arg
do
  if test $last_was_I = yes; then
    for f in "$arg"/*.m4; do
      cp $f aclocal.tmp
    done
    last_was_I=no
  else
    case "$arg" in
      -I) last_was_I=yes;;
      *)  last_was_I=no options="$options $arg";;
    esac
  fi
done

# Now call `aclocal' for real.
"$ACLOCAL" --acdir=aclocal.tmp $options

# Clean up temporary directory.
rm -rf aclocal.tmp

