/* Conversion of files between different charsets and surfaces.
   Copyright © 1990, 93, 97, 98, 99 Free Software Foundation, Inc.
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

/* Step name: latex_latin1.  */

Letter			[a-zA-Z]
Braces			[ \t]*(\{\})?

%%

\\"#"				{ PUT_NON_DIACRITIC_BYTE ('#', task); }
\\"$"				{ PUT_NON_DIACRITIC_BYTE ('$', task); }
\\"%"				{ PUT_NON_DIACRITIC_BYTE ('%', task); }
\\"&"				{ PUT_NON_DIACRITIC_BYTE ('&', task); }
\\"_"				{ PUT_NON_DIACRITIC_BYTE ('_', task); }
\\"{"				{ PUT_NON_DIACRITIC_BYTE ('{', task); }
\\"}"				{ PUT_NON_DIACRITIC_BYTE ('}', task); }

\\backslash{Braces}		{ PUT_NON_DIACRITIC_BYTE ('\\', task); }

"~"				{ PUT_NON_DIACRITIC_BYTE (160, task); }
"!`"				{ PUT_NON_DIACRITIC_BYTE (161, task); }
\\pound{Braces}			{ PUT_NON_DIACRITIC_BYTE (163, task); }
\\S{Braces}			{ PUT_NON_DIACRITIC_BYTE (167, task); }
\\\"				{ PUT_NON_DIACRITIC_BYTE (168, task); }
\\copyright{Braces}		{ PUT_NON_DIACRITIC_BYTE (169, task); }
"``"				{ PUT_NON_DIACRITIC_BYTE (171, task); }
\\neg{Braces}			{ PUT_NON_DIACRITIC_BYTE (172, task); }
\\"-"				{ PUT_NON_DIACRITIC_BYTE (173, task); }
\\mbox"{$^\\circ$}"		{ PUT_NON_DIACRITIC_BYTE (176, task); }
\\mbox"{$\\pm$}"		{ PUT_NON_DIACRITIC_BYTE (177, task); }
\\mbox"{$^2$}"			{ PUT_NON_DIACRITIC_BYTE (178, task); }
\\mbox"{$^3$}"			{ PUT_NON_DIACRITIC_BYTE (179, task); }
\\"'"				{ PUT_NON_DIACRITIC_BYTE (180, task); }
\\mbox"{$\\mu$}"		{ PUT_NON_DIACRITIC_BYTE (181, task); }
\\cdotp				{ PUT_NON_DIACRITIC_BYTE (183, task); }
\\","				{ PUT_NON_DIACRITIC_BYTE (184, task); }
\\mbox"{$^1$}"			{ PUT_NON_DIACRITIC_BYTE (185, task); }
"''"				{ PUT_NON_DIACRITIC_BYTE (187, task); }
\\frac"1/4"{Braces}		{ PUT_NON_DIACRITIC_BYTE (188, task); }
\\frac"1/2"{Braces}		{ PUT_NON_DIACRITIC_BYTE (189, task); }
\\frac"3/4"{Braces}		{ PUT_NON_DIACRITIC_BYTE (190, task); }
"?`"				{ PUT_NON_DIACRITIC_BYTE (191, task); }

