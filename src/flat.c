/* Conversion of files between different charsets and surfaces.
   Copyright © 1990, 93, 94, 97, 98, 99 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1988.

   The `recode' Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License
   as published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The `recode' Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the `recode' Library; see the file `COPYING.LIB'.
   If not, write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA.  */

#include "common.h"

static bool
transform_ascii_flat (RECODE_CONST_STEP step, RECODE_TASK task)
{
  int input_char;		/* current character */
  int temp_char;		/* look ahead character */

  input_char = get_byte (task);
  while (true)
    switch (input_char)
      {
      case EOF:
	TASK_RETURN (task);

      case '\n':
      case '\t':
	put_byte (input_char, task);
	input_char = get_byte (task);
	break;

      case '\b':
	input_char = get_byte (task);
	switch (input_char)
	  {
	  case '\'':
	  case '`':
	  case '^':
	  case '"':
	  case '~':
	  case ',':
	  case '_':
	    RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, step, task);
	    input_char = get_byte (task);
	    break;

	  default:
	    put_byte ('\b', task);
	  }
	break;

      case '\'':
      case '`':
      case '^':
      case '"':
      case '~':
      case ',':
      case '_':
	temp_char = get_byte (task);
	if (temp_char == '\b')
	  {
	    RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, step, task);
	    input_char = get_byte (task);
	  }
	else
	  {
	    put_byte (input_char, task);
	    input_char = temp_char;
	  }
	break;

      default:
	if (!IS_ASCII (input_char))
	  {
	    RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, step, task);
	    put_byte ('M', task);
	    put_byte ('-', task);
	    input_char &= MASK (7);
	  }
	if (input_char < ' ' || input_char == MASK (7))
	  {
	    RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, step, task);
	    put_byte ('^', task);
	    input_char ^= (1 << 6);
	  }
	put_byte (input_char, task);
	input_char = get_byte (task);
      }
}

bool
module_flat (RECODE_OUTER outer)
{
  if (!declare_single (outer, "ASCII-BS", "flat",
		       outer->quality_variable_to_variable,
		       NULL, transform_ascii_flat))
    return false;

  return true;
}
