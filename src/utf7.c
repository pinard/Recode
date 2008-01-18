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

/* Classification of first 128 UCS-2 characters.  */

#define D 1			/* set D -- direct character */
#define O 2			/* set O -- optional direct character */
#define W 4			/* direct white space */

#define z 0
static char classification[128] =
{
  z, z, z, z, z, z, z, z, z, W,  W, z, z, W, z, z, z, z, z, z,	/*   0- 19 */
  z, z, z, z, z, z, z, z, z, z,  z, z, W, O, O, O, O, O, O, D,	/*  20- 39 */
  D, D, O, z, D, D, D, D, D, D,  D, D, D, D, D, D, D, D, D, O,	/*  40- 59 */
  O, O, O, D, O, D, D, D, D, D,  D, D, D, D, D, D, D, D, D, D, 	/*  60- 79 */
  D, D, D, D, D, D, D, D, D, D,  D, O, z, O, O, O, O, D, D, D, 	/*  80- 99 */
  D, D, D, D, D, D, D, D, D, D,  D, D, D, D, D, D, D, D, D, D, 	/* 100-119 */
  D, D, D, O, O, O, z, z					/* 120-127 */
};
#undef z

/* Should the character be directly represented in message headers?  */
#define IS_HEADER_DIRECT(Character) \
  (IS_ASCII (Character) && classification[Character] & (D | W))

/* Should the character be directly represented in message bodies?
   Here, & (D | O | W) is implied, since these are the only flags.  */
#define IS_BODY_DIRECT(Character) \
  (IS_ASCII (Character) && classification[Character])

/* Transformation routines.  */

/* The following diagram shows the logical steps by which three UCS-2
   characters get transformed into eight Base64 characters.  It helps
   understanding shifts and masks in the transformation functions.

	 .--------+--------.  .--------+--------.  .--------+--------.
	 |aaaaaabb|bbbbcccc|  |ccdddddd|eeeeeeff|  |ffffgggg|gghhhhhh|
	 `--------+--------'  `--------+--------'  `--------+--------'
            6   2   4   4       2   6     6   2      4   4    2   6
   .--------+--------+--------+--------+--------+--------+--------+--------.
   |00aaaaaa|00bbbbbb|00cccccc|00dddddd|00eeeeee|00ffffff|00gggggg|00hhhhhh|
   `--------+--------+--------+--------+--------+--------+--------+--------'

   .--------+--------+--------+--------+--------+--------+--------+--------.
   |AAAAAAAA|BBBBBBBB|CCCCCCCC|DDDDDDDD|EEEEEEEE|FFFFFFFF|GGGGGGGG|HHHHHHHH|
   `--------+--------+--------+--------+--------+--------+--------+--------'

   The UCS-2 characters are divided into 6 bit chunks, which are then
   encoded into Base64 characters.  */

static bool
transform_utf16_utf7 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  int character;
  unsigned value;

  if (!get_ucs2 (&value, step, task))
    TASK_RETURN (task);

  while (true)

    if (IS_BODY_DIRECT (value))
      {
	/* Copy one direct character.  */

	put_byte (value, task);
	if (!get_ucs2 (&value, step, task))
	  TASK_RETURN (task);
      }
    else
      {
	/* Copy a string of non-direct characters.  */

	put_byte ('+', task);

	while (!IS_BODY_DIRECT (value))
	  {
	    unsigned split;

	    /* Process first UCS-2 value of a triplet.  */

	    put_byte (base64_value_to_char[MASK (6) & value >> 10], task);
	    put_byte (base64_value_to_char[MASK (6) & value >> 4], task);
	    split = (value & MASK (4)) << 2;
	    if (!get_ucs2 (&value, step, task))
	      {
		put_byte (base64_value_to_char[split], task);
		TASK_RETURN (task);
	      }

	    /* Process second UCS-2 value of a triplet.  */

	    if (IS_BODY_DIRECT (value))
	      {
		put_byte (base64_value_to_char[split], task);
		break;
	      }
	    put_byte (base64_value_to_char[split | (MASK (2) & value >> 14)],
		      task);
	    put_byte (base64_value_to_char[MASK (6) & value >> 8], task);
	    put_byte (base64_value_to_char[MASK (6) & value >> 2], task);
	    split = (value & MASK (2)) << 4;
	    if (!get_ucs2 (&value, step, task))
	      {
		put_byte (base64_value_to_char[split], task);
		TASK_RETURN (task);
	      }

	    /* Process third UCS-2 value of a triplet.  */

	    if (IS_BODY_DIRECT (value))
	      {
		put_byte (base64_value_to_char[split], task);
		break;
	      }
	    put_byte (base64_value_to_char[split | (MASK (4) & value >> 12)],
		      task);
	    put_byte (base64_value_to_char[MASK (6) & value >> 6], task);
	    put_byte (base64_value_to_char[MASK (6) & value], task);
	    if (!get_ucs2 (&value, step, task))
	      TASK_RETURN (task);
	  }

	if (IS_BASE64 (value))
	  put_byte ('-', task);
      }

  TASK_RETURN (task);
}