\\"`"A|\\"`{A}"			{ put_byte (192, task); }
\\"'"A|\\"'{A}"			{ put_byte (193, task); }
\\"^"A|\\"^{A}"			{ put_byte (194, task); }
\\"~"A|\\"~{A}"			{ put_byte (195, task); }
\\\"A|\\\""{A}"			{ put_byte (196, task); }
\\AA{Braces}			{ put_byte (197, task); }
\\AE{Braces}			{ put_byte (198, task); }
\\c[ \t]+C|\\c"{C}"		{ put_byte (199, task); }
\\"`"E|\\"`{E}"			{ put_byte (200, task); }
\\"'"E|\\"'{E}"			{ put_byte (201, task); }
\\"^"E|\\"^{E}"			{ put_byte (202, task); }
\\\"E|\\\""{E}"			{ put_byte (203, task); }
\\"`"I|\\"`{I}"			{ put_byte (204, task); }
\\"'"I|\\"'{I}"			{ put_byte (205, task); }
\\"^"I|\\"^{I}"			{ put_byte (206, task); }
\\\"I|\\\""{I}"			{ put_byte (207, task); }
\\"~"N|\\"~{N}"			{ put_byte (209, task); }
\\"`"O|\\"`{O}"			{ put_byte (210, task); }
\\"'"O|\\"'{O}"			{ put_byte (211, task); }
\\"^"O|\\"^{O}"			{ put_byte (212, task); }
\\"~"O|\\"~{O}"			{ put_byte (213, task); }
\\\"O|\\\""{O}"			{ put_byte (214, task); }
\\O{Braces}			{ put_byte (216, task); }
\\"`"U|\\"`{U}"			{ put_byte (217, task); }
\\"'"U|\\"'{U}"			{ put_byte (218, task); }
\\"^"U|\\"^{U}"			{ put_byte (219, task); }
\\\"U|\\\""{U}"			{ put_byte (220, task); }
\\"'"Y|\\"'{Y}"			{ put_byte (221, task); }
\\ss{Braces}			{ put_byte (223, task); }
\\"`"a|\\"`{a}"			{ put_byte (224, task); }
\\"'"a|\\"'{a}"			{ put_byte (225, task); }
\\"^"a|\\"^{a}"			{ put_byte (226, task); }
\\"~"a|\\"~{a}"			{ put_byte (227, task); }
\\\"a|\\\""{a}"			{ put_byte (228, task); }
\\aa{Braces}			{ put_byte (229, task); }
\\ae{Braces}			{ put_byte (230, task); }
\\c[ \t]+c|\\c"{c}"		{ put_byte (231, task); }
\\"`"e|\\"`{e}"			{ put_byte (232, task); }
\\"'"e|\\"'{e}"			{ put_byte (233, task); }
\\"^"e|\\"^{e}"			{ put_byte (234, task); }
\\\"e|\\\""{e}"			{ put_byte (235, task); }
\\"`"\\i{Braces}|\\"`{\\i}"	{ put_byte (236, task); }
\\"'"\\i{Braces}|\\"'{\\i}"	{ put_byte (237, task); }
\\"^"\\i{Braces}|\\"^{\\i}"	{ put_byte (238, task); }
\\\"\\i{Braces}|\\\""{\\i}"	{ put_byte (239, task); }
\\"~"n|\\"~{n}"			{ put_byte (241, task); }
\\"`"o|\\"`{o}"			{ put_byte (242, task); }
\\"'"o|\\"'{o}"			{ put_byte (243, task); }
\\"^"o|\\"^{o}"			{ put_byte (244, task); }
\\"~"o|\\"~{o}"			{ put_byte (245, task); }
\\\"o|\\\""{o}"			{ put_byte (246, task); }
\\o{Braces}			{ put_byte (248, task); }
\\"`"u|\\"`{u}"			{ put_byte (249, task); }
\\"'"u|\\"'{u}"			{ put_byte (250, task); }
\\"^"u|\\"^{u}"			{ put_byte (251, task); }
\\\"u|\\\""{u}"			{ put_byte (252, task); }
\\"'"y|\\"'{y}"			{ put_byte (253, task); }
\\\"y|\\\""{y}"			{ put_byte (255, task); }

\\[`'^"]\\i{Letter}*{Braces}	{ ECHO; }
\\{Letter}+{Braces}		{ ECHO; }

%%

bool
module_latex_latin1 (RECODE_OUTER outer)
{
  return
declare_single (outer, "LaTeX", "Latin-1",
outer->quality_variable_to_byte,
NULL, transform_latex_latin1)
    && declare_alias (outer, "TeX", "LaTeX")
    && declare_alias (outer, "ltex", "LaTeX");
}
