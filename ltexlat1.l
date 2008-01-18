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

/* Step name: latex_latin1.  */

Letter			[a-zA-Z]
Braces			[ \t]*(\{\})?

%%

\\"#"			{ if (diacritics_only) ECHO; else output ('#'); }
\\"$"			{ if (diacritics_only) ECHO; else output ('$'); }
\\"%"			{ if (diacritics_only) ECHO; else output ('%'); }
\\"&"			{ if (diacritics_only) ECHO; else output ('&'); }
\\"_"			{ if (diacritics_only) ECHO; else output ('_'); }
\\"{"			{ if (diacritics_only) ECHO; else output ('{'); }
\\"}"			{ if (diacritics_only) ECHO; else output ('}'); }

\\backslash{Braces}	{ if (diacritics_only) ECHO; else output ('\\'); }
"~"			{ if (diacritics_only) ECHO; else output (160); }
"!'"			{ if (diacritics_only) ECHO; else output (161); }

\\S{Braces}		{ if (diacritics_only) ECHO; else output (167); }
\\\"			{ if (diacritics_only) ECHO; else output (168); }

"``"			{ if (diacritics_only) ECHO; else output (171); }
\\neg{Braces}		{ if (diacritics_only) ECHO; else output (172); }
\\"-"			{ if (diacritics_only) ECHO; else output (173); }

\\mbox"{$^\\circ$}"	{ if (diacritics_only) ECHO; else output (176); }
\\mbox"{$\\pm$}"	{ if (diacritics_only) ECHO; else output (177); }
\\mbox"{$^2$}"		{ if (diacritics_only) ECHO; else output (178); }
\\mbox"{$^3$}"		{ if (diacritics_only) ECHO; else output (179); }
\\"'"			{ if (diacritics_only) ECHO; else output (180); }
\\mbox"{$\\mu$}"	{ if (diacritics_only) ECHO; else output (181); }

\\cdotp			{ if (diacritics_only) ECHO; else output (183); }
\\","			{ if (diacritics_only) ECHO; else output (184); }
\\mbox"{$^1$}"		{ if (diacritics_only) ECHO; else output (185); }

"''"			{ if (diacritics_only) ECHO; else output (187); }
\\frac"1/4"{Braces}	{ if (diacritics_only) ECHO; else output (188); }
\\frac"1/2"{Braces}	{ if (diacritics_only) ECHO; else output (189); }
\\frac"3/4"{Braces}	{ if (diacritics_only) ECHO; else output (190); }
"?'"			{ if (diacritics_only) ECHO; else output (191); }

\\"`"A|\\"`{A}"			{ output (192); }
\\"'"A|\\"'{A}"			{ output (193); }
\\"^"A|\\"^{A}"			{ output (194); }
\\"~"A|\\"~{A}"			{ output (195); }
\\\"A|\\\""{A}"			{ output (196); }
\\AA{Braces}			{ output (197); }
\\AE{Braces}			{ output (198); }
\\c[ \t]+C|\\c"{C}"		{ output (199); }
\\"`"E|\\"`{E}"			{ output (200); }
\\"'"E|\\"'{E}"			{ output (201); }
\\"^"E|\\"^{E}"			{ output (202); }
\\\"E|\\\""{E}"			{ output (203); }
\\"`"I|\\"`{I}"			{ output (204); }
\\"'"I|\\"'{I}"			{ output (205); }
\\"^"I|\\"^{I}"			{ output (206); }
\\\"I|\\\""{I}"			{ output (207); }
	
\\"~"N|\\"~{N}"			{ output (209); }
\\"`"O|\\"`{O}"			{ output (210); }
\\"'"O|\\"'{O}"			{ output (211); }
\\"^"O|\\"^{O}"			{ output (212); }
\\"~"O|\\"~{O}"			{ output (213); }
\\\"O|\\\""{O}"			{ output (214); }
	
\\O{Braces}			{ output (216); }
\\"`"U|\\"`{U}"			{ output (217); }
\\"'"U|\\"'{U}"			{ output (218); }
\\"^"U|\\"^{U}"			{ output (219); }
\\\"U|\\\""{U}"			{ output (220); }
\\"'"Y|\\"'{Y}"			{ output (221); }
	
\\ss{Braces}			{ output (223); }
\\"`"a|\\"`{a}"			{ output (224); }
\\"'"a|\\"'{a}"			{ output (225); }
\\"^"a|\\"^{a}"			{ output (226); }
\\"~"a|\\"~{a}"			{ output (227); }
\\\"a|\\\""{a}"			{ output (228); }
\\aa{Braces}			{ output (229); }
\\ae{Braces}			{ output (230); }
\\c[ \t]+c|\\c"{c}"		{ output (231); }
\\"`"e|\\"`{e}"			{ output (232); }
\\"'"e|\\"'{e}"			{ output (233); }
\\"^"e|\\"^{e}"			{ output (234); }
\\\"e|\\\""{e}"			{ output (235); }
\\"`"\\i{Braces}|\\"`{\\i}"	{ output (236); }
\\"'"\\i{Braces}|\\"'{\\i}"	{ output (237); }
\\"^"\\i{Braces}|\\"^{\\i}"	{ output (238); }
\\\"\\i{Braces}|\\\""{\\i}"	{ output (239); }

\\"~"n|\\"~{n}"			{ output (241); }
\\"`"o|\\"`{o}"			{ output (242); }
\\"'"o|\\"'{o}"			{ output (243); }
\\"^"o|\\"^{o}"			{ output (244); }
\\"~"o|\\"~{o}"			{ output (245); }
\\\"o|\\\""{o}"			{ output (246); }
	
\\o{Braces}			{ output (248); }
\\"`"u|\\"`{u}"			{ output (249); }
\\"'"u|\\"'{u}"			{ output (250); }
\\"^"u|\\"^{u}"			{ output (251); }
\\\"u|\\\""{u}"			{ output (252); }
\\"'"y|\\"'{y}"			{ output (253); }
	
\\\"y|\\\""{y}"			{ output (255); }

\\[`'^"]\\i{Letter}*{Braces}	{ ECHO; }
\\{Letter}+{Braces}		{ ECHO; }

%%

void
module_latex_latin1 (void)
{
  declare_step ("LaTeX", "Latin-1", MANY_TO_ONE, NULL, file_latex_latin1);

  declare_alias ("TeX", "LaTeX");
  declare_alias ("ltex", "LaTeX");
}
