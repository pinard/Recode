/*
 * Decomposition of JOHAB Hangul in one to three Johab Jamo elements.
 *
 * See Ken Lunde's "CJKV Information Processing", p. 181-184.
 */

/* Note that Jamo XX = UHC 0xA4A0+XX = Unicode 0x3130+XX */
#define NONE 0xfd
#define FILL 0xff
static unsigned char jamo_initial[32] = {
  NONE, FILL, 0x01, 0x02, 0x04, 0x07, 0x08, 0x09,
  0x11, 0x12, 0x13, 0x15, 0x16, 0x17, 0x18, 0x19,
  0x1a, 0x1b, 0x1c, 0x1d, 0x1e, NONE, NONE, NONE,
  NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE,
};
static unsigned char jamo_medial[32] = {
  NONE, NONE, FILL, 0x1f, 0x20, 0x21, 0x22, 0x23,
  NONE, NONE, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
  NONE, NONE, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  NONE, NONE, 0x30, 0x31, 0x32, 0x33, NONE, NONE,
};
static unsigned char jamo_final[32] = {
  NONE, FILL, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
  0x07, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, NONE, 0x12, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, NONE, NONE,
};

/* Decompose wc into r[0..2], and return the number of resulting Jamo elements.
   Return RET_ILSEQ if decomposition is not possible. */

static int johab_hangul_decompose (conv_t conv, unsigned char* r, wchar_t wc)
{
  unsigned char buf[2];
  int ret = johab_hangul_wctomb(conv,buf,wc,2);
  if (ret != RET_ILSEQ) {
    unsigned int hangul = (buf[0] << 8) | buf[1];
    unsigned char jamo1 = jamo_initial[(hangul >> 10) & 31];
    unsigned char jamo2 = jamo_medial[(hangul >> 5) & 31];
    unsigned char jamo3 = jamo_final[hangul & 31];
    if ((hangul >> 15) != 1) abort();
    if (jamo1 != NONE && jamo2 != NONE && jamo3 != NONE) {
      /* They are not all three == FILL because that would correspond to
         hangul = 0x8441, but that is unmapped in the johab_hangul table. */
      unsigned char* p = r;
      if (jamo1 != FILL)
        *p++ = jamo1;
      if (jamo2 != FILL)
        *p++ = jamo2;
      if (jamo3 != FILL)
        *p++ = jamo3;
      return p-r;
    }
  }
  return RET_ILSEQ;
}

#undef FILL
#undef NONE
