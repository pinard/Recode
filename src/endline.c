/* Conversion of files between different charsets and surfaces.
   Copyright © 1990,93,94, 1997-1999, 2000-2001 Free Software Foundation, Inc.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1988.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the `recode' Library; see the file `COPYING.LIB'.
   If not, write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA.  */

#include "common.h"

#define CR 13			/* carriage return */
#define LF 10			/* line feed */


/* Conversions for character unit RECODE_UNIT_1.  */

static bool
transform_data1_cr (RECODE_SUBTASK subtask)
{
  bool strict = subtask->step->fallback_routine != reversibility;
  int character;

  while (character = get_byte (subtask), character != EOF)
    switch (character)
      {
      case '\n':
	put_byte (CR, subtask);
	break;

      case CR:
	if (!strict)
	  {
	    put_byte ('\n', subtask);
	    break;
	  }
	RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, subtask);
	/* Fall through.  */

      default:
	put_byte (character, subtask);
      }

  SUBTASK_RETURN (subtask);
}

static bool
transform_cr_data1 (RECODE_SUBTASK subtask)
{
  bool strict = subtask->step->fallback_routine != reversibility;
  int character;

  while (character = get_byte (subtask), character != EOF)
    switch (character)
      {
      case CR:
	put_byte ('\n', subtask);
	break;

      case '\n':
	if (!strict)
	  {
	    put_byte (CR, subtask);
	    break;
	  }
	RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, subtask);
	/* Fall through.  */

      default:
	put_byte (character, subtask);
      }

  SUBTASK_RETURN (subtask);
}

static bool
transform_data1_crlf (RECODE_SUBTASK subtask)
{
  int character = get_byte (subtask);

  while (character != EOF)
    switch (character)
      {
      case '\n':
	put_byte (CR, subtask);
	put_byte (LF, subtask);
	character = get_byte (subtask);
	break;

      case CR:
	character = get_byte (subtask);
	if (character == LF)
	  RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, subtask);
	put_byte (CR, subtask);
	break;

      default:
	put_byte (character, subtask);
	character = get_byte (subtask);
      }

  SUBTASK_RETURN (subtask);
}

static bool
transform_crlf_data1 (RECODE_SUBTASK subtask)
{
  int character = get_byte (subtask);

  while (character != EOF)
    switch (character)
      {
      case CR:
	character = get_byte (subtask);
	if (character == LF)
	  {
	    put_byte ('\n', subtask);
	    character = get_byte (subtask);
	  }
	else
	  put_byte (CR, subtask);
	break;

      case LF:
	RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, subtask);
	/* Fall through.  */

      default:
	put_byte (character, subtask);
	character = get_byte (subtask);
      }

  SUBTASK_RETURN (subtask);
}


/* Conversions for character unit RECODE_UNIT_2BE.  */

static bool
get_ucs2be (unsigned int *value, RECODE_SUBTASK subtask)
{
  int byte1 = get_byte (subtask);
  if (byte1 != EOF)
    {
      int byte2 = get_byte (subtask);
      if (byte2 != EOF)
	{
	  *value = ((MASK (8) & byte1) << 8) | (MASK (8) & byte2);
	  return true;
	}
    }
  return false;
}

static void
put_ucs2be (unsigned int value, RECODE_SUBTASK subtask)
{
  put_byte (MASK (8) & (value >> 8), subtask);
  put_byte (MASK (8) & value, subtask);
}

#define BOM_DECLARATIONS /* nothing */
#define BOM_HANDLING /* nothing */
#define GET get_ucs2be
#define PUT put_ucs2be
#define TRANSFORM_NL_CR transform_data2be_cr
#define TRANSFORM_CR_NL transform_cr_data2be
#define TRANSFORM_NL_CRLF transform_data2be_crlf
#define TRANSFORM_CRLF_NL transform_crlf_data2be
#include "endline.h"


/* Conversions for character unit RECODE_UNIT_2LE.  */

static bool
get_ucs2le (unsigned int *value, RECODE_SUBTASK subtask)
{
  int byte1 = get_byte (subtask);
  if (byte1 != EOF)
    {
      int byte2 = get_byte (subtask);
      if (byte2 != EOF)
	{
	  *value = (MASK (8) & byte1) | ((MASK (8) & byte2) << 8);
	  return true;
	}
    }
  return false;
}

static void
put_ucs2le (unsigned int value, RECODE_SUBTASK subtask)
{
  put_byte (MASK (8) & value, subtask);
  put_byte (MASK (8) & (value >> 8), subtask);
}

