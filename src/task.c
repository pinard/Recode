/* Conversion of files between different charsets and surfaces.
   Copyright © 1990,92,93,94,96,97,98,99,00 Free Software Foundation, Inc.
   Contributed by François Pinard <pinard@iro.umontreal.ca>, 1990.

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

#include "common.h"

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <sys/types.h>

#include <sys/wait.h>
/* Bruno suggests the following, from GNU make 3.79.0.2 in `job.c'.  He also
   remarks that on Linux, WEXITSTATUS are bits 15..8 and WTERMSIG are bits
   7..0, while BeOS uses the contrary.  */
#if HAVE_UNION_WAIT
# define WAIT_T union wait
# ifndef WTERMSIG
#  define WTERMSIG(x) ((x).w_termsig)
# endif
# ifndef WCOREDUMP
#  define WCOREDUMP(x) ((x).w_coredump)
# endif
# ifndef WEXITSTATUS
#  define WEXITSTATUS(x) ((x).w_retcode)
# endif
# ifndef WIFSIGNALED
#  define WIFSIGNALED(x) (WTERMSIG(x) != 0)
# endif
# ifndef WIFEXITED
#  define WIFEXITED(x) (WTERMSIG(x) == 0)
# endif
#else
# define WAIT_T int
# ifndef WTERMSIG
#  define WTERMSIG(x) ((x) & 0x7f)
# endif
# ifndef WCOREDUMP
#  define WCOREDUMP(x) ((x) & 0x80)
# endif
# ifndef WEXITSTATUS
#  define WEXITSTATUS(x) (((x) >> 8) & 0xff)
# endif
# ifndef WIFSIGNALED
#  define WIFSIGNALED(x) (WTERMSIG (x) != 0)
# endif
# ifndef WIFEXITED
#  define WIFEXITED(x) (WTERMSIG (x) == 0)
# endif
#endif

/* Buffer size used in transform_mere_copy.  */
#define BUFFER_SIZE (16 * 1024)

/* Input or output helpers.  */

/*-------------------------------------------------------------------.
| Read one byte from the input text of TASK, or EOF is none remain.  |
`-------------------------------------------------------------------*/

/* This function is directly called by get_byte whenever USE_HELPERS is
   defined.  Otherwise, get_byte does everything necessary and this routine
   is not needed.  */

#if USE_HELPERS

int
get_byte_helper (RECODE_TASK task)
{
  if (task->input.file)
    return getc (task->input.file);
  else if (task->input.cursor == task->input.limit)
    return EOF;
  else
    return (unsigned char) *task->input.cursor++;
}

#endif /* USE_HELPERS */

/*-----------------------------------------.
| Write BYTE on the output text for TASK.  |
`-----------------------------------------*/

/* This function is directly called by put_byte whenever USE_HELPERS is
   defined.  It is also called when the output buffer needs to be
   reallocated, which put_byte does not know how to handle itself.  Note
   that when INLINE_HARDER is not defined, USE_HELPERS is implied for Flex
   generated code.  */

void
put_byte_helper (int byte, RECODE_SUBTASK subtask)
{
  if (subtask->output.file)
    putc (byte, subtask->output.file);
  else if (subtask->output.cursor == subtask->output.limit)
    {
      RECODE_OUTER outer = subtask->task->request->outer;
      size_t old_size = subtask->output.limit - subtask->output.buffer;
      size_t new_size = old_size * 3 / 2 + 40;

      /* FIXME: Rethink about how the error should be reported.  */

      if (REALLOC (subtask->output.buffer, new_size, char))
	{
	  subtask->output.cursor = subtask->output.buffer + old_size;
	  subtask->output.limit = subtask->output.buffer + new_size;
	  *subtask->output.cursor++ = byte;
	}
    }
  else
    *subtask->output.cursor++ = byte;
}

/* Error processing.  */

/*------------------------------------------------------------------------.
| Handle a given ERROR, while executing STEP within TASK.  Return true if |
| the abort level has been reached.                                       |
`------------------------------------------------------------------------*/

bool
recode_if_nogo (enum recode_error new_error, RECODE_SUBTASK subtask)
{
  RECODE_TASK task = subtask->task;

  if (new_error > task->error_so_far)
    {
      task->error_so_far = new_error;
      task->error_at_step = subtask->step;
    }
  return task->error_so_far >= task->abort_level;
}

