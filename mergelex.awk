# Conversion of files between different charsets and usages.
# Copyright (C) 1990, 1993, 1994 Free Software Foundation, Inc.
# Francois Pinard <pinard@iro.umontreal.ca>, 1990.
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
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# This awk script merges several lex sources intended for GNU recode.

# At beginning, one temporary file is initialized for each section.

BEGIN {
  SECTION1 = "merged1.tmp"
  SECTION2 = "merged2.tmp"
  SECTION3 = "merged3.tmp"

  print "/* This file is generated automatically by mergelex.awk.  */"

  print ""						>SECTION1
  print "%{"						>SECTION1
  print "#include \"recode.h\""				>SECTION1
  print "#ifdef USE_FPUTC"				>SECTION1
  print "#define output(ch) fputc (ch, yyout)"		>SECTION1
  print "#else"						>SECTION1
  print "#define output(ch) putc (ch, yyout)"		>SECTION1
  print "#endif"					>SECTION1
  print "%}"						>SECTION1

  print "%%"						>SECTION2
  print "<<EOF>>			{ return 1; }"	>SECTION2

  print "%%"						>SECTION3
  print ""						>SECTION3
  print "#ifndef yywrap"				>SECTION3
  print "int"						>SECTION3
  print "yywrap (void)"					>SECTION3
  print "{"						>SECTION3
  print "  return 1;"					>SECTION3
  print "}"						>SECTION3
  print "#endif /* not yywrap */"			>SECTION3
}

# A `/* Step name: NAME.  */' line indicates the start of a new file.
# Generate an interface routine.  The step name is saved for broketed
# prefixes in rules.

$1 == "/*" && $2 == "Step" && $3 == "name:" && $5 == "*/" {
  section = 1
  step_name = $4
  sub (".$", "", step_name)
  print ""						>SECTION3
  print "static int"					>SECTION3
  print "file_" step_name " (const STEP *step, " \
	"FILE *input_file, FILE *output_file)" 		>SECTION3
  print "{"						>SECTION3
  print "  yy_init = 1;"				>SECTION3
  print "  yyin = input_file;"				>SECTION3
  print "  yyout = output_file;"			>SECTION3
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

# In section 2, every non-empty line which does not start with white
# space has to be a lex rule, which is then prefixed by a start name
# derived from the step name.  However, a %% in column 1 starts
# section 3, which contains pure C code, which is copied verbatim.

/^$/ && section == 2 {
  print							>SECTION2
  next
}

/^[ 	]/ && section == 2 {
  print							>SECTION2
  next
}

/^%%/ && section == 2 {
  section = 3
  next
}

section == 2 {
  print "<" step_name ">" $0				>SECTION2
  next
}

section == 3 {
  print							>SECTION3
  next
}

# At end, all three temporary files are reread and output, followed by
# the generated interfaces: one routine for each step name.

END {
  close (SECTION1)
  while (getline <SECTION1)
    print
  close (SECTION1)

  close (SECTION2)
  while (getline <SECTION2)
    print
  close (SECTION2)

  close (SECTION3)
  while (getline <SECTION3)
    print
  close (SECTION3)
}

