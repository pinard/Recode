/* Conversion of files between different charsets and surfaces.
   Copyright © 1999, 2000 Free Software Foundation, Inc.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1993.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the `recode' Library; see the file `COPYING.LIB'.
   If not, write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA.  */

#include "common.h"

/* This file contains various temporary tables.  These would ideally all go
   away once Keld will be given references, solid enough, to really integrate
   these tables in RFC1345 (.bis, .ter :-).  */

/* Czech tables.  */

/* Lukas Petrlik <luki@pafos.zcu.cz>, 1996-04-02, and Martin Mares
   <mj@ucw.cz>, 1999-01-05, both sent Kamenicky and Cork tables.  The
   following two macros are used to document the contradicting spots.  */
#define LUKAS 1			/* define to 1 for Lukas definitions */
#define MARTIN 0		/* define to 1 for Martin definitions */

/* These tables use standard latin alphabet with Czech accented letters.
   They use a subset of ISO-8859-2, plus a few strange characters.  */

/* KEYBCS2, Kamenicky.  "Source: the Reality :-)", as says Lukas.  According
   to Martin, several sources list CP859 as being equivalent to the Kamenicky
   Brothers code, but neither of them seems to be authoritative enough.  */

static const unsigned short data_kamenicky[] =
  {
#if MARTIN
    /* Non-Czech characters copied from IBM charset.  */
      1, 0x263A, DONE,		/* white smiling face */
      2, 0x263B, DONE,		/* black smiling face */
      3, 0x2665, DONE,		/* black heart suit */
      4, 0x2666, DONE,		/* black diamond suit */
      5, 0x2663, DONE,		/* black club suit */
      6, 0x2660, DONE,		/* black spade suit */
      7, 0x2022, DONE,		/* bullet */
      8, 0x25D8, DONE,		/* inverse bullet */
      9, 0x25CB, DONE,		/* white circle */
     10, 0x25D9, DONE,		/* inverse white circle */
     11, 0x2642, DONE,		/* male sign */
     12, 0x2640, DONE,		/* female sign */
     13, 0x266A, DONE,		/* eighth note */
     14, 0x266B, DONE,		/* beamed eighth notes */
     15, 0x263C, DONE,		/* white sun with rays */
     16, 0x25B6, DONE,		/* black right-pointing triangle */
     17, 0x25C0, DONE,		/* black left-pointing triangle */
     18, 0x2195, DONE,		/* up down arrow */
     19, 0x203C, DONE,		/* double exclamation mark */
     20, 0x00B6, DONE,		/* pilcrow sign */
     21, 0x00A7, DONE,		/* section sign */
     22, 0x25AC, DONE,		/* black rectangle */
     23, 0x21A8, DONE,		/* up down arrow with base */
     24, 0x2191, DONE,		/* upwards arrow */
     25, 0x2193, DONE,		/* downwards arrow */
     26, 0x2192, DONE,		/* rightwards arrow */
     27, 0x2190, DONE,		/* leftwards arrow */
     28, 0x221F, DONE,		/* right angle */
     29, 0x2194, DONE,		/* left right arrow */
     30, 0x25B2, DONE,		/* black up-pointing triangle */
     31, 0x25BC, DONE,		/* black down-pointing triangle */
    127, 0x2302, DONE,		/* house */
#endif /* MARTIN */
    128, 0x010C, DONE,
    129, 0x00FC, DONE,
    130, 0x00E9, DONE,
    131, 0x010F, DONE,
    132, 0x00E4, DONE,
    133, 0x010E, DONE,
    134, 0x0164, DONE,
    135, 0x010D, DONE,
    136, 0x011B, DONE,
    137, 0x011A, DONE,
    138, 0x0139, DONE,
    139, 0x00CD, DONE,
    140, 0x013E, DONE,
    141, 0x013A, DONE,
    142, 0x00C4, DONE,
    143, 0x00C1, DONE,
    144, 0x00C9, DONE,
    145, 0x017E, DONE,
    146, 0x017D, DONE,
    147, 0x00F4, DONE,
    148, 0x00F6, DONE,
    149, 0x00D3, DONE,
    150, 0x016F, DONE,
    151, 0x00DA, DONE,
    152, 0x00FD, DONE,
    153, 0x00D6, DONE,
    154, 0x00DC, DONE,
    155, 0x0160, DONE,
    156, 0x013D, DONE,
    157, 0x00DD, DONE,
    158, 0x0158, DONE,
    159, 0x0165, DONE,		/* latin small letter t with caron */
    160, 0x00E1, DONE,
    161, 0x00ED, DONE,
    162, 0x00F3, DONE,
    163, 0x00FA, DONE,
    164, 0x0148, DONE,
    165, 0x0147, DONE,
    166, 0x016E, DONE,
    167, 0x00D4, DONE,
    168, 0x0161, DONE,
    169, 0x0159, DONE,
    170, 0x0155, DONE,
    171, 0x0154, DONE,
    172, 0x00BC, DONE,
    173, 0x00A7, DONE,
#if LUKAS
    174, 0x00AB, DONE,
    175, 0x00BB, DONE,
#endif
#if MARTIN
    174, 0x00BB, DONE,		/* right-pointing double angle quotation mark */
    175, 0x00AB, DONE,		/* left-pointing double angle quotation mark */
#endif
    176, 0x2591, DONE,
    177, 0x2592, DONE,
    178, 0x2593, DONE,
    179, 0x2502, DONE,
    180, 0x2524, DONE,
    181, 0x2561, DONE,
    182, 0x2562, DONE,
    183, 0x2556, DONE,
    184, 0x2555, DONE,
    185, 0x2563, DONE,
    186, 0x2551, DONE,
    187, 0x2557, DONE,
    188, 0x255D, DONE,
    189, 0x255C, DONE,
    190, 0x255B, DONE,
    191, 0x2510, DONE,
    192, 0x2514, DONE,
    193, 0x2534, DONE,
    194, 0x252C, DONE,
    195, 0x251C, DONE,
    196, 0x2500, DONE,
    197, 0x253C, DONE,
    198, 0x255E, DONE,
    199, 0x255F, DONE,
    200, 0x255A, DONE,
    201, 0x2554, DONE,
    202, 0x2569, DONE,
    203, 0x2566, DONE,
    204, 0x2560, DONE,
    205, 0x2550, DONE,
    206, 0x256C, DONE,
    207, 0x2567, DONE,
    208, 0x2568, DONE,
    209, 0x2564, DONE,
    210, 0x2565, DONE,
    211, 0x2559, DONE,
    212, 0x2558, DONE,
    213, 0x2552, DONE,
    214, 0x2553, DONE,
    215, 0x256B, DONE,
    216, 0x256A, DONE,
    217, 0x2518, DONE,
    218, 0x250C, DONE,
    219, 0x2588, DONE,
    220, 0x2584, DONE,
    221, 0x258C, DONE,
    222, 0x2590, DONE,
    223, 0x2580, DONE,
    224, 0x03B1, DONE,
    225, 0x03B2, DONE,
#if LUKAS
    226, 0x0393, DONE,
#endif
#if MARTIN
    226, 0x0194, DONE,		/* latin capital letter gamma */
#endif
    227, 0x03C0, DONE,
    228, 0x03A3, DONE,
    229, 0x03C3, DONE,
    230, 0x03BC, DONE,
    231, 0x03C4, DONE,
    232, 0x03A6, DONE,
    233, 0x0398, DONE,
    234, 0x03A9, DONE,
    235, 0x03B4, DONE,
    236, 0x221E, DONE,
#if LUKAS
    237, 0x2205, DONE,
    238, 0x03B5, DONE,
    239, 0x2229, DONE,
    240, 0x2261, DONE,
#endif
#if MARTIN
    237, 0x03C6, DONE,		/* greek small letter phi */
    238, 0x2208, DONE,		/* element of */
    239, 0x2229, DONE,		/* intersection */
    240, 0x224D, DONE,		/* equivalent to */
#endif
    241, 0x00B1, DONE,
    242, 0x2265, DONE,
    243, 0x2264, DONE,
    244, 0x2320, DONE,
    245, 0x2321, DONE,
    246, 0x00F7, DONE,
    247, 0x2248, DONE,
#if LUKAS
    248, 0x2218, DONE,
    249, 0x00B7, DONE,
    250, 0x2219, DONE,
#endif
#if MARTIN
    248, 0x00B0, DONE,		/* degree sign */
    249, 0x2219, DONE,		/* bullet operator */
    250, 0x00B7, DONE,		/* middle dot */
#endif
    251, 0x221A, DONE,
    252, 0x207F, DONE,
    253, 0x00B2, DONE,
    254, 0x25A0, DONE,
    255, 0x00A0, DONE,
    DONE
  };

