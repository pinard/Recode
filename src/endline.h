/* Conversion of files between different charsets and surfaces.
   Copyright © 1990,93,94, 1997-1999, 2000-2001 Free Software Foundation, Inc.

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

/* Conversions for a given character unit.  */

static bool
TRANSFORM_NL_CR (RECODE_SUBTASK subtask)
{
  bool strict = subtask->step->fallback_routine != reversibility;
  unsigned int character;
  BOM_DECLARATIONS;

  if (GET (&character, subtask))
    {
      BOM_HANDLING;

      do
	switch (character)
	  {
	  case '\n':
	    PUT (CR, subtask);
	    break;

	  case CR:
	    if (!strict)
	      {
		PUT ('\n', subtask);
		break;
	      }
	    RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, subtask);
	    /* Fall through.  */

	  default:
	    PUT (character, subtask);
	  }
      while (GET (&character, subtask));
    }

  SUBTASK_RETURN (subtask);
}

static bool
TRANSFORM_CR_NL (RECODE_SUBTASK subtask)
{
  bool strict = subtask->step->fallback_routine != reversibility;
  unsigned int character;
  BOM_DECLARATIONS;

  if (GET (&character, subtask))
    {
      BOM_HANDLING;

      do
	switch (character)
	  {
	  case CR:
	    PUT ('\n', subtask);
	    break;

	  case '\n':
	    if (!strict)
	      {
		PUT (CR, subtask);
		break;
	      }
	    RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, subtask);
	    /* Fall through.  */

	  default:
	    PUT (character, subtask);
	  }
      while (GET (&character, subtask));
    }

  SUBTASK_RETURN (subtask);
}

static bool
TRANSFORM_NL_CRLF (RECODE_SUBTASK subtask)
{
  unsigned int character;
  BOM_DECLARATIONS;

  if (GET (&character, subtask))
    {
      BOM_HANDLING;

      for (;;)
	{
	  if (character == '\n')
	    {
	      PUT (CR, subtask);
	      PUT (LF, subtask);
	      if (!GET (&character, subtask))
		break;
	    }
	  else if (character == CR)
	    {
	      if (GET (&character, subtask))
		{
		  if (character == LF)
		    RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, subtask);
		  PUT (CR, subtask);
		}
	      else
		{
		  PUT (CR, subtask);
		  break;
		}
	    }
	  else
	    {
	      PUT (character, subtask);
	      if (!GET (&character, subtask))
		break;
	    }
	}
    }

  SUBTASK_RETURN (subtask);
}

static bool
TRANSFORM_CRLF_NL (RECODE_SUBTASK subtask)
{
  unsigned int character;
  BOM_DECLARATIONS;

  if (GET (&character, subtask))
    {
      BOM_HANDLING;

      for (;;)
	{
	  if (character == CR)
	    {
	      if (GET (&character, subtask))
		{
		  if (character == LF)
		    {
		      PUT ('\n', subtask);
		      if (!GET (&character, subtask))
			break;
		    }
		  else
		    PUT (CR, subtask);
		}
	      else
		{
		  PUT (CR, subtask);
		  break;
		}
	    }
	  else
	    {
	      if (character == LF)
		RETURN_IF_NOGO (RECODE_AMBIGUOUS_OUTPUT, subtask);
	      PUT (character, subtask);
	      if (!GET (&character, subtask))
		break;
	    }
	}
    }

  SUBTASK_RETURN (subtask);
}

#undef TRANSFORM_NL_CR
#undef TRANSFORM_CR_NL
#undef TRANSFORM_NL_CRLF
#undef TRANSFORM_CRLF_NL
#undef BOM_DECLARATIONS
#undef BOM_HANDLING
#undef GET
#undef PUT
