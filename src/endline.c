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

#define CR 13			/* carriage return */
#define LF 10			/* line feed */
#define OLD_EOF 26		/* oldish end of file */

static bool
transform_data_cr (RECODE_CONST_STEP step, RECODE_TASK task)
{
  bool strict = step->fallback_routine != reversibility;
  int character;

  while (character = get_byte (task), character != EOF)
    switch (character)
      {
      case '\n':
	put_byte (CR, task);
	break;

      case CR:
	if (!strict)
	  {
	    put_byte ('\n', task);
	    break;
	  }
	RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, step, task);
	/* Fall through.  */

      default:
	put_byte (character, task);
      }

  TASK_RETURN (task);
}

static bool
transform_cr_data (RECODE_CONST_STEP step, RECODE_TASK task)
{
  bool strict = step->fallback_routine != reversibility;
  int character;

  while (character = get_byte (task), character != EOF)
    switch (character)
      {
      case CR:
	put_byte ('\n', task);
	break;

      case '\n':
	if (!strict)
	  {
	    put_byte (CR, task);
	    break;
	  }
	RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, step, task);
	/* Fall through.  */

      default:
	put_byte (character, task);
      }

  TASK_RETURN (task);
}

static bool
transform_data_crlf (RECODE_CONST_STEP step, RECODE_TASK task)
{
  int character = get_byte (task);

  while (character != EOF)
    switch (character)
      {
      case '\n':
	put_byte (CR, task);
	put_byte (LF, task);
	character = get_byte (task);
	break;

      case CR:
	character = get_byte (task);
	if (character == LF)
	  RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, step, task);
	put_byte (CR, task);
	break;

      case OLD_EOF:
	RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, step, task);
	/* Fall through.  */

      default:
	put_byte (character, task);
	character = get_byte (task);
      }

  TASK_RETURN (task);
}

static bool
transform_crlf_data (RECODE_CONST_STEP step, RECODE_TASK task)
{
  int character = get_byte (task);

  while (character != EOF)
    switch (character)
      {
      case OLD_EOF:
	RETURN_IF_NOGO (RECODE_NOT_CANONICAL, step, task);
	TASK_RETURN (task);

      case CR:
	character = get_byte (task);
	if (character == LF)
	  {
	    put_byte ('\n', task);
	    character = get_byte (task);
	  }
	else
	  put_byte (CR, task);
	break;

      case LF:
	RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, step, task);
	/* Fall through.  */

      default:
	put_byte (character, task);
	character = get_byte (task);
      }

  TASK_RETURN (task);
}

bool
module_endline (RECODE_OUTER outer)
{
  return
    declare_single (outer, "data", "CR",
		    outer->quality_byte_to_byte,
		    NULL, transform_data_cr)
    && declare_single (outer, "CR", "data",
		       outer->quality_byte_to_byte,
		       NULL, transform_cr_data)
    && declare_single (outer, "data", "CR-LF",
		       outer->quality_byte_to_variable,
		       NULL, transform_data_crlf)
    && declare_single (outer, "CR-LF", "data",
		       outer->quality_variable_to_byte,
		       NULL, transform_crlf_data)

    && declare_alias (outer, "cl", "CR-LF");
}
