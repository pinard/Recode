/*
 * UCS-4-SWAPPED = UCS-4-INTERNAL with inverted endianness
 */

static int
ucs4swapped_mbtowc (conv_t conv, wchar_t *pwc, const unsigned char *s, int n)
{
  if (n >= 4) {
    unsigned int x = *(unsigned int *)s;
    x = ((x & 0xffffffff) >> 24) | ((x >> 8) & 0xff00) | ((x & 0xff00) << 8) | ((x << 24) & 0xffffffff);
    *pwc = x;
    return 4;
  }
  return RET_TOOFEW(0);
}

static int
ucs4swapped_wctomb (conv_t conv, unsigned char *r, wchar_t wc, int n)
{
  if (n >= 4) {
    unsigned int x = wc;
    x = ((x & 0xffffffff) >> 24) | ((x >> 8) & 0xff00) | ((x & 0xff00) << 8) | ((x << 24) & 0xffffffff);
    *(unsigned int *)r = x;
    return 4;
  } else
    return RET_TOOSMALL;
}
