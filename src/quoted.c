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
static const bool bitnet_flag = false;

/* Maximum number of characters per MIME line.  */
#define MIME_LINE_LENGTH 76

/* Characters from 33 to 60 inclusive and from 62 to 126 inclusive do not
   need be quoted.  */
static const char safe_char_usual[128] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*   0- 15 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  16- 31 */
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*  32- 47 */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, /*  48- 63 */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*  64- 79 */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*  80- 95 */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*  96-111 */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0  /* 112-127 */
};

/* In a Bitnet environment, also quote: ! " # $ @ [ \ ] ^ ` { | } ~ */
static const char safe_char_bitnet[128] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*   0- 15 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  16- 31 */
  0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*  32- 47 */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, /*  48- 63 */
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*  64- 79 */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, /*  80- 95 */
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*  96-111 */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0  /* 112-127 */
};

#define PUT_QUOTED(Character) \
  do {								\
    unsigned nibble;							\
								\
    put_byte ('=', task);					\
    nibble = MASK (4) & (Character) >> 4;			\
    put_byte ((nibble < 10 ? '0' : 'A' - 10) + nibble, task);	\
    nibble = MASK (4) & (Character);				\
    put_byte ((nibble < 10 ? '0' : 'A' - 10) + nibble, task);	\
  } while (false)

static bool
transform_data_quoted_printable (RECODE_CONST_STEP step,
				 RECODE_TASK task)
{
  const char *safe_char;
  unsigned available;
  int character;
  int next_character;

  safe_char = bitnet_flag ? safe_char_bitnet : safe_char_usual;

  /* Proper maximum filling of quoted-printable lines is a bit trickier than
     I initially expected, avoiding a line buffer nor much look-ahead.  */

  available = MIME_LINE_LENGTH;
  character = get_byte (task);
  while (character != EOF)
    if (character < 128 && safe_char[character])

      /* Case of a safe character.  */

      if (available > 1)
	{
	  put_byte (character, task);
	  available--;
	  character = get_byte (task);
	}
      else
	{
	  next_character = get_byte (task);
	  if (next_character == '\n')
	    {
	      put_byte (character, task);
	      put_byte ('\n', task);
	      available = MIME_LINE_LENGTH;
	      character = get_byte (task);
	    }
	  else if (next_character == EOF)
	    {
	      put_byte (character, task);
	      available--;
	      character = EOF;
	    }
	  else
	    {
	      put_byte ('=', task);
	      put_byte ('\n', task);
	      put_byte (character, task);
	      available = MIME_LINE_LENGTH - 1;
	      character = next_character;
	    }
	}
    else
      switch (character)
	{
	case '\n':

	  /* Case of a newline.  */

	  put_byte ('\n', task);
	  available = MIME_LINE_LENGTH;
	  character = get_byte (task);
	  break;

	case '\t':
	case ' ':

	  /* Case of a space character.  */

	  next_character = get_byte (task);
	  if (next_character == '\n')
	    {
	      if (available < 3)
		{
		  put_byte ('=', task);
		  put_byte ('\n', task);
		}
	      PUT_QUOTED (character);
	      put_byte ('\n', task);
	      available = MIME_LINE_LENGTH;
	      character = get_byte (task);
	    }
	  else if (next_character == EOF)
	    {
	      if (available < 3)
		{
		  put_byte ('=', task);
		  put_byte ('\n', task);
		  available = MIME_LINE_LENGTH;
		}
	      PUT_QUOTED (character);
	      available--;
	      character = EOF;
	    }
	  else
	    {
	      if (available == 1)
		{
		  put_byte ('=', task);
		  put_byte ('\n', task);
		  available = MIME_LINE_LENGTH;
		}
	      put_byte (character, task);
	      available--;
	      character = next_character;
	    }
	  break;

	default:

	  /* Case of an inconditional quotable character.  */

	  if (available > 3)
	    {
	      PUT_QUOTED (character);
	      available -= 3;
	      character = get_byte (task);
	    }
	  else
	    {
	      next_character = get_byte (task);
	      if (available == 3 && next_character == '\n')
		{
		  PUT_QUOTED (character);
		  put_byte ('\n', task);
		  available = MIME_LINE_LENGTH;
		  character = get_byte (task);
		}
	      else if (next_character == EOF)
		{
		  PUT_QUOTED (character);
		  available--;
		  character = EOF;
		}
	      else
		{
		  put_byte ('=', task);
		  put_byte ('\n', task);
		  PUT_QUOTED (character);
		  available = MIME_LINE_LENGTH - 3;
		  character = next_character;
		}
	    }
	}

  if (available != MIME_LINE_LENGTH)
    {
      /* The last line was incomplete.  */

      put_byte ('=', task);
      put_byte ('\n', task);
    }

  TASK_RETURN (task);
}

