# Conversion of files between different charsets and surfaces.
# Copyright © 1990, 1993, 1994, 1995, 1997, 1999 Free Software Foundation, Inc.
# François Pinard <pinard@iro.umontreal.ca>, 1990.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

# This awk script merges several lex sources intended for `recode'.
# It requires Flex 2.5 or later.

# At beginning, one temporary file is initialized for each section.

BEGIN {
  SECTION1 = "merged1.tmp"
  SECTION2 = "merged2.tmp"
  SECTION3 = "merged3.tmp"

  print "/* This file is generated automatically by mergelex.awk.  */"

  print ""						>SECTION1
  print "%option noyywrap"				>SECTION1
  print "%{"						>SECTION1
  print "#include \"common.h\""				>SECTION1
  print "static RECODE_CONST_REQUEST request;"		>SECTION1
  print "static RECODE_TASK task;"			>SECTION1
  print "%}"						>SECTION1

  print "%%"						>SECTION2
  print "<<EOF>>			{ return 1; }"	>SECTION2

  print "%%"						>SECTION3
}

# A `/* Step name: NAME.  */' line indicates the start of a new file.
# Generate an interface routine.  The step name is saved for broketed
# prefixes in rules.

$1 == "/*" && $2 == "Step" && $3 == "name:" && $5 == "*/" {
  section = 1
  step_name = $4
  sub ("\.$", "", step_name)
  print ""						>SECTION3
  print "static bool"					>SECTION3
  print "transform_" step_name \
        " (RECODE_CONST_STEP step," \
	" RECODE_TASK task_argument)"			>SECTION3
  print "{"						>SECTION3
  print "  task = task_argument;"			>SECTION3
  print "  request = task->request;"			>SECTION3
  print "  yy_init = 1;"				>SECTION3
  print "  yyin = task->input.file;"			>SECTION3
  print "  yyout = task->output.file;"			>SECTION3
  print "  BEGIN " step_name ";"			>SECTION3
  print "  return yylex ();"				>SECTION3
  print "}"						>SECTION3
  next
}

# Remove block C comments in section 1, except the very first.  It is
# assumed that, when a /* comment starts in column 1, there is no code
# following the closing */ on its line.  Also, remove all white lines.

/^\/\*/ && section == 1 {
  while (!match ($0, /\*\//)) {
    if (!comment_done) {
      print
    }
    getline
  }
  if (!comment_done) {
    print
  }
  comment_done = 1
  next
}

/^[ 	]*$/ && section == 1 {
  next
}

# A %% in column 1 signals the beginning of lex rules.

/^%%/ && section == 1 {
  print "%x " step_name					>SECTION1
  section = 2
  print ""						>SECTION2
  print "<" step_name ">{"				>SECTION2
  next
}

# A %{ comment in column 1 signals the start of a C code section ended
# by a %} line.

/^%\{/ {
  c_code = 1
  print ""						>SECTION1
  print							>SECTION1
  next
}

/^%\}/ {
  print							>SECTION1
  print ""						>SECTION1
  c_code = 0
  next
}

c_code {
  print							>SECTION1
  next
}

# Section 1 declarations are studied and copied, once each.
# Conflicting declaractions are reported at beginning of output.

/^[^ 	]/ && section == 1 {
  if ($1 in rules) {
    if ($0 != rules[$1]) {
      print "** Conflicting definition: " $0
    }
  }
  else {
    rules[$1] = $0
    print						>SECTION1
  }
  next
}

# In section 2, a %% in column 1 starts section 3, which contains pure
# C code, which is copied verbatim.

/^%%/ && section == 2 {
  print "}"						>SECTION2
  section = 3
  next
}

section == 2 {
  print							>SECTION2
  next
}

section == 3 {
  print							>SECTION3
  next
}

# At end, all three temporary files are reread and output, followed by
# the generated interfaces: one routine for each step name.

END {
  if (section == 2)
    print "}"						>SECTION2

  close (SECTION1)
  close (SECTION2)
  close (SECTION3)

  while (getline <SECTION1)
    print
  close (SECTION1)

  while (getline <SECTION2)
    print
  close (SECTION2)

  while (getline <SECTION3)
    print
  close (SECTION3)
}
