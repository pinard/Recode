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

/* Step name: html_latin1.  */

%%

&amp;		{ if (diacritics_only) ECHO; else output (38); }
&lt;		{ if (diacritics_only) ECHO; else output (60); }
&gt;		{ if (diacritics_only) ECHO; else output (62); }

&Agrave;	{ output (192); }
&Aacute;	{ output (193); }
&Acircu;	{ output (194); }
&Atilde;	{ output (195); }
&Adiaer;	{ output (196); }
&Aring;		{ output (197); }
&AE;		{ output (198); }
&Ccedil;	{ output (199); }
&Egrave;	{ output (200); }
&Eacute;	{ output (201); }
&Ecircu;	{ output (202); }
&Ediaer;	{ output (203); }
&Igrave;	{ output (204); }
&Iacute;	{ output (205); }
&Icircu;	{ output (206); }
&Idiaer;	{ output (207); }
&ETH;		{ output (208); }
&Ntilde;	{ output (209); }
&Ograve;	{ output (210); }
&Oacute;	{ output (211); }
&Ocircu;	{ output (212); }
&Otilde;	{ output (213); }
&Odiaer;	{ output (214); }
&MULT;		{ output (215); }
&Ostroke;	{ output (216); }
&Ugrave;	{ output (217); }
&Uacute;	{ output (218); }
&Ucircu;	{ output (219); }
&Udiaer;	{ output (220); }
&Yacute;	{ output (221); }
&THORN;		{ output (222); }
&ssharp;	{ output (223); }
&agrave;	{ output (224); }
&aacute;	{ output (225); }
&acircu;	{ output (226); }
&atilde;	{ output (227); }
&adiaer;	{ output (228); }
&aring;		{ output (229); }
&ae;		{ output (230); }
&ccedil;	{ output (231); }
&egrave;	{ output (232); }
&eacute;	{ output (233); }
&ecircu;	{ output (234); }
&ediaer;	{ output (235); }
&igrave;	{ output (236); }
&iacute;	{ output (237); }
&icircu;	{ output (238); }
&idiaer;	{ output (239); }
&eth;		{ output (240); }
&ntilde;	{ output (241); }
&ograve;	{ output (242); }
&oacute;	{ output (243); }
&ocircu;	{ output (244); }
&otilde;	{ output (245); }
&odiaer;	{ output (246); }
&DIVIS;		{ output (247); }
&ostroke;	{ output (248); }
&ugrave;	{ output (249); }
&uacute;	{ output (250); }
&ucircu;	{ output (251); }
&udiaer;	{ output (252); }
&yacute;	{ output (253); }
&thorn;		{ output (254); }
&ydiaer;	{ output (255); }

%%

void
module_html_latin1 (void)
{
  declare_step ("HTML", "latin1", MANY_TO_ONE, NULL, file_html_latin1);

  declare_alias ("WWW", "HTML");
  declare_alias ("w3", "HTML");
}
