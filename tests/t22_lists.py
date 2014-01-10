# -*- coding: utf-8 -*-
import common
from common import setup_module, teardown_module

class Test_decimal:

    # Decimal listing of a charset.

    expected = '''\
ISO-8859-1

  0 NU    16 DL    32 SP    48 0     64 At    80 P     96 '!   112 p
  1 SH    17 D1    33 !     49 1     65 A     81 Q     97 a    113 q
  2 SX    18 D2    34 "     50 2     66 B     82 R     98 b    114 r
  3 EX    19 D3    35 Nb    51 3     67 C     83 S     99 c    115 s
  4 ET    20 D4    36 DO    52 4     68 D     84 T    100 d    116 t
  5 EQ    21 NK    37 %     53 5     69 E     85 U    101 e    117 u
  6 AK    22 SY    38 &     54 6     70 F     86 V    102 f    118 v
  7 BL    23 EB    39 '     55 7     71 G     87 W    103 g    119 w
  8 BS    24 CN    40 (     56 8     72 H     88 X    104 h    120 x
  9 HT    25 EM    41 )     57 9     73 I     89 Y    105 i    121 y
 10 LF    26 SB    42 *     58 :     74 J     90 Z    106 j    122 z
 11 VT    27 EC    43 +     59 ;     75 K     91 <(   107 k    123 (!
 12 FF    28 FS    44 ,     60 <     76 L     92 //   108 l    124 !!
 13 CR    29 GS    45 -     61 =     77 M     93 )>   109 m    125 !)
 14 SO    30 RS    46 .     62 >     78 N     94 '>   110 n    126 '?
 15 SI    31 US    47 /     63 ?     79 O     95 _    111 o    127 DT

128 PA   144 DC   160 NS   176 DG   192 A!   208 D-   224 a!   240 d-
129 HO   145 P1   161 !I   177 +-   193 A'   209 N?   225 a'   241 n?
130 BH   146 P2   162 Ct   178 2S   194 A>   210 O!   226 a>   242 o!
131 NH   147 TS   163 Pd   179 3S   195 A?   211 O'   227 a?   243 o'
132 IN   148 CC   164 Cu   180 ''   196 A:   212 O>   228 a:   244 o>
133 NL   149 MW   165 Ye   181 My   197 AA   213 O?   229 aa   245 o?
134 SA   150 SG   166 BB   182 PI   198 AE   214 O:   230 ae   246 o:
135 ES   151 EG   167 SE   183 .M   199 C,   215 *X   231 c,   247 -:
136 HS   152 SS   168 ':   184 ',   200 E!   216 O/   232 e!   248 o/
137 HJ   153 GC   169 Co   185 1S   201 E'   217 U!   233 e'   249 u!
138 VS   154 SC   170 -a   186 -o   202 E>   218 U'   234 e>   250 u'
139 PD   155 CI   171 <<   187 >>   203 E:   219 U>   235 e:   251 u>
140 PU   156 ST   172 NO   188 14   204 I!   220 U:   236 i!   252 u:
141 RI   157 OC   173 --   189 12   205 I'   221 Y'   237 i'   253 y'
142 S2   158 PM   174 Rg   190 34   206 I>   222 TH   238 i>   254 th
143 S3   159 AC   175 'm   191 ?I   207 I:   223 ss   239 i:   255 y:
'''


    def test_1(self):
        print('*1')
        output = common.external_output('$R -ld latin-1')
        common.assert_or_diff(output, self.expected)

