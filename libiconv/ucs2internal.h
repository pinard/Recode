/*
 * UCS-2-INTERNAL = UCS-2 with machine dependent endianness and alignment
 */

static int
ucs2internal_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  if (n >= 2) {
    unsigned short x = *(const unsigned short *)s;
    if (x >= 0xd800 && x < 0xe000) {
      return RET_ILSEQ;
    } else {
      *pwc = x;
      return 2;
    }
  }
  return RET_TOOFEW(0);
}

static int
ucs2internal_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (wc < 0x10000 && !(wc >= 0xd800 && wc < 0xe000)) {
    if (n >= 2) {
      *(unsigned short *)r = wc;
      return 2;
    } else
      return RET_TOOSMALL;
  } else
    return RET_ILSEQ;
}
