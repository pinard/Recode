
/*
 * EUC-TW
 */

static int
euc_tw_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  unsigned char c = *s;
  /* Code set 0 (ASCII) */
  if (c < 0x80)
    return ascii_mbtowc(conv,pwc,s,n);
  /* Code set 1 (CNS 11643-1992 Plane 1) */
  if (c >= 0xa1 && c < 0xff) {
    if (n < 2)
      return RET_TOOFEW(0);
    {
      unsigned char c2 = s[1];
      if (c2 >= 0xa1 && c2 < 0xff) {
        unsigned char buf[2];
        buf[0] = c-0x80; buf[1] = c2-0x80;
        return cns11643_1_mbtowc(conv,pwc,buf,2);
      } else
        return RET_ILSEQ;
    }
  }
  /* Code set 2 (CNS 11643-1992 Planes 1-16) */
  if (c == 0x8e) {
    if (n < 4)
      return RET_TOOFEW(0);
    {
      unsigned char c2 = s[1];
      if (c2 >= 0xa1 && c2 <= 0xb0) {
        unsigned char c3 = s[2];
        unsigned char c4 = s[3];
        if (c3 >= 0xa1 && c3 < 0xff && c4 >= 0xa1 && c4 < 0xff) {
          unsigned char buf[2];
          int ret;
          buf[0] = c3-0x80; buf[1] = c4-0x80;
          switch (c2-0xa0) {
            case 1: ret = cns11643_1_mbtowc(conv,pwc,buf,2); break;
            case 2: ret = cns11643_2_mbtowc(conv,pwc,buf,2); break;
            case 3: ret = cns11643_3_mbtowc(conv,pwc,buf,2); break;
            default: return RET_ILSEQ;
          }
          if (ret == RET_ILSEQ)
            return RET_ILSEQ;
          if (ret != 2) abort();
          return 4;
        }
      }
    }
  }
  return RET_ILSEQ;
}

static int
euc_tw_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  unsigned char buf[3];
  int ret;

  /* Code set 0 (ASCII) */
  ret = ascii_wctomb(conv,r,wc,n);
  if (ret != RET_ILSEQ)
    return ret;

  ret = cns11643_wctomb(conv,buf,wc,3);
  if (ret != RET_ILSEQ) {
    if (ret != 3) abort();

    /* Code set 1 (CNS 11643-1992 Plane 1) */
    if (buf[0] == 0) {
      if (n < 2)
        return RET_TOOSMALL;
      r[0] = buf[1]+0x80;
      r[1] = buf[2]+0x80;
      return 2;
    }

    /* Code set 2 (CNS 11643-1992 Planes 1-16) */
    if (n < 4)
      return RET_TOOSMALL;
    r[0] = 0x8e;
    r[1] = buf[0]+0xa1;
    r[2] = buf[1]+0x80;
    r[3] = buf[2]+0x80;
    return 4;
  }

  return RET_ILSEQ;
}
