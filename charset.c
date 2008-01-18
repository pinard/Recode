/* Conversion of files between different charsets and usages.
   Copyright (C) 1993, 1994 Free Software Foundation, Inc.
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

/* Maximum number of charset values.  */
#define MAX_CHARSETS 200

/* Hash table size for charset names.  */
#define HASH_TABLE_SIZE 997

/* Maximum number of characters per 10646 symbol.  */
#define MAX_SYMBOL_SIZE 9

/* Known pairs (for restricting listing).  */

static struct known_pair *pair_restriction = NULL;
static int pair_restrictions = 0;

/* Known character sets.  */

struct hash
  {
    const char *name;		/* charset or alias name, or NULL */
    CHARSET *charset;		/* associated charset */
    struct hash *next;		/* next index in table, or NULL */
  };
struct hash hash_table[HASH_TABLE_SIZE];
CHARSET charset_array[MAX_CHARSETS];
int number_of_charsets;

/* Array of strings ready for argmatch.  */
static const char **argmatch_array;

/* Character names.  */

/* This module takes care only of short 10646 forms.  Module charname.c
   takes care of the full descriptive name for characters.  */

/*--------------------------------------------------------------------.
| Return a statically allocated 10646 symbol in a CHARSET for a given |
| CODE, or NULL if this symbol is not defined.  There are two static  |
| buffers used in alternance.					      |
`--------------------------------------------------------------------*/

static char *
code_to_symbol (CHARSET *charset, int code)
{
  static char buffer[2][MAX_SYMBOL_SIZE + 1];
  static int which = 0;
  const char *in;
  char *out;
  int counter;

  if (in = (*charset->table)[code / 32], !in)
    return NULL;

  in += charset->size * (code % 32);
  if (*in == ' ')
    return NULL;

  which = !which;
  out = buffer[which];
  for (counter = 0; counter < charset->size; counter++)
    if (*in == ' ')
      in++;
    else
      *out++ = *in++;
  *out = '\0'; 
  return buffer[which];
}

/*------------------------------------------------------------------------.
| Print a 10646 symbol in a CHARSET for a given CODE, padding with spaces |
| after to the proper width.						  |
`------------------------------------------------------------------------*/

static void
print_symbol (CHARSET *charset, int code)
{
  int counter;
  char *cursor;

  counter = 0;
  cursor = code_to_symbol (charset, code);
  
  if (cursor)
    for (; *cursor && counter < charset->size; counter++)
      {
	putchar (*cursor);
	cursor++;
      }
  for (; counter < charset->size; counter++)
    putchar (' ');
}

/*-----------------------------------------------------------------.
| Decode a known PAIRS argument, given in STRING, constructing the |
| pair_restriction array out of it.				   |
`-----------------------------------------------------------------*/

void
decode_known_pairs (const char *string)
{
  struct known_pair pair;
  const char *cursor;
  int value;

  pair_restriction = (struct known_pair *)
    xmalloc (16 * sizeof (struct known_pair));

  value = -1;
  for (cursor = string; *cursor; cursor++)
    switch (*cursor)
      {
      default:
	usage (EXIT_FAILURE);

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
	if (value < 0)
	  value = *cursor - '0';
	else
	  value = 10 * value + *cursor - '0';
	break;

      case ':':
	if (value < 0 || value > 255)
	  usage (EXIT_FAILURE);
	pair_restriction[pair_restrictions].left = (unsigned char) value;
	value = -1;
	break;

      case ',':
	if (value < 0 || value > 255)
	  usage (EXIT_FAILURE);
	pair_restriction[pair_restrictions++].right = (unsigned char) value;
	if (pair_restrictions % 16 == 0)
	  pair_restriction = (struct known_pair *)
	    xrealloc (pair_restriction,
		      (pair_restrictions + 16) * sizeof (struct known_pair));
	value = -1;
	break;
      }

  if (value < 0 || value > 255)
    usage (EXIT_FAILURE);
  pair_restriction[pair_restrictions++].right = (unsigned char) value;
}

/*-------------------------------------------------------------.
| Return non-zero if BEFORE to AFTER is currently restricted.  |
`-------------------------------------------------------------*/

static int
check_restricted (CHARSET *before, CHARSET *after)
{
  struct known_pair *pair;
  const char *left;
  const char *right;

  /* Reject the charset if not RFC1345.  */

  if (!before->table || !after->table)
    return 1;

  for (pair = pair_restriction;
       pair < pair_restriction + pair_restrictions;
       pair++)
    {

      /* Reject the charset if the characters in the pair do not exist of
	 if their respective definition do not match.  */

      left = code_to_symbol (before, pair->left);
      if (!left)
	return 1;
      right = code_to_symbol (after, pair->right);
      if (!right)
	return 1;
      if (strcmp (left, right))
	return 1;
    }

  /* No restriction found.  */

  return 0;
}

