/*
 * CNS 11643-1992
 */

/* ISO-2022-CN and EUC-TW use CNS 11643-1992 planes 1 to 7. But all tables we
 * have are for CNS 11643-1992 planes 1,2,3. We use a trick to keep the
 * Unicode -> CNS 11643 table as small as possible (see cns11643_inv.h).
 */

#include "cns11643_1.h"
#include "cns11643_2.h"
#include "cns11643_3.h"
#include "cns11643_inv.h"

/* Returns the plane number (0,1,2) in r[0], the two bytes in r[1],r[2]. */
static int
cns11643_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (n >= 3) {
    int ret = cns11643_inv_wctomb(conv,r+1,wc,2);
    if (ret == RET_ILSEQ)
      return RET_ILSEQ;
    if (ret != 2) abort();
    r[0] = ((r[1] & 0x80) >> 6) | ((r[2] & 0x80) >> 7);
    r[1] &= 0x7f;
    r[2] &= 0x7f;
    return 3;
  }
  return RET_TOOSMALL;
}