/* CORK, T1.  */

static const unsigned short data_cork[] =
  {
#if LUKAS
    /* I suspect Lukas used this mapping to convey T1 and CORK in a single
       table, which may not be such a good thing.  I'm not sure...  */
      0, 0x0060, DONE,
      1, 0x00B4, DONE,
      2, 0x005E, DONE,
      3, 0x007E, DONE,
      4, 0x00A8, DONE,
      5, 0x02DD, DONE,
      6, 0x02DA, DONE,
      7, 0x02C7, DONE,
      8, 0x02D8, DONE,
      9, 0x00AF, DONE,
     10, 0x02D9, DONE,
     11, 0x00B8, DONE,
     12, 0x02DB, DONE,
     13, 0x201A, DONE,
     14, 0x2039, DONE,
     15, 0x203A, DONE,
     16, 0x201C, DONE,
     17, 0x201D, DONE,
     18, 0x201E, DONE,
     19, 0x00AB, DONE,
     20, 0x00BB, DONE,
     21, 0x2013, DONE,
     22, 0x2014, DONE,
     23, DONE,
     24, 0x2080, DONE,
     25, 0x0131, DONE,
     26, DONE,			/* latin small letter j dotless, FIXME! */
     27, 0xFB00, DONE,
     28, 0xFB01, DONE,
     29, 0xFB02, DONE,
     30, 0xFB03, DONE,
     31, 0xFB04, DONE,
#endif /* LUKAS */
    127, 0x2010, DONE,
    128, 0x0102, DONE,
    129, 0x0104, DONE,
    130, 0x0106, DONE,
    131, 0x010C, DONE,
    132, 0x010E, DONE,
    133, 0x011A, DONE,
    134, 0x0118, DONE,
    135, 0x011E, DONE,
    136, 0x0139, DONE,
    137, 0x013D, DONE,
    138, 0x0141, DONE,
    139, 0x0143, DONE,
    140, 0x0147, DONE,
    141, 0x014A, DONE,
    142, 0x0150, DONE,
    143, 0x0154, DONE,
    144, 0x0158, DONE,
    145, 0x015A, DONE,
    146, 0x0160, DONE,
    147, 0x015E, DONE,
    148, 0x0164, DONE,
    149, 0x0162, DONE,
    150, 0x0170, DONE,
    151, 0x016E, DONE,
    152, 0x0178, DONE,
    153, 0x0179, DONE,
    154, 0x017D, DONE,
    155, 0x017B, DONE,
    156, 0x0132, DONE,
    157, 0x0130, DONE,
    158, 0x00F0, DONE,
    159, 0x00A7, DONE,
    160, 0x0103, DONE,
    161, 0x0105, DONE,
    162, 0x0107, DONE,
    163, 0x010D, DONE,
    164, 0x010F, DONE,
    165, 0x011B, DONE,
    166, 0x0119, DONE,
    167, 0x011F, DONE,
    168, 0x013A, DONE,
    169, 0x013E, DONE,
    170, 0x0142, DONE,
    171, 0x0144, DONE,
    172, 0x0148, DONE,
    173, 0x014B, DONE,
    174, 0x0151, DONE,
    175, 0x0155, DONE,
    176, 0x0159, DONE,
    177, 0x015B, DONE,
    178, 0x0161, DONE,
    179, 0x015F, DONE,
    180, 0x0165, DONE,
    181, 0x0163, DONE,
    182, 0x0171, DONE,
    183, 0x016F, DONE,
    184, 0x00FF, DONE,
    185, 0x017A, DONE,
    186, 0x017E, DONE,
    187, 0x017C, DONE,
    188, 0x0133, DONE,
    189, 0x00A1, DONE,
    190, 0x00BF, DONE,
    191, 0x00A3, DONE,
    215, 0x0152, DONE,
    223, DONE,			/* latin capital letter sharp s (german)
				   (it is the SS ligature), FIXME! */
    247, 0x0153, DONE,
    255, 0x00DF, DONE,
    DONE
  };