/* Charset names.  */

/*--------------------------------------.
| Prepare charsets for initialization.  |
`--------------------------------------*/

void
prepare_charset_initialization (void)
{
  int counter;

  for (counter = 0; counter < HASH_TABLE_SIZE; counter++)
    {
      hash_table[counter].name = NULL;
      hash_table[counter].next = NULL;
    }
  number_of_charsets = 0;
}

/*-----------------------------------------------------------------------.
| Return a newly allocated copy of charset NAME, with upper case letters |
| turned into lower case, and all non alphanumeric discarded.		 |
`-----------------------------------------------------------------------*/

static char *
cleanup_charset_name (const char *name)
{
  char *result;
  char *out;
  const char *in;
  int character;

  result = xmalloc (strlen (name) + 1);
  out = result;
  for (in = name; *in; in++)
    {
      character = *(const unsigned char *) in;
      if ((character >= 'a' && character <= 'z')
	  || (character >= '0' && character <= '9'))
	*out++ = character;
      else if (character >= 'A' && character <= 'Z')
	*out++ = character - 'A' + 'a';
    }
  *out = '\0';
  return result;
}

/*-----------------------------------.
| Return a hash index for a STRING.  |
`-----------------------------------*/

#ifdef DIFF_HASH

/* Given a hash value and a new character, return a new hash value.  */

#define UINT_BIT (sizeof (unsigned) * CHAR_BIT)
#define ROTATE_LEFT(v, n) ((v) << (n) | (v) >> (UINT_BIT - (n)))
#define HASH(h, c) ((c) + ROTATE_LEFT (h, 7))

static int
hash_string (const char *string)
{
  unsigned value;

  value = 0;
  for (; *string; string++)
    value = HASH (value, *(const unsigned char *) string);
  return value % HASH_TABLE_SIZE;
}

#else /* not DIFF_HASH */

static int
hash_string (const char *string)
{
  unsigned value;

  value = 0;
  for (; *string; string++)
    value = ((value * 31 + (int) *(const unsigned char *) string)
	     % HASH_TABLE_SIZE);
  return value;
}

#endif /* not DIFF_HASH */

/*--------------------------------------------------------------------------.
| Return the charset from its NAME or alias name.  If it does not already   |
| exist, add a new charset entry and initialize it with a brand new value.  |
`--------------------------------------------------------------------------*/

CHARSET *
find_charset (const char *name)
{
  char *hashname;
  struct hash *hash;
  CHARSET *charset;

  /* Search the whole hash bucket and return any match.  */

  hashname = cleanup_charset_name (name);
  for (hash = hash_table + hash_string (hashname);
       hash->name;
       hash = hash->next)
    {
      if (strcmp (hashname, hash->name) == 0)
	{
	  free (hashname);
	  return hash->charset;
	}
      if (!hash->next)
	break;
    }

  /* A new charset has to be created.  */

  if (number_of_charsets == MAX_CHARSETS)
    error (EXIT_FAILURE, 0, "MAX_CHARSETS is too small");

  charset = charset_array + number_of_charsets++;

  /* If the current slot is already used, create an overflow entry and
     initialize it enough so it could be taken for the current slot.  */

  if (hash->name)
    {
      hash->next = (struct hash *) xmalloc (sizeof (struct hash));
      hash = hash->next;
      hash->next = NULL;
    }

  /* Initialize the current slot with the new charset.  */

  hash->name = hashname;
  hash->charset = charset;

  charset->name = name;
  charset->ignore = 0;
  charset->table = NULL;

  return charset;
}

/*-------------------------------------------------------------------------.
| Have NAME as an alternate charset name for OLD_NAME.  Create OLD_NAME if |
| it does not exist already.						   |
`-------------------------------------------------------------------------*/

void
declare_alias (const char *name, const char *old_name)
{
  char *hashname;
  struct hash *hash;
  CHARSET *old_charset;

  /* Find the old value.  */

  old_charset = find_charset (old_name);

  /* Search the whole hash bucket.  */

  hashname = cleanup_charset_name (name);
  for (hash = hash_table + hash_string (hashname);
       hash->name;
       hash = hash->next)
    {
      if (strcmp (hashname, hash->name) == 0)
	{
	  if (hash->charset != old_charset)
	    error (EXIT_FAILURE, 0, "Charset %s already exists and is not %s",
		   name, old_name);
	  free (hashname);
	  return;
	}
      if (!hash->next)
	break;
    }

  /* If the current slot is already used, create an overflow entry and
     initialize it enough so it could be taken for the current slot.  */

  if (hash->name)
    {
      hash->next = (struct hash *) xmalloc (sizeof (struct hash));
      hash = hash->next;
      hash->next = NULL;
    }

  /* Initialize the current slot with the old charset.  */

  hash->name = hashname;
  hash->charset = old_charset;
}

