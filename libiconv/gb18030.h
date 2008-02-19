/*
 * GB18030
 */

/*
 * GB18030, as implemented in glibc-2.2, is an extension of GBK (= CP936).
 * It adds the following ranges:
 * 1. Two-byte range
 *    0xA2E3, 0xA8BF, 0xA98A..0xA995, 0xFE50..0xFE9F
 * 2. Four-byte range
 *    0x{81..84}{30..39}{81..FE}{30..39}
 *    Most of Unicode plane 1 in Unicode order.
 */

#include "gb18030ext.h"
#include "gb18030uni.h"

static int
gb18030_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  int ret;

  /* Code set 0 (ASCII) */
  if (*s < 0x80)
    return ascii_mbtowc(conv,pwc,s,n);

  /* Code set 1 (GBK extended) */
  ret = gbk_mbtowc(conv,pwc,s,n);
  if (ret != RET_ILSEQ)
    return ret;

  ret = gb18030ext_mbtowc(conv,pwc,s,n);
  if (ret != RET_ILSEQ)
    return ret;

  /* Code set 2 (remainder of Unicode) */
  return gb18030uni_mbtowc(conv,pwc,s,n);
}

static int
gb18030_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  int ret;

  /* Code set 0 (ASCII) */
  ret = ascii_wctomb(conv,r,wc,n);
  if (ret != RET_ILSEQ)
    return ret;

  /* Code set 1 (GBK extended) */
  ret = gbk_wctomb(conv,r,wc,n);
  if (ret != RET_ILSEQ)
    return ret;

  ret = gb18030ext_wctomb(conv,r,wc,n);
  if (ret != RET_ILSEQ)
    return ret;

  /* Code set 2 (remainder of Unicode) */
  return gb18030uni_wctomb(conv,r,wc,n);
}
