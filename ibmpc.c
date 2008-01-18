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

#define DOS_EOF 26		/* old end of file */
#define DOS_CR 13		/* carriage return */
#define DOS_LF 10		/* line feed */

/* Correspondance for IBM PC ruler graphics characters into ASCII graphics
   approximations.  The current principles are:

   - Single horizontal rulers are made up of dashes.
   - Double horizontal rulers are made up of equal signs.
   - Both single and double vertical are made up with `|'.
   - Both upper corners are rounded down with periods.
   - Lower corners are rounded up with grave/acute accent on left/right.
   - Other crossing rulers are approximated with plus signs, with exceptions
     for double horizontal ruler crossings but not at corners: they are
     equal signs inside a table, and `|' at left or right margin.
*/

unsigned char convert_rulers[48] =
  {
    '#',			/* 176 */
    '#',			/* 177 */
    '#',			/* 178 */
    '|',			/* 179 */
    '+',			/* 180 */
    '|',			/* 181 */
    '+',			/* 182 */
    '.',			/* 183 */
    '.',			/* 184 */
    '|',			/* 185 */
    '|',			/* 186 */
    '.',			/* 187 */
    '\'',			/* 188 */
    '\'',			/* 189 */
    '\'',			/* 190 */
    '.',			/* 191 */
    '`',			/* 192 */
    '+',			/* 193 */
    '+',			/* 194 */
    '+',			/* 195 */
    '-',			/* 196 */
    '+',			/* 197 */
    '|',			/* 198 */
    '+',			/* 199 */
    '`',			/* 200 */
    '.',			/* 201 */
    '=',			/* 202 */
    '=',			/* 203 */
    '|',			/* 204 */
    '=',			/* 205 */
    '=',			/* 206 */
    '=',			/* 207 */
    '+',			/* 208 */
    '=',			/* 209 */
    '+',			/* 210 */
    '`',			/* 211 */
    '`',			/* 212 */
    '.',			/* 213 */
    '.',			/* 214 */
    '+',			/* 215 */
    '=',			/* 216 */
    '\'',			/* 217 */
    '.',			/* 218 */
    '#',			/* 219 */
    '#',			/* 220 */
    '#',			/* 221 */
    '#',			/* 222 */
    '#',			/* 223 */
  };

/* Data for IBM PC to ISO Latin-1 code conversions.  */

static KNOWN_PAIR known_pairs[] =
  {
    { 20, 182},			/* pilcrow sign */
    { 21, 167},			/* section sign */

    {128, 199},			/* capital letter C with cedilla */
    {129, 252},			/* small letter u with diaeresis */
    {130, 233},			/* small letter e with acute accent */
    {131, 226},			/* small letter a with circumflex accent */
    {132, 228},			/* small letter a with diaeresis */
    {133, 224},			/* small letter a with grave accent */
    {134, 229},			/* small letter a with ring above */
    {135, 231},			/* small letter c with cedilla */
    {136, 234},			/* small letter e with circumflex accent */
    {137, 235},			/* small letter e with diaeresis */
    {138, 232},			/* small letter e with grave accent */
    {139, 239},			/* small letter i with diaeresis */
    {140, 238},			/* small letter i with circumflex accent */
    {141, 236},			/* small letter i with grave accent */
    {142, 196},			/* capital letter A with diaeresis */
    {143, 197},			/* capital letter A with ring above */
    {144, 201},			/* capital letter E with acute accent */
    {145, 230},			/* small ligature a with e */
    {146, 198},			/* capital ligature A with E */
    {147, 244},			/* small letter o with circumblex accent */
    {148, 246},			/* small letter o with diaeresis */
    {149, 242},			/* small letter o with grave accent */
    {150, 251},			/* small letter u with circumflex accent */
    {151, 249},			/* small letter u with grave accent */
    {152, 255},			/* small letter y with diaeresis */
    {153, 214},			/* capital letter O with diaeresis */
    {154, 220},			/* capital letter U with diaeresis */
    {155, 162},			/* cent sign */
    {156, 163},			/* pound sign */
    {157, 165},			/* yen sign */

    {160, 225},			/* small letter a with acute accent */
    {161, 237},			/* small letter i with acute accent */
    {162, 243},			/* small letter o with acute accent */
    {163, 250},			/* small letter u with acute accent */
    {164, 241},			/* small letter n with tilde */
    {165, 209},			/* capital letter N with tilde */
    {166, 170},			/* feminine ordinal indicator */
    {167, 186},			/* masculine ordinal indicator */
    {168, 191},			/* inverted question mark */

    {170, 172},			/* not sign */
    {171, 189},			/* vulgar fraction one half */
    {172, 188},			/* vulgar fraction one quarter */
    {173, 161},			/* inverted exclamation mark */
    {174, 171},			/* left angle quotation mark */
    {175, 187},			/* right angle quotation mark */

    {225, 223},			/* small german letter sharp s */

    {230, 181},			/* small Greek letter mu micro sign */

    {241, 177},			/* plus-minus sign */

    {246, 247},			/* division sign */

    {248, 176},			/* degree sign */

    {250, 183},			/* middle dot */

    {253, 178},			/* superscript two */

    {255, 160},			/* no-break space */
  };
