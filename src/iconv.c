/* Copyright (C) 1999-2000 Free Software Foundation, Inc.
   This file is part of the GNU ICONV Library.

   The GNU ICONV Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU ICONV Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU ICONV Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "common.h"
#include "iconv.h"

#if 0

/*
 * Consider those system dependent encodings that are needed for the
 * current system.
 */
#ifdef _AIX
#define USE_AIX
#endif

#endif

/*
 * Converters.
 */
#include "converters.h"

/*
 * Transliteration tables.
 */
#include "cjk_variants.h"
#include "translit.h"

/*
 * Table of all supported encodings.
 */
struct encoding {
  struct mbtowc_funcs ifuncs; /* conversion multibyte -> unicode */
  struct wctomb_funcs ofuncs; /* conversion unicode -> multibyte */
  int oflags;                 /* flags for unicode -> multibyte conversion */
};
enum {
#define DEFENCODING(xxx_names,xxx,xxx_ifuncs,xxx_ofuncs1,xxx_ofuncs2) \
  ei_##xxx ,
#include "encodings.def"
#ifdef USE_AIX
#include "encodings_aix.def"
#endif
#undef DEFENCODING
ei_for_broken_compilers_that_dont_like_trailing_commas
};
#include "flags.h"
static struct encoding const all_encodings[] = {
#define DEFENCODING(xxx_names,xxx,xxx_ifuncs,xxx_ofuncs1,xxx_ofuncs2) \
  { xxx_ifuncs, xxx_ofuncs1,xxx_ofuncs2, ei_##xxx##_oflags },
#include "encodings.def"
#ifdef USE_AIX
#include "encodings_aix.def"
#endif
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

/*
 * System dependent alias lookup function.
 * Defines
 *   const struct alias * aliases2_lookup (const char *str);
 */
#if defined(USE_AIX) /* || ... */
static struct alias sysdep_aliases[] = {
#ifdef USE_AIX
#include "aliases_aix.h"
#endif
};
#ifdef __GNUC__
__inline
#endif
const struct alias *
aliases2_lookup (register const char *str)
{
  struct alias * ptr;
  unsigned int count;
  for (ptr = sysdep_aliases, count = sizeof(sysdep_aliases)/sizeof(sysdep_aliases[0]); count > 0; ptr++, count--)
    if (!strcmp(str,ptr->name))
      return ptr;
  return NULL;
}
#else
#define aliases2_lookup(str)  NULL
#endif

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
  if (ap == NULL) {
    ap = aliases2_lookup(buf);
    if (ap == NULL)
      goto invalid;
  }
  cd->oindex = ap->encoding_index;
  cd->ofuncs = all_encodings[ap->encoding_index].ofuncs;
  cd->oflags = all_encodings[ap->encoding_index].oflags;
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
  if (ap == NULL) {
    ap = aliases2_lookup(buf);
    if (ap == NULL)
      goto invalid;
  }
  cd->iindex = ap->encoding_index;
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
        int outcount =
          cd->ofuncs.xxx_reset(cd, (unsigned char *) *outbuf, *outbytesleft);
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
        if (outleft == 0) {
          errno = E2BIG;
          result = -1;
          break;
        }
        outcount = cd->ofuncs.xxx_wctomb(cd,outptr,wc,outleft);
        if (outcount != 0)
          goto outcount_ok;
        /* Try transliteration. */
        result++;
        if (cd->transliterate) {
          if (cd->oflags & HAVE_HANGUL_JAMO) {
            /* Decompose Hangul into Jamo. Use double-width Jamo (contained
               in all Korean encodings and ISO-2022-JP-2), not half-width Jamo
               (contained in Unicode only). */
            wchar_t buf[3];
            int ret = johab_hangul_decompose(cd,buf,wc);
            if (ret != RET_ILSEQ) {
              /* we know 1 <= ret <= 3 */
              state_t backup_state = cd->ostate;
              unsigned char* backup_outptr = outptr;
              size_t backup_outleft = outleft;
              int i, sub_outcount;
              for (i = 0; i < ret; i++) {
                if (outleft == 0) {
                  sub_outcount = RET_TOOSMALL;
                  goto johab_hangul_failed;
                }
                sub_outcount = cd->ofuncs.xxx_wctomb(cd,outptr,buf[i],outleft);
                if (sub_outcount <= 0)
                  goto johab_hangul_failed;
                if (!(sub_outcount <= outleft)) abort();
                outptr += sub_outcount; outleft -= sub_outcount;
              }
              goto char_done;
            johab_hangul_failed:
              cd->ostate = backup_state;
              outptr = backup_outptr;
              outleft = backup_outleft;
              if (sub_outcount < 0) {
                errno = E2BIG;
                result = -1;
                break;
              }
            }
          }
          {
            /* Try to use a variant, but postfix it with
               U+303E IDEOGRAPHIC VARIATION INDICATOR
               (cf. Ken Lunde's "CJKV information processing", p. 188). */
            int indx = -1;
            if (wc == 0x3006)
              indx = 0;
            else if (wc == 0x30f6)
              indx = 1;
            else if (wc >= 0x4e00 && wc < 0xa000)
              indx = cjk_variants_indx[wc-0x4e00];
            if (indx >= 0) {
              for (;; indx++) {
                wchar_t buf[2];
                unsigned short variant = cjk_variants[indx];
                unsigned short last = variant & 0x8000;
                variant &= 0x7fff;
                variant += 0x3000;
                buf[0] = variant; buf[1] = 0x303e;
                {
                  state_t backup_state = cd->ostate;
                  unsigned char* backup_outptr = outptr;
                  size_t backup_outleft = outleft;
                  int i, sub_outcount;
                  for (i = 0; i < 2; i++) {
                    if (outleft == 0) {
                      sub_outcount = RET_TOOSMALL;
                      goto variant_failed;
                    }
                    sub_outcount = cd->ofuncs.xxx_wctomb(cd,outptr,buf[i],outleft);
                    if (sub_outcount <= 0)
                      goto variant_failed;
                    if (!(sub_outcount <= outleft)) abort();
                    outptr += sub_outcount; outleft -= sub_outcount;
                  }
                  goto char_done;
                variant_failed:
                  cd->ostate = backup_state;
                  outptr = backup_outptr;
                  outleft = backup_outleft;
                  if (sub_outcount < 0) {
                    errno = E2BIG;
                    result = -1;
                    break;
                  }
                }
                if (last)
                  break;
              }
            }
          }
          if (wc >= 0x2018 && wc <= 0x201a) {
            /* Special case for quotation marks 0x2018, 0x2019, 0x201a */
            wchar_t substitute =
              (cd->oflags & HAVE_QUOTATION_MARKS
               ? (wc == 0x201a ? 0x2018 : wc)
               : (cd->oflags & HAVE_ACCENTS
                  ? (wc==0x2019 ? 0x00b4 : 0x0060) /* use accents */
                  : 0x0027 /* use apostrophe */
              )  );
            outcount = cd->ofuncs.xxx_wctomb(cd,outptr,substitute,outleft);
            if (outcount != 0)
              goto outcount_ok;
          }
          {
            /* Use the transliteration table. */
            int indx = translit_index(wc);
            if (indx >= 0) {
              const unsigned char * cp = &translit_data[indx];
              unsigned int num = *cp++;
              state_t backup_state = cd->ostate;
              unsigned char* backup_outptr = outptr;
              size_t backup_outleft = outleft;
              unsigned int i;
              int sub_outcount;
              for (i = 0; i < num; i++) {
                if (outleft == 0) {
                  sub_outcount = RET_TOOSMALL;
                  goto translit_failed;
                }
                sub_outcount = cd->ofuncs.xxx_wctomb(cd,outptr,cp[i],outleft);
                if (sub_outcount <= 0)
                  goto translit_failed;
                if (!(sub_outcount <= outleft)) abort();
                outptr += sub_outcount; outleft -= sub_outcount;
              }
              goto char_done;
            translit_failed:
              cd->ostate = backup_state;
              outptr = backup_outptr;
              outleft = backup_outleft;
              if (sub_outcount < 0) {
                errno = E2BIG;
                result = -1;
                break;
              }
            }
          }
        }
        outcount = cd->ofuncs.xxx_wctomb(cd,outptr,0xFFFD,outleft);
        if (outcount != 0)
          goto outcount_ok;
        errno = EILSEQ;
        result = -1;
        break;
      outcount_ok:
        if (outcount < 0) {
          errno = E2BIG;
          result = -1;
          break;
        }
        if (!(outcount <= outleft)) abort();
        outptr += outcount; outleft -= outcount;
      char_done:
        ;
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

#ifndef LIBICONV_PLUG

int iconvctl (iconv_t icd, int request, void* argument)
{
  conv_t cd = (conv_t) icd;
  switch (request) {
    case ICONV_TRIVIALP:
      *(int *)argument = (cd->iindex == cd->oindex ? 1 : 0);
      return 0;
    case ICONV_GET_TRANSLITERATE:
      *(int *)argument = cd->transliterate;
      return 0;
    case ICONV_SET_TRANSLITERATE:
      cd->transliterate = (*(const int *)argument ? 1 : 0);
      return 0;
    default:
      errno = EINVAL;
      return -1;
  }
}

#endif
