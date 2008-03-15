@echo off
echo Configuring Free Recode for DJGPP v2.x...

Rem The SmallEnv tests protect against fixed and too small size
Rem of the environment in stock DOS shell.

Rem Find out if NLS is wanted or not
Rem and where the sources are.
Rem We always default to NLS support
Rem and to in place configuration.
set NLS=enabled
if not "%NLS%" == "enabled" goto SmallEnv
set XSRC=.
if not "%XSRC%" == "." goto SmallEnv

Rem This checks the case:
Rem   %1 contains the NLS option.
Rem   %2 contains the src path option.
if "%1" == "" goto InPlace
if "%1" == "NLS" goto SrcDir2
if not "%1" == "no-NLS" goto SrcDir1
set NLS=disabled
if not "%NLS%" == "disabled" goto SmallEnv
:SrcDir2
Rem Find out where the sources are
if "%2" == "" goto InPlace
set XSRC=%2
if not "%XSRC%" == "%2" goto SmallEnv
goto NotInPlace

Rem This checks the case:
Rem   %1 contains the src path option.
Rem   %2 contains the NLS option.
:SrcDir1
Rem Find out where the sources are
if "%1" == "" goto InPlace
set XSRC=%1
if not "%XSRC%" == "%1" goto SmallEnv
if "%2" == "" goto NotInPlace
if "%2" == "NLS" goto NotInPlace
if not "%2" == "no-NLS" goto NotInPlace
set NLS=disabled
if not "%NLS%" == "disabled" goto SmallEnv

:NotInPlace
redir -e /dev/null update %XSRC%/configure.orig ./configure
test -f ./configure
if errorlevel 1 update %XSRC%/configure ./configure

:InPlace
Rem Update configuration files
echo Updating configuration scripts...
test -f ./configure.orig
if errorlevel 1 update configure configure.orig
sed -f %XSRC%/contrib/DJGPP/configdj.sed configure.orig > configure
if errorlevel 1 goto SedError

Rem Make sure they have a config.site file
set CONFIG_SITE=%XSRC%/contrib/DJGPP/config.site
if not "%CONFIG_SITE%" == "%XSRC%/contrib/DJGPP/config.site" goto SmallEnv

Rem Make sure crucial file names are not munged by unpacking
test -f %XSRC%/config.h.in
if not errorlevel 1 mv -f %XSRC%/config.h.in %XSRC%/config.h-in

test -f %XSRC%/stamp-djgppfixes
if not errorlevel 1 goto PackageFixed

Rem Fix libiconv/Makefile.in and src/Makefile.in
Rem to reflect the new directory structure.
test -f %XSRC%/libiconv/Makefile.orig
if errorlevel 1 update %XSRC%/libiconv/Makefile.in %XSRC%/libiconv/Makefile.orig
sed -f %XSRC%/contrib/DJGPP/configdj.sed %XSRC%/libiconv/Makefile.orig > Makefile.tmp
if errorlevel 1 goto SedError
update Makefile.tmp %XSRC%/libiconv/Makefile.in
rm Makefile.tmp
test -f %XSRC%/src/Makefile.orig
if errorlevel 1 update %XSRC%/src/Makefile.in %XSRC%/src/Makefile.orig
sed -f %XSRC%/contrib/DJGPP/configdj.sed %XSRC%/src/Makefile.orig > Makefile.tmp
if errorlevel 1 goto SedError
update Makefile.tmp %XSRC%/src/Makefile.in
rm Makefile.tmp

Rem Fix the source files in libiconv to reflect the new directory structure.
test -f %XSRC%/src/iconv.orig
if errorlevel 1 update %XSRC%/src/iconv.c %XSRC%/src/iconv.orig
sed -f %XSRC%/contrib/DJGPP/configdj.sed %XSRC%/src/iconv.orig > iconv.tmp
if errorlevel 1 goto SedError
update iconv.tmp %XSRC%/src/iconv.c
rm iconv.tmp
test -f %XSRC%/libiconv/converters.orig
if errorlevel 1 update %XSRC%/libiconv/converters.h %XSRC%/libiconv/converters.orig
sed -f %XSRC%/contrib/DJGPP/configdj.sed %XSRC%/libiconv/converters.orig > converters.tmp
if errorlevel 1 goto SedError
update converters.tmp %XSRC%/libiconv/converters.h
rm converters.tmp
test -f %XSRC%/libiconv/cns/11643.orig
if errorlevel 1 update %XSRC%/libiconv/cns/11643.h %XSRC%/libiconv/cns/11643.orig
sed -f %XSRC%/contrib/DJGPP/configdj.sed %XSRC%/libiconv/cns/11643.orig > 11643.tmp
if errorlevel 1 goto SedError
update 11643.tmp %XSRC%/libiconv/cns/11643.h
rm 11643.tmp
test -f %XSRC%/libiconv/iso/ir165.orig
if errorlevel 1 update %XSRC%/libiconv/iso/ir165.h %XSRC%/libiconv/iso/ir165.orig
sed -f %XSRC%/contrib/DJGPP/configdj.sed %XSRC%/libiconv/iso/ir165.orig > ir165.tmp
if errorlevel 1 goto SedError
update ir165.tmp %XSRC%/libiconv/iso/ir165.h
rm ir165.tmp

