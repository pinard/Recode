/* Conversion of files between different charsets and surfaces.
   Copyright © 1999, 2000 Free Software Foundation, Inc.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1999,
   and Bruno Haible <haible@clisp.cons.org>, 2000.

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
#include "iconv.h"
#include "libiconv.h"

/*-----------------------------------------.
| Use `libiconv' to handle a double step.  |
`-----------------------------------------*/

#define LIBICONV_BUFFER_SIZE 2048

static bool
wrapped_transform (iconv_t conversion, iconv_t conversion_to_utf8,
		   RECODE_SUBTASK subtask)
{
  int input_char = get_byte (subtask);
  char input_buffer[LIBICONV_BUFFER_SIZE];
  char output_buffer[LIBICONV_BUFFER_SIZE];
  size_t input_left;
  size_t output_left;
  const char *input;
  char *output;
  char *cursor;
  int transliterate;
  size_t converted;
  int saved_errno;

  /* libiconv has transliteration facilities. Since they are not reversible,
     try first without them.  */
  transliterate = 0;
  iconvctl (conversion, ICONV_SET_TRANSLITERATE, &transliterate);
  iconvctl (conversion_to_utf8, ICONV_SET_TRANSLITERATE, &transliterate);

  cursor = input_buffer;
  while (cursor > input_buffer || input_char != EOF)
    {
      /* Fill the input buffer as much as possible.  */
      while (input_char != EOF && cursor < input_buffer + LIBICONV_BUFFER_SIZE)
	{
	  *cursor++ = input_char;
	  input_char = get_byte (subtask);
	}

      /* We are now sure to have at least some input.  */
      if (!(cursor > input_buffer))
	{
	  recode_error (subtask->task->request->outer,
			"libiconv.c internal error 56");
	  SET_SUBTASK_ERROR (RECODE_INTERNAL_ERROR, subtask);
	  SUBTASK_RETURN (subtask);
	}
      assert (cursor > input_buffer);

      /* Convert accumulated input into the output buffer.  */
      input_left = cursor - input_buffer;
      input = input_buffer;
      output_left = LIBICONV_BUFFER_SIZE;
      output = output_buffer;
      converted
	= iconv (conversion, &input, &input_left, &output, &output_left);

      /* Send the converted result, to free the output buffer.  */
      saved_errno = errno;
      for (cursor = output_buffer; cursor < output; cursor++)
	put_byte (*cursor, subtask);
      errno = saved_errno;

      /* Analyze the iconv return value.  */
      if (converted == (size_t)(-1) && errno != E2BIG)
	{
	  if (errno == EILSEQ)
	    {
	      /* The first thing to try is libiconv's built-in
		 transliteration.  */
	      if (!transliterate)
		{
		  /* Fail if the user requested reversible conversions.  */
		  RETURN_IF_NOGO (RECODE_NOT_CANONICAL, subtask);

		  /* Switch transliteration on for the rest of the subtask.  */
		  transliterate = 1;
		  iconvctl (conversion,
			    ICONV_SET_TRANSLITERATE, &transliterate);
		  iconvctl (conversion_to_utf8,
			    ICONV_SET_TRANSLITERATE, &transliterate);

		  /* Continue, shifting the remaining input buffer.  */
		  goto shift_input;
		}
	      else
		{
		  /* An invalid multibyte sequence was encountered in the
		     input, or a conversion error occurred.  Distinguish the
		     two cases by use of conversion_to_utf8.  In the first
		     case, skip one byte.  In the second case, skip the entire
		     character.
		     FIXME: This heuristic does not work well with stateful
		     encodings like ISO-2022-JP.  */
		  char tmp_buf[6];
		  size_t tmp_input_left;
		  size_t tmp_output_left;
		  const char *tmp_input;
		  char *tmp_output;

		  RETURN_IF_NOGO (RECODE_INVALID_INPUT, subtask);

		  assert (input_left > 0);

		  tmp_input_left = input_left;
		  tmp_input = input;
		  tmp_output_left = sizeof (tmp_buf);
		  tmp_output = tmp_buf;
		  iconv (conversion_to_utf8,
			 &tmp_input, &tmp_input_left,
			 &tmp_output, &tmp_output_left);
		  /* Reset conversion_to_utf8 to the initial state.  */
		  iconv (conversion_to_utf8, NULL, NULL, NULL, NULL);
		  if (tmp_input > input)
		    {
		      /* Conversion error.  Skip the entire character.  */
		      input = tmp_input;
		      input_left = tmp_input_left;
		    }
		  else
		    {
		      /* Invalid input.  Skip one byte.  */
		      input++;
		      input_left--;
		    }

		  /* Reset conversion state.  (Why?)  */
		  output_left = LIBICONV_BUFFER_SIZE;
		  output = output_buffer;
		  converted
		    = iconv (conversion, NULL, NULL, &output, &output_left);
		  /* We don't expect E2BIG here: the buffer is large enough. */
		  assert (converted != (size_t)(-1));
		  for (cursor = output_buffer; cursor < output; cursor++)
		    put_byte (*cursor, subtask);
		}
	    }
	  else if (errno == EINVAL)
	    {
	      /* Incomplete multibyte sequence.  */
	      if (input + input_left < input_buffer + LIBICONV_BUFFER_SIZE
		  && input_char == EOF)
		{
		  /* Incomplete multibyte sequence at end of input.  */
		  RETURN_IF_NOGO (RECODE_INVALID_INPUT, subtask);
		  break;
		}
	      /* Otherwise, we shift the remaining input and see whether the
		 error persists in the next round.  */
	    }
	  else
	    {
	      recode_perror (subtask->task->request->outer, "libiconv ()");
	      SET_SUBTASK_ERROR (RECODE_SYSTEM_ERROR, subtask);
	      SUBTASK_RETURN (subtask);
	    }
	}

      /* If there was no progress, we have a bug in either libiconv or the
	 above logic.  */
      if (!(input > input_buffer))
	{
	  recode_error (subtask->task->request->outer,
			"libiconv.c internal error 154");
	  SET_SUBTASK_ERROR (RECODE_INTERNAL_ERROR, subtask);
	  SUBTASK_RETURN (subtask);
	}
      assert (input > input_buffer);

    shift_input:
      /* Shift back the unconverted part of the input buffer.
	 memcpy() doesn't do here, because the regions might overlap.
	 memmove() isn't worth it, because we rarely have to move more
	 than 12 bytes.  */
      if (input > input_buffer && input_left > 0)
	{
	  cursor = input_buffer;
	  do
	    *cursor++ = *input++;
	  while (--input_left > 0);
	}
    }

  /* Drain all accumulated partial state and emit output to return to the
     initial shift state.  */
  output_left = LIBICONV_BUFFER_SIZE;
  output = output_buffer;
  converted = iconv (conversion, NULL, NULL, &output, &output_left);
  /* We don't expect E2BIG here: the buffer is large enough.  */
  assert (converted != (size_t)(-1));
  for (cursor = output_buffer; cursor < output; cursor++)
    put_byte (*cursor, subtask);

  SUBTASK_RETURN (subtask);
}

