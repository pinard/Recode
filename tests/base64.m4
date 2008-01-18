#							-*- shell-script -*-

AT_SETUP(single lines to Base64)
dnl      ----------------------

AT_CHECK(
[( while read line; do echo $line | recode ../64 || exit 1; done
) < $at_srcdir/dumps-data
], 0,
[Cg==
YQo=
YWIK
YWJjCg==
YWJjZAo=
YWJjZGVmZ2hpCg==
YWJjZGVmZ2hpamtsbW5vcHFycwo=
YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd6eXpBQkMK
YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd6eXpBQkNERUZHSElKS0xNCg==
YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd6eXpBQkNERUZHSElKS0xNTk9QUVJTVFVWVwo=
YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd6eXpBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWjAxMjM0
NTYK
])

AT_CLEANUP()

AT_SETUP(block of lines to Base64)
dnl      ------------------------

AT_CHECK(
[recode /../64 < $at_srcdir/dumps-data
], 0,
[CmEKYWIKYWJjCmFiY2QKYWJjZGVmZ2hpCmFiY2RlZmdoaWprbG1ub3BxcnMKYWJjZGVmZ2hpamts
bW5vcHFyc3R1dnd6eXpBQkMKYWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd6eXpBQkNERUZHSElKS0xN
CmFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3enl6QUJDREVGR0hJSktMTU5PUFFSU1RVVlcKYWJjZGVm
Z2hpamtsbW5vcHFyc3R1dnd6eXpBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWjAxMjM0NTYK
])

AT_CLEANUP()

AT_SETUP(single lines to Base64 and back)
dnl      -------------------------------

AT_CHECK(
[set -e
while read line; do
  echo $line > input
  echo $line > output
  recode /../64 output
  recode /64../ output
  diff input output
done < $at_srcdir/dumps-data
set +e
])

AT_CLEANUP(input output)

AT_SETUP(block of lines to Base64 and back)
dnl      ---------------------------------

AT_CHECK(
[set -e
cp $at_srcdir/dumps-data output
chmod +w output
recode /../64 output
recode /64../ output
diff $at_srcdir/dumps-data output
set +e
])

AT_CLEANUP(output)
