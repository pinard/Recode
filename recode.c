/* Conversion of files between different charsets and usages.
   Copyright (C) 1990, 1992, 1993, 1994 Free Software Foundation, Inc.
   Francois Pinard <pinard@iro.umontreal.ca>, 1990.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "recode.h"

/* Maximum number of single step methods.  */
#define MAX_SINGLE_STEPS 300

/* Maximum length of a conversion sequence.  */
#define MAX_SEQUENCE 12

/* Global declarations and definitions.  */

#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <setjmp.h>
#include <signal.h>
#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#define strchr index
#define strrchr rindex
#endif

#ifdef MSDOS
#define MSDOS_or_OS2 1
#include <dir.h>
#define unlink dummy1
#include <io.h>
#undef unlink
#include <fcntl.h>
#endif

#ifdef OS2
#define MSDOS_or_OS2 1
#include <io.h>
#include <fcntl.h>
#endif

#include <errno.h>
#ifndef errno
extern int errno;
#endif

#include "getopt.h"

/* tmpnam/tmpname/mktemp/tmpfile and the associate logic has been the
   main portability headache of GNU recode :-(.
   
   People reported that tmpname does not exist everywhere.  On OS/2,
   recode aborts if the prefix has more than five characters.
   
   tmpnam seems to exist everywhere so far.  But NeXT's tmpnam() is such
   that, if called many times in succession, it will always return the
   same value.  One has to really open a file with the returned name
   first, for the next call to tmpnam() to return a different value.  I
   can manage it for a single invocation of recode, but using two recode
   invocations connected with a shell pipe, on the NeXT, creates a race
   by which both copies may call tmpnam() in parallel, then getting the
   same value, and will consequently open the same temporary file.
   
   Noah Friedman <friedman@gnu.ai.mit.edu> suggests opening the file with
   O_EXCL, and when the open presumably fails, call tmpnam again, or try
   the mktemp routine in the GNU C library...maybe that will work better.
   
   Michael I Bushnell <mib@gnu.ai.mit.edu> suggests always using tmpfile,
   which opens the file too, using the O_EXCL option to open.
   
   I'm trying this last suggestion, rewinding instead of closing.
   Someone reported, a long while ago, that rewind did not work on his
   system, so I reverted to opening and closing the temporary files all
   the time.  I lost the precise references for this problem.  In any
   case, I'm reusing rewind with tmpfile, now.  Hopefully, someone will
   tell me if this creates a problem somewhere!  */

/* The previous round used tmpnam(3).  This one tries tmpfile(3).  */
/* #define USE_TMPNAM 1 */
#define USE_TMPFILE 1

#ifdef USE_TMPNAM
/* Guarantee some value for L_tmpnam.  */
#ifdef MSDOS

#define L_tmpnam 13

#else /* not MSDOS */

char *tmpnam ();

#ifndef L_tmpnam
#include "pathmax.h"
#define L_tmpnam PATH_MAX
#endif

#endif /* not MSDOS */
#endif /* USE_TMPNAM */

#ifdef USE_TMPFILE

FILE *tmpfile _((void));

#endif /* USE_TMPFILE */

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#ifdef MSDOS
/* Increase stack size, so init_recode_rfc1345 works.  */
extern unsigned _stklen = 10000U;
#endif

/* Variables.  */

const char *const copyright_string = "\
This program is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 2, or (at your option)\n\
any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program; if not, write to the Free Software\n\
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n";

/* The name this program was run with. */
const char *program_path;
const char *program_name;

/* If non-zero, display usage information and exit.  */
static int show_help = 0;

/* If non-zero, print the version on standard output and exit.  */
static int show_version = 0;

/* If non-zero, show a list of one or all known charsets, then exit.  */
static int show_charsets = 0;

/* If non-zero, show a restricted list of charsets.  */
static int show_restricted_charsets = 0;

/* Indicates the format for showing only one charset.  */
enum list_format list_format = NO_FORMAT;

/* If non-zero, merely explore all recoding paths, report and exit.  */
static int auto_check_mode = 0;

/* If non-zero, produce C code for initializing the conversion and exit.  */
static int make_header_mode = 0;

/* Table name in generated C code.  */
static const char *header_name = NULL;

/* If the recoding yields some problems in reversibility in some file,
   this file replacement is denied and it is left unrecoded or, if recode
   is used as a mere filter, the recoding is interrupted.  The following
   option forces the recoding to run to completion and the replacement to
   be done even if the recoding is not reversible.  */
int force_option = 0;

/* This option prevents recode from automatically completing charsets.  */
int strict_mapping = 0;

/* This option merely inhibits messages about non-reversible recodings, but
   it does not prevent recodings to be aborted or exit status to be set.  */
static int quiet_mode = 0;

/* By selecting the following option, the program will echo to stderr the
   sequence of elementary recoding steps which will be taken to effect
   the requested recoding.  */
int verbose_option = 0;

/* When a file is recoded over itself, precautions are taken to move the
   timestamps of the original file into the recoded file, so to make the
   recoding the most transparent possible to make, and other tools.
   However, selecting the following option inhibit the timestamps handling,
   thus effectively `touching' the file.  */
int touch_option = 0;

/* In `texte' charset, some countries use double quotes to mark diaeresis,
   while other countries prefer colons.  The following variable contains the
   diaeresis character for `texte' charset.  Nominally set to a double
   quote, it can be forced to a colon by an option on recode command.  */
char diaeresis_char = '"';

/* For `latex' charset, it is often convenient to recode the diacritics
   only, while letting other LaTeX code using backslashes unrecoded.
   In the other charset, one can edit text as well as LaTeX directives.  */
int diacritics_only = 0;

/* For `ibmpc' charset, characters 176 to 223 are use to draw boxes.
   If this variable is set, while getting out of `ibmpc', ASCII
   characters are selected so to approximate these boxes.  */
int ascii_graphics = 0;

/* The following charset name will be ignored, if given.  */
static const char *ignored_name = NULL;

/* Unabridged names of BEFORE and AFTER charsets, even if still aliases.
   These are used for naming the array in produced C code.  */
static const char *before_full_name;
static const char *after_full_name;

/* Ordinals of list, BEFORE and AFTER charset.  */
static CHARSET *list_charset;
static CHARSET *before_charset;
static CHARSET *after_charset;

/* Flag telling usage that we are decoding charsets.  */
int decoding_charset_flag = 0;

/* Tells how various passes will be interconnected.  */
enum sequence_strategy
  {
    STRATEGY_UNDECIDED,		/* sequencing strategy is undecided yet */
    SEQUENCE_WITH_FILES,	/* do not fork, use intermediate files */
    SEQUENCE_WITH_POPEN,	/* use `popen(3)' to fork processes */
    SEQUENCE_WITH_PIPE		/* fork processes connected with `pipe(2)' */
  };
enum sequence_strategy sequence_strategy = STRATEGY_UNDECIDED;

/* Known single steps.  */

STEP single_step_array[MAX_SINGLE_STEPS];
int number_of_single_steps;	/* number of announced single steps */

const unsigned char *one_to_same; /* identity recoding */

CHARSET *rfc1345;		/* special RFC 1345 charset value */

/* Array stating the sequence of conversions.  */
const STEP *sequence[MAX_SEQUENCE];
int length_of_sequence;

/* Signal handling.  */

/*-----------------.
| Signal handler.  |
`-----------------*/

static jmp_buf signal_label;	/* where to jump when signal received */
static int interrupted = 0;	/* set when some signal has been received */

static RETSIGTYPE
signal_handler (int number)
{
  interrupted = 1;
  signal (number, signal_handler);
}

/*------------------------------------------------------------------------.
| Prepare to handle signals, intercept willingful requests for stopping.  |
`------------------------------------------------------------------------*/

static void
setup_signals (void)
{
  signal (SIGPIPE, signal_handler);
#if 0
  signal (SIGINT, signal_handler);
  signal (SIGTERM, signal_handler);
#endif
}

/* Quality handling.  */

/*---------------------------------------.
| Return a string describing a quality.	 |
`---------------------------------------*/

const char *
quality_to_string (QUALITY quality)
{
  switch (quality)
    {
    default:
      abort ();

    case REVERSIBLE:
      return "reversible";

    case ONE_TO_ONE:
      return "one to one";

    case MANY_TO_ONE:
      return "many to one";

    case ONE_TO_MANY:
      return "one to many";

    case MANY_TO_MANY:
      return "many to many";
    }
}

/*-------------------------------------------------------------------------.
| Return the quality of a step obtained by merging two others steps, given |
| their respective qualities FIRST and SECOND.				   |
`-------------------------------------------------------------------------*/

QUALITY 
merge_qualities (QUALITY first, QUALITY second)
{
  switch (first)
    {
    default:
      abort ();
      
    case REVERSIBLE:
      return second;

    case ONE_TO_ONE:
      switch (second)
	{
	case REVERSIBLE:
	case ONE_TO_ONE:
	  return ONE_TO_ONE;

	case MANY_TO_ONE:
	case ONE_TO_MANY:
	case MANY_TO_MANY:
	  return second;
	}

    case MANY_TO_ONE:
      switch (second)
	{
	case REVERSIBLE:
	case ONE_TO_ONE:
	case MANY_TO_ONE:
	  return MANY_TO_ONE;

	case ONE_TO_MANY:
	case MANY_TO_MANY:
	  return MANY_TO_MANY;
	}

    case ONE_TO_MANY:
      switch (second)
	{
	case REVERSIBLE:
	case ONE_TO_ONE:
	case ONE_TO_MANY:
	  return ONE_TO_MANY;

	case MANY_TO_ONE:
	case MANY_TO_MANY:
	  return MANY_TO_MANY;
	}

    case MANY_TO_MANY:
      return MANY_TO_MANY;
    }
}

/* Charset handling.  */

