/* Conversion of files between different charsets and surfaces.
   Copyright © 1990,92,93,94,96,97,98,99,00 Free Software Foundation, Inc.
   François Pinard <pinard@iro.umontreal.ca>, 1990.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
   Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "common.h"

#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "getopt.h"

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if HAVE_STRUCT_UTIMBUF
# if HAVE_UTIME_H
#  include <utime.h>
# endif
#else

struct utimbuf
{
  time_t actime;
  time_t modtime;
};

#endif

#include <setjmp.h>

#if OS2
# include <io.h>
# include <fcntl.h>
#endif

/* Variables.  */

/* The name this program was run with. */
const char *program_name;

/* If nonzero, display usage information and exit.  */
static int show_help = 0;

/* If nonzero, print the version on standard output and exit.  */
static int show_version = 0;

/* If true, show a list of one or all known charsets, then exit.  */
static bool show_symbols = false;

/* If true, dump all tables as a big C module.  */
static bool freeze_tables = false;

/* If true, check all charsets for subsets, then exit.  */
static bool find_subsets = false;

/* User provided list of charset restrictions (option -k).  */
static const char *charset_restrictions = NULL;

/* Indicates the format for showing only one charset.  */
enum recode_list_format list_format = RECODE_NO_FORMAT;

/* Table name in generated language code.  */
static const char *header_name;

/* Language to use for generated code.  */
static enum recode_programming_language header_language;

/* This option asks recode to report the progress while recoding many files
   in a single call.  */
static bool verbose_flag = false;

/* This option merely inhibits messages about non-reversible recodings, but
   it does not prevent recodings to be aborted or exit status to be set.  */
static bool quiet_flag = false;

/* If the recoding yields some problems in reversibility in some file, this
   file replacement is denied and it is left unrecoded or, if recode is used
   as a mere filter, the recoding is interrupted.  The following flag forces
   the recoding to run to completion and the replacement to be done even if
   the recoding is not reversible.  */
static bool force_flag = false;

/* When a file is recoded over itself, precautions are taken to move the
   timestamps of the original file into the recoded file, so to make the
   recoding the most transparent possible to make, and other tools.
   However, selecting the following option inhibit the timestamps handling,
   thus effectively `touching' the file.  */
static bool touch_option = false;

/* With strict mapping, all reversibility fallbacks get defeated.  */
static bool strict_mapping = false;

/* The following charset name will be ignored, if given.  */
static const char *ignored_name = NULL;

#if 0
/* Unabridged names of BEFORE and AFTER charsets, even if still aliases.
   These are used for naming the array in produced C code.  */
static const char *before_full_name;
static const char *after_full_name;
#endif

/* Ordinals of list, BEFORE and AFTER charset.  */
static RECODE_SYMBOL list_charset;
#if 0
static RECODE_SYMBOL before_charset;
static RECODE_SYMBOL after_charset;
#endif

/* Flag telling usage that we are decoding charsets.  */
bool decoding_charset_flag = false;

/* Error handling.  */

/*-------------------------------------------------.
| Produce a message explaining the error in TASK.  |
`-------------------------------------------------*/

static const char *
task_perror (RECODE_CONST_TASK task)
{
  switch (task->error_so_far)
    {
    case RECODE_NO_ERROR:
      return _("No error");

    case RECODE_NOT_CANONICAL:
      return _("Non canonical input");

    case RECODE_AMBIGUOUS_OUTPUT:
      return _("Ambiguous output");

    case RECODE_UNTRANSLATABLE:
      return _("Untranslatable input");

    case RECODE_INVALID_INPUT:
      return _("Invalid input");

    case RECODE_SYSTEM_ERROR:
      return _("System detected problem");

    case RECODE_USER_ERROR:
      return _("Misuse of recoding library");

    default:
      return _("Internal recoding bug");
    }
}

/*-----------------.
| Signal handler.  |
`-----------------*/

