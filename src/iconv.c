/* Copyright (C) 1999 Free Software Foundation, Inc.
   This file is part of the GNU ICONV Library.

   The GNU ICONV Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU ICONV Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU ICONV Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "common.h"
#include "libiconv.h"

/* Define our own notion of wchar_t, as UCS-4, according to ISO-10646-1. */
#undef wchar_t
#define wchar_t  unsigned int

/* State used by a conversion. 0 denotes the initial state. */
typedef unsigned int state_t;

/* iconv_t is an opaque type. This is the real iconv_t type. */
typedef struct conv_struct * conv_t;

/*
 * Data type for conversion multibyte -> unicode
 */
struct mbtowc_funcs {
  int (*xxx_mbtowc) (conv_t conv, wchar_t *pwc, unsigned char const *s, int n);
  /*
   * int xxx_mbtowc (conv_t conv, wchar_t *pwc, unsigned char const *s, int n)
   * converts the byte sequence starting at s to a wide character. Up to n bytes
   * are available at s. n is >= 1.
   * Result is number of bytes consumed (if a wide character was read),
   * or 0 if invalid, or -1 if n too small, or -1-(number of bytes consumed)
   * if only a shift sequence was read.
   */
};

/*
 * Data type for conversion unicode -> multibyte
 */
struct wctomb_funcs {
  int (*xxx_wctomb) (conv_t conv, unsigned char *r, wchar_t wc, int n);
  /*
   * int xxx_wctomb (conv_t conv, unsigned char *r, wchar_t wc, int n)
   * converts the wide character wc to the character set xxx, and stores the
   * result beginning at r. Up to n bytes may be written at r. n is >= 1.
   * Result is number of bytes written, or 0 if invalid, or -1 if n too small.
   */
  int (*xxx_reset) (conv_t conv, unsigned char *r, int n);
  /*
   * int xxx_reset (conv_t conv, unsigned char *r, int n)
   * stores a shift sequences returning to the initial state beginning at r.
   * Up to n bytes may be written at r. n is >= 0.
   * Result is number of bytes written, or -1 if n too small.
   */
};

/* Return code if invalid. (xxx_mbtowc, xxx_wctomb) */
#define RET_ILSEQ      0
/* Return code if only a shift sequence of n bytes was read. (xxx_mbtowc) */
#define RET_TOOFEW(n)  (-1-(n))
/* Return code if output buffer is too small. (xxx_wctomb, xxx_reset) */
#define RET_TOOSMALL   -1

/*
 * Contents of a conversion descriptor.
 */
struct conv_struct {
  /* Input (conversion multibyte -> unicode) */
  struct mbtowc_funcs ifuncs;
  state_t istate;
  /* Output (conversion unicode -> multibyte) */
  struct wctomb_funcs ofuncs;
  state_t ostate;
  /* Operation flags */
  int transliterate;
};

/*
 * Include all the converters.
 */

#include "ascii.h"

/* General multi-byte encodings */
#include "utf8.h"
#include "ucs2.h"
#include "ucs4.h"
#include "utf16.h"
#include "utf7.h"
#include "ucs2internal.h"
#include "ucs2swapped.h"
#include "ucs4internal.h"
#include "ucs4swapped.h"
#include "java.h"

/* 8-bit encodings */
#include "iso8859_1.h"
#include "iso8859_2.h"
#include "iso8859_3.h"
#include "iso8859_4.h"
#include "iso8859_5.h"
#include "iso8859_6.h"
#include "iso8859_7.h"
#include "iso8859_8.h"
#include "iso8859_9.h"
#include "iso8859_10.h"
#include "iso8859_13.h"
#include "iso8859_14.h"
#include "iso8859_15.h"
#include "koi8_r.h"
#include "koi8_u.h"
#include "koi8_ru.h"
#include "cp1250.h"
#include "cp1251.h"
#include "cp1252.h"
#include "cp1253.h"
#include "cp1254.h"
#include "cp1255.h"
#include "cp1256.h"
#include "cp1257.h"
#include "cp1258.h"
#include "cp850.h"
#include "cp866.h"
#include "mac_roman.h"
#include "mac_centraleurope.h"
#include "mac_iceland.h"
#include "mac_croatian.h"
#include "mac_romania.h"
#include "mac_cyrillic.h"
#include "mac_ukraine.h"
#include "mac_greek.h"
#include "mac_turkish.h"
#include "mac_hebrew.h"
#include "mac_arabic.h"
#include "mac_thai.h"
#include "hp_roman8.h"
#include "nextstep.h"
#include "armscii_8.h"
#include "georgian_academy.h"
#include "georgian_ps.h"
#include "mulelao.h"
#include "cp1133.h"
#include "tis620.h"
#include "cp874.h"
#include "viscii.h"
#include "tcvn.h"

