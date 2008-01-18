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
#include "hash.h"

/* FIXME: Cleanup memory at end of job, and softly report errors.  */

/* Date: 1998-12-11 20:20 -0500
X-From-Line: recode-forum-owner@IRO.UMontreal.CA  Fri Dec 11 21:22:51 1998
Return-Path: <recode-forum-owner@iro.umontreal.ca>
Received: from degusse.IRO.UMontreal.CA (degusse.IRO.UMontreal.CA [132.204.24.51])
	by lagrande.IRO.UMontreal.CA (8.9.1/8.9.1) with ESMTP id VAA08222
	for <pinard@lagrande.IRO.UMontreal.CA>; Fri, 11 Dec 1998 21:22:51 -0500 (EST)
Received: by degusse.IRO.UMontreal.CA (8.9.1/8.9.1) id VAA11542
	for recode-forum-outgoing; Fri, 11 Dec 1998 21:22:44 -0500 (EST)
Received: from jupiter.rtsq.qc.ca (rtsq.grics.qc.ca [199.84.132.81])
	by degusse.IRO.UMontreal.CA (8.9.1/8.9.1) with ESMTP id VAA11529
	for <recode-forum@iro.umontreal.ca>; Fri, 11 Dec 1998 21:22:38 -0500 (EST)
Received: from ariel.progiciels-bpi.ca by jupiter.rtsq.qc.ca (8.8.8/8.8.8) with SMTP id VAA10935 for <@jupiter.rtsq.qc.ca:recode-forum@iro.umontreal.ca>; Fri, 11 Dec 1998 21:22:10 -0500
Received: from icule.progiciels-bpi.ca (uucp@localhost) by ariel.progiciels-bpi.ca (950413.SGI.8.6.12/950213.SGI) via UUCP id VAA17236 for recode-forum@iro.umontreal.ca; Fri, 11 Dec 1998 21:23:51 -0800
Received: by icule.progiciels-bpi.ca (8.8.8/8.8.8) id UAA05839; Fri, 11 Dec 1998 20:21:00 -0500
X-Face: "b_m|CE6#'Q8fliQrwHl9K,]PA_o'*S~Dva{~b1n*)K*A(BIwQW.:LY?t4~xhYka_.LV?Qq
 `}X|71X0ea&H]9Dsk!`kxBXlG;q$mLfv_vtaHK_rHFKu]4'<*LWCyUe@ZcI6"*wB5M@[m<Ok5/cC^=
 CxDhg=TJi^o[E
Message-ID: <oq90ge17gk.fsf@icule.progiciels-bpi.ca>
Mime-Version: 1.0
Content-Type: text/plain; charset=iso-8859-1
Content-Transfer-Encoding: 8bit
Sender: owner-recode-forum@IRO.UMontreal.CA
Precedence: bulk
Lines: 49
Xref: titan.progiciels-bpi.ca entretien.recode:1739

   Let me take a few minutes of rest, and share with you the state of combining
   in `recode'.  Fun enough, my initial writing of it, which was using some
   space while it was boiling, reduced to something small and rather simple
   after some time, from merging of structures and functions, and removal of
   some limits.  It does not even show, now, that it once was more complicated!

   The satisfactory aspects are that `recode' is now able to combine a set of
   sequence of UCS-2 characters into single codes, or explode those single
   codes into the original sequence.  It may happen that many sequences
   reduce to the same code, one of them is arbitrarily taken as canonical.
   Any combining sequence of an equivalent set produces the equivalent code,
   but at explode time, codes are always turned into their canonical sequence.
   I did not limit the length of a combining sequence, yet it is usually small.
   Also, I did not put any limit on the number of possibly equivalent sequences.
   For combining, for each possible partial match in any sequence, there is
   a state.  States corresponding to the scan of a single code are found by
   hash coding, while other states are linearly searched on a linked list of
   possible shifts, rooted on the state corresponding to the partial sequence,
   but without its last code.  I expect this combination of methods to execute
   reasonably fast.  Exploding is much simpler, and I merely use hash coding.

   The less satisfactory aspects are that the user interface is still
   very rude.  For the time being, I merely added a "combined" charset:
   combining is done by `u2..co', exploding is done by `co..u2'.  We surely
   could do much better, and build on the concept of options (introduced by
   `+' in the request syntax), Options are probably going to be used for other
   transliteration matters.  Even if built on UCS-2, combining matters would
   have natural extensions in other _smaller_ charsets, and it would be nice
   being able to derive the functionality automatically.  One difficulty to
   foresee is that, contrarily to surfaces, options like transliteration or
   combining _require_ specific charsets (like UCS-2) as a vehicle, and if
   UCS-2 is not otherwise mandated in the request, we would have to create
   such an UCS-2 intermediate.  On the other hand, if UCS-2 is sandwidched
   somewhere in a complex series of steps, we should implicitly propagate
   options towards the UCS-2 step.  Even if nothing looks utterly difficult,
   I guess that for doing it right, a lot of attention and care is needed.
   Also, I'm pretty sure that scrutinizing the documents (from w3.org or
   Unicode) that were suggested to us will raise new specific concerns.

   In any case, I'll try to clean a bit more around, for a few hours or maybe
   a few days, who knows :-), and prepare a new pretest offering "combined".
   Hmph!  Some documentation, even if terse, would also be welcome.  More
   later.  */