class Test_hexadecimal:

    # Hexadecimal listing of a charset.

    expected = '''\
ISO-8859-1

00 NU   10 DL   20 SP   30 0    40 At   50 P    60 '!   70 p
01 SH   11 D1   21 !    31 1    41 A    51 Q    61 a    71 q
02 SX   12 D2   22 "    32 2    42 B    52 R    62 b    72 r
03 EX   13 D3   23 Nb   33 3    43 C    53 S    63 c    73 s
04 ET   14 D4   24 DO   34 4    44 D    54 T    64 d    74 t
05 EQ   15 NK   25 %    35 5    45 E    55 U    65 e    75 u
06 AK   16 SY   26 &    36 6    46 F    56 V    66 f    76 v
07 BL   17 EB   27 '    37 7    47 G    57 W    67 g    77 w
08 BS   18 CN   28 (    38 8    48 H    58 X    68 h    78 x
09 HT   19 EM   29 )    39 9    49 I    59 Y    69 i    79 y
0a LF   1a SB   2a *    3a :    4a J    5a Z    6a j    7a z
0b VT   1b EC   2b +    3b ;    4b K    5b <(   6b k    7b (!
0c FF   1c FS   2c ,    3c <    4c L    5c //   6c l    7c !!
0d CR   1d GS   2d -    3d =    4d M    5d )>   6d m    7d !)
0e SO   1e RS   2e .    3e >    4e N    5e '>   6e n    7e '?
0f SI   1f US   2f /    3f ?    4f O    5f _    6f o    7f DT

80 PA   90 DC   a0 NS   b0 DG   c0 A!   d0 D-   e0 a!   f0 d-
81 HO   91 P1   a1 !I   b1 +-   c1 A'   d1 N?   e1 a'   f1 n?
82 BH   92 P2   a2 Ct   b2 2S   c2 A>   d2 O!   e2 a>   f2 o!
83 NH   93 TS   a3 Pd   b3 3S   c3 A?   d3 O'   e3 a?   f3 o'
84 IN   94 CC   a4 Cu   b4 ''   c4 A:   d4 O>   e4 a:   f4 o>
85 NL   95 MW   a5 Ye   b5 My   c5 AA   d5 O?   e5 aa   f5 o?
86 SA   96 SG   a6 BB   b6 PI   c6 AE   d6 O:   e6 ae   f6 o:
87 ES   97 EG   a7 SE   b7 .M   c7 C,   d7 *X   e7 c,   f7 -:
88 HS   98 SS   a8 ':   b8 ',   c8 E!   d8 O/   e8 e!   f8 o/
89 HJ   99 GC   a9 Co   b9 1S   c9 E'   d9 U!   e9 e'   f9 u!
8a VS   9a SC   aa -a   ba -o   ca E>   da U'   ea e>   fa u'
8b PD   9b CI   ab <<   bb >>   cb E:   db U>   eb e:   fb u>
8c PU   9c ST   ac NO   bc 14   cc I!   dc U:   ec i!   fc u:
8d RI   9d OC   ad --   bd 12   cd I'   dd Y'   ed i'   fd y'
8e S2   9e PM   ae Rg   be 34   ce I>   de TH   ee i>   fe th
8f S3   9f AC   af 'm   bf ?I   cf I:   df ss   ef i:   ff y:
'''

    def test_1(self):
        output = common.external_output('$R -lh latin-1')
        common.assert_or_diff(output, self.expected)

