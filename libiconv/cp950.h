
/*
 * CP950
 */

/*
 * Microsoft CP950 is a slightly extended and slightly modified version of
 * BIG5. The differences between the EASTASIA/OTHER/BIG5.TXT and
 * VENDORS/MICSFT/WINDOWS/CP950.TXT tables found on ftp.unicode.org are
 * as follows:
 *
 * 1. Some characters in the BIG5 range are defined differently:
 *
 *     code   BIG5.TXT                       CP950.TXT
 *    0xA145  0x2022 # BULLET                0x2027 # HYPHENATION POINT
 *    0xA14E  0xFF64 # HALFWIDTH IDEOGRAPHIC COMMA
 *                                           0xFE51 # SMALL IDEOGRAPHIC COMMA
 *    0xA15A    ---                          0x2574 # BOX DRAWINGS LIGHT LEFT
 *    0xA1C2  0x203E # OVERLINE              0x00AF # MACRON
 *    0xA1C3    ---                          0xFFE3 # FULLWIDTH MACRON
 *    0xA1C5    ---                          0x02CD # MODIFIER LETTER LOW MACRON
 *    0xA1E3  0x223C # TILDE OPERATOR        0xFF5E # FULLWIDTH TILDE
 *    0xA1F2  0x2641 # EARTH                 0x2295 # CIRCLED PLUS
 *    0xA1F3  0x2609 # SUN                   0x2299 # CIRCLED DOT OPERATOR
 *    0xA1FE    ---                          0xFF0F # FULLWIDTH SOLIDUS
 *    0xA240    ---                          0xFF3C # FULLWIDTH REVERSE SOLIDUS
 *    0xA241  0xFF0F # FULLWIDTH SOLIDUS     0x2215 # DIVISION SLASH
 *    0xA242  0xFF3C # FULLWIDTH REVERSE SOLIDUS
 *                                           0xFE68 # SMALL REVERSE SOLIDUS
 *    0xA244  0x00A5 # YEN SIGN              0xFFE5 # FULLWIDTH YEN SIGN
 *    0xA246  0x00A2 # CENT SIGN             0xFFE0 # FULLWIDTH CENT SIGN
 *    0xA247  0x00A3 # POUND SIGN            0xFFE1 # FULLWIDTH POUND SIGN
 *    0xA2CC    ---                          0x5341
 *    0xA2CE    ---                          0x5345
 *
 *    We don't implement these changes.
 *
 * 2. A small new row. See cp950ext.h.
 *
 * 3. CP950.TXT is lacking the range 0xC6A1..0xC7FC (Hiragana, Katakana,
 *    Cyrillic, circled digits, parenthesized digits).
 *
 *    We implement this omission, because said range is marked "uncertain"
 *    in the unicode.org BIG5 table.
 */

#include "cp950ext.h"

static int
cp950_mbtowc (conv_t conv, wchar_t *pwc, const unsigned char *s, int n)
{
  unsigned char c = *s;
  /* Code set 0 (ASCII) */
  if (c < 0x80)
    return ascii_mbtowc(conv,pwc,s,n);
  /* Code set 1 (BIG5 extended) */
  if (c >= 0xa1 && c < 0xff) {
    if (n < 2)
      return RET_TOOFEW(0);
    {
      unsigned char c2 = s[1];
      if ((c2 >= 0x40 && c2 < 0x7f) || (c2 >= 0xa1 && c2 < 0xff)) {
        if (!((c == 0xc6 && c2 >= 0xa1) || c == 0xc7)) {
          int ret = big5_mbtowc(conv,pwc,s,2);
          if (ret != RET_ILSEQ)
            return ret;
        }
      }
    }
    if (c == 0xf9) {
      int ret = cp950ext_mbtowc(conv,pwc,s,2);
      if (ret != RET_ILSEQ)
        return ret;
    }
  }
  return RET_ILSEQ;
}

static int
cp950_wctomb (conv_t conv, unsigned char *r, wchar_t wc, int n)
{
  unsigned char buf[2];
  int ret;

  /* Code set 0 (ASCII) */
  ret = ascii_wctomb(conv,r,wc,n);
  if (ret != RET_ILSEQ)
    return ret;

  /* Code set 1 (BIG5 extended) */
  ret = big5_wctomb(conv,buf,wc,2);
  if (ret != RET_ILSEQ) {
    if (ret != 2) abort();
    if (!((buf[0] == 0xc6 && buf[1] >= 0xa1) || buf[0] == 0xc7)) {
      if (n < 2)
        return RET_TOOSMALL;
      r[0] = buf[0];
      r[1] = buf[1];
      return 2;
    }
  }
  ret = cp950ext_wctomb(conv,buf,wc,2);
  if (ret != RET_ILSEQ) {
    if (ret != 2) abort();
    if (n < 2)
      return RET_TOOSMALL;
    r[0] = buf[0];
    r[1] = buf[1];
    return 2;
  }

  return RET_ILSEQ;
}