/* CJK character sets [CCS = coded character set] [CJKV.INF chapter 3] */

typedef struct {
  unsigned short indx; /* index into big table */
  unsigned short used; /* bitmask of used entries */
} Summary16;

#include "jisx0201.h"
#include "jisx0208.h"
#include "jisx0212.h"

#include "gb2312.h"
/*#include "gb12345.h"*/
#include "gbk.h"
#include "cns11643.h"
#include "big5.h"

#include "ksc5601.h"
#include "johab_hangul.h"
#include "johab_decomp.h"

/* CJK encodings [CES = character encoding scheme] [CJKV.INF chapter 4] */

#include "euc_jp.h"
#include "sjis.h"
#include "cp932.h"
#include "iso2022_jp.h"
#include "iso2022_jp1.h"
#include "iso2022_jp2.h"

#include "euc_cn.h"
#include "ces_gbk.h"
#include "iso2022_cn.h"
#include "iso2022_cnext.h"
#include "hz.h"
#include "euc_tw.h"
#include "ces_big5.h"
#include "cp950.h"

#include "euc_kr.h"
#include "johab.h"
#include "iso2022_kr.h"

/*
 * Table of all supported encodings.
 */
struct encoding {
  struct mbtowc_funcs ifuncs; /* conversion multibyte -> unicode */
  struct wctomb_funcs ofuncs; /* conversion unicode -> multibyte */
};
enum {
#define DEFENCODING(xxx_names,xxx,xxx_ifuncs,xxx_ofuncs1,xxx_ofuncs2) \
  ei_##xxx ,
#include "encodings.def"
#undef DEFENCODING
ei_for_broken_compilers_that_dont_like_trailing_commas
};
static struct encoding const all_encodings[] = {
#define DEFENCODING(xxx_names,xxx,xxx_ifuncs,xxx_ofuncs1,xxx_ofuncs2) \
  { xxx_ifuncs, xxx_ofuncs1,xxx_ofuncs2 },
#include "encodings.def"
#undef DEFENCODING
};

/*
 * Alias lookup function.
 * Defines
 *   struct alias { const char* name; unsigned int encoding_index; };
 *   const struct alias * aliases_lookup (const char *str, unsigned int len);
 *   #define MAX_WORD_LENGTH ...
 */
#include "aliases.h"

#if 0
/* Like !strcasecmp, except that the both strings can be assumed to be ASCII
   and the first string can be assumed to be in uppercase. */
static int strequal (const char* str1, const char* str2)
{
  unsigned char c1;
  unsigned char c2;
  for (;;) {
    c1 = * (unsigned char *) str1++;
    c2 = * (unsigned char *) str2++;
    if (c1 == 0)
      break;
    if (c2 >= 'a' && c2 <= 'z')
      c2 -= 'a'-'A';
    if (c1 != c2)
      break;
  }
  return (c1 == c2);
}
#endif

