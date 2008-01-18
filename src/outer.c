/* Conversion of files between different charsets and surfaces.
   Copyright © 1990, 92, 93, 94, 96, 97, 98, 99 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1990.

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

/*-----------------------------------------------------------------------.
| This dummy fallback routine is used to flag the intent of a reversible |
| coding as a fallback, which is the traditional `recode' behaviour.     |
`-----------------------------------------------------------------------*/

bool
reversibility (RECODE_CONST_STEP step, RECODE_TASK task, unsigned code)
{
  return false;
}

/*-------------------------------------------------------------------------.
| Allocate and initialize a new single step, save for the before and after |
| charsets and quality.							   |
`-------------------------------------------------------------------------*/

static RECODE_SINGLE
new_single_step (RECODE_OUTER outer)
{
  RECODE_SINGLE single;

  if (!ALLOC (single, 1, struct recode_single))
    return NULL;
  single->next = outer->single_list;
  outer->single_list = single;
  outer->number_of_singles++;

  single->initial_step_table = NULL;
  single->init_routine = NULL;
  single->transform_routine = NULL;
  single->fallback_routine = reversibility;

  return single;
}

/*-------------------------------------------------------------------------.
| Create and initialize a new single step for recoding between BEFORE_NAME |
| and AFTER_NAME.  Give it a recoding QUALITY, also saving an INIT_ROUTINE |
| and a TRANSFORM_ROUTINE functions.                                       |
`-------------------------------------------------------------------------*/

RECODE_SINGLE
declare_single (RECODE_OUTER outer,
		const char *before_name, const char *after_name,
		struct recode_quality quality,
		Recode_init init_routine, Recode_transform transform_routine)
{
  RECODE_SYMBOL symbol;
  RECODE_SINGLE single;

  single = new_single_step (outer);
  if (!single)
    return NULL;

  if (symbol = find_symbol (outer, before_name, SYMBOL_CREATE_CHARSET), !symbol)
    return NULL;
  single->before = symbol->charset;

  if (symbol = find_symbol (outer, after_name, SYMBOL_CREATE_CHARSET), !symbol)
    {
#if 0
      /* FIXME: We should delink from the list of charsets before freeing.
	 The symbol should also be freed. */
      free (single->before);
#endif
      return NULL;
    }
  single->after = symbol->charset;

  if (single->before == outer->data_charset)
    {
      if (single->after->resurfacer)
	recode_error (outer, _("Resurfacer set more than once for `%s'"),
		      after_name);
      single->after->resurfacer = single;
    }
  else if (single->after == outer->data_charset)
    {
      if (single->before->unsurfacer)
	recode_error (outer, _("Unsurfacer set more than once for `%s'"),
		      before_name);
      single->before->unsurfacer = single;
    }
  else
    {
      single->before->charset_flag = true;
      single->after->charset_flag = true;
    }

  single->quality = quality;
  single->init_routine = init_routine;
  single->transform_routine = transform_routine;

  return single;
}

#if 0

/*-------------------------------------------------.
| Save explode data within the charset structure.  |
`-------------------------------------------------*/

bool register_explode_data
  PARAMS ((RECODE_OUTER, const char *, const unsigned short *));

bool
register_explode_data (RECODE_OUTER outer,
		       const char *name, const unsigned short *data)
{
  RECODE_CHARSET charset;

  charset = find_symbol (outer, name, SYMBOL_CREATE_CHARSET);
  if (!charset)
    return false;

  charset->charset_flag = true;
  charset->charset_type = RECODE_EXPLODE_DATA;
  /* The cast is a way to silently discard the const.  */
  charset->charset_data = (void *) data;
  return true;
}

#endif

/*--------------------------------------------------------------------------.
| Associate an explode format DATA structure with charset NAME_COMBINED, an |
| 8-bit charset.  A NULL value for NAME_EXPLODED implies UCS-2.  Otherwise, |
| NAME_EXPLODED should be the name of a 8-bit based charset.                |
`--------------------------------------------------------------------------*/