bool
transform_with_libiconv (RECODE_SUBTASK subtask)
{
  RECODE_CONST_STEP step = subtask->step;
  iconv_t conversion = iconv_open (step->after->name, step->before->name);
  iconv_t conversion_to_utf8 = iconv_open ("UTF-8", step->before->name);
  bool status;

  if (conversion == (iconv_t) -1 || conversion_to_utf8 == (iconv_t) -1)
    {
      SET_SUBTASK_ERROR (RECODE_SYSTEM_ERROR, subtask);
      SUBTASK_RETURN (subtask);
    }

  status = wrapped_transform (conversion, conversion_to_utf8, subtask);

  iconv_close (conversion);
  iconv_close (conversion_to_utf8);
  return status;
}

/*---------------------------------------------------------.
| Declare all character sets which `libiconv' may handle.  |
`---------------------------------------------------------*/

bool
module_libiconv (RECODE_OUTER outer)
{
  const char **cursor;

  for (cursor = iconv_name_list; *cursor; cursor++)
    {
      const char **aliases = cursor;
      const char *charset_name = *cursor;

      /* Scan aliases for some charset which would already be known.  If any,
	 use its official name as a charset.  Else, use the first alias.  */

      while (*cursor)
	{
	  RECODE_ALIAS alias
	    = find_alias (outer, *cursor, ALIAS_FIND_AS_CHARSET);

	  if (alias)
	    {
	      charset_name = alias->symbol->name;
	      break;
	    }
	  cursor++;
	}

      if (!declare_libiconv (outer, charset_name))
	return false;

      /* Declare all aliases, given they bring something we do not already
	 know.  Even then, we still declare too many useless aliases, as the
	 desambiguating tables are not recomputed as we go.  FIXME!  */

      for (cursor = aliases; *cursor; cursor++)
	{
	  RECODE_ALIAS alias
	    = find_alias (outer, *cursor, ALIAS_FIND_AS_CHARSET);

	  /* If there is a charset contradiction, call declare_alias
	     nevertheless, as the error processing will occur there.  */
	  if (!alias || alias->symbol->name != charset_name)
	    if (!declare_alias (outer, *cursor, charset_name))
	      return false;
	}
    }

  return true;
}

void
delmodule_libiconv (RECODE_OUTER outer)
{
}
