/* Conversion of files between different charsets and usages.
   Copyright (C) 1990, 1993, 1994 Free Software Foundation, Inc.
   Francois Pinard <pinard@iro.umontreal.ca>, 1988.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "recode.h"

static const char *const translation_table[128] =
  {
    "^5",			/*   0 */
    "^6",			/*   1 */
    "^7",			/*   2 */
    "^8",			/*   3 */
    "^9",			/*   4 */
    "^+",			/*   5 */
    "^-",			/*   6 */
    "^*",			/*   7 */
    "^/",			/*   8 */
    "^(",			/*   9 */
    "\n",			/*  10, would have been "^)" */
    "^$",			/*  11 */
    "^=",			/*  12 */
    "^ ",			/*  13 */
    "^,",			/*  14 */
    "^.",			/*  15 */
    "^#",			/*  16 */
    "^[",			/*  17 */
    "^]",			/*  18 */
    "^%",			/*  19 */
    "^\"",			/*  20 */
    "^_",			/*  21 */
    "^!",			/*  22 */
    "^&",			/*  23 */
    "^'",			/*  24 */
    "^?",			/*  25 */
    "^<",			/*  26 */
    "^>",			/*  27 */
    "^@",			/*  28 */
    "^\\",			/*  29 */
    "^^",			/*  30 */
    "^;",			/*  31 */
    " ",			/*  32 */
    "!",			/*  33 */
    "\"",			/*  34 */
    "#",			/*  35 */
    "$",			/*  36 */
    "%",			/*  37 */
    "&",			/*  38 */
    "'",			/*  39 */
    "(",			/*  40 */
    ")",			/*  41 */
    "*",			/*  42 */
    "+",			/*  43 */
    ",",			/*  44 */
    "-",			/*  45 */
    ".",			/*  46 */
    "/",			/*  47 */
    "0",			/*  48 */
    "1",			/*  49 */
    "2",			/*  50 */
    "3",			/*  51 */
    "4",			/*  52 */
    "5",			/*  53 */
    "6",			/*  54 */
    "7",			/*  55 */
    "8",			/*  56 */
    "9",			/*  57 */
    "@D",			/*  58 */
    ";",			/*  59 */
    "<",			/*  60 */
    "=",			/*  61 */
    ">",			/*  62 */
    "?",			/*  63 */
    "@A",			/*  64 */
    "A",			/*  65 */
    "B",			/*  66 */
    "C",			/*  67 */
    "D",			/*  68 */
    "E",			/*  69 */
    "F",			/*  70 */
    "G",			/*  71 */
    "H",			/*  72 */
    "I",			/*  73 */
    "J",			/*  74 */
    "K",			/*  75 */
    "L",			/*  76 */
    "M",			/*  77 */
    "N",			/*  78 */
    "O",			/*  79 */
    "P",			/*  80 */
    "Q",			/*  81 */
    "R",			/*  82 */
    "S",			/*  83 */
    "T",			/*  84 */
    "U",			/*  85 */
    "V",			/*  86 */
    "W",			/*  87 */
    "X",			/*  88 */
    "Y",			/*  89 */
    "Z",			/*  90 */
    "[",			/*  91 */
    "\\",			/*  92 */
    "]",			/*  93 */
    "@B",			/*  94 */
    "_",			/*  95 */
    "@G",			/*  96 */
    "^A",			/*  97 */
    "^B",			/*  98 */
    "^C",			/*  99 */
    "^D",			/* 100 */
    "^E",			/* 101 */
    "^F",			/* 102 */
    "^G",			/* 103 */
    "^H",			/* 104 */
    "^I",			/* 105 */
    "^J",			/* 106 */
    "^K",			/* 107 */
    "^L",			/* 108 */
    "^M",			/* 109 */
    "^N",			/* 110 */
    "^O",			/* 111 */
    "^P",			/* 112 */
    "^Q",			/* 113 */
    "^R",			/* 114 */
    "^S",			/* 115 */
    "^T",			/* 116 */
    "^U",			/* 117 */
    "^V",			/* 118 */
    "^W",			/* 119 */
    "^X",			/* 120 */
    "^Y",			/* 121 */
    "^Z",			/* 122 */
    "^0",			/* 123 */
    "^1",			/* 124 */
    "^2",			/* 125 */
    "^3",			/* 126 */
    "^4",			/* 127 */
  };

static void
init_ascii_cdcnos (STEP *step)
{
  const char **table;
  int counter;

  table = (const char **) xmalloc (256 * sizeof (char *));
  for (counter = 0; counter < 128; counter++)
    table[counter] = translation_table[counter];
  for (; counter < 256; counter++)
    table[counter] = NULL;

  step->one_to_many = table;
}

