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

static int
file_ascii_flat (const STEP *step, FILE *input_file, FILE *output_file)
{
  int input_char;		/* current character */
  int temp_char;		/* look ahead character */

  input_char = getc (input_file);
  while (1)
    switch (input_char)
      {
      case EOF:
	return 0;

      case '\n':
      case '\t':
	putc (input_char, output_file);
	input_char = getc (input_file);
	break;

      case '\b':
	input_char = getc (input_file);
	switch (input_char)
	  {
	  case '\'':
	  case '`':
	  case '^':
	  case '"':
	  case '~':
	  case ',':
	  case '_':
	    input_char = getc (input_file);
	    break;

	  default:
	    putc ('\b', output_file);
	  }
	break;

      case '\'':
      case '`':
      case '^':
      case '"':
      case '~':
      case ',':
      case '_':
	temp_char = getc (input_file);
	if (temp_char == '\b')
	  input_char = getc (input_file);
	else
	  {
	    putc (input_char, output_file);
	    input_char = temp_char;
	  }
	break;

      default:
	if (input_char & (1 << 7))
	  {
	    putc ('M', output_file);
	    putc ('-', output_file);
	    input_char &= ~(~0 << 7);
	  }
	if (input_char < ' ' || input_char == ~(~0 << 7))
	  {
	    putc ('^', output_file);
	    input_char ^= (1 << 6);
	  }
	putc (input_char, output_file);
	input_char = getc (input_file);
      }
}

void
module_flat (void)
{
  declare_step ("ASCII-BS", "flat", MANY_TO_MANY, NULL, file_ascii_flat);
}