class Test_octal:

    # Octal listing of a charset.

    expected = '''\
ISO-8859-1

000 NU   020 DL   040 SP   060 0    100 At   120 P    140 '!   160 p
001 SH   021 D1   041 !    061 1    101 A    121 Q    141 a    161 q
002 SX   022 D2   042 "    062 2    102 B    122 R    142 b    162 r
003 EX   023 D3   043 Nb   063 3    103 C    123 S    143 c    163 s
004 ET   024 D4   044 DO   064 4    104 D    124 T    144 d    164 t
005 EQ   025 NK   045 %    065 5    105 E    125 U    145 e    165 u
006 AK   026 SY   046 &    066 6    106 F    126 V    146 f    166 v
007 BL   027 EB   047 '    067 7    107 G    127 W    147 g    167 w
010 BS   030 CN   050 (    070 8    110 H    130 X    150 h    170 x
011 HT   031 EM   051 )    071 9    111 I    131 Y    151 i    171 y
012 LF   032 SB   052 *    072 :    112 J    132 Z    152 j    172 z
013 VT   033 EC   053 +    073 ;    113 K    133 <(   153 k    173 (!
014 FF   034 FS   054 ,    074 <    114 L    134 //   154 l    174 !!
015 CR   035 GS   055 -    075 =    115 M    135 )>   155 m    175 !)
016 SO   036 RS   056 .    076 >    116 N    136 '>   156 n    176 '?
017 SI   037 US   057 /    077 ?    117 O    137 _    157 o    177 DT

200 PA   220 DC   240 NS   260 DG   300 A!   320 D-   340 a!   360 d-
201 HO   221 P1   241 !I   261 +-   301 A'   321 N?   341 a'   361 n?
202 BH   222 P2   242 Ct   262 2S   302 A>   322 O!   342 a>   362 o!
203 NH   223 TS   243 Pd   263 3S   303 A?   323 O'   343 a?   363 o'
204 IN   224 CC   244 Cu   264 ''   304 A:   324 O>   344 a:   364 o>
205 NL   225 MW   245 Ye   265 My   305 AA   325 O?   345 aa   365 o?
206 SA   226 SG   246 BB   266 PI   306 AE   326 O:   346 ae   366 o:
207 ES   227 EG   247 SE   267 .M   307 C,   327 *X   347 c,   367 -:
210 HS   230 SS   250 ':   270 ',   310 E!   330 O/   350 e!   370 o/
211 HJ   231 GC   251 Co   271 1S   311 E'   331 U!   351 e'   371 u!
212 VS   232 SC   252 -a   272 -o   312 E>   332 U'   352 e>   372 u'
213 PD   233 CI   253 <<   273 >>   313 E:   333 U>   353 e:   373 u>
214 PU   234 ST   254 NO   274 14   314 I!   334 U:   354 i!   374 u:
215 RI   235 OC   255 --   275 12   315 I'   335 Y'   355 i'   375 y'
216 S2   236 PM   256 Rg   276 34   316 I>   336 TH   356 i>   376 th
217 S3   237 AC   257 'm   277 ?I   317 I:   337 ss   357 i:   377 y:
'''

    def test_1(self):
        output = common.external_output('$R -lo latin-1')
        common.assert_or_diff(output, self.expected)

