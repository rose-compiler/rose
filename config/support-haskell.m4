AC_DEFUN([ROSE_SUPPORT_HASKELL],
[

AC_MSG_CHECKING(for Haskell)

AC_ARG_WITH([haskell],
	AS_HELP_STRING([--with-haskell=PATH],
		[use Haskell for Haskellport (default autodetect).  PATH is bin directory containing ghc and runghc commands if given]),
	[haskellsetting=$withval],
	[haskellsetting=no])

# DQ (12/11/2016): Avoid failing test and allow haskell use only if explicitly specified 
# (fails for me if environment not setup properly using .bashrc).
#       [haskellsetting=try]

RUNHS=
USE_HASKELL=0

# echo "haskellsetting = $haskellsetting"

if test "x$haskellsetting" != xno; then
	if test "x$haskellsetting" = xtry -o "x$haskellsetting" = xyes ; then
		if which ghc > /dev/null 2> /dev/null; then
			GHC="`which ghc`"
			GHCBIN="`dirname $GHC`"
			USE_HASKELL=1
		elif test "x$haskellsetting" = xyes ; then
			AC_MSG_ERROR([--with-haskell set but ghc command not found in PATH])
		fi
	else
		if test -d "$haskellsetting"; then
			GHCBIN="$haskellsetting"
			USE_HASKELL=1
		else
			AC_MSG_ERROR([argument to --with-haskell must be path to bin directory, but argument is not a directory])
		fi
	fi
fi

if test $USE_HASKELL = 1; then
	if test ! -x "$GHCBIN/ghc" ; then
		AC_MSG_ERROR([ghc could not be found in Haskell bin directory $GHCBIN])
	fi
	if test ! -x "$GHCBIN/runghc" ; then
		AC_MSG_ERROR([runghc could not be found in Haskell bin directory $GHCBIN])
	fi
	AC_MSG_RESULT([$GHCBIN])

 # DQ (5/21/2010): Added macro to rose_config.h file so that we can know when to skip Wave support (see rose_attributes_list.h).
 # AC_DEFINE([ROSE_USE_HASKELL_SUPPORT], [], [Whether to use HASKELL interface or not within ROSE])

 # DQ (5/27/2010): We only support a specific version of GHC, since version 6.12 does not
 # include the required packages.
   echo "ghc_version = "`ghc --version`
   ghc_major_version_number=`ghc --version | tr -d \ | cut -d\n -f3 | cut -d\. -f1`
   ghc_minor_version_number=`ghc --version | tr -d \ | cut -d\n -f3 | cut -d\. -f2`
   ghc_patch_version_number=`ghc --version | tr -d \ | cut -d\n -f3 | cut -d\. -f3`

   echo "ghc_major_version_number = $ghc_major_version_number"
   echo "ghc_minor_version_number = $ghc_minor_version_number"
   echo "ghc_patch_version_number = $ghc_patch_version_number"

   echo "*************************************"
   echo "ghc packages required in ROSE: base, haskell98, syb, mtl, containers (see the Build-Depends entry in projects/haskellport/rose.cabal.in.in)."
   echo "ghc packages supported:"
   ghc -v
   echo "*************************************"

   if test "x$ghc_major_version_number" = "x6"; then
      echo "Recognized an accepted major version number."
      if test "x$ghc_minor_version_number" = "x10"; then
         echo "Recognized an accepted minor version number."
      else
         echo "ERROR: Could not identify an acceptable Haskell gch minor version number (ROSE requires 6.10.x)."
         echo "Please use --with-haskell to specify the path of a right version of gch."
         echo "Or you can use --without-haskell to turn off this check."
         exit 1
      fi
   else
      echo "ERROR: Could not identify an acceptable Haskell gch major version number (ROSE requires 6.10.x)."
      echo "Please use --with-haskell to specify the path of a right version of gch."
      echo "Or you can use --without-haskell to turn off this check."
      exit 1
   fi

else
	AC_MSG_RESULT([no])
fi

AC_ARG_WITH([haskell-config-options],
	AS_HELP_STRING([--with-haskell-config-options=OPTS],
		[supply given options to Haskell configure command.  Default "--prefix=$prefix --user".  See http://www.haskell.org/cabal/release/cabal-latest/doc/users-guide/builders.html#setup-configure]),
	[HS_CONFIG_OPTS="$withval"],
	[HS_CONFIG_OPTS="--prefix=$prefix --user"])

AC_SUBST(GHCBIN)
AM_CONDITIONAL(USE_HASKELL,test "$USE_HASKELL" = 1)
AC_SUBST(HS_CONFIG_OPTS)

])
