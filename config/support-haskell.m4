AC_DEFUN([ROSE_SUPPORT_HASKELL],
[

AC_MSG_CHECKING(for Haskell)

AC_ARG_WITH([haskell],
	AS_HELP_STRING([--with-haskell=PATH],
		[use Haskell for Haskellport (default autodetect).  PATH is bin directory containing ghc and runghc commands if given]),
	[haskellsetting=$withval],
	[haskellsetting=try])

RUNHS=
USE_HASKELL=0

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
			AC_MSG_ERROR([Argument to --with-haskell must be path to bin directory, but argument is not a directory])
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
