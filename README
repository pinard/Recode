GNU `recode' converts files between character sets and usages.  When
exact transliterations are not possible, it may get rid of the
offending characters or fall back on approximations.  This program
recognizes or produces nearly 150 different character sets and is able
to transliterate files between almost any pair.  Most RFC 1345
character sets are supported.

GNU `recode' has been written by Franc,ois Pinard.  It is an evolving
product, and specifications might change in future releases.

Note that `-f' is not completely implemented.  If you modify or add
an `.l' source file, you need `mawk', GNU `awk' or `nawk', and Flex
2.4.6 or better for remaking `merged.c'.  Finally, see further down,
below, for various installation hints on special systems.

See file `BACKLOG' for a summary of pending mail and articles.
See file `COPYING' for copying conditions.
See file `INSTALL' for compilation and installation instructions.
See file `NEWS' for a list of major changes in the current release.
See file `README.DOS' for notes about the MSDOS port.
See file `THANKS' for a list of contributors.
Use file `File-Latin1' to play with recode, if you feel like it.

Your feedback will help us to make a better and more portable
product.  Mail suggestions and bug reports (including documentation
errors) for this program to `bug-gnu-utils@prep.ai.mit.edu'.  If you
develop new charsets, new methods, or anything along `recode', let
me know and share your findings, at `pinard@iro.umontreal.ca'.

.-------.
| Hints |
`-------'

Here are a few hints which might help installing `recode' on some
systems.  Most may be applied by temporary presetting environment
variables while calling `./configure'.  File `INSTALL' explains this.

* You may override DEFAULT_CHARSET while configuring.  For example, on
an AtariST using `bash', one could do:

	DEFAULT_CHARSET=AtariST ./configure

* Some C compilers, like Apollo's, have a real hard time compiling
merged.c.  If this is your case, first avoid compiler optimization.
From within the Bourne shell, you may use:

	CPPFLAGS=-DUSE_FPUTC CFLAGS= ./configure

* For 80286 based systems, it has been reported that some compilers
generate wrong code while optimizing for *small* models.  So, from
within the Bourne shell, do:

	CFLAGS=-Ml LDFLAGS=-Ml ./configure

to force large memory model.  For 80286 Xenix compiler, the last time
it was tried a while ago, one ought to use:

	CFLAGS='-Ml -F2000' LDFLAGS=-Ml ./configure

* Some smallish systems have poor `popen' support or trash heavily
when processes do fork.  In this case, just before doing `make', edit
`config.h' and ensure HAVE_POPEN and HAVE_PIPE are *not* defined.
