/* Conversion of files between different charsets and usages.
   Copyright (C) 1990, 1993 Free Software Foundation, Inc.
   Francois Pinard <pinard@iro.umontreal.ca>, 1988.

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

struct translation
  {
    int code;			/* code being translated */
    const char *string;		/* translation string */
  };

static struct translation diacritic_translations [] =
  {
    {192, "&Agrave;"},
    {193, "&Aacute;"},
    {194, "&Acircu;"},
    {195, "&Atilde;"},
    {196, "&Adiaer;"},
    {197, "&Aring;"},
    {198, "&AE;"},
    {199, "&Ccedil;"},
    {200, "&Egrave;"},
    {201, "&Eacute;"},
    {202, "&Ecircu;"},
    {203, "&Ediaer;"},
    {204, "&Igrave;"},
    {205, "&Iacute;"},
    {206, "&Icircu;"},
    {207, "&Idiaer;"},
    {208, "&ETH;"},
    {209, "&Ntilde;"},
    {210, "&Ograve;"},
    {211, "&Oacute;"},
    {212, "&Ocircu;"},
    {213, "&Otilde;"},
    {214, "&Odiaer;"},
    {215, "&MULT;"},
    {216, "&Ostroke;"},
    {217, "&Ugrave;"},
    {218, "&Uacute;"},
    {219, "&Ucircu;"},
    {220, "&Udiaer;"},
    {221, "&Yacute;"},
    {222, "&THORN;"},
    {223, "&ssharp;"},
    {224, "&agrave;"},
    {225, "&aacute;"},
    {226, "&acircu;"},
    {227, "&atilde;"},
    {228, "&adiaer;"},
    {229, "&aring;"},
    {230, "&ae;"},
    {231, "&ccedil;"},
    {232, "&egrave;"},
    {233, "&eacute;"},
    {234, "&ecircu;"},
    {235, "&ediaer;"},
    {236, "&igrave;"},
    {237, "&iacute;"},
    {238, "&icircu;"},
    {239, "&idiaer;"},
    {240, "&eth;"},
    {241, "&ntilde;"},
    {242, "&ograve;"},
    {243, "&oacute;"},
    {244, "&ocircu;"},
    {245, "&otilde;"},
    {246, "&odiaer;"},
    {247, "&DIVIS;"},
    {248, "&ostroke;"},
    {249, "&ugrave;"},
    {250, "&uacute;"},
    {251, "&ucircu;"},
    {252, "&udiaer;"},
    {253, "&yacute;"},
    {254, "&thorn;"},
    {255, "&ydiaer;"},
    {0, NULL}
  };

static struct translation const other_translations [] =
  {
    {38, "&amp;"},
    {60, "&lt;"},
    {62, "&gt;"},
    {0, NULL}
  };

static void
init_latin1_html (STEP *step)
{
  char *pool;
  const char **table;
  int counter;
  struct translation const *cursor;

  table = (const char **) xmalloc (256 * sizeof (char *) + 256);
  pool = (char *) (table + 256);

  for (counter = 0; counter < 128; counter++)
    {
      pool[2 * counter] = counter;
      pool[2 * counter + 1] = '\0';
      table[counter] = pool + 2 * counter;
    }
  for (counter = 128; counter < 256; counter++)
    table[counter] = NULL;
  for (cursor = diacritic_translations; cursor->code; cursor++)
    table[cursor->code] = cursor->string;

  if (!diacritics_only)
    for (cursor = other_translations; cursor->code; cursor++)
      table[cursor->code] = cursor->string;

  step->one_to_many = table;
}

void
module_latin1_html (void)
{
  declare_step ("latin1", "HTML", ONE_TO_MANY, init_latin1_html,
		file_one_to_many);

  declare_alias ("WWW", "HTML");
  declare_alias ("w3", "HTML");
}
