/* Conversion of files between different charsets and surfaces.
   Copyright © 1990, 93, 94, 97, 98, 99 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
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

/* Step name: ascii_latin1.  */

%%

\<\b\"|\"\b\<			{ put_byte (171, task); }
\>\b\"|\"\b\>			{ put_byte (187, task); }
A\b`|`\bA			{ put_byte (192, task); }
A\b'|'\bA			{ put_byte (193, task); }
A\b^|^\bA			{ put_byte (194, task); }
A\b~|~\bA			{ put_byte (195, task); }
A\b\"|\"\bA			{ put_byte (196, task); }
C\b,|,\bC			{ put_byte (199, task); }
E\b`|`\bE			{ put_byte (200, task); }
E\b'|'\bE			{ put_byte (201, task); }
E\b^|^\bE			{ put_byte (202, task); }
E\b\"|\"\bE			{ put_byte (203, task); }
I\b`|`\bI			{ put_byte (204, task); }
I\b'|'\bI			{ put_byte (205, task); }
I\b^|^\bI			{ put_byte (206, task); }
I\b\"|\"\bI			{ put_byte (207, task); }
N\b~|~\bN			{ put_byte (209, task); }
O\b`|`\bO			{ put_byte (210, task); }
O\b'|'\bO			{ put_byte (211, task); }
O\b^|^\bO			{ put_byte (212, task); }
O\b~|~\bO			{ put_byte (213, task); }
O\b\"|\"\bO			{ put_byte (214, task); }
O\b\/|\/\bO			{ put_byte (216, task); }
U\b`|`\bU			{ put_byte (217, task); }
U\b'|'\bU			{ put_byte (218, task); }
U\b^|^\bU			{ put_byte (219, task); }
U\b\"|\"\bU			{ put_byte (220, task); }
Y\b'|'\bY			{ put_byte (221, task); }
s\b\"|\"\bs			{ put_byte (223, task); }
a\b`|`\ba			{ put_byte (224, task); }
a\b'|'\ba			{ put_byte (225, task); }
a\b^|^\ba			{ put_byte (226, task); }
a\b~|~\ba			{ put_byte (227, task); }
a\b\"|\"\ba			{ put_byte (228, task); }
c\b,|,\bc			{ put_byte (231, task); }
e\b`|`\be			{ put_byte (232, task); }
e\b'|'\be			{ put_byte (233, task); }
e\b^|^\be			{ put_byte (234, task); }
e\b\"|\"\be			{ put_byte (235, task); }
i\b`|`\bi			{ put_byte (236, task); }
i\b'|'\bi			{ put_byte (237, task); }
i\b^|^\bi			{ put_byte (238, task); }
i\b\"|\"\bi			{ put_byte (239, task); }
n\b~|~\bn			{ put_byte (241, task); }
o\b`|`\bo			{ put_byte (242, task); }
o\b'|'\bo			{ put_byte (243, task); }
o\b^|^\bo			{ put_byte (244, task); }
o\b~|~\bo			{ put_byte (245, task); }
o\b\"|\"\bo			{ put_byte (246, task); }
o\b\/|\/\bo			{ put_byte (248, task); }
u\b`|`\bu			{ put_byte (249, task); }
u\b'|'\bu			{ put_byte (250, task); }
u\b^|^\bu			{ put_byte (251, task); }
u\b\"|\"\bu			{ put_byte (252, task); }
y\b'|'\by			{ put_byte (253, task); }
y\b\"|\"\by			{ put_byte (255, task); }

%%

bool
module_ascii_latin1 (RECODE_OUTER outer)
{
  if (!declare_single (outer, "ASCII-BS", "Latin-1",
		       outer->quality_variable_to_byte,
		       NULL, transform_ascii_latin1))
    return false;

  return true;
}
