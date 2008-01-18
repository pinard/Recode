/* Conversion of files between different charsets and usages.
   Copyright (C) 1990, 1993 Free Software Foundation, Inc.
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

/* Data for Apple Macintosh to ISO Latin-1 code conversions.  */

static KNOWN_PAIR known_pairs[] =
  {
    { 13,  10},			/* (recoding end of lines for Unix) */

    {128, 196},			/* capital letter A diaeresis */
    {129, 197},			/* capital letter A with ring above */
    {130, 199},			/* capital letter C with cedilla */
    {131, 201},			/* capital letter E with acute accent */
    {132, 209},			/* capital letter N with tilde */
    {133, 214},			/* capital letter O with diaeresis */
    {134, 220},			/* capital letter U with diaeresis */
    {135, 225},			/* small letter a with acute accent */
    {136, 224},			/* small letter a with grave accent */
    {137, 226},			/* small letter a with circumflex accent */
    {138, 228},			/* small letter a with diaeresis */
    {139, 227},			/* small letter a with tilde */
    {140, 229},			/* small letter a with ring above */
    {141, 231},			/* small letter c with cedilla */
    {142, 233},			/* small letter e with acute accent */
    {143, 232},			/* small letter e with grave accent */
    {144, 234},			/* small letter e with circumflex accent */
    {145, 235},			/* small letter e with diaeresis */
    {146, 237},			/* small letter i with acute accent */
    {147, 236},			/* small letter i with grave accent */
    {148, 238},			/* small letter i with circumflex accent */
    {149, 239},			/* small letter i with diaeresis */
    {150, 241},			/* small letter n with tilde */
    {151, 243},			/* small letter o with acute accent */
    {152, 242},			/* small letter o with grave accent */
    {153, 244},			/* small letter o with circumflex accent */
    {154, 246},			/* small letter o with diaeresis */
    {155, 245},			/* small letter o with tilde */
    {156, 250},			/* small letter u with acute accent */
    {157, 249},			/* small letter u with grave accent */
    {158, 251},			/* small letter u with circumflex accent */
    {159, 252},			/* small letter u with diaeresis */

    {161, 176},			/* degree sign */
    {162, 162},			/* cent sign */
    {163, 163},			/* pound sign */
    {164, 167},			/* paragraph sign, section sign */

    {166, 182},			/* pilcrow sign */
    {167, 223},			/* small german letter sharp s */
    {168, 174},			/* registered trade mark sign */
    {169, 169},			/* copyright sign */

    {174, 198},			/* capital diphthong A with E */
    {175, 216},			/* capital letter O with oblique stroke */

    {177, 177},			/* plus-minus sign */

    {180, 165},			/* yen sign */
    {181, 181},			/* small greek letter mu, micro sign */

    {187, 170},			/* feminine ordinal indicator */
    {188, 186},			/* masculine ordinal indicator */

    {190, 230},			/* small diphthong a with e */
    {191, 248},			/* small letter o with oblique stroke */
    {192, 191},			/* inverted question mark */
    {193, 161},			/* inverted exclamation mark */
    {194, 172},			/* not sign */

    {199, 171},			/* left angle quotation mark */
    {200, 187},			/* right angle quotation mark */

    {203, 192},			/* capital letter A with grave accent */
    {204, 195},			/* capital letter A with tilde */
    {205, 213},			/* capital letter O with tilde */

    {214, 247},			/* division sign */

    {216, 255},			/* small letter y with diaeresis */
    {217, 221},			/* capital letter Y with acute accent */

    {229, 194},			/* capital letter A with circumflex accent */
    {230, 202},			/* capital letter E with circumflex accent */
    {231, 193},			/* capital letter A with acute accent */
    {232, 203},			/* capital letter E with diaeresis */
    {233, 200},			/* capital letter E with grave accent */
    {234, 205},			/* capital letter I with acute accent */
    {235, 206},			/* capital letter I with circumflex accent */
    {236, 207},			/* capital letter I with diaeresis */
    {237, 204},			/* capital letter I with grave accent */
    {238, 211},			/* capital letter O with acute accent */
    {239, 212},			/* capital letter O with circumflex accent */

    {241, 210},			/* capital letter O with grave accent */
    {242, 218},			/* capital letter U with acute accent */
    {243, 219},			/* capital letter U with circumflex accent */
    {244, 217},			/* capital letter U with grave accent */
  };
#define NUMBER_OF_PAIRS (sizeof (known_pairs) / sizeof (KNOWN_PAIR))

static void
init_latin1_applemac (STEP *step)
{
  complete_pairs (step, 1, known_pairs, NUMBER_OF_PAIRS, 1);
}

static void
init_applemac_latin1 (STEP *step)
{
  complete_pairs (step, 1, known_pairs, NUMBER_OF_PAIRS, 0);
}

void
module_applemac (void)
{
  declare_step ("Latin-1", "Apple-Mac",
		strict_mapping ? ONE_TO_MANY : REVERSIBLE,
		init_latin1_applemac, NULL);
  declare_step ("Apple-Mac", "Latin-1",
		strict_mapping ? ONE_TO_MANY : REVERSIBLE,
		init_applemac_latin1, NULL);

#if 0
  declare_alias ("Apple-Mac", "macintosh");
#endif
}
