/*
 * UCS-2-INTERNAL = UCS-2 with machine dependent endianness and alignment
 */

static int
ucs2internal_mbtowc (conv_t conv, wchar_t *pwc, const unsigned char *s, int n)
{
  if (n >= 2) {
    *pwc = *(unsigned short *)s;
    return 2;
  }
  return RET_TOOFEW(0);
}

static int
ucs2internal_wctomb (conv_t conv, unsigned char *r, wchar_t wc, int n)
{
  if (wc < 0x10000) {
    if (n >= 2) {
      *(unsigned short *)r = wc;
      return 2;
    } else
      return RET_TOOSMALL;
  } else
    return RET_ILSEQ;
}
