/* Conversion of files between different charsets and usages.
   Copyright (C) 1993, 1994 Free Software Foundation, Inc.
   Francois Pinard <pinard@iro.umontreal.ca>, 1993.

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

/* RFC 1345 style description for NeXTSTEP (non official).  */

/* In the following table, these codes are not represented:

	Dec Oct     Character

	169 251     single quote
	172 254     gouillemot single left
	173 255     gouillemot single right
	184 270     single quote base
	185 271     double qoute base

   Keld also write:
   
   * f2 (florin) was introduced after RFC1345, and it was done according
   to an official answer from ISO/IEC JTC1/SC2/WG2 to the Danish ballot
   on DIS.2 10646.
   
   * '" (double acute accent) is the same as hungarian umlaut.  */

static DOUBLE_TABLE table =
  {
    "NUSHSXEXETEQAKBLBSHTLFVTFFCRSOSIDLD1D2D3D4NKSYEBCNEMSBECFSGSRSUS",
    "SP! \" NbDO% & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? ",
    "AtA B C D E F G H I J K L M N O P Q R S T U V W X Y Z <(//)>'>_ ",
    "'!a b c d e f g h i j k l m n o p q r s t u v w x y z (!!!!)'?DT",
    "NSA!A'A>A?A:AAC,E!E'E>E:I!I'I>I:D-N?O!O'O>O?O:U!U'U>U:Y'THMy*X-:",
    "Co!ICtPd/fYef2SECu  \"6<<    fiflRg-N/-/=.MBBPISb    \"9>>.3%0NO?I",
    "1S'!'''>'?'m'('.':2S'0',3S'\"';'<-M+-141234a!a'a>a?a:aac,e!e'e>e:",
    "i!AEi'-ai>i:d-n?L/O/OE-oo!o'o>o?o:aeu!u'u>i.u:y'l/o/oessthy:    ",
  };

void
module_next (void)
{
  declare_double_step (&table, "NeXT", 2);
}
