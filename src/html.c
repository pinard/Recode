/* Conversion of files between different charsets and surfaces.
   Copyright © 1990, 93, 97, 98, 99 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1988.

   The `recode' Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License
   as published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The `recode' Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the `recode' Library; see the file `COPYING.LIB'.
   If not, write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA.  */

#include "common.h"
#include "hash.h"

/* FIXME: An @code{HTML} text which has spurious semi-colons to end entities
   (in strict mode) or does not always have them (in non-strict mode) is
   not canonical.  */

/* Tables.  */

/* The following entities, said to be from Emacs-w3, are ignored for the
   time being, as recode is not too fond on graphical approximations:

	&ensp;          \
	&emsp;          \ \
	&ndash;         -
	&mdash;         --
	&lsquo;         `
	&rsquo;         '
	&ldquo;         ``
	&rdquo;         ''
	&frac18;        1/8
	&frac38;        3/8
	&frac58;        5/8
	&frac78;        7/8
	&hellip;        . . .
	&larr;          <--
	&rarr;          -->
	&trade;         (TM)
*/

/* Old Emacs-W3, HTML 1.1 ? */
#define V11		(1 << 1)
/* RFC1866, HTML 2.0 */
#define V20		(1 << 2)
/* RFC2070, HTML-i18n */
#define V27		(1 << 3)
/* HTML 3.2 */
#define V32		(1 << 4)
/* HTML 4.0 */
#define V40		(1 << 5)

#define ENTRY(Code, String, Flags) \
  { Code, 0 Flags, String }