bool
declare_explode_data (RECODE_OUTER outer, const unsigned short *data,
		      const char *name_combined, const char *name_exploded)
{
  RECODE_SYMBOL symbol;
  RECODE_CHARSET charset_combined;
  RECODE_CHARSET charset_exploded;
  RECODE_SINGLE single;

  if (symbol = find_symbol (outer, name_combined, SYMBOL_CREATE_CHARSET),
      !symbol)
    return false;

  charset_combined = symbol->charset;
  charset_combined->charset_flag = true;

  if (name_exploded)
    {
      if (symbol = find_symbol (outer, name_exploded, SYMBOL_CREATE_CHARSET),
	  !symbol)
	return false;

      charset_exploded = symbol->charset;
      charset_exploded->charset_flag = true;
    }
  else
    {
      charset_combined->charset_type = RECODE_EXPLODE_DATA;
      charset_combined->charset_data = (void *) data;
      charset_exploded = outer->ucs2_charset;
    }

  single = new_single_step (outer);
  if (!single)
    return false;

  single->before = charset_combined;
  single->after = charset_exploded;
  single->quality = outer->quality_byte_to_variable;
  single->initial_step_table = (void *) data;
  single->init_routine = init_explode;
  single->transform_routine
    = name_exploded ? explode_byte_byte : explode_byte_ucs2;

  single = new_single_step (outer);
  if (!single)
    return false;

  single->before = charset_exploded;
  single->after = charset_combined;
  single->quality = outer->quality_variable_to_byte;
  single->initial_step_table = (void *) data;
  single->init_routine = init_combine;
  single->transform_routine
    = name_exploded ? combine_byte_byte : combine_ucs2_byte;

  return true;
}

/*-------------------------------------------------------------------.
| Associate an UCS-2 strip format DATA structure with charset NAME.  |
`-------------------------------------------------------------------*/

bool
declare_strip_data (RECODE_OUTER outer, struct strip_data *data,
		    const char *name)
{
  RECODE_SYMBOL symbol;
  RECODE_CHARSET charset;
  RECODE_SINGLE single;

  if (symbol = find_symbol (outer, name, SYMBOL_CREATE_CHARSET), !symbol)
    return false;

  charset = symbol->charset;
  charset->charset_flag = true;
  charset->charset_type = RECODE_STRIP_DATA;
  charset->charset_data = data;

  single = new_single_step (outer);
  if (!single)
    return false;

  single->before = charset;
  single->after = outer->ucs2_charset;
  single->quality = outer->quality_byte_to_ucs2;
  single->transform_routine = transform_byte_to_ucs2;

  single = new_single_step (outer);
  if (!single)
    return false;

  single->before = outer->ucs2_charset;
  single->after = charset;
  single->quality = outer->quality_ucs2_to_byte;
  single->init_routine = init_ucs2_to_byte;
  single->transform_routine = transform_ucs2_to_byte;

  return true;
}

/*---------------------------------------------------------------.
| For a given SINGLE step, roughly establish a conversion cost.  |
`---------------------------------------------------------------*/

static void
estimate_single_cost (RECODE_OUTER outer, RECODE_SINGLE single)
{
  int cost;

  /* Ensure a small average cost for each single step, yet much trying to
     avoid single steps prone to loosing information.  */

  cost = single->quality.reversible ? 10 : 200;

  /* Use a few heuristics based on the byte size of both charsets.  */

  switch (single->quality.in_size)
    {
    case RECODE_1:
      /* The fastest is to get one character per read byte.  */
      cost += 15;
      break;

    case RECODE_2:
      /* Reading two require a routine call and swapping considerations.  */
      cost += 25;
      break;

    case RECODE_4:
      /* Reading four is more work than reading two.  */
      cost += 30;
      break;

    case RECODE_N:
      /* Analysing varysizes is surely much harder than producing them.  */
      cost += 60;
    }

  switch (single->quality.out_size)
    {
    case RECODE_1:
      /* Information might be more often lost when not going through UCS.  */
      cost += 20;
      break;

    case RECODE_2:
      /* This is our best bet while writing.  */
      cost += 10;
      break;

    case RECODE_4:
      /* Writing four is more work than writing two.  */
      cost += 15;
      break;

    case RECODE_N:
      /* Writing varysizes requires loops and such.  */
      cost += 35;
      break;
    }

  /* Consider speed for fine tuning the cost.  */

  if (single->quality.slower)
    cost += 3;
  else if (single->quality.faster)
    cost -= 2;

  /* Write the price on the ticket.  */

  single->conversion_cost = cost;
  return;
}

/*------------------------------------------------------------------------.
| Initialize all collected single steps.  If STRICT_MAPPING is true, many |
| recodings loose their reversibility.                                    |
`------------------------------------------------------------------------*/

#include "decsteps.h"

