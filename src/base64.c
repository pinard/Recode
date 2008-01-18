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
#include "base64.h"

/* Table of characters coding the 64 values.  */
char base64_value_to_char[64] =
{
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',	/*  0- 9 */
  'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',	/* 10-19 */
  'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',	/* 20-29 */
  'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',	/* 30-39 */
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',	/* 40-49 */
  'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',	/* 50-59 */
  '8', '9', '+', '/'					/* 60-63 */
};

/* Table of base64 values for first 128 characters.  */
#define z -1
short base64_char_to_value[128] =
{
  z,   z,   z,   z,   z,   z,   z,   z,   z,   z,	/*   0-  9 */
  z,   z,   z,   z,   z,   z,   z,   z,   z,   z,	/*  10- 19 */
  z,   z,   z,   z,   z,   z,   z,   z,   z,   z,	/*  20- 29 */
  z,   z,   z,   z,   z,   z,   z,   z,   z,   z,	/*  30- 39 */
  z,   z,   z,   62,  z,   z,   z,   63,  52,  53,	/*  40- 49 */
  54,  55,  56,  57,  58,  59,  60,  61,  z,   z,	/*  50- 59 */
  z,   z,   z,   z,   z,   0,   1,   2,   3,   4,	/*  60- 69 */
  5,   6,   7,   8,   9,   10,  11,  12,  13,  14,	/*  70- 79 */
  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,	/*  80- 89 */
  25,  z,   z,   z,   z,   z,   z,   26,  27,  28,	/*  90- 99 */
  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,	/* 100-109 */
  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,	/* 110-119 */
  49,  50,  51,  z,   z,   z,   z,   z			/* 120-127 */
};
#undef z

/* The following diagram shows the logical steps by which three octets
   get transformed into eight Base64 characters.  It helps understanding
   shifts and masks in the transformation functions.

		 .--------.  .--------.  .--------.
		 |aaaaaabb|  |bbbbcccc|  |ccdddddd|
		 `--------'  `--------'  `--------'
                    6   2      4   4       2   6
	       .--------+--------+--------+--------.
	       |00aaaaaa|00bbbbbb|00cccccc|00dddddd|
	       `--------+--------+--------+--------'

	       .--------+--------+--------+--------.
	       |AAAAAAAA|BBBBBBBB|CCCCCCCC|DDDDDDDD|
	       `--------+--------+--------+--------'

   The octets are divided into 6 bit chunks, which are then encoded into
   Base64 characters.  */

static bool
transform_data_base64 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  int counter;
  int character;
  unsigned value;

  counter = 0;
  while (true)
    {
      character = get_byte (task);
      if (character == EOF)
	break;

      /* Wrap line every 76 characters.  */

      if (counter < MIME_LINE_LENGTH / 4)
	counter++;
      else
	{
	  put_byte ('\n', task);
	  counter = 1;
	}

      /* Process first byte of a triplet.  */

      put_byte (base64_value_to_char[MASK (6) & character >> 2], task);
      value = (MASK (2) & character) << 4;

      /* Process second byte of a triplet.  */

      character = get_byte (task);
      if (character == EOF)
	{
	  put_byte (base64_value_to_char[value], task);
	  put_byte ('=', task);
	  put_byte ('=', task);
	  break;
	}
      put_byte (base64_value_to_char[value | (MASK (4) & character >> 4)],
		task);
      value = (MASK (4) & character) << 2;

      /* Process third byte of a triplet.  */

      character = get_byte (task);
      if (character == EOF)
	{
	  put_byte (base64_value_to_char[value], task);
	  put_byte ('=', task);
	  break;
	}
      put_byte (base64_value_to_char[value | (MASK (2) & character >> 6)],
		task);
      put_byte (base64_value_to_char[MASK (6) & character], task);
    }

  /* Complete last partial line.  */

  if (counter > 0)
    put_byte ('\n', task);

  TASK_RETURN (task);
}

static bool
transform_base64_data (RECODE_CONST_STEP step, RECODE_TASK task)
{
  int counter = 0;
  int character;
  unsigned value;

  while (true)
    {
      /* Accept wrapping lines, reversibly if at each 76 characters.  */

      character = get_byte (task);
      if (character == EOF)
	{
	  if (counter != 0)
	    RETURN_IF_NOGO (RECODE_NOT_CANONICAL, step, task);
	  TASK_RETURN (task);
	}

      if (character == '\n')
	{
	  if (counter != MIME_LINE_LENGTH / 4)
	    RETURN_IF_NOGO (RECODE_NOT_CANONICAL, step, task);
	  counter = 0;
	  continue;
	}

      /* Process first byte of a quadruplet.  */

      counter++;

      if (IS_BASE64 (character))
	value = base64_char_to_value[character] << 18;
      else
	{
	  RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
	  value = 0;
	}

      /* Process second byte of a quadruplet.  */

      character = get_byte (task);
      if (character == EOF)
	{
	  RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
	  TASK_RETURN (task);
	}

      if (IS_BASE64 (character))
	value |= base64_char_to_value[character] << 12;
      else
	RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);

      put_byte (value >> 16, task);

      /* Process third byte of a quadruplet.  */

      character = get_byte (task);
      if (character == EOF)
	{
	  RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
	  TASK_RETURN (task);
	}

      if (character == '=')
	{
	  character = get_byte (task);
	  if (character != '=')
	    RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
	  continue;
	}

      if (IS_BASE64 (character))
	value |= base64_char_to_value[character] << 6;
      else
	RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);

      put_byte (MASK (8) & value >> 8, task);

      /* Process fourth byte of a quadruplet.  */

      character = get_byte (task);
      if (character == EOF)
	{
	  RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
	  TASK_RETURN (task);
	}

      if (character == '=')
	continue;

      if (IS_BASE64 (character))
	value |= base64_char_to_value[character];
      else
	RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);

      put_byte (MASK (8) & value, task);
    }
}

bool
module_base64 (RECODE_OUTER outer)
{
  return
    declare_single (outer, "data", "Base64",
		    outer->quality_variable_to_variable,
		    NULL, transform_data_base64)
    && declare_single (outer, "Base64", "data",
		       outer->quality_variable_to_variable,
		       NULL, transform_base64_data)
    && declare_alias (outer, "b64", "Base64")
    && declare_alias (outer, "64", "Base64");
}
