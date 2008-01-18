
/*
 * ASCII
 */

static int
ascii_mbtowc (conv_t conv, wchar_t *pwc, const unsigned char *s, int n)
{
  unsigned char c = *s;
  if (c < 0x80) {
    *pwc = (wchar_t) c;
    return 1;
  }
  return RET_ILSEQ;
}

static int
ascii_wctomb (conv_t conv, unsigned char *r, wchar_t wc, int n)
{
  if (wc < 0x0080) {
    *r = wc;
    return 1;
  }
  return RET_ILSEQ;
}