/* Recoding execution control.  */

/*--------------.
| Copy a file.  |
`--------------*/

static bool
transform_mere_copy (RECODE_SUBTASK subtask)
{
  if (subtask->input.file && subtask->output.file)
    {
      /* File to file.  */

      char buffer[BUFFER_SIZE];
      size_t size;

      while (size = fread (buffer, 1, BUFFER_SIZE, subtask->input.file),
	     size == BUFFER_SIZE)
	if (fwrite (buffer, BUFFER_SIZE, 1, subtask->output.file) != 1)
	  {
	    recode_perror (NULL, "fwrite ()");
	    return false;
	  }
      if (size > 0)
	if (fwrite (buffer, size, 1, subtask->output.file) != 1)
	  {
	    recode_perror (NULL, "fwrite ()");
	    return false;
	  }
    }
  else if (subtask->input.file)
    {
      /* File to buffer.  */

      int character;

      /* FIXME: buy now, pay (optimise) only later.   */
      while (character = get_byte (subtask), character != EOF)
	put_byte (character, subtask);
    }
  else if (subtask->output.file)
    {
      /* Buffer to file.  */

      if (subtask->input.cursor < subtask->input.limit)
	if (fwrite (subtask->input.cursor,
		    (unsigned) (subtask->input.limit - subtask->input.cursor),
		    1, subtask->output.file)
	    != 1)
	  {
	    recode_perror (NULL, "fwrite ()");
	    return false;
	  }
    }
  else
    {
      /* Buffer to buffer.  */

      int character;

      /* FIXME: buy now, pay (optimise) only later.  */
      while (character = get_byte (subtask), character != EOF)
	put_byte (character, subtask);
    }

  return true;
}

/*--------------------------------------------------.
| Recode a file using a one-to-one recoding table.  |
`--------------------------------------------------*/

bool
transform_byte_to_byte (RECODE_SUBTASK subtask)
{
  unsigned const char *table = subtask->step->step_table;
  int input_char;

  while (input_char = get_byte (subtask), input_char != EOF)
    put_byte (table[input_char], subtask);

  SUBTASK_RETURN (subtask);
}

/*---------------------------------------------------.
| Recode a file using a one-to-many recoding table.  |
`---------------------------------------------------*/

bool
transform_byte_to_variable (RECODE_SUBTASK subtask)
{
  const char *const *table = subtask->step->step_table;
  int input_char;
  const char *output_string;

  /* Copy the file through the one to many recoding table.  */

  while (input_char = get_byte (subtask), input_char != EOF)
    if (output_string = table[input_char], output_string)
      while (*output_string)
	{
	  put_byte (*output_string, subtask);
	  output_string++;
	}
   else
     RETURN_IF_NOGO (RECODE_UNTRANSLATABLE, subtask);

  SUBTASK_RETURN (subtask);
}

/*---------------------------------------------------------------------.
| Execute the conversion sequence for a recoding TASK, using several   |
| passes with two alternating memory buffers.  This routine assumes at |
| least one needed recoding step.                                      |
`---------------------------------------------------------------------*/