/* KOI-8_CS2.  Source: CSN 36 9103.
   From Lukas Petrlik <luki@pafos.zcu.cz>, 1996-04-02.

   &g1esc x2d49 &g2esc x2e49 &g3esc x2f49 */

static const unsigned short data_koi8cs2[] =
  {
     36, 0x00A4, DONE,
    161, DONE,
    162, 0x00B4, DONE,
    163, DONE,
    164, 0x007E, DONE,
    165, DONE,
    166, 0x02D8, DONE,
    167, 0x02D9, DONE,
    169, DONE,
    170, 0x02DA, DONE,
    171, 0x00B8, DONE,
    172, DONE,
    173, 0x02DD, DONE,
    174, 0x02DB, DONE,
    175, 0x02C7, DONE,
    176, 0x00A9, DONE,
    177, 0x2122, DONE,
    178, 0x250C, DONE,
    179, 0x2510, DONE,
    180, 0x2514, DONE,
    181, 0x2518, DONE,
    182, 0x2500, DONE,
    183, 0x2193, DONE,
    184, 0x03A9, DONE,
    185, 0x00A7, DONE,
    186, 0x03B1, DONE,
    187, 0x03B3, DONE,
    188, 0x03B5, DONE,
    189, 0x03BC, DONE,
    190, 0x03C0, DONE,
    191, 0x03C9, DONE,
    192, 0x00E0, DONE,
    193, 0x00E1, DONE,
    194, 0x01CE, DONE,
    195, 0x010D, DONE,
    196, 0x010F, DONE,
    197, 0x011B, DONE,
    198, 0x0155, DONE,
    199, DONE,			/* ch digraph as a single character,
				   as used in the Czech alphabet, FIXME!  */
    200, 0x00FC, DONE,
    201, 0x00ED, DONE,
    202, 0x016F, DONE,
    203, 0x013A, DONE,
    204, 0x013E, DONE,
    205, 0x00F6, DONE,
    206, 0x0148, DONE,
    207, 0x00F3, DONE,
    208, 0x00F4, DONE,
    209, 0x00E4, DONE,
    210, 0x0159, DONE,
    211, 0x0161, DONE,
    212, 0x0165, DONE,
    213, 0x00FA, DONE,
    214, 0x00EB, DONE,
    215, 0x00E9, DONE,
    216, 0x0171, DONE,
    217, 0x00FD, DONE,
    218, 0x017E, DONE,
    219, DONE,
    220, DONE,
    221, 0x0151, DONE,
    222, 0x0117, DONE,
    224, 0x00C0, DONE,
    225, 0x00C1, DONE,
    226, 0x01CD, DONE,
    227, 0x010C, DONE,
    228, 0x010E, DONE,
    229, 0x011A, DONE,
    230, 0x0154, DONE,
    231, DONE,			/* CH digraph as a single character,
				   as used in the Czech alphabet, FIXME!  */
    232, 0x00DC, DONE,
    233, 0x00CD, DONE,
    234, 0x016E, DONE,
    235, 0x0139, DONE,
    236, 0x013D, DONE,
    237, 0x00D6, DONE,
    238, 0x0147, DONE,
    239, 0x00D3, DONE,
    240, 0x00D4, DONE,
    241, 0x00C4, DONE,
    242, 0x0158, DONE,
    243, 0x0160, DONE,
    244, 0x0164, DONE,
    245, 0x00DA, DONE,
    246, 0x00CB, DONE,
    247, 0x00C9, DONE,
    248, 0x0170, DONE,
    249, 0x00DD, DONE,
    250, 0x017D, DONE,
    251, DONE,
    252, DONE,
    253, 0x0150, DONE,
    254, 0x0116, DONE,
    255, DONE,
    DONE
  };