static struct ucs2_to_string main_translations [] =
  {
    ENTRY (33, "excl",							),
    ENTRY (34, "quot",			| V20				),
    ENTRY (35, "num",							),
    ENTRY (36, "dollar",						),
    ENTRY (37, "percnt",						),
    ENTRY (38, "amp",		| V11	| V20				),
    ENTRY (39, "apos",							),
    ENTRY (40, "lpar",							),
    ENTRY (41, "rpar",							),
    ENTRY (42, "ast",							),
    ENTRY (43, "plus",							),
    ENTRY (44, "comma",							),
    ENTRY (45, "horbar",						),
    ENTRY (46, "period",						),
    ENTRY (58, "colon",							),
    ENTRY (59, "semi",							),
    ENTRY (60, "lt",		| V11	| V20				),
    ENTRY (61, "equals",						),
    ENTRY (62, "gt",		| V11	| V20				),
    ENTRY (63, "quest",							),
    ENTRY (64, "commat",						),
    ENTRY (91, "lsqb",							),
    ENTRY (93, "rsqb",							),
    ENTRY (94, "uarr",							),
    ENTRY (95, "lowbar",						),
    ENTRY (96, "grave",							),
    ENTRY (123, "lcub",							),
    ENTRY (124, "verbar",						),
    ENTRY (125, "rcub",							),
    ENTRY (126, "tilde",						),
    ENTRY (160, "nbsp",				| V27	| V32	| V40	),
    ENTRY (161, "iexcl",			| V27	| V32	| V40	),
    ENTRY (162, "cent",				| V27	| V32	| V40	),
    ENTRY (163, "pound",			| V27	| V32	| V40	),
    ENTRY (164, "curren",			| V27	| V32	| V40	),
    ENTRY (165, "yen",				| V27	| V32	| V40	),
    ENTRY (166, "brkbar",	| V11					),
    ENTRY (166, "brvbar",			| V27	| V32	| V40	),
    ENTRY (167, "sect",				| V27	| V32	| V40	),
    ENTRY (168, "die",		| V11					),
    ENTRY (168, "uml",				| V27	| V32	| V40	),
    ENTRY (169, "copy",				| V27	| V32	| V40	),
    ENTRY (170, "ordf",				| V27	| V32	| V40	),
    ENTRY (171, "laquo",			| V27	| V32	| V40	),
    ENTRY (172, "not",				| V27	| V32	| V40	),
    ENTRY (173, "hyphen",	| V11					),
    ENTRY (173, "shy",				| V27	| V32	| V40	),
    ENTRY (174, "reg",				| V27	| V32	| V40	),
    ENTRY (175, "hibar",	| V11					),
    ENTRY (175, "macr",				| V27	| V32	| V40	),
    ENTRY (176, "deg",				| V27	| V32	| V40	),
    ENTRY (177, "plusmn",			| V27	| V32	| V40	),
    ENTRY (178, "sup2",				| V27	| V32	| V40	),
    ENTRY (179, "sup3",				| V27	| V32	| V40	),
    ENTRY (180, "acute",			| V27	| V32	| V40	),
    ENTRY (181, "micro",			| V27	| V32	| V40	),
    ENTRY (182, "para",				| V27	| V32	| V40	),
    ENTRY (183, "middot",			| V27	| V32	| V40	),
    ENTRY (184, "cedil",			| V27	| V32	| V40	),
    ENTRY (185, "sup1",				| V27	| V32	| V40	),
    ENTRY (186, "ordm",				| V27	| V32	| V40	),
    ENTRY (187, "raquo",			| V27	| V32	| V40	),
    ENTRY (188, "frac14",			| V27	| V32	| V40	),
    ENTRY (189, "half",		| V11					),
    ENTRY (189, "frac12",			| V27	| V32	| V40	),
    ENTRY (190, "frac34",			| V27	| V32	| V40	),
    ENTRY (191, "iquest",			| V27	| V32	| V40	),
    ENTRY (192, "Agrave",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (193, "Aacute",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (194, "Acircu",	| V11					),
    ENTRY (194, "Acirc",		| V20	| V27	| V32	| V40	),
    ENTRY (195, "Atilde",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (196, "Adiaer",	| V11					),
    ENTRY (196, "Auml",			| V20	| V27	| V32	| V40	),
    ENTRY (197, "Aring",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (198, "AE",		| V11					),
    ENTRY (198, "AElig",		| V20	| V27	| V32	| V40	),
    ENTRY (199, "Ccedil",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (200, "Egrave",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (201, "Eacute",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (202, "Ecircu",	| V11					),
    ENTRY (202, "Ecirc",		| V20	| V27	| V32	| V40	),
    ENTRY (203, "Ediaer",	| V11					),
    ENTRY (203, "Euml",			| V20	| V27	| V32	| V40	),
    ENTRY (204, "Igrave",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (205, "Iacute",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (206, "Icircu",	| V11					),
    ENTRY (206, "Icirc",		| V20	| V27	| V32	| V40	),
    ENTRY (207, "Idiaer",	| V11					),
    ENTRY (207, "Iuml",			| V20	| V27	| V32	| V40	),
    ENTRY (208, "ETH",		| V11	| V20	| V27	| V32	| V40	),
    ENTRY (209, "Ntilde",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (210, "Ograve",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (211, "Oacute",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (212, "Ocircu",	| V11					),
    ENTRY (212, "Ocirc",		| V20	| V27	| V32	| V40	),
    ENTRY (213, "Otilde",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (214, "Odiaer",	| V11					),
    ENTRY (214, "Ouml",			| V20	| V27	| V32	| V40	),
    ENTRY (215, "MULT",		| V11					),
    ENTRY (215, "times",			| V27	| V32	| V40	),
    ENTRY (216, "Ostroke",	| V11					),
    ENTRY (216, "Oslash",		| V20	| V27	| V32	| V40	),
    ENTRY (217, "Ugrave",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (218, "Uacute",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (219, "Ucircu",	| V11					),
    ENTRY (219, "Ucirc",		| V20	| V27	| V32	| V40	),
    ENTRY (220, "Udiaer",	| V11					),
    ENTRY (220, "Uuml",			| V20	| V27	| V32	| V40	),
    ENTRY (221, "Yacute",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (222, "THORN",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (223, "ssharp",	| V11					),
    ENTRY (223, "szlig",		| V20	| V27	| V32	| V40	),
    ENTRY (224, "agrave",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (225, "aacute",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (226, "acircu",	| V11					),
    ENTRY (226, "acirc",		| V20	| V27	| V32	| V40	),
    ENTRY (227, "atilde",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (228, "adiaer",	| V11					),
    ENTRY (228, "auml",			| V20	| V27	| V32	| V40	),
    ENTRY (229, "aring",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (230, "ae",		| V11					),
    ENTRY (230, "aelig",		| V20	| V27	| V32	| V40	),
    ENTRY (231, "ccedil",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (232, "egrave",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (233, "eacute",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (234, "ecircu",	| V11					),
    ENTRY (234, "ecirc",		| V20	| V27	| V32	| V40	),
    ENTRY (235, "ediaer",	| V11					),
    ENTRY (235, "euml",			| V20	| V27	| V32	| V40	),
    ENTRY (236, "igrave",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (237, "iacute",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (238, "icircu",	| V11					),
    ENTRY (238, "icirc",		| V20	| V27	| V32	| V40	),
    ENTRY (239, "idiaer",	| V11					),
    ENTRY (239, "iuml",			| V20	| V27	| V32	| V40	),
    ENTRY (240, "eth",		| V11	| V20	| V27	| V32	| V40	),
    ENTRY (241, "ntilde",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (242, "ograve",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (243, "oacute",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (244, "ocircu",	| V11					),
    ENTRY (244, "ocirc",		| V20	| V27	| V32	| V40	),
    ENTRY (245, "otilde",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (246, "odiaer",	| V11					),
    ENTRY (246, "ouml",			| V20	| V27	| V32	| V40	),
    ENTRY (247, "DIVIS",	| V11					),
    ENTRY (247, "divide",			| V27	| V32	| V40	),
    ENTRY (248, "ostroke",	| V11					),
    ENTRY (248, "oslash",		| V20	| V27	| V32	| V40	),
    ENTRY (249, "ugrave",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (250, "uacute",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (251, "ucircu",	| V11					),
    ENTRY (251, "ucirc",		| V20	| V27	| V32	| V40	),
    ENTRY (252, "udiaer",	| V11					),
    ENTRY (252, "uuml",			| V20	| V27	| V32	| V40	),
    ENTRY (253, "yacute",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (254, "thorn",	| V11	| V20	| V27	| V32	| V40	),
    ENTRY (255, "ydiaer",	| V11					),
    ENTRY (255, "yuml",			| V20	| V27	| V32	| V40	),
    ENTRY (338, "OElig",					| V40	),
    ENTRY (339, "oelig",					| V40	),
    ENTRY (352, "Scaron",					| V40	),
    ENTRY (353, "scaron",					| V40	),
    ENTRY (376, "Yuml",						| V40	),
    ENTRY (402, "fnof",						| V40	),
    ENTRY (710, "circ",						| V40	),
    ENTRY (732, "tilde",					| V40	),
    ENTRY (913, "Alpha",					| V40	),
    ENTRY (914, "Beta",						| V40	),
    ENTRY (915, "Gamma",					| V40	),
    ENTRY (916, "Delta",					| V40	),
    ENTRY (917, "Epsilon",					| V40	),
    ENTRY (918, "Zeta",						| V40	),
    ENTRY (919, "Eta",						| V40	),
    ENTRY (920, "Theta",					| V40	),
    ENTRY (921, "Iota",						| V40	),
    ENTRY (922, "Kappa",					| V40	),
    ENTRY (923, "Lambda",					| V40	),
    ENTRY (924, "Mu",						| V40	),
    ENTRY (925, "Nu",						| V40	),
    ENTRY (926, "Xi",						| V40	),
    ENTRY (927, "Omicron",					| V40	),
    ENTRY (928, "Pi",						| V40	),
    ENTRY (929, "Rho",						| V40	),
    ENTRY (931, "Sigma",					| V40	),
    ENTRY (932, "Tau",						| V40	),
    ENTRY (933, "Upsilon",					| V40	),
    ENTRY (934, "Phi",						| V40	),
    ENTRY (935, "Chi",						| V40	),
    ENTRY (936, "Psi",						| V40	),
    ENTRY (937, "Omega",					| V40	),
    ENTRY (945, "alpha",					| V40	),
    ENTRY (946, "beta",						| V40	),
    ENTRY (947, "gamma",					| V40	),
    ENTRY (948, "delta",					| V40	),
    ENTRY (949, "epsilon",					| V40	),
    ENTRY (950, "zeta",						| V40	),
    ENTRY (951, "eta",						| V40	),
    ENTRY (952, "theta",					| V40	),
    ENTRY (953, "iota",						| V40	),
    ENTRY (954, "kappa",					| V40	),
    ENTRY (955, "lambda",					| V40	),
    ENTRY (956, "mu",						| V40	),
    ENTRY (957, "nu",						| V40	),
    ENTRY (958, "xi",						| V40	),
    ENTRY (959, "omicron",					| V40	),
    ENTRY (960, "pi",						| V40	),
    ENTRY (961, "rho",						| V40	),
    ENTRY (962, "sigmaf",					| V40	),
    ENTRY (963, "sigma",					| V40	),
    ENTRY (964, "tau",						| V40	),
    ENTRY (965, "upsilon",					| V40	),
    ENTRY (966, "phi",						| V40	),
    ENTRY (967, "chi",						| V40	),
    ENTRY (968, "psi",						| V40	),
    ENTRY (969, "omega",					| V40	),
    ENTRY (977, "thetasym",					| V40	),
    ENTRY (978, "upsih",					| V40	),
    ENTRY (982, "piv",						| V40	),
    ENTRY (8194, "ensp",					| V40	),
    ENTRY (8195, "emsp",					| V40	),
    ENTRY (8201, "thinsp",					| V40	),
    ENTRY (8204, "zwnj",			| V27		| V40	),
    ENTRY (8205, "zwj",				| V27		| V40	),
    ENTRY (8206, "lrm",				| V27		| V40	),
    ENTRY (8207, "rlm",				| V27		| V40	),
    ENTRY (8211, "ndash",					| V40	),
    ENTRY (8212, "mdash",					| V40	),
    ENTRY (8216, "lsquo",					| V40	),
    ENTRY (8217, "rsquo",					| V40	),
    ENTRY (8218, "sbquo",					| V40	),
    ENTRY (8220, "ldquo",					| V40	),
    ENTRY (8221, "rdquo",					| V40	),
    ENTRY (8222, "bdquo",					| V40	),
    ENTRY (8224, "dagger",					| V40	),
    ENTRY (8225, "Dagger",					| V40	),
    ENTRY (8226, "bull",					| V40	),
    ENTRY (8230, "hellip",					| V40	),
    ENTRY (8240, "permil",					| V40	),
    ENTRY (8242, "prime",					| V40	),
    ENTRY (8243, "Prime",					| V40	),
    ENTRY (8249, "lsaquo",					| V40	),
    ENTRY (8250, "rsaquo",					| V40	),
    ENTRY (8254, "oline",					| V40	),
    ENTRY (8260, "frasl",					| V40	),
    ENTRY (8364, "euro",					| V40	),
    ENTRY (8465, "image",					| V40	),
    ENTRY (8472, "weierp",					| V40	),
    ENTRY (8476, "real",					| V40	),
    ENTRY (8482, "trade",					| V40	),
    ENTRY (8501, "alefsym",					| V40	),
    ENTRY (8592, "larr",					| V40	),
    ENTRY (8593, "uarr",					| V40	),
    ENTRY (8594, "rarr",					| V40	),
    ENTRY (8595, "darr",					| V40	),
    ENTRY (8596, "harr",					| V40	),
    ENTRY (8629, "crarr",					| V40	),
    ENTRY (8656, "lArr",					| V40	),
    ENTRY (8657, "uArr",					| V40	),
    ENTRY (8658, "rArr",					| V40	),
    ENTRY (8659, "dArr",					| V40	),
    ENTRY (8660, "hArr",					| V40	),
    ENTRY (8704, "forall",					| V40	),
    ENTRY (8706, "part",					| V40	),
    ENTRY (8707, "exist",					| V40	),
    ENTRY (8709, "empty",					| V40	),
    ENTRY (8711, "nabla",					| V40	),
    ENTRY (8712, "isin",					| V40	),
    ENTRY (8713, "notin",					| V40	),
    ENTRY (8715, "ni",						| V40	),
    ENTRY (8719, "prod",					| V40	),
    ENTRY (8721, "sum",						| V40	),
    ENTRY (8722, "minus",					| V40	),
    ENTRY (8727, "lowast",					| V40	),
    ENTRY (8730, "radic",					| V40	),
    ENTRY (8733, "prop",					| V40	),
    ENTRY (8734, "infin",					| V40	),
    ENTRY (8736, "ang",						| V40	),
    ENTRY (8743, "and",						| V40	),
    ENTRY (8744, "or",						| V40	),
    ENTRY (8745, "cap",						| V40	),
    ENTRY (8746, "cup",						| V40	),
    ENTRY (8747, "int",						| V40	),
    ENTRY (8756, "there4",					| V40	),
    ENTRY (8764, "sim",						| V40	),
    ENTRY (8773, "cong",					| V40	),
    ENTRY (8776, "asymp",					| V40	),
    ENTRY (8800, "ne",						| V40	),
    ENTRY (8801, "equiv",					| V40	),
    ENTRY (8804, "le",						| V40	),
    ENTRY (8805, "ge",						| V40	),
    ENTRY (8834, "sub",						| V40	),
    ENTRY (8835, "sup",						| V40	),
    ENTRY (8836, "nsub",					| V40	),
    ENTRY (8838, "sube",					| V40	),
    ENTRY (8839, "supe",					| V40	),
    ENTRY (8853, "oplus",					| V40	),
    ENTRY (8855, "otimes",					| V40	),
    ENTRY (8869, "perp",					| V40	),
    ENTRY (8901, "sdot",					| V40	),
    ENTRY (8968, "lceil",					| V40	),
    ENTRY (8969, "rceil",					| V40	),
    ENTRY (8970, "lfloor",					| V40	),
    ENTRY (8971, "rfloor",					| V40	),
    ENTRY (9001, "lang",					| V40	),
    ENTRY (9002, "rang",					| V40	),
    ENTRY (9674, "loz",						| V40	),
    ENTRY (9824, "spades",					| V40	),
    ENTRY (9827, "clubs",					| V40	),
    ENTRY (9829, "hearts",					| V40	),
    ENTRY (9830, "diams",					| V40	),
    ENTRY (0, NULL,							)
  };

static struct ucs2_to_string const other_translations [] =
  {
    ENTRY (34, "quot", 							),
    ENTRY (38, "amp", 							),
    ENTRY (60, "lt", 							),
    ENTRY (62, "gt", 							),
    ENTRY (0, NULL,							)
  };

#undef ENTRY

/* UCS-2 towards HTML.  */

/*-------------------------------------.
| Return hash value given TABLE_SIZE.  |
`-------------------------------------*/

static unsigned
code_hash (const void *void_data, unsigned table_size)
{
  struct ucs2_to_string const *data = void_data;

  return data->code % table_size;
}

/*----------------------------------------.
| Say if two translations are identical.  |
`----------------------------------------*/

static bool
code_compare (const void *void_first, const void *void_second)
{
  struct ucs2_to_string const *first = void_first;
  struct ucs2_to_string const *second = void_second;

  return first->code == second->code;
}

/*-----------------.
| Initialisation.  |
`-----------------*/

static bool
init_ucs2_html (RECODE_STEP step,
		RECODE_CONST_REQUEST request,
		RECODE_CONST_OPTION_LIST before_options,
		RECODE_CONST_OPTION_LIST after_options,
		unsigned mask)
{
  Hash_table *table;
  RECODE_OUTER outer = request->outer;
  struct ucs2_to_string const *cursor;
  bool done;

  if (before_options || after_options)
    return false;

  table = hash_initialize (0, NULL, code_hash, code_compare, NULL);
  if (!table)
    return false;

  for (cursor = main_translations; cursor->code; cursor++)
    if (cursor->code > 128 && cursor->flags & mask)
      if (!hash_insert (table, cursor))
	return false;
  if (!request->diacritics_only)
    for (cursor = other_translations; cursor->code; cursor++)
      if (!hash_insert (table, cursor))
	return false;

  step->step_type = RECODE_UCS2_TO_STRING;
  step->step_table = table;
  return true;
}

/*-----------------.
| Initialisation.  |
`-----------------*/

static bool
init_ucs2_html_v11 (RECODE_STEP step,
		    RECODE_CONST_REQUEST request,
		    RECODE_CONST_OPTION_LIST before_options,
		    RECODE_CONST_OPTION_LIST after_options)
{
  init_ucs2_html (step, request, before_options, after_options, V11);
}

static bool
init_ucs2_html_v20 (RECODE_STEP step,
		    RECODE_CONST_REQUEST request,
		    RECODE_CONST_OPTION_LIST before_options,
		    RECODE_CONST_OPTION_LIST after_options)
{
  init_ucs2_html (step, request, before_options, after_options, V20);
}

static bool
init_ucs2_html_v27 (RECODE_STEP step,
		    RECODE_CONST_REQUEST request,
		    RECODE_CONST_OPTION_LIST before_options,
		    RECODE_CONST_OPTION_LIST after_options)
{
  init_ucs2_html (step, request, before_options, after_options, V27);
}

static bool
init_ucs2_html_v32 (RECODE_STEP step,
		    RECODE_CONST_REQUEST request,
		    RECODE_CONST_OPTION_LIST before_options,
		    RECODE_CONST_OPTION_LIST after_options)
{
  init_ucs2_html (step, request, before_options, after_options, V32);
}

static bool
init_ucs2_html_v40 (RECODE_STEP step,
		    RECODE_CONST_REQUEST request,
		    RECODE_CONST_OPTION_LIST before_options,
		    RECODE_CONST_OPTION_LIST after_options)
{
  init_ucs2_html (step, request, before_options, after_options, V40);
}

static bool
transform_ucs2_html (RECODE_CONST_STEP step, RECODE_TASK task)
{
  Hash_table *table = step->step_table;
  unsigned value;

  while (get_ucs2 (&value, step, task))
    {
      struct ucs2_to_string lookup;
      struct ucs2_to_string *entry;

      lookup.code = value;
      entry = hash_lookup (table, &lookup);
      if (entry)
	{
	  const char *cursor = entry->string;

	  put_byte ('&', task);
	  while (*cursor)
	    {
	      put_byte (*cursor, task);
	      cursor++;
	    }
	  put_byte (';', task);
	}
      else if ((value < 32 || value >= 127) && value != '\n')
	{
	  unsigned divider = 10000;

	  put_byte ('&', task);
	  put_byte ('#', task);
	  while (divider > value)
	    divider /= 10;
	  while (divider)
	    {
	      put_byte ('0' + value / divider, task);
	      divider /= 10;
	    }
	  put_byte (';', task);
	}
      else
	put_byte(value, task);
    }

  TASK_RETURN (task);
}

/* HTML towards UCS-2.  */

#define ENTITY_BUFFER_LENGTH 20

/*
&quot;	{ if (request->diacritics_only) ECHO; else put_ucs2 (34, task); }
&amp;	{ if (request->diacritics_only) ECHO; else put_ucs2 (38, task); }
&lt;	{ if (request->diacritics_only) ECHO; else put_ucs2 (60, task); }
&gt;	{ if (request->diacritics_only) ECHO; else put_ucs2 (62, task); }
*/

/*-------------------------------------.
| Return hash value given TABLE_SIZE.  |
`-------------------------------------*/

static unsigned
string_hash (const void *void_data, unsigned table_size)
{
  struct ucs2_to_string const *data = void_data;

  return hash_string (data->string, table_size);
}

/*----------------------------------------.
| Say if two translations are identical.  |
`----------------------------------------*/

static bool
string_compare (const void *void_first, const void *void_second)
{
  struct ucs2_to_string const *first = void_first;
  struct ucs2_to_string const *second = void_second;

  return strcmp (first->string, second->string) == 0;
}

/*-----------------.
| Initialisation.  |
`-----------------*/

static bool
init_html_ucs2 (RECODE_STEP step,
		RECODE_CONST_REQUEST request,
		RECODE_CONST_OPTION_LIST before_options,
		RECODE_CONST_OPTION_LIST after_options,
		unsigned mask)
{
  Hash_table *table;
  RECODE_OUTER outer = request->outer;
  struct ucs2_to_string const *cursor;
  bool done;

  if (before_options || after_options)
    return false;

  table = hash_initialize (0, NULL, string_hash, string_compare, NULL);
  if (!table)
    return false;

  for (cursor = main_translations; cursor->code; cursor++)
    if (cursor->flags & mask)
      if (!hash_insert (table, cursor))
	return false;
  if (!request->diacritics_only)
    for (cursor = other_translations; cursor->code; cursor++)
      if (!hash_insert (table, cursor))
	return false;

  step->step_type = RECODE_STRING_TO_UCS2;
  step->step_table = table;
  return true;
}

static bool
init_html_v11_ucs2 (RECODE_STEP step,
		    RECODE_CONST_REQUEST request,
		    RECODE_CONST_OPTION_LIST before_options,
		    RECODE_CONST_OPTION_LIST after_options)
{
  init_html_ucs2 (step, request, before_options, after_options, V11);
}

static bool
init_html_v20_ucs2 (RECODE_STEP step,
		    RECODE_CONST_REQUEST request,
		    RECODE_CONST_OPTION_LIST before_options,
		    RECODE_CONST_OPTION_LIST after_options)
{
  init_html_ucs2 (step, request, before_options, after_options, V20);
}

static bool
init_html_v27_ucs2 (RECODE_STEP step,
		    RECODE_CONST_REQUEST request,
		    RECODE_CONST_OPTION_LIST before_options,
		    RECODE_CONST_OPTION_LIST after_options)
{
  init_html_ucs2 (step, request, before_options, after_options, V27);
}

static bool
init_html_v32_ucs2 (RECODE_STEP step,
		    RECODE_CONST_REQUEST request,
		    RECODE_CONST_OPTION_LIST before_options,
		    RECODE_CONST_OPTION_LIST after_options)
{
  init_html_ucs2 (step, request, before_options, after_options, V32);
}

static bool
init_html_v40_ucs2 (RECODE_STEP step,
		    RECODE_CONST_REQUEST request,
		    RECODE_CONST_OPTION_LIST before_options,
		    RECODE_CONST_OPTION_LIST after_options)
{
  init_html_ucs2 (step, request, before_options, after_options, V40);
}

/*-----------------.
| Transformation.  |
`-----------------*/

static bool
transform_html_ucs2 (RECODE_CONST_STEP step, RECODE_TASK task)
{
  RECODE_CONST_REQUEST request = task->request;
  int input_char;

  input_char = get_byte (task);
  while (input_char != EOF)

    if (input_char == '&')
      {
	char buffer[ENTITY_BUFFER_LENGTH];
	char *cursor = buffer;
	bool valid = true;
	bool echo = false;

	input_char = get_byte (task);
	if (input_char == '#')
	  {
	    input_char = get_byte (task);
	    if (input_char == 'x' || input_char == 'X')
	      {
		unsigned value = 0;

		/* Scan &#[xX][0-9a-fA-F]+; notation.  */

		*cursor++ = '#';
		*cursor++ = input_char;
		input_char = get_byte (task);

		while (valid)
		  {
		    if (input_char >= '0' && input_char < '9')
		      value = 16 * value + input_char - '0';
		    else if (input_char >= 'A' && input_char < 'Z')
		      value = 16 * value + input_char - 'A' + 10;
		    else if (input_char >= 'a' && input_char < 'z')
		      value = 16 * value + input_char - 'a' + 10;
		    else
		      break;

		    if (value >= 65535)
		      valid = false;
		    else if (cursor == buffer + ENTITY_BUFFER_LENGTH - 1)
		      valid = false;
		    else
		      {
			*cursor++ = input_char;
			input_char = get_byte (task);
		      }
		  }

		if (valid)
		  if (request->diacritics_only)
		    echo = true;
		  else
		    {
		      put_ucs2 (value, task);
		      if (input_char == ';')
			input_char = get_byte (task);
		    }
		else
		  *cursor = '\0';
	      }
	    else
	      {
		unsigned value = 0;

		/* Scan &#[0-9]+; notation.  */

		while (valid)
		  {
		    if (input_char >= '0' && input_char < '9')
		      value = 10 * value + input_char - '0';
		    else
		      break;

		    if (value >= 65535)
		      valid = false;
		    else if (cursor == buffer + ENTITY_BUFFER_LENGTH - 1)
		      valid = false;
		    else
		      {
			*cursor++ = input_char;
			input_char = get_byte (task);
		      }
		  }

		if (valid)
		  if (request->diacritics_only)
		    echo = true;
		  else
		    {
		      put_ucs2 (value, task);
		      if (input_char == ';')
			input_char = get_byte (task);
		    }
		else
		  *cursor = '\0';
	      }
	  }
	else if ((input_char >= 'A' && input_char <= 'Z')
		 || (input_char >= 'a' && input_char <= 'z'))
	  {
	    /* Scan &[A-Za-z][A-Za-z0-9]*; notation.  */

	    *cursor++ = input_char;
	    input_char = get_byte (task);

	    while (valid
		   && input_char != EOF
		   && ((input_char >= 'A' && input_char <= 'Z')
		       || (input_char >= 'a' && input_char <= 'z')
		       || (input_char >= '0' && input_char <= '9')))
	      if (cursor == buffer + ENTITY_BUFFER_LENGTH - 1)
		valid = false;
	      else
		{
		  *cursor++ = input_char;
		  input_char = get_byte (task);
		}
	    *cursor = '\0';

	    if (valid)
	      {
		struct ucs2_to_string lookup;
		struct ucs2_to_string *entry;

		lookup.string = buffer;
		entry = hash_lookup (step->step_table, &lookup);
		if (entry)
		  {
		    put_ucs2 (entry->code, task);
		    if (input_char == ';')
		      input_char = get_byte (task);
		  }
		else
		  valid = false;
	      }
	  }

	if (echo || !valid)
	  {
	    put_ucs2 ('&', task);
	    for (cursor = buffer; *cursor; cursor++)
	      put_ucs2 (*cursor, task);
	  }
      }
    else
      {
	put_ucs2 (input_char, task);
	input_char = get_byte (task);
      }

  TASK_RETURN (task);
}

/* Module declaration.  */
bool
module_html (RECODE_OUTER outer)
{
  return
    declare_single (outer, "ISO-10646-UCS-2", "HTML_1.1",
		    outer->quality_byte_to_variable,
		    init_ucs2_html_v11, transform_ucs2_html)
    && declare_single (outer, "HTML_1.1", "ISO-10646-UCS-2",
		       outer->quality_variable_to_byte,
		       init_html_v11_ucs2, transform_html_ucs2)
    && declare_single (outer, "ISO-10646-UCS-2", "HTML_2.0",
		       outer->quality_byte_to_variable,
		       init_ucs2_html_v20, transform_ucs2_html)
    && declare_single (outer, "HTML_2.0", "ISO-10646-UCS-2",
		       outer->quality_variable_to_byte,
		       init_html_v20_ucs2, transform_html_ucs2)
    && declare_single (outer, "ISO-10646-UCS-2", "HTML-i18n",
		       outer->quality_byte_to_variable,
		       init_ucs2_html_v27, transform_ucs2_html)
    && declare_single (outer, "HTML-i18n", "ISO-10646-UCS-2",
		       outer->quality_variable_to_byte,
		       init_html_v27_ucs2, transform_html_ucs2)
    && declare_single (outer, "ISO-10646-UCS-2", "HTML_3.2",
		       outer->quality_byte_to_variable,
		       init_ucs2_html_v32, transform_ucs2_html)
    && declare_single (outer, "HTML_3.2", "ISO-10646-UCS-2",
		       outer->quality_variable_to_byte,
		       init_html_v32_ucs2, transform_html_ucs2)
    && declare_single (outer, "ISO-10646-UCS-2", "HTML_4.0",
		       outer->quality_byte_to_variable,
		       init_ucs2_html_v40, transform_ucs2_html)
    && declare_single (outer, "HTML_4.0", "ISO-10646-UCS-2",
		       outer->quality_variable_to_byte,
		       init_html_v40_ucs2, transform_html_ucs2)

    && declare_alias (outer, "h1", "HTML_1.1")
    && declare_alias (outer, "RFC1866", "HTML_2.0")
    && declare_alias (outer, "1866", "HTML_2.0")
    && declare_alias (outer, "h2", "HTML_2.0")
    && declare_alias (outer, "RFC2070", "HTML-i18n")
    && declare_alias (outer, "2070", "HTML-i18n")
    && declare_alias (outer, "h3", "HTML_3.2")
    && declare_alias (outer, "h4", "HTML_4.0")
    /* HTML defaults to the highest level available.  */
    && declare_alias (outer, "HTML", "HTML_4.0")
    && declare_alias (outer, "h", "HTML_4.0");
}
