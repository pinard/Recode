
/*
 * ISO646-CN
 * also known as GB_1988-80
 */

static int
iso646_cn_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  unsigned char c = *s;
  if (c < 0x80) {
    if (c == 0x24)
      *pwc = (ucs4_t) 0x00a5;
    else if (c == 0x7e)
      *pwc = (ucs4_t) 0x203e;
    else
      *pwc = (ucs4_t) c;
    return 1;
  }
  return RET_ILSEQ;
}

static int
iso646_cn_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (wc < 0x0080 && !(wc == 0x0024 || wc == 0x007e)) {
    *r = wc;
    return 1;
  }
  if (wc == 0x00a5) {
    *r = 0x24;
    return 1;
  }
  if (wc == 0x203e) {
    *r = 0x7e;
    return 1;
  }
  return RET_ILSEQ;
}