static bool
perform_memory_sequence (RECODE_TASK task)
{
  RECODE_CONST_REQUEST request = task->request;
  struct recode_subtask subtask_block;
  RECODE_SUBTASK subtask = &subtask_block;
  struct recode_read_write_text input;
  struct recode_read_write_text output;
  unsigned sequence_index;
  RECODE_CONST_STEP step;

  memset (subtask, 0, sizeof (struct recode_subtask));
  memset (&input, 0, sizeof (struct recode_read_write_text));
  memset (&output, 0, sizeof (struct recode_read_write_text));
  subtask->task = task;

  /* Execute one pass for each step of the sequence.  */

  for (sequence_index = 0;
       sequence_index < request->sequence_length
	 && task->error_so_far < task->abort_level;
       sequence_index++)
    {
      /* Select the input text for this step.  */

      if (sequence_index == 0)
	{
	  subtask->input = task->input;

	  if (subtask->input.name)
	    {
	      if (!*subtask->input.name)
		subtask->input.file = stdin;
	      else if (subtask->input.file = fopen (subtask->input.name, "r"),
		       subtask->input.file == NULL)
		{
		  recode_perror (NULL, "fopen (%s)", subtask->input.name);
		  return false;
		}
	    }
	}
      else
	{
	  subtask->input.buffer = input.buffer;
	  subtask->input.cursor = input.buffer;
	  subtask->input.limit = input.cursor;
	}

      /* Select the output text for this step.  */

      if (sequence_index < request->sequence_length - 1)
	{
	  subtask->output = output;
	  subtask->output.cursor = subtask->output.buffer;
	}
      else
	{
	  subtask->output = task->output;

	  if (subtask->output.name)
	    {
	      if (!*subtask->output.name)
		subtask->output.file = stdout;
	      else if (subtask->output.file = fopen (subtask->output.name,
						     "w"),
		       subtask->output.file == NULL)
		{
		  recode_perror (NULL, "fopen (%s)", subtask->output.name);
		  return false;
		}
	    }
	}

      /* Execute one recoding step.  */

      step = request->sequence_array + sequence_index;
      subtask->step = step;
      (*step->transform_routine) (subtask);

      /* Post-step clean up.  */

      if (sequence_index == 0)
	{
	  if (subtask->input.name && *subtask->input.name)
	    fclose (subtask->input.file);

	  task->input = subtask->input;

	  subtask->input.name = NULL;
	  subtask->input.file = NULL;
	}

      /* Prepare for next step.  */

      task->swap_input = RECODE_SWAP_UNDECIDED;

      if (sequence_index < request->sequence_length - 1)
	{
	  output = input;
	  input = subtask->output;
	}
      else
	{
	  if (subtask->output.name && *subtask->output.name)
	    fclose (subtask->output.file);

	  task->output = subtask->output;
	}
    }

  /* Final clean up.  */

  /* FIXME: Maybe we should manage this so it could wait, instead, for
     avoiding buffer reallocation each time a new recoding is done?  */

  if (input.buffer)
    free (input.buffer);
  if (output.buffer)
    free (output.buffer);

  SUBTASK_RETURN (subtask);
}

/*-------------------------------------------------------------------------.
| Execute the conversion sequence for a recoding TASK, using several       |
| passes with two alternating intermediate files.  This routine assumes at |
| least one needed recoding step.                                          |
`-------------------------------------------------------------------------*/

/* tmpnam/tmpname/mktemp/tmpfile and the associate logic has been the
   main portability headache of `recode' :-(.

   People reported that tmpname does not exist everywhere.  Further, on
   OS/2, recode aborts if the prefix has more than five characters.

   tmpnam seems to exist everywhere so far.  But NeXT's tmpnam() is such
   that, if called many times in succession, it will always return the
   same value, one has to really open a file with the returned name first
   for the next call to tmpnam() to return a different value; even worse,
   it cycles after 25 unique file names.  I can manage it for a single
   invocation of recode, but using two recode invocations connected with
   a shell pipe, on the NeXT, creates a race by which both copies may
   call tmpnam() in parallel, then getting the same value, and will
   consequently open the same temporary file.

   Noah Friedman <friedman@gnu.org> suggests opening the file with
   O_EXCL, and when the open presumably fails, call tmpnam again, or try
   the mktemp routine in the GNU C library: maybe that will work better.

   Michael I Bushnell <mib@gnu.org> suggests always using tmpfile,
   which opens the file too, using the O_EXCL option to open.

   I'm trying this last suggestion, but rewinding instead of closing.
   Someone reported, a long while ago, that rewind did not work on his
   system, so I reverted to opening and closing the temporary files all
   the time.  I lost the precise references for this problem.  In any
   case, I'm reusing rewind with tmpfile, now.  Hopefully, someone will
   tell me if this still creates a problem somewhere!  */

/* The previous round used tmpnam(3).  This one tries tmpfile(3).  */
#undef USE_TMPNAM
#define USE_TMPFILE 1

/* Guarantee some value for L_tmpnam.  */
#if USE_TMPNAM
# if DOSWIN
#  ifndef L_tmpnam
#   define L_tmpnam 13
#  endif
# else
char *tmpnam ();
#  ifndef L_tmpnam
#   include "pathmax.h"
#   define L_tmpnam PATH_MAX
#  endif
# endif
#endif

