/* Conversion of files between different charsets and surfaces.
   Copyright © 1990, 93, 94, 97, 98, 99 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1989.

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

/* Step name: texte_latin1.  */

%{

void texte_latin1_diaeresis PARAMS ((void));

%}

s			(^|[^a-zA-Z])
d			[:"]
l			[a-zA-Z]
%%

``			{ put_byte (171, task); }
''			{ put_byte (187, task); }

A`			{ put_byte (192, task); }
A^			{ put_byte (194, task); }
A{d}/{l}		{ if (yytext[1] == request->diaeresis_char)
			    put_byte (196, task);
			  else
			    ECHO;
			}
a`			{ put_byte (224, task); }
a^			{ put_byte (226, task); }
a{d}/{l}		{ if (yytext[1] == request->diaeresis_char)
			    put_byte (228, task);
			  else
			    ECHO;
			}

C\,/[aAoOuU]		{ put_byte (199, task); }
c\,/[aAoOuU]		{ put_byte (231, task); }

E`			{ put_byte (200, task); }
E'''			{ put_byte (201, task); put_byte (187, task); }
E''			{ put_byte ('E', task); put_byte (187, task); }
E'			{ put_byte (201, task); }
E^			{ put_byte (202, task); }
E{d}/{l}		{ if (yytext[1] == request->diaeresis_char)
			    put_byte (203, task);
			  else
			    ECHO;
			}
e`			{ put_byte (232, task); }
e'''			{ put_byte (233, task); put_byte (187, task); }
e''			{ put_byte ('e', task); put_byte (187, task); }
e'			{ put_byte (233, task); }
e^			{ put_byte (234, task); }
e{d}/{l}		{ if (yytext[1] == request->diaeresis_char)
			    put_byte (235, task);
			  else
			    ECHO;
			}

I`			{ put_byte (204, task); }
I^			{ put_byte (206, task); }
I{d}/{l}		{ if (yytext[1] == request->diaeresis_char)
			    put_byte (207, task);
			  else
			    ECHO;
			}
i`			{ put_byte (236, task); }
i^			{ put_byte (238, task); }
i{d}/{l}		{ if (yytext[1] == request->diaeresis_char)
			    put_byte (239, task);
			  else
			    ECHO;
			}

O`			{ put_byte (210, task); }
O^			{ put_byte (212, task); }
O{d}/{l}		{ if (yytext[1] == request->diaeresis_char)
			    put_byte (214, task);
			  else
			    ECHO;
			}
o`			{ put_byte (242, task); }
o^			{ put_byte (244, task); }
o{d}/{l}		{ if (yytext[1] == request->diaeresis_char)
			    put_byte (246, task);
			  else
			    ECHO;
			}

U`			{ put_byte (217, task); }
U^			{ put_byte (219, task); }
U{d}/{l}		{ if (yytext[1] == request->diaeresis_char)
			    put_byte (220, task);
			  else
			    ECHO;
			}
u`			{ put_byte (249, task); }
u^			{ put_byte (251, task); }
u{d}/{l}		{ if (yytext[1] == request->diaeresis_char)
			    put_byte (252, task);
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
{s}[Hh]ai{d}kai{d}	{ if (yytext[4] == request->diaeresis_char)
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
  unsigned counter;

  for (counter = 0; counter < yyleng; counter++)
    if (yytext[counter+1] == request->diaeresis_char)
      {
	switch (yytext[counter])
	  {
	    /* The next "case 'A'" line once triggered a `NULL in input'
	       diagnostic in flex.  This astonishing bug has been hard to
	       isolate, so I'll leave this comment around for a while.  */

	  case 'A': put_byte (196, task); break;
	  case 'E': put_byte (203, task); break;
	  case 'I': put_byte (207, task); break;
	  case 'O': put_byte (214, task); break;
	  case 'U': put_byte (220, task); break;
	  case 'a': put_byte (228, task); break;
	  case 'e': put_byte (235, task); break;
	  case 'i': put_byte (239, task); break;
	  case 'o': put_byte (246, task); break;
	  case 'u': put_byte (252, task); break;
	  case 'y': put_byte (255, task); break;
	  default:  put_byte (yytext[counter], task);
	  }
	counter++;
      }
    else
      put_byte (yytext[counter], task);
}

bool
module_texte_latin1 (RECODE_OUTER outer)
{
  return
    declare_single (outer, "Texte", "Latin-1",
		    outer->quality_variable_to_byte, NULL,
		    transform_texte_latin1)
    && declare_alias (outer, "txte", "Texte");
}
