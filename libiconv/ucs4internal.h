/*
 * UCS-4-INTERNAL = UCS-4 with machine dependent endianness and alignment
 */

static int
ucs4internal_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  if (n >= 4) {
    *pwc = *(const unsigned int *)s;
    return 4;
  }
  return RET_TOOFEW(0);
}

static int
ucs4internal_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (n >= 4) {
    *(unsigned int *)r = wc;
    return 4;
  } else
    return RET_TOOSMALL;
}