/* Cyrillic tables?  */

/* Helping people.

   "Alexander L. Belikoff" <abel@bfr.co.il>
   Andrey A. Chernov <ache@null.net>
   Andries Brouwer <Andries.Brouwer@cwi.nl>
   Roman Czyborra <czyborra@cs.tu-berlin.de>
*/

/* What is apparently still missing, from various mail I got, is:

   IBM866		CP866, alt
   KOI8-Unified

   Here are various sources of information.

   Andrey A. Chernov <ache@null.net>
   http://www.nagual.pp.ru/~ache/

   &charset KOI8-Unified
   &rem source: http://www.cyrillic.com/ref/cyrillic/koi8-uni.html
*/

/* @item IBM866
   @code{866}, @code{cp866}, and @code{alt} are aliases for this charset.
   source: unknown

   @item KOI8-R
   @code{878}, @code{cp878}, @code{ibm878}, @code{koi8r}, and
   @code{rfc1489} are aliases for this charset.
   source: RFC1489
*/

static unsigned char const koi8r_to_ibm866[256-128] =
{
    196, 179, 218, 191, 192, 217, 195, 180, /* 128 - 135 */
    194, 193, 197, 223, 220, 219, 221, 222, /* 136 - 143 */
    176, 177, 178, 244, 254, 249, 251, 247, /* 144 - 151 */
    243, 242, 255, 245, 248, 253, 250, 246, /* 152 - 159 */
    205, 186, 213, 241, 214, 201, 184, 183, /* 160 - 167 */
    187, 212, 211, 200, 190, 189, 188, 198, /* 168 - 175 */
    199, 204, 181, 240, 182, 185, 209, 210, /* 176 - 183 */
    203, 207, 208, 202, 216, 215, 206, 252, /* 184 - 191 */
    238, 160, 161, 230, 164, 165, 228, 163, /* 192 - 199 */
    229, 168, 169, 170, 171, 172, 173, 174, /* 200 - 207 */
    175, 239, 224, 225, 226, 227, 166, 162, /* 208 - 215 */
    236, 235, 167, 232, 237, 233, 231, 234, /* 216 - 223 */
    158, 128, 129, 150, 132, 133, 148, 131, /* 224 - 231 */
    149, 136, 137, 138, 139, 140, 141, 142, /* 232 - 239 */
    143, 159, 144, 145, 146, 147, 134, 130, /* 240 - 247 */
    156, 155, 135, 152, 157, 153, 151, 154  /* 248 - 255 */
};

