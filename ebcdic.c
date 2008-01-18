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

#define ASCII "ASCII"
/* Previously: #define ASCII "ASCII-BS" */

#include "recode.h"

static unsigned char const ascii_to_ebcdic[256] =
  {
      0,   1,   2,   3,  55,  45,  46,  47,     /*   0 -   7 */
     22,   5,  37,  11,  12,  13,  14,  15,     /*   8 -  15 */
     16,  17,  18,  19,  60,  61,  50,  38,     /*  16 -  23 */
     24,  25,  63,  39,  28,  29,  30,  31,     /*  24 -  31 */
     64,  79, 127, 123,  91, 108,  80, 125,     /*  32 -  39 */
     77,  93,  92,  78, 107,  96,  75,  97,     /*  40 -  47 */
    240, 241, 242, 243, 244, 245, 246, 247,     /*  48 -  55 */
    248, 249, 122,  94,  76, 126, 110, 111,     /*  56 -  63 */
    124, 193, 194, 195, 196, 197, 198, 199,     /*  64 -  71 */
    200, 201, 209, 210, 211, 212, 213, 214,     /*  72 -  79 */
    215, 216, 217, 226, 227, 228, 229, 230,     /*  80 -  87 */
    231, 232, 233,  74, 224,  90,  95, 109,     /*  88 -  95 */
    121, 129, 130, 131, 132, 133, 134, 135,     /*  96 - 103 */
    136, 137, 145, 146, 147, 148, 149, 150,     /* 104 - 111 */
    151, 152, 153, 162, 163, 164, 165, 166,     /* 112 - 119 */
    167, 168, 169, 192, 106, 208, 161,   7,     /* 120 - 127 */
     32,  33,  34,  35,  36,  21,   6,  23,     /* 128 - 135 */
     40,  41,  42,  43,  44,   9,  10,  27,     /* 136 - 143 */
     48,  49,  26,  51,  52,  53,  54,   8,     /* 144 - 151 */
     56,  57,  58,  59,   4,  20,  62, 225,     /* 152 - 159 */
     65,  66,  67,  68,  69,  70,  71,  72,     /* 160 - 167 */
     73,  81,  82,  83,  84,  85,  86,  87,     /* 168 - 175 */
     88,  89,  98,  99, 100, 101, 102, 103,     /* 176 - 183 */
    104, 105, 112, 113, 114, 115, 116, 117,     /* 184 - 191 */
    118, 119, 120, 128, 138, 139, 140, 141,     /* 192 - 199 */
    142, 143, 144, 154, 155, 156, 157, 158,     /* 200 - 207 */
    159, 160, 170, 171, 172, 173, 174, 175,     /* 208 - 215 */
    176, 177, 178, 179, 180, 181, 182, 183,     /* 216 - 223 */
    184, 185, 186, 187, 188, 189, 190, 191,     /* 224 - 231 */
    202, 203, 204, 205, 206, 207, 218, 219,     /* 232 - 239 */
    220, 221, 222, 223, 234, 235, 236, 237,     /* 240 - 247 */
    238, 239, 250, 251, 252, 253, 254, 255,     /* 248 - 255 */
  };

static unsigned char const ascii_to_ebcdic_ccc[256] =
  {
      0,   1,   2,   3,  55,  45,  46,  47,     /*   0 -   7 */
     22,   5,  37,  11,  12,  13,  14,  15,     /*   8 -  15 */
     16,  17,  18,  19,  60,  61,  50,  38,     /*  16 -  23 */
     24,  25,  63,  39,  28,  29,  30,  31,     /*  24 -  31 */
     64,  90, 127, 123,  91, 108,  80, 125,     /*  32 -  39 */
     77,  93,  92,  78, 107,  96,  75,  97,     /*  40 -  47 */
    240, 241, 242, 243, 244, 245, 246, 247,     /*  48 -  55 */
    248, 249, 122,  94,  76, 126, 110, 111,     /*  56 -  63 */
    124, 193, 194, 195, 196, 197, 198, 199,     /*  64 -  71 */
    200, 201, 209, 210, 211, 212, 213, 214,     /*  72 -  79 */
    215, 216, 217, 226, 227, 228, 229, 230,     /*  80 -  87 */
    231, 232, 233,  74, 224,  79,  95, 109,     /*  88 -  95 */
    121, 129, 130, 131, 132, 133, 134, 135,     /*  96 - 103 */
    136, 137, 145, 146, 147, 148, 149, 150,     /* 104 - 111 */
    151, 152, 153, 162, 163, 164, 165, 166,     /* 112 - 119 */
    167, 168, 169, 192, 106, 208, 161,   7,     /* 120 - 127 */
      4,   6,   8,   9,  10,  20,  21,  23,     /* 128 - 135 */
     26,  27, 138, 139, 140, 141, 142, 143,     /* 136 - 143 */
     32,  33,  34,  35,  36,  40,  41,  42,     /* 144 - 151 */
     43,  44, 154, 155, 156, 157, 158, 159,     /* 152 - 159 */
     48,  49,  51,  52,  53,  54,  56,  57,     /* 160 - 167 */
     58,  59, 170, 171, 172, 173, 174, 175,     /* 168 - 175 */
    176, 177, 178, 179, 180, 181, 182, 183,     /* 176 - 183 */
    184, 185, 186, 187, 188, 189, 190, 191,     /* 184 - 191 */
    128,  65,  66,  67,  68,  69,  70,  71,     /* 192 - 199 */
     72,  73, 202, 203, 204, 205, 206, 207,     /* 200 - 207 */
    144,  81,  82,  83,  84,  85,  86,  87,     /* 208 - 215 */
     88,  89, 218, 219, 220, 221, 222, 223,     /* 216 - 223 */
    160, 225,  98,  99, 100, 101, 102, 103,     /* 224 - 231 */
    104, 105, 234, 235, 236, 237, 238, 239,     /* 232 - 239 */
    112, 113, 114, 115, 116, 117, 118, 119,     /* 240 - 247 */
    120,  62, 250, 251, 252, 253, 254, 255,     /* 248 - 255 */
  };