class Test_full:

    # Full listing of a charset.

    expected = '''\
Dec  Oct Hex   UCS2  Mne  ISO-8859-1

  0  000  00   0000  NU   null (nul)
  1  001  01   0001  SH   start of heading (soh)
  2  002  02   0002  SX   start of text (stx)
  3  003  03   0003  EX   end of text (etx)
  4  004  04   0004  ET   end of transmission (eot)
  5  005  05   0005  EQ   enquiry (enq)
  6  006  06   0006  AK   acknowledge (ack)
  7  007  07   0007  BL   bell (bel)
  8  010  08   0008  BS   backspace (bs)
  9  011  09   0009  HT   character tabulation (ht)
 10  012  0a   000A  LF   line feed (lf)
 11  013  0b   000B  VT   line tabulation (vt)
 12  014  0c   000C  FF   form feed (ff)
 13  015  0d   000D  CR   carriage return (cr)
 14  016  0e   000E  SO   shift out (so)
 15  017  0f   000F  SI   shift in (si)
 16  020  10   0010  DL   datalink escape (dle)
 17  021  11   0011  D1   device control one (dc1)
 18  022  12   0012  D2   device control two (dc2)
 19  023  13   0013  D3   device control three (dc3)
 20  024  14   0014  D4   device control four (dc4)
 21  025  15   0015  NK   negative acknowledge (nak)
 22  026  16   0016  SY   syncronous idle (syn)
 23  027  17   0017  EB   end of transmission block (etb)
 24  030  18   0018  CN   cancel (can)
 25  031  19   0019  EM   end of medium (em)
 26  032  1a   001A  SB   substitute (sub)
 27  033  1b   001B  EC   escape (esc)
 28  034  1c   001C  FS   file separator (is4)
 29  035  1d   001D  GS   group separator (is3)
 30  036  1e   001E  RS   record separator (is2)
 31  037  1f   001F  US   unit separator (is1)
 32  040  20   0020  SP   space
 33  041  21   0021  !    exclamation mark
 34  042  22   0022  "    quotation mark
 35  043  23   0023  Nb   number sign
 36  044  24   0024  DO   dollar sign
 37  045  25   0025  %    percent sign
 38  046  26   0026  &    ampersand
 39  047  27   0027  '    apostrophe
 40  050  28   0028  (    left parenthesis
 41  051  29   0029  )    right parenthesis
 42  052  2a   002A  *    asterisk
 43  053  2b   002B  +    plus sign
 44  054  2c   002C  ,    comma
 45  055  2d   002D  -    hyphen-minus
 46  056  2e   002E  .    full stop
 47  057  2f   002F  /    solidus
 48  060  30   0030  0    digit zero
 49  061  31   0031  1    digit one
 50  062  32   0032  2    digit two
 51  063  33   0033  3    digit three
 52  064  34   0034  4    digit four
 53  065  35   0035  5    digit five
 54  066  36   0036  6    digit six
 55  067  37   0037  7    digit seven
 56  070  38   0038  8    digit eight
 57  071  39   0039  9    digit nine
 58  072  3a   003A  :    colon
 59  073  3b   003B  ;    semicolon
 60  074  3c   003C  <    less-than sign
 61  075  3d   003D  =    equals sign
 62  076  3e   003E  >    greater-than sign
 63  077  3f   003F  ?    question mark
 64  100  40   0040  At   commercial at
 65  101  41   0041  A    latin capital letter a
 66  102  42   0042  B    latin capital letter b
 67  103  43   0043  C    latin capital letter c
 68  104  44   0044  D    latin capital letter d
 69  105  45   0045  E    latin capital letter e
 70  106  46   0046  F    latin capital letter f
 71  107  47   0047  G    latin capital letter g
 72  110  48   0048  H    latin capital letter h
 73  111  49   0049  I    latin capital letter i
 74  112  4a   004A  J    latin capital letter j
 75  113  4b   004B  K    latin capital letter k
 76  114  4c   004C  L    latin capital letter l
 77  115  4d   004D  M    latin capital letter m
 78  116  4e   004E  N    latin capital letter n
 79  117  4f   004F  O    latin capital letter o
 80  120  50   0050  P    latin capital letter p
 81  121  51   0051  Q    latin capital letter q
 82  122  52   0052  R    latin capital letter r
 83  123  53   0053  S    latin capital letter s
 84  124  54   0054  T    latin capital letter t
 85  125  55   0055  U    latin capital letter u
 86  126  56   0056  V    latin capital letter v
 87  127  57   0057  W    latin capital letter w
 88  130  58   0058  X    latin capital letter x
 89  131  59   0059  Y    latin capital letter y
 90  132  5a   005A  Z    latin capital letter z
 91  133  5b   005B  <(   left square bracket
 92  134  5c   005C  //   reverse solidus
 93  135  5d   005D  )>   right square bracket
 94  136  5e   005E  '>   circumflex accent
 95  137  5f   005F  _    low line
 96  140  60   0060  '!   grave accent
 97  141  61   0061  a    latin small letter a
 98  142  62   0062  b    latin small letter b
 99  143  63   0063  c    latin small letter c
100  144  64   0064  d    latin small letter d
101  145  65   0065  e    latin small letter e
102  146  66   0066  f    latin small letter f
103  147  67   0067  g    latin small letter g
104  150  68   0068  h    latin small letter h
105  151  69   0069  i    latin small letter i
106  152  6a   006A  j    latin small letter j
107  153  6b   006B  k    latin small letter k
108  154  6c   006C  l    latin small letter l
109  155  6d   006D  m    latin small letter m
110  156  6e   006E  n    latin small letter n
111  157  6f   006F  o    latin small letter o
112  160  70   0070  p    latin small letter p
113  161  71   0071  q    latin small letter q
114  162  72   0072  r    latin small letter r
115  163  73   0073  s    latin small letter s
116  164  74   0074  t    latin small letter t
117  165  75   0075  u    latin small letter u
118  166  76   0076  v    latin small letter v
119  167  77   0077  w    latin small letter w
120  170  78   0078  x    latin small letter x
121  171  79   0079  y    latin small letter y
122  172  7a   007A  z    latin small letter z
123  173  7b   007B  (!   left curly bracket
124  174  7c   007C  !!   vertical line
125  175  7d   007D  !)   right curly bracket
126  176  7e   007E  '?   tilde
127  177  7f   007F  DT   delete (del)
128  200  80   0080  PA   padding character (pad)
129  201  81   0081  HO   high octet preset (hop)
130  202  82   0082  BH   break permitted here (bph)
131  203  83   0083  NH   no break here (nbh)
132  204  84   0084  IN   index (ind)
133  205  85   0085  NL   next line (nel)
134  206  86   0086  SA   start of selected area (ssa)
135  207  87   0087  ES   end of selected area (esa)
136  210  88   0088  HS   character tabulation set (hts)
137  211  89   0089  HJ   character tabulation with justification (htj)
138  212  8a   008A  VS   line tabulation set (vts)
139  213  8b   008B  PD   partial line forward (pld)
140  214  8c   008C  PU   partial line backward (plu)
141  215  8d   008D  RI   reverse line feed (ri)
142  216  8e   008E  S2   single-shift two (ss2)
143  217  8f   008F  S3   single-shift three (ss3)
144  220  90   0090  DC   device control string (dcs)
145  221  91   0091  P1   private use one (pu1)
146  222  92   0092  P2   private use two (pu2)
147  223  93   0093  TS   set transmit state (sts)
148  224  94   0094  CC   cancel character (cch)
149  225  95   0095  MW   message waiting (mw)
150  226  96   0096  SG   start of guarded area (spa)
151  227  97   0097  EG   end of guarded area (epa)
152  230  98   0098  SS   start of string (sos)
153  231  99   0099  GC   single graphic character introducer (sgci)
154  232  9a   009A  SC   single character introducer (sci)
155  233  9b   009B  CI   control sequence introducer (csi)
156  234  9c   009C  ST   string terminator (st)
157  235  9d   009D  OC   operating system command (osc)
158  236  9e   009E  PM   privacy message (pm)
159  237  9f   009F  AC   application program command (apc)
160  240  a0   00A0  NS   no-break space
161  241  a1   00A1  !I   inverted exclamation mark
162  242  a2   00A2  Ct   cent sign
163  243  a3   00A3  Pd   pound sign
164  244  a4   00A4  Cu   currency sign
165  245  a5   00A5  Ye   yen sign
166  246  a6   00A6  BB   broken bar
167  247  a7   00A7  SE   section sign
168  250  a8   00A8  ':   diaeresis
169  251  a9   00A9  Co   copyright sign
170  252  aa   00AA  -a   feminine ordinal indicator
171  253  ab   00AB  <<   left-pointing double angle quotation mark
172  254  ac   00AC  NO   not sign
173  255  ad   00AD  --   soft hyphen
174  256  ae   00AE  Rg   registered sign
175  257  af   00AF  'm   macron
176  260  b0   00B0  DG   degree sign
177  261  b1   00B1  +-   plus-minus sign
178  262  b2   00B2  2S   superscript two
179  263  b3   00B3  3S   superscript three
180  264  b4   00B4  ''   acute accent
181  265  b5   00B5  My   micro sign
182  266  b6   00B6  PI   pilcrow sign
183  267  b7   00B7  .M   middle dot
184  270  b8   00B8  ',   cedilla
185  271  b9   00B9  1S   superscript one
186  272  ba   00BA  -o   masculine ordinal indicator
187  273  bb   00BB  >>   right-pointing double angle quotation mark
188  274  bc   00BC  14   vulgar fraction one quarter
189  275  bd   00BD  12   vulgar fraction one half
190  276  be   00BE  34   vulgar fraction three quarters
191  277  bf   00BF  ?I   inverted question mark
192  300  c0   00C0  A!   latin capital letter a with grave
193  301  c1   00C1  A'   latin capital letter a with acute
194  302  c2   00C2  A>   latin capital letter a with circumflex
195  303  c3   00C3  A?   latin capital letter a with tilde
196  304  c4   00C4  A:   latin capital letter a with diaeresis
197  305  c5   00C5  AA   latin capital letter a with ring above
198  306  c6   00C6  AE   latin capital letter ae
199  307  c7   00C7  C,   latin capital letter c with cedilla
200  310  c8   00C8  E!   latin capital letter e with grave
201  311  c9   00C9  E'   latin capital letter e with acute
202  312  ca   00CA  E>   latin capital letter e with circumflex
203  313  cb   00CB  E:   latin capital letter e with diaeresis
204  314  cc   00CC  I!   latin capital letter i with grave
205  315  cd   00CD  I'   latin capital letter i with acute
206  316  ce   00CE  I>   latin capital letter i with circumflex
207  317  cf   00CF  I:   latin capital letter i with diaeresis
208  320  d0   00D0  D-   latin capital letter eth (icelandic)
209  321  d1   00D1  N?   latin capital letter n with tilde
210  322  d2   00D2  O!   latin capital letter o with grave
211  323  d3   00D3  O'   latin capital letter o with acute
212  324  d4   00D4  O>   latin capital letter o with circumflex
213  325  d5   00D5  O?   latin capital letter o with tilde
214  326  d6   00D6  O:   latin capital letter o with diaeresis
215  327  d7   00D7  *X   multiplication sign
216  330  d8   00D8  O/   latin capital letter o with stroke
217  331  d9   00D9  U!   latin capital letter u with grave
218  332  da   00DA  U'   latin capital letter u with acute
219  333  db   00DB  U>   latin capital letter u with circumflex
220  334  dc   00DC  U:   latin capital letter u with diaeresis
221  335  dd   00DD  Y'   latin capital letter y with acute
222  336  de   00DE  TH   latin capital letter thorn (icelandic)
223  337  df   00DF  ss   latin small letter sharp s (german)
224  340  e0   00E0  a!   latin small letter a with grave
225  341  e1   00E1  a'   latin small letter a with acute
226  342  e2   00E2  a>   latin small letter a with circumflex
227  343  e3   00E3  a?   latin small letter a with tilde
228  344  e4   00E4  a:   latin small letter a with diaeresis
229  345  e5   00E5  aa   latin small letter a with ring above
230  346  e6   00E6  ae   latin small letter ae
231  347  e7   00E7  c,   latin small letter c with cedilla
232  350  e8   00E8  e!   latin small letter e with grave
233  351  e9   00E9  e'   latin small letter e with acute
234  352  ea   00EA  e>   latin small letter e with circumflex
235  353  eb   00EB  e:   latin small letter e with diaeresis
236  354  ec   00EC  i!   latin small letter i with grave
237  355  ed   00ED  i'   latin small letter i with acute
238  356  ee   00EE  i>   latin small letter i with circumflex
239  357  ef   00EF  i:   latin small letter i with diaeresis
240  360  f0   00F0  d-   latin small letter eth (icelandic)
241  361  f1   00F1  n?   latin small letter n with tilde
242  362  f2   00F2  o!   latin small letter o with grave
243  363  f3   00F3  o'   latin small letter o with acute
244  364  f4   00F4  o>   latin small letter o with circumflex
245  365  f5   00F5  o?   latin small letter o with tilde
246  366  f6   00F6  o:   latin small letter o with diaeresis
247  367  f7   00F7  -:   division sign
248  370  f8   00F8  o/   latin small letter o with stroke
249  371  f9   00F9  u!   latin small letter u with grave
250  372  fa   00FA  u'   latin small letter u with acute
251  373  fb   00FB  u>   latin small letter u with circumflex
252  374  fc   00FC  u:   latin small letter u with diaeresis
253  375  fd   00FD  y'   latin small letter y with acute
254  376  fe   00FE  th   latin small letter thorn (icelandic)
255  377  ff   00FF  y:   latin small letter y with diaeresis
'''

    def test_1(self):
        output = common.external_output('$R -lf latin-1')
        common.assert_or_diff(output, self.expected)
