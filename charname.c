/* Conversion of files between different charsets and usages.
   Copyright (C) 1993 Free Software Foundation, Inc.
   Francois Pinard <pinard@iro.umontreal.ca>, 1993.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "recode.h"

#include "charname.h"

/*--------------------------------------------------------------------.
| Return a statically allocated full charname associated with a given |
| SYMBOL, or NULL if not found.					      |
`--------------------------------------------------------------------*/

char *
symbol_to_charname (const char *symbol)
{
  static char result[MAX_CHARNAME_LENGTH + 1];
  int first;
  int last;
  int middle;
  int value;
  const char *in;
  char *out;
  const char *cursor;

  /* Find the symbol by binary searching the charname table.  */

  first = 0;
  last = NUMBER_OF_CHARNAMES;
  while (first < last)
    {
      middle = (first + last) / 2;
      value = strcmp (charname[middle].symbol, symbol);
      if (value < 0)
	first = middle + 1;
      else if (value > 0)
	last = middle;
      else
	break;
    }

  /* If the symbol has not been found, return the NULL string.  */

  if (first >= last)
    return NULL;

  /* Else, construct the resulting charname.  */

  out = NULL;
  for (in = charname[middle].crypted; *in; in++)
    {

      /* Decrypt the next word.  */

      value = *(const unsigned char *) in - 1;
      if (value >= NUMBER_OF_SINGLES)
	value = (NUMBER_OF_SINGLES + 255 * (value - NUMBER_OF_SINGLES)
		 + *(const unsigned char *) ++in - 1);

      /* Copy it.  */

      if (out)
	*out++ = ' ';
      else
	out = result;

      for (cursor = word[value]; *cursor; cursor++)
	*out++ = *cursor;
    }

  /* Return the result.  */

  *out = '\0';
  return result;
}
