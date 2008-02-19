/*
 * UCS-4LE = UCS-4 little endian
 */

static int
ucs4le_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  if (n >= 4) {
    *pwc = s[0] + (s[1] << 8) + (s[2] << 16) + (s[3] << 24);
    return 4;
  }
  return RET_TOOFEW(0);
}

static int
ucs4le_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (n >= 4) {
    r[0] = (unsigned char) wc;
    r[1] = (unsigned char) (wc >> 8);
    r[2] = (unsigned char) (wc >> 16);
    r[3] = (unsigned char) (wc >> 24);
    return 4;
  } else
    return RET_TOOSMALL;
}
