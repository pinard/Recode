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

/* Define HANDLE_32_BITS if you want conversion for 2^32 codes instead
   of 2^31.  But this would not be ISO-10646, which says 2^31.  */

/* Read next data byte and check its value, discard an illegal sequence.
   This macro is meant to be used only within the `while' loop in
   `transform_utf8_ucs[24]'.  */
#define GET_DATA_BYTE \
  character = get_byte (task);						\
  if (character == EOF)							\
    {									\
      RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);		\
      break;								\
    }									\
  else if ((MASK (2) << 6 & character) != 1 << 7)			\
    {									\
      RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);		\
      continue;								\
    }									\
  else

/* Read next data byte and check its value, discard an illegal sequence.
   Merge it into `value' at POSITION.  This macro is meant to be used only
   within the `while' loop in `transform_utf8_ucs[24]'.  */
#define GET_DATA_BYTE_AT(Position) \
  GET_DATA_BYTE /* ... else */ value |= (MASK (6) & character) << Position

static bool
transform_ucs2_utf8 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  unsigned value;

  while (get_ucs2 (&value, step, task))
    {
      if (value & ~MASK (7))
	if (value & ~MASK (11))
	  {
	    /* 3 bytes - more than 11 bits, but not more than 16.  */
	    put_byte ((MASK (3) << 5) | (MASK (6) & value >> 12), task);
	    put_byte ((1 << 7) | (MASK (6) & value >> 6), task);
	    put_byte ((1 << 7) | (MASK (6) & value), task);
	  }
	else
	  {
	    /* 2 bytes - more than 7 bits, but not more than 11.  */
	    put_byte ((MASK (2) << 6) | (MASK (6) & value >> 6), task);
	    put_byte ((1 << 7) | (MASK (6) & value), task);
	  }
      else
	/* 1 byte - not more than 7 bits (that is, ASCII).  */
	put_byte (value, task);
    }

  TASK_RETURN (task);
}

static bool
transform_ucs4_utf8 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  unsigned value;

  while (get_ucs4 (&value, step, task))
    if (value & ~MASK (16))
      if (value & ~MASK (26))
	if (value & ~MASK (31))
	  {
#if HANDLE_32_BITS
	    /* 7 bytes - more than 31 bits (that is, exactly 32 :-).  */
	    put_byte (MASK (7) << 1);
	    put_byte ((1 << 7) | (MASK (6) & value >> 30), task);
	    put_byte ((1 << 7) | (MASK (6) & value >> 24), task);
	    put_byte ((1 << 7) | (MASK (6) & value >> 18), task);
	    put_byte ((1 << 7) | (MASK (6) & value >> 12), task);
	    put_byte ((1 << 7) | (MASK (6) & value >> 6), task);
	    put_byte ((1 << 7) | (MASK (6) & value), task);
#else
	    RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
#endif
	  }
  	else
	  {
	    /* 6 bytes - more than 26 bits, but not more than 31.  */
	    put_byte ((MASK (6) << 2) | (MASK (6) & value >> 30), task);
	    put_byte ((1 << 7) | (MASK (6) & value >> 24), task);
	    put_byte ((1 << 7) | (MASK (6) & value >> 18), task);
	    put_byte ((1 << 7) | (MASK (6) & value >> 12), task);
	    put_byte ((1 << 7) | (MASK (6) & value >> 6), task);
	    put_byte ((1 << 7) | (MASK (6) & value), task);
	  }
      else if (value & ~MASK (21))
	{
	  /* 5 bytes - more than 21 bits, but not more than 26.  */
	  put_byte ((MASK (5) << 3) | (MASK (6) & value >> 24), task);
	  put_byte ((1 << 7) | (MASK (6) & value >> 18), task);
	  put_byte ((1 << 7) | (MASK (6) & value >> 12), task);
	  put_byte ((1 << 7) | (MASK (6) & value >> 6), task);
	  put_byte ((1 << 7) | (MASK (6) & value), task);
	}
      else
	{
	  /* 4 bytes - more than 16 bits, but not more than 21.  */
	  put_byte ((MASK (4) << 4) | (MASK (6) & value >> 18), task);
	  put_byte ((1 << 7) | (MASK (6) & value >> 12), task);
	  put_byte ((1 << 7) | (MASK (6) & value >> 6), task);
	  put_byte ((1 << 7) | (MASK (6) & value), task);
	}
    else if (value & ~MASK (7))
      if (value & ~MASK (11))
	{
	  /* 3 bytes - more than 11 bits, but not more than 16.  */
	  put_byte ((MASK (3) << 5) | (MASK (6) & value >> 12), task);
	  put_byte ((1 << 7) | (MASK (6) & value >> 6), task);
	  put_byte ((1 << 7) | (MASK (6) & value), task);
	}
      else
	{
	  /* 2 bytes - more than 7 bits, but not more than 11.  */
	  put_byte ((MASK (2) << 6) | (MASK (6) & value >> 6), task);
	  put_byte ((1 << 7) | (MASK (6) & value), task);
	}
    else
      /* 1 byte - not more than 7 bits (that is, ASCII).  */
      put_byte (value, task);

  TASK_RETURN (task);
}

