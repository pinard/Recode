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

/* Define for using new double step code.  */
#define USE_DOUBLE_STEP 1

#include "recode.h"

#ifdef USE_DOUBLE_STEP

/* RFC 1345 style description for AtariST (non official).  */

/* Andreas Schwab writes:
   
   There are some characters which i'm not sure about.  When compared
   with IBM865 (which i think is the original source for the atari
   charset) the characters "bullet operator" and "middle dot" (dec
   249/250) are swapped.  This may be intentional or a bug in the table
   for IBM865.  Also character dec 238 looks more like "element of" than
   "greek small letter epsilon", actually it is a bit too large for both
   interpretations.  I suppose that the other changes were made for
   copyright reasons.  */

static DOUBLE_TABLE table =
  {
    "NUSHSXEXETEQAKBLBSHTLFVTFFCRSOSIDLD1D2D3D4NKSYEBCNEMSBECFSGSRSUS",
    "SP! \" NbDO% & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? ",
    "AtA B C D E F G H I J K L M N O P Q R S T U V W X Y Z <(//)>'>_ ",
    "'!a b c d e f g h i j k l m n o p q r s t u v w x y z (!!!!)'?DT",
    "C,u:e'a>a:a!aac,e>e:e!i:i>i!A:AAE'aeAEo>o:o!u>u!y:O:U:CtPdYessFl",
    "a'i'o'u'n?N?-a-o?ININO1214!I<<>>a?o?O/o/oeOEA!A?O?':''/-PICoRgTM",
    "ijIJA+B+G+D+H+W+Z+X+TjJ+K%K+L+M%M+N%N+S+E+P%P+ZjZJQ+R+ShT+SEAN00",
    "a*b*G*p*S*s*m*t*F*H*W*d*Io/0(-(U=3+->==<IuIl-:?2ObSb.MRTnS2S3S'-",
  };

#else /* not USE_DOUBLE_STEP */

/* Data for Atari ST to ISO Latin-1 code conversions.  */

static KNOWN_PAIR known_pairs[] =
  {
    {128, 199},			/* C, */
    {129, 252},			/* u" */
    {130, 233},			/* e' */
    {131, 226},			/* a^ */
    {132, 228},			/* a" */
    {133, 224},			/* a` */
    {134, 229},			/* aa */
    {135, 231},			/* c, */
    {136, 234},			/* e^ */
    {137, 235},			/* e" */
    {138, 232},			/* e` */
    {139, 239},			/* i" */
    {140, 238},			/* i^ */
    {141, 236},			/* i` */
    {142, 196},			/* A" */
    {143, 197},			/* AA */
    {144, 201},			/* E' */
    {145, 230},			/* ae */
    {146, 198},			/* AE */
    {147, 244},			/* o^ */
    {148, 246},			/* o" */
    {149, 242},			/* o` */
    {150, 251},			/* u^ */
    {151, 249},			/* u` */
    {152, 255},			/* y" */
    {153, 214},			/* O" */
    {154, 220},			/* U" */
    {155, 162},			/* \cent */
    {156, 163},			/* \pound */
    {157, 165},			/* \yen */
    {158, 223},			/* \ss */

    {160, 225},			/* a' */
    {161, 237},			/* i' */
    {162, 243},			/* o' */
    {163, 250},			/* u' */
    {164, 241},			/* n~ */
    {165, 209},			/* N~ */
    {166, 170},			/* a_ */
    {167, 186},			/* o_ */
    {168, 191},			/* ?' */

    {170, 172},			/* \neg */
    {171, 189},			/* 1/2 */
    {172, 188},			/* 1/4 */
    {173, 161},			/* !` */
    {174, 171},			/* `` */
    {175, 187},			/* '' */
    {176, 227},			/* a~ */
    {177, 245},			/* o~ */
    {178, 216},			/* O/ */
    {179, 248},			/* o/ */

    {182, 192},			/* A` */
    {183, 195},			/* A~ */
    {184, 213},			/* O~ */
    {185, 168},			/* diaeresis */
    {186, 180},			/* acute accent */

    {188, 182},			/* pilcrow sign */
    {189, 169},			/* copyright sign */
    {190, 174},			/* registered trade mark sign */

    {221, 167},			/* paragraph sign, section sign */

    {230, 181},			/* mu, micro */

    {241, 177},			/* +- */

    {246, 247},			/* \div */

    {248, 176},			/* \deg */

    {250, 183},			/* \cdot */

    {253, 178},			/* ^2 */
    {254, 179},			/* ^3 */
  };
#define NUMBER_OF_PAIRS (sizeof (known_pairs) / sizeof (KNOWN_PAIR))

static void
init_latin1_atarist (STEP *step)
{
  complete_pairs (step, 1, known_pairs, NUMBER_OF_PAIRS, 1);
}

static void
init_atarist_latin1 (STEP *step)
{
  complete_pairs (step, 1, known_pairs, NUMBER_OF_PAIRS, 0);
}

#endif /* not USE_DOUBLE_STEP */

void
module_atarist (void)
{

#ifdef USE_DOUBLE_STEP

  declare_double_step (&table, "AtariST", 2);

#else /* not USE_DOUBLE_STEP */

  declare_step ("Latin-1", "AtariST",
		strict_mapping ? ONE_TO_MANY : REVERSIBLE,
		init_latin1_atarist, NULL);
  declare_step ("AtariST", "Latin-1",
		strict_mapping ? ONE_TO_MANY : REVERSIBLE,
		init_atarist_latin1, NULL);

#endif /* not USE_DOUBLE_STEP */

}
