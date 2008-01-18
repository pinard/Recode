/* Conversion of files between different charsets and surfaces.
   Copyright © 1990, 93, 94, 96, 97, 98, 99, 00 Free Software Foundation, Inc.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1988.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the `recode' Library; see the file `COPYING.LIB'.
   If not, write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef PARAMS
# if __STDC__
#  define PARAMS(Args) Args
# else
#  define PARAMS(Args) ()
# endif
#endif

/* Published (opaque) typedefs.  */
typedef struct recode_outer * 			RECODE_OUTER;
typedef struct recode_request *			RECODE_REQUEST;
typedef struct recode_task *			RECODE_TASK;
typedef const struct recode_request *		RECODE_CONST_REQUEST;
typedef const struct recode_symbol *		RECODE_CONST_SYMBOL;

/* Description of list formats.  */

enum recode_list_format
{
  RECODE_NO_FORMAT,		/* format not decided yet */
  RECODE_DECIMAL_FORMAT,	/* concise tabular list using decimal */
  RECODE_OCTAL_FORMAT,		/* concise tabular list using octal */
  RECODE_HEXADECIMAL_FORMAT,	/* concise tabular list using hexadecimal */
  RECODE_FULL_FORMAT		/* full list, one character per line */
};

/* Description of programming languages.  */

enum recode_programming_language
{
  RECODE_NO_LANGUAGE,		/* language not decided yet */
  RECODE_LANGUAGE_C,		/* C (or C++) */
  RECODE_LANGUAGE_PERL		/* Perl */
};

/* Function prototypes.  */

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------.
| Recode library at OUTER level.  |
`--------------------------------*/

RECODE_OUTER recode_new_outer PARAMS ((bool));
bool recode_delete_outer PARAMS ((RECODE_OUTER));

bool list_all_symbols PARAMS ((RECODE_OUTER, RECODE_CONST_SYMBOL));
bool list_concise_charset PARAMS ((RECODE_OUTER, RECODE_CONST_SYMBOL,
				  const enum recode_list_format));
bool list_full_charset PARAMS ((RECODE_OUTER, RECODE_CONST_SYMBOL));

/*----------------------------------.
| Recode library at REQUEST level.  |
`----------------------------------*/

RECODE_REQUEST recode_new_request PARAMS ((RECODE_OUTER));
bool recode_delete_request PARAMS ((RECODE_REQUEST));
bool recode_scan_request PARAMS ((RECODE_REQUEST, const char *));

bool recode_format_table PARAMS ((RECODE_REQUEST,
				  enum recode_programming_language,
				  const char *));

char *recode_string PARAMS ((RECODE_CONST_REQUEST, const char *));

bool recode_string_to_buffer PARAMS ((RECODE_CONST_REQUEST,
				      const char *,
				      char **, size_t *, size_t *));
bool recode_string_to_file PARAMS ((RECODE_CONST_REQUEST,
				    const char *,
				    FILE *));
bool recode_buffer_to_buffer PARAMS ((RECODE_CONST_REQUEST,
				      const char *, size_t,
				      char **, size_t *, size_t *));
bool recode_buffer_to_file PARAMS ((RECODE_CONST_REQUEST,
				    const char *, size_t,
				    FILE *));
bool recode_file_to_buffer PARAMS ((RECODE_CONST_REQUEST,
				    FILE *,
				    char **, size_t *, size_t *));
bool recode_file_to_file PARAMS ((RECODE_CONST_REQUEST,
				  FILE *,
				  FILE *));

/*-------------------------------.
| Recode library at TASK level.  |
`-------------------------------*/

RECODE_TASK recode_new_task PARAMS ((RECODE_CONST_REQUEST));
bool recode_delete_task PARAMS ((RECODE_TASK));
bool recode_perform_task PARAMS ((RECODE_TASK));
/* FILE *recode_filter_open PARAMS ((RECODE_TASK, FILE *)); */
/* bool recode_filter_close PARAMS ((RECODE_TASK)); */

#ifdef __cplusplus
}
#endif