/*------------------------------------------.
| Construct the string array for argmatch.  |
`------------------------------------------*/

void
make_argmatch_array (void)
{
  struct hash *hash;		/* cursor in charsets */
  int number;			/* number of strings */
  int counter;			/* all purpose counter */
#ifdef HASH_STATS
  int buckets;			/* number of non-empty buckets */
#endif

  /* Count how many strings we need.  */

  number = 0;
  for (counter = 0; counter < HASH_TABLE_SIZE; counter++)
    for (hash = hash_table + counter;
	 hash && hash->name;
	 hash = hash->next)
      number++;

#ifdef HASH_STATS
  buckets = 0;
  for (counter = 0; counter < HASH_TABLE_SIZE; counter++)
    if (hash_table[counter].name)
      buckets++;

  fprintf (stderr, "Hash stats: %d names using %d buckets out of %d\n",
	   number, buckets, HASH_TABLE_SIZE);
#endif

  /* Allocate the argmatch array, with place for a NULL sentinel.  */

  argmatch_array
    = (const char **) xmalloc ((number + 1) * sizeof (const char *));

  /* Fill in the array.  */

  number = 0;
  for (counter = 0; counter < HASH_TABLE_SIZE; counter++)
    for (hash = hash_table + counter;
	 hash && hash->name;
	 hash = hash->next)
      argmatch_array[number++] = hash->name;

  argmatch_array[number] = NULL;
}

/*-----------------------------------------------------------------------.
| Return the NAME of a charset, un-abbreviated and cleaned up.  Diagnose |
| and abort if this cannot be done successfully.  A NULL or empty string |
| means the default charset, if this default charset is defined.	 |
`-----------------------------------------------------------------------*/

const char *
clean_charset_name (const char *name)
{
  char *hashname;
  int ordinal;

  /* Look for a match.  */

  if (!name)
    name = "";
#ifdef DEFAULT_CHARSET
  if (!*name)
    name = DEFAULT_CHARSET;
#endif
  hashname = cleanup_charset_name (name);
  ordinal = argmatch (hashname, argmatch_array);
  free (hashname);

  /* Diagnose any match error, notifying usage that we are decoding
     charsets.  */

  switch (ordinal)
    {
    case -2:
      error (0, 0, "Ambiguous charset `%s'", name);
      decoding_charset_flag = 1;
      usage (EXIT_FAILURE);

    case -1:
      error (0, 0, "Unknown charset `%s'", name);
      decoding_charset_flag = 1;
      usage (EXIT_FAILURE);
    }

  return argmatch_array[ordinal];
}

/*----------------------------------------------------------------------.
| Order two struct hash's, using the true charset name as the first key |
| and the current name as the second key.			        |
`----------------------------------------------------------------------*/

static int
compare_struct_hash (const void *void_first, const void *void_second)
{
  int value;

  value = strcmp (((const struct hash *) void_first)->charset->name,
		  ((const struct hash *) void_second)->charset->name);
  if (value != 0)
    return value;
  
  value = strcmp (((const struct hash *) void_first)->name,
		  ((const struct hash *) void_second)->name);
  return value;
}

/*-----------------------------------------------------------------------.
| List all available charsets, obeying restrictions for an AFTER charset |
| if any.								 |
`-----------------------------------------------------------------------*/

void
list_all_charsets (CHARSET *after)
{
  struct hash *array;
  struct hash *hash;
  int number;
  int counter;
  int list_flag;

  /* Count how many charsets we have.  */

  number = 0;
  for (counter = 0; counter < HASH_TABLE_SIZE; counter++)
    for (hash = hash_table + counter;
	 hash && hash->name;
	 hash = hash->next)
      number++;

  /* Allocate a structure to hold them.  */

  array = (struct hash *) xmalloc (number * sizeof (struct hash));

  /* Copy all charsets in it.  */

  number = 0;
  for (counter = 0; counter < HASH_TABLE_SIZE; counter++)
    for (hash = hash_table + counter;
	 hash && hash->name;
	 hash = hash->next)
      array[number++] = *hash;

  /* Sort it.  */

  qsort (array, number, sizeof (struct hash), compare_struct_hash);

  /* Print it, one line per charset, giving the true charset name first,
     followed by all its alias in lexicographic order.  */

  for (hash = array; hash < array + number; hash++)
    {

      /* Begin a new line with the true charset name when it changes.  */

      if (hash == array || hash->charset->name != (hash - 1)->charset->name)
	{
	  if (list_flag && hash != array)
	    printf ("\n");

	  list_flag = !after || !check_restricted (hash->charset, after);

	  if (list_flag)
	    printf ("%s", hash->charset->name);
	}

      /* Print the charset name or alias in its cleaned up form.  */

      if (list_flag)
	printf (" %s", hash->name);
    }
  if (list_flag)
    printf ("\n");

  /* Release the work array.  */

  free (array);
}