/* Exploding.  */

/*---------------------------.
| Initialize for exploding.  |
`---------------------------*/

static unsigned
combined_hash (const void *void_data, unsigned table_size)
{
  const unsigned short *data = void_data;

  return *data % table_size;
}

static bool
combined_compare (const void *void_first, const void *void_second)
{
  const unsigned short *first = void_first;
  const unsigned short *second = void_second;

  return *first == *second;
}

bool
init_explode (RECODE_STEP step,
	      RECODE_CONST_REQUEST request,
	      RECODE_CONST_OPTION_LIST before_options,
	      RECODE_CONST_OPTION_LIST after_options)
{
  const unsigned short *data = step->step_table;
  Hash_table *table;

  if (before_options || after_options)
    return false;

  table = hash_initialize (0, NULL, combined_hash, combined_compare, NULL);

  if (!table)
    return false;
  step->step_type = RECODE_EXPLODE_STEP;
  step->step_table = table;

  if (!data)
    return true;

  while (*data != DONE)
    {
      if (!hash_insert (table, data))
	return false;

      while (*data != DONE)
	data++;
      data++;
    }

  return true;
}

/*------------------------------------.
| Execute exploding transformations.  |
`------------------------------------*/

bool
explode_byte_byte (RECODE_CONST_STEP step, RECODE_TASK task)
{
  Hash_table *table = step->step_table;
  unsigned value;

  while (value = get_byte (task), value != EOF)
    {
      unsigned short lookup = value;
      unsigned short *result = hash_lookup (table, &lookup);

      if (result)
	{
	  result++;
	  while (*result != DONE && *result != ELSE)
	    {
	      put_byte (*result, task);
	      result++;
	    }
	}
      else
	put_byte (value, task);
    }

  TASK_RETURN (task);
}

bool
explode_ucs2_byte (RECODE_CONST_STEP step, RECODE_TASK task)
{
  Hash_table *table = step->step_table;
  unsigned value;

  while (get_ucs2 (&value, step, task))
    {
      unsigned short lookup = value;
      unsigned short *result = hash_lookup (table, &lookup);

      if (result)
	{
	  result++;
	  while (*result != DONE && *result != ELSE)
	    {
	      put_byte (*result, task);
	      result++;
	    }
	}
      else
	put_byte (value, task);
    }

  TASK_RETURN (task);
}

bool
explode_byte_ucs2 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  Hash_table *table = step->step_table;
  unsigned value;

  if (value = get_byte (task), value != EOF)
    {
      if (task->byte_order_mark)
	put_ucs2 (BYTE_ORDER_MARK, task);

      while (true)
	{
	  unsigned short lookup = value;
	  unsigned short *result = hash_lookup (table, &lookup);

	  if (result)
	    {
	      result++;
	      while (*result != DONE && *result != ELSE)
		put_ucs2 (*result++, task);
	    }
	  else
	    put_ucs2 (value, task);

	  if (value = get_byte (task), value == EOF)
	    break;
	}
    }

  TASK_RETURN (task);
}

bool
explode_ucs2_ucs2 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  Hash_table *table = step->step_table;
  unsigned value;

  if (get_ucs2 (&value, step, task))
    {
      if (task->byte_order_mark)
	put_ucs2 (BYTE_ORDER_MARK, task);

      while (true)
	{
	  unsigned short lookup = value;
	  unsigned short *result = hash_lookup (table, &lookup);

	  if (result)
	    {
	      result++;
	      while (*result != DONE && *result != ELSE)
		put_ucs2 (*result++, task);
	    }
	  else
	    put_ucs2 (value, task);

	  if (!get_ucs2 (&value, step, task))
	    break;
	}
    }

  TASK_RETURN (task);
}

