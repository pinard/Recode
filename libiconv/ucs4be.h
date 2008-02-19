/*
 * UCS-4BE = UCS-4 big endian
 */

static int
ucs4be_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  if (n >= 4) {
    *pwc = (s[0] << 24) + (s[1] << 16) + (s[2] << 8) + s[3];
    return 4;
  }
  return RET_TOOFEW(0);
}

static int
ucs4be_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (n >= 4) {
    r[0] = (unsigned char) (wc >> 24);
    r[1] = (unsigned char) (wc >> 16);
    r[2] = (unsigned char) (wc >> 8);
    r[3] = (unsigned char) wc;
    return 4;
  } else
    return RET_TOOSMALL;
}