static unsigned char const koi8r_to_cp1251[256-128] =
{
    128, 129, 130, 131, 132, 133, 134, 135, /* 128 - 135 */
    136, 137, 138, 139, 140, 141, 142, 143, /* 136 - 143 */
    144, 145, 146, 147, 148, 149, 150, 151, /* 144 - 151 */
    152, 153, 154, 155, 156, 157, 158, 159, /* 152 - 159 */
    160, 161, 162, 163, 164, 165, 166, 167, /* 160 - 167 */
    168, 169, 170, 171, 172, 173, 174, 175, /* 168 - 175 */
    176, 177, 178, 179, 180, 181, 182, 183, /* 176 - 183 */
    184, 185, 186, 187, 188, 189, 190, 191, /* 184 - 191 */
    254, 224, 225, 246, 228, 229, 244, 227, /* 192 - 199 */
    245, 232, 233, 234, 235, 236, 237, 238, /* 200 - 207 */
    239, 255, 240, 241, 242, 243, 230, 226, /* 208 - 215 */
    252, 251, 231, 248, 253, 249, 247, 250, /* 216 - 223 */
    222, 192, 193, 214, 196, 197, 212, 195, /* 224 - 231 */
    213, 200, 201, 202, 203, 204, 205, 206, /* 232 - 239 */
    207, 223, 208, 209, 210, 211, 198, 194, /* 240 - 247 */
    220, 219, 199, 216, 221, 217, 215, 218  /* 248 - 255 */
};