/* Previous obsolete lex code:

@A			{ output ('@'); }
@B			{ output ('^'); }
@D			{ output (':'); }
@G			{ output ('`'); }

\^" "			{ output ( 13); }
\^!			{ output ( 22); }
\^\"			{ output ( 20); }
\^#			{ output ( 16); }
\^$			{ output ( 11); }
\^\%			{ output ( 19); }
\^&			{ output ( 23); }
\^'			{ output ( 24); }
\^\(			{ output (  9); }
\^\)			{ output ( 10); }
\^\*			{ output (  7); }
\^\+			{ output (  5); }
\^\,			{ output ( 14); }
\^-			{ output (  6); }
\^\.			{ output ( 15); }
\^\/			{ output (  8); }

\^0			{ output ('{'); }
\^1			{ output ('|'); }
\^2			{ output ('}'); }
\^3			{ output ('~'); }
\^4			{ output (127); }

\^5			{ output (  0); }
\^6			{ output (  1); }
\^7			{ output (  2); }
\^8			{ output (  3); }
\^9			{ output (  4); }

\^;			{ output ( 31); }
\^<			{ output ( 26); }
\^=			{ output ( 12); }
\^>			{ output ( 27); }
\^?			{ output ( 25); }
\^@			{ output ( 28); }

\^[A-Z]			{ output (yytext[1]-'A'+'a'); }

\^\[			{ output ( 17); }
\^\\			{ output ( 29); }
\^]			{ output ( 18); }
\^\^			{ output ( 30); }
\^_			{ output ( 21); }

\^[a-z]			{ output (yytext[1]); }

*/

static int
file_cdcnos_ascii (const STEP *step, FILE *input_file, FILE *output_file)
{
  int reversible;		/* reversibility of recoding */
  int input_char;		/* current character */

  reversible = 1;
  while (input_char = getc (input_file), input_char != EOF)
    {
      switch (input_char)
	{
        case '@':
	  switch ((input_char = getc (input_file)))
	    {
	    case 'A': case 'a': input_char = '@'; break;
	    case 'B': case 'b': input_char = '^'; break;
	    case 'D': case 'd': input_char = ':'; break;
	    case 'G': case 'g': input_char = '`'; break;

	    default:
	      reversible = 0;
	      putc ('@', output_file);
	      if (input_char == EOF)
		return 0;
	    }
	  break;

	case '^':
	  input_char = getc (input_file);
	  if (input_char >= 'A' && input_char <= 'Z')
	    input_char += 'a' - 'A';
	  else if (input_char < 'a' || input_char > 'z')
	    switch (input_char)
	      {
	      case ' ': input_char = 13; break;
	      case '!': input_char = 22; break;
	      case '"': input_char = 20; break;
	      case '#': input_char = 16; break;
	      case '$': input_char = 11; break;
	      case '%': input_char = 19; break;
	      case '&': input_char = 23; break;
	      case '\'': input_char = 24; break;
	      case '(': input_char = 9; break;

	      case ')':
		reversible = 0;
		input_char = '\n'; /* 10 */
		break;

	      case '*': input_char = 7; break;
	      case '+': input_char = 5; break;
	      case ',': input_char = 14; break;
	      case '-': input_char = 6; break;
	      case '.': input_char = 15; break;
	      case '/': input_char = 8; break;
	      case '0': input_char = '{'; break;
	      case '1': input_char = '|'; break;
	      case '2': input_char = '}'; break;
	      case '3': input_char = '~'; break;
	      case '4': input_char = 127; break;
	      case '5': input_char = 0; break;
	      case '6': input_char = 1; break;
	      case '7': input_char = 2; break;
	      case '8': input_char = 3; break;
	      case '9': input_char = 4; break;
	      case ';': input_char = 31; break;
	      case '<': input_char = 26; break;
	      case '=': input_char = 12; break;
	      case '>': input_char = 27; break;
	      case '?': input_char = 25; break;
	      case '@': input_char = 28; break;
	      case '[': input_char = 17; break;
	      case '\\': input_char = 29; break;
	      case ']': input_char = 18; break;
	      case '^': input_char = 30; break;
	      case '_': input_char = 21; break;

	      default:
		reversible = 0;
	        putc ('^', output_file);
	        if (input_char == EOF)
		  return 0;
	      }
	  break;
	}
      putc (input_char, output_file);
    }
  return reversible;
}

void
module_cdcnos (void)
{
  declare_step ("ASCII-BS", "CDC-NOS", ONE_TO_MANY, init_ascii_cdcnos,
		file_one_to_many);
  declare_step ("CDC-NOS", "ASCII-BS", MANY_TO_ONE, NULL, file_cdcnos_ascii);

  declare_alias ("NOS", "CDC-NOS");
}