/*----------------------------------------------------.
| Decode the BEFORE:AFTER argument, given in STRING.  |
`----------------------------------------------------*/

static void
decode_before_after (const char *string)
{
  char *before;
  char *after;
  char *in;
  char *out;

  /* Split the BEFORE:AFTER keyword at the colon.  A backslash can escape
     a colon in both charsets.  */

  before = xstrdup (string);
  after = NULL;
  out = before;

  for (in = before; *in; in++)
    if (*in == ':' && !after)
      {
	*out++ = '\0';
	after = out;
      }
    else
      {
	if (*in == '\\' && *(in + 1))
	  in++;
	*out++ = *in;
      }
  *out = '\0';

  if (!after)
    usage (EXIT_FAILURE);

  /* Decode both charsets.  */

  before_full_name = clean_charset_name (before);
  before_charset = find_charset (before_full_name);

  after_full_name = clean_charset_name (after);
  after_charset = find_charset (after_full_name);

  /* Free the work area.  */

  free (before);
}

/* Single step handling.  */

/*-------------------------------------------------------------------------.
| Allocate and initialize a new single step, save for the before and after |
| charsets and quality.							   |
`-------------------------------------------------------------------------*/

static STEP *
new_single_step (void)
{
  STEP *step;

  if (number_of_single_steps == MAX_SINGLE_STEPS)
    error (EXIT_FAILURE, 0, "MAX_SINGLE_STEPS is too small");

  step = single_step_array + number_of_single_steps++;
  step->init_recode = NULL;
  step->file_recode = NULL;
  step->one_to_one = NULL;
  step->one_to_many = NULL;

  return step;
}
  
/*------------------------------------------------------------------------.
| Create and initialize a new single step for recoding between START_NAME |
| and GOAL_NAME, which are given as strings, give it a recoding QUALITY,  |
| also saving an INIT_RECODE and a FILE_RECODE functions.		  |
`------------------------------------------------------------------------*/

/* If not __STDC__, do not write prototypes for functionnal parameters,
   because ansi2knr does not clean them.  */

#if __STDC__
void
declare_step (const char *before_name, const char *after_name, QUALITY quality,
	      void (*init_recode) (STEP *),
	      int (*file_recode) (const STEP *, FILE *, FILE *))
#else
void
declare_step (const char *before_name, const char *after_name, QUALITY quality,
	      void (*init_recode) (),
	      int (*file_recode) ())
#endif
{
  STEP *step;

  step = new_single_step ();
  step->before = find_charset (before_name);
  step->after = find_charset (after_name);
  step->quality = quality;
  step->init_recode = init_recode;
  step->file_recode = file_recode;
}

/*------------------------------------------------------------------.
| Create a one to one table which is the inverse of the given one.  |
`------------------------------------------------------------------*/

unsigned char *
invert_table (const unsigned char *table)
{
  unsigned char flag[256];
  unsigned char *result;
  int table_error;
  int counter;

  result = (unsigned char *) xmalloc (256);
  memset (flag, 0, 256);
  table_error = 0;

  for (counter = 0; counter < 256; counter++)
    {
      if (flag[table[counter]])
	{
	  error (0, 0, "Codes %3d and %3d both recode to %3d",
		 result[table[counter]], counter, table[counter]);
	  table_error = 1;
	}
      else
	{
	  result[table[counter]] = counter;
	  flag[table[counter]] = 1;
	}
    }
  if (table_error)
    {
      for (counter = 0; counter < 256; counter++)
	if (!flag[counter])
	  error (0, 0, "No character recodes to %3d", counter);
      error (EXIT_FAILURE, 0, "Cannot invert given one-to-one table");
    }
  return result;
}

/*-------------------------------------------------------------------------.
| Complete a STEP descriptor by a constructed recoding array for 256 chars |
| and the adequate recoding routine.  If FIRST_HALF_IMPLIED is not zero,   |
| default the unconstrained characters of the first 128 to the identity	   |
| mapping.  Use an KNOWN_PAIRS array of NUMBER_OF_PAIRS constraints.  If   |
| REVERSE is not zero, use right_table instead of left_table.		   |
`-------------------------------------------------------------------------*/

void
complete_pairs (STEP *step, int first_half_implied,
		const KNOWN_PAIR *known_pairs, int number_of_pairs,
		int reverse)
{
  unsigned char left_flag[256];
  unsigned char right_flag[256];
  unsigned char left_table[256];
  unsigned char right_table[256];
  int table_error;

  unsigned char *flag;
  unsigned char *table;
  const char **table2;
  char *cursor;
  unsigned char left;
  unsigned char right;
  unsigned char search;
  int counter;
  int used;

  /* Init tables with zeroes.  */

  memset (left_flag, 0, 256);
  memset (right_flag, 0, 256);
  table_error = 0;

  /* Establish known data.  */

  for (counter = 0; counter < number_of_pairs; counter++)
    {
      left = known_pairs[counter].left;
      right = known_pairs[counter].right;

      /* Set one known correspondance.  */

      if (left_flag[left])
	{
	  if (!table_error)
	    {
	      error (0, 0, "Following diagnostics for `%s' to `%s'",
		     step->before->name, step->after->name);
	      table_error = 1;
	    }
	  error (0, 0, "Pair no. %d: { %3d, %3d } conflicts with { %3d, %3d }",
		 counter, left, right, left, left_table[left]);
	}
      else if (right_flag[right])
	{
	  if (!table_error)
	    {
	      error (0, 0, "Following diagnostics for `%s' to `%s'",
		     step->before->name, step->after->name);
	      table_error = 1;
	    }
	  error (0, 0, "Pair no. %d: { %3d, %3d } conflicts with { %3d, %3d }",
		 counter, left, right, right_table[right], right);
	}
      else
	{
	  left_flag[left] = 1;
	  left_table[left] = right;
	  right_flag[right] = 1;
	  right_table[right] = left;
	}
    }

  /* Set all the implied correspondances.  */

  if (first_half_implied)
    for (counter = 0; counter < 128; counter++)
      if (!left_flag[counter] && !right_flag[counter])
	{
	  left_flag[counter] = 1;
	  left_table[counter] = counter;
	  right_flag[counter] = 1;
	  right_table[counter] = counter;
	}

  if (strict_mapping)
    {

      /* If the recoding is strict, prepare a one to many table, each
	 entry being NULL or a string of a single character.  */

      /* Select the proper table.  */

      if (reverse)
	{
	  flag = right_flag;
	  table = right_table;
	}
      else
	{
	  flag = left_flag;
	  table = left_table;
	}

      /* Allocate everything in one blow, so it will be freed likewise.  */

      used = 0;
      for (counter = 0; counter < 256; counter++)
	if (flag[counter])
	  used++;

      table2 = (const char **) xmalloc (256 * sizeof (char *) + 2 * used);
      cursor = (char *) (table2 + 256);

      /* Construct the table and the strings in parallel.  */

      for (counter = 0; counter < 256; counter++)
	if (flag[counter])
	  {
	    table2[counter] = cursor;
	    *cursor++ = table[counter];
	    *cursor++ = '\0';
	  }
	else
	  table2[counter] = NULL;

      /* Save a one to many recoding table.  */

      step->file_recode = file_one_to_many;
      step->one_to_many = table2;
    }
  else
    {

      /* If the recoding is not strict, compute a reversible one to one
	 table.  */

      if (table_error)
	error (EXIT_FAILURE, 0,
	       "Cannot complete table from set of known pairs");

      /* Close the table with small permutation cycles.  */

      for (counter = 0; counter < 256; counter++)
	if (!right_flag[counter])
	  {
	    search = counter;
	    while (left_flag[search])
	      search = left_table[search];
	    left_flag[search] = 1;
	    left_table[search] = counter;
	    right_flag[counter] = 1;
	    right_table[counter] = search;
	  }

      /* Save a copy of the proper table.  */

      step->file_recode = file_one_to_one;
      table = (unsigned char *) xmalloc (256);
      memcpy (table, reverse ? right_table : left_table, 256);
      step->one_to_one = table;
    }
}

/*----------------------------------------.
| Initialize all collected single steps.  |
`----------------------------------------*/

void
register_all_modules (void)
{
  STEP *step;
  int counter;
  unsigned char *table;

  table = (unsigned char *) xmalloc (256);
  for (counter = 0; counter < 256; counter++)
    table[counter] = counter;
  one_to_same = table;

  prepare_charset_initialization ();
  number_of_single_steps = 0;

  rfc1345 = find_charset ("RFC 1345");
  declare_alias (".", "RFC 1345");
  declare_alias ("ASCII", "ANSI_X3.4-1968");
  declare_alias ("BS", "ASCII-BS");
  declare_alias ("Latin-1", "ISO_8859-1:1987");

  /* Needed for compatibility with recode version 3.2.  */
  declare_alias ("lat1", "Latin-1");

#include "initstep.h"

  for (step = single_step_array;
       step < single_step_array + number_of_single_steps;
       step++)

    if (step->file_recode == file_one_to_one
	&& step->one_to_one == one_to_same)

      step->conversion_cost = 0;

    else
      switch (step->quality)
	{
	case REVERSIBLE:
	  step->conversion_cost = 2;
	  break;

	case ONE_TO_ONE:
	  step->conversion_cost = 30;
	  break;

	case MANY_TO_ONE:
	  step->conversion_cost = 10;
	  break;

	case ONE_TO_MANY:
	  step->conversion_cost = 40;
	  break;

	case MANY_TO_MANY:
	  step->conversion_cost = 50;
	  break;
	}

  /* For all RFC 1345 participating steps, halve the cost since they
     come in pair.  */

  for (counter = 0; counter < number_of_single_steps; counter++)
    if (single_step_array[counter].before == rfc1345
	|| single_step_array[counter].after == rfc1345)
      single_step_array[counter].conversion_cost /= 2;
}