Rem Let libtool use _libs all the time.
test -f %XSRC%/ltconfig.orig
if errorlevel 1 update %XSRC%/ltconfig %XSRC%/ltconfig.orig
sed "/objdir=/s|\.libs|_libs|" %XSRC%/ltconfig.orig > ltconfig.tmp
if errorlevel 1 goto SedError
update ltconfig.tmp %XSRC%/ltconfig
rm ltconfig.tmp

touch %XSRC%/stamp-djgppfixes
:PackageFixed

Rem This is required because DOS/Windows are case-insensitive
Rem to file names, and "make install" will do nothing if Make
Rem finds a file called `install'.
if exist INSTALL ren INSTALL INSTALL.txt

Rem Set HOSTNAME so it shows in config.status
if not "%HOSTNAME%" == "" goto hostdone
if "%windir%" == "" goto msdos
set OS=MS-Windows
if not "%OS%" == "MS-Windows" goto SmallEnv
goto haveos
:msdos
set OS=MS-DOS
if not "%OS%" == "MS-DOS" goto SmallEnv
:haveos
if not "%USERNAME%" == "" goto haveuname
if not "%USER%" == "" goto haveuser
echo No USERNAME and no USER found in the environment, using default values
set HOSTNAME=Unknown PC
if not "%HOSTNAME%" == "Unknown PC" goto SmallEnv
goto userdone
:haveuser
set HOSTNAME=%USER%'s PC
if not "%HOSTNAME%" == "%USER%'s PC" goto SmallEnv
goto userdone
:haveuname
set HOSTNAME=%USERNAME%'s PC
if not "%HOSTNAME%" == "%USERNAME%'s PC" goto SmallEnv
:userdone
set _HOSTNAME=%HOSTNAME%, %OS%
if not "%_HOSTNAME%" == "%HOSTNAME%, %OS%" goto SmallEnv
set HOSTNAME=%_HOSTNAME%
:hostdone
set _HOSTNAME=
set OS=

Rem install-sh is required by the configure script but clashes with the
Rem various Makefile install-foo targets, so we MUST have it before the
Rem script runs and rename it afterwards
test -f %XSRC%/install-sh
if not errorlevel 1 goto NoRen0
test -f %XSRC%/install-sh.sh
if not errorlevel 1 mv -f %XSRC%/install-sh.sh %XSRC%/install-sh
:NoRen0

if "%NLS%" == "disabled" goto WithoutNLS

Rem If NLS is wanted we will probably have to
Rem recode the .po files. For these purpose
Rem we will run recodepo.sh that will check for
Rem needed libs, headers and binaries and return
Rem with one of the following return codes:
Rem errorlevel 2 : package has no po directory (no NLS support)
Rem                and will be configured with --disable-nls.
Rem errorlevel 1 : recode, msgfmt and/or xgettext can not be found,
Rem                package will be configured with --disable-nls.
Rem errorlevel 0 : .po files have/have not been recoded. They will
Rem                be recoded by recodepo.sh if needed, and the
Rem                package will be configured with --enable-nls.
sh %XSRC%/contrib/DJGPP/recodepo.sh
if errorlevel 1 goto WithoutNLS

:WithNLS
Rem We prefer without-included-gettext because libintl.a from gettext package
Rem is the only one that is garanteed to have been ported to DJGPP.
echo Running the ./configure script...
sh ./configure --src=%XSRC% --enable-nls --without-included-gettext
if errorlevel 1 goto CfgError
echo Done.
goto End

:WithoutNLS
echo Running the ./configure script...
sh ./configure --src=%XSRC% --disable-nls
if errorlevel 1 goto CfgError
echo Done.
goto End

:SedError
echo ./configure script editing failed!
goto End

:CfgError
echo ./configure script exited abnormally!
goto End

:SmallEnv
echo Your environment size is too small.  Enlarge it and run me again.
echo Configuration NOT done!

:End
test -f %XSRC%/install-sh.sh
if not errorlevel 1 goto NoRen1
test -f %XSRC%/install-sh
if not errorlevel 1 mv -f %XSRC%/install-sh %XSRC%/install-sh.sh
:NoRen1
set CONFIG_SITE=
set HOSTNAME=
set NLS=
set XSRC=
