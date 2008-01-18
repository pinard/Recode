/* Conversion of files between different charsets and surfaces.
   Copyright © 1990, 92, 93, 94, 96, 97, 98, 99 Free Software Foundation, Inc.
   François Pinard <pinard@iro.umontreal.ca>, 1990.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
   Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "common.h"

/*================================\
| Mixed charset file processors.  |
\================================*/

struct mixed
{
  /* Original task descriptor.  */
  RECODE_TASK original_task;

  /* File names saved from original task.  */
  const char *input_name;
  const char *output_name;

  /* Task where plumbing takes place.  */
  struct recode_task task;

  /* In memory buffer.  */
  struct recode_read_write_text buffer;
};

static bool
open_mixed (struct mixed *mixed, RECODE_TASK task)
{
  mixed->original_task = task;
  mixed->input_name = task->input.name;
  mixed->output_name = task->output.name;

  /* Open both files ourselves.  */

  if (!*(task->input.name))
    task->input.file = stdin;
  else if (task->input.file = fopen (mixed->input_name, "r"),
	   !task->input.file)
    {
      recode_perror (NULL, "fopen (%s)", task->input.name);
      return false;
    }
  task->input.name = NULL;

  if (!*(task->output.name))
    task->output.file = stdout;
  else if (task->output.file = fopen (mixed->output_name, "w"),
	   !task->output.file)
    {
      recode_perror (NULL, "fopen (%s)", task->output.name);
      if (*(task->input.name))
	fclose (task->input.file);
      return false;
    }
  task->output.name = NULL;

  /* Prepare for dynamic plumbing copy.  */

  mixed->task = *task;
  mixed->task.strategy = RECODE_SEQUENCE_IN_MEMORY;
  mixed->task.byte_order_mark = false;
  memset (&mixed->buffer, 0, sizeof (struct recode_read_write_text));

  return true;
}

static void
close_mixed (struct mixed *mixed)
{
  /* Clean up.  */

  if (*(mixed->input_name))
    fclose (mixed->original_task->input.file);
  if (*(mixed->output_name))
    fclose (mixed->original_task->output.file);
}

static inline void
plumb_input_to_buffer (struct mixed *mixed)
{
  /* Une only after plumb_input_to_output, or after initial state.  */
  mixed->buffer.cursor = mixed->buffer.buffer;
  mixed->task.output = mixed->buffer;
}

static inline void
plumb_buffer_to_output (struct mixed *mixed)
{
  /* Use only after plumb_input_to_buffer.  */
  mixed->buffer = mixed->task.output;
  mixed->task.output = mixed->original_task->output;
  mixed->task.input.file = NULL;
  mixed->task.input.buffer = mixed->buffer.buffer;
  mixed->task.input.cursor = mixed->buffer.buffer;
  mixed->task.input.limit = mixed->buffer.cursor;
}

static inline void
plumb_input_to_output (struct mixed *mixed)
{
  /* Use only after plumb_buffer_to_output.  */
  mixed->task.input = mixed->original_task->input;
}

/*-----------------------------------------------------------------------.
| Transform only strings or comments in an C source, expected in ASCII.  |
`-----------------------------------------------------------------------*/

bool
transform_c_source (RECODE_TASK task)
{
  struct mixed mixed;
  bool status = true;
  int character;

  if (!open_mixed (&mixed, task))
    return false;

  character = get_byte (&mixed.task);
  while (character != EOF)
    switch (character)
      {
      case '\'':
	/* Skip character constant, while copying it untranslated.  */

	put_byte ('\'', &mixed.task);
	character = get_byte (&mixed.task);

	if (character == EOF)
	  {
	    status = false;
	    break;
	  }

	if (character == '\\')
	  {
	    put_byte ('\\', &mixed.task);
	    character = get_byte (&mixed.task);
	    if (character == EOF)
	      {
		status = false;
		break;
	      }
	    put_byte (character, &mixed.task);
	    character = get_byte (&mixed.task);
	  }

	if (character == '\'')
	  {
	    put_byte ('\'', &mixed.task);
	    character = get_byte (&mixed.task);
	  }
	else
	  status = false;
	break;

      case '"':
	/* Copy the string, translated.  */

	put_byte ('"', &mixed.task);
	character = get_byte (&mixed.task);

	/* Read in string.  */

	plumb_input_to_buffer (&mixed);

	while (true)
	  {
	    if (character == EOF)
	      {
		plumb_buffer_to_output (&mixed);
		recode_perform_task (&mixed.task);
		status = false;
		break;
	      }
	    if (character == '"')
	      break;

	    if (character == '\\')
	      {
		put_byte ('\\', &mixed.task);
		character = get_byte (&mixed.task);
		if (character == EOF)
		  {
		    plumb_buffer_to_output (&mixed);
		    recode_perform_task (&mixed.task);
		    status = false;
		    break;
		  }
	      }
	    put_byte (character, &mixed.task);
	    character = get_byte (&mixed.task);
	  }
	if (character == EOF)
	  break;

	/* Translate string and dump it.  */

	plumb_buffer_to_output (&mixed);
	if (!recode_perform_task (&mixed.task))
	  status = false;
	put_byte ('"', &mixed.task);

	plumb_input_to_output (&mixed);
	character = get_byte (&mixed.task);
	break;

      case '/':
	put_byte ('/', &mixed.task);
	character = get_byte (&mixed.task);
	if (character == EOF)
	  break;
	if (character == '*')
	  {
	    /* Copy the comment, translated.  */

	    put_byte ('*', &mixed.task);
	    character = get_byte (&mixed.task);

	    /* Read in comment.  */

	    plumb_input_to_buffer (&mixed);
	    while (true)
	      {
		if (character == EOF)
		  {
		    plumb_buffer_to_output (&mixed);
		    recode_perform_task (&mixed.task);
		    status = false;
		    break;
		  }
		if (character == '*')
		  {
		    character = get_byte (&mixed.task);
		    if (character == EOF)
		      {
			plumb_buffer_to_output (&mixed);
			recode_perform_task (&mixed.task);
			status = false;
			put_byte ('*', &mixed.task);
			break;
		      }
		    if (character == '/')
		      break;
		    put_byte ('*', &mixed.task);
		  }
		else
		  {
		    put_byte (character, &mixed.task);
		    character = get_byte (&mixed.task);
		  }
	      }

	    if (character == EOF)
	      break;

	    /* Translate comment and dump it.  */

	    plumb_buffer_to_output (&mixed);
	    if (!recode_perform_task (&mixed.task))
	      status = false;
	    put_byte ('*', &mixed.task);
	    put_byte ('/', &mixed.task);

	    plumb_input_to_output (&mixed);
	    character = get_byte (&mixed.task);
	  }
	break;

      default:
	put_byte (character, &mixed.task);
	character = get_byte (&mixed.task);
	break;
      }

  close_mixed (&mixed);
  return status;
}

