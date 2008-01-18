/* Conversion of files between different charsets and surfaces.
   Copyright © 1999 Free Software Foundation, Inc.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1988.

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
#include "libiconv.h"

/*-----------------------------------------.
| Use `libiconv' to handle a double step.  |
`-----------------------------------------*/

#define LIBICONV_BUFFER_SIZE 2048

static bool
wrapped_transform (iconv_t conversion,
		   RECODE_SUBTASK subtask)
{
  int input_char = get_byte (subtask);
  char input_buffer[LIBICONV_BUFFER_SIZE];
  char output_buffer[LIBICONV_BUFFER_SIZE];
  size_t input_left;
  size_t output_left;
  size_t converted;
  const char *input;
  char *output;
  char *cursor;

  cursor = input_buffer;
  while (cursor > input_buffer || input_char != EOF)
    {
      /* Fill the input buffer as much as possible.  */
      while (input_char != EOF && cursor < input_buffer + LIBICONV_BUFFER_SIZE)
	{
	  *cursor++ = input_char;
	  input_char = get_byte (subtask);
	}
      if (cursor > input_buffer)
	{
	  /* Convert accumulated input into the output buffer.  */
	  input_left = cursor - input_buffer;
	  input = input_buffer;
	  output_left = LIBICONV_BUFFER_SIZE;
	  output = output_buffer;
	  converted
	    = iconv (conversion, &input, &input_left, &output, &output_left);
	  /* If there was no progress, we have a fatal conversion error.
	     Then, we suspect invalid input, but we do not really know.  */
	  if (input == input_buffer && output == output_buffer)
	    {
	      SET_SUBTASK_ERROR (RECODE_INVALID_INPUT, subtask);
	      SUBTASK_RETURN (subtask);
	    }
	  /* Send the converted result, to free the output buffer.  */
	  for (cursor = output_buffer; cursor < output; cursor++)
	    put_byte (*cursor, subtask);
	  /* Skip one byte if an invalid input sequence.  This is crude,
	     `libiconv' knows much better, and should do it.  FIXME!  */
	  if (converted == (size_t) -1 && errno == EILSEQ)
	    {
	      RETURN_IF_NOGO (RECODE_INVALID_INPUT, subtask);
	      input++;
	      input_left--;
	      /* Reset converstion state.  */
	      output_left = LIBICONV_BUFFER_SIZE;
	      output = output_buffer;
	      iconv (conversion, NULL, NULL, &output, &output_left);
	      for (cursor = output_buffer; cursor < output; cursor++)
		put_byte (*cursor, subtask);
	    }
	  /* Shift back the unconverted part of the input buffer.  */
	  cursor = input_buffer;
	  while (input_left != 0)
	    {
	      *cursor++ = *input++;
	      input_left--;
	    }
	}
    }

  SUBTASK_RETURN (subtask);
}

bool
transform_with_libiconv (RECODE_SUBTASK subtask)
{
  RECODE_CONST_STEP step = subtask->step;
  iconv_t conversion = iconv_open (step->after->name, step->before->name);
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
	  RECODE_SYMBOL symbol
	    = find_symbol (outer, *cursor, SYMBOL_FIND_AS_CHARSET);

	  if (symbol)
	    {
	      charset_name = symbol->charset->name;
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
	  RECODE_SYMBOL symbol
	    = find_symbol (outer, *cursor, SYMBOL_FIND_AS_CHARSET);

	  /* If there is a charset contradiction, call declare_alias
	     nevertheless, as the error processing will occur there.  */
	  if (!symbol || symbol->charset->name != charset_name)
	    if (!declare_alias (outer, *cursor, charset_name))
	      return false;
	}
    }

  return true;
}
