/* Conversion of files between different charsets and surfaces.
   Copyright © 1993, 1994, 1997 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1993.

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

#include "common.h"

/* RFC 1345 style description for NeXTSTEP (non official).

   NUSHSXEXETEQAKBLBSHTLFVTFFCRSOSIDLD1D2D3D4NKSYEBCNEMSBECFSGSRSUS
   SP! " NbDO% & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?
   AtA B C D E F G H I J K L M N O P Q R S T U V W X Y Z <(//)>'>_
   '!a b c d e f g h i j k l m n o p q r s t u v w x y z (!!!!)'?DT
   NSA!A'A>A?A:AAC,E!E'E>E:I!I'I>I:D-N?O!O'O>O?O:U!U'U>U:Y'THMy*X-:
   Co!ICtPd/fYef2SECu  "6<<    fiflRg-N/-/=.MBBPISb    "9>>.3%0NO?I
   1S'!'''>'?'m'('.':2S'0',3S'"';'<-M+-141234a!a'a>a?a:aac,e!e'e>e:
   i!AEi'-ai>i:d-n?L/O/OE-oo!o'o>o?o:aeu!u'u>i.u:y'l/o/oessthy:     */

/* In the following table, these codes are not represented:

	Dec Oct     Character

	169 251     single quote
	172 254     gouillemot single left
	173 255     gouillemot single right
	184 270     single quote base
	185 271     double quote base

   Keld also writes:

   * f2 (florin) was introduced after RFC1345, and it was done according
   to an official answer from ISO/IEC JTC1/SC2/WG2 to the Danish ballot
   on DIS.2 10646.  For now, it is translated to the Latin-1 currency sign.

   * '" (double acute accent) is the same as hungarian umlaut.  */

static const unsigned short pool[256] =
  {
    /*    0 */ 0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
    /*    8 */ 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    /*   16 */ 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
    /*   24 */ 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    /*   32 */ 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    /*   40 */ 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    /*   48 */ 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    /*   56 */ 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    /*   64 */ 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    /*   72 */ 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    /*   80 */ 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    /*   88 */ 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    /*   96 */ 0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    /*  104 */ 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    /*  112 */ 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    /*  120 */ 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
    /*  128 */ 0x00A0, 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C7,
    /*  136 */ 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
    /*  144 */ 0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D9,
    /*  152 */ 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00B5, 0x00D7, 0x00F7,
    /*  160 */ 0x00A9, 0x00A1, 0x00A2, 0x00A3, 0x2215, 0x00A5, 0x0192, 0x00A7,
    /*  168 */ 0x00A4, 0xFFFF, 0x201C, 0x00AB, 0xFFFF, 0xFFFF, 0xFB01, 0xFB02,
    /*  176 */ 0x00AE, 0x2013, 0x2020, 0x2021, 0x00B7, 0x00A6, 0x00B6, 0x2022,
    /*  184 */ 0xFFFF, 0xFFFF, 0x201D, 0x00BB, 0x2026, 0x2030, 0x00AC, 0x00BF,
    /*  192 */ 0x00B9, 0x0060, 0x00B4, 0x005E, 0x007E, 0x00AF, 0x02D8, 0x02D9,
    /*  200 */ 0x00A8, 0x00B2, 0x02DA, 0x00B8, 0x00B3, 0x02DD, 0x02DB, 0x02C7,
    /*  208 */ 0x2014, 0x00B1, 0x00BC, 0x00BD, 0x00BE, 0x00E0, 0x00E1, 0x00E2,
    /*  216 */ 0x00E3, 0x00E4, 0x00E5, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB,
    /*  224 */ 0x00EC, 0x00C6, 0x00ED, 0x00AA, 0x00EE, 0x00EF, 0x00F0, 0x00F1,
    /*  232 */ 0x0141, 0x00D8, 0x0152, 0x00BA, 0x00F2, 0x00F3, 0x00F4, 0x00F5,
    /*  240 */ 0x00F6, 0x00E6, 0x00F9, 0x00FA, 0x00FB, 0x0131, 0x00FC, 0x00FD,
    /*  248 */ 0x0142, 0x00F8, 0x0153, 0x00DF, 0x00FE, 0x00FF, 0xFFFF, 0xFFFF
  };

static UCS2_DATA_TABLE table =
  {
    pool,
    {
	 0,    8,   16,   24,   32,   40,   48,   56,   64,   72,   80,   88,
	96,  104,  112,  120,  128,  136,  144,  152,  160,  168,  176,  184,
       192,  200,  208,  216,  224,  232,  240,  248
    }
  };

bool
module_next (struct recode_outer *outer)
{
  return declare_ucs2_data (outer, &table, "NeXT");
}