/* Combining.  */

/* A combining state represents the history of reading one or more characters
   while forming a combining sequence.

   Hash coding is used to find an initial state from the first character in a
   sequence.  If that state is not found, the character surely does not start
   any sequence.  For subsequent characters, possible shifted states from the
   current state are kept on that state, as a linked chain.  This combination
   of methods is expected to behave fast enough in practice.  */

struct state
{
  unsigned short character;	/* last character seen to trigger this state */
  unsigned short result;	/* character equivalent to the combination */
  struct state *shift;		/* list of states for one more character */
  struct state *unshift;	/* state for one less character (back link) */
  struct state *next;		/* next state within a linked chain of states */
};

/*---------------------------.
| Initialize for combining.  |
`---------------------------*/

static unsigned
state_hash (const void *void_data, unsigned table_size)
{
  const struct state *data = void_data;

  return data->character % table_size;
}

static bool
state_compare (const void *void_first, const void *void_second)
{
  const struct state *first = void_first;
  const struct state *second = void_second;

  return first->character == second->character;
}

static struct state *
prepare_shifted_state (struct state *state, unsigned short character,
		       RECODE_CONST_STEP step)
{
  if (state)
    {
      struct state *shift = state->shift;

      while (shift)
	if (shift->character == character)
	  return shift;
	else
	  shift = shift->next;

      if (shift = (struct state *) malloc (sizeof (struct state)), !shift)
	return NULL;

      shift->character = character;
      shift->result = NOT_A_CHARACTER;
      shift->shift = NULL;
      shift->unshift = state;
      shift->next = state->shift;
      state->shift = shift;
      return shift;
    }
  else
    {
      Hash_table *table = step->step_table;
      struct state lookup;

      lookup.character = character;
      state = hash_lookup (table, &lookup);
      if (!state)
	{
	  if (state= (struct state *) malloc (sizeof (struct state)), !state)
	    return NULL;

	  state->character = character;
	  state->result = character;
	  state->shift = NULL;
	  state->unshift = NULL;
	  state->next = NULL;

	  if (!hash_insert (table, state))
	    return NULL;
	}
      return state;
    }
}

static struct state *
find_shifted_state (struct state *state, unsigned short character,
		    RECODE_CONST_STEP step)
{
  if (state)
    {
      struct state *shift = state->shift;

      while (shift)
	if (shift->character == character)
	  return shift;
	else
	  shift = shift->next;

      return NULL;
    }
  else
    {
      Hash_table *table = step->step_table;
      struct state lookup;

      lookup.character = character;
      return hash_lookup (table, &lookup);
    }
}

bool
init_combine (RECODE_STEP step,
	      RECODE_CONST_REQUEST request,
	      RECODE_CONST_OPTION_LIST before_options,
	      RECODE_CONST_OPTION_LIST after_options)
{
  const unsigned short *data = step->step_table;
  Hash_table *table;

  if (before_options || after_options)
    return false;

  table = hash_initialize (0, NULL, state_hash, state_compare, NULL);

  if (!table)
    return false;
  step->step_type = RECODE_COMBINE_STEP;
  step->step_table = table;

  if (!data)
    return true;

  while (*data != DONE)
    {
      unsigned short result = *data++;
      struct state *state = NULL;

      while (*data != DONE)
	if (*data == ELSE)
	  {
	    if (state)
	      {
		if (state->result != NOT_A_CHARACTER)
		  abort ();
		state->result = result;
		state = NULL;
	      }
	    data++;
	  }
	else
	  {
	    state = prepare_shifted_state (state, *data++, step);
	    if (!state)
	      return false;
	  }

      if (state)
	{
	  if (state->result != NOT_A_CHARACTER
	      && state->result != state->character)
	    abort ();
	  state->result = result;
	}
      data++;
    }

  return true;
}

/*------------------.
| Handle bactrack.  |
`------------------*/

/* When we cannot shift into a state allowing a terminal resulting character,
   we ought to backtrack until such a terminal character if found, then output
   this resulting character for representing the partial sequence which ends
   with that state.  Then, we merely copy characters seen after that state.

   This approach does not properly scan for combinings which might exist in
   the copied characters, presuming that this case does not occur in practice.
   If we later find that it does, backtracing will have to be revisited.  */

