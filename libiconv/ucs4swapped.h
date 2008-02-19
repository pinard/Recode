/*
 * UCS-4-SWAPPED = UCS-4-INTERNAL with inverted endianness
 */

static int
ucs4swapped_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  /* This function assumes that 'unsigned int' has exactly 32 bits. */
  if (sizeof(unsigned int) != 4) abort();

  if (n >= 4) {
    unsigned int x = *(const unsigned int *)s;
    x = (x >> 24) | ((x >> 8) & 0xff00) | ((x & 0xff00) << 8) | (x << 24);
    *pwc = x;
    return 4;
  }
  return RET_TOOFEW(0);
}

static int
ucs4swapped_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  /* This function assumes that 'unsigned int' has exactly 32 bits. */
  if (sizeof(unsigned int) != 4) abort();

  if (n >= 4) {
    unsigned int x = wc;
    x = (x >> 24) | ((x >> 8) & 0xff00) | ((x & 0xff00) << 8) | (x << 24);
    *(unsigned int *)r = x;
    return 4;
  } else
    return RET_TOOSMALL;
}