#if FIXME
static jmp_buf signal_label;	/* where to jump when signal received */
#endif
static bool interrupted = 0;	/* set when some signal has been received */

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
#ifdef SIGPIPE
  signal (SIGPIPE, signal_handler);
#endif
#if 0
  signal (SIGINT, signal_handler);
  signal (SIGTERM, signal_handler);
#endif
}

/* Main control.  */

/*-----------------------------------.
| Prints a more detailed Copyright.  |
`-----------------------------------*/

static void
print_copyright (void)
{
  fputs (_("\
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
along with this program; if not, write to the Free Software Foundation,\n\
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.\n"),
	 stdout);
}

/*-----------------------------------------------.
| Explain how to use the program, then get out.	 |
`-----------------------------------------------*/

static void
usage (int status, bool list)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s %s' for more information.\n"), program_name,
	     list ? "--list" : "--help");
  else
    {
      fputs (_("\
Recode converts files between various character sets and surfaces.\n\
"),
	     stdout);
      printf (_("\
\n\
Usage: %s [OPTION]... [ [CHARSET] | REQUEST [FILE]... ]\n"), program_name);
      fputs (_("\
\n\
If a long option shows an argument as mandatory, then it is mandatory\n\
for the equivalent short option also.  Similarly for optional arguments.\n\
"),
	     stdout);
      fputs (_("\
\n\
Listings:\n\
  -l, --list[=FORMAT]        list one or all known charsets and aliases\n\
  -k, --known=PAIRS          restrict charsets according to known PAIRS list\n\
  -h, --header[=[LN/]NAME]   write table NAME on stdout using LN, then exit\n\
  -F, --freeze-tables        write out a C module holding all tables\n\
  -T, --find-subsets         report all charsets being subset of others\n\
  -C, --copyright            display Copyright and copying conditions\n\
      --help                 display this help and exit\n\
      --version              output version information and exit\n\
"),
	     stdout);
      fputs (_("\
\n\
Operation modes:\n\
  -v, --verbose           explain sequence of steps and report progress\n\
  -q, --quiet, --silent   inhibit messages about irreversible recodings\n\
  -f, --force             force recodings even when not reversible\n\
  -t, --touch             touch the recoded files after replacement\n\
  -i, --sequence=files    use intermediate files for sequencing passes\n\
      --sequence=memory   use memory buffers for sequencing passes\n\
"),
	     stdout);
#if HAVE_PIPE
      fputs (_("\
  -p, --sequence=pipe     use pipe machinery for sequencing passes\n\
"),
	     stdout);
#else
      fputs (_("\
  -p, --sequence=pipe     same as -i (on this system)\n\
"),
	     stdout);
#endif
      fputs (_("\
\n\
Fine tuning:\n\
  -s, --strict           use strict mappings, even loose characters\n\
  -d, --diacritics       convert only diacritics or alike for HTML/LaTeX\n\
  -S, --source[=LN]      limit recoding to strings and comments as for LN\n\
  -c, --colons           use colons instead of double quotes for diaeresis\n\
  -g, --graphics         approximate IBMPC rulers by ASCII graphics\n\
  -x, --ignore=CHARSET   ignore CHARSET while choosing a recoding path\n\
"),
	     stdout);
      fputs (_("\
\n\
Option -l with no FORMAT nor CHARSET list available charsets and surfaces.\n\
FORMAT is `decimal', `octal', `hexadecimal' or `full' (or one of `dohf').\n\
"),
	     stdout);
      fputs (_("\
Unless DEFAULT_CHARSET is set in environment, CHARSET defaults to the locale\n\
dependent encoding, determined by LC_ALL, LC_CTYPE, LANG.\n\
"),
	       stdout);
      fputs (_("\
With -k, possible before charsets are listed for the given after CHARSET,\n\
both being tabular charsets, with PAIRS of the form `BEF1:AFT1,BEF2:AFT2,...'\n\
and BEFs and AFTs being codes are given as decimal numbers.\n"),
	      stdout);
      fputs (_("\
LN is some language, it may be `c', `perl' or `po'; `c' is the default.\n"),
	     stdout);
      fputs (_("\
\n\
REQUEST is SUBREQUEST[,SUBREQUEST]...; SUBREQUEST is ENCODING[..ENCODING]...\n\
ENCODING is [CHARSET][/[SURFACE]]...; REQUEST often looks like BEFORE..AFTER,\n\
with BEFORE and AFTER being charsets.  An omitted CHARSET implies the usual\n\
charset; an omitted [/SURFACE]... means the implied surfaces for CHARSET; a /\n\
with an empty surface name means no surfaces at all.  See the manual.\n"),
	     stdout);
      fputs (_("\
\n\
If none of -i and -p are given, presume -p if no FILE, else -i.\n\
Each FILE is recoded over itself, destroying the original.  If no\n\
FILE is specified, then act as a filter and recode stdin to stdout.\n"),
	     stdout);
      fputs (_("\
\n\
Report bugs to <recode-bugs@iro.umontreal.ca>.\n"),
	       stdout);
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
  {"colons", no_argument, NULL, 'c'},
  {"copyright", no_argument, NULL, 'C'},
  {"diacritics", no_argument, NULL, 'd'},
  {"find-subsets", no_argument, NULL, 'T'},
  {"force", no_argument, NULL, 'f'},
  {"freeze-tables", no_argument, NULL, 'F'},
  {"header", optional_argument, NULL, 'h'},
  {"help", no_argument, &show_help, 1},
  {"ignore", required_argument, NULL, 'x'},
  {"known", required_argument, NULL, 'k'},
  {"list", optional_argument, NULL, 'l'},
  {"quiet", no_argument, NULL, 'q'},
  {"sequence", required_argument, NULL, '\n'},
  {"source", optional_argument, NULL, 'S'},
  {"silent", no_argument, NULL, 'q'},
  {"strict", no_argument, NULL, 's'},
  {"touch", no_argument, NULL, 't'},
  {"verbose", no_argument, NULL, 'v'},
  {"version", no_argument, &show_version, 1},
  {0, 0, 0, 0},
};