/* FIXME: The UTF-8 decoding algorithms do not validate that the minimum
   length surface was indeed used.  This would be necessary for ensuring
   that the recoding is exactly reversible.  In fact, this minimum length
   surface is also a requirement of UTF-8 specification.  */

static bool
transform_utf8_ucs4 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  int character = get_byte (task);
  unsigned value;

  while (character != EOF)

    /* Process one UTF-8 value.  EOF is acceptable on first byte only.  */

    if ((character & MASK (4) << 4) == MASK (4) << 4)
      if ((character & MASK (6) << 2) == MASK (6) << 2)
	if ((character & MASK (7) << 1) == MASK (7) << 1)
	  {
	    /* 7 bytes - more than 31 bits (that is, exactly 32 :-).  */
#if HANDLE_32_BITS
	    value = 0;
	    GET_DATA_BYTE_AT (30);
	    GET_DATA_BYTE_AT (24);
	    GET_DATA_BYTE_AT (18);
	    GET_DATA_BYTE_AT (12);
	    GET_DATA_BYTE_AT (6);
	    GET_DATA_BYTE_AT (0);
	    put_ucs4 (value, task);
	    character = get_byte (task);
#else
	    RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
	    character = get_byte (task);
#endif
	  }
	else
	  {
	    /* 6 bytes - more than 26 bits, but not more than 31.  */
	    value = (MASK (1) & character) << 30;
	    GET_DATA_BYTE_AT (24);
	    GET_DATA_BYTE_AT (18);
	    GET_DATA_BYTE_AT (12);
	    GET_DATA_BYTE_AT (6);
	    GET_DATA_BYTE_AT (0);
	    put_ucs4 (value, task);
	    character = get_byte (task);
	  }
      else if ((character & MASK (5) << 3) == MASK (5) << 3)
	{
	  /* 5 bytes - more than 21 bits, but not more than 26.  */
	  value = (MASK (2) & character) << 24;
	  GET_DATA_BYTE_AT (18);
	  GET_DATA_BYTE_AT (12);
	  GET_DATA_BYTE_AT (6);
	  GET_DATA_BYTE_AT (0);
	  put_ucs4 (value, task);
	  character = get_byte (task);
	}
      else
	{
	  /* 4 bytes - more than 16 bits, but not more than 21.  */
	  value = (MASK (3) & character) << 18;
	  GET_DATA_BYTE_AT (12);
	  GET_DATA_BYTE_AT (6);
	  GET_DATA_BYTE_AT (0);
	  put_ucs4 (value, task);
	  character = get_byte (task);
	}
    else if ((character & MASK (2) << 6) == MASK (2) << 6)
      if ((character & MASK (3) << 5) == MASK (3) << 5)
	{
	  /* 3 bytes - more than 11 bits, but not more than 16.  */
	  value = (MASK (4) & character) << 12;
	  GET_DATA_BYTE_AT (6);
	  GET_DATA_BYTE_AT (0);
	  put_ucs4 (value, task);
	  character = get_byte (task);
	}
      else
	{
	  /* 2 bytes - more than 7 bits, but not more than 11.  */
	  value = (MASK (5) & character) << 6;
	  GET_DATA_BYTE_AT (0);
	  put_ucs4 (value, task);
	  character = get_byte (task);
	}
    else if ((character & 1 << 7) == 1 << 7)
      {
	/* Valid only as a continuation byte.  */
	RETURN_IF_NOGO (RECODE_INVALID_INPUT, step, task);
	character = get_byte (task);
      }
    else
      {
	/* 1 byte - not more than 7 bits (that is, ASCII).  */
	put_ucs4 (MASK (8) & character, task);
	character = get_byte (task);
      }

  TASK_RETURN (task);
}

bool
module_utf8 (RECODE_OUTER outer)
{
  return
    declare_single (outer, "ISO-10646-UCS-4", "UTF-8",
		    outer->quality_variable_to_variable,
		    NULL, transform_ucs4_utf8)
    && declare_single (outer, "UTF-8", "ISO-10646-UCS-4",
		       outer->quality_variable_to_variable,
		       NULL, transform_utf8_ucs4)

    && declare_alias (outer, "UTF-2", "UTF-8")
    && declare_alias (outer, "UTF-FSS", "UTF-8")
    && declare_alias (outer, "FSS_UTF", "UTF-8")
    && declare_alias (outer, "TF-8", "UTF-8")
    && declare_alias (outer, "u8", "UTF-8")

    /* Simple UCS-2 does not have to go through UTF-16.  */
    && declare_single (outer, "ISO-10646-UCS-2", "UTF-8",
		       outer->quality_variable_to_variable,
		       NULL, transform_ucs2_utf8);
}
