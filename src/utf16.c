/* Conversion of files between different charsets and surfaces.
   Copyright © 1996, 97, 98, 99 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1996.

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
transform_ucs4_utf16 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  int character;
  unsigned value;
  unsigned chunk;

  if (get_ucs4 (&value, step, task))
    {
      if (task->byte_order_mark)
	put_ucs2 (BYTE_ORDER_MARK, task);

      while (true)
	{
	  if (value & ~MASK (16))
	    if (value < (1 << 16 | 1 << 20))
	      {
		/* Double UCS-2 character.  */

		value -= 1 << 16;
		put_ucs2 (0xD800 | (MASK (10) & value >> 10), task);
		put_ucs2 (0xDC00 | (MASK (10) & value), task);
	      }
	    else
	      {
		RETURN_IF_NOGO (RECODE_UNTRANSLATABLE, step, task);
		put_ucs2 (REPLACEMENT_CHARACTER, task);
	      }
	  else
	    {
	      /* Single UCS-2 character.  */

	      if (value >= 0xD800 && value < 0xE000)
		RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, step, task);
	      put_ucs2 (value, task);
	    }

	  if (!get_ucs4 (&value, step, task))
	    break;
	}
    }

  TASK_RETURN (task);
}

static bool
transform_utf16_ucs4 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  unsigned value;

  if (get_ucs2 (&value, step, task))

    while (true)
      if (value >= 0xD800 && value < 0xE000)
	if (value < 0xDC00)
	  {
	    unsigned chunk;

	    if (!get_ucs2 (&chunk, step, task))
	      break;

	    if (chunk >= 0xDC00 && chunk < 0xE000)
	      {
		put_ucs4 ((((1 << 16) + ((value - 0xD800) << 10))
			   | (chunk - 0xDC00)),
			  task);
		if (!get_ucs2 (&value, step, task))
		  break;
	      }
	    else
	      {
		/* Discard the first chunk if the pair is invalid.  */

		RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
		value = chunk;
	      }
	  }
	else
	  {
	    /* Discard a second chunk when presented first.  */

	    RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
	    if (!get_ucs2 (&value, step, task))
	      break;
	  }
      else
	{
	  put_ucs4 (value, task);
	  if (!get_ucs2 (&value, step, task))
	    break;
	}

  TASK_RETURN (task);
}

static bool
transform_ucs2_utf16 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  unsigned value;

  /* This function does nothing, besides checking that the number of input
     bytes is even, and that special UTF-16 values do not appear.  */

  while (get_ucs2 (&value, step, task))
    {
      if (value >= 0xD800 && value < 0xE000)
	RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, step, task);
      put_ucs2 (value, task);
    }

  TASK_RETURN (task);
}

static bool
transform_utf16_ucs2 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  unsigned value;

  /* This function does almost nothing, besides checking that the input is
     valid UTF-16, and replacing UTF-16 extended values with the replacement
     character.  */

  if (get_ucs2 (&value, step, task))
    {
      if (task->byte_order_mark)
	put_ucs2 (BYTE_ORDER_MARK, task);

      while (true)
	if (value >= 0xD800 && value < 0xE000)
	  if (value < 0xDC00)
	    {
	      unsigned chunk;

	      if (!get_ucs2 (&chunk, step, task))
		break;

	      if (chunk >= 0xDC00 && chunk < 0xE000)
		{
		  RETURN_IF_NOGO (RECODE_UNTRANSLATABLE, step, task);
		  put_ucs2 (REPLACEMENT_CHARACTER, task);
		  if (!get_ucs2 (&value, step, task))
		    break;
		}
	      else
		{
		  /* Discard the first chunk if the pair is invalid.  */

		  RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
		  value = chunk;
		}
	    }
	  else
	    {
	      /* Discard a second chunk when presented first.  */

	      RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
	      if (!get_ucs2 (&value, step, task))
		break;
	    }
	else
	  {
	    put_ucs2 (value, task);
	    if (!get_ucs2 (&value, step, task))
	      break;
	  }
    }

  TASK_RETURN (task);
}

bool
module_utf16 (RECODE_OUTER outer)
{
  return
    declare_single (outer, "ISO-10646-UCS-4", "UTF-16",
		    outer->quality_variable_to_variable,
		    NULL, transform_ucs4_utf16)
    && declare_single (outer, "UTF-16", "ISO-10646-UCS-4",
		       outer->quality_variable_to_variable,
		       NULL, transform_utf16_ucs4)
    && declare_single (outer, "ISO-10646-UCS-2", "UTF-16",
		       outer->quality_variable_to_variable,
		       NULL, transform_ucs2_utf16)
    && declare_single (outer, "UTF-16", "ISO-10646-UCS-2",
		       outer->quality_variable_to_variable,
		       NULL, transform_utf16_ucs2)

    && declare_alias (outer, "Unicode", "UTF-16")
    && declare_alias (outer, "TF-16", "UTF-16")
    && declare_alias (outer, "u6", "UTF-16");
}
