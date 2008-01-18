/*
 * UCS-2-SWAPPED = UCS-2-INTERNAL with inverted endianness
 */

static int
ucs2swapped_mbtowc (conv_t conv, wchar_t *pwc, const unsigned char *s, int n)
{
  if (n >= 2) {
    unsigned short x = *(unsigned short *)s;
    x = (x >> 8) | (x << 8);
    *pwc = x;
    return 2;
  }
  return RET_TOOFEW(0);
}

static int
ucs2swapped_wctomb (conv_t conv, unsigned char *r, wchar_t wc, int n)
{
  if (wc < 0x10000) {
    if (n >= 2) {
      unsigned short x = wc;
      x = (x >> 8) | (x << 8);
      *(unsigned short *)r = x;
      return 2;
    } else
      return RET_TOOSMALL;
  } else
    return RET_ILSEQ;
}