/*-------------------------------------------------------------------------.
| Produce a C include file representing the recoding, on standard output.  |
`-------------------------------------------------------------------------*/

static void
output_header_file (void)
{
  const STEP *step;		/* step being analysed */
  int column;			/* column counter */
  char *name;			/* constructed name */
  char *cursor;			/* cursor in constructed name */
  const char *cursor2;		/* cursor to study strings */
  int counter;			/* general purpose counter */

  /* This function is called only when the recoding sequence contains a
     single step, so it is safe to use sequence[0] for the step.  */

  step = sequence[0];

  /* Print the header of the header file.  */

  printf ("/* Conversion table from `%s' charset to `%s' charset.\n",
	  before_full_name, after_full_name);
  printf ("   Generated mechanically by GNU %s %s.\n", PRODUCT, VERSION);
  printf ("\n");
  switch (sequence[0]->quality)
    {
    case REVERSIBLE:
      printf ("   The recoding should be reversible.\n");
      break;

    case ONE_TO_ONE:
      printf ("   The recoding might not be reversible.\n");
      break;

    case MANY_TO_ONE:
      printf ("   Programming is needed to handle multichar input.\n");
      break;

    case ONE_TO_MANY:
      printf ("   Each input char transforms into an output string.\n");
      break;

    case MANY_TO_MANY:
      printf ("   Each input char transforms into an output string,\n");
      printf ("   programming is needed to handle multichar input.\n");
      break;
    }
  printf ("*/\n");
  printf ("\n");

  /* Construct the name of the resulting table.  */

  if (header_name)
    name = xstrdup (header_name);
  else
    {
      name = (char *) xmalloc (strlen (before_full_name) + sizeof "_to_"
			       + strlen (after_full_name));
      strcpy (name, before_full_name);
      strcat (name, "_to_");
      strcat (name, after_full_name);
    }

  /* Ensure the table name contains only valid characters for a C
     identifier.  */

  for (cursor = name; *cursor; cursor++)
    if (*cursor != '_'
	&& (*cursor < 'a' || *cursor > 'z')
	&& (*cursor < 'A' || *cursor > 'Z')
	&& (*cursor < '0' || *cursor > '9'))
      *cursor = '_';

  /* Produce the recoding table in the correct format.  */

  if (step->one_to_one)
    {

      /* Produce a one to one recoding table.  */

      printf ("unsigned char const %s[256] =\n", name);
      printf ("  {\n");
      for (counter = 0; counter < 256; counter++)
	{
	  printf ("%s%3d,", counter % 8 == 0 ? "    " : " ",
		   step->one_to_one[counter]);
	  if (counter % 8 == 7)
	    printf ("\t/* %3d - %3d */\n", counter - 7, counter);
	}
      printf ("  };\n");
    }

  else if (step->one_to_many)
    {

      /* Produce a one to many recoding table.  */

      printf ("const char *%s[256] =\n", name);
      printf ("  {\n");
      for (counter = 0; counter < 256; counter++)
	{
	  printf ("    ");
	  column = 4;
	  if (step->one_to_many[counter])
	    {
	      printf ("\"");
	      column++;
	      for (cursor2 = step->one_to_many[counter]; *cursor2; cursor2++)
		switch (*cursor2)
		  {
		  case ' ':
		    printf (" ");
		    column++;
		    break;

		  case '\b':
		    printf ("\\b");
		    column += 2;
		    break;

		  case '\t':
		    printf ("\\t");
		    column += 2;
		    break;

		  case '\n':
		    printf ("\\n");
		    column += 2;
		    break;

		  case '"':
		    printf ("\\\"");
		    column += 2;
		    break;

		  case '\\':
		    printf ("\\\\");
		    column += 2;
		    break;

		  default:
		    if (isprint (*cursor2))
		      {
			printf ("%c", *cursor2);
			column++;
		      }
		    else
		      {
			printf ("\\%0.3o", *(const unsigned char *) cursor2);
			column += 4;
		      }
		  }
	      printf ("\"");
	      column++;
	    }
	  else
	    {
	      printf ("0");
	      column++;
	    }
	  printf (",");
	  column++;
	  while (column < 32)
	    {
	      printf ("\t");
	      column += 8 - column % 8;
	    }
	  printf ("/* %3d */\n", counter);
	}
      printf ("  };\n");
    }

  else
    error (EXIT_FAILURE, 0, "No table to print");

  free (name);
}

/* Double step handling (for RFC 1345).  */

/*-----------------------------------------------------------------------.
| Associate a double TABLE with charset NAME, part of the RFC 1345 fully |
| connected set.  Each entry in table uses SIZE characters.		 |
`-----------------------------------------------------------------------*/

void
declare_double_step (DOUBLE_TABLE *table, const char *name, int size)
{
  CHARSET *charset;
  STEP *step;

  charset = find_charset (name);
  charset->table = table;
  charset->size = size;

  step = new_single_step ();
  step->before = charset;
  step->after = rfc1345;
  step->quality = strict_mapping ? ONE_TO_MANY : REVERSIBLE;
  step->init_recode = NULL;
  step->file_recode = NULL;

  step = new_single_step ();
  step->before = rfc1345;
  step->after = charset;
  step->quality = strict_mapping ? ONE_TO_MANY : REVERSIBLE;
  step->init_recode = NULL;
  step->file_recode = NULL;
}

/*---------------------------------------------------------------.
| Order two struct item's lexicographically of their key value.	 |
`---------------------------------------------------------------*/

struct item
  {
    const char *key;		/* RFC 1345 short mnemonic name */
    int code;			/* corresponding charset code (0..255) */
  };

static int
compare_struct_item (const void *void_first, const void *void_second)
{
  return strcmp (((const struct item *) void_first)->key,
		 ((const struct item *) void_second)->key);
}

/*------------------------------------------------------------------------.
| Complete the initialization of a double step which just has been merged |
| into a single STEP.  Establish known pairings by comparing keys between |
| the before and after charsets.					  |
`------------------------------------------------------------------------*/

void
init_recode_rfc1345 (STEP *step)
{
  struct side
    {
      CHARSET *charset;		/* charset */
      struct item item[256];	/* array of binding items */
      int number_of_items;	/* number of binding items in array */
    };

  char pool[2 * 256 * 6];	/* character pool */
  DOUBLE_TABLE *table;		/* RFC 1345 table */
  struct side side_array[2];	/* information for each side */
  struct side *side;		/* cursor into side_array */
  int reversed;			/* if both sides reversed */
  const char *in;		/* cursor in double table strings */
  char *out;			/* cursor in character pool */
  int code;			/* character code */
  int row_counter;		/* double table row counter */
  int position_counter;		/* double table column counter */
  int counter;			/* counter for characters */
  struct item *item_cursor;	/* cursor in arrays of binding items */
  struct item *left;		/* left binding items cursor */
  struct item *left_limit;	/* limit value for left */
  struct item *right;		/* right binding items cursor */
  struct item *right_limit;	/* limit value for right */
  KNOWN_PAIR pair_array[256];	/* obtained pairings */
  KNOWN_PAIR *pair_cursor;	/* cursor in array of pairings */
  int value;			/* result of lexicographical comparison */

  /* For ensuring reversibility, known pairs should be computed the same
     way regardless of the direction of recoding.  This canonalization is
     ensured through the charset values, which are increasing along the
     initialization order.  This should also reflect the charset order in
     rfc1345.txt.  */

  if (step->before < step->after)
    {
      side_array[0].charset = step->before;
      side_array[1].charset = step->after;
      reversed = 0;
    }
  else
    {
      side_array[0].charset = step->after;
      side_array[1].charset = step->before;
      reversed = 1;
    }

  out = pool;
  for (side = side_array; side < side_array + 2; side++)
    {

      /* Move the string values out of the double table, while constructing
	 the array of binding items for the charset.  */

      table = side->charset->table;
      item_cursor = side->item;
      code = 0;

      for (row_counter = 0; row_counter < 8; row_counter++)
	if (in = (*table)[row_counter], in)
	  for (position_counter = 0; position_counter < 32; position_counter++)
	    {
	      if (*in == ' ')
		in += side->charset->size;
	      else
		{

		  /* Establish a new binding item.  */

		  item_cursor->code = code;
		  item_cursor->key = out;
		  item_cursor++;

		  /* Copy out the value to the character pool, and terminate it
		     with a NULL.  */

		  for (counter = 0; counter < side->charset->size; counter++)
		    if (*in == ' ')
		      in++;
		    else
		      *out++ = *in++;
		  *out++ = '\0';
		}
	      code++;
	    }
	else
	  code += 32;

      side->number_of_items = item_cursor - side->item;
    }

  /* Sort both arrays of binding items into lexicographical order.  The
     taken time, which is O(n.log(n)), is gained back when the further
     pairing is completed in a time which is linear instead of quadratic.  */

  qsort (side_array[0].item, side_array[0].number_of_items,
	 sizeof (struct item), compare_struct_item);
  qsort (side_array[1].item, side_array[1].number_of_items,
	 sizeof (struct item), compare_struct_item);

  /* Scan both arrays of binding items simultaneously, saving as pairs
     those codes having the same key.  */

  left = side_array[0].item;
  left_limit = left + side_array[0].number_of_items;
  right = side_array[1].item;
  right_limit = right + side_array[1].number_of_items;
  pair_cursor = pair_array;

  while (left < left_limit && right < right_limit)
    {
      value = strcmp (left->key, right->key);
      if (value < 0)
	left++;
      else if (value > 0)
	right++;
      else
	{
	  pair_cursor->left = (left++)->code;
	  pair_cursor->right = (right++)->code;
	  pair_cursor++;
	}
    }

  /* Complete the recoding table out of this.  */

  complete_pairs (step, 0, pair_array, pair_cursor - pair_array, reversed);
}

/* Step sequence handling.  */

#define UNREACHABLE	30000		/* No way for this conversion */