static bool
transform_quoted_printable_byte (RECODE_CONST_STEP step,
				 RECODE_TASK task)
{
  const char *safe_char;
  bool status;
  unsigned counter;
  int character;
  unsigned last_length;
  enum {SPACE_TYPE, SAFE_TYPE, QUOTED_TYPE} last_type;
  unsigned value;

  /* FIXME: reversibility validation is not really implemented.  To be
     completed at some later time.  */

  safe_char = bitnet_flag ? safe_char_bitnet : safe_char_usual;

  status = true;
  counter = 0;
  character = get_byte (task);
  last_length = MIME_LINE_LENGTH;
  last_type = SAFE_TYPE;
  while (character != EOF)
    {
      switch (character)
	{
	case '\n':

	  /* Process hard line break.  */

	  last_length = counter;
	  counter = 0;
	  if (last_type == SPACE_TYPE)
	    status = false;
	  put_byte ('\n', task);
	  character = get_byte (task);
	  last_type = SAFE_TYPE;
	  break;

	case ' ':
	case '\t':

	  /* Process white space.  */

	  counter++;
#if 0
	  if (available == 0)
	    status = false;
#endif
	    put_byte (character, task);
	    character = get_byte (task);
	    last_type = SPACE_TYPE;
	    break;

	case '=':
	  character = get_byte (task);
	  if (character == ' ' || character == '\t' || character == '\n')
	    {
	      /* Process soft line break.  */

	      while (character == ' ' || character == '\t')
		character = get_byte (task);
	      if (character != '\n')
		{
		  status = false;
		  break;
		}
	      counter = 0;
	      character = get_byte (task);
	      break;
	    }

	  /* Process quoted value.  */

	  if (character >= '0' && character <= '9')
	    value = (character - '0') << 4;
	  else if (character >= 'a' && character <= 'f')
	    value = (character - 'a' + 10) << 4;
	  else if (character >= 'A' && character <= 'F')
	    value = (character - 'A' + 10) << 4;
	  else
	    {
	      counter++;
	      status = false;
	      break;
	    }
	  character = get_byte (task);
	  if (character >= '0' && character <= '9')
	    value |= character - '0';
	  else if (character >= 'a' && character <= 'f')
	    value |= character - 'a' + 10;
	  else if (character >= 'A' && character <= 'F')
	    value |= character - 'A' + 10;
	  else
	    {
	      counter += 2;
	      status = false;
	      break;
	    }
	  counter += 3;
	  put_byte (value, task);
	  character = get_byte (task);
	  last_type = QUOTED_TYPE;
	  break;

	default:

	  /* Process safe character.  */

	  counter++;
	  put_byte (character, task);
	  character = get_byte (task);
	  last_type = SAFE_TYPE;
	}

      /* Truncate longer lines, ensuring it is white space.  */

      if (counter >= MIME_LINE_LENGTH)
	while (character != '\n' && character != EOF)
	  {
	    if (character != ' ' && character != '\t')
	      status = false;
	    character = get_byte (task);
	  }
    }

  if (counter != 0)
    /* Last line is not terminated.  */
    RETURN_IF_NOGO (RECODE_NOT_CANONICAL, step, task);

  TASK_RETURN (task);
}

bool
module_quoted_printable (RECODE_OUTER outer)
{
  return
    declare_single (outer, "data", "Quoted-Printable",
		    outer->quality_variable_to_variable,
		    NULL, transform_data_quoted_printable)
    && declare_single (outer, "Quoted-Printable", "data",
		       outer->quality_variable_to_variable,
		       NULL, transform_quoted_printable_byte)
    && declare_alias (outer, "quote-printable", "Quoted-Printable")
    && declare_alias (outer, "QP", "Quoted-Printable");
}