#define BOM_DECLARATIONS /* nothing */
#define BOM_HANDLING /* nothing */
#define GET get_ucs2le
#define PUT put_ucs2le
#define TRANSFORM_NL_CR transform_data2le_cr
#define TRANSFORM_CR_NL transform_cr_data2le
#define TRANSFORM_NL_CRLF transform_data2le_crlf
#define TRANSFORM_CRLF_NL transform_crlf_data2le
#include "endline.h"


/* Conversions for character unit RECODE_UNIT_2.  */

#define BOM_DECLARATIONS \
  bool swap = false
#define BOM_HANDLING \
  if (character == 0xFFFE) { swap = true; character = 0xFEFF; }
#define GET(v, st) (swap ? get_ucs2le (v, st) : get_ucs2be (v, st))
#define PUT(v, st) (swap ? put_ucs2le (v, st) : put_ucs2be (v, st))
#define TRANSFORM_NL_CR transform_data2_cr
#define TRANSFORM_CR_NL transform_cr_data2
#define TRANSFORM_NL_CRLF transform_data2_crlf
#define TRANSFORM_CRLF_NL transform_crlf_data2
#include "endline.h"


/* Conversions for character unit RECODE_UNIT_4BE.  */

static bool
get_ucs4be (unsigned int *value, RECODE_SUBTASK subtask)
{
  int byte1 = get_byte (subtask);
  if (byte1 != EOF)
    {
      int byte2 = get_byte (subtask);
      if (byte2 != EOF)
	{
	  int byte3 = get_byte (subtask);
	  if (byte3 != EOF)
	    {
	      int byte4 = get_byte (subtask);
	      if (byte4 != EOF)
		{
		  *value = ((MASK (8) & byte1) << 24)
			   | ((MASK (8) & byte2) << 16)
			   | ((MASK (8) & byte3) << 8)
			   | (MASK (8) & byte4);
		  return true;
		}
	    }
	}
    }
  return false;
}

static void
put_ucs4be (unsigned int value, RECODE_SUBTASK subtask)
{
  put_byte (MASK (8) & (value >> 24), subtask);
  put_byte (MASK (8) & (value >> 16), subtask);
  put_byte (MASK (8) & (value >> 8), subtask);
  put_byte (MASK (8) & value, subtask);
}

#define BOM_DECLARATIONS /* nothing */
#define BOM_HANDLING /* nothing */
#define GET get_ucs4be
#define PUT put_ucs4be
#define TRANSFORM_NL_CR transform_data4be_cr
#define TRANSFORM_CR_NL transform_cr_data4be
#define TRANSFORM_NL_CRLF transform_data4be_crlf
#define TRANSFORM_CRLF_NL transform_crlf_data4be
#include "endline.h"


/* Conversions for character unit RECODE_UNIT_4LE.  */

static bool
get_ucs4le (unsigned int *value, RECODE_SUBTASK subtask)
{
  int byte1 = get_byte (subtask);
  if (byte1 != EOF)
    {
      int byte2 = get_byte (subtask);
      if (byte2 != EOF)
	{
	  int byte3 = get_byte (subtask);
	  if (byte3 != EOF)
	    {
	      int byte4 = get_byte (subtask);
	      if (byte4 != EOF)
		{
		  *value = (MASK (8) & byte1)
			   | ((MASK (8) & byte2) << 8)
			   | ((MASK (8) & byte3) << 16)
			   | ((MASK (8) & byte4) << 24);
		  return true;
		}
	    }
	}
    }
  return false;
}

static void
put_ucs4le (unsigned int value, RECODE_SUBTASK subtask)
{
  put_byte (MASK (8) & value, subtask);
  put_byte (MASK (8) & (value >> 8), subtask);
  put_byte (MASK (8) & (value >> 16), subtask);
  put_byte (MASK (8) & (value >> 24), subtask);
}

#define BOM_DECLARATIONS /* nothing */
#define BOM_HANDLING /* nothing */
#define GET get_ucs4le
#define PUT put_ucs4le
#define TRANSFORM_NL_CR transform_data4le_cr
#define TRANSFORM_CR_NL transform_cr_data4le
#define TRANSFORM_NL_CRLF transform_data4le_crlf
#define TRANSFORM_CRLF_NL transform_crlf_data4le
#include "endline.h"


/* Conversions for character unit RECODE_UNIT_4.  */

#define BOM_DECLARATIONS \
  bool swap = false
#define BOM_HANDLING \
  if (character == 0xFFFE0000) { swap = true; character = 0x0000FEFF; }
