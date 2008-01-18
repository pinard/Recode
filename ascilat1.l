/* Conversion of files between different charsets and usages.
   Copyright (C) 1990, 1993, 1994 Free Software Foundation, Inc.
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

/* Step name: ascii_latin1.  */

%%

\<\b\"|\"\b\<			{ output (171); }
\>\b\"|\"\b\>			{ output (187); }
A\b`|`\bA			{ output (192); }
A\b'|'\bA			{ output (193); }
A\b^|^\bA			{ output (194); }
A\b~|~\bA			{ output (195); }
A\b\"|\"\bA			{ output (196); }
C\b,|,\bC			{ output (199); }
E\b`|`\bE			{ output (200); }
E\b'|'\bE			{ output (201); }
E\b^|^\bE			{ output (202); }
E\b\"|\"\bE			{ output (203); }
I\b`|`\bI			{ output (204); }
I\b'|'\bI			{ output (205); }
I\b^|^\bI			{ output (206); }
I\b\"|\"\bI			{ output (207); }
N\b~|~\bN			{ output (209); }
O\b`|`\bO			{ output (210); }
O\b'|'\bO			{ output (211); }
O\b^|^\bO			{ output (212); }
O\b~|~\bO			{ output (213); }
O\b\"|\"\bO			{ output (214); }
O\b\/|\/\bO			{ output (216); }
U\b`|`\bU			{ output (217); }
U\b'|'\bU			{ output (218); }
U\b^|^\bU			{ output (219); }
U\b\"|\"\bU			{ output (220); }
Y\b'|'\bY			{ output (221); }
s\b\"|\"\bs			{ output (223); }
a\b`|`\ba			{ output (224); }
a\b'|'\ba			{ output (225); }
a\b^|^\ba			{ output (226); }
a\b~|~\ba			{ output (227); }
a\b\"|\"\ba			{ output (228); }
c\b,|,\bc			{ output (231); }
e\b`|`\be			{ output (232); }
e\b'|'\be			{ output (233); }
e\b^|^\be			{ output (234); }
e\b\"|\"\be			{ output (235); }
i\b`|`\bi			{ output (236); }
i\b'|'\bi			{ output (237); }
i\b^|^\bi			{ output (238); }
i\b\"|\"\bi			{ output (239); }
n\b~|~\bn			{ output (241); }
o\b`|`\bo			{ output (242); }
o\b'|'\bo			{ output (243); }
o\b^|^\bo			{ output (244); }
o\b~|~\bo			{ output (245); }
o\b\"|\"\bo			{ output (246); }
o\b\/|\/\bo			{ output (248); }
u\b`|`\bu			{ output (249); }
u\b'|'\bu			{ output (250); }
u\b^|^\bu			{ output (251); }
u\b\"|\"\bu			{ output (252); }
y\b'|'\by			{ output (253); }
y\b\"|\"\by			{ output (255); }

%%

void
module_ascii_latin1 (void)
{
  declare_step ("ASCII-BS", "Latin-1", MANY_TO_ONE, NULL, file_ascii_latin1);
}