#if USE_TMPFILE
FILE *tmpfile PARAMS ((void));
#endif

static bool
perform_pass_sequence (RECODE_TASK task)
{
  RECODE_CONST_REQUEST request = task->request;
  struct recode_subtask subtask_block;
  RECODE_SUBTASK subtask = &subtask_block;
  struct recode_read_write_text input;
  struct recode_read_write_text output;
  unsigned sequence_index;
  RECODE_CONST_STEP step;
#if USE_TMPNAM
  char temporary_name_1[L_tmpnam];
  char temporary_name_2[L_tmpnam];
#endif

  memset (subtask, 0, sizeof (struct recode_subtask));
  memset (&input, 0, sizeof (struct recode_read_write_text));
  memset (&output, 0, sizeof (struct recode_read_write_text));
  subtask->task = task;

#if USE_TMPNAM
# if DOSWIN_OR_OS2
  strcpy (temporary_name_1, "recodex1.tmp");
  strcpy (temporary_name_2, "recodex2.tmp");
# else
  /* Delay name attribution, so NeXT's work.  */
  temporary_name_1[0] = NUL;
  temporary_name_2[0] = NUL;
# endif
  subtask->input.name = temporary_name_1;
  subtask->output.name = temporary_name_2;
#endif

  /* Execute one pass for each step of the sequence.  */

  for (sequence_index = 0;
       sequence_index < request->sequence_length
	 && task->error_so_far < task->abort_level;
       sequence_index++)
    {
      /* Select the input text for this step.  */

      if (sequence_index == 0)
	{
	  subtask->input = task->input;

	  if (subtask->input.name)
	    {
	      if (!*subtask->input.name)
		subtask->input.file = stdin;
	      else if (subtask->input.file = fopen (subtask->input.name, "r"),
		       subtask->input.file == NULL)
		{
		  recode_perror (NULL, "fopen (%s)", subtask->input.name);
		  return false;
		}
	    }
	}
      else
	{
#if USE_TMPNAM
	  if (subtask->input.file = fopen (input.name, "r"),
	      subtask->input.file == NULL)
	    {
	      recode_perror (outer, "fopen (%s)", input.name);
	      return false;
	    }
#endif

#if USE_TMPFILE
	  subtask->input.file = input.file;
	  rewind (subtask->input.file);
#endif
	}

      /* Select the output text for this step.  */

      if (sequence_index < request->sequence_length - 1)
	{
	  subtask->output = output;

#if USE_TMPNAM
# if DOSWIN_OR_OS2
	  if (*subtask->output.name == NUL)
	    tmpnam (subtask->output.name);
# endif
	  if (subtask->output.file = fopen (subtask->output.name, "w"),
	      subtask->output.file == NULL)
	    {
	      recode_perror (outer, "fopen (%s)", subtask->output.name);
	      return false;
	    }
#endif

#if USE_TMPFILE
	  if (subtask->output.file = tmpfile (), subtask->output.file == NULL)
	    {
	      recode_perror (NULL, "tmpfile ()");
	      return false;
	    }
#endif
	}
      else
	{
	  subtask->output = task->output;

	  if (subtask->output.name)
	    {
	      if (!*subtask->output.name)
		subtask->output.file = stdout;
	      else if (subtask->output.file = fopen (subtask->output.name,
						     "w"),
		       subtask->output.file == NULL)
		{
		  recode_perror (NULL, "fopen (%s)", subtask->output.name);
		  return false;
		}
	    }
	}

      /* Execute one recoding step.  */

      step = request->sequence_array + sequence_index;
      subtask->step = step;
      (*step->transform_routine) (subtask);

      /* Post-step clean up.  */

      if (sequence_index == 0)
	{
	  if (subtask->input.name && *subtask->input.name)
	    fclose (subtask->input.file);

	  task->input = subtask->input;

	  subtask->input.name = NULL;
	  subtask->input.buffer = NULL;
	  subtask->input.cursor = NULL;
	  subtask->input.limit = NULL;
	}
      else
	{
	  fclose (subtask->input.file);
#if USE_TMPNAM
	  unlink (subtask->input.name);
#endif
	}

#if USE_TMPNAM
      fclose (subtask->output.file);
#endif

      /* Prepare for next step.  */

      task->swap_input = RECODE_SWAP_UNDECIDED;

      if (sequence_index < request->sequence_length - 1)
	{
	  output = input;
	  input = subtask->output;
	}
      else
	{
	  if (subtask->output.name && *subtask->output.name)
	    fclose (subtask->output.file);

	  task->output = subtask->output;
	}
    }

  SUBTASK_RETURN (subtask);
}

