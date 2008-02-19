/* Conversion of files between different charsets and surfaces.
   Copyright © 1990,92,93,94,96,97,98,99,00 Free Software Foundation, Inc.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1990.

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

/*-----------------------------------------------------------------------.
| This dummy fallback routine is used to flag the intent of a reversible |
| coding as a fallback, which is the traditional `recode' behaviour.     |
`-----------------------------------------------------------------------*/

bool
reversibility (RECODE_SUBTASK subtask, unsigned code)
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
  RECODE_SINGLE single = new_single_step (outer);

  if (!single)
    return NULL;

  if (strcmp (before_name, "data") == 0)
    {
      single->before = outer->data_symbol;
      single->after = find_alias (outer, after_name,
				  SYMBOL_CREATE_DATA_SURFACE)->symbol;
    }
  else if (strcmp(after_name, "data") == 0)
    {
      single->before = find_alias (outer, before_name,
				   SYMBOL_CREATE_DATA_SURFACE)->symbol;
      single->after = outer->data_symbol;
    }
  else if (strcmp (before_name, "tree") == 0)
    {
      single->before = outer->tree_symbol;
      single->after = find_alias (outer, after_name,
				  SYMBOL_CREATE_TREE_SURFACE)->symbol;
    }
  else if (strcmp(after_name, "tree") == 0)
    {
      single->before = find_alias (outer, before_name,
				   SYMBOL_CREATE_TREE_SURFACE)->symbol;
      single->after = outer->tree_symbol;
    }
  else
    {
      single->before = find_alias (outer, before_name,
				   SYMBOL_CREATE_CHARSET)->symbol;
      single->after = find_alias (outer, after_name,
				  SYMBOL_CREATE_CHARSET)->symbol;
    }

#if 0
  /* FIXME: We should delink from the list of charsets before freeing.
     The alias should also be freed. */
  if (!before || !after)
    {
      free (single->before);
      return NULL;
    }
#endif

  single->quality = quality;
  single->init_routine = init_routine;
  single->transform_routine = transform_routine;

  if (single->before == outer->data_symbol
      || single->before == outer->tree_symbol)
    {
      if (single->after->resurfacer)
	recode_error (outer, _("Resurfacer set more than once for `%s'"),
		      after_name);
      single->after->resurfacer = single;
    }
  else if (single->after == outer->data_symbol
	   || single->after == outer->tree_symbol)
    {
      if (single->before->unsurfacer)
	recode_error (outer, _("Unsurfacer set more than once for `%s'"),
		      before_name);
      single->before->unsurfacer = single;
    }

  return single;
}

#if 0

/*-------------------------------------------.
| Create and initialize a new data surface.  |
`-------------------------------------------*/

bool
declare_data_surface (RECODE_OUTER outer, const char *name,
		      struct recode_quality resurfacer_quality,
		      Recode_init resurfacer_init_routine,
		      Recode_transform resurfacer_transform_routine,
		      struct recode_quality unsurfacer_quality,
		      Recode_init unsurfacer_init_routine,
		      Recode_transform unsurfacer_transform_routine)
{
}

/*-------------------------------------------------.
| Create and initialize a new structural surface.  |
`-------------------------------------------------*/

bool
declare_tree_surface (RECODE_OUTER outer, const char *name,
		      struct recode_quality resurfacer_quality,
		      Recode_init resurfacer_init_routine,
		      Recode_transform resurfacer_transform_routine,
		      struct recode_quality unsurfacer_quality,
		      Recode_init unsurfacer_init_routine,
		      Recode_transform unsurfacer_transform_routine)
{
}

#endif

/*---------------------------------------------------------------------------.
| Declare a charset available through `libiconv', given the NAME of this     |
| charset (which might already exist as an alias).  Make two single steps in |
| and out of it.                                                             |
`---------------------------------------------------------------------------*/

static bool
internal_iconv (RECODE_SUBTASK subtask)
{
  SET_SUBTASK_ERROR (RECODE_USER_ERROR, subtask);
  SUBTASK_RETURN (subtask);
}

bool
declare_libiconv (RECODE_OUTER outer, const char *name)
{
  RECODE_ALIAS alias;
  RECODE_SINGLE single;

  if (alias = find_alias (outer, name, ALIAS_FIND_AS_EITHER),
      !alias)
    if (alias = find_alias (outer, name, SYMBOL_CREATE_CHARSET),
	!alias)
      return false;
  assert(alias->symbol->type == RECODE_CHARSET);

  if (single = new_single_step (outer), !single)
    return false;
  single->before = alias->symbol;
  single->after = outer->libiconv_pivot;
  single->quality = outer->quality_variable_to_variable;
  single->init_routine = NULL;
  single->transform_routine = internal_iconv;

  if (single = new_single_step (outer), !single)
    return false;
  single->before = outer->libiconv_pivot;
  single->after = alias->symbol;
  single->quality = outer->quality_variable_to_variable;
  single->init_routine = NULL;
  single->transform_routine = internal_iconv;

  return true;
}

#if 0