/*-------------------------------------------------------.
| Explain what recoding step sequence has been planned.	 |
`-------------------------------------------------------*/

static void
echo_sequence (void)
{
  const char *last;		/* last name printed */
  const char *name;		/* name being printed */
  QUALITY quality;		/* cumulative quality */
  int counter;			/* index into sequence */

  if (length_of_sequence < 0)
    fprintf (stderr, "UNACHIEVABLE recoding!\n");
  else if (length_of_sequence == 0)
    fprintf (stderr, "Mere copy for the trivial recoding\n");
  else
    {
      quality = REVERSIBLE;
      last = NULL;
      for (counter = 0; counter < length_of_sequence; counter++)
	{
	  name = sequence[counter]->before->name;
	  if (counter == 0)
	    fprintf (stderr, "%s", name);
	  else if (name != last)
	    fprintf (stderr, "/%s", name);

	  name = sequence[counter]->after->name;
	  fprintf (stderr, " -> %s", name);

	  quality = merge_qualities (quality, sequence[counter]->quality);
	  last = name;
	}
      fprintf (stderr, " (%s)\n", quality_to_string (quality));
    }
}

/*----------------------------------------------------------.
| Find a sequence of single steps to achieve a conversion.  |
`----------------------------------------------------------*/

static void
find_sequence (CHARSET *before, CHARSET *after)
{
  struct search
    {
      STEP *step;		/* step who will bring us nearer to after */
      int cost;			/* cost from here through after */
    };
  struct search *search_array;	/* critical path search tree */
  struct search *search;	/* item in search_array for charset */
  STEP *step;			/* cursor in possible single_steps */
  int cost;			/* cost under consideration */
  int modified;			/* != 0 if modified since last iteration */
  CHARSET *charset;		/* charset while reconstructing sequence */

  search_array
    = (struct search *) xmalloc (number_of_charsets * sizeof (struct search));

  /* Initialize the search for an economical route, looking our way
     backward from the after towards the before.  */

  for (search = search_array;
       search < search_array + number_of_charsets;
       search++)
    {
      search->step = NULL;
      search->cost = UNREACHABLE;
    }
  search_array[after - charset_array].cost = 0;

  modified = 1;
  while (modified)
    {
      modified = 0;
      for (step = single_step_array;
	   step < single_step_array + number_of_single_steps;
	   step++)
	if (!step->before->ignore)
	  {
	    cost = search_array[step->after - charset_array].cost;
	    if (cost != UNREACHABLE)
	      {
		cost += step->conversion_cost;
		search = search_array + (step->before - charset_array);
		if (cost < search->cost)
		  {
		    search->step = step;
		    search->cost = cost;
		    modified = 1;
		  }
	      }
	  }
    }

  if (search_array[before - charset_array].cost == UNREACHABLE)
    {
      
      /* If no path has been found, return with a negative length.  */

      length_of_sequence = -1;
    }
  else
    {

      /* Save the retained best path in the sequence array.  While doing so,
	 simplify out any single step which merely copies.  Also execute the
	 delayed initialization for those steps which registered one.  */

      length_of_sequence = 0;
      for (charset = before; charset != after; charset = step->after)
	{
	  step = search_array[charset - charset_array].step;
	  if (step->file_recode != file_one_to_one
	      || step->one_to_one != one_to_same)
	    {
	      if (length_of_sequence == MAX_SEQUENCE)
		error (EXIT_FAILURE, 0, "MAX_SEQUENCE is too small");
	      sequence[length_of_sequence++] = step;
	      if (step->init_recode)
		{
		  (*step->init_recode) (step);
		  step->init_recode = NULL;
		}
	    }
	}
    }

  /* Tell what has been decided, for the user.  */

  if (verbose_option)
    echo_sequence ();

  free (search_array);
}

/*-------------------------------------------------------------------------.
| Return 0 if STEP cannot be easily simplified.  Return 1 if it is an all  |
| table driven ONE_TO_ONE recoding.  Return 2 if it is an all table driven |
| ONE_TO_MANY recoding.  If making headers, accept more easily that a step |
| is simplifyable, just looking at tables and ignoring preset functions.   |
`-------------------------------------------------------------------------*/

static int
simplify_type (const STEP *step)
{
  if (step->one_to_one
      && (make_header_mode || step->file_recode == file_one_to_one))
    return 1;

  if (step->one_to_many
      && (make_header_mode || step->file_recode == file_one_to_many))
    return 2;

  return 0;
}

/*---------------------------------------------------------------------.
| Optimize a sequence of single steps by creating new single steps, if |
| this can be done by merging adjacent steps which are simple enough.  |
`---------------------------------------------------------------------*/

static void
simplify_sequence (void)
{
  int saved_steps;		/* number of saved steps */
  int in;			/* ordinal of next studied sequence step */
  int out;			/* ordinal of next output sequence step */
  STEP *step;			/* new single step being constructed */
  unsigned char *accum;		/* one_to_one accumulated recoding */
  const char **string;		/* one_to_many recoding */
  unsigned char temp[256];	/* temporary value for accum array */
  int counter;			/* all purpose counter */

  saved_steps = 0;

  /* See if there are some RFC 1345 double steps to merge.  */

  in = 0;
  out = 0;

  while (in < length_of_sequence)
    if (in < length_of_sequence - 1
	&& sequence[in]->after == rfc1345
	&& sequence[in+1]->before == rfc1345)
      {

	/* Produce a new single step for the double step.  */

	step = new_single_step ();
	step->before = sequence[in]->before;
	step->after = sequence[in+1]->after;
	step->quality = merge_qualities (sequence[in]->quality,
					 sequence[in+1]->quality);
	step->init_recode = init_recode_rfc1345;
	step->file_recode = file_one_to_one;

	in += 2;
	saved_steps++;

	/* Initialize the new single step, so it can be later merged with
	   others.  */

	(*step->init_recode) (step);
	step->init_recode = NULL;

	sequence[out++] = step;
      }
    else if (sequence[in]->before == rfc1345)
      error (EXIT_FAILURE, 0, "You may not explicitely recode from RFC 1345");
    else if (sequence[in]->after == rfc1345)
      {

	/* Produce a new single step for the double step.  */

	step = new_single_step ();
	step->before = sequence[in]->before;
	step->after = sequence[in]->after;
	step->quality = ONE_TO_MANY;

	in++;

	/* Initialize the new single step, so it can be later merged with
	   others.  */

	init_table_for_rfc1345 (step);
	step->file_recode = file_one_to_many;

	sequence[out++] = step;
      }
    else
      sequence[out++] = sequence[in++];

  length_of_sequence = out;

  /* Recopy the sequence array over itself, while merging subsequences of
     one or more consecutive one-to-one recodings, including an optional
     final one-to-many recoding.  */

  in = 0;
  out = 0;
  while (in < length_of_sequence)
    if (in < length_of_sequence - 1
	&& simplify_type (sequence[in]) == 1
	&& simplify_type (sequence[in+1]) > 0)
      {

	/* Construct a new single step, and initialize a cumulative
	   one-to-one recoding with the identity permutation.  */

	accum = (unsigned char *) xmalloc (256);
	for (counter = 0; counter < 256; counter++)
	  accum[counter] = counter;

	step = new_single_step ();
	step->before = sequence[in]->before;
	step->quality = REVERSIBLE;

	/* Merge in all consecutive one-to-one recodings.  */

	while (in < length_of_sequence && simplify_type (sequence[in]) == 1)
	  {
	    for (counter = 0; counter < 256; counter++)
	      temp[counter] = sequence[in]->one_to_one[accum[counter]];
	    for (counter = 0; counter < 256; counter++)
	      accum[counter] = temp[counter];
	    step->after = sequence[in]->after;
	    step->quality
	      = merge_qualities (step->quality, sequence[in]->quality);
	    in++;
	    saved_steps++;
	  }

	/* Check for a possible one-to-many recoding.  */

	if (in < length_of_sequence && simplify_type (sequence[in]) == 2)
	  {

	    /* Merge in the one-to-many recoding, and make the new single
	       step be a one-to-many recoding.  */

	    string = (const char **) xmalloc (256 * sizeof (char *));
	    for (counter = 0; counter < 256; counter++)
	      string[counter] = sequence[in]->one_to_many[accum[counter]];
	    free (accum);
	    step->one_to_many = string;
	    step->file_recode = file_one_to_many;
	    step->after = sequence[in]->after;
	    step->quality
	      = merge_qualities (step->quality, sequence[in]->quality);
	    in++;
	    saved_steps++;
	  }
	else
	  {

	    /* Make the new single step be a one-to-one recoding.  */

	    step->one_to_one = accum;
	    step->file_recode = file_one_to_one;
	  }

	/* Save the newly created step.  */

	sequence[out++] = step;
      }
    else
      sequence[out++] = sequence[in++];

  length_of_sequence = out;

  /* Delete a remaining single step, if it happens to be the identity
     one-to-one recoding.  */

  if (length_of_sequence == 1 && simplify_type (sequence[0]) == 1
      && memcmp (sequence[0]->one_to_one, one_to_same, 256) == 0)
    {
      length_of_sequence = 0;
      saved_steps++;
    }

  /* Save the resulting sequence length, and tell the user if something
     changed.  */

  if (saved_steps > 0 && verbose_option)
    {
      fprintf (stderr, "Simplified to: ");
      echo_sequence ();
    }
}

/* Recoding execution control.  */

/*--------------.
| Copy a file.  |
`--------------*/

/* This should be rewritten to be a lot faster.  */

static void
file_copy (FILE *input_file, FILE *output_file)
{
  int input_char;		/* current character */

  while (input_char = getc (input_file), input_char != EOF)
    putc (input_char, output_file);
}

/*----------------------------------------------------------------------.
| Recode a file using a one-to-one recoding table.  Returns zero if the |
| recoding has been interrupted because of a reversibility problem.     |
`----------------------------------------------------------------------*/

