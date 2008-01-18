
/*
 * ISO-8859-1
 */

static int
iso8859_1_mbtowc (conv_t conv, wchar_t *pwc, const unsigned char *s, int n)
{
  unsigned char c = *s;
  *pwc = (wchar_t) c;
  return 1;
}

static int
iso8859_1_wctomb (conv_t conv, unsigned char *r, wchar_t wc, int n)
{
  if (wc < 0x0100) {
    *r = wc;
    return 1;
  }
  return RET_ILSEQ;
}