#define NUMBER_OF_PAIRS (sizeof (known_pairs) / sizeof (KNOWN_PAIR))

static int
file_latin1_ibmpc (const STEP *step, FILE *input_file, FILE *output_file)
{
  int reversible;		/* reversibility of recoding */
  const unsigned char *table;	/* one to one conversion table */
  const char *const *table2;	/* one to many conversion table */
  int input_char;		/* current character */

  reversible = 1;
  if (strict_mapping)
    {
      table2 = step->one_to_many;
      while (input_char = getc (input_file), input_char != EOF)
	if (input_char == '\n')
	  {
	    putc (DOS_CR, output_file);
	    putc (DOS_LF, output_file);
	  }
	else if (table2[input_char])
	  putc (*table2[input_char], output_file);
	else
	  reversible = 0;
    }
  else
    {
      table = step->one_to_one;
      while (input_char = getc (input_file), input_char != EOF)
	if (input_char == '\n')
	  {
	    putc (DOS_CR, output_file);
	    putc (DOS_LF, output_file);
	  }
	else
	  putc (table[input_char], output_file);
    }
  return reversible;
}

static int
file_ibmpc_latin1 (const STEP *step, FILE *input_file, FILE *output_file)
{
  int reversible;		/* reversibility of recoding */
  const unsigned char *table;	/* one to one conversion table */
  const char *const *table2;	/* one to many conversion table */
  int input_char;		/* current character */

  reversible = 1;
  if (strict_mapping)
    {
      table2 = step->one_to_many;
      input_char = getc (input_file);
      while (1)
	switch (input_char)
	  {
	  case EOF:
	    return reversible;

	  case DOS_EOF:
	    return 0;

	  case DOS_CR:
	    input_char = getc (input_file);
	    if (input_char == DOS_LF)
	      {
		putc ('\n', output_file);
		input_char = getc (input_file);
	      }
	    else if (table2[DOS_CR])
	      putc (*table2[DOS_CR], output_file);
	    else
	      reversible = 0;
	    break;

	  case DOS_LF:
	    reversible = 0;
	    /* Fall through.  */

	  default:
	    if (table2[input_char])
	      putc (*table2[input_char], output_file);
	    else
	      reversible = 0;
	    input_char = getc (input_file);
	  }
    }
  else
    {
      table = step->one_to_one;
      input_char = getc (input_file);
      while (1)
	switch (input_char)
	  {
	  case EOF:
	    return reversible;

	  case DOS_EOF:
	    return 0;

	  case DOS_CR:
	    input_char = getc (input_file);
	    if (input_char == DOS_LF)
	      {
		putc ('\n', output_file);
		input_char = getc (input_file);
	      }
	    else
	      putc (table[DOS_CR], output_file);
	    break;

	  case DOS_LF:
	    reversible = 0;
	    /* Fall through.  */

	  default:
	    putc (table[input_char], output_file);
	    input_char = getc (input_file);
	  }
    }
}

static void
init_latin1_ibmpc (STEP *step)
{
  complete_pairs (step, 1, known_pairs, NUMBER_OF_PAIRS, 1);
  step->file_recode = file_latin1_ibmpc;
}

static void
init_ibmpc_latin1 (STEP *step)
{
  unsigned char *table;

  complete_pairs (step, 1, known_pairs, NUMBER_OF_PAIRS, 0);
  step->file_recode = file_ibmpc_latin1;

  /* FIXME: Allow ascii_graphics even with strict_mapping.  Reported by
     David E. A. Wilson <david@osiris.cs.uow.edu.au>.  */

  if (ascii_graphics)
    {
      table = (unsigned char *) xmalloc (256);
      memcpy (table, step->one_to_one, 256);
      memcpy (table + 176, convert_rulers, 48);
      free ((void *) step->one_to_one);
      step->one_to_one = table;
    }
}

void
module_ibmpc (void)
{
  declare_step ("Latin-1", "IBM-PC", ONE_TO_MANY, init_latin1_ibmpc, NULL);
  declare_step ("IBM-PC", "Latin-1",
		strict_mapping ? MANY_TO_MANY : MANY_TO_ONE,
		init_ibmpc_latin1, NULL);

#if 0
  declare_alias ("IBM-PC", "ibm437");
#endif
}