int
file_one_to_one (const STEP *step, FILE *input_file, FILE *output_file)
{
  const unsigned char *table;	/* conversion table */
  int input_char;		/* current character */

  table = step->one_to_one;
  while (input_char = getc (input_file), input_char != EOF)
    putc (table[input_char], output_file);

  return 1;
}

/*-----------------------------------------------------------------------.
| Recode a file using a one-to-many recoding table.  Returns zero if the |
| recoding has been found to be non reversible.				 |
`-----------------------------------------------------------------------*/

int
file_one_to_many (const STEP *step, FILE *input_file, FILE *output_file)
{
  int reversible;		/* reversibility of recoding */
  const char *const *table;	/* conversion table */
  int input_char;		/* current character */
  const char *output_string;	/* translated characters */

  /* Copy the file through the one to many recoding table.  */

  reversible = 1;
  table = step->one_to_many;
  while (input_char = getc (input_file), input_char != EOF)
    if (output_string = table[input_char], output_string)
      while (*output_string)
	{
	  putc (*output_string, output_file);
	  output_string++;
	}
    else
      reversible = 0;

  return reversible;
}

/*-------------------------------------------------------------------------.
| Execute the conversion sequence, using several passes with two	   |
| alternating intermediate files.  This routine assumes at least one	   |
| needed recoding step.  Returns zero if the recoding has been found to be |
| non-reversible.							   |
`-------------------------------------------------------------------------*/

static int
execute_pass_sequence (const char *input_name, const char *output_name)
{
  int sequence_index;		/* index into sequence */
  const STEP *step;		/* pointer to step */
  FILE *input_file;		/* input file to recoding step */
  FILE *output_file;		/* output file from recoding step */
  int reversible;		/* reversibility of recoding */
#ifdef USE_TMPNAM
  char *temp_input_name;	/* step input file name */
  char *temp_output_name;	/* step output file name */
  char temp_name_1[L_tmpnam];	/* one temporary file name */
  char temp_name_2[L_tmpnam];	/* another temporary file name */
  char *exchange_temp;		/* for exchanging temporary names */
#endif

#ifdef USE_TMPNAM

  /* Choose names for intermediate files.  Use "" for delaying them.  */

#ifdef MSDOS_or_OS2
  strcpy (temp_name_1, "recodex1.tmp");
  strcpy (temp_name_2, "recodex2.tmp");
#else
  temp_name_1[0] = '\0';
  temp_name_2[0] = '\0';
#endif
  temp_input_name = temp_name_1;
  temp_output_name = temp_name_2;

#endif /* USE_TMPNAM */

  /* Execute one pass for each step of the sequence.  */

  reversible = 1;
  for (sequence_index = 0;
       sequence_index < length_of_sequence;
       sequence_index++)
    {

      /* Select the input file for this step.  */

      if (sequence_index == 0)
	{
	  if (!input_name)
	    input_file = stdin;
	  else if (input_file = fopen (input_name, "r"), input_file == NULL)
	    error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
		   __FILE__, __LINE__, input_name);
	}
      else
	{
#ifdef USE_TMPNAM
	  if (input_file = fopen (temp_input_name, "r"), input_file == NULL)
	    error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
		   __FILE__, __LINE__, temp_input_name);
#endif

#ifdef USE_TMPFILE
	  rewind (input_file);
#endif
	}

      /* Select the output file for this step.  */

      if (sequence_index == length_of_sequence - 1)
	{
	  if (!output_name)
	    output_file = stdout;
	  else if (output_file = fopen (output_name, "w"), output_file == NULL)
	    error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
		   __FILE__, __LINE__, output_name);
	}
      else
	{
#ifdef USE_TMPNAM
#ifdef MSDOS_or_OS2
	  if (*temp_output_name == '\0')
	    tmpnam (temp_output_name);
#endif
	  if (output_file = fopen (temp_output_name, "w"), output_file == NULL)
	    error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
		   __FILE__, __LINE__, temp_output_name);
#endif

#ifdef USE_TMPFILE
	  if (output_file = tmpfile (), output_file == NULL)
	    error (EXIT_FAILURE, errno, "%s:%d: tmpfile ()",
		   __FILE__, __LINE__);
#endif
	}

      /* Execute one recoding step.  */

      step = sequence[sequence_index];
      if (!(*step->file_recode) (step, input_file, output_file))
	reversible = 0;

      /* Close the input file, unlink it if it was temporary.  */

      if (sequence_index == 0)
	{
	  if (input_name)
	    fclose (input_file);
	}
      else
	{
	  fclose (input_file);
#ifdef USE_TMPNAM
	  unlink (temp_input_name);
#endif
	}

      /* Close the output file and prepare for subsequent step.  */

      if (sequence_index == length_of_sequence - 1)
	{
	  if (output_name)
	    fclose (output_file);
	}
      else
	{
#ifdef USE_TMPNAM
	  fclose (output_file);

	  exchange_temp = temp_input_name;
	  temp_input_name = temp_output_name;
	  temp_output_name = exchange_temp;
#endif

#ifdef USE_TMPFILE
	  input_file = output_file;
#endif
	}
    }
  return reversible;
}

/*-------------------------------------------------------------------------.
| Execute the conversion sequence, using a chain of invocations of the	   |
| program through popen.  This routine assumes that more than one recoding |
| step is needed.  Returns zero if the recoding has been found to be	   |
| non-reversible.							   |
`-------------------------------------------------------------------------*/

#ifdef HAVE_POPEN

static int
execute_popen_sequence (const char *input_name, const char *output_name)
{
  const STEP *step;		/* current step */
  FILE *input_file;		/* input file to recoding step */
  FILE *output_file;		/* output file from recoding step */
  char popen_command[80];	/* to receive command string */
  int reversible;		/* reversibility of recoding */

  /* Construct a `recode' command for all recoding steps but the first.  */

  strcpy (popen_command, program_path);
  if (diaeresis_char)
    strcat (popen_command, " -c");
  if (diacritics_only)
    strcat (popen_command, " -d");
  if (force_option)
    strcat (popen_command, " -f");
  strcat (popen_command, " -o -q ");
  strcat (popen_command, clean_charset_name (sequence[1]->before->name));
  strcat (popen_command, ":");
  strcat (popen_command,
	  clean_charset_name (sequence[length_of_sequence-1]->after->name));
  if (output_name)
    {
      strcat (popen_command, " >");
      strcat (popen_command, output_name);
    }

  /* Execute the first recoding step.  */

  if (!input_name)
    input_file = stdin;
  else if (input_file = fopen (input_name, "r"), input_file == NULL)
    error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
	   __FILE__, __LINE__, input_name);

  if (output_file = popen (popen_command, "w"), output_file == NULL)
    error (EXIT_FAILURE, errno, "%s:%d: popen (%s)",
	   __FILE__, __LINE__, popen_command);

  step = sequence[0];
  reversible = (*step->file_recode) (step, input_file, output_file);

  if (input_name)
    fclose (input_file);

  /* Return the proper status.  */

  if (pclose (output_file) != 0)
    reversible = 0;

  return reversible;
}

#endif /* HAVE_POPEN */

#ifdef HAVE_PIPE

#ifndef HAVE_DUP2

/*------------------------------------------------------------------------.
| Duplicate the OLD_FD file descriptor into NEW_FD, closing NEW_FD first  |
| if it is used.  This implementation presumes both OLD_FD and NEW_FD are |
| valid file descriptors.						  |
`------------------------------------------------------------------------*/

/* Overall idea taken from GNU Emacs 18.55 dup2 (), in src/sysdep.c.  */

#include <sys/fcntl.h>

#ifndef F_DUPFD

static int
dup2_recursive (int old_fd, int new_fd)
{
  int fd;

  /* Attempt to dup OLD_FD to NEW_FD.  If not successful, call dup2
     recursively, filling the file descriptor table until NEW_FD is
     reached.  Then close all the spurious file descriptors we created.  */

  if (fd = dup (old_fd) && fd != new_fd)
    if (fd < 0 || dup2_recursive (old_fd, new_fd) < 0 || close (fd) < 0)
      return 0;

  return 1;
}

#endif /* not F_DUPFD */

static int
dup2 (int old_fd, int new_fd)
{

  /* If OLD_FD or NEW_FD were not valid file descriptors, dup2 should
     ideally return -1 with errno set to EBADF.  This is not checked.  */

  if (old_fd != new_fd)
    {
      close (new_fd);

#ifdef F_DUPFD
      if (fcntl (old_fd, F_DUPFD, new_fd) != new_fd)
	return -1;
#else
      if (!dup2_recursive (old_fd, new_fd))
	return -1;
#endif
    }
  return new_fd;
}

#endif /* not HAVE_DUP2 */

/*-------------------------------------------------------------------------.
| Execute the conversion sequence, forking the program many times for all  |
| elementary steps, interconnecting them with pipes.  This routine assumes |
| that more than one recoding step is needed.  Returns zero if the	   |
| recoding has been found to be non-reversible.				   |
`-------------------------------------------------------------------------*/

#if 1

/* This is no good.  The main process might open too many files for one
   thing.  All of it should work almost the same way
   execute_popen_sequence does, creating children from left to right,
   instead of all children to a single parent right to left.  */

