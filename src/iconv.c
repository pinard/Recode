/* Conversion of files between different charsets and surfaces.
   Copyright © 1999, 2000, 2001, 2008 Free Software Foundation, Inc.
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
   License along with the Recode Library; see the file `COPYING.LIB'.
   If not, write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA.  */

#include "common.h"
#include <iconv.h>
#include "iconvdecl.h"

/*--------------------------------------.
| Use `iconv' to handle a double step.  |
`--------------------------------------*/

#define BUFFER_SIZE 2048

static bool
wrapped_transform (iconv_t conversion, RECODE_SUBTASK subtask)
{
  char output_buffer[BUFFER_SIZE];
  char input_buffer[BUFFER_SIZE];
  int input_char = get_byte (subtask);
  char *cursor = input_buffer;
  bool drain_first = false;

  while (true)
    {
      /* The output buffer is fully avaible at this point.  */

      char *input = input_buffer;
      char *output = output_buffer;
      size_t input_left = 0;
      size_t output_left = BUFFER_SIZE;
      int saved_errno = 0;
      size_t converted;

      if (drain_first)
        {
          /* Drain all accumulated partial state and emit output
             to return to the initial shift state.  */
          converted = iconv (conversion, NULL, NULL, &output, &output_left);
          if (converted == (size_t) -1)
            saved_errno = errno;
        }

      if (saved_errno == 0)
        {
          /* Continue filling the input buffer.  */
          while (input_char != EOF && cursor < input_buffer + BUFFER_SIZE)
            {
              *cursor++ = input_char;
              input_char = get_byte (subtask);
            }

          if (cursor == input_buffer)
            {
              if (output == output_buffer)
                {
                  /* All work has been done, just make sure we drained.  */
                  if (drain_first)
                    break;
                  drain_first = true;
                  continue;
                }
            }
          else
            {
              /* Convert accumulated input and add it to the output buffer.  */
              input = input_buffer;
              input_left = cursor - input_buffer;
              converted = iconv (conversion,
                                 &input, &input_left,
                                 &output, &output_left);
              if (converted == (size_t) -1)
                saved_errno = errno;
            }
        }

      /* Send the converted result, so freeing the output buffer.  */
      for (cursor = output_buffer; cursor < output; cursor++)
        put_byte (*cursor, subtask);

      /* Act according to the outcome of the iconv call.  */

      drain_first = false;
      if (saved_errno != 0 && saved_errno != E2BIG)
        if (saved_errno == EILSEQ)
          {
            /* Invalid input.  Skip one byte.  */
            RETURN_IF_NOGO (RECODE_INVALID_INPUT, subtask);
            assert (input_left > 0);
            input++;
            input_left--;
            /* Why is draining required?  */
            drain_first = true;
          }
        else if (saved_errno == EINVAL)
          {
            if (input + input_left < input_buffer + BUFFER_SIZE
                && input_char == EOF)
              /* Incomplete multibyte sequence at end of input.  */
              RETURN_IF_NOGO (RECODE_INVALID_INPUT, subtask);
          }
        else
          {
            recode_perror (subtask->task->request->outer, "iconv ()");
            RETURN_IF_NOGO (RECODE_SYSTEM_ERROR, subtask);
          }

      /* Move back any unprocessed part of the input buffer.  */
      for (cursor = input_buffer; input_left != 0; input_left--)
        *cursor++ = *input++;
    }

  SUBTASK_RETURN (subtask);
}

bool
transform_with_iconv (RECODE_SUBTASK subtask)
{
  RECODE_CONST_STEP step = subtask->step;
  iconv_t conversion = iconv_open (step->after->iconv_name,
                                   step->before->iconv_name);
  bool status;

  if (conversion == (iconv_t) -1)
    {
      SET_SUBTASK_ERROR (RECODE_SYSTEM_ERROR, subtask);
      SUBTASK_RETURN (subtask);
    }

  status = wrapped_transform (conversion, subtask);

  iconv_close (conversion);
  return status;
}

/*------------------------------------------------------.
| Declare all character sets which `iconv' may handle.  |
`------------------------------------------------------*/

bool
module_iconv (RECODE_OUTER outer)
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

      if (!declare_iconv (outer, charset_name, *aliases))
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
delmodule_iconv (RECODE_OUTER outer)
{
}