/*------------------------------------------------.
| Save explode data within the symbol structure.  |
`------------------------------------------------*/

bool register_explode_data
  PARAMS ((RECODE_OUTER, const char *, const unsigned short *));

bool
register_explode_data (RECODE_OUTER outer,
		       const char *name, const unsigned short *data)
{
  RECODE_SYMBOL symbol;

  symbol = find_alias (outer, name, SYMBOL_CREATE_CHARSET);
  if (!symbol)
    return false;

  assert(symbol->type = RECODE_CHARSET);
  symbol->data_type = RECODE_EXPLODE_DATA;
  /* The cast is a way to silently discard the const.  */
  symbol->data = (void *) data;
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
  RECODE_ALIAS alias;
  RECODE_SYMBOL charset_combined;
  RECODE_SYMBOL charset_exploded;
  RECODE_SINGLE single;

  if (alias = find_alias (outer, name_combined, SYMBOL_CREATE_CHARSET),
      !alias)
    return false;

  charset_combined = alias->symbol;
  assert(charset_combined->type == RECODE_CHARSET);

  if (name_exploded)
    {
      if (alias = find_alias (outer, name_exploded, SYMBOL_CREATE_CHARSET),
	  !alias)
	return false;

      charset_exploded = alias->symbol;
      assert(charset_exploded->type == RECODE_CHARSET);
    }
  else
    {
      charset_combined->data_type = RECODE_EXPLODE_DATA;
      charset_combined->data = (void *) data;
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
  RECODE_ALIAS alias;
  RECODE_SYMBOL charset;
  RECODE_SINGLE single;

  if (alias = find_alias (outer, name, SYMBOL_CREATE_CHARSET), !alias)
    return false;

  charset = alias->symbol;
  assert(charset->type == RECODE_CHARSET);
  charset->data_type = RECODE_STRIP_DATA;
  charset->data = data;

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
bool module_libiconv PARAMS ((struct recode_outer *));
void delmodule_libiconv PARAMS ((struct recode_outer *));


static bool
register_all_modules (RECODE_OUTER outer)
{
  RECODE_ALIAS alias;
  RECODE_SINGLE single;
  unsigned counter;
  unsigned char *table;

  if (!ALLOC (table, 256, unsigned char))
    return false;
  for (counter = 0; counter < 256; counter++)
    table[counter] = counter;
  outer->one_to_same = table;

  prepare_for_aliases (outer);
  outer->single_list = NULL;
  outer->number_of_singles = 0;

  if (alias = find_alias (outer, "data", SYMBOL_CREATE_CHARSET), !alias)
    return false;
  outer->data_symbol = alias->symbol;

  if (alias = find_alias (outer, "tree", SYMBOL_CREATE_CHARSET), !alias)
    return false;
  outer->tree_symbol = alias->symbol;

  if (alias = find_alias (outer, "ISO-10646-UCS-2", SYMBOL_CREATE_CHARSET),
      !alias)
    return false;
  assert(alias->symbol->type == RECODE_CHARSET);
  outer->ucs2_charset = alias->symbol;

  if (alias = find_alias (outer, ":libiconv:", SYMBOL_CREATE_CHARSET),
      !alias)
    return false;
  assert(alias->symbol->type == RECODE_CHARSET);
  outer->libiconv_pivot = alias->symbol;
  if (!declare_alias (outer, ":", ":libiconv:"))
    return false;

  if (alias = find_alias (outer, "CR-LF", SYMBOL_CREATE_CHARSET), !alias)
    return false;
  alias->symbol->type = RECODE_DATA_SURFACE;
  outer->crlf_surface = alias->symbol;

  if (alias = find_alias (outer, "CR", SYMBOL_CREATE_CHARSET), !alias)
    return false;
  alias->symbol->type = RECODE_DATA_SURFACE;
  outer->cr_surface = alias->symbol;

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

  /* Force this one last: it does not segregate between charsets and aliases,
     confusing some other initialisations that would come after it.  */
  if (!make_argmatch_arrays (outer))
    return false;
  if (!module_libiconv (outer))
    return false;

  for (single = outer->single_list; single; single = single->next)
    estimate_single_cost (outer, single);

  return true;
}

void static
unregister_all_modules (RECODE_OUTER outer)
{
#include "tersteps.h"
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
      return NULL;
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
  unregister_all_modules (outer);
  /* FIXME: Pawel Krawczyk reports that calling new_outer ... delete_outer
     20000 times in a program has the effect of consuming all virtual memory.
     So there might be memory leaks should to track down and resolve.  */
  while (outer->number_of_symbols > 0)
    {
      RECODE_SYMBOL symbol = outer->symbol_list;

      outer->symbol_list = symbol->next;
      outer->number_of_symbols--;
      free (symbol);
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
  if (outer->alias_table)
    hash_free (outer->alias_table);
  if (outer->argmatch_charset_array)
    free (outer->argmatch_charset_array);
  if (outer->one_to_same)
    free ((void *) outer->one_to_same);
  free (outer);
  return true;
}