/* Charset contents.  */

/*-----------------------------------------------------------------------.
| For a given STEP recoding into a RFC 1345 charset, produce an explicit |
| recoding table.							 |
`-----------------------------------------------------------------------*/

void
init_table_for_rfc1345 (STEP *step)
{
  const char *symbol;
  char *pool;
  const char **table;
  int length;
  int counter;

  /* First compute how much memory is needed.  */

  length = 0;
  for (counter = 0; counter < 256; counter++)
    {
      symbol = code_to_symbol (step->before, counter);
      if (symbol)
	length += strlen (symbol) + 1;
    }

  /* Do the actual allocation and filling.  */

  table = (const char **) xmalloc (256 * sizeof (char *) + length);
  pool = (char *) (table + 256);

  for (counter = 0; counter < 256; counter++)
    {
      symbol = code_to_symbol (step->before, counter);
      if (symbol)
	{
	  if (strcmp (symbol, "SP") == 0)
	    symbol = " ";
	  else if (strcmp (symbol, "LF") == 0)
	    symbol = "\n";

	  table[counter] = pool;
	  while (*pool++ = *symbol++)
	    ;
	}
      else
	table[counter] = NULL;
    }

  step->one_to_many = table;
}

/*------------------------------------------------------------------.
| Print a concise, tabular CHARSET description on standard output.  |
`------------------------------------------------------------------*/

void
list_concise_charset (CHARSET *charset)
{
  DOUBLE_TABLE *table;		/* double table */
  int half;			/* half 0, half 1 of the table */
  const char *format;		/* format string */
  int counter;			/* code counter */
  int counter2;			/* code counter */
  int code;			/* code value */

  /* Ensure we have a double table to play with.  */

  if (charset->table)
    table = charset->table;
  else
    error (EXIT_FAILURE, 0,
	   "Cannot list `%s', no 10646 names available for this charset",
	   charset->name);

  printf ("%s\n", charset->name);

  /* Select format for numbers.  */

  switch (list_format)
    {
    default:
      return;			/* cannot happen */

    case NO_FORMAT:
    case DECIMAL_FORMAT:
      format = "%3d";
      break;

    case OCTAL_FORMAT:
      format = "%0.3o";
      break;

    case HEXADECIMAL_FORMAT:
      format = "%0.2x";
      break;
    }

  /* Print both halves of the table.  */

  for (half = 0; half < 2; half++)
    {

      /* Skip printing this half if it is empty.  */

      for (counter = 4 * half; counter < 4 * half + 4; counter++)
	if ((*table)[counter])
	  break;
      if (counter == 4 * half + 4)
	continue;

      /* Print this half.  */

      printf ("\n");
      for (counter = 128 * half; counter < 128 * half + 16; counter++)
	for (counter2 = 0; counter2 < 128; counter2 += 16)
	  {
	    if (counter2 > 0)
	      printf ("  ");

	    code = counter + counter2;
	    printf (format, code);
	    printf (" ");
	    print_symbol (charset, code);

	    if (counter2 == 112)
	      printf ("\n");
	  }
    }
}

/*------------------------------------------------------.
| Print a full CHARSET description on standard output.  |
`------------------------------------------------------*/

void
list_full_charset (CHARSET *charset)
{
  int insert_white;		/* insert a while line before printing */
  int code;			/* code counter */
  const char *symbol;		/* symbol for code */
  const char *charname;		/* charname for code */

  /* Ensure we have a double table to play with.  */

  if (!charset->table)
    error (EXIT_FAILURE, 0,
	   "Sorry, no 10646 names available for `%s'", charset->name);

  /* Print the long table.  */

  printf ("dec  oct hex    ch   %s\n", charset->name);
  insert_white = 1;

  for (code = 0; code < 256; code++)
    if ((symbol = code_to_symbol (charset, code)), symbol)
      {
	if (insert_white)
	  {
	    printf ("\n");
	    insert_white = 0;
	  }
	printf ("%3d  %0.3o  %0.2x    ", code, code, code);
	print_symbol (charset, code);
	if ((charname = symbol_to_charname (symbol)), charname)
	  printf ("   %s", charname);
	printf ("\n");
      }
    else
      insert_white = 1;
}