static void
backtrack_byte (struct state *state,  RECODE_TASK task)
{
  if (state->result == NOT_A_CHARACTER)
    {
      backtrack_byte (state->unshift, task);
      put_byte (state->character, task);
    }
  else
    put_byte (state->result, task);
}

static void
backtrack_ucs2 (struct state *state,  RECODE_TASK task)
{
  if (state->result == NOT_A_CHARACTER)
    {
      backtrack_ucs2 (state->unshift, task);
      put_ucs2 (state->character, task);
    }
  else
    put_ucs2 (state->result, task);
}

/*------------------------------------.
| Execute combining transformations.  |
`------------------------------------*/

bool
combine_byte_byte (RECODE_CONST_STEP step, RECODE_TASK task)
{
  struct state *state = NULL;
  unsigned value;

  if (value = get_byte (task), value != EOF)
    {
      while (true)
	{
	  struct state *shift = find_shifted_state (state, value, step);

	  if (shift)
	    {
	      state = shift;
	      if (value = get_byte (task), value == EOF)
		break;
	    }
	  else if (state)
	    {
	      if (state->result == NOT_A_CHARACTER)
		backtrack_byte (state, task);
	      else
		put_byte (state->result, task);
	      state = NULL;
	    }
	  else
	    {
	      put_byte (value, task);
	      if (value = get_byte (task), value == EOF)
		break;
	    }
	}

      if (state)
	if (state->result == NOT_A_CHARACTER)
	  backtrack_byte (state, task);
	else
	  put_byte (state->result, task);
    }

  TASK_RETURN (task);
}

bool
combine_ucs2_byte (RECODE_CONST_STEP step, RECODE_TASK task)
{
  struct state *state = NULL;
  unsigned value;

  if (get_ucs2 (&value, step, task))
    {
      while (true)
	{
	  struct state *shift = find_shifted_state (state, value, step);

	  if (shift)
	    {
	      state = shift;
	      if (!get_ucs2 (&value, step, task))
		break;
	    }
	  else if (state)
	    {
	      if (state->result == NOT_A_CHARACTER)
		backtrack_byte (state, task);
	      else
		put_byte (state->result, task);
	      state = NULL;
	    }
	  else
	    {
	      put_byte (value, task);
	      if (!get_ucs2 (&value, step, task))
		break;
	    }
	}

      if (state)
	if (state->result == NOT_A_CHARACTER)
	  backtrack_byte (state, task);
	else
	  put_byte (state->result, task);
    }

  TASK_RETURN (task);
}

bool
combine_byte_ucs2 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  unsigned value;

  if (value = get_byte (task), value != EOF)
    {
      struct state *state = NULL;

      if (task->byte_order_mark)
	put_ucs2 (BYTE_ORDER_MARK, task);

      while (true)
	{
	  struct state *shift = find_shifted_state (state, value, step);

	  if (shift)
	    {
	      state = shift;
	      if (value = get_byte (task), value == EOF)
		break;
	    }
	  else if (state)
	    {
	      if (state->result == NOT_A_CHARACTER)
		backtrack_ucs2 (state, task);
	      else
		put_ucs2 (state->result, task);
	      state = NULL;
	    }
	  else
	    {
	      put_ucs2 (value, task);
	      if (value = get_byte (task), value == EOF)
		break;
	    }
	}

      if (state)
	if (state->result == NOT_A_CHARACTER)
	  backtrack_ucs2 (state, task);
	else
	  put_ucs2 (state->result, task);
    }

  TASK_RETURN (task);
}

bool
combine_ucs2_ucs2 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  unsigned value;

  if (get_ucs2 (&value, step, task))
    {
      struct state *state = NULL;

      if (task->byte_order_mark)
	put_ucs2 (BYTE_ORDER_MARK, task);

      while (true)
	{
	  struct state *shift = find_shifted_state (state, value, step);

	  if (shift)
	    {
	      state = shift;
	      if (!get_ucs2 (&value, step, task))
		break;
	    }
	  else if (state)
	    {
	      if (state->result == NOT_A_CHARACTER)
		backtrack_ucs2 (state, task);
	      else
		put_ucs2 (state->result, task);
	      state = NULL;
	    }
	  else
	    {
	      put_ucs2 (value, task);
	      if (!get_ucs2 (&value, step, task))
		break;
	    }
	}

      if (state)
	if (state->result == NOT_A_CHARACTER)
	  backtrack_ucs2 (state, task);
	else
	  put_ucs2 (state->result, task);
    }

  TASK_RETURN (task);
}