/* This is almost identical to GNU dd's "ibm" table.  In dd's "ibm"
   table, both 91 and 213 recode to 173, both 93 and 229 recode to 189,
   and no character recodes to 74 or 106.  I suspect two errors in there.
   So, I arbitrarily choose to recode 213 by 74 and 229 by 106.  */

static unsigned char const ascii_to_ebcdic_ibm[256] =
  {
      0,   1,   2,   3,  55,  45,  46,  47,     /*   0 -   7 */
     22,   5,  37,  11,  12,  13,  14,  15,     /*   8 -  15 */
     16,  17,  18,  19,  60,  61,  50,  38,     /*  16 -  23 */
     24,  25,  63,  39,  28,  29,  30,  31,     /*  24 -  31 */
     64,  90, 127, 123,  91, 108,  80, 125,     /*  32 -  39 */
     77,  93,  92,  78, 107,  96,  75,  97,     /*  40 -  47 */
    240, 241, 242, 243, 244, 245, 246, 247,     /*  48 -  55 */
    248, 249, 122,  94,  76, 126, 110, 111,     /*  56 -  63 */
    124, 193, 194, 195, 196, 197, 198, 199,     /*  64 -  71 */
    200, 201, 209, 210, 211, 212, 213, 214,     /*  72 -  79 */
    215, 216, 217, 226, 227, 228, 229, 230,     /*  80 -  87 */
    231, 232, 233, 173, 224, 189,  95, 109,     /*  88 -  95 */
    121, 129, 130, 131, 132, 133, 134, 135,     /*  96 - 103 */
    136, 137, 145, 146, 147, 148, 149, 150,     /* 104 - 111 */
    151, 152, 153, 162, 163, 164, 165, 166,     /* 112 - 119 */
    167, 168, 169, 192,  79, 208, 161,   7,     /* 120 - 127 */
     32,  33,  34,  35,  36,  21,   6,  23,     /* 128 - 135 */
     40,  41,  42,  43,  44,   9,  10,  27,     /* 136 - 143 */
     48,  49,  26,  51,  52,  53,  54,   8,     /* 144 - 151 */
     56,  57,  58,  59,   4,  20,  62, 225,     /* 152 - 159 */
     65,  66,  67,  68,  69,  70,  71,  72,     /* 160 - 167 */
     73,  81,  82,  83,  84,  85,  86,  87,     /* 168 - 175 */
     88,  89,  98,  99, 100, 101, 102, 103,     /* 176 - 183 */
    104, 105, 112, 113, 114, 115, 116, 117,     /* 184 - 191 */
    118, 119, 120, 128, 138, 139, 140, 141,     /* 192 - 199 */
    142, 143, 144, 154, 155, 156, 157, 158,     /* 200 - 207 */
    159, 160, 170, 171, 172,  74, 174, 175,     /* 208 - 215 */
    176, 177, 178, 179, 180, 181, 182, 183,     /* 216 - 223 */
    184, 185, 186, 187, 188, 106, 190, 191,     /* 224 - 231 */
    202, 203, 204, 205, 206, 207, 218, 219,     /* 232 - 239 */
    220, 221, 222, 223, 234, 235, 236, 237,     /* 240 - 247 */
    238, 239, 250, 251, 252, 253, 254, 255,     /* 248 - 255 */
  };

static void
init_ascii_ebcdic (STEP *step)
{
  step->one_to_one = ascii_to_ebcdic;
}

static void
init_ebcdic_ascii (STEP *step)
{
  step->one_to_one = invert_table (ascii_to_ebcdic);
}

static void
init_ascii_ebcdic_ccc (STEP *step)
{
  step->one_to_one = ascii_to_ebcdic_ccc;
}

static void
init_ebcdic_ccc_ascii (STEP *step)
{
  step->one_to_one = invert_table (ascii_to_ebcdic_ccc);
}

static void
init_ascii_ebcdic_ibm (STEP *step)
{
  step->one_to_one = ascii_to_ebcdic_ibm;
}

static void
init_ebcdic_ibm_ascii (STEP *step)
{
  step->one_to_one = invert_table (ascii_to_ebcdic_ibm);
}

void
module_ebcdic (void)
{
  declare_step (ASCII, "EBCDIC", REVERSIBLE, init_ascii_ebcdic,
		file_one_to_one);
  declare_step ("EBCDIC", ASCII, REVERSIBLE, init_ebcdic_ascii,
		file_one_to_one);
  declare_step (ASCII, "EBCDIC-CCC", REVERSIBLE, init_ascii_ebcdic_ccc,
		file_one_to_one);
  declare_step ("EBCDIC-CCC", ASCII, REVERSIBLE, init_ebcdic_ccc_ascii,
		file_one_to_one);
  declare_step (ASCII, "EBCDIC-IBM", REVERSIBLE, init_ascii_ebcdic_ibm,
		file_one_to_one);
  declare_step ("EBCDIC-IBM", ASCII, REVERSIBLE, init_ebcdic_ibm_ascii,
		file_one_to_one);
}
