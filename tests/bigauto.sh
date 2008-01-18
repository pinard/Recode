#! /bin/sh
# Let recode do its huge autoconsistency check.

echo 1>&2 '==============================================='
echo 1>&2 "The \`bigauto' test may take hours.  Be patient."
echo 1>&2 '==============================================='

export LANGUAGE; LANGUAGE=C
export LANG; LANG=C
export LC_ALL; LC_ALL=C

recode -l | sed 's/ .*//' | egrep -v '^(/|flat)' > namelist

while read before; do
  test "$before" = Texinfo ||
    while read after; do
      test "$before" = "$after" ||
	echo "recode </dev/null -v $* '$before..$after'"
    done < namelist
done < namelist | sh 2>&1

rm namelist