static const char *const format_strings[]
  = { "decimal", "octal", "hexadecimal", "full", NULL };

static const char *const language_strings[]
  = { "c", "perl", "po", NULL };

static const char *const sequence_strings[]
  = { "memory", "files", "pipe", NULL };

int
main (int argc, char *const *argv)
{
  int option_char;		/* option character */
  bool success = true;		/* reversibility of all recodings */

  static bool (*processor) PARAMS ((RECODE_TASK));
  struct recode_outer outer_option;
  struct recode_request request_option;
  struct recode_task task_option;

  RECODE_OUTER outer;
  RECODE_REQUEST request;

  program_name = argv[0];
  /* libtool creates a temporary executable whose names is prefixed with
     "lt-".  Remove this prefix here.  */
  if (strncmp (program_name, "lt-", 3) == 0)
    program_name += 3;

  setlocale (LC_ALL, "");
  textdomain (PACKAGE);

  /* Decode command options.  */

  if (argc == 1)
    usage (EXIT_SUCCESS, 0);

  processor = recode_perform_task;
  memset (&outer_option, 0, sizeof (struct recode_outer));
  memset (&request_option, 0, sizeof (struct recode_request));
  memset (&task_option, 0, sizeof (struct recode_task));

  request_option.diaeresis_char = '"';
  task_option.strategy = RECODE_STRATEGY_UNDECIDED;
  task_option.fail_level = RECODE_AMBIGUOUS_OUTPUT;
  task_option.abort_level = RECODE_AMBIGUOUS_OUTPUT;

  while (option_char = getopt_long (argc, argv, "CFS::Tcdfgh::ik:l::pqstvx:",
				    long_options, NULL),
	 option_char != EOF)
    switch (option_char)
      {
      default:
	usage (EXIT_FAILURE, 0);

      case NUL:
	break;

      case '\n':
	switch (argmatch (optarg, sequence_strings, NULL, 0))
	  {
	  case -2:
	    error (0, 0, _("Sequence `%s' is ambiguous"), optarg);
	    usage (EXIT_FAILURE, 0);

	  case -1:
	    error (0, 0, _("Sequence `%s' is unknown"), optarg);
	    usage (EXIT_FAILURE, 0);

	  case 0:
	    task_option.strategy = RECODE_SEQUENCE_IN_MEMORY;
	    break;

	  case 1:
	    task_option.strategy = RECODE_SEQUENCE_WITH_FILES;
	    break;

	  case 2:
	    task_option.strategy = RECODE_SEQUENCE_WITH_PIPE;
	    break;
	  }
	break;

      case 'C':
	print_copyright ();
	exit (EXIT_SUCCESS);

      case 'F':
	freeze_tables = true;
	break;

      case 'S':
	if (optarg)
	  switch (argmatch (optarg, language_strings, NULL, 0))
	    {
	    case -2:
	      error (0, 0, _("Language `%s' is ambiguous"), optarg);
	      usage (EXIT_FAILURE, 0);

	    default:		/* -1 */
	      error (0, 0, _("Language `%s' is unknown"), optarg);
	      usage (EXIT_FAILURE, 0);

	    case 0:
	      processor = transform_c_source;
	      break;

	    case 2:
	      processor = transform_po_source;
	      break;
	    }
	else
	  processor = transform_c_source;
	break;

      case 'T':
	find_subsets = true;
	break;

      case 'c':
	request_option.diaeresis_char = ':';
	break;

      case 'd':
	request_option.diacritics_only = true;
	break;

      case 'f':
	task_option.fail_level = RECODE_SYSTEM_ERROR;
	task_option.abort_level = RECODE_USER_ERROR;
	break;

      case 'g':
	request_option.ascii_graphics = true;
	break;

      case 'h':
	request_option.make_header_flag = true;
	header_name = optarg ? strrchr (optarg, '/') : NULL;
	if (header_name)
	  {
	    char *buffer;
	    unsigned counter;

	    header_name++;
	    buffer = (char *) xmalloc ((size_t) (header_name - optarg));
	    if (*header_name == NUL)
	      header_name = NULL;
	    for (counter = 0; optarg[counter] != '/'; counter++)
	      buffer[counter] = tolower (optarg[counter]);
	    buffer[counter] = NUL;
	    switch (argmatch (buffer, language_strings, NULL, 0))
	      {
	      case -2:
		error (0, 0, _("Language `%s' is ambiguous"), buffer);
		usage (EXIT_FAILURE, 0);

	      default:		/* -1 */
		error (0, 0, _("Language `%s' is unknown"), buffer);
		usage (EXIT_FAILURE, 0);

	      case 0:
		header_language = RECODE_LANGUAGE_C;
		break;

	      case 1:
		header_language = RECODE_LANGUAGE_PERL;
		break;
	      }
	    free (buffer);
	  }
	else
	  {
	    header_name = optarg;
	    header_language = RECODE_LANGUAGE_C;
	  }
	break;

      case 'i':
	task_option.strategy = RECODE_SEQUENCE_WITH_FILES;
	break;

      case 'k':
	charset_restrictions = optarg;
	break;

      case 'l':
	show_symbols = true;
	if (optarg)
	  switch (argmatch (optarg, format_strings, NULL, 0))
	    {
	    case -2:
	      error (0, 0, _("Format `%s' is ambiguous"), optarg);
	      usage (EXIT_FAILURE, 0);

	    case -1:
	      error (0, 0, _("Format `%s' is unknown"), optarg);
	      usage (EXIT_FAILURE, 0);

	    case 0:
	      list_format = RECODE_DECIMAL_FORMAT;
	      break;

	    case 1:
	      list_format = RECODE_OCTAL_FORMAT;
	      break;

	    case 2:
	      list_format = RECODE_HEXADECIMAL_FORMAT;
	      break;

	    case 3:
	      list_format = RECODE_FULL_FORMAT;
	      break;
	    }
	break;

      case 'p':
	task_option.strategy = RECODE_SEQUENCE_WITH_PIPE;
	break;

      case 'q':
	quiet_flag = true;
	break;

      case 's':
	strict_mapping = true;
	task_option.fail_level = RECODE_NOT_CANONICAL;
	task_option.abort_level = RECODE_NOT_CANONICAL;
	break;

      case 't':
	touch_option = true;
	break;

      case 'v':
	verbose_flag = true;
	request_option.verbose_flag = true;
	break;

      case 'x':
	ignored_name = optarg;
	break;
      }

  if (request_option.ascii_graphics)
    force_flag = true;

  /* Process trivial options.  */

  if (show_version)
    {
      printf ("%s %s\n", PACKAGE, VERSION);
      fputs (_("\
Written by Franc,ois Pinard <pinard@iro.umontreal.ca>.\n"),
	     stdout);
      fputs (_("\
\n\
Copyright (C) 1990, 92-94, 96, 97, 99, 08 Free Software Foundation, Inc.\n"),
	     stdout);
      fputs (_("\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"),
	     stdout);
      exit (EXIT_SUCCESS);
    }

  if (show_help)
    usage (EXIT_SUCCESS, 0);

  /* Register all modules and build internal tables.  */

  {
    unsigned flags = RECODE_AUTO_ABORT_FLAG;
    if (ignored_name && *ignored_name == ':')
      flags |= RECODE_NO_ICONV_FLAG;
    if (request_option.make_header_flag)
      flags |= RECODE_NO_ICONV_FLAG;
    outer = recode_new_outer (flags);
    if (!outer)
      abort ();
  }

  if (freeze_tables)
    {
      recode_freeze_tables (outer);
      exit (EXIT_SUCCESS);
    }

  /* Set strict mapping.  */

  if (strict_mapping)
    {
      RECODE_SINGLE single;

      for (single = outer->single_list; single; single = single->next)
	single->fallback_routine = NULL;
    }

  /* Set the ignored charset.  */

  if (ignored_name)
    {
      RECODE_ALIAS alias
	= find_alias (outer, ignored_name, ALIAS_FIND_AS_CHARSET);

      if (!alias)
	{
	  error (0, 0, _("Symbol `%s' is unknown"), ignored_name);
	  usage (EXIT_FAILURE, 1);
	}

      alias->symbol->ignore = true;
    }

  /* Process charset listing options.  */

  if (find_subsets)
    {
      if (find_and_report_subsets (outer))
	exit (EXIT_SUCCESS);
      else
	exit (EXIT_FAILURE);
    }

  if (show_symbols || charset_restrictions)
    {
      if (charset_restrictions)
	if (!decode_known_pairs (outer, charset_restrictions))
	  {
	    error (0, 0, "Could not understand `%s'", charset_restrictions);
	    usage (EXIT_FAILURE, 0);
	  }
      if (optind + 1 < argc)
	{
	  error (0, 0, "Argument `%s' is extraneous", argv[optind]);
	  usage (EXIT_FAILURE, 0);
	}

      /* Select a possible charset and a default format.  */

      if (optind < argc)
	{
	  RECODE_ALIAS alias
	    = find_alias (outer, argv[optind], ALIAS_FIND_AS_CHARSET);

	  if (!alias)
	    {
	      error (0, 0, _("Charset `%s' is unknown or ambiguous"),
		     argv[optind]);
	      usage (EXIT_FAILURE, 1);
	    }

	  list_charset = alias->symbol;
	}
      else if (list_format != RECODE_NO_FORMAT || charset_restrictions)
	{
	  RECODE_ALIAS alias
	    = find_alias (outer, NULL, ALIAS_FIND_AS_CHARSET);

	  if (!alias)
	    {
	      error (0, 0, _("Charset `%s' is unknown or ambiguous"),
		     argv[optind]);
	      usage (EXIT_FAILURE, 1);
	    }

	  list_charset = alias->symbol;
	}
      else
	list_charset = NULL;

      /* List the charset(s) appropriately.  */

      if (charset_restrictions)
	list_all_symbols (outer, list_charset);
      else if (list_charset)
	if (list_format == RECODE_FULL_FORMAT)
	  list_full_charset (outer, list_charset);
	else
	  list_concise_charset (outer, list_charset, list_format);
      else
	list_all_symbols (outer, NULL);

      /* Then get out.  */

      exit (EXIT_SUCCESS);
    }

  /* Decode the REQUEST argument.  */

  if (optind + 1 > argc)
    {
      error (0, 0, _("Required argument is missing"));
      usage (EXIT_FAILURE, 0);
    }

  request = recode_new_request (outer);
  request->ascii_graphics = request_option.ascii_graphics;
  request->diacritics_only = request_option.diacritics_only;
  request->diaeresis_char = request_option.diaeresis_char;
  request->make_header_flag = request_option.make_header_flag;
  request->verbose_flag = request_option.verbose_flag;

  {
    const char *user_request = argv[optind++];
    char *rewritten_request = NULL;
    const char *colon = strchr (user_request, ':');

    /* Try to detect old syntax, that is, no `..' and a single `:'.  In such
       case, issue a warning, and rewrite the request to new syntax.  */

    if (colon && !strchr (colon + 1, ':'))
      {
	const char *cursor;

	for (cursor = user_request; *cursor; cursor++)
	  if (cursor[0] == '.' && cursor[1] == '.')
	    break;

	if (!*cursor)
	  {
	    char *cursor2;

	    rewritten_request = (char *) xmalloc (strlen (user_request) + 2);
	    cursor2 = rewritten_request;
	    for (cursor = user_request; *cursor != ':'; cursor++)
	      *cursor2++ = *cursor;
	    *cursor2++ = '.';
	    *cursor2++ = '.';
	    for (cursor++; *cursor; cursor++)
	      *cursor2++ = *cursor;
	    *cursor2 = NUL;

	    user_request = rewritten_request;
	    if (!quiet_flag)
	      error (0, 0, _("Syntax is deprecated, please prefer `%s'"),
		     user_request);
	  }
      }

    if (!recode_scan_request (request, user_request))
      error (EXIT_FAILURE, 0, _("Request `%s' is erroneous"), user_request);
    if (rewritten_request)
      free (rewritten_request);
  }

  /* If we merely want source code, do it and get out.  */

  if (request_option.make_header_flag)
    {
      recode_format_table (request, header_language, header_name);
      exit (EXIT_SUCCESS);
    }

  setup_signals ();

  {
    RECODE_TASK task;

    task = recode_new_task (request);
    task->strategy = task_option.strategy;
    task->fail_level = task_option.fail_level;
    task->abort_level = task_option.fail_level;

    /* If there is no input file, act as a filter.  Else, recode all files
       over themselves.  */

    if (optind < argc)
      {
	/* When reading and writing files, unless the user selected
	   otherwise, avoid forking and use intermediate files.  */

	/* FIXME: On a file per file basis, force recoding to be done in
	   memory whenever files are small, which is the usual case.  */

	if (task->strategy == RECODE_STRATEGY_UNDECIDED)
	  task->strategy = RECODE_SEQUENCE_WITH_FILES;

	/* In case files are recoded over themselves and there is no
	   recoding step at all, do not even try to touch the files.  */

	if (request->sequence_length > 0)

	  /* Process files, one at a time.  */

	  for (; optind < argc; optind++)
	    {
	      const char *input_name;
	      char output_name[PATH_MAX];
	      FILE *file;
	      struct stat file_stat;
	      struct utimbuf file_utime;

	      input_name = argv[optind];

	      /* Check if the file can be read and rewritten.  */

	      if (file = fopen (input_name, "r+"), file == NULL)
		error (EXIT_FAILURE, errno, "fopen (%s)", input_name);

	      /* Save the input file attrobites.  */

	      fstat (fileno (file), &file_stat);
	      fclose (file);

	      {
		char *cursor;

		/* Choose an output file in the same directory.  */

		/* FIXME: Scott Schwartz <schwartz@bio.cse.psu.edu> writes:
		   "There's no reason to think that that name is unique."  */
        // To avoid overflows, the size of the array pointed by destination (output_name)
        // shall be long enough to contain the same C string as source
        // (including the terminating null character).
        if (strlen(input_name) >= PATH_MAX) {
            error (EXIT_FAILURE, 0, "input_name reach the PATH_MAX limit");
        }
		strcpy (output_name, input_name);
#if DOSWIN_OR_OS2
		for (cursor = output_name + strlen (output_name);
		     cursor > output_name && cursor[-1] != '/'
		       && cursor[-1] != '\\' && cursor[-1] != ':';
		     cursor--)
		  ;
# if __DJGPP__
		sprintf (cursor, "rec%d.tmp", getpid ());
# else
		strcpy (cursor, "recodeXX.TMP");
# endif
#else
		for (cursor = output_name + strlen (output_name);
		     cursor > output_name && cursor[-1] != '/';
		     cursor--)
		  ;
		sprintf (cursor, "rec%d.tmp", getpid ());
#endif
	      }

	      /* Recode the file.  */

	      task->input.name = input_name;
	      task->output.name = output_name;

	      if (verbose_flag)
		{
		  fprintf (stderr, _("Recoding %s..."), task->input.name);
		  fflush (stderr);
		}

	      if ((*processor) (task))
		{
		  /* Recoding was successful.  */

		  if (verbose_flag)
		    {
		      fprintf (stderr, _(" done\n"));
		      fflush (stderr);
		    }

		  /* Move the new file over the original.  */

		  if (unlink (input_name) < 0)
		    error (EXIT_FAILURE, errno, "unlink (%s)", input_name);

		  /* Preserve the file permissions.  */

		  if (chmod (output_name, file_stat.st_mode & 07777) < 0)
		    error (EXIT_FAILURE, errno, "chmod (%s)", output_name);
#if HAVE_RENAME
		  if (rename (output_name, input_name) < 0)
		    error (EXIT_FAILURE, errno, "rename (%s, %s)",
			   output_name, input_name);
#else
		  if (link (output_name, input_name) < 0)
		    error (EXIT_FAILURE, errno, "link (%s, %s)",
			   output_name, input_name);
		  if (unlink (output_name) < 0)
		    error (EXIT_FAILURE, errno, "unlink (%s)", output_name);
#endif

		  /* Adjust the time stamp for the new file.  */

		  if (!touch_option)
		    {
		      file_utime.actime = file_stat.st_atime;
		      file_utime.modtime = file_stat.st_mtime;
		      utime (input_name, &file_utime);
		    }
		}
	      else
		{
		  /* Recoding failed, discard output.  */

		  success = false;
		  if (verbose_flag)
		    {
		      fprintf (stderr, _(" failed: %s in step `%s..%s'\n"),
			       task_perror (task),
			       task->error_at_step->before->name,
			       task->error_at_step->after->name);
		      fflush (stderr);
		    }
		  else if (!quiet_flag)
		    error (0, 0, _("%s failed: %s in step `%s..%s'"),
			   input_name, task_perror (task),
			   task->error_at_step->before->name,
			   task->error_at_step->after->name);

		  unlink (output_name);
		}
	    }
      }
    else
      {
	/* When reading stdin and writing stdout, unless the user selected
	   otherwise, fork processes interconnected with pipes.  */

	if (task->strategy == RECODE_STRATEGY_UNDECIDED)
	  task->strategy = RECODE_SEQUENCE_WITH_PIPE;

	task->input.name = "";
	task->output.name = "";
	if (!(*processor) (task))
	  {
	    success = false;
	    if (!quiet_flag)
	      error (0, 0, _("%s in step `%s..%s'"),
		     task_perror (task),
		     task->error_at_step->before->name,
		     task->error_at_step->after->name);
	  }
      }
  }

  /* Exit with an appropriate status.  */

  exit (success ? EXIT_SUCCESS : EXIT_FAILURE);
}
