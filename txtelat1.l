/* Conversion of files between different charsets and usages.
   Copyright (C) 1990, 1993, 1994 Free Software Foundation, Inc.
   Francois Pinard <pinard@iro.umontreal.ca>, 1989.

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

/* Step name: texte_latin1.  */

%{

void texte_latin1_diaeresis _((void));

%}

s			(^|[^a-zA-Z])
d			[:"]
l			[a-zA-Z]
%%

``			{ output (171); }
''			{ output (187); }

A`			{ output (192); }
A^			{ output (194); }
A{d}/{l}		{ if (yytext[1] == diaeresis_char)
			    output (196);
			  else
			    ECHO;
			}
a`			{ output (224); }
a^			{ output (226); }
a{d}/{l}		{ if (yytext[1] == diaeresis_char)
			    output (228);
			  else
			    ECHO;
			}

C\,/[aAoOuU]		{ output (199); }
c\,/[aAoOuU]		{ output (231); }

E`			{ output (200); }
E'''			{ output (201); output (187); }
E''			{ output ('E'); output (187); }
E'			{ output (201); }
E^			{ output (202); }
E{d}/{l}		{ if (yytext[1] == diaeresis_char)
			    output (203);
			  else
			    ECHO;
			}
e`			{ output (232); }
e'''			{ output (233); output (187); }
e''			{ output ('e'); output (187); }
e'			{ output (233); }
e^			{ output (234); }
e{d}/{l}		{ if (yytext[1] == diaeresis_char)
			    output (235);
			  else
			    ECHO;
			}

I`			{ output (204); }
I^			{ output (206); }
I{d}/{l}		{ if (yytext[1] == diaeresis_char)
			    output (207);
			  else
			    ECHO;
			}
i`			{ output (236); }
i^			{ output (238); }
i{d}/{l}		{ if (yytext[1] == diaeresis_char)
			    output (239);
			  else
			    ECHO;
			}

O`			{ output (210); }
O^			{ output (212); }
O{d}/{l}		{ if (yytext[1] == diaeresis_char)
			    output (214);
			  else
			    ECHO;
			}
o`			{ output (242); }
o^			{ output (244); }
o{d}/{l}		{ if (yytext[1] == diaeresis_char)
			    output (246);
			  else
			    ECHO;
			}

U`			{ output (217); }
U^			{ output (219); }
U{d}/{l}		{ if (yytext[1] == diaeresis_char)
			    output (220);
			  else
			    ECHO;
			}
u`			{ output (249); }
u^			{ output (251); }
u{d}/{l}		{ if (yytext[1] == diaeresis_char)
			    output (252);
			  else
			    ECHO;
			}

{s}[Bb]esaigue{d}	{ texte_latin1_diaeresis (); }
{s}[Cc]igue{d}		{ texte_latin1_diaeresis (); }
{s}[Aa]igue{d}		{ texte_latin1_diaeresis (); }
{s}[Aa]mbigue{d}	{ texte_latin1_diaeresis (); }
{s}[Cc]ontigue{d}	{ texte_latin1_diaeresis (); }
{s}[Ee]xigue{d}		{ texte_latin1_diaeresis (); }
{s}[Ss]ubaigue{d}	{ texte_latin1_diaeresis (); }
{s}[Ss]uraigue{d}	{ texte_latin1_diaeresis (); }
{s}[Aa]i{d}		{ texte_latin1_diaeresis (); }
{s}[Cc]ongai{d}		{ texte_latin1_diaeresis (); }
{s}[Gg]oi{d}		{ texte_latin1_diaeresis (); }
{s}[Hh]ai{d}kai{d}	{ if (yytext[4] == diaeresis_char)
			    texte_latin1_diaeresis ();
			  else
			    ECHO;
			}
{s}[Ii]noui{d}		{ texte_latin1_diaeresis (); }
[JjTtLl]'[Aa][Ii]{d}	{ ECHO; }
{s}[Ss]ai{d}		{ texte_latin1_diaeresis (); }
{s}[Ss]amurai{d}	{ texte_latin1_diaeresis (); }
{s}[Tt]hai{d}		{ texte_latin1_diaeresis (); }
{s}[Tt]okai{d}		{ texte_latin1_diaeresis (); }
{s}[Cc]anoe{d}		{ texte_latin1_diaeresis (); }
{s}Esau{d}		{ texte_latin1_diaeresis (); }
%%

void
texte_latin1_diaeresis (void)
{
  int counter;

  for (counter = 0; counter < yyleng; counter++)
    if (yytext[counter+1] == diaeresis_char)
      {
	switch (yytext[counter])
	  {

	    /* The next "case 'A'" line once triggered a `NULL in input'
	       diagnostic in flex.  This astonishing bug has been hard to
	       isolate, so I'll leave this comment around for a while.  */

	  case 'A': output (196); break;
	  case 'E': output (203); break;
	  case 'I': output (207); break;
	  case 'O': output (214); break;
	  case 'U': output (220); break;
	  case 'a': output (228); break;
	  case 'e': output (235); break;
	  case 'i': output (239); break;
	  case 'o': output (246); break;
	  case 'u': output (252); break;
	  case 'y': output (255); break;
	  default:  output (yytext[counter]);
	  }
	counter++;
      }
    else
      output (yytext[counter]);
}

void
module_texte_latin1 (void)
{
  declare_step ("Texte", "Latin-1", MANY_TO_ONE, NULL, file_texte_latin1);

  declare_alias ("txte", "Texte");
}
