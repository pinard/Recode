
/*
 * JISX0201.1976-0
 */

static int
jisx0201_mbtowc (conv_t conv, wchar_t *pwc, const unsigned char *s, int n)
{
  unsigned char c = *s;
  if (c < 0x80) {
    if (c == 0x5c)
      *pwc = (wchar_t) 0x00a5;
    else if (c == 0x7e)
      *pwc = (wchar_t) 0x203e;
    else
      *pwc = (wchar_t) c;
    return 1;
  } else {
    if (c >= 0xa1 && c < 0xe0) {
      *pwc = (wchar_t) c + 0xfec0;
      return 1;
    }
  }
  return RET_ILSEQ;
}

static int
jisx0201_wctomb (conv_t conv, unsigned char *r, wchar_t wc, int n)
{
  if (wc < 0x0080 && !(wc == 0x005c || wc == 0x007e)) {
    *r = wc;
    return 1;
  }
  if (wc == 0x00a5) {
    *r = 0x5c;
    return 1;
  }
  if (wc == 0x203e) {
    *r = 0x7e;
    return 1;
  }
  if (wc >= 0xff61 && wc < 0xffa0) {
    *r = wc - 0xfec0;
    return 1;
  }
  return RET_ILSEQ;
}
