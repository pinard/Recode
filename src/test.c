/* Conversion of files between different charsets and surfaces.
   Copyright © 1997, 1998, 1999 Free Software Foundation, Inc.
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

/* One should _remove_ pseudo-surfaces to _produce_ test patterns.  This
   strange-looking decision comes from the fact that test patterns are usually
   requested from the `before' position in the request.  */

static bool
test7_data (const struct recode_step *step, struct recode_task *task)
{
  unsigned counter;
  int value;

  for (counter = 0; counter < 1 << 7; counter++)
    put_byte (counter, task);
  while (value = get_byte (task), value != EOF)
    put_byte (value, task);

  TASK_RETURN (task);
}

static bool
test8_data (const struct recode_step *step, struct recode_task *task)
{
  unsigned counter;
  int value;

  for (counter = 0; counter < 1 << 8; counter++)
    put_byte (counter, task);
  while (value = get_byte (task), value != EOF)
    put_byte (value, task);

  TASK_RETURN (task);
}

static bool
test15_data (const struct recode_step *step, struct recode_task *task)
{
  unsigned counter;
  unsigned value;

  put_ucs2 (BYTE_ORDER_MARK, task);

  /* Before surrogate area.  */
  for (counter = 0; counter < 0xDC00; counter++)
    put_ucs2 (counter, task);
  /* After surrogate area.  */
  for (counter = 0xE000; counter < 1 << 16; counter++)
    switch (counter)
      {
      case BYTE_ORDER_MARK:
      case REPLACEMENT_CHARACTER:
      case BYTE_ORDER_MARK_SWAPPED:
      case NOT_A_CHARACTER:
	break;

      default:
	put_ucs2 (counter, task);
      }
  while (get_ucs2 (&value, task))
    put_ucs2 (value, task);

  TASK_RETURN (task);
}

static bool
test16_data (const struct recode_step *step, struct recode_task *task)
{
  unsigned counter;
  unsigned value;

  for (counter = 0; counter < 1 << 16; counter++)
    put_ucs2 (counter, task);
  while (get_ucs2 (&value, task))
    put_ucs2 (value, task);

  TASK_RETURN (task);
}

bool
module_test (struct recode_outer *outer)
{
  if (!declare_step (outer, "test7", "data",
		     outer->quality_variable_to_byte,
		     NULL, test7_data))
    return false;
  if (!declare_step (outer, "test8", "data",
		     outer->quality_variable_to_byte,
		     NULL, test8_data))
    return false;
  if (!declare_step (outer, "test15", "data",
		     outer->quality_variable_to_ucs2,
		     NULL, test15_data))
    return false;
  if (!declare_step (outer, "test16", "data",
		     outer->quality_variable_to_ucs2,
		     NULL, test16_data))
    return false;

  return true;
}