iconv_t iconv_open (const char* tocode, const char* fromcode)
{
  struct conv_struct * cd = (struct conv_struct *) malloc(sizeof(struct conv_struct));
  char buf[MAX_WORD_LENGTH+1];
  const char* cp;
  char* bp;
  const struct alias * ap;
  unsigned int count;

  if (cd == NULL) {
    errno = ENOMEM;
    return (iconv_t)(-1);
  }
  /* Before calling aliases_lookup, convert the input string to upper case,
   * and check whether it's entirely ASCII (we call gperf with option "-7"
   * to achieve a smaller table) and non-empty. If it's not entirely ASCII,
   * or if it's too long, it is not a valid encoding name.
   */
  /* Search tocode in the table. */
  for (cp = tocode, bp = buf, count = MAX_WORD_LENGTH+1; ; cp++, bp++) {
    unsigned char c = * (unsigned char *) cp;
    if (c >= 0x80)
      goto invalid;
    if (c >= 'a' && c <= 'z')
      c -= 'a'-'A';
    *bp = c;
    if (c == '\0')
      break;
    if (--count == 0)
      goto invalid;
  }
  ap = aliases_lookup(buf,bp-buf);
  if (ap == NULL)
    goto invalid;
  cd->ofuncs = all_encodings[ap->encoding_index].ofuncs;
  /* Search fromcode in the table. */
  for (cp = fromcode, bp = buf, count = MAX_WORD_LENGTH+1; ; cp++, bp++) {
    unsigned char c = * (unsigned char *) cp;
    if (c >= 0x80)
      goto invalid;
    if (c >= 'a' && c <= 'z')
      c -= 'a'-'A';
    *bp = c;
    if (c == '\0')
      break;
    if (--count == 0)
      goto invalid;
  }
  ap = aliases_lookup(buf,bp-buf);
  if (ap == NULL)
    goto invalid;
  cd->ifuncs = all_encodings[ap->encoding_index].ifuncs;
  /* Initialize the states. */
  memset(&cd->istate,'\0',sizeof(state_t));
  memset(&cd->ostate,'\0',sizeof(state_t));
  /* Initialize the operation flags. */
  cd->transliterate = 1;
  /* Done. */
  return (iconv_t)cd;
invalid:
  errno = EINVAL;
  return (iconv_t)(-1);
}

size_t iconv (iconv_t icd,
              const char* * inbuf, size_t *inbytesleft,
              char* * outbuf, size_t *outbytesleft)
{
  conv_t cd = (conv_t) icd;
  if (inbuf == NULL || *inbuf == NULL) {
    if (outbuf == NULL || *outbuf == NULL) {
      /* Reset the states. */
      memset(&cd->istate,'\0',sizeof(state_t));
      memset(&cd->ostate,'\0',sizeof(state_t));
      return 0;
    } else {
      if (cd->ofuncs.xxx_reset) {
        int outcount = cd->ofuncs.xxx_reset(cd,*outbuf,*outbytesleft);
        if (outcount < 0) {
          errno = E2BIG;
          return -1;
        }
        *outbuf += outcount; *outbytesleft -= outcount;
      }
      memset(&cd->istate,'\0',sizeof(state_t));
      memset(&cd->ostate,'\0',sizeof(state_t));
      return 0;
    }
  } else {
    size_t result = 0;
    const unsigned char* inptr = (const unsigned char*) *inbuf;
    size_t inleft = *inbytesleft;
    unsigned char* outptr = (unsigned char*) *outbuf;
    size_t outleft = *outbytesleft;
    while (inleft > 0) {
      wchar_t wc;
      int incount;
      int outcount;
      incount = cd->ifuncs.xxx_mbtowc(cd,&wc,inptr,inleft);
      if (incount <= 0) {
        if (incount == 0) {
          /* Case 1: invalid input */
          errno = EILSEQ;
          result = -1;
          break;
        }
        if (incount == -1) {
          /* Case 2: not enough bytes available to detect anything */
          errno = EINVAL;
          result = -1;
          break;
        }
        /* Case 3: k bytes read, but only a shift sequence */
        incount = -1-incount;
      } else {
        /* Case 4: k bytes read, making up a wide character */
        outcount = cd->ofuncs.xxx_wctomb(cd,outptr,wc,outleft);
        if (outcount == 0) {
          /* This is not optimal. Transliteration would be better. */
          outcount = cd->ofuncs.xxx_wctomb(cd,outptr,0xFFFD,outleft);
          if (outcount == 0) {
            errno = EILSEQ;
            result = -1;
            break;
          }
        }
        if (outcount < 0) {
          errno = E2BIG;
          result = -1;
          break;
        }
        if (!(outcount <= outleft)) abort();
        outptr += outcount; outleft -= outcount;
      }
      if (!(incount <= inleft)) abort();
      inptr += incount; inleft -= incount;
    }
    *inbuf = (const char*) inptr;
    *inbytesleft = inleft;
    *outbuf = (char*) outptr;
    *outbytesleft = outleft;
    return result;
  }
}

int iconv_close (iconv_t icd)
{
  conv_t cd = (conv_t) icd;
  free(cd);
  return 0;
}
