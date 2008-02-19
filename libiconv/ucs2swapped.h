/*
 * UCS-2-SWAPPED = UCS-2-INTERNAL with inverted endianness
 */

static int
ucs2swapped_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  /* This function assumes that 'unsigned short' has exactly 16 bits. */
  if (sizeof(unsigned short) != 2) abort();

  if (n >= 2) {
    unsigned short x = *(const unsigned short *)s;
    x = (x >> 8) | (x << 8);
    if (x >= 0xd800 && x < 0xe000) {
      return RET_ILSEQ;
    } else {
      *pwc = x;
      return 2;
    }
  }
  return RET_TOOFEW(0);
}

static int
ucs2swapped_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  /* This function assumes that 'unsigned short' has exactly 16 bits. */
  if (sizeof(unsigned short) != 2) abort();

  if (wc < 0x10000 && !(wc >= 0xd800 && wc < 0xe000)) {
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