static int
execute_pipe_sequence (const char *input_name, const char *output_name)
{
  int sequence_index;		/* index into sequence */
  const STEP *step;		/* pointer into single_steps */

  FILE *input_file;		/* input file to recoding step */
  FILE *output_file;		/* output file from recoding step */
  int pipe_pair[2];		/* pair of file descriptors for a pipe */
  int child_process;		/* child process number, zero if child */
  int wait_status;		/* status returned by wait() */
  int reversible;		/* reversibility of recoding */

  /* Prepare the final output file.  */

  if (!output_name)
    output_file = stdout;
  else if (output_file = fopen (output_name, "w"), output_file == NULL)
    error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
	   __FILE__, __LINE__, output_name);

  /* Create all subprocesses, from the last to the first, and
     interconnect them.  */

  reversible = 1;
  for (sequence_index = length_of_sequence - 1;
       sequence_index > 0;
       sequence_index--)
    {
      if (pipe (pipe_pair) < 0)
	error (EXIT_FAILURE, errno, "%s:%d: pipe ()",
	       __FILE__, __LINE__);
      if (child_process = fork (), child_process < 0)
	error (EXIT_FAILURE, errno, "%s:%d: fork ()",
	       __FILE__, __LINE__);
      if (child_process == 0)
	{

          /* The child executes its recoding step, reading from the pipe
             and writing to the current output file; then it exits.  */

	  if (close (pipe_pair[1]) < 0)
	    error (EXIT_FAILURE, errno, "%s:%d: close ()",
		   __FILE__, __LINE__);
	  if (input_file = fdopen (pipe_pair[0], "r"), input_file == NULL)
	    error (EXIT_FAILURE, errno, "%s:%d: fdopen ()",
		   __FILE__, __LINE__);

	  step = sequence[sequence_index];
	  if (!(*step->file_recode) (step, input_file, output_file))
	    reversible = 0;

	  fclose (input_file);
	  if (sequence_index < length_of_sequence - 1 || output_name)
	    fclose (output_file);

	  exit (reversible ? EXIT_SUCCESS : EXIT_FAILURE);
	}
      else
	{

          /* The parent redirects the current output file to the pipe.  */

	  if (dup2 (pipe_pair[1], fileno (output_file)) < 0)
	    error (EXIT_FAILURE, errno, "%s:%d: dup2 ()",
		   __FILE__, __LINE__);
	  if (close (pipe_pair[0]) < 0)
	    error (EXIT_FAILURE, errno, "%s:%d: close ()",
		   __FILE__, __LINE__);
	  if (close (pipe_pair[1]) < 0)
	    error (EXIT_FAILURE, errno, "%s:%d: close ()",
		   __FILE__, __LINE__);
	}
    }

  /* All the children are created, blocked on read.  Now, feed the whole
     chain of processes with the output of the first recoding step.  */

  if (!input_name)
    input_file = stdin;
  else if (input_file = fopen (input_name, "r"), input_file == NULL)
    error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
	   __FILE__, __LINE__, input_name);

  step = sequence[0];
  if (!(*step->file_recode) (step, input_file, output_file))
    reversible = 0;

  if (input_name)
    fclose (input_file);

  fclose (output_file);

  /* Wait on all children, mainly to avoid synchronisation problems on
     output file contents, but also to reduce the number of zombie
     processes in case the user recodes many files at once.  */

  while (wait (&wait_status) > 0)
    {

      /* Diagnose and abort on any abnormally terminating child.  */

      if ((wait_status & ~(~0 << 8)) != 0
	  && (wait_status & ~(~0 << 8)) != SIGPIPE)
	error (0, 0, "Child process wait status is 0x%0.2x",
	       wait_status);

      /* Check for a non-zero exit from the terminating child.  */

      if (wait_status & ~(~0 << 16))
	reversible = 0;
    }

  if (interrupted)
    reversible = 0;

  return reversible;
}

#else

static int
execute_pipe_sequence (const char *input_name, const char *output_name)
{
  int sequence_index;		/* index into sequence */
  const STEP *step;		/* pointer into single_steps */

  FILE *input_file;		/* input file to recoding step */
  FILE *output_file;		/* output file from recoding step */
  int pipe_pair[2];		/* pair of file descriptors for a pipe */
  int child_process;		/* child process number, zero if child */
  int wait_status;		/* status returned by wait() */
  int reversible;		/* reversibility of recoding */

  /* Prepare the final files.  */

  if (!input_name)
    input_file = stdin;
  else if (input_file = fopen (input_name, "r"), input_file == NULL)
    error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
	   __FILE__, __LINE__, input_name);

  if (!output_name)
    output_file = stdout;
  else if (output_file = fopen (output_name, "w"), output_file == NULL)
    error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
	   __FILE__, __LINE__, output_name);

  /* Create all subprocesses, from the first to the last, and
     interconnect them.  */

  reversible = 1;
  for (sequence_index = 0;
       sequence_index < length_of_sequence - 1;
       sequence_index++)
    {
      if (pipe (pipe_pair) < 0)
	error (EXIT_FAILURE, errno, "%s:%d: pipe ()",
	       __FILE__, __LINE__);
      if (child_process = fork (), child_process < 0)
	error (EXIT_FAILURE, errno, "%s:%d: fork ()",
	       __FILE__, __LINE__);
      if (child_process == 0)
	{
          /* The child executes its recoding step, reading from the pipe
             and writing to the current output file; then it exits.  */

	  if (close (pipe_pair[1]) < 0)
	    error (EXIT_FAILURE, errno, "%s:%d: close ()",
		   __FILE__, __LINE__);
	  if (input_file = fdopen (pipe_pair[0], "r"), input_file == NULL)
	    error (EXIT_FAILURE, errno, "%s:%d: fdopen ()",
		   __FILE__, __LINE__);

	  step = sequence[sequence_index];
	  if (!(*step->file_recode) (step, input_file, output_file))
	    reversible = 0;

	  fclose (input_file);
	  if (sequence_index < length_of_sequence - 1 || output_name)
	    fclose (output_file);

	  exit (reversible ? EXIT_SUCCESS : EXIT_FAILURE);
	}
      else
	{

          /* The parent redirects the current output file to the pipe.  */

	  if (dup2 (pipe_pair[1], fileno (output_file)) < 0)
	    error (EXIT_FAILURE, errno, "%s:%d: dup2 ()",
		   __FILE__, __LINE__);
	  if (close (pipe_pair[0]) < 0)
	    error (EXIT_FAILURE, errno, "%s:%d: close ()",
		   __FILE__, __LINE__);
	  if (close (pipe_pair[1]) < 0)
	    error (EXIT_FAILURE, errno, "%s:%d: close ()",
		   __FILE__, __LINE__);
	}
      else
	{
	  break;
	}
    }

  /* All processes execute the following common code, each with its proper
     value for SEQUENCE_INDEX, CHILD_PROCESS, etc.  */

  /* All the children are created, blocked on read.  Now, feed the whole
     chain of processes with the output of the first recoding step.  */

  if (!input_name)
    input_file = stdin;
  else if (input_file = fopen (input_name, "r"), input_file == NULL)
    error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
	   __FILE__, __LINE__, input_name);

  step = sequence[0];
  if (!(*step->file_recode) (step, input_file, output_file))
    reversible = 0;

  if (input_name)
    fclose (input_file);

  fclose (output_file);

  /* Wait on all children, mainly to avoid synchronisation problems on
     output file contents, but also to reduce the number of zombie
     processes in case the user recodes many files at once.  */

  while (wait (&wait_status) > 0)
    {

      /* Diagnose and abort on any abnormally terminating child.  */

      if ((wait_status & ~(~0 << 8)) != 0
	  && (wait_status & ~(~0 << 8)) != SIGPIPE)
	error (0, 0, "Child process wait status is 0x%0.2x",
	       wait_status);

      /* Check for a non-zero exit from the terminating child.  */

      if (wait_status & ~(~0 << 16))
	reversible = 0;
    }

  if (interrupted)
    reversible = 0;

  return reversible;
}

#endif

#endif /* HAVE_PIPE */

/*-------------------------------------------------------------------------.
| Execute the conversion sequence, using the selected strategy whenever	   |
| more than one conversion step is needed.  If no conversion are needed,   |
| merely copy the input onto the output.  Returns zero if the recoding has |
| been found to be non-reversible.					   |
`-------------------------------------------------------------------------*/

/* If some sequencing strategies are missing, this routine automatically
   uses fallback strategies.  */

static int
execute_sequence (const char *input_name, const char *output_name)
{
  FILE *input_file;		/* input file to recoding step */
  FILE *output_file;		/* output file from recoding step */
  const STEP *step;		/* current step */
  int reversible;		/* reversibility of recoding */

#ifdef MSDOS_or_OS2
  if (!input_name)
    setmode (fileno (stdin), O_BINARY);
  if (!output_name)
    setmode (fileno (stdout), O_BINARY);
#ifdef __EMX__
  {
    extern int _fmode_bin;
    _fmode_bin = 1;
  }
#else
  _fmode = O_BINARY;
#endif
#endif

  if (verbose_option && input_name)
    {
      fprintf (stderr, "Recoding %s...", input_name);
      fflush (stderr);
    }

  if (length_of_sequence > 1)
    switch (sequence_strategy)
      {
      case STRATEGY_UNDECIDED:
	error (EXIT_FAILURE, 0, "Internal error - strategy undecided");

      case SEQUENCE_WITH_PIPE:
#ifdef HAVE_PIPE
	reversible = execute_pipe_sequence (input_name, output_name);
	break;
#endif

      case SEQUENCE_WITH_POPEN:
#ifdef HAVE_POPEN
	reversible = execute_popen_sequence (input_name, output_name);
	break;
#endif

      case SEQUENCE_WITH_FILES:
	reversible = execute_pass_sequence (input_name, output_name);
	break;
      }
  else
    {

      /* This is a single-step recoding a mere copy.  Do it.  */

      if (!input_name)
	input_file = stdin;
      else if (input_file = fopen (input_name, "r"), input_file == NULL)
	error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
	       __FILE__, __LINE__, input_name);

      if (!output_name)
	output_file = stdout;
      else if (output_file = fopen (output_name, "w"), output_file == NULL)
	error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
	       __FILE__, __LINE__, output_name);

      if (length_of_sequence == 1)
	{
	  step = sequence[0];
	  reversible = (*step->file_recode) (step, input_file, output_file);
	}
      else
	{
	  file_copy (input_file, output_file);
	  reversible = 1;
	}

      if (input_name)
	fclose (input_file);
      if (output_name)
	fclose (output_file);
    }

  if (verbose_option && input_name)
    {
      fprintf (stderr, " done\n");
      fflush (stderr);
    }
  return reversible;
}