static unsigned char const koi8r_to_iso8859_5[256-128] =
{
    128, 129, 130, 131, 132, 133, 134, 135, /* 128 - 135 */
    136, 137, 138, 139, 140, 141, 142, 143, /* 136 - 143 */
    144, 145, 146, 147, 148, 149, 150, 151, /* 144 - 151 */
    152, 153, 154, 155, 156, 157, 158, 159, /* 152 - 159 */
    160, 161, 162, 163, 164, 165, 166, 167, /* 160 - 167 */
    168, 169, 170, 171, 172, 173, 174, 175, /* 168 - 175 */
    240, 241, 242, 243, 244, 245, 246, 247, /* 176 - 183 */
    248, 249, 250, 251, 252, 253, 254, 255, /* 184 - 191 */
    238, 208, 209, 230, 212, 213, 228, 211, /* 192 - 199 */
    229, 216, 217, 218, 219, 220, 221, 222, /* 200 - 207 */
    223, 239, 224, 225, 226, 227, 214, 210, /* 208 - 215 */
    236, 235, 215, 232, 237, 233, 231, 234, /* 216 - 223 */
    206, 176, 177, 198, 180, 181, 196, 179, /* 224 - 231 */
    197, 184, 185, 186, 187, 188, 189, 190, /* 232 - 239 */
    191, 207, 192, 193, 194, 195, 182, 178, /* 240 - 247 */
    204, 203, 183, 200, 205, 201, 199, 202  /* 248 - 255 */
};

bool
module_varia (RECODE_OUTER outer)
{
  /* Czech tables.  */

  if (!declare_explode_data (outer, data_kamenicky, "KEYBCS2", NULL))
    return false;
  if (!declare_explode_data (outer, data_cork, "CORK", NULL))
    return false;
  if (!declare_explode_data (outer, data_koi8cs2, "KOI-8_CS2", NULL))
    return false;

  if (!declare_alias (outer, "Kamenicky", "KEYBCS2"))
    return false;
  if (!declare_alias (outer, "T1", "CORK"))
    return false;

#if NOT_ACTIVE_YET
  return
    && declare_single(outer, "KOI8-R", "IBM866",
		      outer->quality_byte_reversible,
		      NULL, transform_byte_to_byte);

    /* Suggested by Alexander L. Belikoff.  */
    if (!declare_alias("866", "IBM866"))
      return false;
    if (!declare_alias("CP866", "IBM866"))
      return false;
    if (!declare_alias("alt", "IBM866"))
      return false;

    /* Suggested by Alexander L. Belikoff.  */
    if (!declare_alias ("1489", "KOI8-R"))
      return false;
    if (!declare_alias ("RFC1489", "KOI8-R"))
      return false;

    /* Suggested by Alexander L. Belikoff.  */
    if (!declare_alias ("878", "KOI8-R"))
      return false;
    if (!declare_alias ("CP878", "KOI8-R"))
      return false;
    if (!declare_alias ("IBM878", "KOI8-R");)
      return false;
#endif

  return true;
}

void
delmodule_varia (RECODE_OUTER outer)
{
}
