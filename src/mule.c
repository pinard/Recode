/* Conversion of files between different charsets and surfaces.
   Copyright © 1997, 98, 99 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1997.

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
transform_latin_mule (RECODE_CONST_STEP step, RECODE_TASK task,
		      unsigned prefix)
{
  int character;

  while (character = get_byte (task), character != EOF)
    {
      if (!IS_ASCII (character))
	put_byte (prefix, task);
      put_byte (character, task);
    }
  TASK_RETURN (task);
}

static bool
transform_mule_latin (RECODE_CONST_STEP step, RECODE_TASK task,
		      unsigned prefix)
{
  int character;

  while (character = get_byte (task), character != EOF)
    if (IS_ASCII (character))
      put_byte (character, task);
    else if ((character & MASK (8)) == prefix)
      {
	character = get_byte (task);

	while ((character & MASK (8)) == prefix)
	  {
	    /* This happens in practice, sometimes, that Emacs goes a bit
	       berzerk and generates strings of prefix characters.  Remove
	       all succeeding prefixes in a row.  This is irreversible.  */
	    RETURN_IF_NOGO (RECODE_NOT_CANONICAL, step, task);
	    character = get_byte (task);
	  }

	if (character == EOF)
	  {
	    RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
	    break;
	  }

	if (IS_ASCII (character))
	  RETURN_IF_NOGO (RECODE_NOT_CANONICAL, step, task);
	put_byte (character, task);
      }
    else
      RETURN_IF_NOGO (RECODE_UNTRANSLATABLE, step, task);

  TASK_RETURN (task);
}

#define TRANSFORM_LATIN(To_mule, From_mule, Prefix) \
									\
  static bool								\
  To_mule (RECODE_CONST_STEP step, RECODE_TASK task)	\
  {									\
    return transform_latin_mule (step, task, Prefix);			\
  }									\
									\
  static bool								\
  From_mule (RECODE_CONST_STEP step, RECODE_TASK task)	\
  {									\
    return transform_mule_latin (step, task, Prefix);			\
  }

TRANSFORM_LATIN (transform_latin1_mule, transform_mule_latin1, 129)
TRANSFORM_LATIN (transform_latin2_mule, transform_mule_latin2, 130)

/*-------------------.
| Register methods.  |
`-------------------*/

bool
module_mule (RECODE_OUTER outer)
{
  return
    declare_single (outer, "ISO-8859-1", "Mule",
		    outer->quality_byte_to_variable,
		    NULL, transform_latin1_mule)
    && declare_single (outer, "Mule", "ISO-8859-1",
		       outer->quality_variable_to_byte,
		       NULL, transform_mule_latin1)
    && declare_single (outer, "ISO-8859-2", "Mule",
		       outer->quality_byte_to_variable,
		       NULL, transform_latin2_mule)
    && declare_single (outer, "Mule", "ISO-8859-2",
		       outer->quality_variable_to_byte,
		       NULL, transform_mule_latin2);
}
