# Additional editing of Makefiles and atconfig
/ac_given_INSTALL=/,/^CEOF/ {
  /^CEOF$/ i\
# DJGPP specific Makefile changes.\
  /^aliaspath *	*=/s,:,";",g\
  /TEXINPUTS=/s,:,";",g\
  /PATH=/s,:,";",g\
  s,\\.new\\.,_new.,g\
  s,\\.old\\.,_old.,g\
  s,config\\.h\\.in,config.h-in,g\
  /^install-info-am:/,/^$/ {\
    /@list=/ s,\\\$(INFO_DEPS),& recode.i,\
    /@for *file/ s,\\\$(INFO_DEPS),& recode.i,\
    s,file-\\[0-9\\]\\[0-9\\],& \\$\\$file[0-9] \\$\\$file[0-9][0-9],\
  }
}

# Rename config.h.in into config.h-in.
/^ *CONFIG_HEADERS=/,/^EOF/ {
  s|config\.h|&:config.h-in|
}

# Replace (command) > /dev/null with `command > /dev/null`, since
# parenthesized commands always return zero status in the ported Bash,
# even if the named command doesn't exist
/if ([^|;]*null/{
  s,(,`,
  s,),,
  s,;  *then,`; then,
}

# DOS-style absolute file names should be supported as well
/\*) srcdir=/s,/\*,[\\\\/]* | [A-z]:[\\\\/]*,
/\$]\*) INSTALL=/s,\[/\$\]\*,[\\\\/$]* | [A-z]:[\\\\/]*,
/\$]\*) ac_rel_source=/s,\[/\$\]\*,[\\\\/$]* | [A-z]:[\\\\/]*,

# Switch the order of the two Sed commands, since DOS path names
# could include a colon
/ac_file_inputs=/s,\( -e "s%\^%\$ac_given_srcdir/%"\)\( -e "s%:% $ac_given_srcdir/%g"\),\2\1,

# Prevent the spliting of conftest.subs.
# The sed script: conftest.subs is split into 48 or 90 lines long files.
# This will produce sed scripts called conftest.s1, conftest.s2, etc.
# that will not work if conftest.subs contains a multi line sed command
# at line #90. In this case the first part of the sed command will be the
# last line of conftest.s1 and the rest of the command will be the first lines
# of conftest.s2. So both script will not work properly.
# This matches the configure script produced by Autoconf 2.12
/ac_max_sed_cmds=[0-9]/ s,=.*$,=`sed -n "$=" conftest.subs`,
# This matches the configure script produced by Autoconf 2.14a
/ac_max_sed_lines=[0-9]/ s,=.*$,=`sed -n "$=" $ac_cs_root.subs `,

# gettext.m4 (serial 5) from GNU gettext 0.10.35 has a bug.
# When the package is configured with --without-included-gettext option
# and the system libintl.a is found then the LIBS variable *MUST* be set to -lintl
# so that this one is used for linking.
/echo.*\$gt_cv_func_gettext_libintl/,/^EOF$/ {
  /^fi$/ a\
	     if test "$gt_cv_func_gettext_libintl" = yes; then\
	       LIBS="$LIBS -lintl"\
	     fi
}

# We prefer gettext__ instead of gettext.
/checking for gettext in /,/^EOF$/ s,gettext[ 	]*(,gettext__(,g

# The following two items are changes needed for configuring
# and compiling across partitions.
# 1) The given srcdir value is always translated from the
#    "x:" syntax into "/dev/x" syntax while we run configure.
/^[ 	]*-srcdir=\*.*$/ a\
    ac_optarg=`echo "$ac_optarg" | sed "s,^\\([A-Za-z]\\):,/dev/\\1,"`
/set X `ls -Lt \$srcdir/ i\
   if `echo $srcdir | grep "^/dev/" - > /dev/null`; then\
     srcdir=`echo "$srcdir" | sed -e "s%^/dev/%%" -e "s%/%:/%"`\
   fi

#  2) We need links across partitions, so we will use "cp -pf" instead of "ln".
/# Make a symlink if possible; otherwise try a hard link./,/EOF/ {
  s,;.*then, 2>/dev/null || cp -pf \$srcdir/\$ac_source \$ac_dest&,
}

# Fixes for libiconv/Makefile.in and src/Makefile.in
# due to filename renaming in libiconv/ subdir.
/^EXTRA_DIST[ 	]*=/ {
  s|encodings\.def|encodings/&|
  s|encodings_aix\.def|encodings/aix.def|
  s|encodings_dos\.def|encodings/dos.def|
  s|encodings_osf1\.def|encodings/osf1.def|
  s|encodings_local\.def|encodings/local.def|
}
/^noinst_HEADERS[ 	]*=/ {
  s|aliases\.h|aliases/&|
  s|aliases_aix\.h|aliases/aix.h|
  s|aliases_dos\.h|aliases/dos.h|
  s|aliases_osf1\.h|aliases/osf1.h|
  s|aliases_local\.h|aliases/local.h|
  s|cns|&/|g
  s|iso|&/|g
  s|georgian_|georgian/|g
  s|mac_|mac/|g
}
/\/libiconv\// {
  s|cns|&/|g
  s|iso|&/|g
  s|georgian_|georgian/|g
  s|mac_|mac/|g
  s|aliases\.h|aliases/&|g
  s|aliases_aix\.h|aliases/aix.h|g
  s|aliases_dos\.h|aliases/dos.h|g
  s|aliases_osf1\.h|aliases/osf1.h|g
  s|aliases_local\.h|aliases/local.h|g
  s|encodings\.def|encodings/&|g
  s|encodings_aix\.def|encodings/aix.def|g
  s|encodings_dos\.def|encodings/dos.def|g
  s|encodings_osf1\.def|encodings/osf1.def|g
  s|encodings_local\.def|encodings/local.def|g
}

# Fixes for lib/iconv.c.
# All encodings and aliases files recide in their respective subdirs now.
/^#[ 	]*include/ s|"encodings\.def|"encodings/encodings.def|
/^#[ 	]*include/ s|"encodings_aix\.def|"encodings/aix.def|
/^#[ 	]*include/ s|"encodings_dos\.def|"encodings/dos.def|
/^#[ 	]*include/ s|"encodings_osf1\.def|"encodings/osf1.def|
/^#[ 	]*include/ s|"encodings_local\.def|"encodings/local.def|
/^#[ 	]*include/ s|"aliases\.h|"aliases/aliases.h|
/^#[ 	]*include/ s|"aliases_aix\.h|"aliases/aix.h|
/^#[ 	]*include/ s|"aliases_dos\.h|"aliases/dos.h|
/^#[ 	]*include/ s|"aliases_osf1\.h|"aliases/osf1.h|
/^#[ 	]*include/ s|"aliases_local\.h|"aliases/local.h|

# Fixes for lib/converters\.h.
# All cns, iso, georgian and mac files recide in their respective subdirs now.
/^#[ 	]*include/ s|"cns|&/|
/^#[ 	]*include/ s|"iso|&/|
/^#[ 	]*include/ s|"georgian_|"georgian/|
/^#[ 	]*include/ s|"mac_|"mac/|
