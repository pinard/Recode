#							-*- shell-script -*-

AT_DEFINE(TEST_METHOD,
[AT_SETUP($1)

AT_CHECK(
[set -e
ifelse($6, filter,
[recode --quiet --force --sequence=$5 < $2 $3..$4 \
| recode --quiet --force --sequence=$5 $4..$3 > data],
$6, squash,
[cp $2 data
chmod +w data
recode --quiet --force --sequence=$5 $3..$4 data
recode --quiet --force --sequence=$5 $4..$3 data])
diff $2 data
set +e
])

AT_CLEANUP(data)
])

TEST_METHOD(texte:texte through memory while filtering,
  $at_top_srcdir/COPYING, texte, texte, memory, filter)

TEST_METHOD(texte:texte through memory while squashing,
  $at_top_srcdir/COPYING, texte, texte, memory, squash)

TEST_METHOD(texte:texte through files while filtering,
  $at_top_srcdir/COPYING, texte, texte, files, filter)

TEST_METHOD(texte:texte through files while squashing,
  $at_top_srcdir/COPYING, texte, texte, files, squash)

TEST_METHOD(texte:texte through pipe while filtering,
  $at_top_srcdir/COPYING, texte, texte, pipe, filter)

TEST_METHOD(texte:texte through pipe while squashing,
  $at_top_srcdir/COPYING, texte, texte, pipe, squash)

TEST_METHOD(texte:latin1 through memory while filtering,
  $at_top_srcdir/COPYING, texte, latin1, memory, filter)

TEST_METHOD(texte:latin1 through memory while squashing,
  $at_top_srcdir/COPYING, texte, latin1, memory, squash)

TEST_METHOD(texte:latin1 through files while filtering,
  $at_top_srcdir/COPYING, texte, latin1, files, filter)

TEST_METHOD(texte:latin1 through files while squashing,
  $at_top_srcdir/COPYING, texte, latin1, files, squash)

TEST_METHOD(texte:latin1 through pipe while filtering,
  $at_top_srcdir/COPYING, texte, latin1, pipe, filter)

TEST_METHOD(texte:latin1 through pipe while squashing,
  $at_top_srcdir/COPYING, texte, latin1, pipe, squash)

TEST_METHOD(texte:bangbang through memory while filtering,
  $at_top_srcdir/COPYING, texte, bangbang, memory, filter)

TEST_METHOD(texte:bangbang through memory while squashing,
  $at_top_srcdir/COPYING, texte, bangbang, memory, squash)

TEST_METHOD(texte:bangbang through files while filtering,
  $at_top_srcdir/COPYING, texte, bangbang, files, filter)

TEST_METHOD(texte:bangbang through files while squashing,
  $at_top_srcdir/COPYING, texte, bangbang, files, squash)

TEST_METHOD(texte:bangbang through pipe while filtering,
  $at_top_srcdir/COPYING, texte, bangbang, pipe, filter)

TEST_METHOD(texte:bangbang through pipe while squashing,
  $at_top_srcdir/COPYING, texte, bangbang, pipe, squash)

TEST_METHOD(texte:ibmpc through memory while filtering,
  $at_top_srcdir/COPYING, texte, ibmpc, memory, filter)

TEST_METHOD(texte:ibmpc through memory while squashing,
  $at_top_srcdir/COPYING, texte, ibmpc, memory, squash)

TEST_METHOD(texte:ibmpc through files while filtering,
  $at_top_srcdir/COPYING, texte, ibmpc, files, filter)

TEST_METHOD(texte:ibmpc through files while squashing,
  $at_top_srcdir/COPYING, texte, ibmpc, files, squash)

TEST_METHOD(texte:ibmpc through pipe while filtering,
  $at_top_srcdir/COPYING, texte, ibmpc, pipe, filter)

TEST_METHOD(texte:ibmpc through pipe while squashing,
  $at_top_srcdir/COPYING, texte, ibmpc, pipe, squash)

TEST_METHOD(texte:iconqnx through memory while filtering,
  $at_top_srcdir/COPYING, texte, iconqnx, memory, filter)

TEST_METHOD(texte:iconqnx through memory while squashing,
  $at_top_srcdir/COPYING, texte, iconqnx, memory, squash)

TEST_METHOD(texte:iconqnx through files while filtering,
  $at_top_srcdir/COPYING, texte, iconqnx, files, filter)

TEST_METHOD(texte:iconqnx through files while squashing,
  $at_top_srcdir/COPYING, texte, iconqnx, files, squash)

TEST_METHOD(texte:iconqnx through pipe while filtering,
  $at_top_srcdir/COPYING, texte, iconqnx, pipe, filter)

TEST_METHOD(texte:iconqnx through pipe while squashing,
  $at_top_srcdir/COPYING, texte, iconqnx, pipe, squash)

TEST_METHOD(ascii-bs:ebcdic through memory while filtering,
  $at_top_srcdir/COPYING, ascii-bs, ebcdic, memory, filter)

TEST_METHOD(ascii-bs:ebcdic through memory while squashing,
  $at_top_srcdir/COPYING, ascii-bs, ebcdic, memory, squash)

TEST_METHOD(ascii-bs:ebcdic through files while filtering,
  $at_top_srcdir/COPYING, ascii-bs, ebcdic, files, filter)

TEST_METHOD(ascii-bs:ebcdic through files while squashing,
  $at_top_srcdir/COPYING, ascii-bs, ebcdic, files, squash)

TEST_METHOD(ascii-bs:ebcdic through pipe while filtering,
  $at_top_srcdir/COPYING, ascii-bs, ebcdic, pipe, filter)

TEST_METHOD(ascii-bs:ebcdic through pipe while squashing,
  $at_top_srcdir/COPYING, ascii-bs, ebcdic, pipe, squash)
