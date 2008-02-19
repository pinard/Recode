/*
 * UCS-4
 */

/* Here we accept 0000FFFE/0000FEFF marks as endianness indicators everywhere
   in the stream, not just at the beginning. The default is big-endian. */
/* The state is 0 if big-endian, 1 if little-endian. */
static int
ucs4_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  state_t state = conv->istate;
  int count = 0;
  for (; n >= 4;) {
    ucs4_t wc = (state
                  ? s[0] + (s[1] << 8) + (s[2] << 16) + (s[3] << 24)
                  : (s[0] << 24) + (s[1] << 16) + (s[2] << 8) + s[3]);
    s += 4; n -= 4; count += 4;
    if (wc == 0xfeff) {
    } else if (wc == 0xfffe) {
      state ^= 1;
    } else if (wc <= 0x7fffffff) {
      *pwc = wc;
      conv->istate = state;
      return count;
    } else
      return RET_ILSEQ;
  }
  conv->istate = state;
  return RET_TOOFEW(count);
}

/* But we output UCS-4 in big-endian order, without byte-order mark. */
static int
ucs4_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (wc != 0xfffe) {
    if (n >= 4) {
      r[0] = (unsigned char) (wc >> 24);
      r[1] = (unsigned char) (wc >> 16);
      r[2] = (unsigned char) (wc >> 8);
      r[3] = (unsigned char) wc;
      return 4;
    } else
      return RET_TOOSMALL;
  } else
    return RET_ILSEQ;
}