/* Some special option handling.  */

/*-----------------------------------------------------------------.
| Print a truncated charset name, with a guaranteed space at end.  |
`-----------------------------------------------------------------*/

static void
print_truncated_charset_name (const char *name)
{
  char copy[15];

  if ((int) strlen (name) > 14)
    {
      memcpy (copy, name, 14);
      copy[14] = '\0';
      name = copy;
    }
  printf ("%-14s ", name);
}

/*----------------------------------------------------------------------.
| Find all possible sequences and report about them.  If CHARSET is not |
| NULL, limit the study to those recoding starting or ending with the   |
| given charset.						        |
`----------------------------------------------------------------------*/

static void
perform_auto_check_one (CHARSET *before, CHARSET *after)
{
  int saved_length_of_sequence;
  int saved_number_of_single_steps;
  QUALITY quality;
  const char *quality_string;
  int counter;
  STEP *step;

  /* Study what we can do.  */

  find_sequence (before, after);
  if (length_of_sequence < 0)
    {
      if (!ignored_name)
	{
	  print_truncated_charset_name (before->name);
	  print_truncated_charset_name (after->name);
	  printf ("  UNACHIEVABLE\n");
	}
    }
  else
    {

      /* Compute the recoding quality.  */

      quality = REVERSIBLE;
      for (counter = 0; counter < length_of_sequence; counter++)
	quality = merge_qualities (quality,
				   sequence[counter]->quality);
      quality_string = quality_to_string (quality);

      /* Study what optimization can do.  */

      saved_length_of_sequence = length_of_sequence;
      saved_number_of_single_steps = number_of_single_steps;

      simplify_sequence ();

      /* Check and report codes which should be aliases.  */

      if (length_of_sequence == 0)
	quality_string = "ONE to SAME";

      /* Make the report.  */

      print_truncated_charset_name (before->name);
      print_truncated_charset_name (after->name);
      printf ("  %-16s", quality_string);
      printf ("steps: %d", saved_length_of_sequence);

      if (length_of_sequence != saved_length_of_sequence)
	printf (", %d saved by merging",
		saved_length_of_sequence - length_of_sequence);
      printf ("\n");

      /* Unregister and clean up the merged steps.  */

      while (number_of_single_steps > saved_number_of_single_steps)
	{
	  number_of_single_steps--;
	  step = single_step_array + number_of_single_steps;
	  if (step->one_to_one)
	    free ((void *) step->one_to_one);
	  if (step->one_to_many)
	    free ((void *) step->one_to_many);
	}
    }
}

static void
perform_auto_check (CHARSET *charset)
{
  CHARSET *before;
  CHARSET *after;

  if (charset->ignore)
    error (EXIT_FAILURE, 0, "Cannot auto check the ignored charset");
  if (charset == rfc1345)
    error (EXIT_FAILURE, 0, "Cannot auto check on %s", charset->name);

  if (charset)
    {
      for (before = charset_array;
	   before < charset_array + number_of_charsets;
	   before++)
	if (!before->ignore && before != charset && before != rfc1345)

	    perform_auto_check_one (before, charset);

      for (after = charset_array;
	   after < charset_array + number_of_charsets;
	   after++)
	if (!after->ignore && after != charset && after != rfc1345)

	    perform_auto_check_one (charset, after);
    }
  else
    for (before = charset_array;
	 before < charset_array + number_of_charsets;
	 before++)
      if (!before->ignore && before != rfc1345)

	for (after = charset_array;
	     after < charset_array + number_of_charsets;
	     after++)
	  if (!after->ignore && after != before && after != rfc1345)

	    perform_auto_check_one (before, after);
}

/* Main program.  */

/*-----------------------------------------------.
| Explain how to use the program, then get out.	 |
`-----------------------------------------------*/

void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, "Try `%s %s' for more information.\n", program_name,
	     decoding_charset_flag ? "--list" : "--help");
  else
    {
      printf ("GNU %s %s\n", PRODUCT, VERSION);
      printf ("\
\n\
Usage: %s [OPTION]... [CHARSET]\n", program_name);
      fputs ("\
Mandatory or optional arguments to long options are mandatory or optional\n\
for short options too.\n\
\n\
  -C, --copyright       display Copyright and copying conditions\n\
  -a, --auto-check      report about some or all recoding paths, then exit\n\
  -l, --list[=FORMAT]   list one or all known charsets\n\
  -k, --known=PAIRS     restrict charsets according to known PAIRS list\n\
      --help            display this help and exit\n\
      --version         output version information and exit\n\
\n\
FORMAT is a word among decimal, octal, hexadecimal or full (which may be\n\
abbreviated to one of `dohf'), it defaults to just the canonical name.\n\
With -k, possible before charsets are listed for the given after CHARSET,\n\
both being RFC1345 charsets, with PAIRS of the form `BEF1:AFT1,BEF2:AFT2,...\n\
and BEFs and AFTs being codes.  All codes are given as decimal numbers.\n",
	      stdout);
      fputs ("\
Option -l with no FORMAT nor CHARSET list all charsets, also see the Texinfo\n\
documentation.  My preferred charsets are (each user has preferences):\n\
\n\
  ascii-bs   ASCII (7-bit), using backspace to apply diacritics\n\
  ibmpc      IBM-PC 8-bit characters, with proper newlines\n\
  latex      LaTeX coding of foreign and diacriticized characters\n\
  latin1     ISO Latin-1 8-bit extension of ASCII\n\
  texte      Easy French convention for transmitting email messages\n",
	     stdout);
      printf ("\
\n\
Usage: %s [OPTION]... [BEFORE]:[AFTER] [FILE]...\n", program_name);
      fputs ("\
\n\
  -c, --colons            use colons instead of double quotes for diaeresis\n\
  -d, --diacritics        limit conversion to diacritics or alike for LaTeX\n\
  -f, --force             force recodings even if they are not reversible\n\
                          (BEWARE: in this version, -f is always selected)\n\
  -g, --graphics          approximate IBMPC rulers by ASCII graphics\n\
  -h, --header[=NAME]     write C code with table NAME on stdout, then exit\n\
  -i, --sequence=files    use intermediate files for sequencing passes\n",
	     stdout);

#ifdef HAVE_POPEN
      fputs ("\
  -o, --sequence=popen    use popen machinery for sequencing passes\n",
	     stdout);
#else
      fputs ("\
  -o, --sequence=popen    same as -i (on this system)\n",
	     stdout);
#endif

#ifdef HAVE_PIPE
      fputs ("\
  -p, --sequence=pipe     use pipe machinery for sequencing passes\n",
	     stdout);
#else
      fputs ("\
  -p, --sequence=pipe     same as -o (on this system)\n",
	     stdout);
#endif

      fputs ("\
  -q, --quiet, --silent   inhibit messages about irreversible recodings\n\
  -s, --strict            use strict mappings, even loose characters\n\
  -t, --touch             touch the recoded files after replacement\n\
  -v, --verbose           explain sequence of steps and report progress\n\
  -x, --ignore=CHARSET    ignore CHARSET while choosing a recoding path\n\
\n\
If none of -i, -o and -p are given, presume -p if no FILE, else -i.\n\
Each FILE is recoded over itself, destroying the original.  If no\n\
FILE is specified, then act as a filter and recode stdin to stdout.\n",
	     stdout);
      printf ("\
BEFORE and AFTER both default to `%s' when needed.\n", DEFAULT_CHARSET);
    }
  exit (status);
}

/*----------------------------------------------------------------------.
| Main program.  Decode ARGC arguments passed through the ARGV array of |
| strings, then launch execution.				        |
`----------------------------------------------------------------------*/

/* Long options equivalences.  */
static const struct option long_options[] =
{
  {"auto-check", no_argument, NULL, 'a'},
  {"colons", no_argument, NULL, 'c'},
  {"copyright", no_argument, NULL, 'C'},
  {"diacritics", no_argument, NULL, 'd'},
  {"force", no_argument, NULL, 'f'},
  {"header", optional_argument, NULL, 'h'},
  {"help", no_argument, &show_help, 1},
  {"ignore", required_argument, NULL, 'x'},
  {"list", optional_argument, NULL, 'l'},
  {"quiet", no_argument, NULL, 'q'},
  {"sequence", required_argument, NULL, '\n'},
  {"silent", no_argument, NULL, 'q'},
  {"strict", no_argument, NULL, 's'},
  {"touch", no_argument, NULL, 't'},
  {"verbose", no_argument, NULL, 'v'},
  {"version", no_argument, &show_version, 1},
  {0, 0, 0, 0},
};

static const char *const format_strings[] =
  {
    "decimal",
    "octal",
    "hexadecimal",
    "full",
    NULL,
  };

static const char *const sequence_strings[] =
  {
    "files",
    "popen",
    "pipe",
    NULL,
  };

