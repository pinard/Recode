/* Conversion of files between different charsets and surfaces.
   Copyright © 1996, 97, 98, 99, 00 Free Software Foundation, Inc.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1997.

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
#include "hash.h"

/*------------------------.
| Produce test patterns.  |
`------------------------*/

/* One should _remove_ pseudo-surfaces to _produce_ test patterns.  This
   strange-looking decision comes from the fact that test patterns are usually
   requested from the `before' position in the request.  */

static bool
test7_data (RECODE_SUBTASK subtask)
{
  unsigned counter;
  int value;

  for (counter = 0; counter < 1 << 7; counter++)
    put_byte (counter, subtask);

  /* Copy the rest verbatim.  */
  while (value = get_byte (subtask), value != EOF)
    put_byte (value, subtask);

  SUBTASK_RETURN (subtask);
}

static bool
test8_data (RECODE_SUBTASK subtask)
{
  unsigned counter;
  int value;

  for (counter = 0; counter < 1 << 8; counter++)
    put_byte (counter, subtask);

  /* Copy the rest verbatim.  */
  while (value = get_byte (subtask), value != EOF)
    put_byte (value, subtask);

  SUBTASK_RETURN (subtask);
}

static bool
test15_data (RECODE_SUBTASK subtask)
{
  unsigned counter;
  unsigned value;

  put_ucs2 (BYTE_ORDER_MARK, subtask);

  /* Before surrogate area.  */
  for (counter = 0; counter < 0xDC00; counter++)
    put_ucs2 (counter, subtask);
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
	put_ucs2 (counter, subtask);
      }

  /* Copy the rest verbatim.  */
  while (value = get_byte (subtask), value != EOF)
    put_byte (value, subtask);

  SUBTASK_RETURN (subtask);
}

static bool
test16_data (RECODE_SUBTASK subtask)
{
  unsigned counter;
  unsigned value;

  for (counter = 0; counter < 1 << 16; counter++)
    put_ucs2 (counter, subtask);

  /* Copy the rest verbatim.  */
  while (value = get_byte (subtask), value != EOF)
    put_byte (value, subtask);

  SUBTASK_RETURN (subtask);
}

/*-----------------------------------------------.
| Produce frequency count for UCS-2 characters.  |
`-----------------------------------------------*/

struct ucs2_to_count
  {
    recode_ucs2 code;		/* UCS-2 value */
    unsigned count;		/* corresponding count */
  };

static unsigned
ucs2_to_count_hash (const void *void_data, unsigned table_size)
{
  const struct ucs2_to_count *data = void_data;

  return data->code % table_size;
}

static bool
ucs2_to_count_compare (const void *void_first, const void *void_second)
{
  const struct ucs2_to_count *first = void_first;
  const struct ucs2_to_count *second = void_second;

  return first->code == second->code;
}

static int
compare_item (const void *void_first, const void *void_second)
{
  struct ucs2_to_count *const *first = void_first;
  struct ucs2_to_count *const *second = void_second;

  return (*first)->code - (*second)->code;
}