#if HAVE_PIPE

#if !HAVE_DUP2

/*------------------------------------------------------------------------.
| Duplicate the OLD_FD file descriptor into NEW_FD, closing NEW_FD first  |
| if it is used.  This implementation presumes both OLD_FD and NEW_FD are |
| valid file descriptors.						  |
`------------------------------------------------------------------------*/

/* Overall idea taken from GNU Emacs 18.55 dup2 (), in src/sysdep.c.  */

#include <sys/fcntl.h>

#ifndef F_DUPFD

static bool
dup2_recursive (int old_fd, int new_fd)
{
  int fd;

  /* Attempt to dup OLD_FD to NEW_FD.  If not successful, call dup2
     recursively, filling the file descriptor table until NEW_FD is
     reached.  Then close all the spurious file descriptors we created.  */

  if (fd = dup (old_fd) && fd != new_fd)
    if (fd < 0 || dup2_recursive (old_fd, new_fd) < 0 || close (fd) < 0)
      return false;

  return true;
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
| that more than one recoding step is needed.				   |
`-------------------------------------------------------------------------*/

#if 1

/* FIXME: This is no good.  The main process might open too many files for
   one thing.  All of it should create children from left to right, instead
   of all children to a single parent right to left.  */

static bool
perform_pipe_sequence (RECODE_TASK task)
{
  RECODE_CONST_REQUEST request = task->request;
  RECODE_OUTER outer = request->outer;
  struct recode_subtask subtask_block;
  RECODE_SUBTASK subtask = &subtask_block;

  unsigned sequence_index;	/* index into sequence */
  RECODE_CONST_STEP step;	/* pointer into single_steps */

  int pipe_pair[2];		/* pair of file descriptors for a pipe */
  int child_process;		/* child process number, zero if child */
  WAIT_T wait_status;		/* status returned by wait() */

  memset (subtask, 0, sizeof (struct recode_subtask));
  subtask->task = task;
  subtask->input = task->input;
  subtask->output = task->output;

  /* Prepare the final output file.  */

  if (!*subtask->output.name)
    subtask->output.file = stdout;
  else if (subtask->output.file = fopen (subtask->output.name, "w"),
	   subtask->output.file == NULL)
    {
      recode_perror (outer, "fopen (%s)", subtask->output.name);
      return false;
    }

  /* Create all subprocesses, from the last to the first, and
     interconnect them.  */

  for (sequence_index = request->sequence_length - 1;
       sequence_index > 0;
       sequence_index--)
    {
      if (pipe (pipe_pair) < 0)
	{
	  recode_perror (outer, "pipe ()");
	  return false;
	}
      if (child_process = fork (), child_process < 0)
	{
	  recode_perror (outer, "fork ()");
	  return false;
	}
      if (child_process == 0)
	{
          /* The child executes its recoding step, reading from the pipe
             and writing to the current output file; then it exits.  */

	  if (close (pipe_pair[1]) < 0)
	    {
	      recode_perror (outer, "close ()");
	      return false;
	    }
	  if (subtask->input.file = fdopen (pipe_pair[0], "r"),
	      subtask->input.file == NULL)
	    {
	      recode_perror (outer, "fdopen ()");
	      return false;
	    }

	  step = request->sequence_array + sequence_index;
	  subtask->step = step;
	  (*step->transform_routine) (subtask);

	  fclose (subtask->input.file);
	  if (sequence_index < request->sequence_length - 1
	      || *subtask->output.name)
	    fclose (subtask->output.file);

	  exit (task->error_so_far < task->fail_level ? EXIT_SUCCESS
		: EXIT_FAILURE);
	}
      else
	{
          /* The parent redirects the current output file to the pipe.  */

	  if (dup2 (pipe_pair[1], fileno (subtask->output.file)) < 0)
	    {
	      recode_perror (outer, "dup2 ()");
	      return false;
	    }
	  if (close (pipe_pair[0]) < 0)
	    {
	      recode_perror (outer, "close ()");
	      return false;
	    }
	  if (close (pipe_pair[1]) < 0)
	    {
	      recode_perror (outer, "close ()");
	      return false;
	    }
	}
    }

  /* All the children are created, blocked on read.  Now, feed the whole
     chain of processes with the output of the first recoding step.  */

  if (!*subtask->input.name)
    subtask->input.file = stdin;
  else if (subtask->input.file = fopen (subtask->input.name, "r"),
	   subtask->input.file == NULL)
    {
      recode_perror (outer, "fopen (%s)", subtask->input.name);
      return false;
    }

  step = request->sequence_array;
  subtask->step = step;
  (*step->transform_routine) (subtask);

  if (*subtask->input.name)
    fclose (subtask->input.file);

  fclose (subtask->output.file);

  /* Wait on all children, mainly to avoid synchronisation problems on
     output file contents, but also to reduce the number of zombie
     processes in case the user recodes many files at once.  */

  while (wait (&wait_status) > 0)
    {
      /* Diagnose and abort on any abnormally terminating child.  */

#if 0				/* FIXME: Bruno would prefer this.  */
      if (!(WIFEXITED (wait_status)
	    || (WIFSIGNALED (wait_status)
		&& WTERMSIG (wait_status) == SIGPIPE)))
#endif
      if ((wait_status & MASK (8)) != 0
	  && (wait_status & MASK (8)) != SIGPIPE)
	{
	  recode_error (outer, _("Child process wait status is 0x%0.2x"),
			wait_status);
	  return false;
	}

      /* Check for a nonzero exit from the terminating child.  */

      if (WIFEXITED (wait_status)
	  ? WEXITSTATUS (wait_status) != 0
	  : WTERMSIG (wait_status) != 0)
	/* FIXME: It is not very clear what happened in sub-processes.  */
	if (task->error_so_far < task->fail_level)
	  {
	    task->error_so_far = task->fail_level;
	    task->error_at_step = step;
	  }
    }

#if FIXME
  if (interrupted)
    /* FIXME: It is not very clear what happened in sub-processes.  */
    if (task->error_so_far < task->fail_level)
      {
	task->error_so_far = task->fail_level;
	task->error_at_step = step;
      }
#endif

  SUBTASK_RETURN (subtask);
}

#else /* not 1 */

static bool
perform_pipe_sequence (RECODE_TASK task)
{
  RECODE_CONST_REQUEST request = task->request;
  struct recode_subtask subtask_block;
  RECODE_SUBTASK subtask = &subtask_block;

  unsigned sequence_index;	/* index into sequence */
  RECODE_CONST_STEP step;	/* pointer into single_steps */

  int pipe_pair[2];		/* pair of file descriptors for a pipe */
  int child_process;		/* child process number, zero if child */
  WAIT_T wait_status;		/* status returned by wait() */

  memset (subtask, 0, sizeof (struct recode_subtask));
  subtask->task = task;
  subtask->input = task->input;
  subtask->output = task->output;

  /* Prepare the final files.  */

  if (!*subtask->input.name)
    subtask->input.file = stdin;
  else if (subtask->input.file = fopen (subtask->input.name, "r"),
	   subtask->input.file == NULL)
    {
      recode_perror (outer, "fopen (%s)", subtask->input.name);
      return false;
    }

  if (!*subtask->output.name)
    subtask->output.file = stdout;
  else if (subtask->output.file = fopen (subtask->output.name, "w"),
	   subtask->output.file == NULL)
    {
      recode_perror (outer, "fopen (%s)", subtask->output.name);
      return false;
    }

  /* Create all subprocesses, from the first to the last, and
     interconnect them.  */

  for (sequence_index = 0;
       sequence_index < request->sequence_length - 1;
       sequence_index++)
    {
      if (pipe (pipe_pair) < 0)
	{
	  recode_perror (outer, "pipe ()");
	  return false;
	}
      if (child_process = fork (), child_process < 0)
	{
	  recode_perror (outer, "fork ()");
	  return false;
	}
      if (child_process == 0)
	{
          /* The child executes its recoding step, reading from the pipe
             and writing to the current output file; then it exits.  */

	  if (close (pipe_pair[1]) < 0)
	    {
	      recode_perror (outer, "close ()");
	      return false;
	    }
	  if (subtask->input.file = fdopen (pipe_pair[0], "r"),
	      subtask->input.file == NULL)
	    {
	      recode_perror (outer, "fdopen ()");
	      return false;
	    }

	  step = request->sequence_array[sequence_index];
	  subtask->step = step;
	  (*step->transform_routine) (subtask);

	  fclose (subtask->input.file);
	  if (sequence_index < request->sequence_length - 1
	      || *subtask->output.name)
	    fclose (subtask->output.file);

	  exit (task->error_so_far < task->fail_level ? EXIT_SUCCESS
		: EXIT_FAILURE);
	}
      else
	{
          /* The parent redirects the current output file to the pipe.  */

	  if (dup2 (pipe_pair[1], fileno (subtask->output.file)) < 0)
	    {
	      recode_perror (outer, "dup2 ()");
	      return false;
	    }
	  if (close (pipe_pair[0]) < 0)
	    {
	      recode_perror (outer, "close ()");
	      return false;
	    }
	  if (close (pipe_pair[1]) < 0)
	    {
	      recode_perror (outer, "close ()");
	      return false;
	    }
	}
    }

  /* All processes execute the following common code, each with its proper
     value for SEQUENCE_INDEX, CHILD_PROCESS, etc.  */

  /* All the children are created, blocked on read.  Now, feed the whole
     chain of processes with the output of the first recoding step.  */

  if (!*subtask->input.name)
    subtask->input.file = stdin;
  else if (subtask->input.file = fopen (subtask->input.name, "r"),
	   subtask->input.file == NULL)
    {
      recode_perror (outer, "fopen (%s)", subtask->input.name);
      return false;
    }

  step = request->sequence_array[0];
  subtask->step = step;
  (*step->transform_routine) (subtask);

  if (*subtask->input.name)
    fclose (subtask->input.file);

  fclose (subtask->output.file);

  /* Wait on all children, mainly to avoid synchronisation problems on
     output file contents, but also to reduce the number of zombie
     processes in case the user recodes many files at once.  */

  while (wait (&wait_status) > 0)
    {
      /* Diagnose and abort on any abnormally terminating child.  */

      if (!(WIFEXITED (wait_status)
	    || (WIFSIGNALED (wait_status)
		&& WTERMSIG (wait_status) == SIGPIPE)))
	{
	  recode_error (outer, _("Child process wait status is 0x%0.2x"),
			wait_status);
	  return false;
	}

      /* Check for a nonzero exit from the terminating child.  */

      if (WIFEXITED (wait_status)
	  ? WEXITSTATUS (wait_status) != 0
	  : WTERMSIG (wait_status) != 0)
	/* FIXME: It is not very clear what happened in sub-processes.  */
	if (task->error_so_far < task->fail_level)
	  {
	    task->error_so_far = task->fail_level;
	    task->error_at_step = step;
	  }
    }

  if (interrupted)
    /* FIXME: It is not very clear what happened in sub-processes.  */
    if (task->error_so_far < task->fail_level)
      {
	task->error_so_far = task->fail_level;
	task->error_at_step = step;
      }

  SUBTASK_RETURN (subtask);
}

#endif /* not 1 */

#endif /* HAVE_PIPE */

/* Library interface.  */

/* See the recode manual for a more detailed description of the library
   interface.  */

/*-----------------------.
| TASK level functions.  |
`-----------------------*/

RECODE_TASK
recode_new_task (RECODE_CONST_REQUEST request)
{
  RECODE_OUTER outer = request->outer;
  RECODE_TASK task;

  if (!ALLOC (task, 1, struct recode_task))
    return NULL;

  memset (task, 0, sizeof (struct recode_task));
  task->request = request;
  task->strategy = RECODE_STRATEGY_UNDECIDED;
  task->fail_level = RECODE_NOT_CANONICAL;
  task->abort_level = RECODE_USER_ERROR;
  task->error_so_far = RECODE_NO_ERROR;
  task->swap_input = RECODE_SWAP_UNDECIDED;
  task->byte_order_mark = true;

  return task;
}

bool
recode_delete_task (RECODE_TASK task)
{
  free (task);
  return true;
}

#if DOSWIN
# if HAVE_UNISTD_H
#  include <unistd.h>		/* for isatty */
# endif
# include <fcntl.h>		/* for O_BINARY and _fmode */
# include <io.h>		/* for setmode */
#endif

/*------------------------------------------------------------------------.
| Execute the conversion sequence for a recoding TASK, using the selected |
| strategy whenever more than one conversion step is needed.  If no       |
| conversion are needed, merely copy the input onto the output.  Returns  |
| zero if the recoding has been found to be non-reversible.  Tell what    |
| goes on if VERBOSE.                                                     |
`------------------------------------------------------------------------*/

/* If some sequencing strategies are missing, this routine automatically
   uses fallback strategies.  */

bool
recode_perform_task (RECODE_TASK task)
{
  RECODE_CONST_REQUEST request = task->request;
  bool success;

#if DOSWIN_OR_OS2
  /* Don't switch the console device to binary mode.  On several DOSish
     systems this has unpleasant side effects.  For example, the Ctrl-Z
     character is no longer interpreted as EOF, and thus the poor user cannot
     signal end of input; the INTR character also doesn't work, so they cannot
     even interrupt the program, and are stuck.  On the other hand, output to
     the screen doesn't have to follow the end-of-line format exactly, since
     it is going to be discarded anyway.  */
  if (task->input.name && !*task->input.name && !isatty (fileno (stdin)))
    setmode (fileno (stdin), O_BINARY);
  if (task->output.name && !*task->output.name && !isatty (fileno (stdout)))
    setmode (fileno (stdout), O_BINARY);
# ifdef __EMX__
  {
    extern int _fmode_bin;
    _fmode_bin = 1;
  }
# else
  _fmode = O_BINARY;
# endif
#endif

  if (request->sequence_length > 1)
    switch (task->strategy)
      {
      case RECODE_STRATEGY_UNDECIDED:
	/* Let's use only memory if either end is memory, or only temporary
	   files if both ends are files.  This is a crude choice, FIXME!
	   Leave task->strategy alone, as the same task may be used many
	   times differently, and the fact the strategy is undecided is a
	   clue we want to protect between calls.  */

	if ((task->input.name || task->input.file)
	    && (task->output.name || task->output.file))
	  success = perform_pass_sequence (task);
	else
	  success = perform_memory_sequence (task);
	break;

      case RECODE_SEQUENCE_IN_MEMORY:
	success = perform_memory_sequence (task);
	break;

      case RECODE_SEQUENCE_WITH_PIPE:
#if HAVE_PIPE
	success = perform_pipe_sequence (task);
	break;
#else
	/* Fall through on files if there are no pipes.  */
#endif

      case RECODE_SEQUENCE_WITH_FILES:
	success = perform_pass_sequence (task);
	break;

      default:
	success = false;	/* for lint */
      }
  else
    {
      struct recode_subtask subtask_block;
      RECODE_SUBTASK subtask = &subtask_block;

      /* Execute a simple recoding (a single step, or no step at all).  */

      memset (subtask, 0, sizeof (struct recode_subtask));
      subtask->task = task;
      subtask->input = task->input;
      subtask->output = task->output;

      if (subtask->input.name)
	{
	  if (!*subtask->input.name)
	    subtask->input.file = stdin;
	  else if (subtask->input.file = fopen (subtask->input.name, "r"),
		   subtask->input.file == NULL)
	    {
	      recode_perror (NULL, "fopen (%s)", subtask->input.name);
	      return false;
	    }
	}

      if (subtask->output.name)
	{
	  if (!*subtask->output.name)
	    subtask->output.file = stdout;
	  else if (subtask->output.file = fopen (subtask->output.name, "w"),
		   subtask->output.file == NULL)
	    {
	      recode_perror (NULL, "fopen (%s)", subtask->output.name);
	      return false;
	    }
	}

      if (request->sequence_length == 1)
	{
	  RECODE_CONST_STEP step = request->sequence_array;

	  subtask->step = step;
	  success = (*step->transform_routine) (subtask);
	}
      else
	success = transform_mere_copy (subtask);

      if (subtask->input.name && *subtask->input.name)
	fclose (subtask->input.file);
      if (subtask->output.name && *subtask->output.name)
	fclose (subtask->output.file);
    }

  return success;
}