int
main (int argc, char *const *argv)
{
  extern int optind;		/* index of argument */
  int option_char;		/* option character */
  int reversible;		/* reversibility of all recodings */
  const char *input_name;	/* input file name */
  char output_name[200];	/* output file name */
  FILE *file;			/* file to check or stat */
#ifdef MSDOS
  struct ftime stamp_stat;	/* input file time stamps */
#else /* not MSDOS */
  struct stat stamp_stat;	/* input file time stamps */
  time_t stamp_utime[2];	/* recoded file time stamps */
#endif /* not MSDOS */
  char *cursor;			/* all purpose cursor */

  /* Decode command options.  */

  program_path = argv[0];
  program_name = strrchr (program_path, '/');
  program_name = program_name ? program_name + 1 : program_path;

  if (argc == 1)
    usage (EXIT_SUCCESS);

  while (option_char = getopt_long (argc, argv, "aCcdfgh::ik:l::opqstvx:",
				    long_options, NULL),
	 option_char != EOF)
    switch (option_char)
      {
      default:
	usage (EXIT_FAILURE);

      case '\0':
	break;

      case '\n':
	switch (argmatch (optarg, sequence_strings))
	  {
	  case -2:
	    error (0, 0, "Ambiguous sequence `%s'", optarg);
	    usage (EXIT_FAILURE);

	  case -1:
	    error (0, 0, "Unknown sequence `%s'", optarg);
	    usage (EXIT_FAILURE);

	  case 0:
	    sequence_strategy = SEQUENCE_WITH_FILES;
	    break;

	  case 1:
	    sequence_strategy = SEQUENCE_WITH_POPEN;
	    break;

	  case 2:
	    sequence_strategy = SEQUENCE_WITH_PIPE;
	    break;
	  }
	break;

      case 'a':
	auto_check_mode = 1;
	break;

      case 'C':
	fprintf (stderr, "%s", copyright_string);
	exit (EXIT_SUCCESS);

      case 'c':
	diaeresis_char = ':';
	break;

      case 'd':
	diacritics_only = 1;
	break;

      case 'f':
	force_option = 1;
	break;

      case 'g':
	ascii_graphics = 1;
	break;

      case 'h':
	make_header_mode = 1;
	header_name = optarg;
	break;

      case 'i':
	sequence_strategy = SEQUENCE_WITH_FILES;
	break;

      case 'k':
	decode_known_pairs (optarg);
	show_restricted_charsets = 1;
	break;

      case 'l':
	show_charsets = 1;
	if (optarg)
	  switch (argmatch (optarg, format_strings))
	    {
	    case -2:
	      error (0, 0, "Ambiguous format `%s'", optarg);
	      usage (EXIT_FAILURE);

	    case -1:
	      error (0, 0, "Unknown format `%s'", optarg);
	      usage (EXIT_FAILURE);

	    case 0:
	      list_format = DECIMAL_FORMAT;
	      break;

	    case 1:
	      list_format = OCTAL_FORMAT;
	      break;

	    case 2:
	      list_format = HEXADECIMAL_FORMAT;
	      break;

	    case 3:
	      list_format = FULL_FORMAT;
	      break;
	    }
	break;

      case 'o':
	sequence_strategy = SEQUENCE_WITH_POPEN;
	break;

      case 'p':
	sequence_strategy = SEQUENCE_WITH_PIPE;
	break;

      case 'q':
	quiet_mode = 1;
	break;

      case 's':
	strict_mapping = 1;
	break;

      case 't':
	touch_option = 1;
	break;

      case 'v':
	verbose_option = 1;
	break;

      case 'x':
	ignored_name = optarg;
	break;
      }

  if (ascii_graphics && strict_mapping)
    {
      error (0, 0, "Currently, -s is ignored when -g is selected");
      strict_mapping = 0;
    }

  if (ascii_graphics || strict_mapping)
    force_option = 1;

  /* Process trivial options.  */

  if (show_version)
    {
      printf ("GNU %s %s\n", PRODUCT, VERSION);
      exit (EXIT_SUCCESS);
    }

  if (show_help)
    usage (EXIT_SUCCESS);

  /* Register all modules, then set the ignored charset.  */

  register_all_modules ();
  make_argmatch_array ();

  if (ignored_name)
    find_charset (clean_charset_name (ignored_name))->ignore = 1;

  /* Process auto check option.  */

  if (auto_check_mode)
    {
      if (optind + 1 < argc)
	usage (EXIT_FAILURE);

      /* Accept a possible charset.  */

      if (optind < argc)
	perform_auto_check (find_charset (clean_charset_name (argv[optind])));
      else
	perform_auto_check (NULL);
      exit (EXIT_SUCCESS);
    }

  /* Process charset listing options.  */

  if (show_charsets || show_restricted_charsets)
    {
      if (optind + 1 < argc)
	usage (EXIT_FAILURE);

      /* Select a possible charset and a default format.  */

      if (optind < argc)
	list_charset = find_charset (clean_charset_name (argv[optind]));
      else if (list_format != NO_FORMAT || show_restricted_charsets)
	list_charset = find_charset (clean_charset_name (NULL));
      else
	list_charset = NULL;

      /* List the charset(s) appropriately.  */
      
      if (show_restricted_charsets)
	list_all_charsets (list_charset);
      else if (list_charset)
	if (list_format == FULL_FORMAT)
	  list_full_charset (list_charset);
	else
	  list_concise_charset (list_charset);
      else
	list_all_charsets (NULL);

      /* Then get out.  */

      exit (EXIT_SUCCESS);
    }

  /* Decode the BEFORE:AFTER argument.  */

  if (optind + 1 > argc)
    usage (EXIT_FAILURE);

  decode_before_after (argv[optind++]);

  /* Establish the sequence of recoding steps.  */

  length_of_sequence = 0;
  find_sequence (before_charset, after_charset);
  if (length_of_sequence < 0)
    error (EXIT_FAILURE, 0, "No way to recode from %s to %s",
	   before_charset->name, after_charset->name);

  simplify_sequence ();

  /* If we merely want C code, do it and get out.  */

  if (make_header_mode)
    {
      if (length_of_sequence == 0)
	error (EXIT_FAILURE, 0, "Recoding is trivial, not worth a table");
      if (length_of_sequence > 1
	  || !(sequence[0]->one_to_one || sequence[0]->one_to_many))
	error (EXIT_FAILURE, 0, "Recoding is too complex for a mere table");

      output_header_file ();
      exit (EXIT_SUCCESS);
    }

  /* If the recoding might be not reversible, do not proceed further without
     --force option.  */

  /* DO IT!  */

  /* If there is no input file, act as a filter.  Else, recode all files
     over themselves.  */

  setup_signals ();

  if (optind < argc)
    {

      /* When reading and writing files, unless the user selected otherwise,
	 avoid forking and use intermediate files.  */

      if (sequence_strategy == STRATEGY_UNDECIDED)
	sequence_strategy = SEQUENCE_WITH_FILES;

      /* In case files are recoded over themselves and there is no
         recoding step at all, do not even try to touch the files.  */

      reversible = 1;
      if (length_of_sequence > 0)

	/* Process files, one at a time.  */

	for (; optind < argc; optind++)
	  {
	    input_name = argv[optind];

	    /* Check if the file can be read and rewritten.  */

	    if (file = fopen (input_name, "r+"), file == NULL)
	      error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
		     __FILE__, __LINE__, input_name);

	    /* Save the input file time stamp.  */

	    if (!touch_option)
	      {
#ifdef MSDOS
		getftime (fileno (file), &stamp_stat);
#else
		fstat (fileno (file), &stamp_stat);
#endif
	      }

	    fclose (file);

	    /* Choose an output file in the same directory.  */

#ifdef MSDOS_or_OS2

	    strcpy (output_name, input_name);
	    for (cursor = output_name + strlen (output_name);
		 cursor > output_name && cursor[-1] != '/'
		 && cursor[-1] != '\\' && cursor[-1] != ':';
		 cursor--)
	      ;
	    strcpy (cursor, "recodeXX.TMP");

#else /* not MSDOS_or_OS2 */

	    strcpy (output_name, input_name);
	    for (cursor = output_name + strlen (output_name);
		 cursor > output_name && cursor[-1] != '/';
		 cursor--)
	      ;
	    sprintf (cursor, "rec%d.tmp", getpid ());

#endif /* not MSDOS_or_OS2 */

	    /* Recode the file.  */

	    if (!execute_sequence (input_name, output_name))
	      {
		reversible = 0;
		if (!quiet_mode)
		  error (0, 0, "%s: Recoding is not reversible", input_name);
	      }

	    /* Move the new file over the original.  */

	    if (unlink (input_name) < 0)
	      error (EXIT_FAILURE, errno, "%s:%d: unlink (%s)",
		     __FILE__, __LINE__, input_name);
#ifdef HAVE_RENAME
	    if (rename (output_name, input_name) < 0)
	      error (EXIT_FAILURE, errno, "%s:%d: rename (%s, %s)",
		     __FILE__, __LINE__, output_name, input_name);
#else
	    if (link (output_name, input_name) < 0)
	      error (EXIT_FAILURE, errno, "%s:%d: link (%s, %s)",
		     __FILE__, __LINE__, output_name, input_name);
	    if (unlink (output_name) < 0)
	      error (EXIT_FAILURE, errno, "%s:%d: unlink (%s)",
		     __FILE__, __LINE__, output_name);
#endif

	    /* Adjust the time stamp for the new file.  */

	    if (!touch_option)
	      {
#ifdef MSDOS
		file = fopen (input_name, "r");
		if (file == NULL)
		  error (EXIT_FAILURE, errno, "%s:%d: fopen (%s)",
			 __FILE__, __LINE__, input_name);
		setftime (fileno (file), &stamp_stat);
		fclose (file);
#else
		stamp_utime[0] = stamp_stat.st_atime;
		stamp_utime[1] = stamp_stat.st_mtime;
		utime (input_name, stamp_utime);
#endif
	      }
	  }
    }
  else
    {

      /* When reading stdin and writing stdout, unless the user selected
         otherwise, fork processes interconnected with pipes.  */

      if (sequence_strategy == STRATEGY_UNDECIDED)
	sequence_strategy = SEQUENCE_WITH_PIPE;

      if (!execute_sequence (NULL, NULL))
	{
	  reversible = 0;
	  if (!quiet_mode)
	    error (0, 0, "Recoding is not reversible");
	}
    }

  /* Exit with an appropriate status.  */

  exit ((force_option || reversible) ? EXIT_SUCCESS : EXIT_FAILURE);
}