static bool
register_all_modules (RECODE_OUTER outer)
{
  RECODE_SYMBOL symbol;
  RECODE_SINGLE single;
  unsigned counter;
  unsigned char *table;

  if (!ALLOC (table, 256, unsigned char))
    return false;
  for (counter = 0; counter < 256; counter++)
    table[counter] = counter;
  outer->one_to_same = table;

  prepare_for_names (outer);
  outer->single_list = NULL;
  outer->number_of_singles = 0;

  if (symbol = find_symbol (outer, "data", SYMBOL_CREATE_CHARSET), !symbol)
    return false;
  outer->data_charset = symbol->charset;

  if (symbol = find_symbol (outer, "ISO-10646-UCS-2", SYMBOL_CREATE_CHARSET),
      !symbol)
    return false;
  outer->ucs2_charset = symbol->charset;

  if (symbol = find_symbol (outer, "CR-LF", SYMBOL_CREATE_CHARSET), !symbol)
    return false;
  outer->crlf_surface = symbol->charset;

  if (symbol = find_symbol (outer, "CR", SYMBOL_CREATE_CHARSET), !symbol)
    return false;
  outer->cr_surface = symbol->charset;

#if 0
  /* `.' might be later used for asking recode to guess the charset.  */
  if (!declare_alias (outer, ".", "RFC 1345"))
    return false;
#endif
  if (!declare_alias (outer, "ASCII", "ANSI_X3.4-1968"))
    return false;
  if (!declare_alias (outer, "BS", "ASCII-BS"))
    return false;
  if (!declare_alias (outer, "Latin-1", "ISO-8859-1"))
    return false;

  /* Needed for compatibility with recode version 3.2.  */
  if (!declare_alias (outer, "lat1", "Latin-1"))
    return false;

#include "inisteps.h"

  for (single = outer->single_list; single; single = single->next)
    estimate_single_cost (outer, single);

  return true;
}

/* Library interface.  */

/* See the recode manual for a more detailed description of the library
   interface.  */

/*-------------------------.
| GLOBAL level functions.  |
`-------------------------*/

RECODE_OUTER
recode_new_outer (bool auto_abort)
{
  RECODE_OUTER outer = malloc (sizeof (struct recode_outer));

  if (!outer)
    {
      /* Diagnostic?  FIXME!  */
      if (auto_abort)
	exit (1);
      return NULL;
    }

  memset (outer, 0, sizeof (struct recode_outer));
  outer->auto_abort = auto_abort;

  if (!register_all_modules (outer) || !make_argmatch_arrays (outer))
    {
      recode_delete_outer (outer);
      return false;
    }

  outer->quality_byte_reversible.in_size = RECODE_1;
  outer->quality_byte_reversible.out_size = RECODE_1;
  outer->quality_byte_reversible.reversible = true;
  outer->quality_byte_reversible.faster = true;

  outer->quality_byte_to_byte.in_size = RECODE_1;
  outer->quality_byte_to_byte.out_size = RECODE_1;
  outer->quality_byte_to_byte.faster = true;

  outer->quality_byte_to_ucs2.in_size = RECODE_1;
  outer->quality_byte_to_ucs2.out_size = RECODE_2;

  outer->quality_byte_to_variable.in_size = RECODE_1;
  outer->quality_byte_to_variable.out_size = RECODE_N;

  outer->quality_ucs2_to_byte.in_size = RECODE_2;
  outer->quality_ucs2_to_byte.out_size = RECODE_1;

  outer->quality_ucs2_to_variable.in_size = RECODE_2;
  outer->quality_ucs2_to_variable.out_size = RECODE_N;

  outer->quality_variable_to_byte.in_size = RECODE_N;
  outer->quality_variable_to_byte.out_size = RECODE_1;
  outer->quality_variable_to_byte.slower = true;

  outer->quality_variable_to_ucs2.in_size = RECODE_N;
  outer->quality_variable_to_ucs2.out_size = RECODE_2;
  outer->quality_variable_to_ucs2.slower = true;

  outer->quality_variable_to_variable.in_size = RECODE_N;
  outer->quality_variable_to_variable.out_size = RECODE_N;
  outer->quality_variable_to_variable.slower = true;

  return outer;
}

bool
recode_delete_outer (RECODE_OUTER outer)
{
  /* FIXME: Pawel Krawczyk reports that calling new_outer ... delete_outer
     20000 times in a program has the effect of consuming all virtual memory.
     So there might be memory leaks should to track down and resolve.  */
  while (outer->number_of_charsets > 0)
    {
      RECODE_CHARSET charset = outer->charset_list;

      outer->charset_list = charset->next;
      outer->number_of_charsets--;
      free (charset);
    }
  while (outer->number_of_singles > 0)
    {
      RECODE_SINGLE single = outer->single_list;

      outer->single_list = single->next;
      outer->number_of_singles--;
      free (single);
    }
  if (outer->pair_restriction)
    free (outer->pair_restriction);
  if (outer->symbol_table)
    free (outer->symbol_table);
  if (outer->argmatch_charset_array)
    free (outer->argmatch_charset_array);
#if 0
  if (outer->one_to_same)
    free (outer->one_to_same);
#endif
  free (outer);
  return true;
}