static bool
transform_utf7_utf16 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  int character;
  unsigned value;
  unsigned split;

  character = get_byte (task);

  if (character != EOF && task->byte_order_mark)
    put_ucs2 (BYTE_ORDER_MARK, task);

  while (character != EOF)
    if (character == '+')
      {
	character = get_byte (task);
	while (IS_BASE64 (character))
	  {
	    /* Process first byte of first quadruplet.  */

	    value = base64_char_to_value[character] << 10;
	    character = get_byte (task);

	    /* Process second byte of first quadruplet.  */

	    if (!IS_BASE64 (character))
	      {
		RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
		break;
	      }
	    value |= base64_char_to_value[character] << 4;
	    character = get_byte (task);

	    /* Process third byte of first quadruplet.  */

	    if (!IS_BASE64 (character))
	      {
		RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
		break;
	      }
	    split = base64_char_to_value[character];
	    value |= split >> 2;
	    if (IS_BODY_DIRECT (value))
	      RETURN_IF_NOGO (RECODE_NOT_CANONICAL, step, task);
	    put_ucs2 (value, task);
	    character = get_byte (task);

	    /* Process fourth byte of first quadruplet.  */

	    if (!IS_BASE64 (character))
	      {
		if (MASK (2) & split)
		  RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
		break;
	      }
	    value = ((MASK (2) & split) << 14
		     | base64_char_to_value[character] << 8);
	    character = get_byte (task);

	    /* Process first byte of second quadruplet.  */

	    if (!IS_BASE64 (character))
	      {
		RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
		break;
	      }
	    value |= base64_char_to_value[character] << 2;
	    character = get_byte (task);

	    /* Process second byte of second quadruplet.  */

	    if (!IS_BASE64 (character))
	      {
		RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
		break;
	      }
	    split = base64_char_to_value[character];
	    value |= split >> 4;
	    if (IS_BODY_DIRECT (value))
	      RETURN_IF_NOGO (RECODE_NOT_CANONICAL, step, task);
	    put_ucs2 (value, task);
	    character = get_byte (task);

	    /* Process third byte of second quadruplet.  */

	    if (!IS_BASE64 (character))
	      {
		if (MASK (4) & split)
		  RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
		break;
	      }
	    value = ((MASK (4) & split) << 12
		     | base64_char_to_value[character] << 6);
	    character = get_byte (task);

	    /* Process fourth byte of second quadruplet.  */

	    if (!IS_BASE64 (character))
	      {
		RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
		break;
	      }
	    value |= base64_char_to_value[character];
	    if (IS_BODY_DIRECT (value))
	      RETURN_IF_NOGO (RECODE_NOT_CANONICAL, step, task);
	    put_ucs2 (value, task);
	    character = get_byte (task);
	  }
	if (character == '-')
	  {
	    character = get_byte (task);
	    if (!IS_BASE64 (character))
	      RETURN_IF_NOGO (RECODE_NOT_CANONICAL, step, task);
	  }
      }
    else
      {
	if (!IS_BODY_DIRECT (character))
	  RETURN_IF_NOGO (RECODE_NOT_CANONICAL, step, task);
	put_byte (NUL, task);
	put_byte (character, task);
	character = get_byte (task);
      }

  TASK_RETURN (task);
}

bool
module_utf7 (RECODE_OUTER outer)
{
  return
    declare_single (outer, "UTF-16", "UNICODE-1-1-UTF-7",	/* RFC1642 */
		    outer->quality_variable_to_variable,
		    NULL, transform_utf16_utf7)
    && declare_single (outer, "UNICODE-1-1-UTF-7", "UTF-16",
		       outer->quality_variable_to_variable,
		       NULL, transform_utf7_utf16)

    && declare_alias (outer, "UTF-7", "UNICODE-1-1-UTF-7")
    && declare_alias (outer, "TF-7", "UNICODE-1-1-UTF-7")
    && declare_alias (outer, "u7", "UNICODE-1-1-UTF-7")

    /* Simple UCS-2 does not have to go through UTF-16.  */
    && declare_single (outer, "ISO-10646-UCS-2", "UNICODE-1-1-UTF-7",
		       outer->quality_variable_to_variable,
		       NULL, transform_utf16_utf7);
}
