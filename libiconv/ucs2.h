/*
 * UCS-2
 */

/* Here we accept FFFE/FEFF marks as endianness indicators everywhere
   in the stream, not just at the beginning. The default is big-endian. */
/* The state is 0 if big-endian, 1 if little-endian. */
static int
ucs2_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  state_t state = conv->istate;
  int count = 0;
  for (; n >= 2;) {
    ucs4_t wc = (state ? s[0] + (s[1] << 8) : (s[0] << 8) + s[1]);
    s += 2; n -= 2; count += 2;
    if (wc == 0xfeff) {
    } else if (wc == 0xfffe) {
      state ^= 1;
    } else if (wc >= 0xd800 && wc < 0xe000) {
      return RET_ILSEQ;
    } else {
      *pwc = wc;
      conv->istate = state;
      return count;
    }
  }
  conv->istate = state;
  return RET_TOOFEW(count);
}

/* But we output UCS-2 in big-endian order, without byte-order mark. */
/* RFC 2152 says:
   "ISO/IEC 10646-1:1993(E) specifies that when characters the UCS-2 form are
    serialized as octets, that the most significant octet appear first." */
static int
ucs2_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (wc < 0x10000 && wc != 0xfffe && !(wc >= 0xd800 && wc < 0xe000)) {
    if (n >= 2) {
      r[0] = (unsigned char) (wc >> 8);
      r[1] = (unsigned char) wc;
      return 2;
    } else
      return RET_TOOSMALL;
  } else
    return RET_ILSEQ;
}
