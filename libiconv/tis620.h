
/*
 * TIS620.2533-1
 */

static int
tis620_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  unsigned char c = *s;
  if (c < 0x80) {
    *pwc = (ucs4_t) c;
    return 1;
  }
  else if (c >= 0xa1 && c <= 0xfb && !(c >= 0xdb && c <= 0xde)) {
    *pwc = (ucs4_t) (c + 0x0d60);
    return 1;
  }
  return RET_ILSEQ;
}

static int
tis620_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (wc < 0x0080) {
    *r = wc;
    return 1;
  }
  else if (wc >= 0x0e01 && wc <= 0x0e5b && !(wc >= 0x0e3b && wc <= 0x0e3e)) {
    *r = wc-0x0d60;
    return 1;
  }
  return RET_ILSEQ;
}
