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

#define DOS_CR 13		/* carriage return */
#define DOS_LF 10		/* line feed */
#define DOS_EOF 26		/* old end of file */

#define ESCAPE 25		/* escape for diacritic application */
#define ENDLINE 30		/* end-line code for QNX */

#define TRANSLATE_AND_BREAK(c2, c3) \
  putc (ESCAPE, output_file); \
  putc (c2, output_file); \
  putc (c3, output_file); \
  input_char = getc (input_file); \
  break;

static int
file_ibmpc_iconqnx (const STEP *step, FILE *input_file, FILE *output_file)
{
  int reversible;		/* reversibility of recoding */
  int input_char;

  reversible = 1;
  input_char = getc (input_file);
  while (1)
    switch (input_char)
      {
      case EOF:
	return reversible;

      case DOS_EOF:
	return 0;

      case 133: TRANSLATE_AND_BREAK ('A', 'a');
      case 138: TRANSLATE_AND_BREAK ('A', 'e');
      case 151: TRANSLATE_AND_BREAK ('A', 'u');
      case 130: TRANSLATE_AND_BREAK ('B', 'e');
      case 144: TRANSLATE_AND_BREAK ('B', 'E');
      case 131: TRANSLATE_AND_BREAK ('C', 'a');
      case 136: TRANSLATE_AND_BREAK ('C', 'e');
      case 140: TRANSLATE_AND_BREAK ('C', 'i');
      case 147: TRANSLATE_AND_BREAK ('C', 'o');
      case 150: TRANSLATE_AND_BREAK ('C', 'u');
      case 137: TRANSLATE_AND_BREAK ('H', 'e');
      case 139: TRANSLATE_AND_BREAK ('H', 'i');
      case 129: TRANSLATE_AND_BREAK ('H', 'u');
      case 135: TRANSLATE_AND_BREAK ('K', 'c');
      case 128: TRANSLATE_AND_BREAK ('K', 'C');

      case DOS_CR:
	input_char = getc (input_file);
	if (input_char == DOS_LF)
	  {
	    putc (ENDLINE, output_file);
	    input_char = getc (input_file);
	  }
	else
	  putc (DOS_CR, output_file);
	break;

      case ENDLINE:
      case ESCAPE:
	reversible = 0;
	/* Fall through.  */

      default:
	putc (input_char, output_file);
	input_char = getc (input_file);
      }
}

static int
file_iconqnx_ibmpc (const STEP *step, FILE *input_file, FILE *output_file)
{
  int reversible;		/* reversibility of recoding */
  int input_char;		/* current character */

  reversible = 1;
  input_char = getc (input_file);
  while (1)
    switch (input_char)
      {
      case EOF:
	return reversible;

      case ENDLINE:
	putc (DOS_CR, output_file);
	putc (DOS_LF, output_file);
	input_char = getc (input_file);
	break;

      case DOS_CR:
	input_char = getc (input_file);
	if (input_char == DOS_LF)
	  reversible = 0;
	putc (DOS_CR, output_file);
	break;

      case ESCAPE:
	input_char = getc (input_file);
	switch (input_char)
	  {
	  case 'A':
	    input_char = getc (input_file);
	    switch (input_char)
	      {
	      case 'a': input_char = 133; break;
	      case 'e': input_char = 138; break;
	      case 'u': input_char = 151; break;

	      default:
		reversible = 0;
		putc (ESCAPE, output_file);
		putc ('A', output_file);
		if (input_char == EOF)
		  return 0;
	      }
	    break;

	  case 'B':
	    input_char = getc (input_file);
	    switch (input_char)
	      {
	      case 'e': input_char = 130; break;
	      case 'E': input_char = 144; break;

	      default:
		reversible = 0;
		putc (ESCAPE, output_file);
		putc ('B', output_file);
		if (input_char == EOF)
		  return 0;
	      }
	    break;

	  case 'C':
	    input_char = getc (input_file);
	    switch (input_char)
	      {
	      case 'a': input_char = 131; break;
	      case 'e': input_char = 136; break;
	      case 'i': input_char = 140; break;
	      case 'o': input_char = 147; break;
	      case 'u': input_char = 150; break;

	      default:
		reversible = 0;
		putc (ESCAPE, output_file);
		putc ('C', output_file);
		if (input_char == EOF)
		  return 0;
	      }
	    break;

	  case 'H':
	    input_char = getc (input_file);
	    switch (input_char)
	      {
	      case 'e': input_char = 137; break;
	      case 'i': input_char = 139; break;
	      case 'u': input_char = 129; break;

	      default:
		reversible = 0;
		putc (ESCAPE, output_file);
		putc ('H', output_file);
		if (input_char == EOF)
		  return 0;
	      }
	    break;

	  case 'K':
	    input_char = getc (input_file);
	    switch (input_char)
	      {
	      case 'c': input_char = 135; break;
	      case 'C': input_char = 128; break;

	      default:
		reversible = 0;
		putc (ESCAPE, output_file);
		putc ('K', output_file);
		if (input_char == EOF)
		  return 0;
	      }
	    break;

	  default:
	    reversible = 0;
	    putc (ESCAPE, output_file);
	    if (input_char == EOF)
	      return 0;
	  }
	/* Fall through.  */

      default:
	putc (input_char, output_file);
	input_char = getc (input_file);
      }
}

void
module_iconqnx (void)
{
  declare_step ("IBM-PC", "Icon-QNX", MANY_TO_MANY, NULL, file_ibmpc_iconqnx);
  declare_step ("Icon-QNX", "IBM-PC", MANY_TO_MANY, NULL, file_iconqnx_ibmpc);

  declare_alias ("QNX", "Icon-QNX");
}
