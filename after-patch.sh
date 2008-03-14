#!/bin/sh

# After "patch", "git clone" or "git checkout", file timestamps may be a
# bit random, and unless you have all maintainer tools handy, it can put
# you in some misery.  This script makes all timestamps to be identical.

if test -f configure.ac; then
  find . -name .git -prune -o -type f -print | xargs touch -r configure.ac
else
  echo >2 "You should first cd to the distribution directory."
  exit 1
fi