#define GET(v, st) (swap ? get_ucs4le (v, st) : get_ucs4be (v, st))
#define PUT(v, st) (swap ? put_ucs4le (v, st) : put_ucs4be (v, st))
#define TRANSFORM_NL_CR transform_data4_cr
#define TRANSFORM_CR_NL transform_cr_data4
#define TRANSFORM_NL_CRLF transform_data4_crlf
#define TRANSFORM_CRLF_NL transform_crlf_data4
#include "endline.h"


bool
module_endline (RECODE_OUTER outer)
{
  return
    declare_single (outer, "data1", "CR",
		    outer->quality_byte_to_byte,
		    NULL, transform_data1_cr)
    && declare_single (outer, "CR", "data1",
		       outer->quality_byte_to_byte,
		       NULL, transform_cr_data1)
    && declare_single (outer, "data1", "CR-LF",
		       outer->quality_byte_to_variable,
		       NULL, transform_data1_crlf)
    && declare_single (outer, "CR-LF", "data1",
		       outer->quality_variable_to_byte,
		       NULL, transform_crlf_data1)

    && declare_single (outer, "data2", "CR",
		       outer->quality_byte_to_byte,
		       NULL, transform_data2_cr)
    && declare_single (outer, "CR", "data2",
		       outer->quality_byte_to_byte,
		       NULL, transform_cr_data2)
    && declare_single (outer, "data2", "CR-LF",
		       outer->quality_byte_to_variable,
		       NULL, transform_data2_crlf)
    && declare_single (outer, "CR-LF", "data2",
		       outer->quality_variable_to_byte,
		       NULL, transform_crlf_data2)

    && declare_single (outer, "data2be", "CR",
		       outer->quality_byte_to_byte,
		       NULL, transform_data2be_cr)
    && declare_single (outer, "CR", "data2be",
		       outer->quality_byte_to_byte,
		       NULL, transform_cr_data2be)
    && declare_single (outer, "data2be", "CR-LF",
		       outer->quality_byte_to_variable,
		       NULL, transform_data2be_crlf)
    && declare_single (outer, "CR-LF", "data2be",
		       outer->quality_variable_to_byte,
		       NULL, transform_crlf_data2be)

    && declare_single (outer, "data2le", "CR",
		       outer->quality_byte_to_byte,
		       NULL, transform_data2le_cr)
    && declare_single (outer, "CR", "data2le",
		       outer->quality_byte_to_byte,
		       NULL, transform_cr_data2le)
    && declare_single (outer, "data2le", "CR-LF",
		       outer->quality_byte_to_variable,
		       NULL, transform_data2le_crlf)
    && declare_single (outer, "CR-LF", "data2le",
		       outer->quality_variable_to_byte,
		       NULL, transform_crlf_data2le)

    && declare_single (outer, "data4", "CR",
		       outer->quality_byte_to_byte,
		       NULL, transform_data4_cr)
    && declare_single (outer, "CR", "data4",
		       outer->quality_byte_to_byte,
		       NULL, transform_cr_data4)
    && declare_single (outer, "data4", "CR-LF",
		       outer->quality_byte_to_variable,
		       NULL, transform_data4_crlf)
    && declare_single (outer, "CR-LF", "data4",
		       outer->quality_variable_to_byte,
		       NULL, transform_crlf_data4)

    && declare_single (outer, "data4be", "CR",
		       outer->quality_byte_to_byte,
		       NULL, transform_data4be_cr)
    && declare_single (outer, "CR", "data4be",
		       outer->quality_byte_to_byte,
		       NULL, transform_cr_data4be)
    && declare_single (outer, "data4be", "CR-LF",
		       outer->quality_byte_to_variable,
		       NULL, transform_data4be_crlf)
    && declare_single (outer, "CR-LF", "data4be",
		       outer->quality_variable_to_byte,
		       NULL, transform_crlf_data4be)

    && declare_single (outer, "data4le", "CR",
		       outer->quality_byte_to_byte,
		       NULL, transform_data4le_cr)
    && declare_single (outer, "CR", "data4le",
		       outer->quality_byte_to_byte,
		       NULL, transform_cr_data4le)
    && declare_single (outer, "data4le", "CR-LF",
		       outer->quality_byte_to_variable,
		       NULL, transform_data4le_crlf)
    && declare_single (outer, "CR-LF", "data4le",
		       outer->quality_variable_to_byte,
		       NULL, transform_crlf_data4le)

    && declare_alias (outer, "cl", "CR-LF");
}

void
delmodule_endline (RECODE_OUTER outer)
{
}