/*------------------------------------------------------------------------.
| Transform only strings or comments in an PO source, expected in ASCII.  |
`------------------------------------------------------------------------*/

/* There is a limitation to -Spo: if `recode' converts some `msgstr' in a way
   that might produce quotes (or backslashes), these should then be requoted.
   Doing this would then also require to fully unquote the original `msgstr'
   string.  But it seems that such a need does not occur in most cases I can
   imagine as practical, as the ASCII subset is generally invariant under
   recoding.  My guess is that we should wait for someone to report the bug
   with a real case, to believe it is worth adding the complexity.  */

bool
transform_po_source (RECODE_TASK task)
{
  struct mixed mixed;
  bool status = true;
  bool recode = false;
  bool msgstr = false;
  int character;

  if (!open_mixed (&mixed, task))
    return false;

  character = get_byte (&mixed.task);
  while (character != EOF)
    switch (character)
      {
      case '#':
	/* Copy a comment, recoding only those written by translators.  */

	put_byte ('#', &mixed.task);
	character = get_byte (&mixed.task);
	if (character == EOF)
	  break;
	recode = character == ' ' || character == '\t';

	if (recode)
	  plumb_input_to_buffer (&mixed);

	while (character != '\n' && character != EOF)
	  {
	    put_byte (character, &mixed.task);
	    character = get_byte (&mixed.task);
	  }

	if (recode)
	  {
	    plumb_buffer_to_output (&mixed);
	    if (!recode_perform_task (&mixed.task))
	      status = false;
	    plumb_input_to_output (&mixed);
	  }

	if (character == EOF)
	  break;
	put_byte ('\n', &mixed.task);
	character = get_byte (&mixed.task);
	break;

      case 'm':
	/* Attempt to recognise `msgstr'.  */

	msgstr = false;

	put_byte ('m', &mixed.task);
	character = get_byte (&mixed.task);
	if (character != 's')
	  break;
	put_byte ('s', &mixed.task);
	character = get_byte (&mixed.task);
	if (character != 'g')
	  break;
	put_byte ('g', &mixed.task);
	character = get_byte (&mixed.task);
	if (character != 's')
	  break;
	put_byte ('s', &mixed.task);
	character = get_byte (&mixed.task);
	if (character != 't')
	  break;
	put_byte ('t', &mixed.task);
	character = get_byte (&mixed.task);
	if (character != 'r')
	  break;
	put_byte ('r', &mixed.task);
	character = get_byte (&mixed.task);

	msgstr = true;
	break;

      case '"':
	/* Copy the string, translating only the `msgstr' ones.  */

	put_byte ('"', &mixed.task);
	character = get_byte (&mixed.task);
	recode = msgstr;

	if (recode)
	  plumb_input_to_buffer (&mixed);

	while (true)
	  {
	    if (character == EOF)
	      {
		if (recode)
		  {
		    plumb_buffer_to_output (&mixed);
		    recode_perform_task (&mixed.task);
		    status = false;
		  }
		break;
	      }
	    if (character == '"')
	      break;

	    if (character == '\\')
	      {
		put_byte ('\\', &mixed.task);
		character = get_byte (&mixed.task);
		if (character == EOF)
		  {
		    if (recode)
		      {
			plumb_buffer_to_output (&mixed);
			recode_perform_task (&mixed.task);
			status = false;
		      }
		    break;
		  }
	      }
	    put_byte (character, &mixed.task);
	    character = get_byte (&mixed.task);
	  }

	if (character == EOF)
	  break;

	if (recode)
	  {
	    plumb_buffer_to_output (&mixed);
	    if (!recode_perform_task (&mixed.task))
	      status = false;
	    plumb_input_to_output (&mixed);
	  }

	put_byte ('"', &mixed.task);
	character = get_byte (&mixed.task);
	break;

      default:
	put_byte (character, &mixed.task);
	character = get_byte (&mixed.task);
	break;
      }

  close_mixed (&mixed);
  return status;
}
