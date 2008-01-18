#							-*- shell-script -*-

AT_SETUP(single lines from UTF-7)
dnl      -----------------------

AT_CHECK(
[set -e
while read line; do
  echo "$line" | recode ../,u7..u6/x2
done < $at_srcdir/utf7-data
set +e
], ,
[0xFEFF, 0x000A
0xFEFF, 0x0041, 0x2262, 0x0391, 0x002E, 0x000A
0xFEFF, 0x0048, 0x0069, 0x0020, 0x004D, 0x006F, 0x006D, 0x0020,
0x263A, 0x0021, 0x000A
0xFEFF, 0x65E5, 0x672C, 0x8A9E, 0x000A
0xFEFF, 0x0049, 0x0074, 0x0065, 0x006D, 0x0020, 0x0033, 0x0020,
0x0069, 0x0073, 0x0020, 0x00A3, 0x0031, 0x002E, 0x000A
])

AT_CLEANUP()

AT_SETUP(single lines from UTF-7 and back)
dnl      --------------------------------

AT_CHECK(
[set -e
while read line; do
  echo "$line" > input
  echo "$line" > output
  recode u7..u6/x2 output
  recode u6/x2..u7 output
  diff input output
done < $at_srcdir/utf7-data
set +e
])

AT_CLEANUP(input output)