static bool
produce_count (RECODE_SUBTASK subtask)
{
  RECODE_OUTER outer = subtask->task->request->outer;
  Hash_table *table;		/* hash table for UCS-2 characters */
  size_t size;			/* number of different characters */
  struct ucs2_to_count **array;	/* array into hash table items */

  table = hash_initialize (0, NULL,
			   ucs2_to_count_hash, ucs2_to_count_compare, free);
  if (!table)
    return false;

  /* Count characters.  */

  {
    unsigned character;		/* current character being counted */

    while (get_ucs2 (&character, subtask))
      {
	struct ucs2_to_count lookup;
	struct ucs2_to_count *entry;

	lookup.code = character;
	entry = hash_lookup (table, &lookup);
	if (entry)
	  entry->count++;
	else
	  {
	    if (!ALLOC (entry, 1, struct ucs2_to_count))
	      {
		hash_free (table);
		return false;
	      }
	    entry->code = character;
	    entry->count = 1;
	    if (!hash_insert (table, entry))
	      {
		hash_free (table);
		free (entry);
		return false;
	      }
	  }
      }
  }

  /* Sort results.  */

  size = hash_get_n_entries (table);

  if (!ALLOC (array, size, struct ucs2_to_count *))
    {
      hash_free (table);
      return false;
    }
  hash_get_entries (table, (void **) array, size);

  qsort (array, size, sizeof (struct ucs2_to_count *), compare_item);

  /* Produce the report.  */

  /* FIXME: Produce it column-wise.  (See transp.c).  */

  {
    const unsigned non_count_width = 12;
    char buffer[50];
    struct ucs2_to_count **cursor;
    unsigned count_width;
    unsigned maximum_count = 0;
    unsigned column = 0;
    unsigned delayed = 0;

    for (cursor = array; cursor < array + size; cursor++)
      if ((*cursor)->count > maximum_count)
	maximum_count = (*cursor)->count;
    sprintf (buffer, "%d", maximum_count);
    count_width = strlen (buffer);

    for (cursor = array; cursor < array + size; cursor++)
      {
	unsigned character = (*cursor)->code;
	const char *mnemonic = ucs2_to_rfc1345 (character);

	if (column + count_width + non_count_width > 80)
	  {
	    putchar ('\n');
	    delayed = 0;
	    column = 0;
	  }
	else
	  while (delayed)
	    {
	      putchar (' ');
	      delayed--;
	    }

	printf ("%*d  %.4X", count_width, (*cursor)->count, character);
	if (mnemonic)
	  {
	    putchar (' ');
	    fputs (mnemonic, stdout);
	    delayed = 6 - 1 - strlen (mnemonic);
	  }
	else
	  delayed = 6;

	column += count_width + non_count_width;
      }

    if (column)
      putchar ('\n');
  }

  /* Clean-up.  */

  free (array);
  hash_free (table);

  SUBTASK_RETURN (subtask);
}

/*---------------------------.
| Fully dump an UCS-2 file.  |
`---------------------------*/

static bool
produce_full_dump (RECODE_SUBTASK subtask)
{
  unsigned character;		/* character to dump */

  /* Dump all characters.  */

  if (get_ucs2 (&character, subtask))
    {
      const char *charname;	/* charname for code */
      bool french;		/* if output should be in French */
      const char *string;	/* environment value */

      /* Decide if we prefer French or English output.  */

      french = false;
      string = getenv ("LANGUAGE");
      if (string && string[0] == 'f' && string[1] == 'r')
	french = true;
      else
	{
	  string = getenv ("LANG");
	  if (string && string[0] == 'f' && string[1] == 'r')
	    french = true;
	}

      fputs (_("UCS2   Mne   Description\n\n"), stdout);

      while (1)
	{
	  const char *mnemonic = ucs2_to_rfc1345 (character);

	  printf ("%.4X", character);
	  if (mnemonic)
	    printf ("   %-3s", mnemonic);
	  else
	    fputs ("      ", stdout);

	  if (french)
	    {
	      charname = ucs2_to_french_charname (character);
	      if (!charname)
		charname = ucs2_to_charname (character);
	    }
	  else
	    {
	      charname = ucs2_to_charname (character);
	      if (!charname)
		charname = ucs2_to_french_charname (character);
	    }

	  if (charname)
	    {
	      fputs ("   ", stdout);
	      fputs (charname, stdout);
	    }
	  printf ("\n");

	  if (!get_ucs2 (&character, subtask))
	    break;
	}
    }

  SUBTASK_RETURN (subtask);
}

/*-----------------------------------------.
| Declare charsets, surfaces and aliases.  |
`-----------------------------------------*/

bool
module_testdump (RECODE_OUTER outer)
{
  /* Test surfaces.  */

  if (!declare_single (outer, "test7", "data",
		       outer->quality_variable_to_byte,
		       NULL, test7_data))
    return false;
  if (!declare_single (outer, "test8", "data",
		       outer->quality_variable_to_byte,
		       NULL, test8_data))
    return false;
  if (!declare_single (outer, "test15", "data",
		       outer->quality_variable_to_ucs2,
		       NULL, test15_data))
    return false;
  if (!declare_single (outer, "test16", "data",
		       outer->quality_variable_to_ucs2,
		       NULL, test16_data))
    return false;

  /* Analysis charsets.  */

  if (!declare_single (outer, "ISO-10646-UCS-2", "count-characters",
		       outer->quality_ucs2_to_variable,
		       NULL, produce_count))
    return false;
  if (!declare_single (outer, "ISO-10646-UCS-2", "dump-with-names",
		       outer->quality_ucs2_to_variable,
		       NULL, produce_full_dump))
    return false;

  return true;
}

void
delmodule_testdump (RECODE_OUTER outer)
{
}
