/*
 * UTF-16BE
 */

/* Specification: RFC 2781 */

static int
utf16be_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  int count = 0;
  if (n >= 2) {
    ucs4_t wc = (s[0] << 8) + s[1];
    if (wc >= 0xd800 && wc < 0xdc00) {
      if (n >= 4) {
        ucs4_t wc2 = (s[2] << 8) + s[3];
        if (!(wc2 >= 0xdc00 && wc2 < 0xe000))
          return RET_ILSEQ;
        *pwc = 0x10000 + ((wc - 0xd800) << 10) + (wc2 - 0xdc00);
        return count+4;
      }
    } else if (wc >= 0xdc00 && wc < 0xe000) {
      return RET_ILSEQ;
    } else {
      *pwc = wc;
      return count+2;
    }
  }
  return RET_TOOFEW(count);
}

static int
utf16be_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (!(wc >= 0xd800 && wc < 0xe000)) {
    if (wc < 0x10000) {
      if (n >= 2) {
        r[0] = (unsigned char) (wc >> 8);
        r[1] = (unsigned char) wc;
        return 2;
      } else
        return RET_TOOSMALL;
    }
    else if (wc < 0x110000) {
      if (n >= 4) {
        ucs4_t wc1 = 0xd800 + ((wc - 0x10000) >> 10);
        ucs4_t wc2 = 0xdc00 + ((wc - 0x10000) & 0x3ff);
        r[0] = (unsigned char) (wc1 >> 8);
        r[1] = (unsigned char) wc1;
        r[2] = (unsigned char) (wc2 >> 8);
        r[3] = (unsigned char) wc2;
        return 4;
      } else
        return RET_TOOSMALL;
    }
  }
  return RET_ILSEQ;
}
