# generated automatically by aclocal 1.9.6 -*- Autoconf -*-

# Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004,
# 2005  Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

# Copyright (C) 2002, 2003, 2005  Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# AM_AUTOMAKE_VERSION(VERSION)
# ----------------------------
# Automake X.Y traces this macro to ensure aclocal.m4 has been
# generated from the m4 files accompanying Automake X.Y.
AC_DEFUN([AM_AUTOMAKE_VERSION], [am__api_version="1.9"])

# AM_SET_CURRENT_AUTOMAKE_VERSION
# -------------------------------
# Call AM_AUTOMAKE_VERSION so it can be traced.
# This function is AC_REQUIREd by AC_INIT_AUTOMAKE.
AC_DEFUN([AM_SET_CURRENT_AUTOMAKE_VERSION],
	 [AM_AUTOMAKE_VERSION([1.9.6])])

# AM_AUX_DIR_EXPAND                                         -*- Autoconf -*-

# Copyright (C) 2001, 2003, 2005  Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# For projects using AC_CONFIG_AUX_DIR([foo]), Autoconf sets
# $ac_aux_dir to `$srcdir/foo'.  In other projects, it is set to
# `$srcdir', `$srcdir/..', or `$srcdir/../..'.
#
# Of course, Automake must honor this variable whenever it calls a
# tool from the auxiliary directory.  The problem is that $srcdir (and
# therefore $ac_aux_dir as well) can be either absolute or relative,
# depending on how configure is run.  This is pretty annoying, since
# it makes $ac_aux_dir quite unusable in subdirectories: in the top
# source directory, any form will work fine, but in subdirectories a
# relative path needs to be adjusted first.
#
# $ac_aux_dir/missing
#    fails when called from a subdirectory if $ac_aux_dir is relative
# $top_srcdir/$ac_aux_dir/missing
#    fails if $ac_aux_dir is absolute,
#    fails when called from a subdirectory in a VPATH build with
#          a relative $ac_aux_dir
#
# The reason of the latter failure is that $top_srcdir and $ac_aux_dir
# are both prefixed by $srcdir.  In an in-source build this is usually
# harmless because $srcdir is `.', but things will broke when you
# start a VPATH build or use an absolute $srcdir.
#
# So we could use something similar to $top_srcdir/$ac_aux_dir/missing,
# iff we strip the leading $srcdir from $ac_aux_dir.  That would be:
#   am_aux_dir='\$(top_srcdir)/'`expr "$ac_aux_dir" : "$srcdir//*\(.*\)"`
# and then we would define $MISSING as
#   MISSING="\${SHELL} $am_aux_dir/missing"
# This will work as long as MISSING is not called from configure, because
# unfortunately $(top_srcdir) has no meaning in configure.
# However there are other variables, like CC, which are often used in
# configure, and could therefore not use this "fixed" $ac_aux_dir.
#
# Another solution, used here, is to always expand $ac_aux_dir to an
# absolute PATH.  The drawback is that using absolute paths prevent a
# configured tree to be moved without reconfiguration.

AC_DEFUN([AM_AUX_DIR_EXPAND],
[dnl Rely on autoconf to set up CDPATH properly.
AC_PREREQ([2.50])dnl
# expand $ac_aux_dir to an absolute path
am_aux_dir=`cd $ac_aux_dir && pwd`
])

# AM_CONDITIONAL                                            -*- Autoconf -*-

# Copyright (C) 1997, 2000, 2001, 2003, 2004, 2005
# Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 7

# AM_CONDITIONAL(NAME, SHELL-CONDITION)
# -------------------------------------
# Define a conditional.
AC_DEFUN([AM_CONDITIONAL],
[AC_PREREQ(2.52)dnl
 ifelse([$1], [TRUE],  [AC_FATAL([$0: invalid condition: $1])],
	[$1], [FALSE], [AC_FATAL([$0: invalid condition: $1])])dnl
AC_SUBST([$1_TRUE])
AC_SUBST([$1_FALSE])
if $2; then
  $1_TRUE=
  $1_FALSE='#'
else
  $1_TRUE='#'
  $1_FALSE=
fi
AC_CONFIG_COMMANDS_PRE(
[if test -z "${$1_TRUE}" && test -z "${$1_FALSE}"; then
  AC_MSG_ERROR([[conditional "$1" was never defined.
Usually this means the macro was only invoked conditionally.]])
fi])])


# Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005
# Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 8

# There are a few dirty hacks below to avoid letting `AC_PROG_CC' be
# written in clear, in which case automake, when reading aclocal.m4,
# will think it sees a *use*, and therefore will trigger all it's
# C support machinery.  Also note that it means that autoscan, seeing
# CC etc. in the Makefile, will ask for an AC_PROG_CC use...


# _AM_DEPENDENCIES(NAME)
# ----------------------
# See how the compiler implements dependency checking.
# NAME is "CC", "CXX", "GCJ", or "OBJC".
# We try a few techniques and use that to set a single cache variable.
#
# We don't AC_REQUIRE the corresponding AC_PROG_CC since the latter was
# modified to invoke _AM_DEPENDENCIES(CC); we would have a circular
# dependency, and given that the user is not expected to run this macro,
# just rely on AC_PROG_CC.
AC_DEFUN([_AM_DEPENDENCIES],
[AC_REQUIRE([AM_SET_DEPDIR])dnl
AC_REQUIRE([AM_OUTPUT_DEPENDENCY_COMMANDS])dnl
AC_REQUIRE([AM_MAKE_INCLUDE])dnl
AC_REQUIRE([AM_DEP_TRACK])dnl

ifelse([$1], CC,   [depcc="$CC"   am_compiler_list=],
       [$1], CXX,  [depcc="$CXX"  am_compiler_list=],
       [$1], OBJC, [depcc="$OBJC" am_compiler_list='gcc3 gcc'],
       [$1], GCJ,  [depcc="$GCJ"  am_compiler_list='gcc3 gcc'],
                   [depcc="$$1"   am_compiler_list=])

AC_CACHE_CHECK([dependency style of $depcc],
               [am_cv_$1_dependencies_compiler_type],
[if test -z "$AMDEP_TRUE" && test -f "$am_depcomp"; then
  # We make a subdir and do the tests there.  Otherwise we can end up
  # making bogus files that we don't know about and never remove.  For
  # instance it was reported that on HP-UX the gcc test will end up
  # making a dummy file named `D' -- because `-MD' means `put the output
  # in D'.
  mkdir conftest.dir
  # Copy depcomp to subdir because otherwise we won't find it if we're
  # using a relative directory.
  cp "$am_depcomp" conftest.dir
  cd conftest.dir
  # We will build objects and dependencies in a subdirectory because
  # it helps to detect inapplicable dependency modes.  For instance
  # both Tru64's cc and ICC support -MD to output dependencies as a
  # side effect of compilation, but ICC will put the dependencies in
  # the current directory while Tru64 will put them in the object
  # directory.
  mkdir sub

  am_cv_$1_dependencies_compiler_type=none
  if test "$am_compiler_list" = ""; then
     am_compiler_list=`sed -n ['s/^#*\([a-zA-Z0-9]*\))$/\1/p'] < ./depcomp`
  fi
  for depmode in $am_compiler_list; do
    # Setup a source with many dependencies, because some compilers
    # like to wrap large dependency lists on column 80 (with \), and
    # we should not choose a depcomp mode which is confused by this.
    #
    # We need to recreate these files for each test, as the compiler may
    # overwrite some of them when testing with obscure command lines.
    # This happens at least with the AIX C compiler.
    : > sub/conftest.c
    for i in 1 2 3 4 5 6; do
      echo '#include "conftst'$i'.h"' >> sub/conftest.c
      # Using `: > sub/conftst$i.h' creates only sub/conftst1.h with
      # Solaris 8's {/usr,}/bin/sh.
      touch sub/conftst$i.h
    done
    echo "${am__include} ${am__quote}sub/conftest.Po${am__quote}" > confmf

    case $depmode in
    nosideeffect)
      # after this tag, mechanisms are not by side-effect, so they'll
      # only be used when explicitly requested
      if test "x$enable_dependency_tracking" = xyes; then
	continue
      else
	break
      fi
      ;;
    none) break ;;
    esac
    # We check with `-c' and `-o' for the sake of the "dashmstdout"
    # mode.  It turns out that the SunPro C++ compiler does not properly
    # handle `-M -o', and we need to detect this.
    if depmode=$depmode \
       source=sub/conftest.c object=sub/conftest.${OBJEXT-o} \
       depfile=sub/conftest.Po tmpdepfile=sub/conftest.TPo \
       $SHELL ./depcomp $depcc -c -o sub/conftest.${OBJEXT-o} sub/conftest.c \
         >/dev/null 2>conftest.err &&
       grep sub/conftst6.h sub/conftest.Po > /dev/null 2>&1 &&
       grep sub/conftest.${OBJEXT-o} sub/conftest.Po > /dev/null 2>&1 &&
       ${MAKE-make} -s -f confmf > /dev/null 2>&1; then
      # icc doesn't choke on unknown options, it will just issue warnings
      # or remarks (even with -Werror).  So we grep stderr for any message
      # that says an option was ignored or not supported.
      # When given -MP, icc 7.0 and 7.1 complain thusly:
      #   icc: Command line warning: ignoring option '-M'; no argument required
      # The diagnosis changed in icc 8.0:
      #   icc: Command line remark: option '-MP' not supported
      if (grep 'ignoring option' conftest.err ||
          grep 'not supported' conftest.err) >/dev/null 2>&1; then :; else
        am_cv_$1_dependencies_compiler_type=$depmode
        break
      fi
    fi
  done

  cd ..
  rm -rf conftest.dir
else
  am_cv_$1_dependencies_compiler_type=none
fi
])
AC_SUBST([$1DEPMODE], [depmode=$am_cv_$1_dependencies_compiler_type])
AM_CONDITIONAL([am__fastdep$1], [
  test "x$enable_dependency_tracking" != xno \
  && test "$am_cv_$1_dependencies_compiler_type" = gcc3])
])


# AM_SET_DEPDIR
# -------------
# Choose a directory name for dependency files.
# This macro is AC_REQUIREd in _AM_DEPENDENCIES
AC_DEFUN([AM_SET_DEPDIR],
[AC_REQUIRE([AM_SET_LEADING_DOT])dnl
AC_SUBST([DEPDIR], ["${am__leading_dot}deps"])dnl
])


# AM_DEP_TRACK
# ------------
AC_DEFUN([AM_DEP_TRACK],
[AC_ARG_ENABLE(dependency-tracking,
[  --disable-dependency-tracking  speeds up one-time build
  --enable-dependency-tracking   do not reject slow dependency extractors])
if test "x$enable_dependency_tracking" != xno; then
  am_depcomp="$ac_aux_dir/depcomp"
  AMDEPBACKSLASH='\'
fi
AM_CONDITIONAL([AMDEP], [test "x$enable_dependency_tracking" != xno])
AC_SUBST([AMDEPBACKSLASH])
])

# Generate code to set up dependency tracking.              -*- Autoconf -*-

# Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005
# Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

#serial 3

# _AM_OUTPUT_DEPENDENCY_COMMANDS
# ------------------------------
AC_DEFUN([_AM_OUTPUT_DEPENDENCY_COMMANDS],
[for mf in $CONFIG_FILES; do
  # Strip MF so we end up with the name of the file.
  mf=`echo "$mf" | sed -e 's/:.*$//'`
  # Check whether this is an Automake generated Makefile or not.
  # We used to match only the files named `Makefile.in', but
  # some people rename them; so instead we look at the file content.
  # Grep'ing the first line is not enough: some people post-process
  # each Makefile.in and add a new line on top of each file to say so.
  # So let's grep whole file.
  if grep '^#.*generated by automake' $mf > /dev/null 2>&1; then
    dirpart=`AS_DIRNAME("$mf")`
  else
    continue
  fi
  # Extract the definition of DEPDIR, am__include, and am__quote
  # from the Makefile without running `make'.
  DEPDIR=`sed -n 's/^DEPDIR = //p' < "$mf"`
  test -z "$DEPDIR" && continue
  am__include=`sed -n 's/^am__include = //p' < "$mf"`
  test -z "am__include" && continue
  am__quote=`sed -n 's/^am__quote = //p' < "$mf"`
  # When using ansi2knr, U may be empty or an underscore; expand it
  U=`sed -n 's/^U = //p' < "$mf"`
  # Find all dependency output files, they are included files with
  # $(DEPDIR) in their names.  We invoke sed twice because it is the
  # simplest approach to changing $(DEPDIR) to its actual value in the
  # expansion.
  for file in `sed -n "
    s/^$am__include $am__quote\(.*(DEPDIR).*\)$am__quote"'$/\1/p' <"$mf" | \
       sed -e 's/\$(DEPDIR)/'"$DEPDIR"'/g' -e 's/\$U/'"$U"'/g'`; do
    # Make sure the directory exists.
    test -f "$dirpart/$file" && continue
    fdir=`AS_DIRNAME(["$file"])`
    AS_MKDIR_P([$dirpart/$fdir])
    # echo "creating $dirpart/$file"
    echo '# dummy' > "$dirpart/$file"
  done
done
])# _AM_OUTPUT_DEPENDENCY_COMMANDS


# AM_OUTPUT_DEPENDENCY_COMMANDS
# -----------------------------
# This macro should only be invoked once -- use via AC_REQUIRE.
#
# This code is only required when automatic dependency tracking
# is enabled.  FIXME.  This creates each `.P' file that we will
# need in order to bootstrap the dependency handling code.
AC_DEFUN([AM_OUTPUT_DEPENDENCY_COMMANDS],
[AC_CONFIG_COMMANDS([depfiles],
     [test x"$AMDEP_TRUE" != x"" || _AM_OUTPUT_DEPENDENCY_COMMANDS],
     [AMDEP_TRUE="$AMDEP_TRUE" ac_aux_dir="$ac_aux_dir"])
])

# Do all the work for Automake.                             -*- Autoconf -*-

# Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
# Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 12

# This macro actually does too much.  Some checks are only needed if
# your package does certain things.  But this isn't really a big deal.

# AM_INIT_AUTOMAKE(PACKAGE, VERSION, [NO-DEFINE])
# AM_INIT_AUTOMAKE([OPTIONS])
# -----------------------------------------------
# The call with PACKAGE and VERSION arguments is the old style
# call (pre autoconf-2.50), which is being phased out.  PACKAGE
# and VERSION should now be passed to AC_INIT and removed from
# the call to AM_INIT_AUTOMAKE.
# We support both call styles for the transition.  After
# the next Automake release, Autoconf can make the AC_INIT
# arguments mandatory, and then we can depend on a new Autoconf
# release and drop the old call support.
AC_DEFUN([AM_INIT_AUTOMAKE],
[AC_PREREQ([2.58])dnl
dnl Autoconf wants to disallow AM_ names.  We explicitly allow
dnl the ones we care about.
m4_pattern_allow([^AM_[A-Z]+FLAGS$])dnl
AC_REQUIRE([AM_SET_CURRENT_AUTOMAKE_VERSION])dnl
AC_REQUIRE([AC_PROG_INSTALL])dnl
# test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" &&
   test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi

# test whether we have cygpath
if test -z "$CYGPATH_W"; then
  if (cygpath --version) >/dev/null 2>/dev/null; then
    CYGPATH_W='cygpath -w'
  else
    CYGPATH_W=echo
  fi
fi
AC_SUBST([CYGPATH_W])

# Define the identity of the package.
dnl Distinguish between old-style and new-style calls.
m4_ifval([$2],
[m4_ifval([$3], [_AM_SET_OPTION([no-define])])dnl
 AC_SUBST([PACKAGE], [$1])dnl
 AC_SUBST([VERSION], [$2])],
[_AM_SET_OPTIONS([$1])dnl
 AC_SUBST([PACKAGE], ['AC_PACKAGE_TARNAME'])dnl
 AC_SUBST([VERSION], ['AC_PACKAGE_VERSION'])])dnl

_AM_IF_OPTION([no-define],,
[AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of package])
 AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version number of package])])dnl

# Some tools Automake needs.
AC_REQUIRE([AM_SANITY_CHECK])dnl
AC_REQUIRE([AC_ARG_PROGRAM])dnl
AM_MISSING_PROG(ACLOCAL, aclocal-${am__api_version})
AM_MISSING_PROG(AUTOCONF, autoconf)
AM_MISSING_PROG(AUTOMAKE, automake-${am__api_version})
AM_MISSING_PROG(AUTOHEADER, autoheader)
AM_MISSING_PROG(MAKEINFO, makeinfo)
AM_PROG_INSTALL_SH
AM_PROG_INSTALL_STRIP
AC_REQUIRE([AM_PROG_MKDIR_P])dnl
# We need awk for the "check" target.  The system "awk" is bad on
# some platforms.
AC_REQUIRE([AC_PROG_AWK])dnl
AC_REQUIRE([AC_PROG_MAKE_SET])dnl
AC_REQUIRE([AM_SET_LEADING_DOT])dnl
_AM_IF_OPTION([tar-ustar], [_AM_PROG_TAR([ustar])],
              [_AM_IF_OPTION([tar-pax], [_AM_PROG_TAR([pax])],
	      		     [_AM_PROG_TAR([v7])])])
_AM_IF_OPTION([no-dependencies],,
[AC_PROVIDE_IFELSE([AC_PROG_CC],
                  [_AM_DEPENDENCIES(CC)],
                  [define([AC_PROG_CC],
                          defn([AC_PROG_CC])[_AM_DEPENDENCIES(CC)])])dnl
AC_PROVIDE_IFELSE([AC_PROG_CXX],
                  [_AM_DEPENDENCIES(CXX)],
                  [define([AC_PROG_CXX],
                          defn([AC_PROG_CXX])[_AM_DEPENDENCIES(CXX)])])dnl
])
])


# When config.status generates a header, we must update the stamp-h file.
# This file resides in the same directory as the config header
# that is generated.  The stamp files are numbered to have different names.

# Autoconf calls _AC_AM_CONFIG_HEADER_HOOK (when defined) in the
# loop where config.status creates the headers, so we can generate
# our stamp files there.
AC_DEFUN([_AC_AM_CONFIG_HEADER_HOOK],
[# Compute $1's index in $config_headers.
_am_stamp_count=1
for _am_header in $config_headers :; do
  case $_am_header in
    $1 | $1:* )
      break ;;
    * )
      _am_stamp_count=`expr $_am_stamp_count + 1` ;;
  esac
done
echo "timestamp for $1" >`AS_DIRNAME([$1])`/stamp-h[]$_am_stamp_count])

# Copyright (C) 2001, 2003, 2005  Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# AM_PROG_INSTALL_SH
# ------------------
# Define $install_sh.
AC_DEFUN([AM_PROG_INSTALL_SH],
[AC_REQUIRE([AM_AUX_DIR_EXPAND])dnl
install_sh=${install_sh-"$am_aux_dir/install-sh"}
AC_SUBST(install_sh)])

# Copyright (C) 2003, 2005  Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 2

# Check whether the underlying file-system supports filenames
# with a leading dot.  For instance MS-DOS doesn't.
AC_DEFUN([AM_SET_LEADING_DOT],
[rm -rf .tst 2>/dev/null
mkdir .tst 2>/dev/null
if test -d .tst; then
  am__leading_dot=.
else
  am__leading_dot=_
fi
rmdir .tst 2>/dev/null
AC_SUBST([am__leading_dot])])

# Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2005
# Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 5

# AM_PROG_LEX
# -----------
# Autoconf leaves LEX=: if lex or flex can't be found.  Change that to a
# "missing" invocation, for better error output.
AC_DEFUN([AM_PROG_LEX],
[AC_PREREQ(2.50)dnl
AC_REQUIRE([AM_MISSING_HAS_RUN])dnl
AC_REQUIRE([AC_PROG_LEX])dnl
if test "$LEX" = :; then
  LEX=${am_missing_run}flex
fi])

# Check to see how 'make' treats includes.	            -*- Autoconf -*-

# Copyright (C) 2001, 2002, 2003, 2005  Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 3

# AM_MAKE_INCLUDE()
# -----------------
# Check to see how make treats includes.
AC_DEFUN([AM_MAKE_INCLUDE],
[am_make=${MAKE-make}
cat > confinc << 'END'
am__doit:
	@echo done
.PHONY: am__doit
END
# If we don't find an include directive, just comment out the code.
AC_MSG_CHECKING([for style of include used by $am_make])
am__include="#"
am__quote=
_am_result=none
# First try GNU make style include.
echo "include confinc" > confmf
# We grep out `Entering directory' and `Leaving directory'
# messages which can occur if `w' ends up in MAKEFLAGS.
# In particular we don't look at `^make:' because GNU make might
# be invoked under some other name (usually "gmake"), in which
# case it prints its new name instead of `make'.
if test "`$am_make -s -f confmf 2> /dev/null | grep -v 'ing directory'`" = "done"; then
   am__include=include
   am__quote=
   _am_result=GNU
fi
# Now try BSD make style include.
if test "$am__include" = "#"; then
   echo '.include "confinc"' > confmf
   if test "`$am_make -s -f confmf 2> /dev/null`" = "done"; then
      am__include=.include
      am__quote="\""
      _am_result=BSD
   fi
fi
AC_SUBST([am__include])
AC_SUBST([am__quote])
AC_MSG_RESULT([$_am_result])
rm -f confinc confmf
])

# Fake the existence of programs that GNU maintainers use.  -*- Autoconf -*-

# Copyright (C) 1997, 1999, 2000, 2001, 2003, 2005
# Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 4

# AM_MISSING_PROG(NAME, PROGRAM)
# ------------------------------
AC_DEFUN([AM_MISSING_PROG],
[AC_REQUIRE([AM_MISSING_HAS_RUN])
$1=${$1-"${am_missing_run}$2"}
AC_SUBST($1)])


# AM_MISSING_HAS_RUN
# ------------------
# Define MISSING if not defined so far and test if it supports --run.
# If it does, set am_missing_run to use it, otherwise, to nothing.
AC_DEFUN([AM_MISSING_HAS_RUN],
[AC_REQUIRE([AM_AUX_DIR_EXPAND])dnl
test x"${MISSING+set}" = xset || MISSING="\${SHELL} $am_aux_dir/missing"
# Use eval to expand $SHELL
if eval "$MISSING --run true"; then
  am_missing_run="$MISSING --run "
else
  am_missing_run=
  AC_MSG_WARN([`missing' script is too old or missing])
fi
])

# Copyright (C) 2003, 2004, 2005  Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# AM_PROG_MKDIR_P
# ---------------
# Check whether `mkdir -p' is supported, fallback to mkinstalldirs otherwise.
#
# Automake 1.8 used `mkdir -m 0755 -p --' to ensure that directories
# created by `make install' are always world readable, even if the
# installer happens to have an overly restrictive umask (e.g. 077).
# This was a mistake.  There are at least two reasons why we must not
# use `-m 0755':
#   - it causes special bits like SGID to be ignored,
#   - it may be too restrictive (some setups expect 775 directories).
#
# Do not use -m 0755 and let people choose whatever they expect by
# setting umask.
#
# We cannot accept any implementation of `mkdir' that recognizes `-p'.
# Some implementations (such as Solaris 8's) are not thread-safe: if a
# parallel make tries to run `mkdir -p a/b' and `mkdir -p a/c'
# concurrently, both version can detect that a/ is missing, but only
# one can create it and the other will error out.  Consequently we
# restrict ourselves to GNU make (using the --version option ensures
# this.)
AC_DEFUN([AM_PROG_MKDIR_P],
[if mkdir -p --version . >/dev/null 2>&1 && test ! -d ./--version; then
  # We used to keeping the `.' as first argument, in order to
  # allow $(mkdir_p) to be used without argument.  As in
  #   $(mkdir_p) $(somedir)
  # where $(somedir) is conditionally defined.  However this is wrong
  # for two reasons:
  #  1. if the package is installed by a user who cannot write `.'
  #     make install will fail,
  #  2. the above comment should most certainly read
  #     $(mkdir_p) $(DESTDIR)$(somedir)
  #     so it does not work when $(somedir) is undefined and
  #     $(DESTDIR) is not.
  #  To support the latter case, we have to write
  #     test -z "$(somedir)" || $(mkdir_p) $(DESTDIR)$(somedir),
  #  so the `.' trick is pointless.
  mkdir_p='mkdir -p --'
else
  # On NextStep and OpenStep, the `mkdir' command does not
  # recognize any option.  It will interpret all options as
  # directories to create, and then abort because `.' already
  # exists.
  for d in ./-p ./--version;
  do
    test -d $d && rmdir $d
  done
  # $(mkinstalldirs) is defined by Automake if mkinstalldirs exists.
  if test -f "$ac_aux_dir/mkinstalldirs"; then
    mkdir_p='$(mkinstalldirs)'
  else
    mkdir_p='$(install_sh) -d'
  fi
fi
AC_SUBST([mkdir_p])])

# Helper functions for option handling.                     -*- Autoconf -*-

# Copyright (C) 2001, 2002, 2003, 2005  Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 3

# _AM_MANGLE_OPTION(NAME)
# -----------------------
AC_DEFUN([_AM_MANGLE_OPTION],
[[_AM_OPTION_]m4_bpatsubst($1, [[^a-zA-Z0-9_]], [_])])

# _AM_SET_OPTION(NAME)
# ------------------------------
# Set option NAME.  Presently that only means defining a flag for this option.
AC_DEFUN([_AM_SET_OPTION],
[m4_define(_AM_MANGLE_OPTION([$1]), 1)])

# _AM_SET_OPTIONS(OPTIONS)
# ----------------------------------
# OPTIONS is a space-separated list of Automake options.
AC_DEFUN([_AM_SET_OPTIONS],
[AC_FOREACH([_AM_Option], [$1], [_AM_SET_OPTION(_AM_Option)])])

# _AM_IF_OPTION(OPTION, IF-SET, [IF-NOT-SET])
# -------------------------------------------
# Execute IF-SET if OPTION is set, IF-NOT-SET otherwise.
AC_DEFUN([_AM_IF_OPTION],
[m4_ifset(_AM_MANGLE_OPTION([$1]), [$2], [$3])])

# Check to make sure that the build environment is sane.    -*- Autoconf -*-

# Copyright (C) 1996, 1997, 2000, 2001, 2003, 2005
# Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 4

# AM_SANITY_CHECK
# ---------------
AC_DEFUN([AM_SANITY_CHECK],
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftest.file
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftest.file 2> /dev/null`
   if test "$[*]" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftest.file`
   fi
   rm -f conftest.file
   if test "$[*]" != "X $srcdir/configure conftest.file" \
      && test "$[*]" != "X conftest.file $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "$[2]" = conftest.file
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
AC_MSG_RESULT(yes)])

# Copyright (C) 2001, 2003, 2005  Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# AM_PROG_INSTALL_STRIP
# ---------------------
# One issue with vendor `install' (even GNU) is that you can't
# specify the program used to strip binaries.  This is especially
# annoying in cross-compiling environments, where the build's strip
# is unlikely to handle the host's binaries.
# Fortunately install-sh will honor a STRIPPROG variable, so we
# always use install-sh in `make install-strip', and initialize
# STRIPPROG with the value of the STRIP variable (set by the user).
AC_DEFUN([AM_PROG_INSTALL_STRIP],
[AC_REQUIRE([AM_PROG_INSTALL_SH])dnl
# Installed binaries are usually stripped using `strip' when the user
# run `make install-strip'.  However `strip' might not be the right
# tool to use in cross-compilation environments, therefore Automake
# will honor the `STRIP' environment variable to overrule this program.
dnl Don't test for $cross_compiling = yes, because it might be `maybe'.
if test "$cross_compiling" != no; then
  AC_CHECK_TOOL([STRIP], [strip], :)
fi
INSTALL_STRIP_PROGRAM="\${SHELL} \$(install_sh) -c -s"
AC_SUBST([INSTALL_STRIP_PROGRAM])])

# Check how to create a tarball.                            -*- Autoconf -*-

# Copyright (C) 2004, 2005  Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 2

# _AM_PROG_TAR(FORMAT)
# --------------------
# Check how to create a tarball in format FORMAT.
# FORMAT should be one of `v7', `ustar', or `pax'.
#
# Substitute a variable $(am__tar) that is a command
# writing to stdout a FORMAT-tarball containing the directory
# $tardir.
#     tardir=directory && $(am__tar) > result.tar
#
# Substitute a variable $(am__untar) that extract such
# a tarball read from stdin.
#     $(am__untar) < result.tar
AC_DEFUN([_AM_PROG_TAR],
[# Always define AMTAR for backward compatibility.
AM_MISSING_PROG([AMTAR], [tar])
m4_if([$1], [v7],
     [am__tar='${AMTAR} chof - "$$tardir"'; am__untar='${AMTAR} xf -'],
     [m4_case([$1], [ustar],, [pax],,
              [m4_fatal([Unknown tar format])])
AC_MSG_CHECKING([how to create a $1 tar archive])
# Loop over all known methods to create a tar archive until one works.
_am_tools='gnutar m4_if([$1], [ustar], [plaintar]) pax cpio none'
_am_tools=${am_cv_prog_tar_$1-$_am_tools}
# Do not fold the above two line into one, because Tru64 sh and
# Solaris sh will not grok spaces in the rhs of `-'.
for _am_tool in $_am_tools
do
  case $_am_tool in
  gnutar)
    for _am_tar in tar gnutar gtar;
    do
      AM_RUN_LOG([$_am_tar --version]) && break
    done
    am__tar="$_am_tar --format=m4_if([$1], [pax], [posix], [$1]) -chf - "'"$$tardir"'
    am__tar_="$_am_tar --format=m4_if([$1], [pax], [posix], [$1]) -chf - "'"$tardir"'
    am__untar="$_am_tar -xf -"
    ;;
  plaintar)
    # Must skip GNU tar: if it does not support --format= it doesn't create
    # ustar tarball either.
    (tar --version) >/dev/null 2>&1 && continue
    am__tar='tar chf - "$$tardir"'
    am__tar_='tar chf - "$tardir"'
    am__untar='tar xf -'
    ;;
  pax)
    am__tar='pax -L -x $1 -w "$$tardir"'
    am__tar_='pax -L -x $1 -w "$tardir"'
    am__untar='pax -r'
    ;;
  cpio)
    am__tar='find "$$tardir" -print | cpio -o -H $1 -L'
    am__tar_='find "$tardir" -print | cpio -o -H $1 -L'
    am__untar='cpio -i -H $1 -d'
    ;;
  none)
    am__tar=false
    am__tar_=false
    am__untar=false
    ;;
  esac

  # If the value was cached, stop now.  We just wanted to have am__tar
  # and am__untar set.
  test -n "${am_cv_prog_tar_$1}" && break

  # tar/untar a dummy directory, and stop if the command works
  rm -rf conftest.dir
  mkdir conftest.dir
  echo GrepMe > conftest.dir/file
  AM_RUN_LOG([tardir=conftest.dir && eval $am__tar_ >conftest.tar])
  rm -rf conftest.dir
  if test -s conftest.tar; then
    AM_RUN_LOG([$am__untar <conftest.tar])
    grep GrepMe conftest.dir/file >/dev/null 2>&1 && break
  fi
done
rm -rf conftest.dir

AC_CACHE_VAL([am_cv_prog_tar_$1], [am_cv_prog_tar_$1=$_am_tool])
AC_MSG_RESULT([$am_cv_prog_tar_$1])])
AC_SUBST([am__tar])
AC_SUBST([am__untar])
]) # _AM_PROG_TAR

# libtool.m4 - Configure libtool for the host system. -*-Autoconf-*-

# serial 48 AC_PROG_LIBTOOL


# AC_PROVIDE_IFELSE(MACRO-NAME, IF-PROVIDED, IF-NOT-PROVIDED)
# -----------------------------------------------------------
# If this macro is not defined by Autoconf, define it here.
m4_ifdef([AC_PROVIDE_IFELSE],
         [],
         [m4_define([AC_PROVIDE_IFELSE],
	         [m4_ifdef([AC_PROVIDE_$1],
		           [$2], [$3])])])


# AC_PROG_LIBTOOL
# ---------------
AC_DEFUN([AC_PROG_LIBTOOL],
[AC_REQUIRE([_AC_PROG_LIBTOOL])dnl
dnl If AC_PROG_CXX has already been expanded, run AC_LIBTOOL_CXX
dnl immediately, otherwise, hook it in at the end of AC_PROG_CXX.
  AC_PROVIDE_IFELSE([AC_PROG_CXX],
    [AC_LIBTOOL_CXX],
    [define([AC_PROG_CXX], defn([AC_PROG_CXX])[AC_LIBTOOL_CXX
  ])])
dnl And a similar setup for Fortran 77 support
  AC_PROVIDE_IFELSE([AC_PROG_F77],
    [AC_LIBTOOL_F77],
    [define([AC_PROG_F77], defn([AC_PROG_F77])[AC_LIBTOOL_F77
])])

dnl Quote A][M_PROG_GCJ so that aclocal doesn't bring it in needlessly.
dnl If either AC_PROG_GCJ or A][M_PROG_GCJ have already been expanded, run
dnl AC_LIBTOOL_GCJ immediately, otherwise, hook it in at the end of both.
  AC_PROVIDE_IFELSE([AC_PROG_GCJ],
    [AC_LIBTOOL_GCJ],
    [AC_PROVIDE_IFELSE([A][M_PROG_GCJ],
      [AC_LIBTOOL_GCJ],
      [AC_PROVIDE_IFELSE([LT_AC_PROG_GCJ],
	[AC_LIBTOOL_GCJ],
      [ifdef([AC_PROG_GCJ],
	     [define([AC_PROG_GCJ], defn([AC_PROG_GCJ])[AC_LIBTOOL_GCJ])])
       ifdef([A][M_PROG_GCJ],
	     [define([A][M_PROG_GCJ], defn([A][M_PROG_GCJ])[AC_LIBTOOL_GCJ])])
       ifdef([LT_AC_PROG_GCJ],
	     [define([LT_AC_PROG_GCJ],
		defn([LT_AC_PROG_GCJ])[AC_LIBTOOL_GCJ])])])])
])])# AC_PROG_LIBTOOL


# _AC_PROG_LIBTOOL
# ----------------
AC_DEFUN([_AC_PROG_LIBTOOL],
[AC_REQUIRE([AC_LIBTOOL_SETUP])dnl
AC_BEFORE([$0],[AC_LIBTOOL_CXX])dnl
AC_BEFORE([$0],[AC_LIBTOOL_F77])dnl
AC_BEFORE([$0],[AC_LIBTOOL_GCJ])dnl

# This can be used to rebuild libtool when needed
LIBTOOL_DEPS="$ac_aux_dir/ltmain.sh"

# Always use our own libtool.
LIBTOOL='$(SHELL) $(top_builddir)/libtool'
AC_SUBST(LIBTOOL)dnl

# Prevent multiple expansion
define([AC_PROG_LIBTOOL], [])
])# _AC_PROG_LIBTOOL


# AC_LIBTOOL_SETUP
# ----------------
AC_DEFUN([AC_LIBTOOL_SETUP],
[AC_PREREQ(2.50)dnl
AC_REQUIRE([AC_ENABLE_SHARED])dnl
AC_REQUIRE([AC_ENABLE_STATIC])dnl
AC_REQUIRE([AC_ENABLE_FAST_INSTALL])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_CANONICAL_BUILD])dnl
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_PROG_LD])dnl
AC_REQUIRE([AC_PROG_LD_RELOAD_FLAG])dnl
AC_REQUIRE([AC_PROG_NM])dnl

AC_REQUIRE([AC_PROG_LN_S])dnl
AC_REQUIRE([AC_DEPLIBS_CHECK_METHOD])dnl
# Autoconf 2.13's AC_OBJEXT and AC_EXEEXT macros only works for C compilers!
AC_REQUIRE([AC_OBJEXT])dnl
AC_REQUIRE([AC_EXEEXT])dnl
dnl

AC_LIBTOOL_SYS_MAX_CMD_LEN
AC_LIBTOOL_SYS_GLOBAL_SYMBOL_PIPE
AC_LIBTOOL_OBJDIR

AC_REQUIRE([_LT_AC_SYS_COMPILER])dnl
_LT_AC_PROG_ECHO_BACKSLASH

case $host_os in
aix3*)
  # AIX sometimes has problems with the GCC collect2 program.  For some
  # reason, if we set the COLLECT_NAMES environment variable, the problems
  # vanish in a puff of smoke.
  if test "X${COLLECT_NAMES+set}" != Xset; then
    COLLECT_NAMES=
    export COLLECT_NAMES
  fi
  ;;
esac

# Sed substitution that helps us do robust quoting.  It backslashifies
# metacharacters that are still active within double-quoted strings.
Xsed='sed -e 1s/^X//'
[sed_quote_subst='s/\([\\"\\`$\\\\]\)/\\\1/g']

# Same as above, but do not quote variable references.
[double_quote_subst='s/\([\\"\\`\\\\]\)/\\\1/g']

# Sed substitution to delay expansion of an escaped shell variable in a
# double_quote_subst'ed string.
delay_variable_subst='s/\\\\\\\\\\\$/\\\\\\$/g'

# Sed substitution to avoid accidental globbing in evaled expressions
no_glob_subst='s/\*/\\\*/g'

# Constants:
rm="rm -f"

# Global variables:
default_ofile=libtool
can_build_shared=yes

# All known linkers require a `.a' archive for static linking (except MSVC,
# which needs '.lib').
libext=a
ltmain="$ac_aux_dir/ltmain.sh"
ofile="$default_ofile"
with_gnu_ld="$lt_cv_prog_gnu_ld"

AC_CHECK_TOOL(AR, ar, false)
AC_CHECK_TOOL(RANLIB, ranlib, :)
AC_CHECK_TOOL(STRIP, strip, :)

old_CC="$CC"
old_CFLAGS="$CFLAGS"

# Set sane defaults for various variables
test -z "$AR" && AR=ar
test -z "$AR_FLAGS" && AR_FLAGS=cru
test -z "$AS" && AS=as
test -z "$CC" && CC=cc
test -z "$LTCC" && LTCC=$CC
test -z "$LTCFLAGS" && LTCFLAGS=$CFLAGS
test -z "$DLLTOOL" && DLLTOOL=dlltool
test -z "$LD" && LD=ld
test -z "$LN_S" && LN_S="ln -s"
test -z "$MAGIC_CMD" && MAGIC_CMD=file
test -z "$NM" && NM=nm
test -z "$SED" && SED=sed
test -z "$OBJDUMP" && OBJDUMP=objdump
test -z "$RANLIB" && RANLIB=:
test -z "$STRIP" && STRIP=:
test -z "$ac_objext" && ac_objext=o

# Determine commands to create old-style static archives.
old_archive_cmds='$AR $AR_FLAGS $oldlib$oldobjs$old_deplibs'
old_postinstall_cmds='chmod 644 $oldlib'
old_postuninstall_cmds=

if test -n "$RANLIB"; then
  case $host_os in
  openbsd*)
    old_postinstall_cmds="$old_postinstall_cmds~\$RANLIB -t \$oldlib"
    ;;
  *)
    old_postinstall_cmds="$old_postinstall_cmds~\$RANLIB \$oldlib"
    ;;
  esac
  old_archive_cmds="$old_archive_cmds~\$RANLIB \$oldlib"
fi

_LT_CC_BASENAME([$compiler])

# Only perform the check for file, if the check method requires it
case $deplibs_check_method in
file_magic*)
  if test "$file_magic_cmd" = '$MAGIC_CMD'; then
    AC_PATH_MAGIC
  fi
  ;;
esac

AC_PROVIDE_IFELSE([AC_LIBTOOL_DLOPEN], enable_dlopen=yes, enable_dlopen=no)
AC_PROVIDE_IFELSE([AC_LIBTOOL_WIN32_DLL],
enable_win32_dll=yes, enable_win32_dll=no)

AC_ARG_ENABLE([libtool-lock],
    [AC_HELP_STRING([--disable-libtool-lock],
	[avoid locking (might break parallel builds)])])
test "x$enable_libtool_lock" != xno && enable_libtool_lock=yes

AC_ARG_WITH([pic],
    [AC_HELP_STRING([--with-pic],
	[try to use only PIC/non-PIC objects @<:@default=use both@:>@])],
    [pic_mode="$withval"],
    [pic_mode=default])
test -z "$pic_mode" && pic_mode=default

# Use C for the default configuration in the libtool script
tagname=
AC_LIBTOOL_LANG_C_CONFIG
_LT_AC_TAGCONFIG
])# AC_LIBTOOL_SETUP


# _LT_AC_SYS_COMPILER
# -------------------
AC_DEFUN([_LT_AC_SYS_COMPILER],
[AC_REQUIRE([AC_PROG_CC])dnl

# If no C compiler was specified, use CC.
LTCC=${LTCC-"$CC"}

# If no C compiler flags were specified, use CFLAGS.
LTCFLAGS=${LTCFLAGS-"$CFLAGS"}

# Allow CC to be a program name with arguments.
compiler=$CC
])# _LT_AC_SYS_COMPILER


# _LT_CC_BASENAME(CC)
# -------------------
# Calculate cc_basename.  Skip known compiler wrappers and cross-prefix.
AC_DEFUN([_LT_CC_BASENAME],
[for cc_temp in $1""; do
  case $cc_temp in
    compile | *[[\\/]]compile | ccache | *[[\\/]]ccache ) ;;
    distcc | *[[\\/]]distcc | purify | *[[\\/]]purify ) ;;
    \-*) ;;
    *) break;;
  esac
done
cc_basename=`$echo "X$cc_temp" | $Xsed -e 's%.*/%%' -e "s%^$host_alias-%%"`
])


# _LT_COMPILER_BOILERPLATE
# ------------------------
# Check for compiler boilerplate output or warnings with
# the simple compiler test code.
AC_DEFUN([_LT_COMPILER_BOILERPLATE],
[ac_outfile=conftest.$ac_objext
printf "$lt_simple_compile_test_code" >conftest.$ac_ext
eval "$ac_compile" 2>&1 >/dev/null | $SED '/^$/d; /^ *+/d' >conftest.err
_lt_compiler_boilerplate=`cat conftest.err`
$rm conftest*
])# _LT_COMPILER_BOILERPLATE


# _LT_LINKER_BOILERPLATE
# ----------------------
# Check for linker boilerplate output or warnings with
# the simple link test code.
AC_DEFUN([_LT_LINKER_BOILERPLATE],
[ac_outfile=conftest.$ac_objext
printf "$lt_simple_link_test_code" >conftest.$ac_ext
eval "$ac_link" 2>&1 >/dev/null | $SED '/^$/d; /^ *+/d' >conftest.err
_lt_linker_boilerplate=`cat conftest.err`
$rm conftest*
])# _LT_LINKER_BOILERPLATE


# _LT_AC_SYS_LIBPATH_AIX
# ----------------------
# Links a minimal program and checks the executable
# for the system default hardcoded library path. In most cases,
# this is /usr/lib:/lib, but when the MPI compilers are used
# the location of the communication and MPI libs are included too.
# If we don't find anything, use the default library path according
# to the aix ld manual.
AC_DEFUN([_LT_AC_SYS_LIBPATH_AIX],
[AC_LINK_IFELSE(AC_LANG_PROGRAM,[
aix_libpath=`dump -H conftest$ac_exeext 2>/dev/null | $SED -n -e '/Import File Strings/,/^$/ { /^0/ { s/^0  *\(.*\)$/\1/; p; }
}'`
# Check for a 64-bit object if we didn't find anything.
if test -z "$aix_libpath"; then aix_libpath=`dump -HX64 conftest$ac_exeext 2>/dev/null | $SED -n -e '/Import File Strings/,/^$/ { /^0/ { s/^0  *\(.*\)$/\1/; p; }
}'`; fi],[])
if test -z "$aix_libpath"; then aix_libpath="/usr/lib:/lib"; fi
])# _LT_AC_SYS_LIBPATH_AIX


# _LT_AC_SHELL_INIT(ARG)
# ----------------------
AC_DEFUN([_LT_AC_SHELL_INIT],
[ifdef([AC_DIVERSION_NOTICE],
	     [AC_DIVERT_PUSH(AC_DIVERSION_NOTICE)],
	 [AC_DIVERT_PUSH(NOTICE)])
$1
AC_DIVERT_POP
])# _LT_AC_SHELL_INIT


# _LT_AC_PROG_ECHO_BACKSLASH
# --------------------------
# Add some code to the start of the generated configure script which
# will find an echo command which doesn't interpret backslashes.
AC_DEFUN([_LT_AC_PROG_ECHO_BACKSLASH],
[_LT_AC_SHELL_INIT([
# Check that we are running under the correct shell.
SHELL=${CONFIG_SHELL-/bin/sh}

case X$ECHO in
X*--fallback-echo)
  # Remove one level of quotation (which was required for Make).
  ECHO=`echo "$ECHO" | sed 's,\\\\\[$]\\[$]0,'[$]0','`
  ;;
esac

echo=${ECHO-echo}
if test "X[$]1" = X--no-reexec; then
  # Discard the --no-reexec flag, and continue.
  shift
elif test "X[$]1" = X--fallback-echo; then
  # Avoid inline document here, it may be left over
  :
elif test "X`($echo '\t') 2>/dev/null`" = 'X\t' ; then
  # Yippee, $echo works!
  :
else
  # Restart under the correct shell.
  exec $SHELL "[$]0" --no-reexec ${1+"[$]@"}
fi

if test "X[$]1" = X--fallback-echo; then
  # used as fallback echo
  shift
  cat <<EOF
[$]*
EOF
  exit 0
fi

# The HP-UX ksh and POSIX shell print the target directory to stdout
# if CDPATH is set.
(unset CDPATH) >/dev/null 2>&1 && unset CDPATH

if test -z "$ECHO"; then
if test "X${echo_test_string+set}" != Xset; then
# find a string as large as possible, as long as the shell can cope with it
  for cmd in 'sed 50q "[$]0"' 'sed 20q "[$]0"' 'sed 10q "[$]0"' 'sed 2q "[$]0"' 'echo test'; do
    # expected sizes: less than 2Kb, 1Kb, 512 bytes, 16 bytes, ...
    if (echo_test_string=`eval $cmd`) 2>/dev/null &&
       echo_test_string=`eval $cmd` &&
       (test "X$echo_test_string" = "X$echo_test_string") 2>/dev/null
    then
      break
    fi
  done
fi

if test "X`($echo '\t') 2>/dev/null`" = 'X\t' &&
   echo_testing_string=`($echo "$echo_test_string") 2>/dev/null` &&
   test "X$echo_testing_string" = "X$echo_test_string"; then
  :
else
  # The Solaris, AIX, and Digital Unix default echo programs unquote
  # backslashes.  This makes it impossible to quote backslashes using
  #   echo "$something" | sed 's/\\/\\\\/g'
  #
  # So, first we look for a working echo in the user's PATH.

  lt_save_ifs="$IFS"; IFS=$PATH_SEPARATOR
  for dir in $PATH /usr/ucb; do
    IFS="$lt_save_ifs"
    if (test -f $dir/echo || test -f $dir/echo$ac_exeext) &&
       test "X`($dir/echo '\t') 2>/dev/null`" = 'X\t' &&
       echo_testing_string=`($dir/echo "$echo_test_string") 2>/dev/null` &&
       test "X$echo_testing_string" = "X$echo_test_string"; then
      echo="$dir/echo"
      break
    fi
  done
  IFS="$lt_save_ifs"

  if test "X$echo" = Xecho; then
    # We didn't find a better echo, so look for alternatives.
    if test "X`(print -r '\t') 2>/dev/null`" = 'X\t' &&
       echo_testing_string=`(print -r "$echo_test_string") 2>/dev/null` &&
       test "X$echo_testing_string" = "X$echo_test_string"; then
      # This shell has a builtin print -r that does the trick.
      echo='print -r'
    elif (test -f /bin/ksh || test -f /bin/ksh$ac_exeext) &&
	 test "X$CONFIG_SHELL" != X/bin/ksh; then
      # If we have ksh, try running configure again with it.
      ORIGINAL_CONFIG_SHELL=${CONFIG_SHELL-/bin/sh}
      export ORIGINAL_CONFIG_SHELL
      CONFIG_SHELL=/bin/ksh
      export CONFIG_SHELL
      exec $CONFIG_SHELL "[$]0" --no-reexec ${1+"[$]@"}
    else
      # Try using printf.
      echo='printf %s\n'
      if test "X`($echo '\t') 2>/dev/null`" = 'X\t' &&
	 echo_testing_string=`($echo "$echo_test_string") 2>/dev/null` &&
	 test "X$echo_testing_string" = "X$echo_test_string"; then
	# Cool, printf works
	:
      elif echo_testing_string=`($ORIGINAL_CONFIG_SHELL "[$]0" --fallback-echo '\t') 2>/dev/null` &&
	   test "X$echo_testing_string" = 'X\t' &&
	   echo_testing_string=`($ORIGINAL_CONFIG_SHELL "[$]0" --fallback-echo "$echo_test_string") 2>/dev/null` &&
	   test "X$echo_testing_string" = "X$echo_test_string"; then
	CONFIG_SHELL=$ORIGINAL_CONFIG_SHELL
	export CONFIG_SHELL
	SHELL="$CONFIG_SHELL"
	export SHELL
	echo="$CONFIG_SHELL [$]0 --fallback-echo"
      elif echo_testing_string=`($CONFIG_SHELL "[$]0" --fallback-echo '\t') 2>/dev/null` &&
	   test "X$echo_testing_string" = 'X\t' &&
	   echo_testing_string=`($CONFIG_SHELL "[$]0" --fallback-echo "$echo_test_string") 2>/dev/null` &&
	   test "X$echo_testing_string" = "X$echo_test_string"; then
	echo="$CONFIG_SHELL [$]0 --fallback-echo"
      else
	# maybe with a smaller string...
	prev=:

	for cmd in 'echo test' 'sed 2q "[$]0"' 'sed 10q "[$]0"' 'sed 20q "[$]0"' 'sed 50q "[$]0"'; do
	  if (test "X$echo_test_string" = "X`eval $cmd`") 2>/dev/null
	  then
	    break
	  fi
	  prev="$cmd"
	done

	if test "$prev" != 'sed 50q "[$]0"'; then
	  echo_test_string=`eval $prev`
	  export echo_test_string
	  exec ${ORIGINAL_CONFIG_SHELL-${CONFIG_SHELL-/bin/sh}} "[$]0" ${1+"[$]@"}
	else
	  # Oops.  We lost completely, so just stick with echo.
	  echo=echo
	fi
      fi
    fi
  fi
fi
fi

# Copy echo and quote the copy suitably for passing to libtool from
# the Makefile, instead of quoting the original, which is used later.
ECHO=$echo
if test "X$ECHO" = "X$CONFIG_SHELL [$]0 --fallback-echo"; then
   ECHO="$CONFIG_SHELL \\\$\[$]0 --fallback-echo"
fi

AC_SUBST(ECHO)
])])# _LT_AC_PROG_ECHO_BACKSLASH


# _LT_AC_LOCK
# -----------
AC_DEFUN([_LT_AC_LOCK],
[AC_ARG_ENABLE([libtool-lock],
    [AC_HELP_STRING([--disable-libtool-lock],
	[avoid locking (might break parallel builds)])])
test "x$enable_libtool_lock" != xno && enable_libtool_lock=yes

# Some flags need to be propagated to the compiler or linker for good
# libtool support.
case $host in
ia64-*-hpux*)
  # Find out which ABI we are using.
  echo 'int i;' > conftest.$ac_ext
  if AC_TRY_EVAL(ac_compile); then
    case `/usr/bin/file conftest.$ac_objext` in
    *ELF-32*)
      HPUX_IA64_MODE="32"
      ;;
    *ELF-64*)
      HPUX_IA64_MODE="64"
      ;;
    esac
  fi
  rm -rf conftest*
  ;;
*-*-irix6*)
  # Find out which ABI we are using.
  echo '[#]line __oline__ "configure"' > conftest.$ac_ext
  if AC_TRY_EVAL(ac_compile); then
   if test "$lt_cv_prog_gnu_ld" = yes; then
    case `/usr/bin/file conftest.$ac_objext` in
    *32-bit*)
      LD="${LD-ld} -melf32bsmip"
      ;;
    *N32*)
      LD="${LD-ld} -melf32bmipn32"
      ;;
    *64-bit*)
      LD="${LD-ld} -melf64bmip"
      ;;
    esac
   else
    case `/usr/bin/file conftest.$ac_objext` in
    *32-bit*)
      LD="${LD-ld} -32"
      ;;
    *N32*)
      LD="${LD-ld} -n32"
      ;;
    *64-bit*)
      LD="${LD-ld} -64"
      ;;
    esac
   fi
  fi
  rm -rf conftest*
  ;;

x86_64-*linux*|ppc*-*linux*|powerpc*-*linux*|s390*-*linux*|sparc*-*linux*)
  # Find out which ABI we are using.
  echo 'int i;' > conftest.$ac_ext
  if AC_TRY_EVAL(ac_compile); then
    case `/usr/bin/file conftest.o` in
    *32-bit*)
      case $host in
        x86_64-*linux*)
          LD="${LD-ld} -m elf_i386"
          ;;
        ppc64-*linux*|powerpc64-*linux*)
          LD="${LD-ld} -m elf32ppclinux"
          ;;
        s390x-*linux*)
          LD="${LD-ld} -m elf_s390"
          ;;
        sparc64-*linux*)
          LD="${LD-ld} -m elf32_sparc"
          ;;
      esac
      ;;
    *64-bit*)
      case $host in
        x86_64-*linux*)
          LD="${LD-ld} -m elf_x86_64"
          ;;
        ppc*-*linux*|powerpc*-*linux*)
          LD="${LD-ld} -m elf64ppc"
          ;;
        s390*-*linux*)
          LD="${LD-ld} -m elf64_s390"
          ;;
        sparc*-*linux*)
          LD="${LD-ld} -m elf64_sparc"
          ;;
      esac
      ;;
    esac
  fi
  rm -rf conftest*
  ;;

*-*-sco3.2v5*)
  # On SCO OpenServer 5, we need -belf to get full-featured binaries.
  SAVE_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS -belf"
  AC_CACHE_CHECK([whether the C compiler needs -belf], lt_cv_cc_needs_belf,
    [AC_LANG_PUSH(C)
     AC_TRY_LINK([],[],[lt_cv_cc_needs_belf=yes],[lt_cv_cc_needs_belf=no])
     AC_LANG_POP])
  if test x"$lt_cv_cc_needs_belf" != x"yes"; then
    # this is probably gcc 2.8.0, egcs 1.0 or newer; no need for -belf
    CFLAGS="$SAVE_CFLAGS"
  fi
  ;;
sparc*-*solaris*)
  # Find out which ABI we are using.
  echo 'int i;' > conftest.$ac_ext
  if AC_TRY_EVAL(ac_compile); then
    case `/usr/bin/file conftest.o` in
    *64-bit*)
      case $lt_cv_prog_gnu_ld in
      yes*) LD="${LD-ld} -m elf64_sparc" ;;
      *)    LD="${LD-ld} -64" ;;
      esac
      ;;
    esac
  fi
  rm -rf conftest*
  ;;

AC_PROVIDE_IFELSE([AC_LIBTOOL_WIN32_DLL],
[*-*-cygwin* | *-*-mingw* | *-*-pw32*)
  AC_CHECK_TOOL(DLLTOOL, dlltool, false)
  AC_CHECK_TOOL(AS, as, false)
  AC_CHECK_TOOL(OBJDUMP, objdump, false)
  ;;
  ])
esac

need_locks="$enable_libtool_lock"

])# _LT_AC_LOCK


# AC_LIBTOOL_COMPILER_OPTION(MESSAGE, VARIABLE-NAME, FLAGS,
#		[OUTPUT-FILE], [ACTION-SUCCESS], [ACTION-FAILURE])
# ----------------------------------------------------------------
# Check whether the given compiler option works
AC_DEFUN([AC_LIBTOOL_COMPILER_OPTION],
[AC_REQUIRE([LT_AC_PROG_SED])
AC_CACHE_CHECK([$1], [$2],
  [$2=no
  ifelse([$4], , [ac_outfile=conftest.$ac_objext], [ac_outfile=$4])
   printf "$lt_simple_compile_test_code" > conftest.$ac_ext
   lt_compiler_flag="$3"
   # Insert the option either (1) after the last *FLAGS variable, or
   # (2) before a word containing "conftest.", or (3) at the end.
   # Note that $ac_compile itself does not contain backslashes and begins
   # with a dollar sign (not a hyphen), so the echo should work correctly.
   # The option is referenced via a variable to avoid confusing sed.
   lt_compile=`echo "$ac_compile" | $SED \
   -e 's:.*FLAGS}\{0,1\} :&$lt_compiler_flag :; t' \
   -e 's: [[^ ]]*conftest\.: $lt_compiler_flag&:; t' \
   -e 's:$: $lt_compiler_flag:'`
   (eval echo "\"\$as_me:__oline__: $lt_compile\"" >&AS_MESSAGE_LOG_FD)
   (eval "$lt_compile" 2>conftest.err)
   ac_status=$?
   cat conftest.err >&AS_MESSAGE_LOG_FD
   echo "$as_me:__oline__: \$? = $ac_status" >&AS_MESSAGE_LOG_FD
   if (exit $ac_status) && test -s "$ac_outfile"; then
     # The compiler can only warn and ignore the option if not recognized
     # So say no if there are warnings other than the usual output.
     $echo "X$_lt_compiler_boilerplate" | $Xsed -e '/^$/d' >conftest.exp
     $SED '/^$/d; /^ *+/d' conftest.err >conftest.er2
     if test ! -s conftest.er2 || diff conftest.exp conftest.er2 >/dev/null; then
       $2=yes
     fi
   fi
   $rm conftest*
])

if test x"[$]$2" = xyes; then
    ifelse([$5], , :, [$5])
else
    ifelse([$6], , :, [$6])
fi
])# AC_LIBTOOL_COMPILER_OPTION


# AC_LIBTOOL_LINKER_OPTION(MESSAGE, VARIABLE-NAME, FLAGS,
#                          [ACTION-SUCCESS], [ACTION-FAILURE])
# ------------------------------------------------------------
# Check whether the given compiler option works
AC_DEFUN([AC_LIBTOOL_LINKER_OPTION],
[AC_CACHE_CHECK([$1], [$2],
  [$2=no
   save_LDFLAGS="$LDFLAGS"
   LDFLAGS="$LDFLAGS $3"
   printf "$lt_simple_link_test_code" > conftest.$ac_ext
   if (eval $ac_link 2>conftest.err) && test -s conftest$ac_exeext; then
     # The linker can only warn and ignore the option if not recognized
     # So say no if there are warnings
     if test -s conftest.err; then
       # Append any errors to the config.log.
       cat conftest.err 1>&AS_MESSAGE_LOG_FD
       $echo "X$_lt_linker_boilerplate" | $Xsed -e '/^$/d' > conftest.exp
       $SED '/^$/d; /^ *+/d' conftest.err >conftest.er2
       if diff conftest.exp conftest.er2 >/dev/null; then
         $2=yes
       fi
     else
       $2=yes
     fi
   fi
   $rm conftest*
   LDFLAGS="$save_LDFLAGS"
])

if test x"[$]$2" = xyes; then
    ifelse([$4], , :, [$4])
else
    ifelse([$5], , :, [$5])
fi
])# AC_LIBTOOL_LINKER_OPTION


# AC_LIBTOOL_SYS_MAX_CMD_LEN
# --------------------------
AC_DEFUN([AC_LIBTOOL_SYS_MAX_CMD_LEN],
[# find the maximum length of command line arguments
AC_MSG_CHECKING([the maximum length of command line arguments])
AC_CACHE_VAL([lt_cv_sys_max_cmd_len], [dnl
  i=0
  teststring="ABCD"

  case $build_os in
  msdosdjgpp*)
    # On DJGPP, this test can blow up pretty badly due to problems in libc
    # (any single argument exceeding 2000 bytes causes a buffer overrun
    # during glob expansion).  Even if it were fixed, the result of this
    # check would be larger than it should be.
    lt_cv_sys_max_cmd_len=12288;    # 12K is about right
    ;;

  gnu*)
    # Under GNU Hurd, this test is not required because there is
    # no limit to the length of command line arguments.
    # Libtool will interpret -1 as no limit whatsoever
    lt_cv_sys_max_cmd_len=-1;
    ;;

  cygwin* | mingw*)
    # On Win9x/ME, this test blows up -- it succeeds, but takes
    # about 5 minutes as the teststring grows exponentially.
    # Worse, since 9x/ME are not pre-emptively multitasking,
    # you end up with a "frozen" computer, even though with patience
    # the test eventually succeeds (with a max line length of 256k).
    # Instead, let's just punt: use the minimum linelength reported by
    # all of the supported platforms: 8192 (on NT/2K/XP).
    lt_cv_sys_max_cmd_len=8192;
    ;;

  amigaos*)
    # On AmigaOS with pdksh, this test takes hours, literally.
    # So we just punt and use a minimum line length of 8192.
    lt_cv_sys_max_cmd_len=8192;
    ;;

  netbsd* | freebsd* | openbsd* | darwin* | dragonfly*)
    # This has been around since 386BSD, at least.  Likely further.
    if test -x /sbin/sysctl; then
      lt_cv_sys_max_cmd_len=`/sbin/sysctl -n kern.argmax`
    elif test -x /usr/sbin/sysctl; then
      lt_cv_sys_max_cmd_len=`/usr/sbin/sysctl -n kern.argmax`
    else
      lt_cv_sys_max_cmd_len=65536	# usable default for all BSDs
    fi
    # And add a safety zone
    lt_cv_sys_max_cmd_len=`expr $lt_cv_sys_max_cmd_len \/ 4`
    lt_cv_sys_max_cmd_len=`expr $lt_cv_sys_max_cmd_len \* 3`
    ;;

  interix*)
    # We know the value 262144 and hardcode it with a safety zone (like BSD)
    lt_cv_sys_max_cmd_len=196608
    ;;

  osf*)
    # Dr. Hans Ekkehard Plesser reports seeing a kernel panic running configure
    # due to this test when exec_disable_arg_limit is 1 on Tru64. It is not
    # nice to cause kernel panics so lets avoid the loop below.
    # First set a reasonable default.
    lt_cv_sys_max_cmd_len=16384
    #
    if test -x /sbin/sysconfig; then
      case `/sbin/sysconfig -q proc exec_disable_arg_limit` in
        *1*) lt_cv_sys_max_cmd_len=-1 ;;
      esac
    fi
    ;;
  sco3.2v5*)
    lt_cv_sys_max_cmd_len=102400
    ;;
  sysv5* | sco5v6* | sysv4.2uw2*)
    kargmax=`grep ARG_MAX /etc/conf/cf.d/stune 2>/dev/null`
    if test -n "$kargmax"; then
      lt_cv_sys_max_cmd_len=`echo $kargmax | sed 's/.*[[ 	]]//'`
    else
      lt_cv_sys_max_cmd_len=32768
    fi
    ;;
  *)
    # If test is not a shell built-in, we'll probably end up computing a
    # maximum length that is only half of the actual maximum length, but
    # we can't tell.
    SHELL=${SHELL-${CONFIG_SHELL-/bin/sh}}
    while (test "X"`$SHELL [$]0 --fallback-echo "X$teststring" 2>/dev/null` \
	       = "XX$teststring") >/dev/null 2>&1 &&
	    new_result=`expr "X$teststring" : ".*" 2>&1` &&
	    lt_cv_sys_max_cmd_len=$new_result &&
	    test $i != 17 # 1/2 MB should be enough
    do
      i=`expr $i + 1`
      teststring=$teststring$teststring
    done
    teststring=
    # Add a significant safety factor because C++ compilers can tack on massive
    # amounts of additional arguments before passing them to the linker.
    # It appears as though 1/2 is a usable value.
    lt_cv_sys_max_cmd_len=`expr $lt_cv_sys_max_cmd_len \/ 2`
    ;;
  esac
])
if test -n $lt_cv_sys_max_cmd_len ; then
  AC_MSG_RESULT($lt_cv_sys_max_cmd_len)
else
  AC_MSG_RESULT(none)
fi
])# AC_LIBTOOL_SYS_MAX_CMD_LEN


# _LT_AC_CHECK_DLFCN
# ------------------
AC_DEFUN([_LT_AC_CHECK_DLFCN],
[AC_CHECK_HEADERS(dlfcn.h)dnl
])# _LT_AC_CHECK_DLFCN


# _LT_AC_TRY_DLOPEN_SELF (ACTION-IF-TRUE, ACTION-IF-TRUE-W-USCORE,
#                           ACTION-IF-FALSE, ACTION-IF-CROSS-COMPILING)
# ---------------------------------------------------------------------
AC_DEFUN([_LT_AC_TRY_DLOPEN_SELF],
[AC_REQUIRE([_LT_AC_CHECK_DLFCN])dnl
if test "$cross_compiling" = yes; then :
  [$4]
else
  lt_dlunknown=0; lt_dlno_uscore=1; lt_dlneed_uscore=2
  lt_status=$lt_dlunknown
  cat > conftest.$ac_ext <<EOF
[#line __oline__ "configure"
#include "confdefs.h"

#if HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#include <stdio.h>

#ifdef RTLD_GLOBAL
#  define LT_DLGLOBAL		RTLD_GLOBAL
#else
#  ifdef DL_GLOBAL
#    define LT_DLGLOBAL		DL_GLOBAL
#  else
#    define LT_DLGLOBAL		0
#  endif
#endif

/* We may have to define LT_DLLAZY_OR_NOW in the command line if we
   find out it does not work in some platform. */
#ifndef LT_DLLAZY_OR_NOW
#  ifdef RTLD_LAZY
#    define LT_DLLAZY_OR_NOW		RTLD_LAZY
#  else
#    ifdef DL_LAZY
#      define LT_DLLAZY_OR_NOW		DL_LAZY
#    else
#      ifdef RTLD_NOW
#        define LT_DLLAZY_OR_NOW	RTLD_NOW
#      else
#        ifdef DL_NOW
#          define LT_DLLAZY_OR_NOW	DL_NOW
#        else
#          define LT_DLLAZY_OR_NOW	0
#        endif
#      endif
#    endif
#  endif
#endif

#ifdef __cplusplus
extern "C" void exit (int);
#endif

void fnord() { int i=42;}
int main ()
{
  void *self = dlopen (0, LT_DLGLOBAL|LT_DLLAZY_OR_NOW);
  int status = $lt_dlunknown;

  if (self)
    {
      if (dlsym (self,"fnord"))       status = $lt_dlno_uscore;
      else if (dlsym( self,"_fnord")) status = $lt_dlneed_uscore;
      /* dlclose (self); */
    }
  else
    puts (dlerror ());

    exit (status);
}]
EOF
  if AC_TRY_EVAL(ac_link) && test -s conftest${ac_exeext} 2>/dev/null; then
    (./conftest; exit; ) >&AS_MESSAGE_LOG_FD 2>/dev/null
    lt_status=$?
    case x$lt_status in
      x$lt_dlno_uscore) $1 ;;
      x$lt_dlneed_uscore) $2 ;;
      x$lt_dlunknown|x*) $3 ;;
    esac
  else :
    # compilation failed
    $3
  fi
fi
rm -fr conftest*
])# _LT_AC_TRY_DLOPEN_SELF


# AC_LIBTOOL_DLOPEN_SELF
# ----------------------
AC_DEFUN([AC_LIBTOOL_DLOPEN_SELF],
[AC_REQUIRE([_LT_AC_CHECK_DLFCN])dnl
if test "x$enable_dlopen" != xyes; then
  enable_dlopen=unknown
  enable_dlopen_self=unknown
  enable_dlopen_self_static=unknown
else
  lt_cv_dlopen=no
  lt_cv_dlopen_libs=

  case $host_os in
  beos*)
    lt_cv_dlopen="load_add_on"
    lt_cv_dlopen_libs=
    lt_cv_dlopen_self=yes
    ;;

  mingw* | pw32*)
    lt_cv_dlopen="LoadLibrary"
    lt_cv_dlopen_libs=
   ;;

  cygwin*)
    lt_cv_dlopen="dlopen"
    lt_cv_dlopen_libs=
   ;;

  darwin*)
  # if libdl is installed we need to link against it
    AC_CHECK_LIB([dl], [dlopen],
		[lt_cv_dlopen="dlopen" lt_cv_dlopen_libs="-ldl"],[
    lt_cv_dlopen="dyld"
    lt_cv_dlopen_libs=
    lt_cv_dlopen_self=yes
    ])
   ;;

  *)
    AC_CHECK_FUNC([shl_load],
	  [lt_cv_dlopen="shl_load"],
      [AC_CHECK_LIB([dld], [shl_load],
	    [lt_cv_dlopen="shl_load" lt_cv_dlopen_libs="-dld"],
	[AC_CHECK_FUNC([dlopen],
	      [lt_cv_dlopen="dlopen"],
	  [AC_CHECK_LIB([dl], [dlopen],
		[lt_cv_dlopen="dlopen" lt_cv_dlopen_libs="-ldl"],
	    [AC_CHECK_LIB([svld], [dlopen],
		  [lt_cv_dlopen="dlopen" lt_cv_dlopen_libs="-lsvld"],
	      [AC_CHECK_LIB([dld], [dld_link],
		    [lt_cv_dlopen="dld_link" lt_cv_dlopen_libs="-dld"])
	      ])
	    ])
	  ])
	])
      ])
    ;;
  esac

  if test "x$lt_cv_dlopen" != xno; then
    enable_dlopen=yes
  else
    enable_dlopen=no
  fi

  case $lt_cv_dlopen in
  dlopen)
    save_CPPFLAGS="$CPPFLAGS"
    test "x$ac_cv_header_dlfcn_h" = xyes && CPPFLAGS="$CPPFLAGS -DHAVE_DLFCN_H"

    save_LDFLAGS="$LDFLAGS"
    wl=$lt_prog_compiler_wl eval LDFLAGS=\"\$LDFLAGS $export_dynamic_flag_spec\"

    save_LIBS="$LIBS"
    LIBS="$lt_cv_dlopen_libs $LIBS"

    AC_CACHE_CHECK([whether a program can dlopen itself],
	  lt_cv_dlopen_self, [dnl
	  _LT_AC_TRY_DLOPEN_SELF(
	    lt_cv_dlopen_self=yes, lt_cv_dlopen_self=yes,
	    lt_cv_dlopen_self=no, lt_cv_dlopen_self=cross)
    ])

    if test "x$lt_cv_dlopen_self" = xyes; then
      wl=$lt_prog_compiler_wl eval LDFLAGS=\"\$LDFLAGS $lt_prog_compiler_static\"
      AC_CACHE_CHECK([whether a statically linked program can dlopen itself],
    	  lt_cv_dlopen_self_static, [dnl
	  _LT_AC_TRY_DLOPEN_SELF(
	    lt_cv_dlopen_self_static=yes, lt_cv_dlopen_self_static=yes,
	    lt_cv_dlopen_self_static=no,  lt_cv_dlopen_self_static=cross)
      ])
    fi

    CPPFLAGS="$save_CPPFLAGS"
    LDFLAGS="$save_LDFLAGS"
    LIBS="$save_LIBS"
    ;;
  esac

  case $lt_cv_dlopen_self in
  yes|no) enable_dlopen_self=$lt_cv_dlopen_self ;;
  *) enable_dlopen_self=unknown ;;
  esac

  case $lt_cv_dlopen_self_static in
  yes|no) enable_dlopen_self_static=$lt_cv_dlopen_self_static ;;
  *) enable_dlopen_self_static=unknown ;;
  esac
fi
])# AC_LIBTOOL_DLOPEN_SELF


# AC_LIBTOOL_PROG_CC_C_O([TAGNAME])
# ---------------------------------
# Check to see if options -c and -o are simultaneously supported by compiler
AC_DEFUN([AC_LIBTOOL_PROG_CC_C_O],
[AC_REQUIRE([_LT_AC_SYS_COMPILER])dnl
AC_CACHE_CHECK([if $compiler supports -c -o file.$ac_objext],
  [_LT_AC_TAGVAR(lt_cv_prog_compiler_c_o, $1)],
  [_LT_AC_TAGVAR(lt_cv_prog_compiler_c_o, $1)=no
   $rm -r conftest 2>/dev/null
   mkdir conftest
   cd conftest
   mkdir out
   printf "$lt_simple_compile_test_code" > conftest.$ac_ext

   lt_compiler_flag="-o out/conftest2.$ac_objext"
   # Insert the option either (1) after the last *FLAGS variable, or
   # (2) before a word containing "conftest.", or (3) at the end.
   # Note that $ac_compile itself does not contain backslashes and begins
   # with a dollar sign (not a hyphen), so the echo should work correctly.
   lt_compile=`echo "$ac_compile" | $SED \
   -e 's:.*FLAGS}\{0,1\} :&$lt_compiler_flag :; t' \
   -e 's: [[^ ]]*conftest\.: $lt_compiler_flag&:; t' \
   -e 's:$: $lt_compiler_flag:'`
   (eval echo "\"\$as_me:__oline__: $lt_compile\"" >&AS_MESSAGE_LOG_FD)
   (eval "$lt_compile" 2>out/conftest.err)
   ac_status=$?
   cat out/conftest.err >&AS_MESSAGE_LOG_FD
   echo "$as_me:__oline__: \$? = $ac_status" >&AS_MESSAGE_LOG_FD
   if (exit $ac_status) && test -s out/conftest2.$ac_objext
   then
     # The compiler can only warn and ignore the option if not recognized
     # So say no if there are warnings
     $echo "X$_lt_compiler_boilerplate" | $Xsed -e '/^$/d' > out/conftest.exp
     $SED '/^$/d; /^ *+/d' out/conftest.err >out/conftest.er2
     if test ! -s out/conftest.er2 || diff out/conftest.exp out/conftest.er2 >/dev/null; then
       _LT_AC_TAGVAR(lt_cv_prog_compiler_c_o, $1)=yes
     fi
   fi
   chmod u+w . 2>&AS_MESSAGE_LOG_FD
   $rm conftest*
   # SGI C++ compiler will create directory out/ii_files/ for
   # template instantiation
   test -d out/ii_files && $rm out/ii_files/* && rmdir out/ii_files
   $rm out/* && rmdir out
   cd ..
   rmdir conftest
   $rm conftest*
])
])# AC_LIBTOOL_PROG_CC_C_O


# AC_LIBTOOL_SYS_HARD_LINK_LOCKS([TAGNAME])
# -----------------------------------------
# Check to see if we can do hard links to lock some files if needed
AC_DEFUN([AC_LIBTOOL_SYS_HARD_LINK_LOCKS],
[AC_REQUIRE([_LT_AC_LOCK])dnl

hard_links="nottested"
if test "$_LT_AC_TAGVAR(lt_cv_prog_compiler_c_o, $1)" = no && test "$need_locks" != no; then
  # do not overwrite the value of need_locks provided by the user
  AC_MSG_CHECKING([if we can lock with hard links])
  hard_links=yes
  $rm conftest*
  ln conftest.a conftest.b 2>/dev/null && hard_links=no
  touch conftest.a
  ln conftest.a conftest.b 2>&5 || hard_links=no
  ln conftest.a conftest.b 2>/dev/null && hard_links=no
  AC_MSG_RESULT([$hard_links])
  if test "$hard_links" = no; then
    AC_MSG_WARN([`$CC' does not support `-c -o', so `make -j' may be unsafe])
    need_locks=warn
  fi
else
  need_locks=no
fi
])# AC_LIBTOOL_SYS_HARD_LINK_LOCKS


# AC_LIBTOOL_OBJDIR
# -----------------
AC_DEFUN([AC_LIBTOOL_OBJDIR],
[AC_CACHE_CHECK([for objdir], [lt_cv_objdir],
[rm -f .libs 2>/dev/null
mkdir .libs 2>/dev/null
if test -d .libs; then
  lt_cv_objdir=.libs
else
  # MS-DOS does not allow filenames that begin with a dot.
  lt_cv_objdir=_libs
fi
rmdir .libs 2>/dev/null])
objdir=$lt_cv_objdir
])# AC_LIBTOOL_OBJDIR


# AC_LIBTOOL_PROG_LD_HARDCODE_LIBPATH([TAGNAME])
# ----------------------------------------------
# Check hardcoding attributes.
AC_DEFUN([AC_LIBTOOL_PROG_LD_HARDCODE_LIBPATH],
[AC_MSG_CHECKING([how to hardcode library paths into programs])
_LT_AC_TAGVAR(hardcode_action, $1)=
if test -n "$_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)" || \
   test -n "$_LT_AC_TAGVAR(runpath_var, $1)" || \
   test "X$_LT_AC_TAGVAR(hardcode_automatic, $1)" = "Xyes" ; then

  # We can hardcode non-existant directories.
  if test "$_LT_AC_TAGVAR(hardcode_direct, $1)" != no &&
     # If the only mechanism to avoid hardcoding is shlibpath_var, we
     # have to relink, otherwise we might link with an installed library
     # when we should be linking with a yet-to-be-installed one
     ## test "$_LT_AC_TAGVAR(hardcode_shlibpath_var, $1)" != no &&
     test "$_LT_AC_TAGVAR(hardcode_minus_L, $1)" != no; then
    # Linking always hardcodes the temporary library directory.
    _LT_AC_TAGVAR(hardcode_action, $1)=relink
  else
    # We can link without hardcoding, and we can hardcode nonexisting dirs.
    _LT_AC_TAGVAR(hardcode_action, $1)=immediate
  fi
else
  # We cannot hardcode anything, or else we can only hardcode existing
  # directories.
  _LT_AC_TAGVAR(hardcode_action, $1)=unsupported
fi
AC_MSG_RESULT([$_LT_AC_TAGVAR(hardcode_action, $1)])

if test "$_LT_AC_TAGVAR(hardcode_action, $1)" = relink; then
  # Fast installation is not supported
  enable_fast_install=no
elif test "$shlibpath_overrides_runpath" = yes ||
     test "$enable_shared" = no; then
  # Fast installation is not necessary
  enable_fast_install=needless
fi
])# AC_LIBTOOL_PROG_LD_HARDCODE_LIBPATH


# AC_LIBTOOL_SYS_LIB_STRIP
# ------------------------
AC_DEFUN([AC_LIBTOOL_SYS_LIB_STRIP],
[striplib=
old_striplib=
AC_MSG_CHECKING([whether stripping libraries is possible])
if test -n "$STRIP" && $STRIP -V 2>&1 | grep "GNU strip" >/dev/null; then
  test -z "$old_striplib" && old_striplib="$STRIP --strip-debug"
  test -z "$striplib" && striplib="$STRIP --strip-unneeded"
  AC_MSG_RESULT([yes])
else
# FIXME - insert some real tests, host_os isn't really good enough
  case $host_os in
   darwin*)
       if test -n "$STRIP" ; then
         striplib="$STRIP -x"
         AC_MSG_RESULT([yes])
       else
  AC_MSG_RESULT([no])
fi
       ;;
   *)
  AC_MSG_RESULT([no])
    ;;
  esac
fi
])# AC_LIBTOOL_SYS_LIB_STRIP


# AC_LIBTOOL_SYS_DYNAMIC_LINKER
# -----------------------------
# PORTME Fill in your ld.so characteristics
AC_DEFUN([AC_LIBTOOL_SYS_DYNAMIC_LINKER],
[AC_MSG_CHECKING([dynamic linker characteristics])
library_names_spec=
libname_spec='lib$name'
soname_spec=
shrext_cmds=".so"
postinstall_cmds=
postuninstall_cmds=
finish_cmds=
finish_eval=
shlibpath_var=
shlibpath_overrides_runpath=unknown
version_type=none
dynamic_linker="$host_os ld.so"
sys_lib_dlsearch_path_spec="/lib /usr/lib"
if test "$GCC" = yes; then
  sys_lib_search_path_spec=`$CC -print-search-dirs | grep "^libraries:" | $SED -e "s/^libraries://" -e "s,=/,/,g"`
  if echo "$sys_lib_search_path_spec" | grep ';' >/dev/null ; then
    # if the path contains ";" then we assume it to be the separator
    # otherwise default to the standard path separator (i.e. ":") - it is
    # assumed that no part of a normal pathname contains ";" but that should
    # okay in the real world where ";" in dirpaths is itself problematic.
    sys_lib_search_path_spec=`echo "$sys_lib_search_path_spec" | $SED -e 's/;/ /g'`
  else
    sys_lib_search_path_spec=`echo "$sys_lib_search_path_spec" | $SED  -e "s/$PATH_SEPARATOR/ /g"`
  fi
else
  sys_lib_search_path_spec="/lib /usr/lib /usr/local/lib"
fi
need_lib_prefix=unknown
hardcode_into_libs=no

# when you set need_version to no, make sure it does not cause -set_version
# flags to be left without arguments
need_version=unknown

case $host_os in
aix3*)
  version_type=linux
  library_names_spec='${libname}${release}${shared_ext}$versuffix $libname.a'
  shlibpath_var=LIBPATH

  # AIX 3 has no versioning support, so we append a major version to the name.
  soname_spec='${libname}${release}${shared_ext}$major'
  ;;

aix4* | aix5*)
  version_type=linux
  need_lib_prefix=no
  need_version=no
  hardcode_into_libs=yes
  if test "$host_cpu" = ia64; then
    # AIX 5 supports IA64
    library_names_spec='${libname}${release}${shared_ext}$major ${libname}${release}${shared_ext}$versuffix $libname${shared_ext}'
    shlibpath_var=LD_LIBRARY_PATH
  else
    # With GCC up to 2.95.x, collect2 would create an import file
    # for dependence libraries.  The import file would start with
    # the line `#! .'.  This would cause the generated library to
    # depend on `.', always an invalid library.  This was fixed in
    # development snapshots of GCC prior to 3.0.
    case $host_os in
      aix4 | aix4.[[01]] | aix4.[[01]].*)
      if { echo '#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 97)'
	   echo ' yes '
	   echo '#endif'; } | ${CC} -E - | grep yes > /dev/null; then
	:
      else
	can_build_shared=no
      fi
      ;;
    esac
    # AIX (on Power*) has no versioning support, so currently we can not hardcode correct
    # soname into executable. Probably we can add versioning support to
    # collect2, so additional links can be useful in future.
    if test "$aix_use_runtimelinking" = yes; then
      # If using run time linking (on AIX 4.2 or later) use lib<name>.so
      # instead of lib<name>.a to let people know that these are not
      # typical AIX shared libraries.
      library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major $libname${shared_ext}'
    else
      # We preserve .a as extension for shared libraries through AIX4.2
      # and later when we are not doing run time linking.
      library_names_spec='${libname}${release}.a $libname.a'
      soname_spec='${libname}${release}${shared_ext}$major'
    fi
    shlibpath_var=LIBPATH
  fi
  ;;

amigaos*)
  library_names_spec='$libname.ixlibrary $libname.a'
  # Create ${libname}_ixlibrary.a entries in /sys/libs.
  finish_eval='for lib in `ls $libdir/*.ixlibrary 2>/dev/null`; do libname=`$echo "X$lib" | $Xsed -e '\''s%^.*/\([[^/]]*\)\.ixlibrary$%\1%'\''`; test $rm /sys/libs/${libname}_ixlibrary.a; $show "cd /sys/libs && $LN_S $lib ${libname}_ixlibrary.a"; cd /sys/libs && $LN_S $lib ${libname}_ixlibrary.a || exit 1; done'
  ;;

beos*)
  library_names_spec='${libname}${shared_ext}'
  dynamic_linker="$host_os ld.so"
  shlibpath_var=LIBRARY_PATH
  ;;

bsdi[[45]]*)
  version_type=linux
  need_version=no
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major $libname${shared_ext}'
  soname_spec='${libname}${release}${shared_ext}$major'
  finish_cmds='PATH="\$PATH:/sbin" ldconfig $libdir'
  shlibpath_var=LD_LIBRARY_PATH
  sys_lib_search_path_spec="/shlib /usr/lib /usr/X11/lib /usr/contrib/lib /lib /usr/local/lib"
  sys_lib_dlsearch_path_spec="/shlib /usr/lib /usr/local/lib"
  # the default ld.so.conf also contains /usr/contrib/lib and
  # /usr/X11R6/lib (/usr/X11 is a link to /usr/X11R6), but let us allow
  # libtool to hard-code these into programs
  ;;

cygwin* | mingw* | pw32*)
  version_type=windows
  shrext_cmds=".dll"
  need_version=no
  need_lib_prefix=no

  case $GCC,$host_os in
  yes,cygwin* | yes,mingw* | yes,pw32*)
    library_names_spec='$libname.dll.a'
    # DLL is installed to $(libdir)/../bin by postinstall_cmds
    postinstall_cmds='base_file=`basename \${file}`~
      dlpath=`$SHELL 2>&1 -c '\''. $dir/'\''\${base_file}'\''i;echo \$dlname'\''`~
      dldir=$destdir/`dirname \$dlpath`~
      test -d \$dldir || mkdir -p \$dldir~
      $install_prog $dir/$dlname \$dldir/$dlname~
      chmod a+x \$dldir/$dlname'
    postuninstall_cmds='dldll=`$SHELL 2>&1 -c '\''. $file; echo \$dlname'\''`~
      dlpath=$dir/\$dldll~
       $rm \$dlpath'
    shlibpath_overrides_runpath=yes

    case $host_os in
    cygwin*)
      # Cygwin DLLs use 'cyg' prefix rather than 'lib'
      soname_spec='`echo ${libname} | sed -e 's/^lib/cyg/'``echo ${release} | $SED -e 's/[[.]]/-/g'`${versuffix}${shared_ext}'
      sys_lib_search_path_spec="/usr/lib /lib/w32api /lib /usr/local/lib"
      ;;
    mingw*)
      # MinGW DLLs use traditional 'lib' prefix
      soname_spec='${libname}`echo ${release} | $SED -e 's/[[.]]/-/g'`${versuffix}${shared_ext}'
      sys_lib_search_path_spec=`$CC -print-search-dirs | grep "^libraries:" | $SED -e "s/^libraries://" -e "s,=/,/,g"`
      if echo "$sys_lib_search_path_spec" | [grep ';[c-zC-Z]:/' >/dev/null]; then
        # It is most probably a Windows format PATH printed by
        # mingw gcc, but we are running on Cygwin. Gcc prints its search
        # path with ; separators, and with drive letters. We can handle the
        # drive letters (cygwin fileutils understands them), so leave them,
        # especially as we might pass files found there to a mingw objdump,
        # which wouldn't understand a cygwinified path. Ahh.
        sys_lib_search_path_spec=`echo "$sys_lib_search_path_spec" | $SED -e 's/;/ /g'`
      else
        sys_lib_search_path_spec=`echo "$sys_lib_search_path_spec" | $SED  -e "s/$PATH_SEPARATOR/ /g"`
      fi
      ;;
    pw32*)
      # pw32 DLLs use 'pw' prefix rather than 'lib'
      library_names_spec='`echo ${libname} | sed -e 's/^lib/pw/'``echo ${release} | $SED -e 's/[[.]]/-/g'`${versuffix}${shared_ext}'
      ;;
    esac
    ;;

  *)
    library_names_spec='${libname}`echo ${release} | $SED -e 's/[[.]]/-/g'`${versuffix}${shared_ext} $libname.lib'
    ;;
  esac
  dynamic_linker='Win32 ld.exe'
  # FIXME: first we should search . and the directory the executable is in
  shlibpath_var=PATH
  ;;

darwin* | rhapsody*)
  dynamic_linker="$host_os dyld"
  version_type=darwin
  need_lib_prefix=no
  need_version=no
  library_names_spec='${libname}${release}${versuffix}$shared_ext ${libname}${release}${major}$shared_ext ${libname}$shared_ext'
  soname_spec='${libname}${release}${major}$shared_ext'
  shlibpath_overrides_runpath=yes
  shlibpath_var=DYLD_LIBRARY_PATH
  shrext_cmds='`test .$module = .yes && echo .so || echo .dylib`'
  # Apple's gcc prints 'gcc -print-search-dirs' doesn't operate the same.
  if test "$GCC" = yes; then
    sys_lib_search_path_spec=`$CC -print-search-dirs | tr "\n" "$PATH_SEPARATOR" | sed -e 's/libraries:/@libraries:/' | tr "@" "\n" | grep "^libraries:" | sed -e "s/^libraries://" -e "s,=/,/,g" -e "s,$PATH_SEPARATOR, ,g" -e "s,.*,& /lib /usr/lib /usr/local/lib,g"`
  else
    sys_lib_search_path_spec='/lib /usr/lib /usr/local/lib'
  fi
  sys_lib_dlsearch_path_spec='/usr/local/lib /lib /usr/lib'
  ;;

dgux*)
  version_type=linux
  need_lib_prefix=no
  need_version=no
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major $libname$shared_ext'
  soname_spec='${libname}${release}${shared_ext}$major'
  shlibpath_var=LD_LIBRARY_PATH
  ;;

freebsd1*)
  dynamic_linker=no
  ;;

kfreebsd*-gnu)
  version_type=linux
  need_lib_prefix=no
  need_version=no
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major ${libname}${shared_ext}'
  soname_spec='${libname}${release}${shared_ext}$major'
  shlibpath_var=LD_LIBRARY_PATH
  shlibpath_overrides_runpath=no
  hardcode_into_libs=yes
  dynamic_linker='GNU ld.so'
  ;;

freebsd* | dragonfly*)
  # DragonFly does not have aout.  When/if they implement a new
  # versioning mechanism, adjust this.
  if test -x /usr/bin/objformat; then
    objformat=`/usr/bin/objformat`
  else
    case $host_os in
    freebsd[[123]]*) objformat=aout ;;
    *) objformat=elf ;;
    esac
  fi
  version_type=freebsd-$objformat
  case $version_type in
    freebsd-elf*)
      library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext} $libname${shared_ext}'
      need_version=no
      need_lib_prefix=no
      ;;
    freebsd-*)
      library_names_spec='${libname}${release}${shared_ext}$versuffix $libname${shared_ext}$versuffix'
      need_version=yes
      ;;
  esac
  shlibpath_var=LD_LIBRARY_PATH
  case $host_os in
  freebsd2*)
    shlibpath_overrides_runpath=yes
    ;;
  freebsd3.[[01]]* | freebsdelf3.[[01]]*)
    shlibpath_overrides_runpath=yes
    hardcode_into_libs=yes
    ;;
  freebsd3.[[2-9]]* | freebsdelf3.[[2-9]]* | \
  freebsd4.[[0-5]] | freebsdelf4.[[0-5]] | freebsd4.1.1 | freebsdelf4.1.1)
    shlibpath_overrides_runpath=no
    hardcode_into_libs=yes
    ;;
  freebsd*) # from 4.6 on
    shlibpath_overrides_runpath=yes
    hardcode_into_libs=yes
    ;;
  esac
  ;;

gnu*)
  version_type=linux
  need_lib_prefix=no
  need_version=no
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}${major} ${libname}${shared_ext}'
  soname_spec='${libname}${release}${shared_ext}$major'
  shlibpath_var=LD_LIBRARY_PATH
  hardcode_into_libs=yes
  ;;

hpux9* | hpux10* | hpux11*)
  # Give a soname corresponding to the major version so that dld.sl refuses to
  # link against other versions.
  version_type=sunos
  need_lib_prefix=no
  need_version=no
  case $host_cpu in
  ia64*)
    shrext_cmds='.so'
    hardcode_into_libs=yes
    dynamic_linker="$host_os dld.so"
    shlibpath_var=LD_LIBRARY_PATH
    shlibpath_overrides_runpath=yes # Unless +noenvvar is specified.
    library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major $libname${shared_ext}'
    soname_spec='${libname}${release}${shared_ext}$major'
    if test "X$HPUX_IA64_MODE" = X32; then
      sys_lib_search_path_spec="/usr/lib/hpux32 /usr/local/lib/hpux32 /usr/local/lib"
    else
      sys_lib_search_path_spec="/usr/lib/hpux64 /usr/local/lib/hpux64"
    fi
    sys_lib_dlsearch_path_spec=$sys_lib_search_path_spec
    ;;
   hppa*64*)
     shrext_cmds='.sl'
     hardcode_into_libs=yes
     dynamic_linker="$host_os dld.sl"
     shlibpath_var=LD_LIBRARY_PATH # How should we handle SHLIB_PATH
     shlibpath_overrides_runpath=yes # Unless +noenvvar is specified.
     library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major $libname${shared_ext}'
     soname_spec='${libname}${release}${shared_ext}$major'
     sys_lib_search_path_spec="/usr/lib/pa20_64 /usr/ccs/lib/pa20_64"
     sys_lib_dlsearch_path_spec=$sys_lib_search_path_spec
     ;;
   *)
    shrext_cmds='.sl'
    dynamic_linker="$host_os dld.sl"
    shlibpath_var=SHLIB_PATH
    shlibpath_overrides_runpath=no # +s is required to enable SHLIB_PATH
    library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major $libname${shared_ext}'
    soname_spec='${libname}${release}${shared_ext}$major'
    ;;
  esac
  # HP-UX runs *really* slowly unless shared libraries are mode 555.
  postinstall_cmds='chmod 555 $lib'
  ;;

interix3*)
  version_type=linux
  need_lib_prefix=no
  need_version=no
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major ${libname}${shared_ext}'
  soname_spec='${libname}${release}${shared_ext}$major'
  dynamic_linker='Interix 3.x ld.so.1 (PE, like ELF)'
  shlibpath_var=LD_LIBRARY_PATH
  shlibpath_overrides_runpath=no
  hardcode_into_libs=yes
  ;;

irix5* | irix6* | nonstopux*)
  case $host_os in
    nonstopux*) version_type=nonstopux ;;
    *)
	if test "$lt_cv_prog_gnu_ld" = yes; then
		version_type=linux
	else
		version_type=irix
	fi ;;
  esac
  need_lib_prefix=no
  need_version=no
  soname_spec='${libname}${release}${shared_ext}$major'
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major ${libname}${release}${shared_ext} $libname${shared_ext}'
  case $host_os in
  irix5* | nonstopux*)
    libsuff= shlibsuff=
    ;;
  *)
    case $LD in # libtool.m4 will add one of these switches to LD
    *-32|*"-32 "|*-melf32bsmip|*"-melf32bsmip ")
      libsuff= shlibsuff= libmagic=32-bit;;
    *-n32|*"-n32 "|*-melf32bmipn32|*"-melf32bmipn32 ")
      libsuff=32 shlibsuff=N32 libmagic=N32;;
    *-64|*"-64 "|*-melf64bmip|*"-melf64bmip ")
      libsuff=64 shlibsuff=64 libmagic=64-bit;;
    *) libsuff= shlibsuff= libmagic=never-match;;
    esac
    ;;
  esac
  shlibpath_var=LD_LIBRARY${shlibsuff}_PATH
  shlibpath_overrides_runpath=no
  sys_lib_search_path_spec="/usr/lib${libsuff} /lib${libsuff} /usr/local/lib${libsuff}"
  sys_lib_dlsearch_path_spec="/usr/lib${libsuff} /lib${libsuff}"
  hardcode_into_libs=yes
  ;;

# No shared lib support for Linux oldld, aout, or coff.
linux*oldld* | linux*aout* | linux*coff*)
  dynamic_linker=no
  ;;

# This must be Linux ELF.
linux*)
  version_type=linux
  need_lib_prefix=no
  need_version=no
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major $libname${shared_ext}'
  soname_spec='${libname}${release}${shared_ext}$major'
  finish_cmds='PATH="\$PATH:/sbin" ldconfig -n $libdir'
  shlibpath_var=LD_LIBRARY_PATH
  shlibpath_overrides_runpath=no
  # This implies no fast_install, which is unacceptable.
  # Some rework will be needed to allow for fast_install
  # before this can be enabled.
  hardcode_into_libs=yes

  # find out which ABI we are using
  libsuff=
  case "$host_cpu" in
  x86_64*|s390x*|powerpc64*)
    echo '[#]line __oline__ "configure"' > conftest.$ac_ext
    if AC_TRY_EVAL(ac_compile); then
      case `/usr/bin/file conftest.$ac_objext` in
      *64-bit*)
        libsuff=64
        sys_lib_search_path_spec="/lib${libsuff} /usr/lib${libsuff} /usr/local/lib${libsuff}"
        ;;
      esac
    fi
    rm -rf conftest*
    ;;
  esac

  # Append ld.so.conf contents to the search path
  if test -f /etc/ld.so.conf; then
    lt_ld_extra=`awk '/^include / { system(sprintf("cd /etc; cat %s 2>/dev/null", \[$]2)); skip = 1; } { if (!skip) print \[$]0; skip = 0; }' < /etc/ld.so.conf | $SED -e 's/#.*//;s/[:,	]/ /g;s/=[^=]*$//;s/=[^= ]* / /g;/^$/d' | tr '\n' ' '`
    sys_lib_dlsearch_path_spec="/lib${libsuff} /usr/lib${libsuff} $lt_ld_extra"
  fi

  # We used to test for /lib/ld.so.1 and disable shared libraries on
  # powerpc, because MkLinux only supported shared libraries with the
  # GNU dynamic linker.  Since this was broken with cross compilers,
  # most powerpc-linux boxes support dynamic linking these days and
  # people can always --disable-shared, the test was removed, and we
  # assume the GNU/Linux dynamic linker is in use.
  dynamic_linker='GNU/Linux ld.so'
  ;;

knetbsd*-gnu)
  version_type=linux
  need_lib_prefix=no
  need_version=no
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major ${libname}${shared_ext}'
  soname_spec='${libname}${release}${shared_ext}$major'
  shlibpath_var=LD_LIBRARY_PATH
  shlibpath_overrides_runpath=no
  hardcode_into_libs=yes
  dynamic_linker='GNU ld.so'
  ;;

netbsd*)
  version_type=sunos
  need_lib_prefix=no
  need_version=no
  if echo __ELF__ | $CC -E - | grep __ELF__ >/dev/null; then
    library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${shared_ext}$versuffix'
    finish_cmds='PATH="\$PATH:/sbin" ldconfig -m $libdir'
    dynamic_linker='NetBSD (a.out) ld.so'
  else
    library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major ${libname}${shared_ext}'
    soname_spec='${libname}${release}${shared_ext}$major'
    dynamic_linker='NetBSD ld.elf_so'
  fi
  shlibpath_var=LD_LIBRARY_PATH
  shlibpath_overrides_runpath=yes
  hardcode_into_libs=yes
  ;;

newsos6)
  version_type=linux
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major $libname${shared_ext}'
  shlibpath_var=LD_LIBRARY_PATH
  shlibpath_overrides_runpath=yes
  ;;

nto-qnx*)
  version_type=linux
  need_lib_prefix=no
  need_version=no
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major $libname${shared_ext}'
  soname_spec='${libname}${release}${shared_ext}$major'
  shlibpath_var=LD_LIBRARY_PATH
  shlibpath_overrides_runpath=yes
  ;;

openbsd*)
  version_type=sunos
  sys_lib_dlsearch_path_spec="/usr/lib"
  need_lib_prefix=no
  # Some older versions of OpenBSD (3.3 at least) *do* need versioned libs.
  case $host_os in
    openbsd3.3 | openbsd3.3.*) need_version=yes ;;
    *)                         need_version=no  ;;
  esac
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${shared_ext}$versuffix'
  finish_cmds='PATH="\$PATH:/sbin" ldconfig -m $libdir'
  shlibpath_var=LD_LIBRARY_PATH
  if test -z "`echo __ELF__ | $CC -E - | grep __ELF__`" || test "$host_os-$host_cpu" = "openbsd2.8-powerpc"; then
    case $host_os in
      openbsd2.[[89]] | openbsd2.[[89]].*)
	shlibpath_overrides_runpath=no
	;;
      *)
	shlibpath_overrides_runpath=yes
	;;
      esac
  else
    shlibpath_overrides_runpath=yes
  fi
  ;;

os2*)
  libname_spec='$name'
  shrext_cmds=".dll"
  need_lib_prefix=no
  library_names_spec='$libname${shared_ext} $libname.a'
  dynamic_linker='OS/2 ld.exe'
  shlibpath_var=LIBPATH
  ;;

osf3* | osf4* | osf5*)
  version_type=osf
  need_lib_prefix=no
  need_version=no
  soname_spec='${libname}${release}${shared_ext}$major'
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major $libname${shared_ext}'
  shlibpath_var=LD_LIBRARY_PATH
  sys_lib_search_path_spec="/usr/shlib /usr/ccs/lib /usr/lib/cmplrs/cc /usr/lib /usr/local/lib /var/shlib"
  sys_lib_dlsearch_path_spec="$sys_lib_search_path_spec"
  ;;

solaris*)
  version_type=linux
  need_lib_prefix=no
  need_version=no
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major $libname${shared_ext}'
  soname_spec='${libname}${release}${shared_ext}$major'
  shlibpath_var=LD_LIBRARY_PATH
  shlibpath_overrides_runpath=yes
  hardcode_into_libs=yes
  # ldd complains unless libraries are executable
  postinstall_cmds='chmod +x $lib'
  ;;

sunos4*)
  version_type=sunos
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${shared_ext}$versuffix'
  finish_cmds='PATH="\$PATH:/usr/etc" ldconfig $libdir'
  shlibpath_var=LD_LIBRARY_PATH
  shlibpath_overrides_runpath=yes
  if test "$with_gnu_ld" = yes; then
    need_lib_prefix=no
  fi
  need_version=yes
  ;;

sysv4 | sysv4.3*)
  version_type=linux
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major $libname${shared_ext}'
  soname_spec='${libname}${release}${shared_ext}$major'
  shlibpath_var=LD_LIBRARY_PATH
  case $host_vendor in
    sni)
      shlibpath_overrides_runpath=no
      need_lib_prefix=no
      export_dynamic_flag_spec='${wl}-Blargedynsym'
      runpath_var=LD_RUN_PATH
      ;;
    siemens)
      need_lib_prefix=no
      ;;
    motorola)
      need_lib_prefix=no
      need_version=no
      shlibpath_overrides_runpath=no
      sys_lib_search_path_spec='/lib /usr/lib /usr/ccs/lib'
      ;;
  esac
  ;;

sysv4*MP*)
  if test -d /usr/nec ;then
    version_type=linux
    library_names_spec='$libname${shared_ext}.$versuffix $libname${shared_ext}.$major $libname${shared_ext}'
    soname_spec='$libname${shared_ext}.$major'
    shlibpath_var=LD_LIBRARY_PATH
  fi
  ;;

sysv5* | sco3.2v5* | sco5v6* | unixware* | OpenUNIX* | sysv4*uw2*)
  version_type=freebsd-elf
  need_lib_prefix=no
  need_version=no
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext} $libname${shared_ext}'
  soname_spec='${libname}${release}${shared_ext}$major'
  shlibpath_var=LD_LIBRARY_PATH
  hardcode_into_libs=yes
  if test "$with_gnu_ld" = yes; then
    sys_lib_search_path_spec='/usr/local/lib /usr/gnu/lib /usr/ccs/lib /usr/lib /lib'
    shlibpath_overrides_runpath=no
  else
    sys_lib_search_path_spec='/usr/ccs/lib /usr/lib'
    shlibpath_overrides_runpath=yes
    case $host_os in
      sco3.2v5*)
        sys_lib_search_path_spec="$sys_lib_search_path_spec /lib"
	;;
    esac
  fi
  sys_lib_dlsearch_path_spec='/usr/lib'
  ;;

uts4*)
  version_type=linux
  library_names_spec='${libname}${release}${shared_ext}$versuffix ${libname}${release}${shared_ext}$major $libname${shared_ext}'
  soname_spec='${libname}${release}${shared_ext}$major'
  shlibpath_var=LD_LIBRARY_PATH
  ;;

*)
  dynamic_linker=no
  ;;
esac
AC_MSG_RESULT([$dynamic_linker])
test "$dynamic_linker" = no && can_build_shared=no

variables_saved_for_relink="PATH $shlibpath_var $runpath_var"
if test "$GCC" = yes; then
  variables_saved_for_relink="$variables_saved_for_relink GCC_EXEC_PREFIX COMPILER_PATH LIBRARY_PATH"
fi
])# AC_LIBTOOL_SYS_DYNAMIC_LINKER


# _LT_AC_TAGCONFIG
# ----------------
AC_DEFUN([_LT_AC_TAGCONFIG],
[AC_ARG_WITH([tags],
    [AC_HELP_STRING([--with-tags@<:@=TAGS@:>@],
        [include additional configurations @<:@automatic@:>@])],
    [tagnames="$withval"])

if test -f "$ltmain" && test -n "$tagnames"; then
  if test ! -f "${ofile}"; then
    AC_MSG_WARN([output file `$ofile' does not exist])
  fi

  if test -z "$LTCC"; then
    eval "`$SHELL ${ofile} --config | grep '^LTCC='`"
    if test -z "$LTCC"; then
      AC_MSG_WARN([output file `$ofile' does not look like a libtool script])
    else
      AC_MSG_WARN([using `LTCC=$LTCC', extracted from `$ofile'])
    fi
  fi
  if test -z "$LTCFLAGS"; then
    eval "`$SHELL ${ofile} --config | grep '^LTCFLAGS='`"
  fi

  # Extract list of available tagged configurations in $ofile.
  # Note that this assumes the entire list is on one line.
  available_tags=`grep "^available_tags=" "${ofile}" | $SED -e 's/available_tags=\(.*$\)/\1/' -e 's/\"//g'`

  lt_save_ifs="$IFS"; IFS="${IFS}$PATH_SEPARATOR,"
  for tagname in $tagnames; do
    IFS="$lt_save_ifs"
    # Check whether tagname contains only valid characters
    case `$echo "X$tagname" | $Xsed -e 's:[[-_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890,/]]::g'` in
    "") ;;
    *)  AC_MSG_ERROR([invalid tag name: $tagname])
	;;
    esac

    if grep "^# ### BEGIN LIBTOOL TAG CONFIG: $tagname$" < "${ofile}" > /dev/null
    then
      AC_MSG_ERROR([tag name \"$tagname\" already exists])
    fi

    # Update the list of available tags.
    if test -n "$tagname"; then
      echo appending configuration tag \"$tagname\" to $ofile

      case $tagname in
      CXX)
	if test -n "$CXX" && ( test "X$CXX" != "Xno" &&
	    ( (test "X$CXX" = "Xg++" && `g++ -v >/dev/null 2>&1` ) ||
	    (test "X$CXX" != "Xg++"))) ; then
	  AC_LIBTOOL_LANG_CXX_CONFIG
	else
	  tagname=""
	fi
	;;

      F77)
	if test -n "$F77" && test "X$F77" != "Xno"; then
	  AC_LIBTOOL_LANG_F77_CONFIG
	else
	  tagname=""
	fi
	;;

      GCJ)
	if test -n "$GCJ" && test "X$GCJ" != "Xno"; then
	  AC_LIBTOOL_LANG_GCJ_CONFIG
	else
	  tagname=""
	fi
	;;

      RC)
	AC_LIBTOOL_LANG_RC_CONFIG
	;;

      *)
	AC_MSG_ERROR([Unsupported tag name: $tagname])
	;;
      esac

      # Append the new tag name to the list of available tags.
      if test -n "$tagname" ; then
      available_tags="$available_tags $tagname"
    fi
    fi
  done
  IFS="$lt_save_ifs"

  # Now substitute the updated list of available tags.
  if eval "sed -e 's/^available_tags=.*\$/available_tags=\"$available_tags\"/' \"$ofile\" > \"${ofile}T\""; then
    mv "${ofile}T" "$ofile"
    chmod +x "$ofile"
  else
    rm -f "${ofile}T"
    AC_MSG_ERROR([unable to update list of available tagged configurations.])
  fi
fi
])# _LT_AC_TAGCONFIG


# AC_LIBTOOL_DLOPEN
# -----------------
# enable checks for dlopen support
AC_DEFUN([AC_LIBTOOL_DLOPEN],
 [AC_BEFORE([$0],[AC_LIBTOOL_SETUP])
])# AC_LIBTOOL_DLOPEN


# AC_LIBTOOL_WIN32_DLL
# --------------------
# declare package support for building win32 DLLs
AC_DEFUN([AC_LIBTOOL_WIN32_DLL],
[AC_BEFORE([$0], [AC_LIBTOOL_SETUP])
])# AC_LIBTOOL_WIN32_DLL


# AC_ENABLE_SHARED([DEFAULT])
# ---------------------------
# implement the --enable-shared flag
# DEFAULT is either `yes' or `no'.  If omitted, it defaults to `yes'.
AC_DEFUN([AC_ENABLE_SHARED],
[define([AC_ENABLE_SHARED_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE([shared],
    [AC_HELP_STRING([--enable-shared@<:@=PKGS@:>@],
	[build shared libraries @<:@default=]AC_ENABLE_SHARED_DEFAULT[@:>@])],
    [p=${PACKAGE-default}
    case $enableval in
    yes) enable_shared=yes ;;
    no) enable_shared=no ;;
    *)
      enable_shared=no
      # Look at the argument we got.  We use all the common list separators.
      lt_save_ifs="$IFS"; IFS="${IFS}$PATH_SEPARATOR,"
      for pkg in $enableval; do
	IFS="$lt_save_ifs"
	if test "X$pkg" = "X$p"; then
	  enable_shared=yes
	fi
      done
      IFS="$lt_save_ifs"
      ;;
    esac],
    [enable_shared=]AC_ENABLE_SHARED_DEFAULT)
])# AC_ENABLE_SHARED


# AC_DISABLE_SHARED
# -----------------
# set the default shared flag to --disable-shared
AC_DEFUN([AC_DISABLE_SHARED],
[AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
AC_ENABLE_SHARED(no)
])# AC_DISABLE_SHARED


# AC_ENABLE_STATIC([DEFAULT])
# ---------------------------
# implement the --enable-static flag
# DEFAULT is either `yes' or `no'.  If omitted, it defaults to `yes'.
AC_DEFUN([AC_ENABLE_STATIC],
[define([AC_ENABLE_STATIC_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE([static],
    [AC_HELP_STRING([--enable-static@<:@=PKGS@:>@],
	[build static libraries @<:@default=]AC_ENABLE_STATIC_DEFAULT[@:>@])],
    [p=${PACKAGE-default}
    case $enableval in
    yes) enable_static=yes ;;
    no) enable_static=no ;;
    *)
     enable_static=no
      # Look at the argument we got.  We use all the common list separators.
      lt_save_ifs="$IFS"; IFS="${IFS}$PATH_SEPARATOR,"
      for pkg in $enableval; do
	IFS="$lt_save_ifs"
	if test "X$pkg" = "X$p"; then
	  enable_static=yes
	fi
      done
      IFS="$lt_save_ifs"
      ;;
    esac],
    [enable_static=]AC_ENABLE_STATIC_DEFAULT)
])# AC_ENABLE_STATIC


# AC_DISABLE_STATIC
# -----------------
# set the default static flag to --disable-static
AC_DEFUN([AC_DISABLE_STATIC],
[AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
AC_ENABLE_STATIC(no)
])# AC_DISABLE_STATIC


# AC_ENABLE_FAST_INSTALL([DEFAULT])
# ---------------------------------
# implement the --enable-fast-install flag
# DEFAULT is either `yes' or `no'.  If omitted, it defaults to `yes'.
AC_DEFUN([AC_ENABLE_FAST_INSTALL],
[define([AC_ENABLE_FAST_INSTALL_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE([fast-install],
    [AC_HELP_STRING([--enable-fast-install@<:@=PKGS@:>@],
    [optimize for fast installation @<:@default=]AC_ENABLE_FAST_INSTALL_DEFAULT[@:>@])],
    [p=${PACKAGE-default}
    case $enableval in
    yes) enable_fast_install=yes ;;
    no) enable_fast_install=no ;;
    *)
      enable_fast_install=no
      # Look at the argument we got.  We use all the common list separators.
      lt_save_ifs="$IFS"; IFS="${IFS}$PATH_SEPARATOR,"
      for pkg in $enableval; do
	IFS="$lt_save_ifs"
	if test "X$pkg" = "X$p"; then
	  enable_fast_install=yes
	fi
      done
      IFS="$lt_save_ifs"
      ;;
    esac],
    [enable_fast_install=]AC_ENABLE_FAST_INSTALL_DEFAULT)
])# AC_ENABLE_FAST_INSTALL


# AC_DISABLE_FAST_INSTALL
# -----------------------
# set the default to --disable-fast-install
AC_DEFUN([AC_DISABLE_FAST_INSTALL],
[AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
AC_ENABLE_FAST_INSTALL(no)
])# AC_DISABLE_FAST_INSTALL


# AC_LIBTOOL_PICMODE([MODE])
# --------------------------
# implement the --with-pic flag
# MODE is either `yes' or `no'.  If omitted, it defaults to `both'.
AC_DEFUN([AC_LIBTOOL_PICMODE],
[AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
pic_mode=ifelse($#,1,$1,default)
])# AC_LIBTOOL_PICMODE


# AC_PROG_EGREP
# -------------
# This is predefined starting with Autoconf 2.54, so this conditional
# definition can be removed once we require Autoconf 2.54 or later.
m4_ifndef([AC_PROG_EGREP], [AC_DEFUN([AC_PROG_EGREP],
[AC_CACHE_CHECK([for egrep], [ac_cv_prog_egrep],
   [if echo a | (grep -E '(a|b)') >/dev/null 2>&1
    then ac_cv_prog_egrep='grep -E'
    else ac_cv_prog_egrep='egrep'
    fi])
 EGREP=$ac_cv_prog_egrep
 AC_SUBST([EGREP])
])])


# AC_PATH_TOOL_PREFIX
# -------------------
# find a file program which can recognise shared library
AC_DEFUN([AC_PATH_TOOL_PREFIX],
[AC_REQUIRE([AC_PROG_EGREP])dnl
AC_MSG_CHECKING([for $1])
AC_CACHE_VAL(lt_cv_path_MAGIC_CMD,
[case $MAGIC_CMD in
[[\\/*] |  ?:[\\/]*])
  lt_cv_path_MAGIC_CMD="$MAGIC_CMD" # Let the user override the test with a path.
  ;;
*)
  lt_save_MAGIC_CMD="$MAGIC_CMD"
  lt_save_ifs="$IFS"; IFS=$PATH_SEPARATOR
dnl $ac_dummy forces splitting on constant user-supplied paths.
dnl POSIX.2 word splitting is done only on the output of word expansions,
dnl not every word.  This closes a longstanding sh security hole.
  ac_dummy="ifelse([$2], , $PATH, [$2])"
  for ac_dir in $ac_dummy; do
    IFS="$lt_save_ifs"
    test -z "$ac_dir" && ac_dir=.
    if test -f $ac_dir/$1; then
      lt_cv_path_MAGIC_CMD="$ac_dir/$1"
      if test -n "$file_magic_test_file"; then
	case $deplibs_check_method in
	"file_magic "*)
	  file_magic_regex=`expr "$deplibs_check_method" : "file_magic \(.*\)"`
	  MAGIC_CMD="$lt_cv_path_MAGIC_CMD"
	  if eval $file_magic_cmd \$file_magic_test_file 2> /dev/null |
	    $EGREP "$file_magic_regex" > /dev/null; then
	    :
	  else
	    cat <<EOF 1>&2

*** Warning: the command libtool uses to detect shared libraries,
*** $file_magic_cmd, produces output that libtool cannot recognize.
*** The result is that libtool may fail to recognize shared libraries
*** as such.  This will affect the creation of libtool libraries that
*** depend on shared libraries, but programs linked with such libtool
*** libraries will work regardless of this problem.  Nevertheless, you
*** may want to report the problem to your system manager and/or to
*** bug-libtool@gnu.org

EOF
	  fi ;;
	esac
      fi
      break
    fi
  done
  IFS="$lt_save_ifs"
  MAGIC_CMD="$lt_save_MAGIC_CMD"
  ;;
esac])
MAGIC_CMD="$lt_cv_path_MAGIC_CMD"
if test -n "$MAGIC_CMD"; then
  AC_MSG_RESULT($MAGIC_CMD)
else
  AC_MSG_RESULT(no)
fi
])# AC_PATH_TOOL_PREFIX


# AC_PATH_MAGIC
# -------------
# find a file program which can recognise a shared library
AC_DEFUN([AC_PATH_MAGIC],
[AC_PATH_TOOL_PREFIX(${ac_tool_prefix}file, /usr/bin$PATH_SEPARATOR$PATH)
if test -z "$lt_cv_path_MAGIC_CMD"; then
  if test -n "$ac_tool_prefix"; then
    AC_PATH_TOOL_PREFIX(file, /usr/bin$PATH_SEPARATOR$PATH)
  else
    MAGIC_CMD=:
  fi
fi
])# AC_PATH_MAGIC


# AC_PROG_LD
# ----------
# find the pathname to the GNU or non-GNU linker
AC_DEFUN([AC_PROG_LD],
[AC_ARG_WITH([gnu-ld],
    [AC_HELP_STRING([--with-gnu-ld],
	[assume the C compiler uses GNU ld @<:@default=no@:>@])],
    [test "$withval" = no || with_gnu_ld=yes],
    [with_gnu_ld=no])
AC_REQUIRE([LT_AC_PROG_SED])dnl
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_CANONICAL_BUILD])dnl
ac_prog=ld
if test "$GCC" = yes; then
  # Check if gcc -print-prog-name=ld gives a path.
  AC_MSG_CHECKING([for ld used by $CC])
  case $host in
  *-*-mingw*)
    # gcc leaves a trailing carriage return which upsets mingw
    ac_prog=`($CC -print-prog-name=ld) 2>&5 | tr -d '\015'` ;;
  *)
    ac_prog=`($CC -print-prog-name=ld) 2>&5` ;;
  esac
  case $ac_prog in
    # Accept absolute paths.
    [[\\/]]* | ?:[[\\/]]*)
      re_direlt='/[[^/]][[^/]]*/\.\./'
      # Canonicalize the pathname of ld
      ac_prog=`echo $ac_prog| $SED 's%\\\\%/%g'`
      while echo $ac_prog | grep "$re_direlt" > /dev/null 2>&1; do
	ac_prog=`echo $ac_prog| $SED "s%$re_direlt%/%"`
      done
      test -z "$LD" && LD="$ac_prog"
      ;;
  "")
    # If it fails, then pretend we aren't using GCC.
    ac_prog=ld
    ;;
  *)
    # If it is relative, then search for the first ld in PATH.
    with_gnu_ld=unknown
    ;;
  esac
elif test "$with_gnu_ld" = yes; then
  AC_MSG_CHECKING([for GNU ld])
else
  AC_MSG_CHECKING([for non-GNU ld])
fi
AC_CACHE_VAL(lt_cv_path_LD,
[if test -z "$LD"; then
  lt_save_ifs="$IFS"; IFS=$PATH_SEPARATOR
  for ac_dir in $PATH; do
    IFS="$lt_save_ifs"
    test -z "$ac_dir" && ac_dir=.
    if test -f "$ac_dir/$ac_prog" || test -f "$ac_dir/$ac_prog$ac_exeext"; then
      lt_cv_path_LD="$ac_dir/$ac_prog"
      # Check to see if the program is GNU ld.  I'd rather use --version,
      # but apparently some variants of GNU ld only accept -v.
      # Break only if it was the GNU/non-GNU ld that we prefer.
      case `"$lt_cv_path_LD" -v 2>&1 </dev/null` in
      *GNU* | *'with BFD'*)
	test "$with_gnu_ld" != no && break
	;;
      *)
	test "$with_gnu_ld" != yes && break
	;;
      esac
    fi
  done
  IFS="$lt_save_ifs"
else
  lt_cv_path_LD="$LD" # Let the user override the test with a path.
fi])
LD="$lt_cv_path_LD"
if test -n "$LD"; then
  AC_MSG_RESULT($LD)
else
  AC_MSG_RESULT(no)
fi
test -z "$LD" && AC_MSG_ERROR([no acceptable ld found in \$PATH])
AC_PROG_LD_GNU
])# AC_PROG_LD


# AC_PROG_LD_GNU
# --------------
AC_DEFUN([AC_PROG_LD_GNU],
[AC_REQUIRE([AC_PROG_EGREP])dnl
AC_CACHE_CHECK([if the linker ($LD) is GNU ld], lt_cv_prog_gnu_ld,
[# I'd rather use --version here, but apparently some GNU lds only accept -v.
case `$LD -v 2>&1 </dev/null` in
*GNU* | *'with BFD'*)
  lt_cv_prog_gnu_ld=yes
  ;;
*)
  lt_cv_prog_gnu_ld=no
  ;;
esac])
with_gnu_ld=$lt_cv_prog_gnu_ld
])# AC_PROG_LD_GNU


# AC_PROG_LD_RELOAD_FLAG
# ----------------------
# find reload flag for linker
#   -- PORTME Some linkers may need a different reload flag.
AC_DEFUN([AC_PROG_LD_RELOAD_FLAG],
[AC_CACHE_CHECK([for $LD option to reload object files],
  lt_cv_ld_reload_flag,
  [lt_cv_ld_reload_flag='-r'])
reload_flag=$lt_cv_ld_reload_flag
case $reload_flag in
"" | " "*) ;;
*) reload_flag=" $reload_flag" ;;
esac
reload_cmds='$LD$reload_flag -o $output$reload_objs'
case $host_os in
  darwin*)
    if test "$GCC" = yes; then
      reload_cmds='$LTCC $LTCFLAGS -nostdlib ${wl}-r -o $output$reload_objs'
    else
      reload_cmds='$LD$reload_flag -o $output$reload_objs'
    fi
    ;;
esac
])# AC_PROG_LD_RELOAD_FLAG


# AC_DEPLIBS_CHECK_METHOD
# -----------------------
# how to check for library dependencies
#  -- PORTME fill in with the dynamic library characteristics
AC_DEFUN([AC_DEPLIBS_CHECK_METHOD],
[AC_CACHE_CHECK([how to recognise dependent libraries],
lt_cv_deplibs_check_method,
[lt_cv_file_magic_cmd='$MAGIC_CMD'
lt_cv_file_magic_test_file=
lt_cv_deplibs_check_method='unknown'
# Need to set the preceding variable on all platforms that support
# interlibrary dependencies.
# 'none' -- dependencies not supported.
# `unknown' -- same as none, but documents that we really don't know.
# 'pass_all' -- all dependencies passed with no checks.
# 'test_compile' -- check by making test program.
# 'file_magic [[regex]]' -- check by looking for files in library path
# which responds to the $file_magic_cmd with a given extended regex.
# If you have `file' or equivalent on your system and you're not sure
# whether `pass_all' will *always* work, you probably want this one.

case $host_os in
aix4* | aix5*)
  lt_cv_deplibs_check_method=pass_all
  ;;

beos*)
  lt_cv_deplibs_check_method=pass_all
  ;;

bsdi[[45]]*)
  lt_cv_deplibs_check_method='file_magic ELF [[0-9]][[0-9]]*-bit [[ML]]SB (shared object|dynamic lib)'
  lt_cv_file_magic_cmd='/usr/bin/file -L'
  lt_cv_file_magic_test_file=/shlib/libc.so
  ;;

cygwin*)
  # func_win32_libid is a shell function defined in ltmain.sh
  lt_cv_deplibs_check_method='file_magic ^x86 archive import|^x86 DLL'
  lt_cv_file_magic_cmd='func_win32_libid'
  ;;

mingw* | pw32*)
  # Base MSYS/MinGW do not provide the 'file' command needed by
  # func_win32_libid shell function, so use a weaker test based on 'objdump'.
  lt_cv_deplibs_check_method='file_magic file format pei*-i386(.*architecture: i386)?'
  lt_cv_file_magic_cmd='$OBJDUMP -f'
  ;;

darwin* | rhapsody*)
  lt_cv_deplibs_check_method=pass_all
  ;;

freebsd* | kfreebsd*-gnu | dragonfly*)
  if echo __ELF__ | $CC -E - | grep __ELF__ > /dev/null; then
    case $host_cpu in
    i*86 )
      # Not sure whether the presence of OpenBSD here was a mistake.
      # Let's accept both of them until this is cleared up.
      lt_cv_deplibs_check_method='file_magic (FreeBSD|OpenBSD|DragonFly)/i[[3-9]]86 (compact )?demand paged shared library'
      lt_cv_file_magic_cmd=/usr/bin/file
      lt_cv_file_magic_test_file=`echo /usr/lib/libc.so.*`
      ;;
    esac
  else
    lt_cv_deplibs_check_method=pass_all
  fi
  ;;

gnu*)
  lt_cv_deplibs_check_method=pass_all
  ;;

hpux10.20* | hpux11*)
  lt_cv_file_magic_cmd=/usr/bin/file
  case $host_cpu in
  ia64*)
    lt_cv_deplibs_check_method='file_magic (s[[0-9]][[0-9]][[0-9]]|ELF-[[0-9]][[0-9]]) shared object file - IA64'
    lt_cv_file_magic_test_file=/usr/lib/hpux32/libc.so
    ;;
  hppa*64*)
    [lt_cv_deplibs_check_method='file_magic (s[0-9][0-9][0-9]|ELF-[0-9][0-9]) shared object file - PA-RISC [0-9].[0-9]']
    lt_cv_file_magic_test_file=/usr/lib/pa20_64/libc.sl
    ;;
  *)
    lt_cv_deplibs_check_method='file_magic (s[[0-9]][[0-9]][[0-9]]|PA-RISC[[0-9]].[[0-9]]) shared library'
    lt_cv_file_magic_test_file=/usr/lib/libc.sl
    ;;
  esac
  ;;

interix3*)
  # PIC code is broken on Interix 3.x, that's why |\.a not |_pic\.a here
  lt_cv_deplibs_check_method='match_pattern /lib[[^/]]+(\.so|\.a)$'
  ;;

irix5* | irix6* | nonstopux*)
  case $LD in
  *-32|*"-32 ") libmagic=32-bit;;
  *-n32|*"-n32 ") libmagic=N32;;
  *-64|*"-64 ") libmagic=64-bit;;
  *) libmagic=never-match;;
  esac
  lt_cv_deplibs_check_method=pass_all
  ;;

# This must be Linux ELF.
linux*)
  lt_cv_deplibs_check_method=pass_all
  ;;

netbsd*)
  if echo __ELF__ | $CC -E - | grep __ELF__ > /dev/null; then
    lt_cv_deplibs_check_method='match_pattern /lib[[^/]]+(\.so\.[[0-9]]+\.[[0-9]]+|_pic\.a)$'
  else
    lt_cv_deplibs_check_method='match_pattern /lib[[^/]]+(\.so|_pic\.a)$'
  fi
  ;;

newos6*)
  lt_cv_deplibs_check_method='file_magic ELF [[0-9]][[0-9]]*-bit [[ML]]SB (executable|dynamic lib)'
  lt_cv_file_magic_cmd=/usr/bin/file
  lt_cv_file_magic_test_file=/usr/lib/libnls.so
  ;;

nto-qnx*)
  lt_cv_deplibs_check_method=unknown
  ;;

openbsd*)
  if test -z "`echo __ELF__ | $CC -E - | grep __ELF__`" || test "$host_os-$host_cpu" = "openbsd2.8-powerpc"; then
    lt_cv_deplibs_check_method='match_pattern /lib[[^/]]+(\.so\.[[0-9]]+\.[[0-9]]+|\.so|_pic\.a)$'
  else
    lt_cv_deplibs_check_method='match_pattern /lib[[^/]]+(\.so\.[[0-9]]+\.[[0-9]]+|_pic\.a)$'
  fi
  ;;

osf3* | osf4* | osf5*)
  lt_cv_deplibs_check_method=pass_all
  ;;

solaris*)
  lt_cv_deplibs_check_method=pass_all
  ;;

sysv4 | sysv4.3*)
  case $host_vendor in
  motorola)
    lt_cv_deplibs_check_method='file_magic ELF [[0-9]][[0-9]]*-bit [[ML]]SB (shared object|dynamic lib) M[[0-9]][[0-9]]* Version [[0-9]]'
    lt_cv_file_magic_test_file=`echo /usr/lib/libc.so*`
    ;;
  ncr)
    lt_cv_deplibs_check_method=pass_all
    ;;
  sequent)
    lt_cv_file_magic_cmd='/bin/file'
    lt_cv_deplibs_check_method='file_magic ELF [[0-9]][[0-9]]*-bit [[LM]]SB (shared object|dynamic lib )'
    ;;
  sni)
    lt_cv_file_magic_cmd='/bin/file'
    lt_cv_deplibs_check_method="file_magic ELF [[0-9]][[0-9]]*-bit [[LM]]SB dynamic lib"
    lt_cv_file_magic_test_file=/lib/libc.so
    ;;
  siemens)
    lt_cv_deplibs_check_method=pass_all
    ;;
  pc)
    lt_cv_deplibs_check_method=pass_all
    ;;
  esac
  ;;

sysv5* | sco3.2v5* | sco5v6* | unixware* | OpenUNIX* | sysv4*uw2*)
  lt_cv_deplibs_check_method=pass_all
  ;;
esac
])
file_magic_cmd=$lt_cv_file_magic_cmd
deplibs_check_method=$lt_cv_deplibs_check_method
test -z "$deplibs_check_method" && deplibs_check_method=unknown
])# AC_DEPLIBS_CHECK_METHOD


# AC_PROG_NM
# ----------
# find the pathname to a BSD-compatible name lister
AC_DEFUN([AC_PROG_NM],
[AC_CACHE_CHECK([for BSD-compatible nm], lt_cv_path_NM,
[if test -n "$NM"; then
  # Let the user override the test.
  lt_cv_path_NM="$NM"
else
  lt_nm_to_check="${ac_tool_prefix}nm"
  if test -n "$ac_tool_prefix" && test "$build" = "$host"; then 
    lt_nm_to_check="$lt_nm_to_check nm"
  fi
  for lt_tmp_nm in $lt_nm_to_check; do
    lt_save_ifs="$IFS"; IFS=$PATH_SEPARATOR
    for ac_dir in $PATH /usr/ccs/bin/elf /usr/ccs/bin /usr/ucb /bin; do
      IFS="$lt_save_ifs"
      test -z "$ac_dir" && ac_dir=.
      tmp_nm="$ac_dir/$lt_tmp_nm"
      if test -f "$tmp_nm" || test -f "$tmp_nm$ac_exeext" ; then
	# Check to see if the nm accepts a BSD-compat flag.
	# Adding the `sed 1q' prevents false positives on HP-UX, which says:
	#   nm: unknown option "B" ignored
	# Tru64's nm complains that /dev/null is an invalid object file
	case `"$tmp_nm" -B /dev/null 2>&1 | sed '1q'` in
	*/dev/null* | *'Invalid file or object type'*)
	  lt_cv_path_NM="$tmp_nm -B"
	  break
	  ;;
	*)
	  case `"$tmp_nm" -p /dev/null 2>&1 | sed '1q'` in
	  */dev/null*)
	    lt_cv_path_NM="$tmp_nm -p"
	    break
	    ;;
	  *)
	    lt_cv_path_NM=${lt_cv_path_NM="$tmp_nm"} # keep the first match, but
	    continue # so that we can try to find one that supports BSD flags
	    ;;
	  esac
	  ;;
	esac
      fi
    done
    IFS="$lt_save_ifs"
  done
  test -z "$lt_cv_path_NM" && lt_cv_path_NM=nm
fi])
NM="$lt_cv_path_NM"
])# AC_PROG_NM


# AC_CHECK_LIBM
# -------------
# check for math library
AC_DEFUN([AC_CHECK_LIBM],
[AC_REQUIRE([AC_CANONICAL_HOST])dnl
LIBM=
case $host in
*-*-beos* | *-*-cygwin* | *-*-pw32* | *-*-darwin*)
  # These system don't have libm, or don't need it
  ;;
*-ncr-sysv4.3*)
  AC_CHECK_LIB(mw, _mwvalidcheckl, LIBM="-lmw")
  AC_CHECK_LIB(m, cos, LIBM="$LIBM -lm")
  ;;
*)
  AC_CHECK_LIB(m, cos, LIBM="-lm")
  ;;
esac
])# AC_CHECK_LIBM


# AC_LIBLTDL_CONVENIENCE([DIRECTORY])
# -----------------------------------
# sets LIBLTDL to the link flags for the libltdl convenience library and
# LTDLINCL to the include flags for the libltdl header and adds
# --enable-ltdl-convenience to the configure arguments.  Note that
# AC_CONFIG_SUBDIRS is not called here.  If DIRECTORY is not provided,
# it is assumed to be `libltdl'.  LIBLTDL will be prefixed with
# '${top_builddir}/' and LTDLINCL will be prefixed with '${top_srcdir}/'
# (note the single quotes!).  If your package is not flat and you're not
# using automake, define top_builddir and top_srcdir appropriately in
# the Makefiles.
AC_DEFUN([AC_LIBLTDL_CONVENIENCE],
[AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
  case $enable_ltdl_convenience in
  no) AC_MSG_ERROR([this package needs a convenience libltdl]) ;;
  "") enable_ltdl_convenience=yes
      ac_configure_args="$ac_configure_args --enable-ltdl-convenience" ;;
  esac
  LIBLTDL='${top_builddir}/'ifelse($#,1,[$1],['libltdl'])/libltdlc.la
  LTDLINCL='-I${top_srcdir}/'ifelse($#,1,[$1],['libltdl'])
  # For backwards non-gettext consistent compatibility...
  INCLTDL="$LTDLINCL"
])# AC_LIBLTDL_CONVENIENCE


# AC_LIBLTDL_INSTALLABLE([DIRECTORY])
# -----------------------------------
# sets LIBLTDL to the link flags for the libltdl installable library and
# LTDLINCL to the include flags for the libltdl header and adds
# --enable-ltdl-install to the configure arguments.  Note that
# AC_CONFIG_SUBDIRS is not called here.  If DIRECTORY is not provided,
# and an installed libltdl is not found, it is assumed to be `libltdl'.
# LIBLTDL will be prefixed with '${top_builddir}/'# and LTDLINCL with
# '${top_srcdir}/' (note the single quotes!).  If your package is not
# flat and you're not using automake, define top_builddir and top_srcdir
# appropriately in the Makefiles.
# In the future, this macro may have to be called after AC_PROG_LIBTOOL.
AC_DEFUN([AC_LIBLTDL_INSTALLABLE],
[AC_BEFORE([$0],[AC_LIBTOOL_SETUP])dnl
  AC_CHECK_LIB(ltdl, lt_dlinit,
  [test x"$enable_ltdl_install" != xyes && enable_ltdl_install=no],
  [if test x"$enable_ltdl_install" = xno; then
     AC_MSG_WARN([libltdl not installed, but installation disabled])
   else
     enable_ltdl_install=yes
   fi
  ])
  if test x"$enable_ltdl_install" = x"yes"; then
    ac_configure_args="$ac_configure_args --enable-ltdl-install"
    LIBLTDL='${top_builddir}/'ifelse($#,1,[$1],['libltdl'])/libltdl.la
    LTDLINCL='-I${top_srcdir}/'ifelse($#,1,[$1],['libltdl'])
  else
    ac_configure_args="$ac_configure_args --enable-ltdl-install=no"
    LIBLTDL="-lltdl"
    LTDLINCL=
  fi
  # For backwards non-gettext consistent compatibility...
  INCLTDL="$LTDLINCL"
])# AC_LIBLTDL_INSTALLABLE


# AC_LIBTOOL_CXX
# --------------
# enable support for C++ libraries
AC_DEFUN([AC_LIBTOOL_CXX],
[AC_REQUIRE([_LT_AC_LANG_CXX])
])# AC_LIBTOOL_CXX


# _LT_AC_LANG_CXX
# ---------------
AC_DEFUN([_LT_AC_LANG_CXX],
[AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([_LT_AC_PROG_CXXCPP])
_LT_AC_SHELL_INIT([tagnames=${tagnames+${tagnames},}CXX])
])# _LT_AC_LANG_CXX

# _LT_AC_PROG_CXXCPP
# ------------------
AC_DEFUN([_LT_AC_PROG_CXXCPP],
[
AC_REQUIRE([AC_PROG_CXX])
if test -n "$CXX" && ( test "X$CXX" != "Xno" &&
    ( (test "X$CXX" = "Xg++" && `g++ -v >/dev/null 2>&1` ) ||
    (test "X$CXX" != "Xg++"))) ; then
  AC_PROG_CXXCPP
fi
])# _LT_AC_PROG_CXXCPP

# AC_LIBTOOL_F77
# --------------
# enable support for Fortran 77 libraries
AC_DEFUN([AC_LIBTOOL_F77],
[AC_REQUIRE([_LT_AC_LANG_F77])
])# AC_LIBTOOL_F77


# _LT_AC_LANG_F77
# ---------------
AC_DEFUN([_LT_AC_LANG_F77],
[AC_REQUIRE([AC_PROG_F77])
_LT_AC_SHELL_INIT([tagnames=${tagnames+${tagnames},}F77])
])# _LT_AC_LANG_F77


# AC_LIBTOOL_GCJ
# --------------
# enable support for GCJ libraries
AC_DEFUN([AC_LIBTOOL_GCJ],
[AC_REQUIRE([_LT_AC_LANG_GCJ])
])# AC_LIBTOOL_GCJ


# _LT_AC_LANG_GCJ
# ---------------
AC_DEFUN([_LT_AC_LANG_GCJ],
[AC_PROVIDE_IFELSE([AC_PROG_GCJ],[],
  [AC_PROVIDE_IFELSE([A][M_PROG_GCJ],[],
    [AC_PROVIDE_IFELSE([LT_AC_PROG_GCJ],[],
      [ifdef([AC_PROG_GCJ],[AC_REQUIRE([AC_PROG_GCJ])],
	 [ifdef([A][M_PROG_GCJ],[AC_REQUIRE([A][M_PROG_GCJ])],
	   [AC_REQUIRE([A][C_PROG_GCJ_OR_A][M_PROG_GCJ])])])])])])
_LT_AC_SHELL_INIT([tagnames=${tagnames+${tagnames},}GCJ])
])# _LT_AC_LANG_GCJ


# AC_LIBTOOL_RC
# -------------
# enable support for Windows resource files
AC_DEFUN([AC_LIBTOOL_RC],
[AC_REQUIRE([LT_AC_PROG_RC])
_LT_AC_SHELL_INIT([tagnames=${tagnames+${tagnames},}RC])
])# AC_LIBTOOL_RC


# AC_LIBTOOL_LANG_C_CONFIG
# ------------------------
# Ensure that the configuration vars for the C compiler are
# suitably defined.  Those variables are subsequently used by
# AC_LIBTOOL_CONFIG to write the compiler configuration to `libtool'.
AC_DEFUN([AC_LIBTOOL_LANG_C_CONFIG], [_LT_AC_LANG_C_CONFIG])
AC_DEFUN([_LT_AC_LANG_C_CONFIG],
[lt_save_CC="$CC"
AC_LANG_PUSH(C)

# Source file extension for C test sources.
ac_ext=c

# Object file extension for compiled C test sources.
objext=o
_LT_AC_TAGVAR(objext, $1)=$objext

# Code to be used in simple compile tests
lt_simple_compile_test_code="int some_variable = 0;\n"

# Code to be used in simple link tests
lt_simple_link_test_code='int main(){return(0);}\n'

_LT_AC_SYS_COMPILER

# save warnings/boilerplate of simple test code
_LT_COMPILER_BOILERPLATE
_LT_LINKER_BOILERPLATE

AC_LIBTOOL_PROG_COMPILER_NO_RTTI($1)
AC_LIBTOOL_PROG_COMPILER_PIC($1)
AC_LIBTOOL_PROG_CC_C_O($1)
AC_LIBTOOL_SYS_HARD_LINK_LOCKS($1)
AC_LIBTOOL_PROG_LD_SHLIBS($1)
AC_LIBTOOL_SYS_DYNAMIC_LINKER($1)
AC_LIBTOOL_PROG_LD_HARDCODE_LIBPATH($1)
AC_LIBTOOL_SYS_LIB_STRIP
AC_LIBTOOL_DLOPEN_SELF

# Report which library types will actually be built
AC_MSG_CHECKING([if libtool supports shared libraries])
AC_MSG_RESULT([$can_build_shared])

AC_MSG_CHECKING([whether to build shared libraries])
test "$can_build_shared" = "no" && enable_shared=no

# On AIX, shared libraries and static libraries use the same namespace, and
# are all built from PIC.
case $host_os in
aix3*)
  test "$enable_shared" = yes && enable_static=no
  if test -n "$RANLIB"; then
    archive_cmds="$archive_cmds~\$RANLIB \$lib"
    postinstall_cmds='$RANLIB $lib'
  fi
  ;;

aix4* | aix5*)
  if test "$host_cpu" != ia64 && test "$aix_use_runtimelinking" = no ; then
    test "$enable_shared" = yes && enable_static=no
  fi
    ;;
esac
AC_MSG_RESULT([$enable_shared])

AC_MSG_CHECKING([whether to build static libraries])
# Make sure either enable_shared or enable_static is yes.
test "$enable_shared" = yes || enable_static=yes
AC_MSG_RESULT([$enable_static])

AC_LIBTOOL_CONFIG($1)

AC_LANG_POP
CC="$lt_save_CC"
])# AC_LIBTOOL_LANG_C_CONFIG


# AC_LIBTOOL_LANG_CXX_CONFIG
# --------------------------
# Ensure that the configuration vars for the C compiler are
# suitably defined.  Those variables are subsequently used by
# AC_LIBTOOL_CONFIG to write the compiler configuration to `libtool'.
AC_DEFUN([AC_LIBTOOL_LANG_CXX_CONFIG], [_LT_AC_LANG_CXX_CONFIG(CXX)])
AC_DEFUN([_LT_AC_LANG_CXX_CONFIG],
[AC_LANG_PUSH(C++)
AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([_LT_AC_PROG_CXXCPP])

_LT_AC_TAGVAR(archive_cmds_need_lc, $1)=no
_LT_AC_TAGVAR(allow_undefined_flag, $1)=
_LT_AC_TAGVAR(always_export_symbols, $1)=no
_LT_AC_TAGVAR(archive_expsym_cmds, $1)=
_LT_AC_TAGVAR(export_dynamic_flag_spec, $1)=
_LT_AC_TAGVAR(hardcode_direct, $1)=no
_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)=
_LT_AC_TAGVAR(hardcode_libdir_flag_spec_ld, $1)=
_LT_AC_TAGVAR(hardcode_libdir_separator, $1)=
_LT_AC_TAGVAR(hardcode_minus_L, $1)=no
_LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=unsupported
_LT_AC_TAGVAR(hardcode_automatic, $1)=no
_LT_AC_TAGVAR(module_cmds, $1)=
_LT_AC_TAGVAR(module_expsym_cmds, $1)=
_LT_AC_TAGVAR(link_all_deplibs, $1)=unknown
_LT_AC_TAGVAR(old_archive_cmds, $1)=$old_archive_cmds
_LT_AC_TAGVAR(no_undefined_flag, $1)=
_LT_AC_TAGVAR(whole_archive_flag_spec, $1)=
_LT_AC_TAGVAR(enable_shared_with_static_runtimes, $1)=no

# Dependencies to place before and after the object being linked:
_LT_AC_TAGVAR(predep_objects, $1)=
_LT_AC_TAGVAR(postdep_objects, $1)=
_LT_AC_TAGVAR(predeps, $1)=
_LT_AC_TAGVAR(postdeps, $1)=
_LT_AC_TAGVAR(compiler_lib_search_path, $1)=

# Source file extension for C++ test sources.
ac_ext=cpp

# Object file extension for compiled C++ test sources.
objext=o
_LT_AC_TAGVAR(objext, $1)=$objext

# Code to be used in simple compile tests
lt_simple_compile_test_code="int some_variable = 0;\n"

# Code to be used in simple link tests
lt_simple_link_test_code='int main(int, char *[[]]) { return(0); }\n'

# ltmain only uses $CC for tagged configurations so make sure $CC is set.
_LT_AC_SYS_COMPILER

# save warnings/boilerplate of simple test code
_LT_COMPILER_BOILERPLATE
_LT_LINKER_BOILERPLATE

# Allow CC to be a program name with arguments.
lt_save_CC=$CC
lt_save_LD=$LD
lt_save_GCC=$GCC
GCC=$GXX
lt_save_with_gnu_ld=$with_gnu_ld
lt_save_path_LD=$lt_cv_path_LD
if test -n "${lt_cv_prog_gnu_ldcxx+set}"; then
  lt_cv_prog_gnu_ld=$lt_cv_prog_gnu_ldcxx
else
  $as_unset lt_cv_prog_gnu_ld
fi
if test -n "${lt_cv_path_LDCXX+set}"; then
  lt_cv_path_LD=$lt_cv_path_LDCXX
else
  $as_unset lt_cv_path_LD
fi
test -z "${LDCXX+set}" || LD=$LDCXX
CC=${CXX-"c++"}
compiler=$CC
_LT_AC_TAGVAR(compiler, $1)=$CC
_LT_CC_BASENAME([$compiler])

# We don't want -fno-exception wen compiling C++ code, so set the
# no_builtin_flag separately
if test "$GXX" = yes; then
  _LT_AC_TAGVAR(lt_prog_compiler_no_builtin_flag, $1)=' -fno-builtin'
else
  _LT_AC_TAGVAR(lt_prog_compiler_no_builtin_flag, $1)=
fi

if test "$GXX" = yes; then
  # Set up default GNU C++ configuration

  AC_PROG_LD

  # Check if GNU C++ uses GNU ld as the underlying linker, since the
  # archiving commands below assume that GNU ld is being used.
  if test "$with_gnu_ld" = yes; then
    _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -nostdlib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-soname $wl$soname -o $lib'
    _LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared -nostdlib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-soname $wl$soname ${wl}-retain-symbols-file $wl$export_symbols -o $lib'

    _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}--rpath ${wl}$libdir'
    _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}--export-dynamic'

    # If archive_cmds runs LD, not CC, wlarc should be empty
    # XXX I think wlarc can be eliminated in ltcf-cxx, but I need to
    #     investigate it a little bit more. (MM)
    wlarc='${wl}'

    # ancient GNU ld didn't support --whole-archive et. al.
    if eval "`$CC -print-prog-name=ld` --help 2>&1" | \
	grep 'no-whole-archive' > /dev/null; then
      _LT_AC_TAGVAR(whole_archive_flag_spec, $1)="$wlarc"'--whole-archive$convenience '"$wlarc"'--no-whole-archive'
    else
      _LT_AC_TAGVAR(whole_archive_flag_spec, $1)=
    fi
  else
    with_gnu_ld=no
    wlarc=

    # A generic and very simple default shared library creation
    # command for GNU C++ for the case where it uses the native
    # linker, instead of GNU ld.  If possible, this setting should
    # overridden to take advantage of the native linker features on
    # the platform it is being used on.
    _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -nostdlib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags -o $lib'
  fi

  # Commands to make compiler produce verbose output that lists
  # what "hidden" libraries, object files and flags are used when
  # linking a shared library.
  output_verbose_link_cmd='$CC -shared $CFLAGS -v conftest.$objext 2>&1 | grep "\-L"'

else
  GXX=no
  with_gnu_ld=no
  wlarc=
fi

# PORTME: fill in a description of your system's C++ link characteristics
AC_MSG_CHECKING([whether the $compiler linker ($LD) supports shared libraries])
_LT_AC_TAGVAR(ld_shlibs, $1)=yes
case $host_os in
  aix3*)
    # FIXME: insert proper C++ library support
    _LT_AC_TAGVAR(ld_shlibs, $1)=no
    ;;
  aix4* | aix5*)
    if test "$host_cpu" = ia64; then
      # On IA64, the linker does run time linking by default, so we don't
      # have to do anything special.
      aix_use_runtimelinking=no
      exp_sym_flag='-Bexport'
      no_entry_flag=""
    else
      aix_use_runtimelinking=no

      # Test if we are trying to use run time linking or normal
      # AIX style linking. If -brtl is somewhere in LDFLAGS, we
      # need to do runtime linking.
      case $host_os in aix4.[[23]]|aix4.[[23]].*|aix5*)
	for ld_flag in $LDFLAGS; do
	  case $ld_flag in
	  *-brtl*)
	    aix_use_runtimelinking=yes
	    break
	    ;;
	  esac
	done
	;;
      esac

      exp_sym_flag='-bexport'
      no_entry_flag='-bnoentry'
    fi

    # When large executables or shared objects are built, AIX ld can
    # have problems creating the table of contents.  If linking a library
    # or program results in "error TOC overflow" add -mminimal-toc to
    # CXXFLAGS/CFLAGS for g++/gcc.  In the cases where that is not
    # enough to fix the problem, add -Wl,-bbigtoc to LDFLAGS.

    _LT_AC_TAGVAR(archive_cmds, $1)=''
    _LT_AC_TAGVAR(hardcode_direct, $1)=yes
    _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=':'
    _LT_AC_TAGVAR(link_all_deplibs, $1)=yes

    if test "$GXX" = yes; then
      case $host_os in aix4.[[012]]|aix4.[[012]].*)
      # We only want to do this on AIX 4.2 and lower, the check
      # below for broken collect2 doesn't work under 4.3+
	collect2name=`${CC} -print-prog-name=collect2`
	if test -f "$collect2name" && \
	   strings "$collect2name" | grep resolve_lib_name >/dev/null
	then
	  # We have reworked collect2
	  _LT_AC_TAGVAR(hardcode_direct, $1)=yes
	else
	  # We have old collect2
	  _LT_AC_TAGVAR(hardcode_direct, $1)=unsupported
	  # It fails to find uninstalled libraries when the uninstalled
	  # path is not listed in the libpath.  Setting hardcode_minus_L
	  # to unsupported forces relinking
	  _LT_AC_TAGVAR(hardcode_minus_L, $1)=yes
	  _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-L$libdir'
	  _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=
	fi
	;;
      esac
      shared_flag='-shared'
      if test "$aix_use_runtimelinking" = yes; then
	shared_flag="$shared_flag "'${wl}-G'
      fi
    else
      # not using gcc
      if test "$host_cpu" = ia64; then
	# VisualAge C++, Version 5.5 for AIX 5L for IA-64, Beta 3 Release
	# chokes on -Wl,-G. The following line is correct:
	shared_flag='-G'
      else
	if test "$aix_use_runtimelinking" = yes; then
	  shared_flag='${wl}-G'
	else
	  shared_flag='${wl}-bM:SRE'
	fi
      fi
    fi

    # It seems that -bexpall does not export symbols beginning with
    # underscore (_), so it is better to generate a list of symbols to export.
    _LT_AC_TAGVAR(always_export_symbols, $1)=yes
    if test "$aix_use_runtimelinking" = yes; then
      # Warning - without using the other runtime loading flags (-brtl),
      # -berok will link without error, but may produce a broken library.
      _LT_AC_TAGVAR(allow_undefined_flag, $1)='-berok'
      # Determine the default libpath from the value encoded in an empty executable.
      _LT_AC_SYS_LIBPATH_AIX
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-blibpath:$libdir:'"$aix_libpath"

      _LT_AC_TAGVAR(archive_expsym_cmds, $1)="\$CC"' -o $output_objdir/$soname $libobjs $deplibs '"\${wl}$no_entry_flag"' $compiler_flags `if test "x${allow_undefined_flag}" != "x"; then echo "${wl}${allow_undefined_flag}"; else :; fi` '"\${wl}$exp_sym_flag:\$export_symbols $shared_flag"
     else
      if test "$host_cpu" = ia64; then
	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-R $libdir:/usr/lib:/lib'
	_LT_AC_TAGVAR(allow_undefined_flag, $1)="-z nodefs"
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)="\$CC $shared_flag"' -o $output_objdir/$soname $libobjs $deplibs '"\${wl}$no_entry_flag"' $compiler_flags ${wl}${allow_undefined_flag} '"\${wl}$exp_sym_flag:\$export_symbols"
      else
	# Determine the default libpath from the value encoded in an empty executable.
	_LT_AC_SYS_LIBPATH_AIX
	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-blibpath:$libdir:'"$aix_libpath"
	# Warning - without using the other run time loading flags,
	# -berok will link without error, but may produce a broken library.
	_LT_AC_TAGVAR(no_undefined_flag, $1)=' ${wl}-bernotok'
	_LT_AC_TAGVAR(allow_undefined_flag, $1)=' ${wl}-berok'
	# Exported symbols can be pulled into shared objects from archives
	_LT_AC_TAGVAR(whole_archive_flag_spec, $1)='$convenience'
	_LT_AC_TAGVAR(archive_cmds_need_lc, $1)=yes
	# This is similar to how AIX traditionally builds its shared libraries.
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)="\$CC $shared_flag"' -o $output_objdir/$soname $libobjs $deplibs ${wl}-bnoentry $compiler_flags ${wl}-bE:$export_symbols${allow_undefined_flag}~$AR $AR_FLAGS $output_objdir/$libname$release.a $output_objdir/$soname'
      fi
    fi
    ;;

  beos*)
    if $LD --help 2>&1 | grep ': supported targets:.* elf' > /dev/null; then
      _LT_AC_TAGVAR(allow_undefined_flag, $1)=unsupported
      # Joseph Beckenbach <jrb3@best.com> says some releases of gcc
      # support --undefined.  This deserves some investigation.  FIXME
      _LT_AC_TAGVAR(archive_cmds, $1)='$CC -nostart $libobjs $deplibs $compiler_flags ${wl}-soname $wl$soname -o $lib'
    else
      _LT_AC_TAGVAR(ld_shlibs, $1)=no
    fi
    ;;

  chorus*)
    case $cc_basename in
      *)
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
    esac
    ;;

  cygwin* | mingw* | pw32*)
    # _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1) is actually meaningless,
    # as there is no search path for DLLs.
    _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-L$libdir'
    _LT_AC_TAGVAR(allow_undefined_flag, $1)=unsupported
    _LT_AC_TAGVAR(always_export_symbols, $1)=no
    _LT_AC_TAGVAR(enable_shared_with_static_runtimes, $1)=yes

    if $LD --help 2>&1 | grep 'auto-import' > /dev/null; then
      _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -nostdlib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags -o $output_objdir/$soname ${wl}--enable-auto-image-base -Xlinker --out-implib -Xlinker $lib'
      # If the export-symbols file already is a .def file (1st line
      # is EXPORTS), use it as is; otherwise, prepend...
      _LT_AC_TAGVAR(archive_expsym_cmds, $1)='if test "x`$SED 1q $export_symbols`" = xEXPORTS; then
	cp $export_symbols $output_objdir/$soname.def;
      else
	echo EXPORTS > $output_objdir/$soname.def;
	cat $export_symbols >> $output_objdir/$soname.def;
      fi~
      $CC -shared -nostdlib $output_objdir/$soname.def $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags -o $output_objdir/$soname ${wl}--enable-auto-image-base -Xlinker --out-implib -Xlinker $lib'
    else
      _LT_AC_TAGVAR(ld_shlibs, $1)=no
    fi
  ;;
      darwin* | rhapsody*)
        case $host_os in
        rhapsody* | darwin1.[[012]])
         _LT_AC_TAGVAR(allow_undefined_flag, $1)='${wl}-undefined ${wl}suppress'
         ;;
       *) # Darwin 1.3 on
         if test -z ${MACOSX_DEPLOYMENT_TARGET} ; then
           _LT_AC_TAGVAR(allow_undefined_flag, $1)='${wl}-flat_namespace ${wl}-undefined ${wl}suppress'
         else
           case ${MACOSX_DEPLOYMENT_TARGET} in
             10.[[012]])
               _LT_AC_TAGVAR(allow_undefined_flag, $1)='${wl}-flat_namespace ${wl}-undefined ${wl}suppress'
               ;;
             10.*)
               _LT_AC_TAGVAR(allow_undefined_flag, $1)='${wl}-undefined ${wl}dynamic_lookup'
               ;;
           esac
         fi
         ;;
        esac
      _LT_AC_TAGVAR(archive_cmds_need_lc, $1)=no
      _LT_AC_TAGVAR(hardcode_direct, $1)=no
      _LT_AC_TAGVAR(hardcode_automatic, $1)=yes
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=unsupported
      _LT_AC_TAGVAR(whole_archive_flag_spec, $1)=''
      _LT_AC_TAGVAR(link_all_deplibs, $1)=yes

    if test "$GXX" = yes ; then
      lt_int_apple_cc_single_mod=no
      output_verbose_link_cmd='echo'
      if $CC -dumpspecs 2>&1 | $EGREP 'single_module' >/dev/null ; then
       lt_int_apple_cc_single_mod=yes
      fi
      if test "X$lt_int_apple_cc_single_mod" = Xyes ; then
       _LT_AC_TAGVAR(archive_cmds, $1)='$CC -dynamiclib -single_module $allow_undefined_flag -o $lib $libobjs $deplibs $compiler_flags -install_name $rpath/$soname $verstring'
      else
          _LT_AC_TAGVAR(archive_cmds, $1)='$CC -r -keep_private_externs -nostdlib -o ${lib}-master.o $libobjs~$CC -dynamiclib $allow_undefined_flag -o $lib ${lib}-master.o $deplibs $compiler_flags -install_name $rpath/$soname $verstring'
        fi
        _LT_AC_TAGVAR(module_cmds, $1)='$CC $allow_undefined_flag -o $lib -bundle $libobjs $deplibs$compiler_flags'
        # Don't fix this by using the ld -exported_symbols_list flag, it doesn't exist in older darwin lds
          if test "X$lt_int_apple_cc_single_mod" = Xyes ; then
            _LT_AC_TAGVAR(archive_expsym_cmds, $1)='sed -e "s,#.*,," -e "s,^[    ]*,," -e "s,^\(..*\),_&," < $export_symbols > $output_objdir/${libname}-symbols.expsym~$CC -dynamiclib -single_module $allow_undefined_flag -o $lib $libobjs $deplibs $compiler_flags -install_name $rpath/$soname $verstring~nmedit -s $output_objdir/${libname}-symbols.expsym ${lib}'
          else
            _LT_AC_TAGVAR(archive_expsym_cmds, $1)='sed -e "s,#.*,," -e "s,^[    ]*,," -e "s,^\(..*\),_&," < $export_symbols > $output_objdir/${libname}-symbols.expsym~$CC -r -keep_private_externs -nostdlib -o ${lib}-master.o $libobjs~$CC -dynamiclib $allow_undefined_flag -o $lib ${lib}-master.o $deplibs $compiler_flags -install_name $rpath/$soname $verstring~nmedit -s $output_objdir/${libname}-symbols.expsym ${lib}'
          fi
            _LT_AC_TAGVAR(module_expsym_cmds, $1)='sed -e "s,#.*,," -e "s,^[    ]*,," -e "s,^\(..*\),_&," < $export_symbols > $output_objdir/${libname}-symbols.expsym~$CC $allow_undefined_flag  -o $lib -bundle $libobjs $deplibs$compiler_flags~nmedit -s $output_objdir/${libname}-symbols.expsym ${lib}'
      else
      case $cc_basename in
        xlc*)
         output_verbose_link_cmd='echo'
          _LT_AC_TAGVAR(archive_cmds, $1)='$CC -qmkshrobj ${wl}-single_module $allow_undefined_flag -o $lib $libobjs $deplibs $compiler_flags ${wl}-install_name ${wl}`echo $rpath/$soname` $verstring'
          _LT_AC_TAGVAR(module_cmds, $1)='$CC $allow_undefined_flag -o $lib -bundle $libobjs $deplibs$compiler_flags'
          # Don't fix this by using the ld -exported_symbols_list flag, it doesn't exist in older darwin lds
          _LT_AC_TAGVAR(archive_expsym_cmds, $1)='sed -e "s,#.*,," -e "s,^[    ]*,," -e "s,^\(..*\),_&," < $export_symbols > $output_objdir/${libname}-symbols.expsym~$CC -qmkshrobj ${wl}-single_module $allow_undefined_flag -o $lib $libobjs $deplibs $compiler_flags ${wl}-install_name ${wl}$rpath/$soname $verstring~nmedit -s $output_objdir/${libname}-symbols.expsym ${lib}'
          _LT_AC_TAGVAR(module_expsym_cmds, $1)='sed -e "s,#.*,," -e "s,^[    ]*,," -e "s,^\(..*\),_&," < $export_symbols > $output_objdir/${libname}-symbols.expsym~$CC $allow_undefined_flag  -o $lib -bundle $libobjs $deplibs$compiler_flags~nmedit -s $output_objdir/${libname}-symbols.expsym ${lib}'
          ;;
       *)
         _LT_AC_TAGVAR(ld_shlibs, $1)=no
          ;;
      esac
      fi
        ;;

  dgux*)
    case $cc_basename in
      ec++*)
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
      ghcx*)
	# Green Hills C++ Compiler
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
      *)
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
    esac
    ;;
  freebsd[[12]]*)
    # C++ shared libraries reported to be fairly broken before switch to ELF
    _LT_AC_TAGVAR(ld_shlibs, $1)=no
    ;;
  freebsd-elf*)
    _LT_AC_TAGVAR(archive_cmds_need_lc, $1)=no
    ;;
  freebsd* | kfreebsd*-gnu | dragonfly*)
    # FreeBSD 3 and later use GNU C++ and GNU ld with standard ELF
    # conventions
    _LT_AC_TAGVAR(ld_shlibs, $1)=yes
    ;;
  gnu*)
    ;;
  hpux9*)
    _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}+b ${wl}$libdir'
    _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:
    _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}-E'
    _LT_AC_TAGVAR(hardcode_direct, $1)=yes
    _LT_AC_TAGVAR(hardcode_minus_L, $1)=yes # Not in the search PATH,
				# but as the default
				# location of the library.

    case $cc_basename in
    CC*)
      # FIXME: insert proper C++ library support
      _LT_AC_TAGVAR(ld_shlibs, $1)=no
      ;;
    aCC*)
      _LT_AC_TAGVAR(archive_cmds, $1)='$rm $output_objdir/$soname~$CC -b ${wl}+b ${wl}$install_libdir -o $output_objdir/$soname $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags~test $output_objdir/$soname = $lib || mv $output_objdir/$soname $lib'
      # Commands to make compiler produce verbose output that lists
      # what "hidden" libraries, object files and flags are used when
      # linking a shared library.
      #
      # There doesn't appear to be a way to prevent this compiler from
      # explicitly linking system object files so we need to strip them
      # from the output so that they don't get included in the library
      # dependencies.
      output_verbose_link_cmd='templist=`($CC -b $CFLAGS -v conftest.$objext 2>&1) | grep "[[-]]L"`; list=""; for z in $templist; do case $z in conftest.$objext) list="$list $z";; *.$objext);; *) list="$list $z";;esac; done; echo $list'
      ;;
    *)
      if test "$GXX" = yes; then
        _LT_AC_TAGVAR(archive_cmds, $1)='$rm $output_objdir/$soname~$CC -shared -nostdlib -fPIC ${wl}+b ${wl}$install_libdir -o $output_objdir/$soname $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags~test $output_objdir/$soname = $lib || mv $output_objdir/$soname $lib'
      else
        # FIXME: insert proper C++ library support
        _LT_AC_TAGVAR(ld_shlibs, $1)=no
      fi
      ;;
    esac
    ;;
  hpux10*|hpux11*)
    if test $with_gnu_ld = no; then
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}+b ${wl}$libdir'
      _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:

      case $host_cpu in
      hppa*64*|ia64*)
	_LT_AC_TAGVAR(hardcode_libdir_flag_spec_ld, $1)='+b $libdir'
        ;;
      *)
	_LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}-E'
        ;;
      esac
    fi
    case $host_cpu in
    hppa*64*|ia64*)
      _LT_AC_TAGVAR(hardcode_direct, $1)=no
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      ;;
    *)
      _LT_AC_TAGVAR(hardcode_direct, $1)=yes
      _LT_AC_TAGVAR(hardcode_minus_L, $1)=yes # Not in the search PATH,
					      # but as the default
					      # location of the library.
      ;;
    esac

    case $cc_basename in
      CC*)
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
      aCC*)
	case $host_cpu in
	hppa*64*)
	  _LT_AC_TAGVAR(archive_cmds, $1)='$CC -b ${wl}+h ${wl}$soname -o $lib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags'
	  ;;
	ia64*)
	  _LT_AC_TAGVAR(archive_cmds, $1)='$CC -b ${wl}+h ${wl}$soname ${wl}+nodefaultrpath -o $lib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags'
	  ;;
	*)
	  _LT_AC_TAGVAR(archive_cmds, $1)='$CC -b ${wl}+h ${wl}$soname ${wl}+b ${wl}$install_libdir -o $lib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags'
	  ;;
	esac
	# Commands to make compiler produce verbose output that lists
	# what "hidden" libraries, object files and flags are used when
	# linking a shared library.
	#
	# There doesn't appear to be a way to prevent this compiler from
	# explicitly linking system object files so we need to strip them
	# from the output so that they don't get included in the library
	# dependencies.
	output_verbose_link_cmd='templist=`($CC -b $CFLAGS -v conftest.$objext 2>&1) | grep "\-L"`; list=""; for z in $templist; do case $z in conftest.$objext) list="$list $z";; *.$objext);; *) list="$list $z";;esac; done; echo $list'
	;;
      *)
	if test "$GXX" = yes; then
	  if test $with_gnu_ld = no; then
	    case $host_cpu in
	    hppa*64*)
	      _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -nostdlib -fPIC ${wl}+h ${wl}$soname -o $lib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags'
	      ;;
	    ia64*)
	      _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -nostdlib -fPIC ${wl}+h ${wl}$soname ${wl}+nodefaultrpath -o $lib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags'
	      ;;
	    *)
	      _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -nostdlib -fPIC ${wl}+h ${wl}$soname ${wl}+b ${wl}$install_libdir -o $lib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags'
	      ;;
	    esac
	  fi
	else
	  # FIXME: insert proper C++ library support
	  _LT_AC_TAGVAR(ld_shlibs, $1)=no
	fi
	;;
    esac
    ;;
  interix3*)
    _LT_AC_TAGVAR(hardcode_direct, $1)=no
    _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
    _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath,$libdir'
    _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}-E'
    # Hack: On Interix 3.x, we cannot compile PIC because of a broken gcc.
    # Instead, shared libraries are loaded at an image base (0x10000000 by
    # default) and relocated if they conflict, which is a slow very memory
    # consuming and fragmenting process.  To avoid this, we pick a random,
    # 256 KiB-aligned image base between 0x50000000 and 0x6FFC0000 at link
    # time.  Moving up from 0x10000000 also allows more sbrk(2) space.
    _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $pic_flag $libobjs $deplibs $compiler_flags ${wl}-h,$soname ${wl}--image-base,`expr ${RANDOM-$$} % 4096 / 2 \* 262144 + 1342177280` -o $lib'
    _LT_AC_TAGVAR(archive_expsym_cmds, $1)='sed "s,^,_," $export_symbols >$output_objdir/$soname.expsym~$CC -shared $pic_flag $libobjs $deplibs $compiler_flags ${wl}-h,$soname ${wl}--retain-symbols-file,$output_objdir/$soname.expsym ${wl}--image-base,`expr ${RANDOM-$$} % 4096 / 2 \* 262144 + 1342177280` -o $lib'
    ;;
  irix5* | irix6*)
    case $cc_basename in
      CC*)
	# SGI C++
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -all -multigot $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags -soname $soname `test -n "$verstring" && echo -set_version $verstring` -update_registry ${output_objdir}/so_locations -o $lib'

	# Archives containing C++ object files must be created using
	# "CC -ar", where "CC" is the IRIX C++ compiler.  This is
	# necessary to make sure instantiated templates are included
	# in the archive.
	_LT_AC_TAGVAR(old_archive_cmds, $1)='$CC -ar -WR,-u -o $oldlib $oldobjs'
	;;
      *)
	if test "$GXX" = yes; then
	  if test "$with_gnu_ld" = no; then
	    _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -nostdlib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-soname ${wl}$soname `test -n "$verstring" && echo ${wl}-set_version ${wl}$verstring` ${wl}-update_registry ${wl}${output_objdir}/so_locations -o $lib'
	  else
	    _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -nostdlib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-soname ${wl}$soname `test -n "$verstring" && echo ${wl}-set_version ${wl}$verstring` -o $lib'
	  fi
	fi
	_LT_AC_TAGVAR(link_all_deplibs, $1)=yes
	;;
    esac
    _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath ${wl}$libdir'
    _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:
    ;;
  linux*)
    case $cc_basename in
      KCC*)
	# Kuck and Associates, Inc. (KAI) C++ Compiler

	# KCC will only create a shared library if the output file
	# ends with ".so" (or ".sl" for HP-UX), so rename the library
	# to its proper name (with version) after linking.
	_LT_AC_TAGVAR(archive_cmds, $1)='tempext=`echo $shared_ext | $SED -e '\''s/\([[^()0-9A-Za-z{}]]\)/\\\\\1/g'\''`; templib=`echo $lib | $SED -e "s/\${tempext}\..*/.so/"`; $CC $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags --soname $soname -o \$templib; mv \$templib $lib'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='tempext=`echo $shared_ext | $SED -e '\''s/\([[^()0-9A-Za-z{}]]\)/\\\\\1/g'\''`; templib=`echo $lib | $SED -e "s/\${tempext}\..*/.so/"`; $CC $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags --soname $soname -o \$templib ${wl}-retain-symbols-file,$export_symbols; mv \$templib $lib'
	# Commands to make compiler produce verbose output that lists
	# what "hidden" libraries, object files and flags are used when
	# linking a shared library.
	#
	# There doesn't appear to be a way to prevent this compiler from
	# explicitly linking system object files so we need to strip them
	# from the output so that they don't get included in the library
	# dependencies.
	output_verbose_link_cmd='templist=`$CC $CFLAGS -v conftest.$objext -o libconftest$shared_ext 2>&1 | grep "ld"`; rm -f libconftest$shared_ext; list=""; for z in $templist; do case $z in conftest.$objext) list="$list $z";; *.$objext);; *) list="$list $z";;esac; done; echo $list'

	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}--rpath,$libdir'
	_LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}--export-dynamic'

	# Archives containing C++ object files must be created using
	# "CC -Bstatic", where "CC" is the KAI C++ compiler.
	_LT_AC_TAGVAR(old_archive_cmds, $1)='$CC -Bstatic -o $oldlib $oldobjs'
	;;
      icpc*)
	# Intel C++
	with_gnu_ld=yes
	# version 8.0 and above of icpc choke on multiply defined symbols
	# if we add $predep_objects and $postdep_objects, however 7.1 and
	# earlier do not add the objects themselves.
	case `$CC -V 2>&1` in
	*"Version 7."*)
  	  _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-soname $wl$soname -o $lib'
  	  _LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-soname $wl$soname ${wl}-retain-symbols-file $wl$export_symbols -o $lib'
	  ;;
	*)  # Version 8.0 or newer
	  tmp_idyn=
	  case $host_cpu in
	    ia64*) tmp_idyn=' -i_dynamic';;
	  esac
  	  _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared'"$tmp_idyn"' $libobjs $deplibs $compiler_flags ${wl}-soname $wl$soname -o $lib'
	  _LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared'"$tmp_idyn"' $libobjs $deplibs $compiler_flags ${wl}-soname $wl$soname ${wl}-retain-symbols-file $wl$export_symbols -o $lib'
	  ;;
	esac
	_LT_AC_TAGVAR(archive_cmds_need_lc, $1)=no
	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath,$libdir'
	_LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}--export-dynamic'
	_LT_AC_TAGVAR(whole_archive_flag_spec, $1)='${wl}--whole-archive$convenience ${wl}--no-whole-archive'
	;;
      pgCC*)
        # Portland Group C++ compiler
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $pic_flag $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-soname ${wl}$soname -o $lib'
  	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared $pic_flag $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-soname ${wl}$soname ${wl}-retain-symbols-file ${wl}$export_symbols -o $lib'

	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}--rpath ${wl}$libdir'
	_LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}--export-dynamic'
	_LT_AC_TAGVAR(whole_archive_flag_spec, $1)='${wl}--whole-archive`for conv in $convenience\"\"; do test  -n \"$conv\" && new_convenience=\"$new_convenience,$conv\"; done; $echo \"$new_convenience\"` ${wl}--no-whole-archive'
        ;;
      cxx*)
	# Compaq C++
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-soname $wl$soname -o $lib'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-soname $wl$soname  -o $lib ${wl}-retain-symbols-file $wl$export_symbols'

	runpath_var=LD_RUN_PATH
	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-rpath $libdir'
	_LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:

	# Commands to make compiler produce verbose output that lists
	# what "hidden" libraries, object files and flags are used when
	# linking a shared library.
	#
	# There doesn't appear to be a way to prevent this compiler from
	# explicitly linking system object files so we need to strip them
	# from the output so that they don't get included in the library
	# dependencies.
	output_verbose_link_cmd='templist=`$CC -shared $CFLAGS -v conftest.$objext 2>&1 | grep "ld"`; templist=`echo $templist | $SED "s/\(^.*ld.*\)\( .*ld .*$\)/\1/"`; list=""; for z in $templist; do case $z in conftest.$objext) list="$list $z";; *.$objext);; *) list="$list $z";;esac; done; echo $list'
	;;
    esac
    ;;
  lynxos*)
    # FIXME: insert proper C++ library support
    _LT_AC_TAGVAR(ld_shlibs, $1)=no
    ;;
  m88k*)
    # FIXME: insert proper C++ library support
    _LT_AC_TAGVAR(ld_shlibs, $1)=no
    ;;
  mvs*)
    case $cc_basename in
      cxx*)
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
      *)
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
    esac
    ;;
  netbsd*)
    if echo __ELF__ | $CC -E - | grep __ELF__ >/dev/null; then
      _LT_AC_TAGVAR(archive_cmds, $1)='$LD -Bshareable  -o $lib $predep_objects $libobjs $deplibs $postdep_objects $linker_flags'
      wlarc=
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-R$libdir'
      _LT_AC_TAGVAR(hardcode_direct, $1)=yes
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
    fi
    # Workaround some broken pre-1.5 toolchains
    output_verbose_link_cmd='$CC -shared $CFLAGS -v conftest.$objext 2>&1 | grep conftest.$objext | $SED -e "s:-lgcc -lc -lgcc::"'
    ;;
  openbsd2*)
    # C++ shared libraries are fairly broken
    _LT_AC_TAGVAR(ld_shlibs, $1)=no
    ;;
  openbsd*)
    _LT_AC_TAGVAR(hardcode_direct, $1)=yes
    _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
    _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $pic_flag $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags -o $lib'
    _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath,$libdir'
    if test -z "`echo __ELF__ | $CC -E - | grep __ELF__`" || test "$host_os-$host_cpu" = "openbsd2.8-powerpc"; then
      _LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared $pic_flag $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-retain-symbols-file,$export_symbols -o $lib'
      _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}-E'
      _LT_AC_TAGVAR(whole_archive_flag_spec, $1)="$wlarc"'--whole-archive$convenience '"$wlarc"'--no-whole-archive'
    fi
    output_verbose_link_cmd='echo'
    ;;
  osf3*)
    case $cc_basename in
      KCC*)
	# Kuck and Associates, Inc. (KAI) C++ Compiler

	# KCC will only create a shared library if the output file
	# ends with ".so" (or ".sl" for HP-UX), so rename the library
	# to its proper name (with version) after linking.
	_LT_AC_TAGVAR(archive_cmds, $1)='tempext=`echo $shared_ext | $SED -e '\''s/\([[^()0-9A-Za-z{}]]\)/\\\\\1/g'\''`; templib=`echo $lib | $SED -e "s/\${tempext}\..*/.so/"`; $CC $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags --soname $soname -o \$templib; mv \$templib $lib'

	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath,$libdir'
	_LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:

	# Archives containing C++ object files must be created using
	# "CC -Bstatic", where "CC" is the KAI C++ compiler.
	_LT_AC_TAGVAR(old_archive_cmds, $1)='$CC -Bstatic -o $oldlib $oldobjs'

	;;
      RCC*)
	# Rational C++ 2.4.1
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
      cxx*)
	_LT_AC_TAGVAR(allow_undefined_flag, $1)=' ${wl}-expect_unresolved ${wl}\*'
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared${allow_undefined_flag} $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-soname $soname `test -n "$verstring" && echo ${wl}-set_version $verstring` -update_registry ${output_objdir}/so_locations -o $lib'

	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath ${wl}$libdir'
	_LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:

	# Commands to make compiler produce verbose output that lists
	# what "hidden" libraries, object files and flags are used when
	# linking a shared library.
	#
	# There doesn't appear to be a way to prevent this compiler from
	# explicitly linking system object files so we need to strip them
	# from the output so that they don't get included in the library
	# dependencies.
	output_verbose_link_cmd='templist=`$CC -shared $CFLAGS -v conftest.$objext 2>&1 | grep "ld" | grep -v "ld:"`; templist=`echo $templist | $SED "s/\(^.*ld.*\)\( .*ld.*$\)/\1/"`; list=""; for z in $templist; do case $z in conftest.$objext) list="$list $z";; *.$objext);; *) list="$list $z";;esac; done; echo $list'
	;;
      *)
	if test "$GXX" = yes && test "$with_gnu_ld" = no; then
	  _LT_AC_TAGVAR(allow_undefined_flag, $1)=' ${wl}-expect_unresolved ${wl}\*'
	  _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -nostdlib ${allow_undefined_flag} $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-soname ${wl}$soname `test -n "$verstring" && echo ${wl}-set_version ${wl}$verstring` ${wl}-update_registry ${wl}${output_objdir}/so_locations -o $lib'

	  _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath ${wl}$libdir'
	  _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:

	  # Commands to make compiler produce verbose output that lists
	  # what "hidden" libraries, object files and flags are used when
	  # linking a shared library.
	  output_verbose_link_cmd='$CC -shared $CFLAGS -v conftest.$objext 2>&1 | grep "\-L"'

	else
	  # FIXME: insert proper C++ library support
	  _LT_AC_TAGVAR(ld_shlibs, $1)=no
	fi
	;;
    esac
    ;;
  osf4* | osf5*)
    case $cc_basename in
      KCC*)
	# Kuck and Associates, Inc. (KAI) C++ Compiler

	# KCC will only create a shared library if the output file
	# ends with ".so" (or ".sl" for HP-UX), so rename the library
	# to its proper name (with version) after linking.
	_LT_AC_TAGVAR(archive_cmds, $1)='tempext=`echo $shared_ext | $SED -e '\''s/\([[^()0-9A-Za-z{}]]\)/\\\\\1/g'\''`; templib=`echo $lib | $SED -e "s/\${tempext}\..*/.so/"`; $CC $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags --soname $soname -o \$templib; mv \$templib $lib'

	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath,$libdir'
	_LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:

	# Archives containing C++ object files must be created using
	# the KAI C++ compiler.
	_LT_AC_TAGVAR(old_archive_cmds, $1)='$CC -o $oldlib $oldobjs'
	;;
      RCC*)
	# Rational C++ 2.4.1
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
      cxx*)
	_LT_AC_TAGVAR(allow_undefined_flag, $1)=' -expect_unresolved \*'
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared${allow_undefined_flag} $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags -msym -soname $soname `test -n "$verstring" && echo -set_version $verstring` -update_registry ${output_objdir}/so_locations -o $lib'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='for i in `cat $export_symbols`; do printf "%s %s\\n" -exported_symbol "\$i" >> $lib.exp; done~
	  echo "-hidden">> $lib.exp~
	  $CC -shared$allow_undefined_flag $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags -msym -soname $soname -Wl,-input -Wl,$lib.exp  `test -n "$verstring" && echo -set_version	$verstring` -update_registry ${output_objdir}/so_locations -o $lib~
	  $rm $lib.exp'

	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-rpath $libdir'
	_LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:

	# Commands to make compiler produce verbose output that lists
	# what "hidden" libraries, object files and flags are used when
	# linking a shared library.
	#
	# There doesn't appear to be a way to prevent this compiler from
	# explicitly linking system object files so we need to strip them
	# from the output so that they don't get included in the library
	# dependencies.
	output_verbose_link_cmd='templist=`$CC -shared $CFLAGS -v conftest.$objext 2>&1 | grep "ld" | grep -v "ld:"`; templist=`echo $templist | $SED "s/\(^.*ld.*\)\( .*ld.*$\)/\1/"`; list=""; for z in $templist; do case $z in conftest.$objext) list="$list $z";; *.$objext);; *) list="$list $z";;esac; done; echo $list'
	;;
      *)
	if test "$GXX" = yes && test "$with_gnu_ld" = no; then
	  _LT_AC_TAGVAR(allow_undefined_flag, $1)=' ${wl}-expect_unresolved ${wl}\*'
	 _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -nostdlib ${allow_undefined_flag} $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-msym ${wl}-soname ${wl}$soname `test -n "$verstring" && echo ${wl}-set_version ${wl}$verstring` ${wl}-update_registry ${wl}${output_objdir}/so_locations -o $lib'

	  _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath ${wl}$libdir'
	  _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:

	  # Commands to make compiler produce verbose output that lists
	  # what "hidden" libraries, object files and flags are used when
	  # linking a shared library.
	  output_verbose_link_cmd='$CC -shared $CFLAGS -v conftest.$objext 2>&1 | grep "\-L"'

	else
	  # FIXME: insert proper C++ library support
	  _LT_AC_TAGVAR(ld_shlibs, $1)=no
	fi
	;;
    esac
    ;;
  psos*)
    # FIXME: insert proper C++ library support
    _LT_AC_TAGVAR(ld_shlibs, $1)=no
    ;;
  sunos4*)
    case $cc_basename in
      CC*)
	# Sun C++ 4.x
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
      lcc*)
	# Lucid
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
      *)
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
    esac
    ;;
  solaris*)
    case $cc_basename in
      CC*)
	# Sun C++ 4.2, 5.x and Centerline C++
        _LT_AC_TAGVAR(archive_cmds_need_lc,$1)=yes
	_LT_AC_TAGVAR(no_undefined_flag, $1)=' -zdefs'
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -G${allow_undefined_flag}  -h$soname -o $lib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$echo "{ global:" > $lib.exp~cat $export_symbols | $SED -e "s/\(.*\)/\1;/" >> $lib.exp~$echo "local: *; };" >> $lib.exp~
	$CC -G${allow_undefined_flag}  ${wl}-M ${wl}$lib.exp -h$soname -o $lib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags~$rm $lib.exp'

	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-R$libdir'
	_LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
	case $host_os in
	  solaris2.[[0-5]] | solaris2.[[0-5]].*) ;;
	  *)
	    # The C++ compiler is used as linker so we must use $wl
	    # flag to pass the commands to the underlying system
	    # linker. We must also pass each convience library through
	    # to the system linker between allextract/defaultextract.
	    # The C++ compiler will combine linker options so we
	    # cannot just pass the convience library names through
	    # without $wl.
	    # Supported since Solaris 2.6 (maybe 2.5.1?)
	    _LT_AC_TAGVAR(whole_archive_flag_spec, $1)='${wl}-z ${wl}allextract`for conv in $convenience\"\"; do test -n \"$conv\" && new_convenience=\"$new_convenience,$conv\"; done; $echo \"$new_convenience\"` ${wl}-z ${wl}defaultextract'
	    ;;
	esac
	_LT_AC_TAGVAR(link_all_deplibs, $1)=yes

	output_verbose_link_cmd='echo'

	# Archives containing C++ object files must be created using
	# "CC -xar", where "CC" is the Sun C++ compiler.  This is
	# necessary to make sure instantiated templates are included
	# in the archive.
	_LT_AC_TAGVAR(old_archive_cmds, $1)='$CC -xar -o $oldlib $oldobjs'
	;;
      gcx*)
	# Green Hills C++ Compiler
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-h $wl$soname -o $lib'

	# The C++ compiler must be used to create the archive.
	_LT_AC_TAGVAR(old_archive_cmds, $1)='$CC $LDFLAGS -archive -o $oldlib $oldobjs'
	;;
      *)
	# GNU C++ compiler with Solaris linker
	if test "$GXX" = yes && test "$with_gnu_ld" = no; then
	  _LT_AC_TAGVAR(no_undefined_flag, $1)=' ${wl}-z ${wl}defs'
	  if $CC --version | grep -v '^2\.7' > /dev/null; then
	    _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -nostdlib $LDFLAGS $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-h $wl$soname -o $lib'
	    _LT_AC_TAGVAR(archive_expsym_cmds, $1)='$echo "{ global:" > $lib.exp~cat $export_symbols | $SED -e "s/\(.*\)/\1;/" >> $lib.exp~$echo "local: *; };" >> $lib.exp~
		$CC -shared -nostdlib ${wl}-M $wl$lib.exp -o $lib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags~$rm $lib.exp'

	    # Commands to make compiler produce verbose output that lists
	    # what "hidden" libraries, object files and flags are used when
	    # linking a shared library.
	    output_verbose_link_cmd="$CC -shared $CFLAGS -v conftest.$objext 2>&1 | grep \"\-L\""
	  else
	    # g++ 2.7 appears to require `-G' NOT `-shared' on this
	    # platform.
	    _LT_AC_TAGVAR(archive_cmds, $1)='$CC -G -nostdlib $LDFLAGS $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags ${wl}-h $wl$soname -o $lib'
	    _LT_AC_TAGVAR(archive_expsym_cmds, $1)='$echo "{ global:" > $lib.exp~cat $export_symbols | $SED -e "s/\(.*\)/\1;/" >> $lib.exp~$echo "local: *; };" >> $lib.exp~
		$CC -G -nostdlib ${wl}-M $wl$lib.exp -o $lib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags~$rm $lib.exp'

	    # Commands to make compiler produce verbose output that lists
	    # what "hidden" libraries, object files and flags are used when
	    # linking a shared library.
	    output_verbose_link_cmd="$CC -G $CFLAGS -v conftest.$objext 2>&1 | grep \"\-L\""
	  fi

	  _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-R $wl$libdir'
	fi
	;;
    esac
    ;;
  sysv4*uw2* | sysv5OpenUNIX* | sysv5UnixWare7.[[01]].[[10]]* | unixware7* | sco3.2v5.0.[[024]]*)
    _LT_AC_TAGVAR(no_undefined_flag, $1)='${wl}-z,text'
    _LT_AC_TAGVAR(archive_cmds_need_lc, $1)=no
    _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
    runpath_var='LD_RUN_PATH'

    case $cc_basename in
      CC*)
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -G ${wl}-h,$soname -o $lib $libobjs $deplibs $compiler_flags'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -G ${wl}-Bexport:$export_symbols ${wl}-h,$soname -o $lib $libobjs $deplibs $compiler_flags'
	;;
      *)
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared ${wl}-h,$soname -o $lib $libobjs $deplibs $compiler_flags'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared ${wl}-Bexport:$export_symbols ${wl}-h,$soname -o $lib $libobjs $deplibs $compiler_flags'
	;;
    esac
    ;;
  sysv5* | sco3.2v5* | sco5v6*)
    # Note: We can NOT use -z defs as we might desire, because we do not
    # link with -lc, and that would cause any symbols used from libc to
    # always be unresolved, which means just about no library would
    # ever link correctly.  If we're not using GNU ld we use -z text
    # though, which does catch some bad symbols but isn't as heavy-handed
    # as -z defs.
    # For security reasons, it is highly recommended that you always
    # use absolute paths for naming shared libraries, and exclude the
    # DT_RUNPATH tag from executables and libraries.  But doing so
    # requires that you compile everything twice, which is a pain.
    # So that behaviour is only enabled if SCOABSPATH is set to a
    # non-empty value in the environment.  Most likely only useful for
    # creating official distributions of packages.
    # This is a hack until libtool officially supports absolute path
    # names for shared libraries.
    _LT_AC_TAGVAR(no_undefined_flag, $1)='${wl}-z,text'
    _LT_AC_TAGVAR(allow_undefined_flag, $1)='${wl}-z,nodefs'
    _LT_AC_TAGVAR(archive_cmds_need_lc, $1)=no
    _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
    _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='`test -z "$SCOABSPATH" && echo ${wl}-R,$libdir`'
    _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=':'
    _LT_AC_TAGVAR(link_all_deplibs, $1)=yes
    _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}-Bexport'
    runpath_var='LD_RUN_PATH'

    case $cc_basename in
      CC*)
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -G ${wl}-h,\${SCOABSPATH:+${install_libdir}/}$soname -o $lib $libobjs $deplibs $compiler_flags'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -G ${wl}-Bexport:$export_symbols ${wl}-h,\${SCOABSPATH:+${install_libdir}/}$soname -o $lib $libobjs $deplibs $compiler_flags'
	;;
      *)
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared ${wl}-h,\${SCOABSPATH:+${install_libdir}/}$soname -o $lib $libobjs $deplibs $compiler_flags'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared ${wl}-Bexport:$export_symbols ${wl}-h,\${SCOABSPATH:+${install_libdir}/}$soname -o $lib $libobjs $deplibs $compiler_flags'
	;;
    esac
    ;;
  tandem*)
    case $cc_basename in
      NCC*)
	# NonStop-UX NCC 3.20
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
      *)
	# FIXME: insert proper C++ library support
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	;;
    esac
    ;;
  vxworks*)
    # FIXME: insert proper C++ library support
    _LT_AC_TAGVAR(ld_shlibs, $1)=no
    ;;
  *)
    # FIXME: insert proper C++ library support
    _LT_AC_TAGVAR(ld_shlibs, $1)=no
    ;;
esac
AC_MSG_RESULT([$_LT_AC_TAGVAR(ld_shlibs, $1)])
test "$_LT_AC_TAGVAR(ld_shlibs, $1)" = no && can_build_shared=no

_LT_AC_TAGVAR(GCC, $1)="$GXX"
_LT_AC_TAGVAR(LD, $1)="$LD"

AC_LIBTOOL_POSTDEP_PREDEP($1)
AC_LIBTOOL_PROG_COMPILER_PIC($1)
AC_LIBTOOL_PROG_CC_C_O($1)
AC_LIBTOOL_SYS_HARD_LINK_LOCKS($1)
AC_LIBTOOL_PROG_LD_SHLIBS($1)
AC_LIBTOOL_SYS_DYNAMIC_LINKER($1)
AC_LIBTOOL_PROG_LD_HARDCODE_LIBPATH($1)

AC_LIBTOOL_CONFIG($1)

AC_LANG_POP
CC=$lt_save_CC
LDCXX=$LD
LD=$lt_save_LD
GCC=$lt_save_GCC
with_gnu_ldcxx=$with_gnu_ld
with_gnu_ld=$lt_save_with_gnu_ld
lt_cv_path_LDCXX=$lt_cv_path_LD
lt_cv_path_LD=$lt_save_path_LD
lt_cv_prog_gnu_ldcxx=$lt_cv_prog_gnu_ld
lt_cv_prog_gnu_ld=$lt_save_with_gnu_ld
])# AC_LIBTOOL_LANG_CXX_CONFIG

# AC_LIBTOOL_POSTDEP_PREDEP([TAGNAME])
# ------------------------------------
# Figure out "hidden" library dependencies from verbose
# compiler output when linking a shared library.
# Parse the compiler output and extract the necessary
# objects, libraries and library flags.
AC_DEFUN([AC_LIBTOOL_POSTDEP_PREDEP],[
dnl we can't use the lt_simple_compile_test_code here,
dnl because it contains code intended for an executable,
dnl not a library.  It's possible we should let each
dnl tag define a new lt_????_link_test_code variable,
dnl but it's only used here...
ifelse([$1],[],[cat > conftest.$ac_ext <<EOF
int a;
void foo (void) { a = 0; }
EOF
],[$1],[CXX],[cat > conftest.$ac_ext <<EOF
class Foo
{
public:
  Foo (void) { a = 0; }
private:
  int a;
};
EOF
],[$1],[F77],[cat > conftest.$ac_ext <<EOF
      subroutine foo
      implicit none
      integer*4 a
      a=0
      return
      end
EOF
],[$1],[GCJ],[cat > conftest.$ac_ext <<EOF
public class foo {
  private int a;
  public void bar (void) {
    a = 0;
  }
};
EOF
])
dnl Parse the compiler output and extract the necessary
dnl objects, libraries and library flags.
if AC_TRY_EVAL(ac_compile); then
  # Parse the compiler output and extract the necessary
  # objects, libraries and library flags.

  # Sentinel used to keep track of whether or not we are before
  # the conftest object file.
  pre_test_object_deps_done=no

  # The `*' in the case matches for architectures that use `case' in
  # $output_verbose_cmd can trigger glob expansion during the loop
  # eval without this substitution.
  output_verbose_link_cmd=`$echo "X$output_verbose_link_cmd" | $Xsed -e "$no_glob_subst"`

  for p in `eval $output_verbose_link_cmd`; do
    case $p in

    -L* | -R* | -l*)
       # Some compilers place space between "-{L,R}" and the path.
       # Remove the space.
       if test $p = "-L" \
	  || test $p = "-R"; then
	 prev=$p
	 continue
       else
	 prev=
       fi

       if test "$pre_test_object_deps_done" = no; then
	 case $p in
	 -L* | -R*)
	   # Internal compiler library paths should come after those
	   # provided the user.  The postdeps already come after the
	   # user supplied libs so there is no need to process them.
	   if test -z "$_LT_AC_TAGVAR(compiler_lib_search_path, $1)"; then
	     _LT_AC_TAGVAR(compiler_lib_search_path, $1)="${prev}${p}"
	   else
	     _LT_AC_TAGVAR(compiler_lib_search_path, $1)="${_LT_AC_TAGVAR(compiler_lib_search_path, $1)} ${prev}${p}"
	   fi
	   ;;
	 # The "-l" case would never come before the object being
	 # linked, so don't bother handling this case.
	 esac
       else
	 if test -z "$_LT_AC_TAGVAR(postdeps, $1)"; then
	   _LT_AC_TAGVAR(postdeps, $1)="${prev}${p}"
	 else
	   _LT_AC_TAGVAR(postdeps, $1)="${_LT_AC_TAGVAR(postdeps, $1)} ${prev}${p}"
	 fi
       fi
       ;;

    *.$objext)
       # This assumes that the test object file only shows up
       # once in the compiler output.
       if test "$p" = "conftest.$objext"; then
	 pre_test_object_deps_done=yes
	 continue
       fi

       if test "$pre_test_object_deps_done" = no; then
	 if test -z "$_LT_AC_TAGVAR(predep_objects, $1)"; then
	   _LT_AC_TAGVAR(predep_objects, $1)="$p"
	 else
	   _LT_AC_TAGVAR(predep_objects, $1)="$_LT_AC_TAGVAR(predep_objects, $1) $p"
	 fi
       else
	 if test -z "$_LT_AC_TAGVAR(postdep_objects, $1)"; then
	   _LT_AC_TAGVAR(postdep_objects, $1)="$p"
	 else
	   _LT_AC_TAGVAR(postdep_objects, $1)="$_LT_AC_TAGVAR(postdep_objects, $1) $p"
	 fi
       fi
       ;;

    *) ;; # Ignore the rest.

    esac
  done

  # Clean up.
  rm -f a.out a.exe
else
  echo "libtool.m4: error: problem compiling $1 test program"
fi

$rm -f confest.$objext

# PORTME: override above test on systems where it is broken
ifelse([$1],[CXX],
[case $host_os in
interix3*)
  # Interix 3.5 installs completely hosed .la files for C++, so rather than
  # hack all around it, let's just trust "g++" to DTRT.
  _LT_AC_TAGVAR(predep_objects,$1)=
  _LT_AC_TAGVAR(postdep_objects,$1)=
  _LT_AC_TAGVAR(postdeps,$1)=
  ;;

solaris*)
  case $cc_basename in
  CC*)
    # Adding this requires a known-good setup of shared libraries for
    # Sun compiler versions before 5.6, else PIC objects from an old
    # archive will be linked into the output, leading to subtle bugs.
    _LT_AC_TAGVAR(postdeps,$1)='-lCstd -lCrun'
    ;;
  esac
  ;;
esac
])

case " $_LT_AC_TAGVAR(postdeps, $1) " in
*" -lc "*) _LT_AC_TAGVAR(archive_cmds_need_lc, $1)=no ;;
esac
])# AC_LIBTOOL_POSTDEP_PREDEP

# AC_LIBTOOL_LANG_F77_CONFIG
# --------------------------
# Ensure that the configuration vars for the C compiler are
# suitably defined.  Those variables are subsequently used by
# AC_LIBTOOL_CONFIG to write the compiler configuration to `libtool'.
AC_DEFUN([AC_LIBTOOL_LANG_F77_CONFIG], [_LT_AC_LANG_F77_CONFIG(F77)])
AC_DEFUN([_LT_AC_LANG_F77_CONFIG],
[AC_REQUIRE([AC_PROG_F77])
AC_LANG_PUSH(Fortran 77)

_LT_AC_TAGVAR(archive_cmds_need_lc, $1)=no
_LT_AC_TAGVAR(allow_undefined_flag, $1)=
_LT_AC_TAGVAR(always_export_symbols, $1)=no
_LT_AC_TAGVAR(archive_expsym_cmds, $1)=
_LT_AC_TAGVAR(export_dynamic_flag_spec, $1)=
_LT_AC_TAGVAR(hardcode_direct, $1)=no
_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)=
_LT_AC_TAGVAR(hardcode_libdir_flag_spec_ld, $1)=
_LT_AC_TAGVAR(hardcode_libdir_separator, $1)=
_LT_AC_TAGVAR(hardcode_minus_L, $1)=no
_LT_AC_TAGVAR(hardcode_automatic, $1)=no
_LT_AC_TAGVAR(module_cmds, $1)=
_LT_AC_TAGVAR(module_expsym_cmds, $1)=
_LT_AC_TAGVAR(link_all_deplibs, $1)=unknown
_LT_AC_TAGVAR(old_archive_cmds, $1)=$old_archive_cmds
_LT_AC_TAGVAR(no_undefined_flag, $1)=
_LT_AC_TAGVAR(whole_archive_flag_spec, $1)=
_LT_AC_TAGVAR(enable_shared_with_static_runtimes, $1)=no

# Source file extension for f77 test sources.
ac_ext=f

# Object file extension for compiled f77 test sources.
objext=o
_LT_AC_TAGVAR(objext, $1)=$objext

# Code to be used in simple compile tests
lt_simple_compile_test_code="      subroutine t\n      return\n      end\n"

# Code to be used in simple link tests
lt_simple_link_test_code="      program t\n      end\n"

# ltmain only uses $CC for tagged configurations so make sure $CC is set.
_LT_AC_SYS_COMPILER

# save warnings/boilerplate of simple test code
_LT_COMPILER_BOILERPLATE
_LT_LINKER_BOILERPLATE

# Allow CC to be a program name with arguments.
lt_save_CC="$CC"
CC=${F77-"f77"}
compiler=$CC
_LT_AC_TAGVAR(compiler, $1)=$CC
_LT_CC_BASENAME([$compiler])

AC_MSG_CHECKING([if libtool supports shared libraries])
AC_MSG_RESULT([$can_build_shared])

AC_MSG_CHECKING([whether to build shared libraries])
test "$can_build_shared" = "no" && enable_shared=no

# On AIX, shared libraries and static libraries use the same namespace, and
# are all built from PIC.
case $host_os in
aix3*)
  test "$enable_shared" = yes && enable_static=no
  if test -n "$RANLIB"; then
    archive_cmds="$archive_cmds~\$RANLIB \$lib"
    postinstall_cmds='$RANLIB $lib'
  fi
  ;;
aix4* | aix5*)
  if test "$host_cpu" != ia64 && test "$aix_use_runtimelinking" = no ; then
    test "$enable_shared" = yes && enable_static=no
  fi
  ;;
esac
AC_MSG_RESULT([$enable_shared])

AC_MSG_CHECKING([whether to build static libraries])
# Make sure either enable_shared or enable_static is yes.
test "$enable_shared" = yes || enable_static=yes
AC_MSG_RESULT([$enable_static])

_LT_AC_TAGVAR(GCC, $1)="$G77"
_LT_AC_TAGVAR(LD, $1)="$LD"

AC_LIBTOOL_PROG_COMPILER_PIC($1)
AC_LIBTOOL_PROG_CC_C_O($1)
AC_LIBTOOL_SYS_HARD_LINK_LOCKS($1)
AC_LIBTOOL_PROG_LD_SHLIBS($1)
AC_LIBTOOL_SYS_DYNAMIC_LINKER($1)
AC_LIBTOOL_PROG_LD_HARDCODE_LIBPATH($1)

AC_LIBTOOL_CONFIG($1)

AC_LANG_POP
CC="$lt_save_CC"
])# AC_LIBTOOL_LANG_F77_CONFIG


# AC_LIBTOOL_LANG_GCJ_CONFIG
# --------------------------
# Ensure that the configuration vars for the C compiler are
# suitably defined.  Those variables are subsequently used by
# AC_LIBTOOL_CONFIG to write the compiler configuration to `libtool'.
AC_DEFUN([AC_LIBTOOL_LANG_GCJ_CONFIG], [_LT_AC_LANG_GCJ_CONFIG(GCJ)])
AC_DEFUN([_LT_AC_LANG_GCJ_CONFIG],
[AC_LANG_SAVE

# Source file extension for Java test sources.
ac_ext=java

# Object file extension for compiled Java test sources.
objext=o
_LT_AC_TAGVAR(objext, $1)=$objext

# Code to be used in simple compile tests
lt_simple_compile_test_code="class foo {}\n"

# Code to be used in simple link tests
lt_simple_link_test_code='public class conftest { public static void main(String[[]] argv) {}; }\n'

# ltmain only uses $CC for tagged configurations so make sure $CC is set.
_LT_AC_SYS_COMPILER

# save warnings/boilerplate of simple test code
_LT_COMPILER_BOILERPLATE
_LT_LINKER_BOILERPLATE

# Allow CC to be a program name with arguments.
lt_save_CC="$CC"
CC=${GCJ-"gcj"}
compiler=$CC
_LT_AC_TAGVAR(compiler, $1)=$CC
_LT_CC_BASENAME([$compiler])

# GCJ did not exist at the time GCC didn't implicitly link libc in.
_LT_AC_TAGVAR(archive_cmds_need_lc, $1)=no

_LT_AC_TAGVAR(old_archive_cmds, $1)=$old_archive_cmds

AC_LIBTOOL_PROG_COMPILER_NO_RTTI($1)
AC_LIBTOOL_PROG_COMPILER_PIC($1)
AC_LIBTOOL_PROG_CC_C_O($1)
AC_LIBTOOL_SYS_HARD_LINK_LOCKS($1)
AC_LIBTOOL_PROG_LD_SHLIBS($1)
AC_LIBTOOL_SYS_DYNAMIC_LINKER($1)
AC_LIBTOOL_PROG_LD_HARDCODE_LIBPATH($1)

AC_LIBTOOL_CONFIG($1)

AC_LANG_RESTORE
CC="$lt_save_CC"
])# AC_LIBTOOL_LANG_GCJ_CONFIG


# AC_LIBTOOL_LANG_RC_CONFIG
# -------------------------
# Ensure that the configuration vars for the Windows resource compiler are
# suitably defined.  Those variables are subsequently used by
# AC_LIBTOOL_CONFIG to write the compiler configuration to `libtool'.
AC_DEFUN([AC_LIBTOOL_LANG_RC_CONFIG], [_LT_AC_LANG_RC_CONFIG(RC)])
AC_DEFUN([_LT_AC_LANG_RC_CONFIG],
[AC_LANG_SAVE

# Source file extension for RC test sources.
ac_ext=rc

# Object file extension for compiled RC test sources.
objext=o
_LT_AC_TAGVAR(objext, $1)=$objext

# Code to be used in simple compile tests
lt_simple_compile_test_code='sample MENU { MENUITEM "&Soup", 100, CHECKED }\n'

# Code to be used in simple link tests
lt_simple_link_test_code="$lt_simple_compile_test_code"

# ltmain only uses $CC for tagged configurations so make sure $CC is set.
_LT_AC_SYS_COMPILER

# save warnings/boilerplate of simple test code
_LT_COMPILER_BOILERPLATE
_LT_LINKER_BOILERPLATE

# Allow CC to be a program name with arguments.
lt_save_CC="$CC"
CC=${RC-"windres"}
compiler=$CC
_LT_AC_TAGVAR(compiler, $1)=$CC
_LT_CC_BASENAME([$compiler])
_LT_AC_TAGVAR(lt_cv_prog_compiler_c_o, $1)=yes

AC_LIBTOOL_CONFIG($1)

AC_LANG_RESTORE
CC="$lt_save_CC"
])# AC_LIBTOOL_LANG_RC_CONFIG


# AC_LIBTOOL_CONFIG([TAGNAME])
# ----------------------------
# If TAGNAME is not passed, then create an initial libtool script
# with a default configuration from the untagged config vars.  Otherwise
# add code to config.status for appending the configuration named by
# TAGNAME from the matching tagged config vars.
AC_DEFUN([AC_LIBTOOL_CONFIG],
[# The else clause should only fire when bootstrapping the
# libtool distribution, otherwise you forgot to ship ltmain.sh
# with your package, and you will get complaints that there are
# no rules to generate ltmain.sh.
if test -f "$ltmain"; then
  # See if we are running on zsh, and set the options which allow our commands through
  # without removal of \ escapes.
  if test -n "${ZSH_VERSION+set}" ; then
    setopt NO_GLOB_SUBST
  fi
  # Now quote all the things that may contain metacharacters while being
  # careful not to overquote the AC_SUBSTed values.  We take copies of the
  # variables and quote the copies for generation of the libtool script.
  for var in echo old_CC old_CFLAGS AR AR_FLAGS EGREP RANLIB LN_S LTCC LTCFLAGS NM \
    SED SHELL STRIP \
    libname_spec library_names_spec soname_spec extract_expsyms_cmds \
    old_striplib striplib file_magic_cmd finish_cmds finish_eval \
    deplibs_check_method reload_flag reload_cmds need_locks \
    lt_cv_sys_global_symbol_pipe lt_cv_sys_global_symbol_to_cdecl \
    lt_cv_sys_global_symbol_to_c_name_address \
    sys_lib_search_path_spec sys_lib_dlsearch_path_spec \
    old_postinstall_cmds old_postuninstall_cmds \
    _LT_AC_TAGVAR(compiler, $1) \
    _LT_AC_TAGVAR(CC, $1) \
    _LT_AC_TAGVAR(LD, $1) \
    _LT_AC_TAGVAR(lt_prog_compiler_wl, $1) \
    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1) \
    _LT_AC_TAGVAR(lt_prog_compiler_static, $1) \
    _LT_AC_TAGVAR(lt_prog_compiler_no_builtin_flag, $1) \
    _LT_AC_TAGVAR(export_dynamic_flag_spec, $1) \
    _LT_AC_TAGVAR(thread_safe_flag_spec, $1) \
    _LT_AC_TAGVAR(whole_archive_flag_spec, $1) \
    _LT_AC_TAGVAR(enable_shared_with_static_runtimes, $1) \
    _LT_AC_TAGVAR(old_archive_cmds, $1) \
    _LT_AC_TAGVAR(old_archive_from_new_cmds, $1) \
    _LT_AC_TAGVAR(predep_objects, $1) \
    _LT_AC_TAGVAR(postdep_objects, $1) \
    _LT_AC_TAGVAR(predeps, $1) \
    _LT_AC_TAGVAR(postdeps, $1) \
    _LT_AC_TAGVAR(compiler_lib_search_path, $1) \
    _LT_AC_TAGVAR(archive_cmds, $1) \
    _LT_AC_TAGVAR(archive_expsym_cmds, $1) \
    _LT_AC_TAGVAR(postinstall_cmds, $1) \
    _LT_AC_TAGVAR(postuninstall_cmds, $1) \
    _LT_AC_TAGVAR(old_archive_from_expsyms_cmds, $1) \
    _LT_AC_TAGVAR(allow_undefined_flag, $1) \
    _LT_AC_TAGVAR(no_undefined_flag, $1) \
    _LT_AC_TAGVAR(export_symbols_cmds, $1) \
    _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1) \
    _LT_AC_TAGVAR(hardcode_libdir_flag_spec_ld, $1) \
    _LT_AC_TAGVAR(hardcode_libdir_separator, $1) \
    _LT_AC_TAGVAR(hardcode_automatic, $1) \
    _LT_AC_TAGVAR(module_cmds, $1) \
    _LT_AC_TAGVAR(module_expsym_cmds, $1) \
    _LT_AC_TAGVAR(lt_cv_prog_compiler_c_o, $1) \
    _LT_AC_TAGVAR(exclude_expsyms, $1) \
    _LT_AC_TAGVAR(include_expsyms, $1); do

    case $var in
    _LT_AC_TAGVAR(old_archive_cmds, $1) | \
    _LT_AC_TAGVAR(old_archive_from_new_cmds, $1) | \
    _LT_AC_TAGVAR(archive_cmds, $1) | \
    _LT_AC_TAGVAR(archive_expsym_cmds, $1) | \
    _LT_AC_TAGVAR(module_cmds, $1) | \
    _LT_AC_TAGVAR(module_expsym_cmds, $1) | \
    _LT_AC_TAGVAR(old_archive_from_expsyms_cmds, $1) | \
    _LT_AC_TAGVAR(export_symbols_cmds, $1) | \
    extract_expsyms_cmds | reload_cmds | finish_cmds | \
    postinstall_cmds | postuninstall_cmds | \
    old_postinstall_cmds | old_postuninstall_cmds | \
    sys_lib_search_path_spec | sys_lib_dlsearch_path_spec)
      # Double-quote double-evaled strings.
      eval "lt_$var=\\\"\`\$echo \"X\$$var\" | \$Xsed -e \"\$double_quote_subst\" -e \"\$sed_quote_subst\" -e \"\$delay_variable_subst\"\`\\\""
      ;;
    *)
      eval "lt_$var=\\\"\`\$echo \"X\$$var\" | \$Xsed -e \"\$sed_quote_subst\"\`\\\""
      ;;
    esac
  done

  case $lt_echo in
  *'\[$]0 --fallback-echo"')
    lt_echo=`$echo "X$lt_echo" | $Xsed -e 's/\\\\\\\[$]0 --fallback-echo"[$]/[$]0 --fallback-echo"/'`
    ;;
  esac

ifelse([$1], [],
  [cfgfile="${ofile}T"
  trap "$rm \"$cfgfile\"; exit 1" 1 2 15
  $rm -f "$cfgfile"
  AC_MSG_NOTICE([creating $ofile])],
  [cfgfile="$ofile"])

  cat <<__EOF__ >> "$cfgfile"
ifelse([$1], [],
[#! $SHELL

# `$echo "$cfgfile" | sed 's%^.*/%%'` - Provide generalized library-building support services.
# Generated automatically by $PROGRAM (GNU $PACKAGE $VERSION$TIMESTAMP)
# NOTE: Changes made to this file will be lost: look at ltmain.sh.
#
# Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001
# Free Software Foundation, Inc.
#
# This file is part of GNU Libtool:
# Originally by Gordon Matzigkeit <gord@gnu.ai.mit.edu>, 1996
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# As a special exception to the GNU General Public License, if you
# distribute this file as part of a program that contains a
# configuration script generated by Autoconf, you may include it under
# the same distribution terms that you use for the rest of that program.

# A sed program that does not truncate output.
SED=$lt_SED

# Sed that helps us avoid accidentally triggering echo(1) options like -n.
Xsed="$SED -e 1s/^X//"

# The HP-UX ksh and POSIX shell print the target directory to stdout
# if CDPATH is set.
(unset CDPATH) >/dev/null 2>&1 && unset CDPATH

# The names of the tagged configurations supported by this script.
available_tags=

# ### BEGIN LIBTOOL CONFIG],
[# ### BEGIN LIBTOOL TAG CONFIG: $tagname])

# Libtool was configured on host `(hostname || uname -n) 2>/dev/null | sed 1q`:

# Shell to use when invoking shell scripts.
SHELL=$lt_SHELL

# Whether or not to build shared libraries.
build_libtool_libs=$enable_shared

# Whether or not to build static libraries.
build_old_libs=$enable_static

# Whether or not to add -lc for building shared libraries.
build_libtool_need_lc=$_LT_AC_TAGVAR(archive_cmds_need_lc, $1)

# Whether or not to disallow shared libs when runtime libs are static
allow_libtool_libs_with_static_runtimes=$_LT_AC_TAGVAR(enable_shared_with_static_runtimes, $1)

# Whether or not to optimize for fast installation.
fast_install=$enable_fast_install

# The host system.
host_alias=$host_alias
host=$host
host_os=$host_os

# The build system.
build_alias=$build_alias
build=$build
build_os=$build_os

# An echo program that does not interpret backslashes.
echo=$lt_echo

# The archiver.
AR=$lt_AR
AR_FLAGS=$lt_AR_FLAGS

# A C compiler.
LTCC=$lt_LTCC

# LTCC compiler flags.
LTCFLAGS=$lt_LTCFLAGS

# A language-specific compiler.
CC=$lt_[]_LT_AC_TAGVAR(compiler, $1)

# Is the compiler the GNU C compiler?
with_gcc=$_LT_AC_TAGVAR(GCC, $1)

gcc_dir=\`gcc -print-file-name=. | $SED 's,/\.$,,'\`
gcc_ver=\`gcc -dumpversion\`

# An ERE matcher.
EGREP=$lt_EGREP

# The linker used to build libraries.
LD=$lt_[]_LT_AC_TAGVAR(LD, $1)

# Whether we need hard or soft links.
LN_S=$lt_LN_S

# A BSD-compatible nm program.
NM=$lt_NM

# A symbol stripping program
STRIP=$lt_STRIP

# Used to examine libraries when file_magic_cmd begins "file"
MAGIC_CMD=$MAGIC_CMD

# Used on cygwin: DLL creation program.
DLLTOOL="$DLLTOOL"

# Used on cygwin: object dumper.
OBJDUMP="$OBJDUMP"

# Used on cygwin: assembler.
AS="$AS"

# The name of the directory that contains temporary libtool files.
objdir=$objdir

# How to create reloadable object files.
reload_flag=$lt_reload_flag
reload_cmds=$lt_reload_cmds

# How to pass a linker flag through the compiler.
wl=$lt_[]_LT_AC_TAGVAR(lt_prog_compiler_wl, $1)

# Object file suffix (normally "o").
objext="$ac_objext"

# Old archive suffix (normally "a").
libext="$libext"

# Shared library suffix (normally ".so").
shrext_cmds='$shrext_cmds'

# Executable file suffix (normally "").
exeext="$exeext"

# Additional compiler flags for building library objects.
pic_flag=$lt_[]_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)
pic_mode=$pic_mode

# What is the maximum length of a command?
max_cmd_len=$lt_cv_sys_max_cmd_len

# Does compiler simultaneously support -c and -o options?
compiler_c_o=$lt_[]_LT_AC_TAGVAR(lt_cv_prog_compiler_c_o, $1)

# Must we lock files when doing compilation?
need_locks=$lt_need_locks

# Do we need the lib prefix for modules?
need_lib_prefix=$need_lib_prefix

# Do we need a version for libraries?
need_version=$need_version

# Whether dlopen is supported.
dlopen_support=$enable_dlopen

# Whether dlopen of programs is supported.
dlopen_self=$enable_dlopen_self

# Whether dlopen of statically linked programs is supported.
dlopen_self_static=$enable_dlopen_self_static

# Compiler flag to prevent dynamic linking.
link_static_flag=$lt_[]_LT_AC_TAGVAR(lt_prog_compiler_static, $1)

# Compiler flag to turn off builtin functions.
no_builtin_flag=$lt_[]_LT_AC_TAGVAR(lt_prog_compiler_no_builtin_flag, $1)

# Compiler flag to allow reflexive dlopens.
export_dynamic_flag_spec=$lt_[]_LT_AC_TAGVAR(export_dynamic_flag_spec, $1)

# Compiler flag to generate shared objects directly from archives.
whole_archive_flag_spec=$lt_[]_LT_AC_TAGVAR(whole_archive_flag_spec, $1)

# Compiler flag to generate thread-safe objects.
thread_safe_flag_spec=$lt_[]_LT_AC_TAGVAR(thread_safe_flag_spec, $1)

# Library versioning type.
version_type=$version_type

# Format of library name prefix.
libname_spec=$lt_libname_spec

# List of archive names.  First name is the real one, the rest are links.
# The last name is the one that the linker finds with -lNAME.
library_names_spec=$lt_library_names_spec

# The coded name of the library, if different from the real name.
soname_spec=$lt_soname_spec

# Commands used to build and install an old-style archive.
RANLIB=$lt_RANLIB
old_archive_cmds=$lt_[]_LT_AC_TAGVAR(old_archive_cmds, $1)
old_postinstall_cmds=$lt_old_postinstall_cmds
old_postuninstall_cmds=$lt_old_postuninstall_cmds

# Create an old-style archive from a shared archive.
old_archive_from_new_cmds=$lt_[]_LT_AC_TAGVAR(old_archive_from_new_cmds, $1)

# Create a temporary old-style archive to link instead of a shared archive.
old_archive_from_expsyms_cmds=$lt_[]_LT_AC_TAGVAR(old_archive_from_expsyms_cmds, $1)

# Commands used to build and install a shared archive.
archive_cmds=$lt_[]_LT_AC_TAGVAR(archive_cmds, $1)
archive_expsym_cmds=$lt_[]_LT_AC_TAGVAR(archive_expsym_cmds, $1)
postinstall_cmds=$lt_postinstall_cmds
postuninstall_cmds=$lt_postuninstall_cmds

# Commands used to build a loadable module (assumed same as above if empty)
module_cmds=$lt_[]_LT_AC_TAGVAR(module_cmds, $1)
module_expsym_cmds=$lt_[]_LT_AC_TAGVAR(module_expsym_cmds, $1)

# Commands to strip libraries.
old_striplib=$lt_old_striplib
striplib=$lt_striplib

# Dependencies to place before the objects being linked to create a
# shared library.
predep_objects=\`echo $lt_[]_LT_AC_TAGVAR(predep_objects, $1) | \$SED -e "s@\${gcc_dir}@\\\${gcc_dir}@g;s@\${gcc_ver}@\\\${gcc_ver}@g"\`

# Dependencies to place after the objects being linked to create a
# shared library.
postdep_objects=\`echo $lt_[]_LT_AC_TAGVAR(postdep_objects, $1) | \$SED -e "s@\${gcc_dir}@\\\${gcc_dir}@g;s@\${gcc_ver}@\\\${gcc_ver}@g"\`

# Dependencies to place before the objects being linked to create a
# shared library.
predeps=$lt_[]_LT_AC_TAGVAR(predeps, $1)

# Dependencies to place after the objects being linked to create a
# shared library.
postdeps=$lt_[]_LT_AC_TAGVAR(postdeps, $1)

# The library search path used internally by the compiler when linking
# a shared library.
compiler_lib_search_path=\`echo $lt_[]_LT_AC_TAGVAR(compiler_lib_search_path, $1) | \$SED -e "s@\${gcc_dir}@\\\${gcc_dir}@g;s@\${gcc_ver}@\\\${gcc_ver}@g"\`

# Method to check whether dependent libraries are shared objects.
deplibs_check_method=$lt_deplibs_check_method

# Command to use when deplibs_check_method == file_magic.
file_magic_cmd=$lt_file_magic_cmd

# Flag that allows shared libraries with undefined symbols to be built.
allow_undefined_flag=$lt_[]_LT_AC_TAGVAR(allow_undefined_flag, $1)

# Flag that forces no undefined symbols.
no_undefined_flag=$lt_[]_LT_AC_TAGVAR(no_undefined_flag, $1)

# Commands used to finish a libtool library installation in a directory.
finish_cmds=$lt_finish_cmds

# Same as above, but a single script fragment to be evaled but not shown.
finish_eval=$lt_finish_eval

# Take the output of nm and produce a listing of raw symbols and C names.
global_symbol_pipe=$lt_lt_cv_sys_global_symbol_pipe

# Transform the output of nm in a proper C declaration
global_symbol_to_cdecl=$lt_lt_cv_sys_global_symbol_to_cdecl

# Transform the output of nm in a C name address pair
global_symbol_to_c_name_address=$lt_lt_cv_sys_global_symbol_to_c_name_address

# This is the shared library runtime path variable.
runpath_var=$runpath_var

# This is the shared library path variable.
shlibpath_var=$shlibpath_var

# Is shlibpath searched before the hard-coded library search path?
shlibpath_overrides_runpath=$shlibpath_overrides_runpath

# How to hardcode a shared library path into an executable.
hardcode_action=$_LT_AC_TAGVAR(hardcode_action, $1)

# Whether we should hardcode library paths into libraries.
hardcode_into_libs=$hardcode_into_libs

# Flag to hardcode \$libdir into a binary during linking.
# This must work even if \$libdir does not exist.
hardcode_libdir_flag_spec=$lt_[]_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)

# If ld is used when linking, flag to hardcode \$libdir into
# a binary during linking. This must work even if \$libdir does
# not exist.
hardcode_libdir_flag_spec_ld=$lt_[]_LT_AC_TAGVAR(hardcode_libdir_flag_spec_ld, $1)

# Whether we need a single -rpath flag with a separated argument.
hardcode_libdir_separator=$lt_[]_LT_AC_TAGVAR(hardcode_libdir_separator, $1)

# Set to yes if using DIR/libNAME${shared_ext} during linking hardcodes DIR into the
# resulting binary.
hardcode_direct=$_LT_AC_TAGVAR(hardcode_direct, $1)

# Set to yes if using the -LDIR flag during linking hardcodes DIR into the
# resulting binary.
hardcode_minus_L=$_LT_AC_TAGVAR(hardcode_minus_L, $1)

# Set to yes if using SHLIBPATH_VAR=DIR during linking hardcodes DIR into
# the resulting binary.
hardcode_shlibpath_var=$_LT_AC_TAGVAR(hardcode_shlibpath_var, $1)

# Set to yes if building a shared library automatically hardcodes DIR into the library
# and all subsequent libraries and executables linked against it.
hardcode_automatic=$_LT_AC_TAGVAR(hardcode_automatic, $1)

# Variables whose values should be saved in libtool wrapper scripts and
# restored at relink time.
variables_saved_for_relink="$variables_saved_for_relink"

# Whether libtool must link a program against all its dependency libraries.
link_all_deplibs=$_LT_AC_TAGVAR(link_all_deplibs, $1)

# Compile-time system search path for libraries
sys_lib_search_path_spec=\`echo $lt_sys_lib_search_path_spec | \$SED -e "s@\${gcc_dir}@\\\${gcc_dir}@g;s@\${gcc_ver}@\\\${gcc_ver}@g"\`

# Run-time system search path for libraries
sys_lib_dlsearch_path_spec=$lt_sys_lib_dlsearch_path_spec

# Fix the shell variable \$srcfile for the compiler.
fix_srcfile_path="$_LT_AC_TAGVAR(fix_srcfile_path, $1)"

# Set to yes if exported symbols are required.
always_export_symbols=$_LT_AC_TAGVAR(always_export_symbols, $1)

# The commands to list exported symbols.
export_symbols_cmds=$lt_[]_LT_AC_TAGVAR(export_symbols_cmds, $1)

# The commands to extract the exported symbol list from a shared archive.
extract_expsyms_cmds=$lt_extract_expsyms_cmds

# Symbols that should not be listed in the preloaded symbols.
exclude_expsyms=$lt_[]_LT_AC_TAGVAR(exclude_expsyms, $1)

# Symbols that must always be exported.
include_expsyms=$lt_[]_LT_AC_TAGVAR(include_expsyms, $1)

ifelse([$1],[],
[# ### END LIBTOOL CONFIG],
[# ### END LIBTOOL TAG CONFIG: $tagname])

__EOF__

ifelse([$1],[], [
  case $host_os in
  aix3*)
    cat <<\EOF >> "$cfgfile"

# AIX sometimes has problems with the GCC collect2 program.  For some
# reason, if we set the COLLECT_NAMES environment variable, the problems
# vanish in a puff of smoke.
if test "X${COLLECT_NAMES+set}" != Xset; then
  COLLECT_NAMES=
  export COLLECT_NAMES
fi
EOF
    ;;
  esac

  # We use sed instead of cat because bash on DJGPP gets confused if
  # if finds mixed CR/LF and LF-only lines.  Since sed operates in
  # text mode, it properly converts lines to CR/LF.  This bash problem
  # is reportedly fixed, but why not run on old versions too?
  sed '$q' "$ltmain" >> "$cfgfile" || (rm -f "$cfgfile"; exit 1)

  mv -f "$cfgfile" "$ofile" || \
    (rm -f "$ofile" && cp "$cfgfile" "$ofile" && rm -f "$cfgfile")
  chmod +x "$ofile"
])
else
  # If there is no Makefile yet, we rely on a make rule to execute
  # `config.status --recheck' to rerun these tests and create the
  # libtool script then.
  ltmain_in=`echo $ltmain | sed -e 's/\.sh$/.in/'`
  if test -f "$ltmain_in"; then
    test -f Makefile && make "$ltmain"
  fi
fi
])# AC_LIBTOOL_CONFIG


# AC_LIBTOOL_PROG_COMPILER_NO_RTTI([TAGNAME])
# -------------------------------------------
AC_DEFUN([AC_LIBTOOL_PROG_COMPILER_NO_RTTI],
[AC_REQUIRE([_LT_AC_SYS_COMPILER])dnl

_LT_AC_TAGVAR(lt_prog_compiler_no_builtin_flag, $1)=

if test "$GCC" = yes; then
  _LT_AC_TAGVAR(lt_prog_compiler_no_builtin_flag, $1)=' -fno-builtin'

  AC_LIBTOOL_COMPILER_OPTION([if $compiler supports -fno-rtti -fno-exceptions],
    lt_cv_prog_compiler_rtti_exceptions,
    [-fno-rtti -fno-exceptions], [],
    [_LT_AC_TAGVAR(lt_prog_compiler_no_builtin_flag, $1)="$_LT_AC_TAGVAR(lt_prog_compiler_no_builtin_flag, $1) -fno-rtti -fno-exceptions"])
fi
])# AC_LIBTOOL_PROG_COMPILER_NO_RTTI


# AC_LIBTOOL_SYS_GLOBAL_SYMBOL_PIPE
# ---------------------------------
AC_DEFUN([AC_LIBTOOL_SYS_GLOBAL_SYMBOL_PIPE],
[AC_REQUIRE([AC_CANONICAL_HOST])
AC_REQUIRE([AC_PROG_NM])
AC_REQUIRE([AC_OBJEXT])
# Check for command to grab the raw symbol name followed by C symbol from nm.
AC_MSG_CHECKING([command to parse $NM output from $compiler object])
AC_CACHE_VAL([lt_cv_sys_global_symbol_pipe],
[
# These are sane defaults that work on at least a few old systems.
# [They come from Ultrix.  What could be older than Ultrix?!! ;)]

# Character class describing NM global symbol codes.
symcode='[[BCDEGRST]]'

# Regexp to match symbols that can be accessed directly from C.
sympat='\([[_A-Za-z]][[_A-Za-z0-9]]*\)'

# Transform an extracted symbol line into a proper C declaration
lt_cv_sys_global_symbol_to_cdecl="sed -n -e 's/^. .* \(.*\)$/extern int \1;/p'"

# Transform an extracted symbol line into symbol name and symbol address
lt_cv_sys_global_symbol_to_c_name_address="sed -n -e 's/^: \([[^ ]]*\) $/  {\\\"\1\\\", (lt_ptr) 0},/p' -e 's/^$symcode \([[^ ]]*\) \([[^ ]]*\)$/  {\"\2\", (lt_ptr) \&\2},/p'"

# Define system-specific variables.
case $host_os in
aix*)
  symcode='[[BCDT]]'
  ;;
cygwin* | mingw* | pw32*)
  symcode='[[ABCDGISTW]]'
  ;;
hpux*) # Its linker distinguishes data from code symbols
  if test "$host_cpu" = ia64; then
    symcode='[[ABCDEGRST]]'
  fi
  lt_cv_sys_global_symbol_to_cdecl="sed -n -e 's/^T .* \(.*\)$/extern int \1();/p' -e 's/^$symcode* .* \(.*\)$/extern char \1;/p'"
  lt_cv_sys_global_symbol_to_c_name_address="sed -n -e 's/^: \([[^ ]]*\) $/  {\\\"\1\\\", (lt_ptr) 0},/p' -e 's/^$symcode* \([[^ ]]*\) \([[^ ]]*\)$/  {\"\2\", (lt_ptr) \&\2},/p'"
  ;;
linux*)
  if test "$host_cpu" = ia64; then
    symcode='[[ABCDGIRSTW]]'
    lt_cv_sys_global_symbol_to_cdecl="sed -n -e 's/^T .* \(.*\)$/extern int \1();/p' -e 's/^$symcode* .* \(.*\)$/extern char \1;/p'"
    lt_cv_sys_global_symbol_to_c_name_address="sed -n -e 's/^: \([[^ ]]*\) $/  {\\\"\1\\\", (lt_ptr) 0},/p' -e 's/^$symcode* \([[^ ]]*\) \([[^ ]]*\)$/  {\"\2\", (lt_ptr) \&\2},/p'"
  fi
  ;;
irix* | nonstopux*)
  symcode='[[BCDEGRST]]'
  ;;
osf*)
  symcode='[[BCDEGQRST]]'
  ;;
solaris*)
  symcode='[[BDRT]]'
  ;;
sco3.2v5*)
  symcode='[[DT]]'
  ;;
sysv4.2uw2*)
  symcode='[[DT]]'
  ;;
sysv5* | sco5v6* | unixware* | OpenUNIX*)
  symcode='[[ABDT]]'
  ;;
sysv4)
  symcode='[[DFNSTU]]'
  ;;
esac

# Handle CRLF in mingw tool chain
opt_cr=
case $build_os in
mingw*)
  opt_cr=`echo 'x\{0,1\}' | tr x '\015'` # option cr in regexp
  ;;
esac

# If we're using GNU nm, then use its standard symbol codes.
case `$NM -V 2>&1` in
*GNU* | *'with BFD'*)
  symcode='[[ABCDGIRSTW]]' ;;
esac

# Try without a prefix undercore, then with it.
for ac_symprfx in "" "_"; do

  # Transform symcode, sympat, and symprfx into a raw symbol and a C symbol.
  symxfrm="\\1 $ac_symprfx\\2 \\2"

  # Write the raw and C identifiers.
  lt_cv_sys_global_symbol_pipe="sed -n -e 's/^.*[[ 	]]\($symcode$symcode*\)[[ 	]][[ 	]]*$ac_symprfx$sympat$opt_cr$/$symxfrm/p'"

  # Check to see that the pipe works correctly.
  pipe_works=no

  rm -f conftest*
  cat > conftest.$ac_ext <<EOF
#ifdef __cplusplus
extern "C" {
#endif
char nm_test_var;
void nm_test_func(){}
#ifdef __cplusplus
}
#endif
int main(){nm_test_var='a';nm_test_func();return(0);}
EOF

  if AC_TRY_EVAL(ac_compile); then
    # Now try to grab the symbols.
    nlist=conftest.nm
    if AC_TRY_EVAL(NM conftest.$ac_objext \| $lt_cv_sys_global_symbol_pipe \> $nlist) && test -s "$nlist"; then
      # Try sorting and uniquifying the output.
      if sort "$nlist" | uniq > "$nlist"T; then
	mv -f "$nlist"T "$nlist"
      else
	rm -f "$nlist"T
      fi

      # Make sure that we snagged all the symbols we need.
      if grep ' nm_test_var$' "$nlist" >/dev/null; then
	if grep ' nm_test_func$' "$nlist" >/dev/null; then
	  cat <<EOF > conftest.$ac_ext
#ifdef __cplusplus
extern "C" {
#endif

EOF
	  # Now generate the symbol file.
	  eval "$lt_cv_sys_global_symbol_to_cdecl"' < "$nlist" | grep -v main >> conftest.$ac_ext'

	  cat <<EOF >> conftest.$ac_ext
#if defined (__STDC__) && __STDC__
# define lt_ptr_t void *
#else
# define lt_ptr_t char *
# define const
#endif

/* The mapping between symbol names and symbols. */
const struct {
  const char *name;
  lt_ptr_t address;
}
lt_preloaded_symbols[[]] =
{
EOF
	  $SED "s/^$symcode$symcode* \(.*\) \(.*\)$/  {\"\2\", (lt_ptr_t) \&\2},/" < "$nlist" | grep -v main >> conftest.$ac_ext
	  cat <<\EOF >> conftest.$ac_ext
  {0, (lt_ptr_t) 0}
};

#ifdef __cplusplus
}
#endif
EOF
	  # Now try linking the two files.
	  mv conftest.$ac_objext conftstm.$ac_objext
	  lt_save_LIBS="$LIBS"
	  lt_save_CFLAGS="$CFLAGS"
	  LIBS="conftstm.$ac_objext"
	  CFLAGS="$CFLAGS$_LT_AC_TAGVAR(lt_prog_compiler_no_builtin_flag, $1)"
	  if AC_TRY_EVAL(ac_link) && test -s conftest${ac_exeext}; then
	    pipe_works=yes
	  fi
	  LIBS="$lt_save_LIBS"
	  CFLAGS="$lt_save_CFLAGS"
	else
	  echo "cannot find nm_test_func in $nlist" >&AS_MESSAGE_LOG_FD
	fi
      else
	echo "cannot find nm_test_var in $nlist" >&AS_MESSAGE_LOG_FD
      fi
    else
      echo "cannot run $lt_cv_sys_global_symbol_pipe" >&AS_MESSAGE_LOG_FD
    fi
  else
    echo "$progname: failed program was:" >&AS_MESSAGE_LOG_FD
    cat conftest.$ac_ext >&5
  fi
  rm -f conftest* conftst*

  # Do not use the global_symbol_pipe unless it works.
  if test "$pipe_works" = yes; then
    break
  else
    lt_cv_sys_global_symbol_pipe=
  fi
done
])
if test -z "$lt_cv_sys_global_symbol_pipe"; then
  lt_cv_sys_global_symbol_to_cdecl=
fi
if test -z "$lt_cv_sys_global_symbol_pipe$lt_cv_sys_global_symbol_to_cdecl"; then
  AC_MSG_RESULT(failed)
else
  AC_MSG_RESULT(ok)
fi
]) # AC_LIBTOOL_SYS_GLOBAL_SYMBOL_PIPE


# AC_LIBTOOL_PROG_COMPILER_PIC([TAGNAME])
# ---------------------------------------
AC_DEFUN([AC_LIBTOOL_PROG_COMPILER_PIC],
[_LT_AC_TAGVAR(lt_prog_compiler_wl, $1)=
_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)=
_LT_AC_TAGVAR(lt_prog_compiler_static, $1)=

AC_MSG_CHECKING([for $compiler option to produce PIC])
 ifelse([$1],[CXX],[
  # C++ specific cases for pic, static, wl, etc.
  if test "$GXX" = yes; then
    _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
    _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-static'

    case $host_os in
    aix*)
      # All AIX code is PIC.
      if test "$host_cpu" = ia64; then
	# AIX 5 now supports IA64 processor
	_LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
      fi
      ;;
    amigaos*)
      # FIXME: we need at least 68020 code to build shared libraries, but
      # adding the `-m68020' flag to GCC prevents building anything better,
      # like `-m68040'.
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-m68020 -resident32 -malways-restore-a4'
      ;;
    beos* | cygwin* | irix5* | irix6* | nonstopux* | osf3* | osf4* | osf5*)
      # PIC is the default for these OSes.
      ;;
    mingw* | os2* | pw32*)
      # This hack is so that the source file can tell whether it is being
      # built for inclusion in a dll (and should export symbols for example).
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-DDLL_EXPORT'
      ;;
    darwin* | rhapsody*)
      # PIC is the default on this platform
      # Common symbols not allowed in MH_DYLIB files
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-fno-common'
      ;;
    *djgpp*)
      # DJGPP does not support shared libraries at all
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)=
      ;;
    interix3*)
      # Interix 3.x gcc -fpic/-fPIC options generate broken code.
      # Instead, we relocate shared libraries at runtime.
      ;;
    sysv4*MP*)
      if test -d /usr/nec; then
	_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)=-Kconform_pic
      fi
      ;;
    hpux*)
      # PIC is the default for IA64 HP-UX and 64-bit HP-UX, but
      # not for PA HP-UX.
      case $host_cpu in
      hppa*64*|ia64*)
	;;
      *)
	_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-fPIC'
	;;
      esac
      ;;
    *)
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-fPIC'
      ;;
    esac
  else
    case $host_os in
      aix4* | aix5*)
	# All AIX code is PIC.
	if test "$host_cpu" = ia64; then
	  # AIX 5 now supports IA64 processor
	  _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
	else
	  _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-bnso -bI:/lib/syscalls.exp'
	fi
	;;
      chorus*)
	case $cc_basename in
	cxch68*)
	  # Green Hills C++ Compiler
	  # _LT_AC_TAGVAR(lt_prog_compiler_static, $1)="--no_auto_instantiation -u __main -u __premain -u _abort -r $COOL_DIR/lib/libOrb.a $MVME_DIR/lib/CC/libC.a $MVME_DIR/lib/classix/libcx.s.a"
	  ;;
	esac
	;;
       darwin*)
         # PIC is the default on this platform
         # Common symbols not allowed in MH_DYLIB files
         case $cc_basename in
           xlc*)
           _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-qnocommon'
           _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
           ;;
         esac
       ;;
      dgux*)
	case $cc_basename in
	  ec++*)
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-KPIC'
	    ;;
	  ghcx*)
	    # Green Hills C++ Compiler
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-pic'
	    ;;
	  *)
	    ;;
	esac
	;;
      freebsd* | kfreebsd*-gnu | dragonfly*)
	# FreeBSD uses GNU C++
	;;
      hpux9* | hpux10* | hpux11*)
	case $cc_basename in
	  CC*)
	    _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
	    _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='${wl}-a ${wl}archive'
	    if test "$host_cpu" != ia64; then
	      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='+Z'
	    fi
	    ;;
	  aCC*)
	    _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
	    _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='${wl}-a ${wl}archive'
	    case $host_cpu in
	    hppa*64*|ia64*)
	      # +Z the default
	      ;;
	    *)
	      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='+Z'
	      ;;
	    esac
	    ;;
	  *)
	    ;;
	esac
	;;
      interix*)
	# This is c89, which is MS Visual C++ (no shared libs)
	# Anyone wants to do a port?
	;;
      irix5* | irix6* | nonstopux*)
	case $cc_basename in
	  CC*)
	    _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
	    _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-non_shared'
	    # CC pic flag -KPIC is the default.
	    ;;
	  *)
	    ;;
	esac
	;;
      linux*)
	case $cc_basename in
	  KCC*)
	    # KAI C++ Compiler
	    _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='--backend -Wl,'
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-fPIC'
	    ;;
	  icpc* | ecpc*)
	    # Intel C++
	    _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-KPIC'
	    _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-static'
	    ;;
	  pgCC*)
	    # Portland Group C++ compiler.
	    _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-fpic'
	    _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
	    ;;
	  cxx*)
	    # Compaq C++
	    # Make sure the PIC flag is empty.  It appears that all Alpha
	    # Linux and Compaq Tru64 Unix objects are PIC.
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)=
	    _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-non_shared'
	    ;;
	  *)
	    ;;
	esac
	;;
      lynxos*)
	;;
      m88k*)
	;;
      mvs*)
	case $cc_basename in
	  cxx*)
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-W c,exportall'
	    ;;
	  *)
	    ;;
	esac
	;;
      netbsd*)
	;;
      osf3* | osf4* | osf5*)
	case $cc_basename in
	  KCC*)
	    _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='--backend -Wl,'
	    ;;
	  RCC*)
	    # Rational C++ 2.4.1
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-pic'
	    ;;
	  cxx*)
	    # Digital/Compaq C++
	    _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
	    # Make sure the PIC flag is empty.  It appears that all Alpha
	    # Linux and Compaq Tru64 Unix objects are PIC.
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)=
	    _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-non_shared'
	    ;;
	  *)
	    ;;
	esac
	;;
      psos*)
	;;
      solaris*)
	case $cc_basename in
	  CC*)
	    # Sun C++ 4.2, 5.x and Centerline C++
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-KPIC'
	    _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
	    _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Qoption ld '
	    ;;
	  gcx*)
	    # Green Hills C++ Compiler
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-PIC'
	    ;;
	  *)
	    ;;
	esac
	;;
      sunos4*)
	case $cc_basename in
	  CC*)
	    # Sun C++ 4.x
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-pic'
	    _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
	    ;;
	  lcc*)
	    # Lucid
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-pic'
	    ;;
	  *)
	    ;;
	esac
	;;
      tandem*)
	case $cc_basename in
	  NCC*)
	    # NonStop-UX NCC 3.20
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-KPIC'
	    ;;
	  *)
	    ;;
	esac
	;;
      sysv5* | unixware* | sco3.2v5* | sco5v6* | OpenUNIX*)
	case $cc_basename in
	  CC*)
	    _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
	    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-KPIC'
	    _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
	    ;;
	esac
	;;
      vxworks*)
	;;
      *)
	_LT_AC_TAGVAR(lt_prog_compiler_can_build_shared, $1)=no
	;;
    esac
  fi
],
[
  if test "$GCC" = yes; then
    _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
    _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-static'

    case $host_os in
      aix*)
      # All AIX code is PIC.
      if test "$host_cpu" = ia64; then
	# AIX 5 now supports IA64 processor
	_LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
      fi
      ;;

    amigaos*)
      # FIXME: we need at least 68020 code to build shared libraries, but
      # adding the `-m68020' flag to GCC prevents building anything better,
      # like `-m68040'.
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-m68020 -resident32 -malways-restore-a4'
      ;;

    beos* | cygwin* | irix5* | irix6* | nonstopux* | osf3* | osf4* | osf5*)
      # PIC is the default for these OSes.
      ;;

    mingw* | pw32* | os2*)
      # This hack is so that the source file can tell whether it is being
      # built for inclusion in a dll (and should export symbols for example).
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-DDLL_EXPORT'
      ;;

    darwin* | rhapsody*)
      # PIC is the default on this platform
      # Common symbols not allowed in MH_DYLIB files
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-fno-common'
      ;;

    interix3*)
      # Interix 3.x gcc -fpic/-fPIC options generate broken code.
      # Instead, we relocate shared libraries at runtime.
      ;;

    msdosdjgpp*)
      # Just because we use GCC doesn't mean we suddenly get shared libraries
      # on systems that don't support them.
      _LT_AC_TAGVAR(lt_prog_compiler_can_build_shared, $1)=no
      enable_shared=no
      ;;

    sysv4*MP*)
      if test -d /usr/nec; then
	_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)=-Kconform_pic
      fi
      ;;

    hpux*)
      # PIC is the default for IA64 HP-UX and 64-bit HP-UX, but
      # not for PA HP-UX.
      case $host_cpu in
      hppa*64*|ia64*)
	# +Z the default
	;;
      *)
	_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-fPIC'
	;;
      esac
      ;;

    *)
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-fPIC'
      ;;
    esac
  else
    # PORTME Check for flag to pass linker flags through the system compiler.
    case $host_os in
    aix*)
      _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
      if test "$host_cpu" = ia64; then
	# AIX 5 now supports IA64 processor
	_LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
      else
	_LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-bnso -bI:/lib/syscalls.exp'
      fi
      ;;
      darwin*)
        # PIC is the default on this platform
        # Common symbols not allowed in MH_DYLIB files
       case $cc_basename in
         xlc*)
         _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-qnocommon'
         _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
         ;;
       esac
       ;;

    mingw* | pw32* | os2*)
      # This hack is so that the source file can tell whether it is being
      # built for inclusion in a dll (and should export symbols for example).
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-DDLL_EXPORT'
      ;;

    hpux9* | hpux10* | hpux11*)
      _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
      # PIC is the default for IA64 HP-UX and 64-bit HP-UX, but
      # not for PA HP-UX.
      case $host_cpu in
      hppa*64*|ia64*)
	# +Z the default
	;;
      *)
	_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='+Z'
	;;
      esac
      # Is there a better lt_prog_compiler_static that works with the bundled CC?
      _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='${wl}-a ${wl}archive'
      ;;

    irix5* | irix6* | nonstopux*)
      _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
      # PIC (with -KPIC) is the default.
      _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-non_shared'
      ;;

    newsos6)
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-KPIC'
      _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
      ;;

    linux*)
      case $cc_basename in
      icc* | ecc*)
	_LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
	_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-KPIC'
	_LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-static'
        ;;
      pgcc* | pgf77* | pgf90* | pgf95*)
        # Portland Group compilers (*not* the Pentium gcc compiler,
	# which looks to be a dead project)
	_LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
	_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-fpic'
	_LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
        ;;
      ccc*)
        _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
        # All Alpha code is PIC.
        _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-non_shared'
        ;;
      esac
      ;;

    osf3* | osf4* | osf5*)
      _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
      # All OSF/1 code is PIC.
      _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-non_shared'
      ;;

    solaris*)
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-KPIC'
      _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
      case $cc_basename in
      f77* | f90* | f95*)
	_LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Qoption ld ';;
      *)
	_LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,';;
      esac
      ;;

    sunos4*)
      _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Qoption ld '
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-PIC'
      _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
      ;;

    sysv4 | sysv4.2uw2* | sysv4.3*)
      _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-KPIC'
      _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
      ;;

    sysv4*MP*)
      if test -d /usr/nec ;then
	_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-Kconform_pic'
	_LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
      fi
      ;;

    sysv5* | unixware* | sco3.2v5* | sco5v6* | OpenUNIX*)
      _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-KPIC'
      _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
      ;;

    unicos*)
      _LT_AC_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
      _LT_AC_TAGVAR(lt_prog_compiler_can_build_shared, $1)=no
      ;;

    uts4*)
      _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)='-pic'
      _LT_AC_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
      ;;

    *)
      _LT_AC_TAGVAR(lt_prog_compiler_can_build_shared, $1)=no
      ;;
    esac
  fi
])
AC_MSG_RESULT([$_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)])

#
# Check to make sure the PIC flag actually works.
#
if test -n "$_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)"; then
  AC_LIBTOOL_COMPILER_OPTION([if $compiler PIC flag $_LT_AC_TAGVAR(lt_prog_compiler_pic, $1) works],
    _LT_AC_TAGVAR(lt_prog_compiler_pic_works, $1),
    [$_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)ifelse([$1],[],[ -DPIC],[ifelse([$1],[CXX],[ -DPIC],[])])], [],
    [case $_LT_AC_TAGVAR(lt_prog_compiler_pic, $1) in
     "" | " "*) ;;
     *) _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)=" $_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)" ;;
     esac],
    [_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)=
     _LT_AC_TAGVAR(lt_prog_compiler_can_build_shared, $1)=no])
fi
case $host_os in
  # For platforms which do not support PIC, -DPIC is meaningless:
  *djgpp*)
    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)=
    ;;
  *)
    _LT_AC_TAGVAR(lt_prog_compiler_pic, $1)="$_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)ifelse([$1],[],[ -DPIC],[ifelse([$1],[CXX],[ -DPIC],[])])"
    ;;
esac

#
# Check to make sure the static flag actually works.
#
wl=$_LT_AC_TAGVAR(lt_prog_compiler_wl, $1) eval lt_tmp_static_flag=\"$_LT_AC_TAGVAR(lt_prog_compiler_static, $1)\"
AC_LIBTOOL_LINKER_OPTION([if $compiler static flag $lt_tmp_static_flag works],
  _LT_AC_TAGVAR(lt_prog_compiler_static_works, $1),
  $lt_tmp_static_flag,
  [],
  [_LT_AC_TAGVAR(lt_prog_compiler_static, $1)=])
])


# AC_LIBTOOL_PROG_LD_SHLIBS([TAGNAME])
# ------------------------------------
# See if the linker supports building shared libraries.
AC_DEFUN([AC_LIBTOOL_PROG_LD_SHLIBS],
[AC_MSG_CHECKING([whether the $compiler linker ($LD) supports shared libraries])
ifelse([$1],[CXX],[
  _LT_AC_TAGVAR(export_symbols_cmds, $1)='$NM $libobjs $convenience | $global_symbol_pipe | $SED '\''s/.* //'\'' | sort | uniq > $export_symbols'
  case $host_os in
  aix4* | aix5*)
    # If we're using GNU nm, then we don't want the "-C" option.
    # -C means demangle to AIX nm, but means don't demangle with GNU nm
    if $NM -V 2>&1 | grep 'GNU' > /dev/null; then
      _LT_AC_TAGVAR(export_symbols_cmds, $1)='$NM -Bpg $libobjs $convenience | awk '\''{ if (((\[$]2 == "T") || (\[$]2 == "D") || (\[$]2 == "B")) && ([substr](\[$]3,1,1) != ".")) { print \[$]3 } }'\'' | sort -u > $export_symbols'
    else
      _LT_AC_TAGVAR(export_symbols_cmds, $1)='$NM -BCpg $libobjs $convenience | awk '\''{ if (((\[$]2 == "T") || (\[$]2 == "D") || (\[$]2 == "B")) && ([substr](\[$]3,1,1) != ".")) { print \[$]3 } }'\'' | sort -u > $export_symbols'
    fi
    ;;
  pw32*)
    _LT_AC_TAGVAR(export_symbols_cmds, $1)="$ltdll_cmds"
  ;;
  cygwin* | mingw*)
    _LT_AC_TAGVAR(export_symbols_cmds, $1)='$NM $libobjs $convenience | $global_symbol_pipe | $SED -e '\''/^[[BCDGRS]] /s/.* \([[^ ]]*\)/\1 DATA/;/^.* __nm__/s/^.* __nm__\([[^ ]]*\) [[^ ]]*/\1 DATA/;/^I /d;/^[[AITW]] /s/.* //'\'' | sort | uniq > $export_symbols'
  ;;
  *)
    _LT_AC_TAGVAR(export_symbols_cmds, $1)='$NM $libobjs $convenience | $global_symbol_pipe | $SED '\''s/.* //'\'' | sort | uniq > $export_symbols'
  ;;
  esac
],[
  runpath_var=
  _LT_AC_TAGVAR(allow_undefined_flag, $1)=
  _LT_AC_TAGVAR(enable_shared_with_static_runtimes, $1)=no
  _LT_AC_TAGVAR(archive_cmds, $1)=
  _LT_AC_TAGVAR(archive_expsym_cmds, $1)=
  _LT_AC_TAGVAR(old_archive_From_new_cmds, $1)=
  _LT_AC_TAGVAR(old_archive_from_expsyms_cmds, $1)=
  _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)=
  _LT_AC_TAGVAR(whole_archive_flag_spec, $1)=
  _LT_AC_TAGVAR(thread_safe_flag_spec, $1)=
  _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)=
  _LT_AC_TAGVAR(hardcode_libdir_flag_spec_ld, $1)=
  _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=
  _LT_AC_TAGVAR(hardcode_direct, $1)=no
  _LT_AC_TAGVAR(hardcode_minus_L, $1)=no
  _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=unsupported
  _LT_AC_TAGVAR(link_all_deplibs, $1)=unknown
  _LT_AC_TAGVAR(hardcode_automatic, $1)=no
  _LT_AC_TAGVAR(module_cmds, $1)=
  _LT_AC_TAGVAR(module_expsym_cmds, $1)=
  _LT_AC_TAGVAR(always_export_symbols, $1)=no
  _LT_AC_TAGVAR(export_symbols_cmds, $1)='$NM $libobjs $convenience | $global_symbol_pipe | $SED '\''s/.* //'\'' | sort | uniq > $export_symbols'
  # include_expsyms should be a list of space-separated symbols to be *always*
  # included in the symbol list
  _LT_AC_TAGVAR(include_expsyms, $1)=
  # exclude_expsyms can be an extended regexp of symbols to exclude
  # it will be wrapped by ` (' and `)$', so one must not match beginning or
  # end of line.  Example: `a|bc|.*d.*' will exclude the symbols `a' and `bc',
  # as well as any symbol that contains `d'.
  _LT_AC_TAGVAR(exclude_expsyms, $1)="_GLOBAL_OFFSET_TABLE_"
  # Although _GLOBAL_OFFSET_TABLE_ is a valid symbol C name, most a.out
  # platforms (ab)use it in PIC code, but their linkers get confused if
  # the symbol is explicitly referenced.  Since portable code cannot
  # rely on this symbol name, it's probably fine to never include it in
  # preloaded symbol tables.
  extract_expsyms_cmds=
  # Just being paranoid about ensuring that cc_basename is set.
  _LT_CC_BASENAME([$compiler])
  case $host_os in
  cygwin* | mingw* | pw32*)
    # FIXME: the MSVC++ port hasn't been tested in a loooong time
    # When not using gcc, we currently assume that we are using
    # Microsoft Visual C++.
    if test "$GCC" != yes; then
      with_gnu_ld=no
    fi
    ;;
  interix*)
    # we just hope/assume this is gcc and not c89 (= MSVC++)
    with_gnu_ld=yes
    ;;
  openbsd*)
    with_gnu_ld=no
    ;;
  esac

  _LT_AC_TAGVAR(ld_shlibs, $1)=yes
  if test "$with_gnu_ld" = yes; then
    # If archive_cmds runs LD, not CC, wlarc should be empty
    wlarc='${wl}'

    # Set some defaults for GNU ld with shared library support. These
    # are reset later if shared libraries are not supported. Putting them
    # here allows them to be overridden if necessary.
    runpath_var=LD_RUN_PATH
    _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}--rpath ${wl}$libdir'
    _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}--export-dynamic'
    # ancient GNU ld didn't support --whole-archive et. al.
    if $LD --help 2>&1 | grep 'no-whole-archive' > /dev/null; then
	_LT_AC_TAGVAR(whole_archive_flag_spec, $1)="$wlarc"'--whole-archive$convenience '"$wlarc"'--no-whole-archive'
      else
  	_LT_AC_TAGVAR(whole_archive_flag_spec, $1)=
    fi
    supports_anon_versioning=no
    case `$LD -v 2>/dev/null` in
      *\ [[01]].* | *\ 2.[[0-9]].* | *\ 2.10.*) ;; # catch versions < 2.11
      *\ 2.11.93.0.2\ *) supports_anon_versioning=yes ;; # RH7.3 ...
      *\ 2.11.92.0.12\ *) supports_anon_versioning=yes ;; # Mandrake 8.2 ...
      *\ 2.11.*) ;; # other 2.11 versions
      *) supports_anon_versioning=yes ;;
    esac

    # See if GNU ld supports shared libraries.
    case $host_os in
    aix3* | aix4* | aix5*)
      # On AIX/PPC, the GNU linker is very broken
      if test "$host_cpu" != ia64; then
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	cat <<EOF 1>&2

*** Warning: the GNU linker, at least up to release 2.9.1, is reported
*** to be unable to reliably create shared libraries on AIX.
*** Therefore, libtool is disabling shared libraries support.  If you
*** really care for shared libraries, you may want to modify your PATH
*** so that a non-GNU linker is found, and then restart.

EOF
      fi
      ;;

    amigaos*)
      _LT_AC_TAGVAR(archive_cmds, $1)='$rm $output_objdir/a2ixlibrary.data~$echo "#define NAME $libname" > $output_objdir/a2ixlibrary.data~$echo "#define LIBRARY_ID 1" >> $output_objdir/a2ixlibrary.data~$echo "#define VERSION $major" >> $output_objdir/a2ixlibrary.data~$echo "#define REVISION $revision" >> $output_objdir/a2ixlibrary.data~$AR $AR_FLAGS $lib $libobjs~$RANLIB $lib~(cd $output_objdir && a2ixlibrary -32)'
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-L$libdir'
      _LT_AC_TAGVAR(hardcode_minus_L, $1)=yes

      # Samuel A. Falvo II <kc5tja@dolphin.openprojects.net> reports
      # that the semantics of dynamic libraries on AmigaOS, at least up
      # to version 4, is to share data among multiple programs linked
      # with the same dynamic library.  Since this doesn't match the
      # behavior of shared libraries on other platforms, we can't use
      # them.
      _LT_AC_TAGVAR(ld_shlibs, $1)=no
      ;;

    beos*)
      if $LD --help 2>&1 | grep ': supported targets:.* elf' > /dev/null; then
	_LT_AC_TAGVAR(allow_undefined_flag, $1)=unsupported
	# Joseph Beckenbach <jrb3@best.com> says some releases of gcc
	# support --undefined.  This deserves some investigation.  FIXME
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -nostart $libobjs $deplibs $compiler_flags ${wl}-soname $wl$soname -o $lib'
      else
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
      fi
      ;;

    cygwin* | mingw* | pw32*)
      # _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1) is actually meaningless,
      # as there is no search path for DLLs.
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-L$libdir'
      _LT_AC_TAGVAR(allow_undefined_flag, $1)=unsupported
      _LT_AC_TAGVAR(always_export_symbols, $1)=no
      _LT_AC_TAGVAR(enable_shared_with_static_runtimes, $1)=yes
      _LT_AC_TAGVAR(export_symbols_cmds, $1)='$NM $libobjs $convenience | $global_symbol_pipe | $SED -e '\''/^[[BCDGRS]] /s/.* \([[^ ]]*\)/\1 DATA/'\'' | $SED -e '\''/^[[AITW]] /s/.* //'\'' | sort | uniq > $export_symbols'

      if $LD --help 2>&1 | grep 'auto-import' > /dev/null; then
        _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $libobjs $deplibs $compiler_flags -o $output_objdir/$soname ${wl}--enable-auto-image-base -Xlinker --out-implib -Xlinker $lib'
	# If the export-symbols file already is a .def file (1st line
	# is EXPORTS), use it as is; otherwise, prepend...
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='if test "x`$SED 1q $export_symbols`" = xEXPORTS; then
	  cp $export_symbols $output_objdir/$soname.def;
	else
	  echo EXPORTS > $output_objdir/$soname.def;
	  cat $export_symbols >> $output_objdir/$soname.def;
	fi~
	$CC -shared $output_objdir/$soname.def $libobjs $deplibs $compiler_flags -o $output_objdir/$soname ${wl}--enable-auto-image-base -Xlinker --out-implib -Xlinker $lib'
      else
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
      fi
      ;;

    interix3*)
      _LT_AC_TAGVAR(hardcode_direct, $1)=no
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath,$libdir'
      _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}-E'
      # Hack: On Interix 3.x, we cannot compile PIC because of a broken gcc.
      # Instead, shared libraries are loaded at an image base (0x10000000 by
      # default) and relocated if they conflict, which is a slow very memory
      # consuming and fragmenting process.  To avoid this, we pick a random,
      # 256 KiB-aligned image base between 0x50000000 and 0x6FFC0000 at link
      # time.  Moving up from 0x10000000 also allows more sbrk(2) space.
      _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $pic_flag $libobjs $deplibs $compiler_flags ${wl}-h,$soname ${wl}--image-base,`expr ${RANDOM-$$} % 4096 / 2 \* 262144 + 1342177280` -o $lib'
      _LT_AC_TAGVAR(archive_expsym_cmds, $1)='sed "s,^,_," $export_symbols >$output_objdir/$soname.expsym~$CC -shared $pic_flag $libobjs $deplibs $compiler_flags ${wl}-h,$soname ${wl}--retain-symbols-file,$output_objdir/$soname.expsym ${wl}--image-base,`expr ${RANDOM-$$} % 4096 / 2 \* 262144 + 1342177280` -o $lib'
      ;;

    linux*)
      if $LD --help 2>&1 | grep ': supported targets:.* elf' > /dev/null; then
	tmp_addflag=
	case $cc_basename,$host_cpu in
	pgcc*)				# Portland Group C compiler
	  _LT_AC_TAGVAR(whole_archive_flag_spec, $1)='${wl}--whole-archive`for conv in $convenience\"\"; do test  -n \"$conv\" && new_convenience=\"$new_convenience,$conv\"; done; $echo \"$new_convenience\"` ${wl}--no-whole-archive'
	  tmp_addflag=' $pic_flag'
	  ;;
	pgf77* | pgf90* | pgf95*)	# Portland Group f77 and f90 compilers
	  _LT_AC_TAGVAR(whole_archive_flag_spec, $1)='${wl}--whole-archive`for conv in $convenience\"\"; do test  -n \"$conv\" && new_convenience=\"$new_convenience,$conv\"; done; $echo \"$new_convenience\"` ${wl}--no-whole-archive'
	  tmp_addflag=' $pic_flag -Mnomain' ;;
	ecc*,ia64* | icc*,ia64*)		# Intel C compiler on ia64
	  tmp_addflag=' -i_dynamic' ;;
	efc*,ia64* | ifort*,ia64*)	# Intel Fortran compiler on ia64
	  tmp_addflag=' -i_dynamic -nofor_main' ;;
	ifc* | ifort*)			# Intel Fortran compiler
	  tmp_addflag=' -nofor_main' ;;
	esac
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared'"$tmp_addflag"' $libobjs $deplibs $compiler_flags ${wl}-soname $wl$soname -o $lib'

	if test $supports_anon_versioning = yes; then
	  _LT_AC_TAGVAR(archive_expsym_cmds, $1)='$echo "{ global:" > $output_objdir/$libname.ver~
  cat $export_symbols | sed -e "s/\(.*\)/\1;/" >> $output_objdir/$libname.ver~
  $echo "local: *; };" >> $output_objdir/$libname.ver~
	  $CC -shared'"$tmp_addflag"' $libobjs $deplibs $compiler_flags ${wl}-soname $wl$soname ${wl}-version-script ${wl}$output_objdir/$libname.ver -o $lib'
	fi
      else
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
      fi
      ;;

    netbsd*)
      if echo __ELF__ | $CC -E - | grep __ELF__ >/dev/null; then
	_LT_AC_TAGVAR(archive_cmds, $1)='$LD -Bshareable $libobjs $deplibs $linker_flags -o $lib'
	wlarc=
      else
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $libobjs $deplibs $compiler_flags ${wl}-soname $wl$soname -o $lib'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared $libobjs $deplibs $compiler_flags ${wl}-soname $wl$soname ${wl}-retain-symbols-file $wl$export_symbols -o $lib'
      fi
      ;;

    solaris*)
      if $LD -v 2>&1 | grep 'BFD 2\.8' > /dev/null; then
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	cat <<EOF 1>&2

*** Warning: The releases 2.8.* of the GNU linker cannot reliably
*** create shared libraries on Solaris systems.  Therefore, libtool
*** is disabling shared libraries support.  We urge you to upgrade GNU
*** binutils to release 2.9.1 or newer.  Another option is to modify
*** your PATH or compiler configuration so that the native linker is
*** used, and then restart.

EOF
      elif $LD --help 2>&1 | grep ': supported targets:.* elf' > /dev/null; then
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $libobjs $deplibs $compiler_flags ${wl}-soname $wl$soname -o $lib'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared $libobjs $deplibs $compiler_flags ${wl}-soname $wl$soname ${wl}-retain-symbols-file $wl$export_symbols -o $lib'
      else
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
      fi
      ;;

    sysv5* | sco3.2v5* | sco5v6* | unixware* | OpenUNIX*)
      case `$LD -v 2>&1` in
        *\ [[01]].* | *\ 2.[[0-9]].* | *\ 2.1[[0-5]].*) 
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
	cat <<_LT_EOF 1>&2

*** Warning: Releases of the GNU linker prior to 2.16.91.0.3 can not
*** reliably create shared libraries on SCO systems.  Therefore, libtool
*** is disabling shared libraries support.  We urge you to upgrade GNU
*** binutils to release 2.16.91.0.3 or newer.  Another option is to modify
*** your PATH or compiler configuration so that the native linker is
*** used, and then restart.

_LT_EOF
	;;
	*)
	  if $LD --help 2>&1 | grep ': supported targets:.* elf' > /dev/null; then
	    _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='`test -z "$SCOABSPATH" && echo ${wl}-rpath,$libdir`'
	    _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $libobjs $deplibs $compiler_flags ${wl}-soname,\${SCOABSPATH:+${install_libdir}/}$soname -o $lib'
	    _LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared $libobjs $deplibs $compiler_flags ${wl}-soname,\${SCOABSPATH:+${install_libdir}/}$soname,-retain-symbols-file,$export_symbols -o $lib'
	  else
	    _LT_AC_TAGVAR(ld_shlibs, $1)=no
	  fi
	;;
      esac
      ;;

    sunos4*)
      _LT_AC_TAGVAR(archive_cmds, $1)='$LD -assert pure-text -Bshareable -o $lib $libobjs $deplibs $linker_flags'
      wlarc=
      _LT_AC_TAGVAR(hardcode_direct, $1)=yes
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      ;;

    *)
      if $LD --help 2>&1 | grep ': supported targets:.* elf' > /dev/null; then
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $libobjs $deplibs $compiler_flags ${wl}-soname $wl$soname -o $lib'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared $libobjs $deplibs $compiler_flags ${wl}-soname $wl$soname ${wl}-retain-symbols-file $wl$export_symbols -o $lib'
      else
	_LT_AC_TAGVAR(ld_shlibs, $1)=no
      fi
      ;;
    esac

    if test "$_LT_AC_TAGVAR(ld_shlibs, $1)" = no; then
      runpath_var=
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)=
      _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)=
      _LT_AC_TAGVAR(whole_archive_flag_spec, $1)=
    fi
  else
    # PORTME fill in a description of your system's linker (not GNU ld)
    case $host_os in
    aix3*)
      _LT_AC_TAGVAR(allow_undefined_flag, $1)=unsupported
      _LT_AC_TAGVAR(always_export_symbols, $1)=yes
      _LT_AC_TAGVAR(archive_expsym_cmds, $1)='$LD -o $output_objdir/$soname $libobjs $deplibs $linker_flags -bE:$export_symbols -T512 -H512 -bM:SRE~$AR $AR_FLAGS $lib $output_objdir/$soname'
      # Note: this linker hardcodes the directories in LIBPATH if there
      # are no directories specified by -L.
      _LT_AC_TAGVAR(hardcode_minus_L, $1)=yes
      if test "$GCC" = yes && test -z "$lt_prog_compiler_static"; then
	# Neither direct hardcoding nor static linking is supported with a
	# broken collect2.
	_LT_AC_TAGVAR(hardcode_direct, $1)=unsupported
      fi
      ;;

    aix4* | aix5*)
      if test "$host_cpu" = ia64; then
	# On IA64, the linker does run time linking by default, so we don't
	# have to do anything special.
	aix_use_runtimelinking=no
	exp_sym_flag='-Bexport'
	no_entry_flag=""
      else
	# If we're using GNU nm, then we don't want the "-C" option.
	# -C means demangle to AIX nm, but means don't demangle with GNU nm
	if $NM -V 2>&1 | grep 'GNU' > /dev/null; then
	  _LT_AC_TAGVAR(export_symbols_cmds, $1)='$NM -Bpg $libobjs $convenience | awk '\''{ if (((\[$]2 == "T") || (\[$]2 == "D") || (\[$]2 == "B")) && ([substr](\[$]3,1,1) != ".")) { print \[$]3 } }'\'' | sort -u > $export_symbols'
	else
	  _LT_AC_TAGVAR(export_symbols_cmds, $1)='$NM -BCpg $libobjs $convenience | awk '\''{ if (((\[$]2 == "T") || (\[$]2 == "D") || (\[$]2 == "B")) && ([substr](\[$]3,1,1) != ".")) { print \[$]3 } }'\'' | sort -u > $export_symbols'
	fi
	aix_use_runtimelinking=no

	# Test if we are trying to use run time linking or normal
	# AIX style linking. If -brtl is somewhere in LDFLAGS, we
	# need to do runtime linking.
	case $host_os in aix4.[[23]]|aix4.[[23]].*|aix5*)
	  for ld_flag in $LDFLAGS; do
  	  if (test $ld_flag = "-brtl" || test $ld_flag = "-Wl,-brtl"); then
  	    aix_use_runtimelinking=yes
  	    break
  	  fi
	  done
	  ;;
	esac

	exp_sym_flag='-bexport'
	no_entry_flag='-bnoentry'
      fi

      # When large executables or shared objects are built, AIX ld can
      # have problems creating the table of contents.  If linking a library
      # or program results in "error TOC overflow" add -mminimal-toc to
      # CXXFLAGS/CFLAGS for g++/gcc.  In the cases where that is not
      # enough to fix the problem, add -Wl,-bbigtoc to LDFLAGS.

      _LT_AC_TAGVAR(archive_cmds, $1)=''
      _LT_AC_TAGVAR(hardcode_direct, $1)=yes
      _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=':'
      _LT_AC_TAGVAR(link_all_deplibs, $1)=yes

      if test "$GCC" = yes; then
	case $host_os in aix4.[[012]]|aix4.[[012]].*)
	# We only want to do this on AIX 4.2 and lower, the check
	# below for broken collect2 doesn't work under 4.3+
	  collect2name=`${CC} -print-prog-name=collect2`
	  if test -f "$collect2name" && \
  	   strings "$collect2name" | grep resolve_lib_name >/dev/null
	  then
  	  # We have reworked collect2
  	  _LT_AC_TAGVAR(hardcode_direct, $1)=yes
	  else
  	  # We have old collect2
  	  _LT_AC_TAGVAR(hardcode_direct, $1)=unsupported
  	  # It fails to find uninstalled libraries when the uninstalled
  	  # path is not listed in the libpath.  Setting hardcode_minus_L
  	  # to unsupported forces relinking
  	  _LT_AC_TAGVAR(hardcode_minus_L, $1)=yes
  	  _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-L$libdir'
  	  _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=
	  fi
	  ;;
	esac
	shared_flag='-shared'
	if test "$aix_use_runtimelinking" = yes; then
	  shared_flag="$shared_flag "'${wl}-G'
	fi
      else
	# not using gcc
	if test "$host_cpu" = ia64; then
  	# VisualAge C++, Version 5.5 for AIX 5L for IA-64, Beta 3 Release
  	# chokes on -Wl,-G. The following line is correct:
	  shared_flag='-G'
	else
	  if test "$aix_use_runtimelinking" = yes; then
	    shared_flag='${wl}-G'
	  else
	    shared_flag='${wl}-bM:SRE'
	  fi
	fi
      fi

      # It seems that -bexpall does not export symbols beginning with
      # underscore (_), so it is better to generate a list of symbols to export.
      _LT_AC_TAGVAR(always_export_symbols, $1)=yes
      if test "$aix_use_runtimelinking" = yes; then
	# Warning - without using the other runtime loading flags (-brtl),
	# -berok will link without error, but may produce a broken library.
	_LT_AC_TAGVAR(allow_undefined_flag, $1)='-berok'
       # Determine the default libpath from the value encoded in an empty executable.
       _LT_AC_SYS_LIBPATH_AIX
       _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-blibpath:$libdir:'"$aix_libpath"
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)="\$CC"' -o $output_objdir/$soname $libobjs $deplibs '"\${wl}$no_entry_flag"' $compiler_flags `if test "x${allow_undefined_flag}" != "x"; then echo "${wl}${allow_undefined_flag}"; else :; fi` '"\${wl}$exp_sym_flag:\$export_symbols $shared_flag"
       else
	if test "$host_cpu" = ia64; then
	  _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-R $libdir:/usr/lib:/lib'
	  _LT_AC_TAGVAR(allow_undefined_flag, $1)="-z nodefs"
	  _LT_AC_TAGVAR(archive_expsym_cmds, $1)="\$CC $shared_flag"' -o $output_objdir/$soname $libobjs $deplibs '"\${wl}$no_entry_flag"' $compiler_flags ${wl}${allow_undefined_flag} '"\${wl}$exp_sym_flag:\$export_symbols"
	else
	 # Determine the default libpath from the value encoded in an empty executable.
	 _LT_AC_SYS_LIBPATH_AIX
	 _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-blibpath:$libdir:'"$aix_libpath"
	  # Warning - without using the other run time loading flags,
	  # -berok will link without error, but may produce a broken library.
	  _LT_AC_TAGVAR(no_undefined_flag, $1)=' ${wl}-bernotok'
	  _LT_AC_TAGVAR(allow_undefined_flag, $1)=' ${wl}-berok'
	  # Exported symbols can be pulled into shared objects from archives
	  _LT_AC_TAGVAR(whole_archive_flag_spec, $1)='$convenience'
	  _LT_AC_TAGVAR(archive_cmds_need_lc, $1)=yes
	  # This is similar to how AIX traditionally builds its shared libraries.
	  _LT_AC_TAGVAR(archive_expsym_cmds, $1)="\$CC $shared_flag"' -o $output_objdir/$soname $libobjs $deplibs ${wl}-bnoentry $compiler_flags ${wl}-bE:$export_symbols${allow_undefined_flag}~$AR $AR_FLAGS $output_objdir/$libname$release.a $output_objdir/$soname'
	fi
      fi
      ;;

    amigaos*)
      _LT_AC_TAGVAR(archive_cmds, $1)='$rm $output_objdir/a2ixlibrary.data~$echo "#define NAME $libname" > $output_objdir/a2ixlibrary.data~$echo "#define LIBRARY_ID 1" >> $output_objdir/a2ixlibrary.data~$echo "#define VERSION $major" >> $output_objdir/a2ixlibrary.data~$echo "#define REVISION $revision" >> $output_objdir/a2ixlibrary.data~$AR $AR_FLAGS $lib $libobjs~$RANLIB $lib~(cd $output_objdir && a2ixlibrary -32)'
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-L$libdir'
      _LT_AC_TAGVAR(hardcode_minus_L, $1)=yes
      # see comment about different semantics on the GNU ld section
      _LT_AC_TAGVAR(ld_shlibs, $1)=no
      ;;

    bsdi[[45]]*)
      _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)=-rdynamic
      ;;

    cygwin* | mingw* | pw32*)
      # When not using gcc, we currently assume that we are using
      # Microsoft Visual C++.
      # hardcode_libdir_flag_spec is actually meaningless, as there is
      # no search path for DLLs.
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)=' '
      _LT_AC_TAGVAR(allow_undefined_flag, $1)=unsupported
      # Tell ltmain to make .lib files, not .a files.
      libext=lib
      # Tell ltmain to make .dll files, not .so files.
      shrext_cmds=".dll"
      # FIXME: Setting linknames here is a bad hack.
      _LT_AC_TAGVAR(archive_cmds, $1)='$CC -o $lib $libobjs $compiler_flags `echo "$deplibs" | $SED -e '\''s/ -lc$//'\''` -link -dll~linknames='
      # The linker will automatically build a .lib file if we build a DLL.
      _LT_AC_TAGVAR(old_archive_From_new_cmds, $1)='true'
      # FIXME: Should let the user specify the lib program.
      _LT_AC_TAGVAR(old_archive_cmds, $1)='lib /OUT:$oldlib$oldobjs$old_deplibs'
      _LT_AC_TAGVAR(fix_srcfile_path, $1)='`cygpath -w "$srcfile"`'
      _LT_AC_TAGVAR(enable_shared_with_static_runtimes, $1)=yes
      ;;

    darwin* | rhapsody*)
      case $host_os in
        rhapsody* | darwin1.[[012]])
         _LT_AC_TAGVAR(allow_undefined_flag, $1)='${wl}-undefined ${wl}suppress'
         ;;
       *) # Darwin 1.3 on
         if test -z ${MACOSX_DEPLOYMENT_TARGET} ; then
           _LT_AC_TAGVAR(allow_undefined_flag, $1)='${wl}-flat_namespace ${wl}-undefined ${wl}suppress'
         else
           case ${MACOSX_DEPLOYMENT_TARGET} in
             10.[[012]])
               _LT_AC_TAGVAR(allow_undefined_flag, $1)='${wl}-flat_namespace ${wl}-undefined ${wl}suppress'
               ;;
             10.*)
               _LT_AC_TAGVAR(allow_undefined_flag, $1)='${wl}-undefined ${wl}dynamic_lookup'
               ;;
           esac
         fi
         ;;
      esac
      _LT_AC_TAGVAR(archive_cmds_need_lc, $1)=no
      _LT_AC_TAGVAR(hardcode_direct, $1)=no
      _LT_AC_TAGVAR(hardcode_automatic, $1)=yes
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=unsupported
      _LT_AC_TAGVAR(whole_archive_flag_spec, $1)=''
      _LT_AC_TAGVAR(link_all_deplibs, $1)=yes
    if test "$GCC" = yes ; then
    	output_verbose_link_cmd='echo'
        _LT_AC_TAGVAR(archive_cmds, $1)='$CC -dynamiclib $allow_undefined_flag -o $lib $libobjs $deplibs $compiler_flags -install_name $rpath/$soname $verstring'
      _LT_AC_TAGVAR(module_cmds, $1)='$CC $allow_undefined_flag -o $lib -bundle $libobjs $deplibs$compiler_flags'
      # Don't fix this by using the ld -exported_symbols_list flag, it doesn't exist in older darwin lds
      _LT_AC_TAGVAR(archive_expsym_cmds, $1)='sed -e "s,#.*,," -e "s,^[    ]*,," -e "s,^\(..*\),_&," < $export_symbols > $output_objdir/${libname}-symbols.expsym~$CC -dynamiclib $allow_undefined_flag -o $lib $libobjs $deplibs $compiler_flags -install_name $rpath/$soname $verstring~nmedit -s $output_objdir/${libname}-symbols.expsym ${lib}'
      _LT_AC_TAGVAR(module_expsym_cmds, $1)='sed -e "s,#.*,," -e "s,^[    ]*,," -e "s,^\(..*\),_&," < $export_symbols > $output_objdir/${libname}-symbols.expsym~$CC $allow_undefined_flag  -o $lib -bundle $libobjs $deplibs$compiler_flags~nmedit -s $output_objdir/${libname}-symbols.expsym ${lib}'
    else
      case $cc_basename in
        xlc*)
         output_verbose_link_cmd='echo'
         _LT_AC_TAGVAR(archive_cmds, $1)='$CC -qmkshrobj $allow_undefined_flag -o $lib $libobjs $deplibs $compiler_flags ${wl}-install_name ${wl}`echo $rpath/$soname` $verstring'
         _LT_AC_TAGVAR(module_cmds, $1)='$CC $allow_undefined_flag -o $lib -bundle $libobjs $deplibs$compiler_flags'
          # Don't fix this by using the ld -exported_symbols_list flag, it doesn't exist in older darwin lds
         _LT_AC_TAGVAR(archive_expsym_cmds, $1)='sed -e "s,#.*,," -e "s,^[    ]*,," -e "s,^\(..*\),_&," < $export_symbols > $output_objdir/${libname}-symbols.expsym~$CC -qmkshrobj $allow_undefined_flag -o $lib $libobjs $deplibs $compiler_flags ${wl}-install_name ${wl}$rpath/$soname $verstring~nmedit -s $output_objdir/${libname}-symbols.expsym ${lib}'
          _LT_AC_TAGVAR(module_expsym_cmds, $1)='sed -e "s,#.*,," -e "s,^[    ]*,," -e "s,^\(..*\),_&," < $export_symbols > $output_objdir/${libname}-symbols.expsym~$CC $allow_undefined_flag  -o $lib -bundle $libobjs $deplibs$compiler_flags~nmedit -s $output_objdir/${libname}-symbols.expsym ${lib}'
          ;;
       *)
         _LT_AC_TAGVAR(ld_shlibs, $1)=no
          ;;
      esac
    fi
      ;;

    dgux*)
      _LT_AC_TAGVAR(archive_cmds, $1)='$LD -G -h $soname -o $lib $libobjs $deplibs $linker_flags'
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-L$libdir'
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      ;;

    freebsd1*)
      _LT_AC_TAGVAR(ld_shlibs, $1)=no
      ;;

    # FreeBSD 2.2.[012] allows us to include c++rt0.o to get C++ constructor
    # support.  Future versions do this automatically, but an explicit c++rt0.o
    # does not break anything, and helps significantly (at the cost of a little
    # extra space).
    freebsd2.2*)
      _LT_AC_TAGVAR(archive_cmds, $1)='$LD -Bshareable -o $lib $libobjs $deplibs $linker_flags /usr/lib/c++rt0.o'
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-R$libdir'
      _LT_AC_TAGVAR(hardcode_direct, $1)=yes
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      ;;

    # Unfortunately, older versions of FreeBSD 2 do not have this feature.
    freebsd2*)
      _LT_AC_TAGVAR(archive_cmds, $1)='$LD -Bshareable -o $lib $libobjs $deplibs $linker_flags'
      _LT_AC_TAGVAR(hardcode_direct, $1)=yes
      _LT_AC_TAGVAR(hardcode_minus_L, $1)=yes
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      ;;

    # FreeBSD 3 and greater uses gcc -shared to do shared libraries.
    freebsd* | kfreebsd*-gnu | dragonfly*)
      _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -o $lib $libobjs $deplibs $compiler_flags'
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-R$libdir'
      _LT_AC_TAGVAR(hardcode_direct, $1)=yes
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      ;;

    hpux9*)
      if test "$GCC" = yes; then
	_LT_AC_TAGVAR(archive_cmds, $1)='$rm $output_objdir/$soname~$CC -shared -fPIC ${wl}+b ${wl}$install_libdir -o $output_objdir/$soname $libobjs $deplibs $compiler_flags~test $output_objdir/$soname = $lib || mv $output_objdir/$soname $lib'
      else
	_LT_AC_TAGVAR(archive_cmds, $1)='$rm $output_objdir/$soname~$LD -b +b $install_libdir -o $output_objdir/$soname $libobjs $deplibs $linker_flags~test $output_objdir/$soname = $lib || mv $output_objdir/$soname $lib'
      fi
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}+b ${wl}$libdir'
      _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:
      _LT_AC_TAGVAR(hardcode_direct, $1)=yes

      # hardcode_minus_L: Not really in the search PATH,
      # but as the default location of the library.
      _LT_AC_TAGVAR(hardcode_minus_L, $1)=yes
      _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}-E'
      ;;

    hpux10*)
      if test "$GCC" = yes -a "$with_gnu_ld" = no; then
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -fPIC ${wl}+h ${wl}$soname ${wl}+b ${wl}$install_libdir -o $lib $libobjs $deplibs $compiler_flags'
      else
	_LT_AC_TAGVAR(archive_cmds, $1)='$LD -b +h $soname +b $install_libdir -o $lib $libobjs $deplibs $linker_flags'
      fi
      if test "$with_gnu_ld" = no; then
	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}+b ${wl}$libdir'
	_LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:

	_LT_AC_TAGVAR(hardcode_direct, $1)=yes
	_LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}-E'

	# hardcode_minus_L: Not really in the search PATH,
	# but as the default location of the library.
	_LT_AC_TAGVAR(hardcode_minus_L, $1)=yes
      fi
      ;;

    hpux11*)
      if test "$GCC" = yes -a "$with_gnu_ld" = no; then
	case $host_cpu in
	hppa*64*)
	  _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared ${wl}+h ${wl}$soname -o $lib $libobjs $deplibs $compiler_flags'
	  ;;
	ia64*)
	  _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared ${wl}+h ${wl}$soname ${wl}+nodefaultrpath -o $lib $libobjs $deplibs $compiler_flags'
	  ;;
	*)
	  _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared -fPIC ${wl}+h ${wl}$soname ${wl}+b ${wl}$install_libdir -o $lib $libobjs $deplibs $compiler_flags'
	  ;;
	esac
      else
	case $host_cpu in
	hppa*64*)
	  _LT_AC_TAGVAR(archive_cmds, $1)='$CC -b ${wl}+h ${wl}$soname -o $lib $libobjs $deplibs $compiler_flags'
	  ;;
	ia64*)
	  _LT_AC_TAGVAR(archive_cmds, $1)='$CC -b ${wl}+h ${wl}$soname ${wl}+nodefaultrpath -o $lib $libobjs $deplibs $compiler_flags'
	  ;;
	*)
	  _LT_AC_TAGVAR(archive_cmds, $1)='$CC -b ${wl}+h ${wl}$soname ${wl}+b ${wl}$install_libdir -o $lib $libobjs $deplibs $compiler_flags'
	  ;;
	esac
      fi
      if test "$with_gnu_ld" = no; then
	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}+b ${wl}$libdir'
	_LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:

	case $host_cpu in
	hppa*64*|ia64*)
	  _LT_AC_TAGVAR(hardcode_libdir_flag_spec_ld, $1)='+b $libdir'
	  _LT_AC_TAGVAR(hardcode_direct, $1)=no
	  _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
	  ;;
	*)
	  _LT_AC_TAGVAR(hardcode_direct, $1)=yes
	  _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}-E'

	  # hardcode_minus_L: Not really in the search PATH,
	  # but as the default location of the library.
	  _LT_AC_TAGVAR(hardcode_minus_L, $1)=yes
	  ;;
	esac
      fi
      ;;

    irix5* | irix6* | nonstopux*)
      if test "$GCC" = yes; then
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $libobjs $deplibs $compiler_flags ${wl}-soname ${wl}$soname `test -n "$verstring" && echo ${wl}-set_version ${wl}$verstring` ${wl}-update_registry ${wl}${output_objdir}/so_locations -o $lib'
      else
	_LT_AC_TAGVAR(archive_cmds, $1)='$LD -shared $libobjs $deplibs $linker_flags -soname $soname `test -n "$verstring" && echo -set_version $verstring` -update_registry ${output_objdir}/so_locations -o $lib'
	_LT_AC_TAGVAR(hardcode_libdir_flag_spec_ld, $1)='-rpath $libdir'
      fi
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath ${wl}$libdir'
      _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:
      _LT_AC_TAGVAR(link_all_deplibs, $1)=yes
      ;;

    netbsd*)
      if echo __ELF__ | $CC -E - | grep __ELF__ >/dev/null; then
	_LT_AC_TAGVAR(archive_cmds, $1)='$LD -Bshareable -o $lib $libobjs $deplibs $linker_flags'  # a.out
      else
	_LT_AC_TAGVAR(archive_cmds, $1)='$LD -shared -o $lib $libobjs $deplibs $linker_flags'      # ELF
      fi
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-R$libdir'
      _LT_AC_TAGVAR(hardcode_direct, $1)=yes
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      ;;

    newsos6)
      _LT_AC_TAGVAR(archive_cmds, $1)='$LD -G -h $soname -o $lib $libobjs $deplibs $linker_flags'
      _LT_AC_TAGVAR(hardcode_direct, $1)=yes
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath ${wl}$libdir'
      _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      ;;

    openbsd*)
      _LT_AC_TAGVAR(hardcode_direct, $1)=yes
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      if test -z "`echo __ELF__ | $CC -E - | grep __ELF__`" || test "$host_os-$host_cpu" = "openbsd2.8-powerpc"; then
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $pic_flag -o $lib $libobjs $deplibs $compiler_flags'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared $pic_flag -o $lib $libobjs $deplibs $compiler_flags ${wl}-retain-symbols-file,$export_symbols'
	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath,$libdir'
	_LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}-E'
      else
       case $host_os in
	 openbsd[[01]].* | openbsd2.[[0-7]] | openbsd2.[[0-7]].*)
	   _LT_AC_TAGVAR(archive_cmds, $1)='$LD -Bshareable -o $lib $libobjs $deplibs $linker_flags'
	   _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-R$libdir'
	   ;;
	 *)
	   _LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared $pic_flag -o $lib $libobjs $deplibs $compiler_flags'
	   _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath,$libdir'
	   ;;
       esac
      fi
      ;;

    os2*)
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-L$libdir'
      _LT_AC_TAGVAR(hardcode_minus_L, $1)=yes
      _LT_AC_TAGVAR(allow_undefined_flag, $1)=unsupported
      _LT_AC_TAGVAR(archive_cmds, $1)='$echo "LIBRARY $libname INITINSTANCE" > $output_objdir/$libname.def~$echo "DESCRIPTION \"$libname\"" >> $output_objdir/$libname.def~$echo DATA >> $output_objdir/$libname.def~$echo " SINGLE NONSHARED" >> $output_objdir/$libname.def~$echo EXPORTS >> $output_objdir/$libname.def~emxexp $libobjs >> $output_objdir/$libname.def~$CC -Zdll -Zcrtdll -o $lib $libobjs $deplibs $compiler_flags $output_objdir/$libname.def'
      _LT_AC_TAGVAR(old_archive_From_new_cmds, $1)='emximp -o $output_objdir/$libname.a $output_objdir/$libname.def'
      ;;

    osf3*)
      if test "$GCC" = yes; then
	_LT_AC_TAGVAR(allow_undefined_flag, $1)=' ${wl}-expect_unresolved ${wl}\*'
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared${allow_undefined_flag} $libobjs $deplibs $compiler_flags ${wl}-soname ${wl}$soname `test -n "$verstring" && echo ${wl}-set_version ${wl}$verstring` ${wl}-update_registry ${wl}${output_objdir}/so_locations -o $lib'
      else
	_LT_AC_TAGVAR(allow_undefined_flag, $1)=' -expect_unresolved \*'
	_LT_AC_TAGVAR(archive_cmds, $1)='$LD -shared${allow_undefined_flag} $libobjs $deplibs $linker_flags -soname $soname `test -n "$verstring" && echo -set_version $verstring` -update_registry ${output_objdir}/so_locations -o $lib'
      fi
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath ${wl}$libdir'
      _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:
      ;;

    osf4* | osf5*)	# as osf3* with the addition of -msym flag
      if test "$GCC" = yes; then
	_LT_AC_TAGVAR(allow_undefined_flag, $1)=' ${wl}-expect_unresolved ${wl}\*'
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared${allow_undefined_flag} $libobjs $deplibs $compiler_flags ${wl}-msym ${wl}-soname ${wl}$soname `test -n "$verstring" && echo ${wl}-set_version ${wl}$verstring` ${wl}-update_registry ${wl}${output_objdir}/so_locations -o $lib'
	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='${wl}-rpath ${wl}$libdir'
      else
	_LT_AC_TAGVAR(allow_undefined_flag, $1)=' -expect_unresolved \*'
	_LT_AC_TAGVAR(archive_cmds, $1)='$LD -shared${allow_undefined_flag} $libobjs $deplibs $linker_flags -msym -soname $soname `test -n "$verstring" && echo -set_version $verstring` -update_registry ${output_objdir}/so_locations -o $lib'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='for i in `cat $export_symbols`; do printf "%s %s\\n" -exported_symbol "\$i" >> $lib.exp; done; echo "-hidden">> $lib.exp~
	$LD -shared${allow_undefined_flag} -input $lib.exp $linker_flags $libobjs $deplibs -soname $soname `test -n "$verstring" && echo -set_version $verstring` -update_registry ${output_objdir}/so_locations -o $lib~$rm $lib.exp'

	# Both c and cxx compiler support -rpath directly
	_LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-rpath $libdir'
      fi
      _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=:
      ;;

    solaris*)
      _LT_AC_TAGVAR(no_undefined_flag, $1)=' -z text'
      if test "$GCC" = yes; then
	wlarc='${wl}'
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared ${wl}-h ${wl}$soname -o $lib $libobjs $deplibs $compiler_flags'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$echo "{ global:" > $lib.exp~cat $export_symbols | $SED -e "s/\(.*\)/\1;/" >> $lib.exp~$echo "local: *; };" >> $lib.exp~
	  $CC -shared ${wl}-M ${wl}$lib.exp ${wl}-h ${wl}$soname -o $lib $libobjs $deplibs $compiler_flags~$rm $lib.exp'
      else
	wlarc=''
	_LT_AC_TAGVAR(archive_cmds, $1)='$LD -G${allow_undefined_flag} -h $soname -o $lib $libobjs $deplibs $linker_flags'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$echo "{ global:" > $lib.exp~cat $export_symbols | $SED -e "s/\(.*\)/\1;/" >> $lib.exp~$echo "local: *; };" >> $lib.exp~
  	$LD -G${allow_undefined_flag} -M $lib.exp -h $soname -o $lib $libobjs $deplibs $linker_flags~$rm $lib.exp'
      fi
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-R$libdir'
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      case $host_os in
      solaris2.[[0-5]] | solaris2.[[0-5]].*) ;;
      *)
 	# The compiler driver will combine linker options so we
 	# cannot just pass the convience library names through
 	# without $wl, iff we do not link with $LD.
 	# Luckily, gcc supports the same syntax we need for Sun Studio.
 	# Supported since Solaris 2.6 (maybe 2.5.1?)
 	case $wlarc in
 	'')
 	  _LT_AC_TAGVAR(whole_archive_flag_spec, $1)='-z allextract$convenience -z defaultextract' ;;
 	*)
 	  _LT_AC_TAGVAR(whole_archive_flag_spec, $1)='${wl}-z ${wl}allextract`for conv in $convenience\"\"; do test -n \"$conv\" && new_convenience=\"$new_convenience,$conv\"; done; $echo \"$new_convenience\"` ${wl}-z ${wl}defaultextract' ;;
 	esac ;;
      esac
      _LT_AC_TAGVAR(link_all_deplibs, $1)=yes
      ;;

    sunos4*)
      if test "x$host_vendor" = xsequent; then
	# Use $CC to link under sequent, because it throws in some extra .o
	# files that make .init and .fini sections work.
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -G ${wl}-h $soname -o $lib $libobjs $deplibs $compiler_flags'
      else
	_LT_AC_TAGVAR(archive_cmds, $1)='$LD -assert pure-text -Bstatic -o $lib $libobjs $deplibs $linker_flags'
      fi
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-L$libdir'
      _LT_AC_TAGVAR(hardcode_direct, $1)=yes
      _LT_AC_TAGVAR(hardcode_minus_L, $1)=yes
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      ;;

    sysv4)
      case $host_vendor in
	sni)
	  _LT_AC_TAGVAR(archive_cmds, $1)='$LD -G -h $soname -o $lib $libobjs $deplibs $linker_flags'
	  _LT_AC_TAGVAR(hardcode_direct, $1)=yes # is this really true???
	;;
	siemens)
	  ## LD is ld it makes a PLAMLIB
	  ## CC just makes a GrossModule.
	  _LT_AC_TAGVAR(archive_cmds, $1)='$LD -G -o $lib $libobjs $deplibs $linker_flags'
	  _LT_AC_TAGVAR(reload_cmds, $1)='$CC -r -o $output$reload_objs'
	  _LT_AC_TAGVAR(hardcode_direct, $1)=no
        ;;
	motorola)
	  _LT_AC_TAGVAR(archive_cmds, $1)='$LD -G -h $soname -o $lib $libobjs $deplibs $linker_flags'
	  _LT_AC_TAGVAR(hardcode_direct, $1)=no #Motorola manual says yes, but my tests say they lie
	;;
      esac
      runpath_var='LD_RUN_PATH'
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      ;;

    sysv4.3*)
      _LT_AC_TAGVAR(archive_cmds, $1)='$LD -G -h $soname -o $lib $libobjs $deplibs $linker_flags'
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='-Bexport'
      ;;

    sysv4*MP*)
      if test -d /usr/nec; then
	_LT_AC_TAGVAR(archive_cmds, $1)='$LD -G -h $soname -o $lib $libobjs $deplibs $linker_flags'
	_LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
	runpath_var=LD_RUN_PATH
	hardcode_runpath_var=yes
	_LT_AC_TAGVAR(ld_shlibs, $1)=yes
      fi
      ;;

    sysv4*uw2* | sysv5OpenUNIX* | sysv5UnixWare7.[[01]].[[10]]* | unixware7*)
      _LT_AC_TAGVAR(no_undefined_flag, $1)='${wl}-z,text'
      _LT_AC_TAGVAR(archive_cmds_need_lc, $1)=no
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      runpath_var='LD_RUN_PATH'

      if test "$GCC" = yes; then
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared ${wl}-h,$soname -o $lib $libobjs $deplibs $compiler_flags'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared ${wl}-Bexport:$export_symbols ${wl}-h,$soname -o $lib $libobjs $deplibs $compiler_flags'
      else
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -G ${wl}-h,$soname -o $lib $libobjs $deplibs $compiler_flags'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -G ${wl}-Bexport:$export_symbols ${wl}-h,$soname -o $lib $libobjs $deplibs $compiler_flags'
      fi
      ;;

    sysv5* | sco3.2v5* | sco5v6*)
      # Note: We can NOT use -z defs as we might desire, because we do not
      # link with -lc, and that would cause any symbols used from libc to
      # always be unresolved, which means just about no library would
      # ever link correctly.  If we're not using GNU ld we use -z text
      # though, which does catch some bad symbols but isn't as heavy-handed
      # as -z defs.
      _LT_AC_TAGVAR(no_undefined_flag, $1)='${wl}-z,text'
      _LT_AC_TAGVAR(allow_undefined_flag, $1)='${wl}-z,nodefs'
      _LT_AC_TAGVAR(archive_cmds_need_lc, $1)=no
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='`test -z "$SCOABSPATH" && echo ${wl}-R,$libdir`'
      _LT_AC_TAGVAR(hardcode_libdir_separator, $1)=':'
      _LT_AC_TAGVAR(link_all_deplibs, $1)=yes
      _LT_AC_TAGVAR(export_dynamic_flag_spec, $1)='${wl}-Bexport'
      runpath_var='LD_RUN_PATH'

      if test "$GCC" = yes; then
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -shared ${wl}-h,\${SCOABSPATH:+${install_libdir}/}$soname -o $lib $libobjs $deplibs $compiler_flags'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -shared ${wl}-Bexport:$export_symbols ${wl}-h,\${SCOABSPATH:+${install_libdir}/}$soname -o $lib $libobjs $deplibs $compiler_flags'
      else
	_LT_AC_TAGVAR(archive_cmds, $1)='$CC -G ${wl}-h,\${SCOABSPATH:+${install_libdir}/}$soname -o $lib $libobjs $deplibs $compiler_flags'
	_LT_AC_TAGVAR(archive_expsym_cmds, $1)='$CC -G ${wl}-Bexport:$export_symbols ${wl}-h,\${SCOABSPATH:+${install_libdir}/}$soname -o $lib $libobjs $deplibs $compiler_flags'
      fi
      ;;

    uts4*)
      _LT_AC_TAGVAR(archive_cmds, $1)='$LD -G -h $soname -o $lib $libobjs $deplibs $linker_flags'
      _LT_AC_TAGVAR(hardcode_libdir_flag_spec, $1)='-L$libdir'
      _LT_AC_TAGVAR(hardcode_shlibpath_var, $1)=no
      ;;

    *)
      _LT_AC_TAGVAR(ld_shlibs, $1)=no
      ;;
    esac
  fi
])
AC_MSG_RESULT([$_LT_AC_TAGVAR(ld_shlibs, $1)])
test "$_LT_AC_TAGVAR(ld_shlibs, $1)" = no && can_build_shared=no

#
# Do we need to explicitly link libc?
#
case "x$_LT_AC_TAGVAR(archive_cmds_need_lc, $1)" in
x|xyes)
  # Assume -lc should be added
  _LT_AC_TAGVAR(archive_cmds_need_lc, $1)=yes

  if test "$enable_shared" = yes && test "$GCC" = yes; then
    case $_LT_AC_TAGVAR(archive_cmds, $1) in
    *'~'*)
      # FIXME: we may have to deal with multi-command sequences.
      ;;
    '$CC '*)
      # Test whether the compiler implicitly links with -lc since on some
      # systems, -lgcc has to come before -lc. If gcc already passes -lc
      # to ld, don't add -lc before -lgcc.
      AC_MSG_CHECKING([whether -lc should be explicitly linked in])
      $rm conftest*
      printf "$lt_simple_compile_test_code" > conftest.$ac_ext

      if AC_TRY_EVAL(ac_compile) 2>conftest.err; then
        soname=conftest
        lib=conftest
        libobjs=conftest.$ac_objext
        deplibs=
        wl=$_LT_AC_TAGVAR(lt_prog_compiler_wl, $1)
	pic_flag=$_LT_AC_TAGVAR(lt_prog_compiler_pic, $1)
        compiler_flags=-v
        linker_flags=-v
        verstring=
        output_objdir=.
        libname=conftest
        lt_save_allow_undefined_flag=$_LT_AC_TAGVAR(allow_undefined_flag, $1)
        _LT_AC_TAGVAR(allow_undefined_flag, $1)=
        if AC_TRY_EVAL(_LT_AC_TAGVAR(archive_cmds, $1) 2\>\&1 \| grep \" -lc \" \>/dev/null 2\>\&1)
        then
	  _LT_AC_TAGVAR(archive_cmds_need_lc, $1)=no
        else
	  _LT_AC_TAGVAR(archive_cmds_need_lc, $1)=yes
        fi
        _LT_AC_TAGVAR(allow_undefined_flag, $1)=$lt_save_allow_undefined_flag
      else
        cat conftest.err 1>&5
      fi
      $rm conftest*
      AC_MSG_RESULT([$_LT_AC_TAGVAR(archive_cmds_need_lc, $1)])
      ;;
    esac
  fi
  ;;
esac
])# AC_LIBTOOL_PROG_LD_SHLIBS


# _LT_AC_FILE_LTDLL_C
# -------------------
# Be careful that the start marker always follows a newline.
AC_DEFUN([_LT_AC_FILE_LTDLL_C], [
# /* ltdll.c starts here */
# #define WIN32_LEAN_AND_MEAN
# #include <windows.h>
# #undef WIN32_LEAN_AND_MEAN
# #include <stdio.h>
#
# #ifndef __CYGWIN__
# #  ifdef __CYGWIN32__
# #    define __CYGWIN__ __CYGWIN32__
# #  endif
# #endif
#
# #ifdef __cplusplus
# extern "C" {
# #endif
# BOOL APIENTRY DllMain (HINSTANCE hInst, DWORD reason, LPVOID reserved);
# #ifdef __cplusplus
# }
# #endif
#
# #ifdef __CYGWIN__
# #include <cygwin/cygwin_dll.h>
# DECLARE_CYGWIN_DLL( DllMain );
# #endif
# HINSTANCE __hDllInstance_base;
#
# BOOL APIENTRY
# DllMain (HINSTANCE hInst, DWORD reason, LPVOID reserved)
# {
#   __hDllInstance_base = hInst;
#   return TRUE;
# }
# /* ltdll.c ends here */
])# _LT_AC_FILE_LTDLL_C


# _LT_AC_TAGVAR(VARNAME, [TAGNAME])
# ---------------------------------
AC_DEFUN([_LT_AC_TAGVAR], [ifelse([$2], [], [$1], [$1_$2])])


# old names
AC_DEFUN([AM_PROG_LIBTOOL],   [AC_PROG_LIBTOOL])
AC_DEFUN([AM_ENABLE_SHARED],  [AC_ENABLE_SHARED($@)])
AC_DEFUN([AM_ENABLE_STATIC],  [AC_ENABLE_STATIC($@)])
AC_DEFUN([AM_DISABLE_SHARED], [AC_DISABLE_SHARED($@)])
AC_DEFUN([AM_DISABLE_STATIC], [AC_DISABLE_STATIC($@)])
AC_DEFUN([AM_PROG_LD],        [AC_PROG_LD])
AC_DEFUN([AM_PROG_NM],        [AC_PROG_NM])

# This is just to silence aclocal about the macro not being used
ifelse([AC_DISABLE_FAST_INSTALL])

AC_DEFUN([LT_AC_PROG_GCJ],
[AC_CHECK_TOOL(GCJ, gcj, no)
  test "x${GCJFLAGS+set}" = xset || GCJFLAGS="-g -O2"
  AC_SUBST(GCJFLAGS)
])

AC_DEFUN([LT_AC_PROG_RC],
[AC_CHECK_TOOL(RC, windres, no)
])

# NOTE: This macro has been submitted for inclusion into   #
#  GNU Autoconf as AC_PROG_SED.  When it is available in   #
#  a released version of Autoconf we should remove this    #
#  macro and use it instead.                               #
# LT_AC_PROG_SED
# --------------
# Check for a fully-functional sed program, that truncates
# as few characters as possible.  Prefer GNU sed if found.
AC_DEFUN([LT_AC_PROG_SED],
[AC_MSG_CHECKING([for a sed that does not truncate output])
AC_CACHE_VAL(lt_cv_path_SED,
[# Loop through the user's path and test for sed and gsed.
# Then use that list of sed's as ones to test for truncation.
as_save_IFS=$IFS; IFS=$PATH_SEPARATOR
for as_dir in $PATH
do
  IFS=$as_save_IFS
  test -z "$as_dir" && as_dir=.
  for lt_ac_prog in sed gsed; do
    for ac_exec_ext in '' $ac_executable_extensions; do
      if $as_executable_p "$as_dir/$lt_ac_prog$ac_exec_ext"; then
        lt_ac_sed_list="$lt_ac_sed_list $as_dir/$lt_ac_prog$ac_exec_ext"
      fi
    done
  done
done
IFS=$as_save_IFS
lt_ac_max=0
lt_ac_count=0
# Add /usr/xpg4/bin/sed as it is typically found on Solaris
# along with /bin/sed that truncates output.
for lt_ac_sed in $lt_ac_sed_list /usr/xpg4/bin/sed; do
  test ! -f $lt_ac_sed && continue
  cat /dev/null > conftest.in
  lt_ac_count=0
  echo $ECHO_N "0123456789$ECHO_C" >conftest.in
  # Check for GNU sed and select it if it is found.
  if "$lt_ac_sed" --version 2>&1 < /dev/null | grep 'GNU' > /dev/null; then
    lt_cv_path_SED=$lt_ac_sed
    break
  fi
  while true; do
    cat conftest.in conftest.in >conftest.tmp
    mv conftest.tmp conftest.in
    cp conftest.in conftest.nl
    echo >>conftest.nl
    $lt_ac_sed -e 's/a$//' < conftest.nl >conftest.out || break
    cmp -s conftest.out conftest.nl || break
    # 10000 chars as input seems more than enough
    test $lt_ac_count -gt 10 && break
    lt_ac_count=`expr $lt_ac_count + 1`
    if test $lt_ac_count -gt $lt_ac_max; then
      lt_ac_max=$lt_ac_count
      lt_cv_path_SED=$lt_ac_sed
    fi
  done
done
])
SED=$lt_cv_path_SED
AC_SUBST([SED])
AC_MSG_RESULT([$SED])
])

# Configure paths for LIBXML2
# Mike Hommey 2004-06-19
# use CPPFLAGS instead of CFLAGS
# Toshio Kuratomi 2001-04-21
# Adapted from:
# Configure paths for GLIB
# Owen Taylor     97-11-3

dnl AM_PATH_XML2([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for XML, and define XML_CPPFLAGS and XML_LIBS
dnl
AC_DEFUN([AM_PATH_XML2],[ 
AC_ARG_WITH(xml-prefix,
            [  --with-xml-prefix=PFX   Prefix where libxml is installed (optional)],
            xml_config_prefix="$withval", xml_config_prefix="")
AC_ARG_WITH(xml-exec-prefix,
            [  --with-xml-exec-prefix=PFX Exec prefix where libxml is installed (optional)],
            xml_config_exec_prefix="$withval", xml_config_exec_prefix="")
AC_ARG_ENABLE(xmltest,
              [  --disable-xmltest       Do not try to compile and run a test LIBXML program],,
              enable_xmltest=yes)

  if test x$xml_config_exec_prefix != x ; then
     xml_config_args="$xml_config_args"
     if test x${XML2_CONFIG+set} != xset ; then
        XML2_CONFIG=$xml_config_exec_prefix/bin/xml2-config
     fi
  fi
  if test x$xml_config_prefix != x ; then
     xml_config_args="$xml_config_args --prefix=$xml_config_prefix"
     if test x${XML2_CONFIG+set} != xset ; then
        XML2_CONFIG=$xml_config_prefix/bin/xml2-config
     fi
  fi

  AC_PATH_PROG(XML2_CONFIG, xml2-config, no)
  min_xml_version=ifelse([$1], ,2.0.0,[$1])
  AC_MSG_CHECKING(for libxml - version >= $min_xml_version)
  no_xml=""
  if test "$XML2_CONFIG" = "no" ; then
    no_xml=yes
  else
    XML_CPPFLAGS=`$XML2_CONFIG $xml_config_args --cflags`
    XML_LIBS=`$XML2_CONFIG $xml_config_args --libs`
    xml_config_major_version=`$XML2_CONFIG $xml_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    xml_config_minor_version=`$XML2_CONFIG $xml_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    xml_config_micro_version=`$XML2_CONFIG $xml_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_xmltest" = "xyes" ; then
      ac_save_CPPFLAGS="$CPPFLAGS"
      ac_save_LIBS="$LIBS"
      CPPFLAGS="$CPPFLAGS $XML_CPPFLAGS"
      LIBS="$XML_LIBS $LIBS"
dnl
dnl Now check if the installed libxml is sufficiently new.
dnl (Also sanity checks the results of xml2-config to some extent)
dnl
      rm -f conf.xmltest
      AC_TRY_RUN([
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libxml/xmlversion.h>

int 
main()
{
  int xml_major_version, xml_minor_version, xml_micro_version;
  int major, minor, micro;
  char *tmp_version;

  system("touch conf.xmltest");

  /* Capture xml2-config output via autoconf/configure variables */
  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = (char *)strdup("$min_xml_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string from xml2-config\n", "$min_xml_version");
     exit(1);
   }
   free(tmp_version);

   /* Capture the version information from the header files */
   tmp_version = (char *)strdup(LIBXML_DOTTED_VERSION);
   if (sscanf(tmp_version, "%d.%d.%d", &xml_major_version, &xml_minor_version, &xml_micro_version) != 3) {
     printf("%s, bad version string from libxml includes\n", "LIBXML_DOTTED_VERSION");
     exit(1);
   }
   free(tmp_version);

 /* Compare xml2-config output to the libxml headers */
  if ((xml_major_version != $xml_config_major_version) ||
      (xml_minor_version != $xml_config_minor_version) ||
      (xml_micro_version != $xml_config_micro_version))
    {
      printf("*** libxml header files (version %d.%d.%d) do not match\n",
         xml_major_version, xml_minor_version, xml_micro_version);
      printf("*** xml2-config (version %d.%d.%d)\n",
         $xml_config_major_version, $xml_config_minor_version, $xml_config_micro_version);
      return 1;
    } 
/* Compare the headers to the library to make sure we match */
  /* Less than ideal -- doesn't provide us with return value feedback, 
   * only exits if there's a serious mismatch between header and library.
   */
    LIBXML_TEST_VERSION;

    /* Test that the library is greater than our minimum version */
    if ((xml_major_version > major) ||
        ((xml_major_version == major) && (xml_minor_version > minor)) ||
        ((xml_major_version == major) && (xml_minor_version == minor) &&
        (xml_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of libxml (%d.%d.%d) was found.\n",
               xml_major_version, xml_minor_version, xml_micro_version);
        printf("*** You need a version of libxml newer than %d.%d.%d. The latest version of\n",
           major, minor, micro);
        printf("*** libxml is always available from ftp://ftp.xmlsoft.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the xml2-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of LIBXML, but you can also set the XML2_CONFIG environment to point to the\n");
        printf("*** correct copy of xml2-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
    }
  return 1;
}
],, no_xml=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CPPFLAGS="$ac_save_CPPFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi

  if test "x$no_xml" = x ; then
     AC_MSG_RESULT(yes (version $xml_config_major_version.$xml_config_minor_version.$xml_config_micro_version))
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$XML2_CONFIG" = "no" ; then
       echo "*** The xml2-config script installed by LIBXML could not be found"
       echo "*** If libxml was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the XML2_CONFIG environment variable to the"
       echo "*** full path to xml2-config."
     else
       if test -f conf.xmltest ; then
        :
       else
          echo "*** Could not run libxml test program, checking why..."
          CPPFLAGS="$CPPFLAGS $XML_CPPFLAGS"
          LIBS="$LIBS $XML_LIBS"
          AC_TRY_LINK([
#include <libxml/xmlversion.h>
#include <stdio.h>
],      [ LIBXML_TEST_VERSION; return 0;],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding LIBXML or finding the wrong"
          echo "*** version of LIBXML. If it is not finding LIBXML, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
          echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means LIBXML was incorrectly installed"
          echo "*** or that you have moved LIBXML since it was installed. In the latter case, you"
          echo "*** may want to edit the xml2-config script: $XML2_CONFIG" ])
          CPPFLAGS="$ac_save_CPPFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi

     XML_CPPFLAGS=""
     XML_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(XML_CPPFLAGS)
  AC_SUBST(XML_LIBS)
  rm -f conf.xmltest
])


# serial 7 AC_LIB_LTDL

# AC_WITH_LTDL
# ------------
# Clients of libltdl can use this macro to allow the installer to
# choose between a shipped copy of the ltdl sources or a preinstalled
# version of the library.
AC_DEFUN([AC_WITH_LTDL],
[AC_REQUIRE([AC_LIB_LTDL])
AC_SUBST([LIBLTDL])
AC_SUBST([INCLTDL])

# Unless the user asks us to check, assume no installed ltdl exists.
use_installed_libltdl=no

AC_ARG_WITH([included_ltdl],
    [  --with-included-ltdl    use the GNU ltdl sources included here])

if test "x$with_included_ltdl" != xyes; then
  # We are not being forced to use the included libltdl sources, so
  # decide whether there is a useful installed version we can use.
  AC_CHECK_HEADER([ltdl.h],
      [AC_CHECK_LIB([ltdl], [lt_dlcaller_register],
          [with_included_ltdl=no],
          [with_included_ltdl=yes])
  ])
fi

if test "x$enable_ltdl_install" != xyes; then
  # If the user did not specify an installable libltdl, then default
  # to a convenience lib.
  AC_LIBLTDL_CONVENIENCE
fi

if test "x$with_included_ltdl" = xno; then
  # If the included ltdl is not to be used. then Use the
  # preinstalled libltdl we found.
  AC_DEFINE([HAVE_LTDL], [1],
    [Define this if a modern libltdl is already installed])
  LIBLTDL=-lltdl
fi

# Report our decision...
AC_MSG_CHECKING([whether to use included libltdl])
AC_MSG_RESULT([$with_included_ltdl])

AC_CONFIG_SUBDIRS([libltdl])
])# AC_WITH_LTDL


# AC_LIB_LTDL
# -----------
# Perform all the checks necessary for compilation of the ltdl objects
#  -- including compiler checks and header checks.
AC_DEFUN([AC_LIB_LTDL],
[AC_PREREQ(2.50)
AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([AC_C_CONST])
AC_REQUIRE([AC_HEADER_STDC])
AC_REQUIRE([AC_HEADER_DIRENT])
AC_REQUIRE([_LT_AC_CHECK_DLFCN])
AC_REQUIRE([AC_LTDL_ENABLE_INSTALL])
AC_REQUIRE([AC_LTDL_SHLIBEXT])
AC_REQUIRE([AC_LTDL_SHLIBPATH])
AC_REQUIRE([AC_LTDL_SYSSEARCHPATH])
AC_REQUIRE([AC_LTDL_OBJDIR])
AC_REQUIRE([AC_LTDL_DLPREOPEN])
AC_REQUIRE([AC_LTDL_DLLIB])
AC_REQUIRE([AC_LTDL_SYMBOL_USCORE])
AC_REQUIRE([AC_LTDL_DLSYM_USCORE])
AC_REQUIRE([AC_LTDL_SYS_DLOPEN_DEPLIBS])
AC_REQUIRE([AC_LTDL_FUNC_ARGZ])

AC_CHECK_HEADERS([assert.h ctype.h errno.h malloc.h memory.h stdlib.h \
		  stdio.h unistd.h])
AC_CHECK_HEADERS([dl.h sys/dl.h dld.h mach-o/dyld.h])
AC_CHECK_HEADERS([string.h strings.h], [break])

AC_CHECK_FUNCS([strchr index], [break])
AC_CHECK_FUNCS([strrchr rindex], [break])
AC_CHECK_FUNCS([memcpy bcopy], [break])
AC_CHECK_FUNCS([memmove strcmp])
AC_CHECK_FUNCS([closedir opendir readdir])
])# AC_LIB_LTDL


# AC_LTDL_ENABLE_INSTALL
# ----------------------
AC_DEFUN([AC_LTDL_ENABLE_INSTALL],
[AC_ARG_ENABLE([ltdl-install],
    [AC_HELP_STRING([--enable-ltdl-install], [install libltdl])])

AM_CONDITIONAL(INSTALL_LTDL, test x"${enable_ltdl_install-no}" != xno)
AM_CONDITIONAL(CONVENIENCE_LTDL, test x"${enable_ltdl_convenience-no}" != xno)
])# AC_LTDL_ENABLE_INSTALL


# AC_LTDL_SYS_DLOPEN_DEPLIBS
# --------------------------
AC_DEFUN([AC_LTDL_SYS_DLOPEN_DEPLIBS],
[AC_REQUIRE([AC_CANONICAL_HOST])
AC_CACHE_CHECK([whether deplibs are loaded by dlopen],
  [libltdl_cv_sys_dlopen_deplibs],
  [# PORTME does your system automatically load deplibs for dlopen?
  # or its logical equivalent (e.g. shl_load for HP-UX < 11)
  # For now, we just catch OSes we know something about -- in the
  # future, we'll try test this programmatically.
  libltdl_cv_sys_dlopen_deplibs=unknown
  case "$host_os" in
  aix3*|aix4.1.*|aix4.2.*)
    # Unknown whether this is true for these versions of AIX, but
    # we want this `case' here to explicitly catch those versions.
    libltdl_cv_sys_dlopen_deplibs=unknown
    ;;
  aix[[45]]*)
    libltdl_cv_sys_dlopen_deplibs=yes
    ;;
  darwin*)
    # Assuming the user has installed a libdl from somewhere, this is true
    # If you are looking for one http://www.opendarwin.org/projects/dlcompat
    libltdl_cv_sys_dlopen_deplibs=yes
    ;;
  gnu* | linux* | kfreebsd*-gnu | knetbsd*-gnu)
    # GNU and its variants, using gnu ld.so (Glibc)
    libltdl_cv_sys_dlopen_deplibs=yes
    ;;
  hpux10*|hpux11*)
    libltdl_cv_sys_dlopen_deplibs=yes
    ;;
  interix*)
    libltdl_cv_sys_dlopen_deplibs=yes
    ;;
  irix[[12345]]*|irix6.[[01]]*)
    # Catch all versions of IRIX before 6.2, and indicate that we don't
    # know how it worked for any of those versions.
    libltdl_cv_sys_dlopen_deplibs=unknown
    ;;
  irix*)
    # The case above catches anything before 6.2, and it's known that
    # at 6.2 and later dlopen does load deplibs.
    libltdl_cv_sys_dlopen_deplibs=yes
    ;;
  netbsd*)
    libltdl_cv_sys_dlopen_deplibs=yes
    ;;
  openbsd*)
    libltdl_cv_sys_dlopen_deplibs=yes
    ;;
  osf[[1234]]*)
    # dlopen did load deplibs (at least at 4.x), but until the 5.x series,
    # it did *not* use an RPATH in a shared library to find objects the
    # library depends on, so we explictly say `no'.
    libltdl_cv_sys_dlopen_deplibs=no
    ;;
  osf5.0|osf5.0a|osf5.1)
    # dlopen *does* load deplibs and with the right loader patch applied
    # it even uses RPATH in a shared library to search for shared objects
    # that the library depends on, but there's no easy way to know if that
    # patch is installed.  Since this is the case, all we can really
    # say is unknown -- it depends on the patch being installed.  If
    # it is, this changes to `yes'.  Without it, it would be `no'.
    libltdl_cv_sys_dlopen_deplibs=unknown
    ;;
  osf*)
    # the two cases above should catch all versions of osf <= 5.1.  Read
    # the comments above for what we know about them.
    # At > 5.1, deplibs are loaded *and* any RPATH in a shared library
    # is used to find them so we can finally say `yes'.
    libltdl_cv_sys_dlopen_deplibs=yes
    ;;
  solaris*)
    libltdl_cv_sys_dlopen_deplibs=yes
    ;;
  sysv5* | sco3.2v5* | sco5v6* | unixware* | OpenUNIX* | sysv4*uw2*)
    libltdl_cv_sys_dlopen_deplibs=yes
    ;;
  esac
  ])
if test "$libltdl_cv_sys_dlopen_deplibs" != yes; then
 AC_DEFINE([LTDL_DLOPEN_DEPLIBS], [1],
    [Define if the OS needs help to load dependent libraries for dlopen().])
fi
])# AC_LTDL_SYS_DLOPEN_DEPLIBS


# AC_LTDL_SHLIBEXT
# ----------------
AC_DEFUN([AC_LTDL_SHLIBEXT],
[AC_REQUIRE([AC_LIBTOOL_SYS_DYNAMIC_LINKER])
AC_CACHE_CHECK([which extension is used for loadable modules],
  [libltdl_cv_shlibext],
[
module=yes
eval libltdl_cv_shlibext=$shrext_cmds
  ])
if test -n "$libltdl_cv_shlibext"; then
  AC_DEFINE_UNQUOTED([LTDL_SHLIB_EXT], ["$libltdl_cv_shlibext"],
    [Define to the extension used for shared libraries, say, ".so".])
fi
])# AC_LTDL_SHLIBEXT


# AC_LTDL_SHLIBPATH
# -----------------
AC_DEFUN([AC_LTDL_SHLIBPATH],
[AC_REQUIRE([AC_LIBTOOL_SYS_DYNAMIC_LINKER])
AC_CACHE_CHECK([which variable specifies run-time library path],
  [libltdl_cv_shlibpath_var], [libltdl_cv_shlibpath_var="$shlibpath_var"])
if test -n "$libltdl_cv_shlibpath_var"; then
  AC_DEFINE_UNQUOTED([LTDL_SHLIBPATH_VAR], ["$libltdl_cv_shlibpath_var"],
    [Define to the name of the environment variable that determines the dynamic library search path.])
fi
])# AC_LTDL_SHLIBPATH


# AC_LTDL_SYSSEARCHPATH
# ---------------------
AC_DEFUN([AC_LTDL_SYSSEARCHPATH],
[AC_REQUIRE([AC_LIBTOOL_SYS_DYNAMIC_LINKER])
AC_CACHE_CHECK([for the default library search path],
  [libltdl_cv_sys_search_path],
  [libltdl_cv_sys_search_path="$sys_lib_dlsearch_path_spec"])
if test -n "$libltdl_cv_sys_search_path"; then
  sys_search_path=
  for dir in $libltdl_cv_sys_search_path; do
    if test -z "$sys_search_path"; then
      sys_search_path="$dir"
    else
      sys_search_path="$sys_search_path$PATH_SEPARATOR$dir"
    fi
  done
  AC_DEFINE_UNQUOTED([LTDL_SYSSEARCHPATH], ["$sys_search_path"],
    [Define to the system default library search path.])
fi
])# AC_LTDL_SYSSEARCHPATH


# AC_LTDL_OBJDIR
# --------------
AC_DEFUN([AC_LTDL_OBJDIR],
[AC_CACHE_CHECK([for objdir],
  [libltdl_cv_objdir],
  [libltdl_cv_objdir="$objdir"
  if test -n "$objdir"; then
    :
  else
    rm -f .libs 2>/dev/null
    mkdir .libs 2>/dev/null
    if test -d .libs; then
      libltdl_cv_objdir=.libs
    else
      # MS-DOS does not allow filenames that begin with a dot.
      libltdl_cv_objdir=_libs
    fi
  rmdir .libs 2>/dev/null
  fi
  ])
AC_DEFINE_UNQUOTED([LTDL_OBJDIR], ["$libltdl_cv_objdir/"],
  [Define to the sub-directory in which libtool stores uninstalled libraries.])
])# AC_LTDL_OBJDIR


# AC_LTDL_DLPREOPEN
# -----------------
AC_DEFUN([AC_LTDL_DLPREOPEN],
[AC_REQUIRE([AC_LIBTOOL_SYS_GLOBAL_SYMBOL_PIPE])
AC_CACHE_CHECK([whether libtool supports -dlopen/-dlpreopen],
  [libltdl_cv_preloaded_symbols],
  [if test -n "$lt_cv_sys_global_symbol_pipe"; then
    libltdl_cv_preloaded_symbols=yes
  else
    libltdl_cv_preloaded_symbols=no
  fi
  ])
if test x"$libltdl_cv_preloaded_symbols" = xyes; then
  AC_DEFINE([HAVE_PRELOADED_SYMBOLS], [1],
    [Define if libtool can extract symbol lists from object files.])
fi
])# AC_LTDL_DLPREOPEN


# AC_LTDL_DLLIB
# -------------
AC_DEFUN([AC_LTDL_DLLIB],
[LIBADD_DL=
AC_SUBST(LIBADD_DL)
AC_LANG_PUSH([C])

AC_CHECK_FUNC([shl_load],
      [AC_DEFINE([HAVE_SHL_LOAD], [1],
		 [Define if you have the shl_load function.])],
  [AC_CHECK_LIB([dld], [shl_load],
	[AC_DEFINE([HAVE_SHL_LOAD], [1],
		   [Define if you have the shl_load function.])
	LIBADD_DL="$LIBADD_DL -ldld"],
    [AC_CHECK_LIB([dl], [dlopen],
	  [AC_DEFINE([HAVE_LIBDL], [1],
		     [Define if you have the libdl library or equivalent.])
	        LIBADD_DL="-ldl" libltdl_cv_lib_dl_dlopen="yes"],
      [AC_TRY_LINK([#if HAVE_DLFCN_H
#  include <dlfcn.h>
#endif
      ],
	[dlopen(0, 0);],
	    [AC_DEFINE([HAVE_LIBDL], [1],
		             [Define if you have the libdl library or equivalent.]) libltdl_cv_func_dlopen="yes"],
	[AC_CHECK_LIB([svld], [dlopen],
	      [AC_DEFINE([HAVE_LIBDL], [1],
			 [Define if you have the libdl library or equivalent.])
	            LIBADD_DL="-lsvld" libltdl_cv_func_dlopen="yes"],
	  [AC_CHECK_LIB([dld], [dld_link],
	        [AC_DEFINE([HAVE_DLD], [1],
			   [Define if you have the GNU dld library.])
	 	LIBADD_DL="$LIBADD_DL -ldld"],
	 	[AC_CHECK_FUNC([_dyld_func_lookup],
	 	       [AC_DEFINE([HAVE_DYLD], [1],
	 	          [Define if you have the _dyld_func_lookup function.])])
          ])
        ])
      ])
    ])
  ])
])

if test x"$libltdl_cv_func_dlopen" = xyes || test x"$libltdl_cv_lib_dl_dlopen" = xyes
then
  lt_save_LIBS="$LIBS"
  LIBS="$LIBS $LIBADD_DL"
  AC_CHECK_FUNCS([dlerror])
  LIBS="$lt_save_LIBS"
fi
AC_LANG_POP
])# AC_LTDL_DLLIB


# AC_LTDL_SYMBOL_USCORE
# ---------------------
# does the compiler prefix global symbols with an underscore?
AC_DEFUN([AC_LTDL_SYMBOL_USCORE],
[AC_REQUIRE([AC_LIBTOOL_SYS_GLOBAL_SYMBOL_PIPE])
AC_CACHE_CHECK([for _ prefix in compiled symbols],
  [ac_cv_sys_symbol_underscore],
  [ac_cv_sys_symbol_underscore=no
  cat > conftest.$ac_ext <<EOF
void nm_test_func(){}
int main(){nm_test_func;return 0;}
EOF
  if AC_TRY_EVAL(ac_compile); then
    # Now try to grab the symbols.
    ac_nlist=conftest.nm
    if AC_TRY_EVAL(NM conftest.$ac_objext \| $lt_cv_sys_global_symbol_pipe \> $ac_nlist) && test -s "$ac_nlist"; then
      # See whether the symbols have a leading underscore.
      if grep '^. _nm_test_func' "$ac_nlist" >/dev/null; then
        ac_cv_sys_symbol_underscore=yes
      else
        if grep '^. nm_test_func ' "$ac_nlist" >/dev/null; then
	  :
        else
	  echo "configure: cannot find nm_test_func in $ac_nlist" >&AC_FD_CC
        fi
      fi
    else
      echo "configure: cannot run $lt_cv_sys_global_symbol_pipe" >&AC_FD_CC
    fi
  else
    echo "configure: failed program was:" >&AC_FD_CC
    cat conftest.c >&AC_FD_CC
  fi
  rm -rf conftest*
  ])
])# AC_LTDL_SYMBOL_USCORE


# AC_LTDL_DLSYM_USCORE
# --------------------
AC_DEFUN([AC_LTDL_DLSYM_USCORE],
[AC_REQUIRE([AC_LTDL_SYMBOL_USCORE])
if test x"$ac_cv_sys_symbol_underscore" = xyes; then
  if test x"$libltdl_cv_func_dlopen" = xyes ||
     test x"$libltdl_cv_lib_dl_dlopen" = xyes ; then
	AC_CACHE_CHECK([whether we have to add an underscore for dlsym],
	  [libltdl_cv_need_uscore],
	  [libltdl_cv_need_uscore=unknown
          save_LIBS="$LIBS"
          LIBS="$LIBS $LIBADD_DL"
	  _LT_AC_TRY_DLOPEN_SELF(
	    [libltdl_cv_need_uscore=no], [libltdl_cv_need_uscore=yes],
	    [],				 [libltdl_cv_need_uscore=cross])
	  LIBS="$save_LIBS"
	])
  fi
fi

if test x"$libltdl_cv_need_uscore" = xyes; then
  AC_DEFINE([NEED_USCORE], [1],
    [Define if dlsym() requires a leading underscore in symbol names.])
fi
])# AC_LTDL_DLSYM_USCORE

# AC_LTDL_FUNC_ARGZ
# -----------------
AC_DEFUN([AC_LTDL_FUNC_ARGZ],
[AC_CHECK_HEADERS([argz.h])

AC_CHECK_TYPES([error_t],
  [],
  [AC_DEFINE([error_t], [int],
    [Define to a type to use for `error_t' if it is not otherwise available.])],
  [#if HAVE_ARGZ_H
#  include <argz.h>
#endif])

AC_CHECK_FUNCS([argz_append argz_create_sep argz_insert argz_next argz_stringify])
])# AC_LTDL_FUNC_ARGZ

dnl Params are:
dnl $1 Name of variable to receive canonicalized filename
dnl $2 Non-canonical filename
AC_DEFUN([ROSE_CANON_SYMLINK], [
	$1=$2
	while test -h "$$1" ; do
		__rose_canon_symlink_base="`readlink "$$1"`"
		AS_SET_CATFILE([__rose_canon_symlink_full_path], ["`dirname "$$1"`"], ["$__rose_canon_symlink_base"])
		__rose_canon_symlink_dir="`dirname "$__rose_canon_symlink_full_path"`"
		__rose_canon_symlink_canon_dir="`cd "$__rose_canon_symlink_dir" && pwd`"
		AS_SET_CATFILE([$1], ["$__rose_canon_symlink_canon_dir"], ["`basename $__rose_canon_symlink_full_path`"])
	done
])

dnl $Id: compiler-ar.m4,v 1.2 2008/01/26 00:25:52 dquinlan Exp $

dnl Define variables that generalizes the command to generate a library.
dnl This is usually done with something like "ar ruv".  But ar does not
dnl work well with C++ because compiling may be done at link time, to
dnl instantiate templates.  In general, a command using the C++ compiler
dnl is preferred over ar.  The variables defined are aimed at literally
dnl replacing the "ar ruv" in the command "ar ruv libxyz.a *.o".
dnl BTNG.

AC_DEFUN([BTNG_CXX_AR],[
dnl Set the variables CXX_ID and CXX_VERSION.

# Determine what compiler we ar using.
# The library-building command depends on the compiler characteristics.
AC_REQUIRE([BTNG_INFO_CXX_ID])


# Create the command syntax such that they can substitute
# the generic case of using ar, i.e. ar ruv.
# This means that the libraries will be generated using
# "$CXX_STATIC_LIB_UPDATE libxyz.a *.o".
case "$CXX_ID" in
  # Each block here defines CXX_STATIC_LIB_UPDATE and CXX_SHARED_LIB_UPDATE
  # for a specific compiler.
  gnu)
    CXX_STATIC_LIB_UPDATE='${AR} ruv'
    # I tried 'gcc -o' for static libraries, but got unresolved symbols
    # and no library.  BTNG.
    case "$host_os" in
      sun*|solaris*)	CXX_SHARED_LIB_UPDATE='${CC} -shared -o' ;;
      # Note that CC is used instead of CXX if CXX is GNU compiler.
      # Assume that if CXX is g++, then CC had better be gcc.
      # For some reason, running "g++ -shared" on the Sun writes
      # a perfectly good file then due to an error on ld, removes
      # that file.  Maybe a future version will correct this.  BTNG
      *)		CXX_SHARED_LIB_UPDATE='${CXX} -shared -o' ;;
      # Manual says I should also include the compile flags such as
      # -fpic and -fPIC but this seems to be working right now and I
      # fear breaking it.  BTNG
    esac
  ;;
  sunpro)
    CXX_STATIC_LIB_UPDATE='${CXX} -xar -o'
    CXX_SHARED_LIB_UPDATE='${CXX} -G -o'
  ;;
  dec)
    CXX_STATIC_LIB_UPDATE='${AR} ruv'
    # I tried 'cxx -o' for static libraries, but got unresolved symbols
    # and no library.  BTNG.
    CXX_SHARED_LIB_UPDATE='${CXX} -shared -o'
  ;;
  kai)
    CXX_STATIC_LIB_UPDATE='${CXX} -o'
    CXX_SHARED_LIB_UPDATE='${CXX} -o'
    # The KAI compiler generates shared or static based on name of output file.
  ;;
  sgi)
    CXX_STATIC_LIB_UPDATE='${AR} ruv'
    CXX_SHARED_LIB_UPDATE='${CXX} -64 -shared -o'
  ;;
  ibm)
    CXX_STATIC_LIB_UPDATE='${AR} -r -u -v'
    # IBM does not provide a method for creating shared libraries.
  ;;
  *)
    # Set the default values.
    # (These generally do not work well when templates are involved.)
    CXX_STATIC_LIB_UPDATE='${AR} ruv'
    CXX_SHARED_LIB_UPDATE='${CXX} -o'
  ;;
esac


# Let user override.
AC_ARG_WITH(cxx_static_lib_update,
[  --with-cxx_static_lib_update=COMMAND
			Use COMMAND (not 'ar ruv') to make static C++ library.],
CXX_STATIC_LIB_UPDATE=$with_cxx_static_lib_update)
AC_ARG_WITH(cxx_shared_lib_update,
[  --with-cxx_shared_lib_update=COMMAND
			Use COMMAND (not 'ar ruv') to make shared C++ library.],
CXX_SHARED_LIB_UPDATE=$with_cxx_shared_lib_update)

])

dnl $Id: compiler-id.m4,v 1.2 2008/01/26 00:25:52 dquinlan Exp $

dnl Determines which compiler is being used.
dnl This check uses the compiler behavior when possible.
dnl For some compiler, we resort to a best guess,
dnl because we do not know a foolproof way to get the info.

dnl Much of the information used here came from the very
dnl helpful predef project (http://predef.sourceforge.net/).

dnl Simple wrappers to allow using BTNG_INFO_CXX_ID_NAMES and
dnl BTNG_INFO_CC_ID_NAMES without arguments.
dnl The names CC_ID and CC_VERSION are used for the C compiler id and version.
dnl The names CXX_ID and CXX_VERSION are used for the C++ compiler id and version.
AC_DEFUN([BTNG_INFO_CXX_ID],[
  BTNG_INFO_CXX_ID_NAMES(CXX_ID,CXX_VERSION)
])
AC_DEFUN([BTNG_INFO_CC_ID],[
  BTNG_INFO_CC_ID_NAMES(CC_ID,CC_VERSION)
])
AC_DEFUN([BTNG_INFO_CC_CXX_ID],[
  AC_REQUIRE([BTNG_INFO_CC_ID])
  AC_REQUIRE([BTNG_INFO_CXX_ID])
])


dnl BTNG_INFO_CXX_ID and BTNG_INFO_C_ID determine which C or C++ compiler
dnl is being used.
# Set the variables CXX_ID or C_ID as follows:
# Gnu		-> gnu
# SUNWspro	-> sunpro
# Dec		-> dec
# KCC		-> kai
# SGI		-> sgi
# IBM xlc	-> xlc


AC_DEFUN([BTNG_INFO_CXX_ID_NAMES],
dnl Arguments are:
dnl 1. Name of variable to set to the ID string.
dnl 2. Name of variable to set to the version number.
[
# Start macro BTNG_INFO_CXX_ID_NAMES
  AC_REQUIRE([AC_PROG_CXXCPP])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  BTNG_AC_LOG(CXXP is $CXX)
  BTNG_AC_LOG(CXXCPP is $CXXCPP)

  $1=unknown
  $2=unknown

dnl Do not change the following chain of if blocks into a case statement.
dnl We may eventually have a compiler that must be tested in a different
dnl method


  # Check if it is a Sun compiler.
  if test $$1 = unknown; then
    BTNG_AC_LOG(checking if $CXX is sunpro)
changequote(BEG,END)
    AC_EGREP_CPP(^0x[0-9]+,__SUNPRO_CC,
changequote([,])
      $1=sunpro
      # SUN compiler defines __SUNPRO_CC to the version number.
      echo __SUNPRO_CC > conftest.C
      $2=`${CXXCPP} conftest.C | sed -n 2p`
      rm -f conftest.C
    )
  fi


  # Check if it is a GNU compiler.
  if test $$1 = unknown; then
    BTNG_AC_LOG(checking if $CXX is gnu)
    AC_EGREP_CPP(^yes,
#ifdef __GNUC__
yes;
#endif
,
    $1=gnu
    # GNU compilers output version number with option --version.
    # Alternatively, it also defines __GNUC__, GNUC_MINOR__ and
    # __GNUC_PATCHLEVEL__
    [[$2=`$CXX --version | sed -e 's/[^0-9]\{0,\}\([^ ]\{1,\}\).\{0,\}/\1/' -e 1q`]]
    )
  fi


  # Check if it is a DEC compiler.
  if test $$1 = unknown; then
    BTNG_AC_LOG(checking if $CXX is dec)
    AC_EGREP_CPP(^1,__DECCXX,
      $1=dec
      # DEC compiler defines __DECCXX_VER to the version number.
      echo __DECCXX_VER > conftest.C
      $2=`${CXXCPP} conftest.C | sed -n 2p`
      rm -f conftest.C
    )
  fi


  # Check if it is a KAI compiler.
  if test $$1 = unknown; then
    BTNG_AC_LOG(checking if $CXX is kai)
    AC_EGREP_CPP(^1,__KCC,
      $1=kai
      # KCC compiler defines __KCC_VERSION to the version number.
      echo __KCC_VERSION > conftest.C
      $2=`${CXXCPP} conftest.C | sed -n 2p`
      rm -f conftest.C
    )
  fi


  # Check if it is a SGI compiler.
  if test $$1 = unknown; then
    BTNG_AC_LOG(checking if $CXX is sgi)
    AC_EGREP_CPP(^1,__sgi,
      $1=sgi
      # SGI compiler defines _COMPILER_VERSION to the version number.
      echo _COMPILER_VERSION > conftest.C
      $2=`${CXXCPP} conftest.C | sed /^\\#/d`
      rm -f conftest.C
    )
  fi


  # Check if it is a IBM compiler.
  if test $$1 = unknown; then
    BTNG_AC_LOG(checking if $CXX is xlc)
    AC_EGREP_CPP(^yes,
#ifdef __xlC__
yes;
#endif
,
    $1=xlc
    # IBM compiler defines __xlC__ to the version number.
    echo __xlC__ > conftest.C
    $2=`${CXXCPP} conftest.C | sed /^\\#/d`
    rm -f conftest.C
    )
  fi


  AC_LANG_RESTORE
  BTNG_AC_LOG_VAR(CXX_ID CXX_VERSION)
# End macro BTNG_INFO_CXX_ID_NAMES
])





AC_DEFUN([BTNG_INFO_CC_ID_NAMES],
dnl Arguments are:
dnl 1. Name of variable to set to the ID string.
dnl 2. Name of variable to set to the version number.
[
# Start macro BTNG_INFO_CC_ID_NAMES
  AC_REQUIRE([AC_PROG_CPP])
  AC_LANG_SAVE
  AC_LANG_C
  BTNG_AC_LOG(CC is $CC)
  BTNG_AC_LOG(CPP is $CPP)

  $1=unknown
  $2=unknown

dnl Do not change the following chain of if blocks into a case statement.
dnl We may eventually have a compiler that must be tested in a different
dnl method


  # Check if it is a Sun compiler.
  if test $$1 = unknown; then
    BTNG_AC_LOG(checking if $CC is sunpro)
changequote(BEG,END)
    AC_EGREP_CPP(^ 0x[0-9]+,__SUNPRO_C,
changequote([,])
      $1=sunpro
      # SUN compiler defines __SUNPRO_C to the version number.
      echo __SUNPRO_C > conftest.c
      $2=`${CPP} ${CPPFLAGS} conftest.c | sed -n -e 's/^ //' -e 2p`
      rm -f conftest.c
    )
  fi


  # Check if it is a GNU compiler.
  if test $$1 = unknown; then
    BTNG_AC_LOG(checking if $CC is gnu)
    AC_EGREP_CPP(^yes,
#ifdef __GNUC__
yes;
#endif
,
    $1=gnu
    [[$2=`$CC --version | sed -e 's/[^0-9]\{0,\}\([^ ]\{1,\}\).\{0,\}/\1/' -e 1q`]]
    )
  fi


  # Check if it is a DEC compiler.
  if test $$1 = unknown; then
    BTNG_AC_LOG(checking if $CC is dec)
    AC_EGREP_CPP(^ 1,__DECC,
      $1=dec
      # DEC compiler defines __DECC_VER to the version number.
      echo __DECC_VER > conftest.c
      $2=`${CPP} ${CPPFLAGS} conftest.c | sed -n -e 's/^ //' -e 2p`
      rm -f conftest.c
    )
  fi


  # Check if it is a KAI compiler.
  if test $$1 = unknown; then
    BTNG_AC_LOG(checking if $CC is kai)
    AC_EGREP_CPP(^1,__KCC,
      $1=kai
      # KCC compiler defines __KCC_VERSION to the version number.
      echo __KCC_VERSION > conftest.c
      $2=`${CPP} ${CPPFLAGS} conftest.c | sed -n 2p`
      rm -f conftest.c
    )
  fi


  # Check if it is a SGI compiler.
  if test $$1 = unknown; then
    BTNG_AC_LOG(checking if $CC is sgi)
    AC_EGREP_CPP(^1,__sgi,
      $1=sgi
      # SGI compiler defines _COMPILER_VERSION to the version number.
      echo _COMPILER_VERSION > conftest.c
      $2=`${CPP} ${CPPFLAGS} conftest.c | sed /^\\#/d`
      rm -f conftest.c
    )
  fi


  # Check if it is a IBM compiler.
  if test $$1 = unknown; then
    BTNG_AC_LOG(checking if $CC is xlc)
    if echo "$host_os" | grep "aix" >/dev/null ; then
      # The wretched IBM shell does not eval correctly,
      # so we have to help it with a pre-eval eval statement.
      ac_cpp=`eval "echo $ac_cpp"`
      save_ac_cpp=$ac_cpp
      BTNG_AC_LOG(ac_cpp is temporarily set to $ac_cpp)
    else
      save_ac_cpp=
    fi
    BTNG_AC_LOG(ac_cpp is $ac_cpp)
    AC_EGREP_CPP(^yes,
#ifdef __xlC__
yes;
#endif
,
    $1=xlc
    # IBM compiler defines __xlC__ to the version number.
    echo __xlC__ > conftest.C
    $2=`${CPP} conftest.C | sed /^\\#/d`
    rm -f conftest.C
    )
    test "$save_ac_cpp" && ac_cpp=$save_ac_cpp
    BTNG_AC_LOG(ac_cpp is restored to $ac_cpp)
  fi


  AC_LANG_RESTORE
  BTNG_AC_LOG_VAR(CC_ID CC_VERSION)
# End macro BTNG_INFO_CC_ID_NAMES
])

AC_DEFUN([BTNG_AC_LOG],[echo "configure:__oline__:" $1 >&AC_FD_CC])

AC_DEFUN([BTNG_AC_LOG_VAR],[
dnl arg1 is list of variables to log.
dnl arg2 (optional) is a label.
dnl
dnl This macro makes code that write out at configure time
dnl label: x is '...'
dnl if x is set and
dnl label: x is unset
dnl otherwise.
define([btng_log_label],ifelse($2,,,[$2: ]))
btng_log_vars="$1"
for btng_log_vars_index in $btng_log_vars ; do
  eval "test \"\${${btng_log_vars_index}+set}\" = set"
  if test $? = 0; then
    btng_log_vars_value="'`eval echo \\${$btng_log_vars_index}`'";
  else
    btng_log_vars_value="unset";
  fi
  BTNG_AC_LOG("btng_log_label$btng_log_vars_index is $btng_log_vars_value");
dnl
dnl This is a shorter version, but it does not work for some Bourne shells
dnl due to misinterpretation of the multiple backslashes
dnl BTNG_AC_LOG("btng_log_label$btng_log_vars_index is `eval if test \\\"\$\{$btng_log_vars_index+set\}\\\"\; then echo \\\""'"\$\{$btng_log_vars_index\}"'"\\\"\; else echo 'unset'\; fi`")
done
undefine([btng_log_label])
])


dnl PKG_CHECK_MODULES(GSTUFF, gtk+-2.0 >= 1.3 glib = 1.3.4, action-if, action-not)
dnl defines GSTUFF_LIBS, GSTUFF_CFLAGS, see pkg-config man page
dnl also defines GSTUFF_PKG_ERRORS on error
AC_DEFUN([PKG_CHECK_MODULES], [
  succeeded=no

  if test -z "$PKG_CONFIG"; then
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  fi

  if test "$PKG_CONFIG" = "no" ; then
     echo "*** The pkg-config script could not be found. Make sure it is"
     echo "*** in your path, or set the PKG_CONFIG environment variable"
     echo "*** to the full path to pkg-config."
     echo "*** Or see http://www.freedesktop.org/software/pkgconfig to get pkg-config."
  else
     PKG_CONFIG_MIN_VERSION=0.9.0
     if $PKG_CONFIG --atleast-pkgconfig-version $PKG_CONFIG_MIN_VERSION; then
        AC_MSG_CHECKING(for $2)

        if $PKG_CONFIG --exists "$2" ; then
            AC_MSG_RESULT(yes)
            succeeded=yes

            AC_MSG_CHECKING($1_CFLAGS)
            $1_CFLAGS=`$PKG_CONFIG --cflags "$2"`
            AC_MSG_RESULT($$1_CFLAGS)

            AC_MSG_CHECKING($1_LIBS)
            $1_LIBS=`$PKG_CONFIG --libs "$2"`
            AC_MSG_RESULT($$1_LIBS)
        else
            $1_CFLAGS=""
            $1_LIBS=""
            ## If we have a custom action on failure, don't print errors, but 
            ## do set a variable so people can do so.
            $1_PKG_ERRORS=`$PKG_CONFIG --errors-to-stdout --print-errors "$2"`
            ifelse([$4], ,echo $$1_PKG_ERRORS,)
        fi

        AC_SUBST($1_CFLAGS)
        AC_SUBST($1_LIBS)
     else
        echo "*** Your version of pkg-config is too old. You need version $PKG_CONFIG_MIN_VERSION or newer."
        echo "*** See http://www.freedesktop.org/software/pkgconfig"
     fi
  fi

  if test $succeeded = yes; then
     ifelse([$3], , :, [$3])
  else
     ifelse([$4], , AC_MSG_ERROR([Library requirements ($2) not met; consider adjusting the PKG_CONFIG_PATH environment variable if your libraries are in a nonstandard prefix so pkg-config can find them.]), [$4])
  fi
])



AC_DEFUN([SETUP_EDG], [

# SAGE_VAR_INCLUDES_AND_LIBS
AC_DEFINE([SAGE_TRANSFORM],[1],[Control use of SAGE within EDG.])

# AC_AIX

# Checks for programs.

# LAC_SAGE_PATH
# LAC_COMPILERS_ARGS
# LAC_CXX_TYPE
# LAC_CXX_VERSION
# LAC_COMPILERS_TARGET(no)

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h sys/time.h unistd.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# This screws up the C++ compiler, it tests to see if the C compiler supports the inline keyword
# (usually not) and then defines inline to be empty which interferes with the processing of inline
# code used with the C++ compiler. So comment it out and handle it within EDG if it is an issue.
# This is also listed as an obsolete construct within the new autoconf 2.52.
# AC_C_INLINE

AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_STRUCT_TM

# AC_DIAGNOSE([obsolete],[instead of using `AC_LANG', `AC_LANG_SAVE',
# and `AC_LANG_RESTORE', you should use `AC_LANG_PUSH' and `AC_LANG_POP'.])

AC_C_CHAR_UNSIGNED
AC_C_LONG_DOUBLE

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(void *)
AC_CHECK_SIZEOF(long double)
AC_CHECK_SIZEOF(long long)

# Checks for library functions.
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_SETVBUF_REVERSED
AC_TYPE_SIGNAL
AC_FUNC_STRCOLL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(getcwd getwd mktime strcspn strerror strftime strspn strstr strtod strtol strtoul)

# CC++ specific macros
OBJ=".o"
AC_SUBST(OBJ)

EXE=""
AC_SUBST(EXE)

AC_ARG_ENABLE(purify,
[  --enable-purify use memory managment that purify can understand],
[if test $enable_purify = yes; then
     AC_DEFINE([USING_PURIFY],[],[Support for PURIFY debugging software.])
 fi])

# AC_ARG_ENABLE(ccpp,
# [  --disable-ccpp don't support CC++ extensions],
# [if test $enable_cc++ = yes; then
#      AC_DEFINE([CCPP_EXTENSIONS_ALLOWED],[],[Support for CC++.])
#  fi],[AC_DEFINE([CCPP_EXTENSIONS_ALLOWED],[],[Support for CC++.])])

consistency_checking_define_value=0
AC_ARG_ENABLE(checking,
[  --disable-checking don't do EDG specific consistency checking in parser],
[if test $enable_checking = yes; then
    # AC_DEFINE([CHECKING],[1],[Internal consistency tests.])
      consistency_checking_define_value=0
 fi],[
 # AC_DEFINE([CHECKING],[0],[Internal consistency tests.])
   consistency_checking_define_value=1
])

# DQ (1/6/2009): Made the comment more clear that this is an EDG feature.
AC_DEFINE_UNQUOTED([CHECKING],[$consistency_checking_define_value],[Internal EDG specific consistency tests.])

AC_ARG_ENABLE(stand-alone,
	[  --enable-stand-alone compile standalone edgcpfe],
	[standalone="$enableval"],
	[standalone="no"]
)

AC_ARG_ENABLE(cp-backend,
	[  --enable-cp-backend generate c++ code as output],
	[cp_backend="$enableval"],
	[cp_backend="no"]
)

AC_ARG_ENABLE(sage-backend,
	[  --enable-sage-backend generate sage++ tree],
	[sage_backend="$enableval"],
	[sage_backend="no"]
)

if test "$cp_backend" = "yes" -a "$sage_backend" = "yes"; then 
  AC_MSG_ERROR(Cannot have both sage and c++ backend)
fi

# Initialized values
sage_backend_defined_value=0
cp_backend_defined_value=0
build_parser_as_lib_defined_value=0

build_stand_alone_C_generator_defined_value=0
build_stand_alone_CP_generator_defined_value=0
build_stand_alone_SAGE_generator_defined_value=0

if test "$cp_backend" = "yes"; then 
  # AC_DEFINE([BACK_END_IS_CP_GEN_BE],1,[Define use of C++ generating backend (not used except for debugging).])
  GEN_BE="cp_gen_be$OBJ"
  cp_backend_defined_value=1
else
  # AC_DEFINE([BACK_END_IS_SAGE_GEN_BE],1,[Define use of SAGE specific backend to generate the SAGE AST.])
  GEN_BE="sage_gen_be$OBJ"
  sage_backend_defined_value=1
  if test "$standalone"="no"; then
     # AC_DEFINE([BUILDING_PARSER_AS_LIB],[1],[Build parser as a standalone tool (not connected to SAGE).])
     build_parser_as_lib_defined_value=1
  fi
fi

AC_SUBST(GEN_BE)

])












dnl
dnl ROSE_FLAG_CXX set the C++ compiler flags.
dnl This macro modifies CXXFLAGS.  Secondarily,
dnl it defines variables such as CXX_DEBUG, CXX_OPTIMIZE, etc.
dnl
AC_DEFUN([ROSE_FLAG_CXX_OPTIONS], [
# Begin macro ROSE_FLAG_CXX.

# We may be asked to guess the correct flags,
# so we must know our host and the compiler used.
AC_REQUIRE([AC_CANONICAL_HOST])
AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([BTNG_INFO_CXX_ID])

dnl *********************************************************************
dnl * Set up the Preprocessor -D options CXXDEBUG and ARCH_DEFINES
dnl *********************************************************************
AC_ARG_WITH(CXX_DEBUG, [  --with-CXX_DEBUG=ARG    manually set the C++ compiler debug flags
                           to ARG (leave blank to choose automatically)])
if test "$with_CXX_DEBUG" = yes; then
  # CXX_DEBUG was activated but not specified, so set it.
  case $CXX in
    *) CXX_DEBUG='-g' ;;
  esac
elif test "$with_CXX_DEBUG" = no; then
  CXX_DEBUG=''
else
  CXX_DEBUG=$with_CXX_DEBUG
fi
AC_SUBST(CXX_DEBUG)
if test -n "$CXX_DEBUG"; then CXXFLAGS="$CXXFLAGS $CXX_DEBUG"; fi

# echo "Am I set: CXX_DEBUG= $CXX_DEBUG"
# echo "CXXFLAGS currently set to $CXXFLAGS"

echo "Setup CXX_OPTIMIZE"

dnl *********************************************************************
dnl * Set the C++ compiler optimization flags in CXXOPT
dnl *********************************************************************
dnl This should use the AC_ARG_ENABLE not AC_ARC_WITH!
AC_ARG_WITH(CXX_OPTIMIZE, [  --with-CXX_OPTIMIZE=ARG   manually set the C++ compiler optimization
                           to ARG (leave blank to choose automatically)])
if test "$with_CXX_OPTIMIZE" = yes; then
  # CXX_OPTIMIZE was activated but not specified, so set it.
  case $CXX in
    KCC | mpKCC) CXX_OPTIMIZE='-g +K0 --no_exceptions --no_rtti --keep_gen_c' ;;
    *) CXX_OPTIMIZE='' ;;
  esac
elif test "$with_CXX_OPTIMIZE" = no; then
  CXX_OPTIMIZE=''
else
  CXX_OPTIMIZE=$with_CXX_OPTIMIZE
fi
AC_SUBST(CXX_OPTIMIZE)
if test "$CXX_OPTIMIZE"; then CXXFLAGS="$CXXFLAGS $CXX_OPTIMIZE"; fi

dnl *********************************************************************
dnl * Set the C++ compiler flags in CXX_WARNINGS
dnl *********************************************************************
dnl This should use the AC_ARG_ENABLE not AC_ARC_WITH!

echo "Setup CXX_WARNING CXX = $CXX"

AC_ARG_WITH(CXX_WARNINGS, [  --with-CXX_WARNINGS=ARG   manually set the C++ compiler warning flags
                           to ARG (leave blank to choose automatically)])
if test "$with_CXX_WARNINGS" = yes; then
  # CXX_WARNINGS was activated but not specified, so set it.
  case $CXX in
    g++)
      CXX_WARNINGS="-Wall -Wstrict-prototypes -Wmissing-prototypes"
      ;;
    icpc)
    # For Intel turn on 64bit migration/portability warnings
      CXX_WARNINGS="-w1 -Wall -Wcheck -Wp64"
      ;;
    KCC | mpKCC)
      CXX_WARNINGS="--for_init_diff_warning --new_for_init -w"
      ;;
    CC)
      case $host_os in
        solaris*| sun4*)
          CXX_WARNINGS=""
    esac
    ;;
  esac
elif test "$with_CXX_WARNINGS" = no; then
  CXX_WARNINGS=''
# DQ (1/15/2007): turn on warnings by default.
  case $CC in
    g++)
    # CXX_WARNINGS="-Wall -Wstrict-prototypes -Wmissing-prototypes"
      CXX_WARNINGS="-Wall"
      ;;
    icpc)
    # For Intel turn on 64bit migration/portability warnings
      CXX_WARNINGS="-w1 -Wall -Wcheck -Wp64"
      ;;
    "KCC --c" | mpKCC)
      CXX_WARNINGS="--for_init_diff_warning --new_for_init -w"
      ;;
    CC)
      case $host_os in
        solaris*| sun4*)
          CXX_WARNINGS=""
    esac
    ;;
  esac
else
  CXX_WARNINGS=$with_CXX_WARNINGS
fi
AC_SUBST(CXX_WARNINGS)
if test "$CXX_WARNINGS"; then CXXFLAGS="$CXXFLAGS $CXX_WARNINGS"; fi

# echo "debugging: CXX_ID               = $CXX_ID"
# echo "debugging: CXX_VERSION          = $CXX_VERSION"
# echo "debugging: CXX_TEMPLATE_OBJECTS = $CXX_TEMPLATE_OBJECTS"

CXX_TEMPLATE_REPOSITORY_PATH='$(top_builddir)/src'
CXX_TEMPLATE_OBJECTS= # A bunch of Makefile.ams use this

AC_SUBST(CXX_TEMPLATE_REPOSITORY_PATH)
AC_SUBST(CXX_TEMPLATE_OBJECTS)

dnl if test "$CXX_TEMPLATES"; then CXXFLAGS="$CXXFLAGS $CXX_TEMPLATES"; fi

# End macro ROSE_FLAG_CXX_OPTIONS.
])


dnl
dnl ROSE_FLAG_CC set the C compiler flags.
dnl This macro modifies CFLAGS.  Secondarily,
dnl it defines variables such as C_DEBUG, C_OPTIMIZE, etc.
dnl
AC_DEFUN([ROSE_FLAG_C_OPTIONS], [
# Begin macro ROSE_FLAG_C_OPTIONS.

# We may be asked to guess the correct flags,
# so we must know our host and the compiler used.
AC_REQUIRE([AC_CANONICAL_HOST])
AC_REQUIRE([AC_PROG_CC])
# AC_REQUIRE([BTNG_INFO_CXX_ID])

dnl *********************************************************************
dnl * Set up the C compiler options C_DEBUG
dnl *********************************************************************
AC_ARG_WITH(C_DEBUG, [  --with-C_DEBUG=ARG    manually set the C compiler debug flags
                           to ARG (leave blank to choose automatically)])
if test "$with_C_DEBUG" = yes; then
  # C_DEBUG was activated but not specified, so set it.
  case $CC in
    *) C_DEBUG='-g' ;;
  esac
elif test "$with_C_DEBUG" = no; then
  C_DEBUG=''
else
  C_DEBUG=$with_C_DEBUG
fi
AC_SUBST(C_DEBUG)
if test -n "$C_DEBUG"; then CFLAGS="$CFLAGS $C_DEBUG"; fi

# echo "Am I set: C_DEBUG= $C_DEBUG"


echo "Setup C_OPTIMIZE"

dnl *********************************************************************
dnl * Set the C compiler optimization flags in C_OPTIMIZE
dnl *********************************************************************
dnl This should use the AC_ARG_ENABLE not AC_ARC_WITH!
AC_ARG_WITH(C_OPTIMIZE, [  --with-C_OPTIMIZE=ARG   manually set the C compiler optimization
                           to ARG (leave blank to choose automatically)])
if test "$with_C_OPTIMIZE" = yes; then
  # C_OPTIMIZE was activated but not specified, so set it.
  case $CC in
    "KCC --c" | mpKCC) C_OPTIMIZE='-g +K0 --no_exceptions --no_rtti --keep_gen_c' ;;
    *) C_OPTIMIZE='' ;;
  esac
elif test "$with_C_OPTIMIZE" = no; then
  C_OPTIMIZE=''
else
  C_OPTIMIZE=$with_C_OPTIMIZE
fi
AC_SUBST(C_OPTIMIZE)
if test "$C_OPTIMIZE"; then CFLAGS="$CFLAGS $C_OPTIMIZE"; fi

dnl *********************************************************************
dnl * Set the C++ compiler flags in C_WARNINGS
dnl *********************************************************************
dnl This should use the AC_ARG_ENABLE not AC_ARC_WITH!

echo "Setup C_WARNINGS CC = $CC"

AC_ARG_WITH(C_WARNINGS, [  --with-C_WARNINGS=ARG   manually set the C compiler warning flags
                           to ARG (leave blank to choose automatically)])
if test "$with_C_WARNINGS" = yes; then
  # C_WARNINGS was activated but not specified, so set it.
  case $CC in
    gcc)
      C_WARNINGS="-Wall -Wstrict-prototypes -Wmissing-prototypes"
      ;;
    icc)
    # For Intel turn on 64bit migration/portability warnings
      C_WARNINGS="-w -Wall -Wcheck -Wp64"
      ;;
    "KCC --c" | mpKCC)
      C_WARNINGS="--for_init_diff_warning --new_for_init -w"
      ;;
    cc)
      case $host_os in
        solaris*| sun4*)
          C_WARNINGS=""
    esac
    ;;
  esac
elif test "$with_C_WARNINGS" = no; then
  C_WARNINGS=''
# DQ (1/15/2007): turn on warnings by default.
  case $CC in
    gcc)
    # C_WARNINGS="-Wall -Wstrict-prototypes -Wmissing-prototypes"
      C_WARNINGS="-Wall"
      ;;
    icc)
    # For Intel turn on 64bit migration/portability warnings
      C_WARNINGS="-w1 -Wall -Wcheck -Wp64"
      ;;
    "KCC --c" | mpKCC)
      C_WARNINGS="--for_init_diff_warning --new_for_init -w"
      ;;
    cc)
      case $host_os in
        solaris*| sun4*)
          C_WARNINGS=""
    esac
    ;;
  esac
else
  C_WARNINGS=$with_C_WARNINGS
fi
AC_SUBST(C_WARNINGS)
if test "$C_WARNINGS"; then CFLAGS="$CFLAGS $C_WARNINGS"; fi

# End macro ROSE_FLAG_C_OPTIONS.
])


dnl *****************************************************************************
dnl * Specify the C/C++ header files used by EDG (should match back-end compiler)
dnl *****************************************************************************

AC_DEFUN([ROSE_CXX_HEADER_OPTIONS], [
# Begin macro ROSE_CXX_HEADER_OPTIONS.

# DQ (1/15/2007): This is an older discontinued option (when we used to package a 
# set of system header files for use by ROSE.
# This macro is useful if there is a prepared set of system header files that should be
# used instead of the ones that the ROSE configure process will set up automatically.
# such header are specified, they should be put into the srcdir.

echo "Setup ROSE_CXX_HEADERS_DIR"
# echo "GCC_MAJOR = $GCC_MAJOR"

AC_ARG_WITH(GNU_CXX_HEADERS, [  --with-GNU_CXX_HEADERS            use the the directory of included GNU header files in the ROSE source tree])

# For the g++ 3.x compilers we can generally use the 2.96 header files. However
# it does not always work (using namespace std requires a previously defined 
# namespace std which is in the 3.x headers but NOT in the 2.96 headers).
if test "$with_GNU_CXX_HEADERS" = yes; then
# if ((test "$with_GNU_HEADERS" = yes) || (test "$GCC_MAJOR" = 3)); then
  # GNU_HEADERS was activated but not specified, so set it.
    echo "with-GNU_CXX_HEADERS = yes (use default GNU headers (preprocessed) distributed with ROSE)"
    ROSE_CXX_HEADERS_DIR="${srcdir}/GNU_CXX_HEADERS"
else
    echo "with-GNU_CXX_HEADERS = no"
fi

# echo "with-GNU_CXX_HEADERS = $with_GNU_CXX_HEADERS"
# echo "ROSE_CXX_HEADERS_DIR = $ROSE_CXX_HEADERS_DIR"

AC_ARG_WITH(ROSE_INTERNAL_HEADERS, [  --with-ROSE_INTERNAL_HEADERS=ARG  manually set the directory of header files used internally)])
if test "$with_ROSE_INTERNAL_HEADERS" = ""; then
    echo "with-ROSE_INTERNAL_HEADERS not set"
else
    echo "with-ROSE_INTERNAL_HEADERS = user defined directory"
    ROSE_CXX_HEADERS_DIR=$with_ROSE_INTERNAL_HEADERS
fi

# echo "with-ROSE_INTERNAL_HEADERS = $with_ROSE_INTERNAL_HEADERS"
# echo "ROSE_CXX_HEADERS_DIR = $ROSE_CXX_HEADERS_DIR"

export ROSE_CXX_HEADERS_DIR
# AC_SUBST(ROSE_CXX_HEADERS_DIR)

])

dnl *****************************************************************************
dnl * Specify the C/C++ header files used by EDG (should match back-end compiler)
dnl *****************************************************************************

AC_DEFUN([ROSE_C_HEADER_OPTIONS], [
# Begin macro ROSE_C_HEADER_OPTIONS.

# DQ (1/15/2007): This is an older discontinued option (when we used to package a 
# set of system header files for use by ROSE.
# This macro is useful if there is a prepared set of system header files that should be
# used instead of the ones that the ROSE configure process will set up automatically.
# such header are specified, they should be put into the srcdir.

echo "Setup ROSE_C_HEADERS_DIR"
# echo "GCC_MAJOR = $GCC_MAJOR"

AC_ARG_WITH(GNU_C_HEADERS, [  --with-GNU_C_HEADERS              use the the directory of included GNU header files in the ROSE source tree])

# For the g++ 3.x compilers we can generally use the 2.96 header files. However
# it does not always work (using namespace std requires a previously defined 
# namespace std which is in the 3.x headers but NOT in the 2.96 headers).
if test "$with_GNU_C_HEADERS" = yes; then
# if ((test "$with_GNU_HEADERS" = yes) || (test "$GCC_MAJOR" = 3)); then
  # GNU_HEADERS was activated but not specified, so set it.
    echo "with-GNU_C_HEADERS = yes (use default GNU headers (preprocessed) distributed with ROSE)"
    ROSE_C_HEADERS_DIR="${srcdir}/GNU_C_HEADERS"
else
    echo "with-GNU_C_HEADERS = no"
fi

# echo "with-GNU_C_HEADERS = $with_GNU_C_HEADERS"
# echo "ROSE_C_HEADERS_DIR = $ROSE_C_HEADERS_DIR"

AC_ARG_WITH(ROSE_INTERNAL_HEADERS, [  --with-ROSE_INTERNAL_HEADERS=ARG  manually set the directory of header files used internally)])
if test "$with_ROSE_INTERNAL_HEADERS" = ""; then
    echo "with-ROSE_INTERNAL_HEADERS not set"
else
    echo "with-ROSE_INTERNAL_HEADERS = user defined directory"
    ROSE_C_HEADERS_DIR=$with_ROSE_INTERNAL_HEADERS
fi

# echo "with-ROSE_INTERNAL_HEADERS = $with_ROSE_INTERNAL_HEADERS"
# echo "ROSE_C_HEADERS_DIR = $ROSE_C_HEADERS_DIR"

export ROSE_C_HEADERS_DIR
# AC_SUBST(ROSE_C_HEADERS_DIR)

])



AC_DEFUN([ROSE_SUPPORT_LONG_MAKE_CHECK_RULE], [
# Begin macro ROSE_TESTING_OPTIONS.

# This macro defines short and long versions of internla testing
# Most users just require a subset of tests to verify general 
# working behavior.  Current internal ROSE testing can however
# be quite long (particularly on older machines).  Current tests
# can be run with full levels of parallelism for multi-core or
# parallel architectures, significantly reducing the time required
# to test a distribution of ROSE.  The short tests are also used
# to test the automated "make distcheck" process for building 
# distributions.

AC_ARG_ENABLE(ROSE_LONG_MAKE_CHECK_RULE, [  --with-ROSE_LONG_MAKE_CHECK_RULE=yes  specify longer internal testsing by "make check" rule)])
if test "$with_ROSE_LONG_MAKE_CHECK_RULE" = "yes"; then
    echo "with_ROSE_LONG_MAKE_CHECK_RULE set (long testing used for \"make check\" rule)"
else
    echo "with_ROSE_LONG_MAKE_CHECK_RULE not set (short testing used by default)"
fi

])

#
# SYNOPSIS
#
#   AC_PROG_SWIG([major.minor.micro])
#
# DESCRIPTION
#
#   This macro searches for a SWIG installation on your system. If
#   found you should call SWIG via $(SWIG). You can use the optional
#   first argument to check if the version of the available SWIG is
#   greater than or equal to the value of the argument. It should have
#   the format: N[.N[.N]] (N is a number between 0 and 999. Only the
#   first N is mandatory.)
#
#   If the version argument is given (e.g. 1.3.17), AC_PROG_SWIG checks
#   that the swig package is this version number or higher.
#
#   In configure.in, use as:
#
#     AC_PROG_SWIG(1.3.17)
#     SWIG_ENABLE_CXX
#     SWIG_MULTI_MODULE_SUPPORT
#     SWIG_PYTHON
#
# LAST MODIFICATION
#
#   2006-10-22
#
# COPYLEFT
#
#   Copyright (c) 2006 Sebastian Huber <sebastian-huber@web.de>
#   Copyright (c) 2006 Alan W. Irwin <irwin@beluga.phys.uvic.ca>
#   Copyright (c) 2006 Rafael Laboissiere <rafael@laboissiere.net>
#   Copyright (c) 2006 Andrew Collier <colliera@ukzn.ac.za>
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 2 of the
#   License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
#   02111-1307, USA.
#
#   As a special exception, the respective Autoconf Macro's copyright
#   owner gives unlimited permission to copy, distribute and modify the
#   configure scripts that are the output of Autoconf when processing
#   the Macro. You need not follow the terms of the GNU General Public
#   License when using or distributing such scripts, even though
#   portions of the text of the Macro appear in them. The GNU General
#   Public License (GPL) does govern all other use of the material that
#   constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the
#   Autoconf Macro released by the Autoconf Macro Archive. When you
#   make and distribute a modified version of the Autoconf Macro, you
#   may extend this special exception to the GPL to apply to your
#   modified version as well.

AC_DEFUN([AC_PROG_SWIG],[
        AC_PATH_PROG([SWIG],[swig])
        if test -z "$SWIG" ; then
                AC_MSG_WARN([cannot find 'swig' program. You should look at http://www.swig.org])
                SWIG='echo "Error: SWIG is not installed. You should look at http://www.swig.org" ; false'
        elif test -n "$1" ; then
                AC_MSG_CHECKING([for SWIG version])
                [swig_version=`$SWIG -version 2>&1 | grep 'SWIG Version' | sed 's/.*\([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*/\1/g'`]
                AC_MSG_RESULT([$swig_version])
                if test -n "$swig_version" ; then
                        # Calculate the required version number components
                        [required=$1]
                        [required_major=`echo $required | sed 's/[^0-9].*//'`]
                        if test -z "$required_major" ; then
                                [required_major=0]
                        fi
                        [required=`echo $required | sed 's/[0-9]*[^0-9]//'`]
                        [required_minor=`echo $required | sed 's/[^0-9].*//'`]
                        if test -z "$required_minor" ; then
                                [required_minor=0]
                        fi
                        [required=`echo $required | sed 's/[0-9]*[^0-9]//'`]
                        [required_patch=`echo $required | sed 's/[^0-9].*//'`]
                        if test -z "$required_patch" ; then
                                [required_patch=0]
                        fi
                        # Calculate the available version number components
                        [available=$swig_version]
                        [available_major=`echo $available | sed 's/[^0-9].*//'`]
                        if test -z "$available_major" ; then
                                [available_major=0]
                        fi
                        [available=`echo $available | sed 's/[0-9]*[^0-9]//'`]
                        [available_minor=`echo $available | sed 's/[^0-9].*//'`]
                        if test -z "$available_minor" ; then
                                [available_minor=0]
                        fi
                        [available=`echo $available | sed 's/[0-9]*[^0-9]//'`]
                        [available_patch=`echo $available | sed 's/[^0-9].*//'`]
                        if test -z "$available_patch" ; then
                                [available_patch=0]
                        fi
                        if test $available_major -ne $required_major \
                                -o $available_minor -ne $required_minor \
                                -o $available_patch -lt $required_patch ; then
                                AC_MSG_WARN([SWIG version >= $1 is required.  You have $swig_version.  You should look at http://www.swig.org])
                                SWIG='echo "Error: SWIG version >= $1 is required.  You have '"$swig_version"'.  You should look at http://www.swig.org" ; false'
                        else
                                AC_MSG_NOTICE([SWIG executable is '$SWIG'])
                                SWIG_LIB=`$SWIG -swiglib`
                                AC_MSG_NOTICE([SWIG library directory is '$SWIG_LIB'])
                        fi
                else
                        AC_MSG_WARN([cannot determine SWIG version])
                        SWIG='echo "Error: Cannot determine SWIG version.  You should look at http://www.swig.org" ; false'
                fi
        fi
        AC_SUBST([SWIG_LIB])
])

#
# SYNOPSIS
#
#   AX_BOOST_BASE([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the Boost C++ libraries of a particular version (or newer)
#
#   If no path to the installed boost library is given the macro
#   searchs under /usr, /usr/local, /opt and /opt/local and evaluates
#   the $BOOST_ROOT environment variable. Further documentation is
#   available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_CPPFLAGS) / AC_SUBST(BOOST_LDFLAGS)
#
#   And sets:
#
#     HAVE_BOOST
#
# LAST MODIFICATION
#
#   2008-03-24
#
# COPYLEFT
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_BOOST_BASE],
[
AC_ARG_WITH([boost],
	AS_HELP_STRING([--with-boost@<:@=DIR@:>@], [use boost (default is yes) - it is possible to specify the root directory for boost (optional)]),
	[
    if test "$withval" = "no"; then
		want_boost="no"
    elif test "$withval" = "yes"; then
        want_boost="yes"
        ac_boost_path=""
    else
	    want_boost="yes"
        ac_boost_path="$withval"
	fi
    ],
    [want_boost="yes"])


AC_ARG_WITH([boost-libdir],
        AS_HELP_STRING([--with-boost-libdir=LIB_DIR],
        [Force given directory for boost libraries. Note that this will overwrite library path detection, so use this parameter only if default library detection fails and you know exactly where your boost libraries are located.]),
        [
        if test -d $withval
        then
                ac_boost_lib_path="$withval"
        else
                AC_MSG_ERROR(--with-boost-libdir expected directory name)
        fi
        ],
        [ac_boost_lib_path=""]
)

if test "x$want_boost" = "xyes"; then
	boost_lib_version_req=ifelse([$1], ,1.20.0,$1)
	boost_lib_version_req_shorten=`expr $boost_lib_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
	boost_lib_version_req_major=`expr $boost_lib_version_req : '\([[0-9]]*\)'`
	boost_lib_version_req_minor=`expr $boost_lib_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
	boost_lib_version_req_sub_minor=`expr $boost_lib_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
	if test "x$boost_lib_version_req_sub_minor" = "x" ; then
		boost_lib_version_req_sub_minor="0"
    	fi
	WANT_BOOST_VERSION=`expr $boost_lib_version_req_major \* 100000 \+  $boost_lib_version_req_minor \* 100 \+ $boost_lib_version_req_sub_minor`
	AC_MSG_CHECKING(for boostlib >= $boost_lib_version_req)
	succeeded=no

	dnl first we check the system location for boost libraries
	dnl this location ist chosen if boost libraries are installed with the --layout=system option
	dnl or if you install boost with RPM
	if test "$ac_boost_path" != ""; then
		BOOST_LDFLAGS=" -L$ac_boost_path/lib"
		BOOST_CPPFLAGS="-I$ac_boost_path/include"
                ROSE_BOOST_INCLUDE_PATH="$ac_boost_path/include"
	else
		for ac_boost_path_tmp in /usr /usr/local /opt /opt/local ; do
			if test -d "$ac_boost_path_tmp/include/boost" && test -r "$ac_boost_path_tmp/include/boost"; then
				BOOST_LDFLAGS="-L$ac_boost_path_tmp/lib"
				BOOST_CPPFLAGS="-I$ac_boost_path_tmp/include"
                                ROSE_BOOST_INCLUDE_PATH="$ac_boost_path_tmp/include"
				break;
			fi
		done
	fi

# echo "WANT_BOOST_VERSION = $WANT_BOOST_VERSION"
# echo "boost_lib_version_req = $boost_lib_version_req"
# echo "BOOST_LDFLAGS  = $BOOST_LDFLAGS"
# echo "BOOST_CPPFLAGS = $BOOST_CPPFLAGS"

# DQ (1/1/2009): Set the default value based on BOOST_CPPFLAGS
# Liao (1/12/2009): ac_boost_path_tmp is one of the if's branches. 
# Moved the assignment into the two branches above
#   ROSE_BOOST_INCLUDE_PATH="$ac_boost_path_tmp/include"

 # DQ (1/1/2009): Added testing for previously installed Boost (always older version)
 # so that we can trigger the use of "-isystem" option (to g++) only when required
 # (it appears to be a problem for SWIG).
 # Use this set of paths, and the set including "/home/dquinlan" for testing this macro.
 # for ac_boost_path_tmp in /usr /usr/local /opt /opt/local /home/dquinlan; do
   for ac_boost_path_tmp in /usr /usr/local /opt /opt/local ; do
		if test -d "$ac_boost_path_tmp/include/boost" && test -r "$ac_boost_path_tmp/include/boost"; then
			PREVIOUSLY_INSTALLED_BOOST="$ac_boost_path_tmp/include/boost"
       # echo "Detected a previously installed version of boost library: PREVIOUSLY_INSTALLED_BOOST = $PREVIOUSLY_INSTALLED_BOOST"
			break;
		fi
	done

    dnl overwrite ld flags if we have required special directory with
    dnl --with-boost-libdir parameter
    if test "$ac_boost_lib_path" != ""; then
       BOOST_LDFLAGS="-L$ac_boost_lib_path"
    fi

	CPPFLAGS_SAVED="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
	export CPPFLAGS

	LDFLAGS_SAVED="$LDFLAGS"
	LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
	export LDFLAGS

	AC_LANG_PUSH(C++)
     	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
	@%:@include <boost/version.hpp>
	]], [[
	#if BOOST_VERSION >= $WANT_BOOST_VERSION
	// Everything is okay
	#else
	#  error Boost version is too old
	#endif
	]])],[
        AC_MSG_RESULT(yes)
	succeeded=yes
	found_system=yes
       	],[
       	])
	AC_LANG_POP([C++])

	dnl if we found no boost with system layout we search for boost libraries
	dnl built and installed without the --layout=system option or for a staged(not installed) version
	if test "x$succeeded" != "xyes"; then
		_version=0
		if test "$ac_boost_path" != ""; then
			if test -d "$ac_boost_path" && test -r "$ac_boost_path"; then
				for i in `ls -d $ac_boost_path/include/boost-* 2>/dev/null`; do
					_version_tmp=`echo $i | sed "s#$ac_boost_path##" | sed 's/\/include\/boost-//' | sed 's/_/./'`
					V_CHECK=`expr $_version_tmp \> $_version`
					if test "$V_CHECK" = "1" ; then
						_version=$_version_tmp
					fi
					VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
					BOOST_CPPFLAGS="-I$ac_boost_path/include/boost-$VERSION_UNDERSCORE"
             # DQ (12/22/2008): Modified macro to save the boost path so that it could be used with "-isystem" option (gcc).
					ROSE_BOOST_INCLUDE_PATH="$ac_boost_path/include/boost-$VERSION_UNDERSCORE"
				done
			fi
		else
			for ac_boost_path in /usr /usr/local /opt /opt/local ; do
				if test -d "$ac_boost_path" && test -r "$ac_boost_path"; then
					for i in `ls -d $ac_boost_path/include/boost-* 2>/dev/null`; do
						_version_tmp=`echo $i | sed "s#$ac_boost_path##" | sed 's/\/include\/boost-//' | sed 's/_/./'`
						V_CHECK=`expr $_version_tmp \> $_version`
						if test "$V_CHECK" = "1" ; then
							_version=$_version_tmp
	               					best_path=$ac_boost_path
						fi
					done
				fi
			done

			VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
			BOOST_CPPFLAGS="-I$best_path/include/boost-$VERSION_UNDERSCORE"
            if test "$ac_boost_lib_path" = ""
            then
               BOOST_LDFLAGS="-L$best_path/lib"
            fi

	    		if test "x$BOOST_ROOT" != "x"; then
				if test -d "$BOOST_ROOT" && test -r "$BOOST_ROOT" && test -d "$BOOST_ROOT/stage/lib" && test -r "$BOOST_ROOT/stage/lib"; then
					version_dir=`expr //$BOOST_ROOT : '.*/\(.*\)'`
					stage_version=`echo $version_dir | sed 's/boost_//' | sed 's/_/./g'`
			        	stage_version_shorten=`expr $stage_version : '\([[0-9]]*\.[[0-9]]*\)'`
					V_CHECK=`expr $stage_version_shorten \>\= $_version`
                    if test "$V_CHECK" = "1" -a "$ac_boost_lib_path" = "" ; then
						AC_MSG_NOTICE(We will use a staged boost library from $BOOST_ROOT)
						BOOST_CPPFLAGS="-I$BOOST_ROOT"
						BOOST_LDFLAGS="-L$BOOST_ROOT/stage/lib"
					fi
				fi
	    		fi
		fi

    # DQ (12/22/2008): Fixup Boost to not use the system (OS) installation of Boost
    # BOOST_CPPFLAGS="$BOOST_CPPFLAGS/boost"

		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

		AC_LANG_PUSH(C++)
	     	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
		@%:@include <boost/version.hpp>
		]], [[
		#if BOOST_VERSION >= $WANT_BOOST_VERSION
		// Everything is okay
		#else
		#  error Boost version is too old
		#endif
		]])],[
        	AC_MSG_RESULT(yes)
		succeeded=yes
		found_system=yes
       		],[
	       	])
		AC_LANG_POP([C++])
	fi

	if test "$succeeded" != "yes" ; then
		if test "$_version" = "0" ; then
       # This was always a confusing error message so make it more explicit for users.
       # AC_MSG_ERROR([[We could not detect the boost libraries (version $boost_lib_version_req_shorten or higher). If you have a staged boost library (still not installed) please specify \$BOOST_ROOT in your environment and do not give a PATH to --with-boost option.  If you are sure you have boost installed, then check your version number looking in <boost/version.hpp>. See http://randspringer.de/boost for more documentation.]])
			AC_MSG_ERROR([[Boost libraries (version $boost_lib_version_req_shorten or higher) must be specified on the configure line (using the --with-boost=<path> option) and the boost libraries must be in your LD_LIBRARY_PATH.]])
		else
			AC_MSG_NOTICE([Your boost libraries seems to old (version $_version).])
		fi
	else
		AC_SUBST(BOOST_CPPFLAGS)
		AC_SUBST(BOOST_LDFLAGS)
		AC_DEFINE(HAVE_BOOST,,[define if the Boost library is available])
	fi

        CPPFLAGS="$CPPFLAGS_SAVED"
       	LDFLAGS="$LDFLAGS_SAVED"
fi

# DQ (12/22/2008): Modified macro to save the boost path so that it could be used with "-isystem" 
# option to include the boost path specified on the configure command ahead of "/usr/local/include" 
# so that we can get the required version of Boost on systems that have it installed by default.
# echo "Final Test: ROSE_BOOST_INCLUDE_PATH = $ROSE_BOOST_INCLUDE_PATH"
# AC_SUBST(ROSE_BOOST_INCLUDE_PATH)

# DQ (1/1/2009): This use of "-isystem" is not triggered only when there is
# a previously installed version of ROSE detected (e.g. in /usr/liclude/boost).
# Note that use of "-isystem" option with g++ will cause SWIG to fail.
if test "$PREVIOUSLY_INSTALLED_BOOST" != ""; then
 # echo "Using the -isystem option of g++ to force the use of the specified version of Boost ahead of a previously installed version of boost on your system at: $PREVIOUSLY_INSTALLED_BOOST"
   AC_MSG_NOTICE(Using the -isystem option of g++ to force the use of the specified version of Boost ahead of a previously installed version of boost on your system at: $PREVIOUSLY_INSTALLED_BOOST)
   AC_MSG_WARN([Note that the --with-javaport can NOT be used with the -isystem option])
   ROSE_BOOST_PREINCLUDE_PATH="-isystem $ROSE_BOOST_INCLUDE_PATH"
   ROSE_BOOST_NORMAL_INCLUDE_PATH=""
   AC_MSG_WARN([[Detected previously installed version of boost (please remove older version of Boost before installing ROSE) (continuing but expect Boost to be a problem...)]])
 # AC_MSG_ERROR([[Detected previously installed version of boost (please remove older version of Boost before installing ROSE) (continuing but expect Boost to be a problem...)]])
 # Remove this exit (as a test) after detecting what I expect is a problem...
 # exit 1
else
   AC_MSG_NOTICE(No previously installed version of boost detected: using boost include directories with normal -I option)
   ROSE_BOOST_PREINCLUDE_PATH=""
   ROSE_BOOST_NORMAL_INCLUDE_PATH="-I$ROSE_BOOST_INCLUDE_PATH"
fi

AC_SUBST(ROSE_BOOST_PREINCLUDE_PATH)
AC_SUBST(ROSE_BOOST_NORMAL_INCLUDE_PATH)


])

#
# SYNOPSIS
#
#   AX_BOOST_DATE_TIME
#
# DESCRIPTION
#
#   Test for Date_Time library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation
#   is available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_DATE_TIME_LIB)
#
#   And sets:
#
#     HAVE_BOOST_DATE_TIME
#
# LAST MODIFICATION
#
#   2007-11-22
#
# COPYLEFT
#
#   Copyright (c) 2007 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2007 Michael Tindal
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_BOOST_DATE_TIME],
[
	AC_ARG_WITH([boost-date-time],
	AS_HELP_STRING([--with-boost-date-time@<:@=special-lib@:>@],
                   [use the Date_Time library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-date-time=boost_date_time-gcc-mt-d-1_33_1 ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_date_time_lib=""
        else
		    want_boost="yes"
        	ax_boost_user_date_time_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

        AC_CACHE_CHECK(whether the Boost::Date_Time library is available,
					   ax_cv_boost_date_time,
        [AC_LANG_PUSH([C++])
		 AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[@%:@include <boost/date_time/gregorian/gregorian_types.hpp>]],
                                   [[using namespace boost::gregorian; date d(2002,Jan,10);
                                     return 0;
                                   ]]),
         ax_cv_boost_date_time=yes, ax_cv_boost_date_time=no)
         AC_LANG_POP([C++])
		])
		if test "x$ax_cv_boost_date_time" = "xyes"; then
			AC_DEFINE(HAVE_BOOST_DATE_TIME,,[define if the Boost::Date_Time library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
            if test "x$ax_boost_user_date_time_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_date_time*.{so,dylib,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_date_time.*\)\.so.*$;\1;' -e 's;^lib\(boost_date_time.*\)\.a*$;\1;' -e 's;^lib\(boost_date_time.*\)\.dylib$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_DATE_TIME_LIB="-l$ax_lib"; AC_SUBST(BOOST_DATE_TIME_LIB) link_date_time="yes"; break],
                                 [link_date_time="no"])
  				done
                if test "x$link_date_time" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_date_time*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_date_time.*\)\.dll.*$;\1;' -e 's;^\(boost_date_time.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_DATE_TIME_LIB="-l$ax_lib"; AC_SUBST(BOOST_DATE_TIME_LIB) link_date_time="yes"; break],
                                 [link_date_time="no"])
  				done
                fi

            else
               for ax_lib in $ax_boost_user_date_time_lib boost_date_time-$ax_boost_user_date_time_lib; do
				      AC_CHECK_LIB($ax_lib, main,
                                   [BOOST_DATE_TIME_LIB="-l$ax_lib"; AC_SUBST(BOOST_DATE_TIME_LIB) link_date_time="yes"; break],
                                   [link_date_time="no"])
                  done

            fi
			if test "x$link_date_time" != "xyes"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
			fi
		fi

		CPPFLAGS="$CPPFLAGS_SAVED"
    	LDFLAGS="$LDFLAGS_SAVED"
	fi
])

# ===========================================================================
#          http://autoconf-archive.cryp.to/ax_boost_filesystem.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_FILESYSTEM
#
# DESCRIPTION
#
#   Test for Filesystem library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation is
#   available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_FILESYSTEM_LIB)
#
#   And sets:
#
#     HAVE_BOOST_FILESYSTEM
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Michael Tindal
#   Copyright (c) 2008 Roman Rybalko <libtorrent@romanr.info> (using BOOST_SYSTEM_LIB)
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_BOOST_FILESYSTEM],
[
	AC_ARG_WITH([boost-filesystem],
	AS_HELP_STRING([--with-boost-filesystem@<:@=special-lib@:>@],
                   [use the Filesystem library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-filesystem=boost_filesystem-gcc-mt ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_filesystem_lib=""
        else
		    want_boost="yes"
        	ax_boost_user_filesystem_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS
		
		LIBS_SAVED=$LIBS
		LIBS="$LIBS $BOOST_SYSTEM_LIB"
		export LIBS

        AC_CACHE_CHECK(whether the Boost::Filesystem library is available,
					   ax_cv_boost_filesystem,
        [AC_LANG_PUSH([C++])
         AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[@%:@include <boost/filesystem/path.hpp>]],
                                   [[using namespace boost::filesystem;
                                   path my_path( "foo/bar/data.txt" );
                                   return 0;]]),
            				       ax_cv_boost_filesystem=yes, ax_cv_boost_filesystem=no)
         AC_LANG_POP([C++])
		])
		if test "x$ax_cv_boost_filesystem" = "xyes"; then
			AC_DEFINE(HAVE_BOOST_FILESYSTEM,,[define if the Boost::Filesystem library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
            if test "x$ax_boost_user_filesystem_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_filesystem*.{so,dylib,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_filesystem.*\)\.so.*$;\1;' -e 's;^lib\(boost_filesystem.*\)\.a*$;\1;' -e 's;^lib\(boost_filesystem.*\)\.dylib$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_FILESYSTEM_LIB="-l$ax_lib"; AC_SUBST(BOOST_FILESYSTEM_LIB) link_filesystem="yes"; break],
                                 [link_filesystem="no"])
  				done
                if test "x$link_program_options" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_filesystem*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_filesystem.*\)\.dll.*$;\1;' -e 's;^\(boost_filesystem.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_FILESYSTEM_LIB="-l$ax_lib"; AC_SUBST(BOOST_FILESYSTEM_LIB) link_filesystem="yes"; break],
                                 [link_filesystem="no"])
  				done
	            fi
            else
               for ax_lib in $ax_boost_user_filesystem_lib boost_filesystem-$ax_boost_user_filesystem_lib; do
				      AC_CHECK_LIB($ax_lib, toupper,
                                   [BOOST_FILESYSTEM_LIB="-l$ax_lib"; AC_SUBST(BOOST_FILESYSTEM_LIB) link_filesystem="yes"; break],
                                   [link_filesystem="no"])
                  done

            fi
			if test "x$link_filesystem" != "xyes"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
			fi
		fi

		CPPFLAGS="$CPPFLAGS_SAVED"
    		LDFLAGS="$LDFLAGS_SAVED"
		LIBS="$LIBS_SAVED"
	fi
])

#
# SYNOPSIS
#
#   AX_BOOST_PROGRAM_OPTIONS
#
# DESCRIPTION
#
#   Test for program options library from the Boost C++ libraries. The
#   macro requires a preceding call to AX_BOOST_BASE. Further
#   documentation is available at
#   <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB)
#
#   And sets:
#
#     HAVE_BOOST_PROGRAM_OPTIONS
#
# LAST MODIFICATION
#
#   2007-11-22
#
# COPYLEFT
#
#   Copyright (c) 2007 Thomas Porschberg <thomas@randspringer.de>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_BOOST_PROGRAM_OPTIONS],
[
	AC_ARG_WITH([boost-program-options],
		AS_HELP_STRING([--with-boost-program-options@<:@=special-lib@:>@],
                       [use the program options library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-program-options=boost_program_options-gcc-mt-1_33_1 ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_program_options_lib=""
        else
		    want_boost="yes"
        	ax_boost_user_program_options_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
	    export want_boost
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS
		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS
		AC_CACHE_CHECK([whether the Boost::Program_Options library is available],
					   ax_cv_boost_program_options,
					   [AC_LANG_PUSH(C++)
 		                AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[@%:@include <boost/program_options.hpp>
                                                          ]],
                                  [[boost::program_options::options_description generic("Generic options");
                                   return 0;]]),
                           ax_cv_boost_program_options=yes, ax_cv_boost_program_options=no)
			               	AC_LANG_POP([C++])
		])
		if test "$ax_cv_boost_program_options" = yes; then
				AC_DEFINE(HAVE_BOOST_PROGRAM_OPTIONS,,[define if the Boost::PROGRAM_OPTIONS library is available])
                  BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
                if test "x$ax_boost_user_program_options_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_program_options*.{so,dylib,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_program_options.*\)\.so.*$;\1;' -e 's;^lib\(boost_program_options.*\)\.a*$;\1;' -e 's;^lib\(boost_program_options.*\)\.dylib$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_PROGRAM_OPTIONS_LIB="-l$ax_lib"; AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB) link_program_options="yes"; break],
                                 [link_program_options="no"])
  				done
                if test "x$link_program_options" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_program_options*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_program_options.*\)\.dll.*$;\1;' -e 's;^\(boost_program_options.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_PROGRAM_OPTIONS_LIB="-l$ax_lib"; AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB) link_program_options="yes"; break],
                                 [link_program_options="no"])
  				done
                fi
                else
                  for ax_lib in $ax_boost_user_program_options_lib boost_program_options-$ax_boost_user_program_options_lib; do
				      AC_CHECK_LIB($ax_lib, main,
                                   [BOOST_PROGRAM_OPTIONS_LIB="-l$ax_lib"; AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB) link_program_options="yes"; break],
                                   [link_program_options="no"])
                  done
                fi
				if test "x$link_program_options" != "xyes"; then
					AC_MSG_ERROR([Could not link against [$ax_lib] !])
				fi
		fi
		CPPFLAGS="$CPPFLAGS_SAVED"
    	LDFLAGS="$LDFLAGS_SAVED"
	fi
])

#
# SYNOPSIS
#
#   AX_BOOST_REGEX
#
# DESCRIPTION
#
#   Test for Regex library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation
#   is available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_REGEX_LIB)
#
#   And sets:
#
#     HAVE_BOOST_REGEX
#
# LAST MODIFICATION
#
#   2007-11-22
#
# COPYLEFT
#
#   Copyright (c) 2007 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2007 Michael Tindal
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_BOOST_REGEX],
[
	AC_ARG_WITH([boost-regex],
	AS_HELP_STRING([--with-boost-regex@<:@=special-lib@:>@],
                   [use the Regex library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-regex=boost_regex-gcc-mt-d-1_33_1 ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_regex_lib=""
        else
		    want_boost="yes"
        	ax_boost_user_regex_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

        AC_CACHE_CHECK(whether the Boost::Regex library is available,
					   ax_cv_boost_regex,
        [AC_LANG_PUSH([C++])
			 AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[@%:@include <boost/regex.hpp>
												]],
                                   [[boost::regex r(); return 0;]]),
                   ax_cv_boost_regex=yes, ax_cv_boost_regex=no)
         AC_LANG_POP([C++])
		])
		if test "x$ax_cv_boost_regex" = "xyes"; then
			AC_DEFINE(HAVE_BOOST_REGEX,,[define if the Boost::Regex library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
            if test "x$ax_boost_user_regex_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_regex*.{so,dylib,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_regex.*\)\.so.*$;\1;' -e 's;^lib\(boost_regex.*\)\.a*$;\1;' -e 's;^lib\(boost_regex.*\)\.dylib$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_REGEX_LIB="-l$ax_lib"; AC_SUBST(BOOST_REGEX_LIB) link_regex="yes"; break],
                                 [link_regex="no"])
  				done
                if test "x$link_regex" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_regex*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_regex.*\)\.dll.*$;\1;' -e 's;^\(boost_regex.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_REGEX_LIB="-l$ax_lib"; AC_SUBST(BOOST_REGEX_LIB) link_regex="yes"; break],
                                 [link_regex="no"])
  				done
                fi

            else
               for ax_lib in $ax_boost_user_regex_lib boost_regex-$ax_boost_user_regex_lib; do
				      AC_CHECK_LIB($ax_lib, main,
                                   [BOOST_REGEX_LIB="-l$ax_lib"; AC_SUBST(BOOST_REGEX_LIB) link_regex="yes"; break],
                                   [link_regex="no"])
               done
            fi
			if test "x$link_regex" != "xyes"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
			fi
		fi

		CPPFLAGS="$CPPFLAGS_SAVED"
    	LDFLAGS="$LDFLAGS_SAVED"
	fi
])

#
# SYNOPSIS
#
#   AX_BOOST_SYSTEM
#
# DESCRIPTION
#
#   Test for System library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation
#   is available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_SYSTEM_LIB)
#
#   And sets:
#
#     HAVE_BOOST_SYSTEM
#
# LAST MODIFICATION
#
#   2008-02-20
#
# COPYLEFT
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Michael Tindal
#   Copyright (c) 2008 Daniel Casimiro <dan.casimiro@gmail.com>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_BOOST_SYSTEM],
[
	AC_ARG_WITH([boost-system],
	AS_HELP_STRING([--with-boost-system@<:@=special-lib@:>@],
                   [use the System library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-system=boost_system-gcc-mt ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_system_lib=""
        else
		    want_boost="yes"
        	ax_boost_user_system_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
        AC_REQUIRE([AC_CANONICAL_BUILD])
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

        AC_CACHE_CHECK(whether the Boost::System library is available,
					   ax_cv_boost_system,
        [AC_LANG_PUSH([C++])
			 CXXFLAGS_SAVE=$CXXFLAGS

			 AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[@%:@include <boost/system/error_code.hpp>]],
                                   [[boost::system::system_category]]),
                   ax_cv_boost_system=yes, ax_cv_boost_system=no)
			 CXXFLAGS=$CXXFLAGS_SAVE
             AC_LANG_POP([C++])
		])
		if test "x$ax_cv_boost_system" = "xyes"; then
			AC_SUBST(BOOST_CPPFLAGS)

			AC_DEFINE(HAVE_BOOST_SYSTEM,,[define if the Boost::System library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

			LDFLAGS_SAVE=$LDFLAGS
            if test "x$ax_boost_user_system_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_system*.{so,dylib,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_system.*\)\.so.*$;\1;' -e 's;^lib\(boost_system.*\)\.a*$;\1;' -e 's;^lib\(boost_system.*\)\.dylib$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_SYSTEM_LIB="-l$ax_lib"; AC_SUBST(BOOST_SYSTEM_LIB) link_system="yes"; break],
                                 [link_system="no"])
  				done
                if test "x$link_system" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_system*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_system.*\)\.dll.*$;\1;' -e 's;^\(boost_system.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_SYSTEM_LIB="-l$ax_lib"; AC_SUBST(BOOST_SYSTEM_LIB) link_system="yes"; break],
                                 [link_system="no"])
  				done
                fi

            else
               for ax_lib in $ax_boost_user_system_lib boost_system-$ax_boost_user_system_lib; do
				      AC_CHECK_LIB($ax_lib, toupper,
                                   [BOOST_SYSTEM_LIB="-l$ax_lib"; AC_SUBST(BOOST_SYSTEM_LIB) link_system="yes"; break],
                                   [link_system="no"])
                  done

            fi
			if test "x$link_system" = "xno"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
                AC_DEFINE([USE_ROSE_BOOST_WAVE_SUPPORT],1,[Controls use of BOOST WAVE support in ROSE.])
			fi
		fi
		CPPFLAGS="$CPPFLAGS_SAVED"
    	LDFLAGS="$LDFLAGS_SAVED"
	fi
])

#
# SYNOPSIS
#
#   AX_BOOST_THREAD
#
# DESCRIPTION
#
#   Test for Thread library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation
#   is available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_THREAD_LIB)
#
#   And sets:
#
#     HAVE_BOOST_THREAD
#
# LAST MODIFICATION
#
#   2007-11-22
#
# COPYLEFT
#
#   Copyright (c) 2007 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2007 Michael Tindal
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_BOOST_THREAD],
[
	AC_ARG_WITH([boost-thread],
	AS_HELP_STRING([--with-boost-thread@<:@=special-lib@:>@],
                   [use the Thread library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-thread=boost_thread-gcc-mt ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_thread_lib=""
        else
		    want_boost="yes"
        	ax_boost_user_thread_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
        AC_REQUIRE([AC_CANONICAL_BUILD])
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

        AC_CACHE_CHECK(whether the Boost::Thread library is available,
					   ax_cv_boost_thread,
        [AC_LANG_PUSH([C++])
			 CXXFLAGS_SAVE=$CXXFLAGS

			 case "$build_os" in
			   solaris ) CXXFLAGS="-pthreads $CXXFLAGS" ;;
			   ming32 ) CXXFLAGS="-mthreads $CXXFLAGS" ;;
			   darwin* ) CXXFLAGS="$CXXFLAGS" ;;
			   * ) CXXFLAGS="-pthread $CXXFLAGS" ;;
			 esac
			 AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[@%:@include <boost/thread/thread.hpp>]],
                                   [[boost::thread_group thrds;
                                   return 0;]]),
                   ax_cv_boost_thread=yes, ax_cv_boost_thread=no)
			 CXXFLAGS=$CXXFLAGS_SAVE
             AC_LANG_POP([C++])
		])
		if test "x$ax_cv_boost_thread" = "xyes"; then
			case "$build_os" in
			  solaris ) BOOST_CPPFLAGS="-pthreads $BOOST_CPPFLAGS" ;;
			  ming32 ) BOOST_CPPFLAGS="-mthreads $BOOST_CPPFLAGS" ;;
			  darwin* ) BOOST_CPPFLAGS="$BOOST_CPPFLAGS" ;;
			  * ) BOOST_CPPFLAGS="-pthread $BOOST_CPPFLAGS" ;;
			esac

			AC_SUBST(BOOST_CPPFLAGS)

			AC_DEFINE(HAVE_BOOST_THREAD,,[define if the Boost::Thread library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

			LDFLAGS_SAVE=$LDFLAGS
                        case "x$build_os" in
                          *bsd* )
                               LDFLAGS="-pthread $LDFLAGS"
                          break;
                          ;;
                        esac
            if test "x$ax_boost_user_thread_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_thread*.{so,dylib,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_thread.*\)\.so.*$;\1;' -e 's;^lib\(boost_thread.*\)\.a*$;\1;' -e 's;^lib\(boost_thread.*\)\.dylib$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_THREAD_LIB="-l$ax_lib"; AC_SUBST(BOOST_THREAD_LIB) link_thread="yes"; break],
                                 [link_thread="no"])
  				done
                if test "x$link_thread" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_thread*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_thread.*\)\.dll.*$;\1;' -e 's;^\(boost_thread.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_THREAD_LIB="-l$ax_lib"; AC_SUBST(BOOST_THREAD_LIB) link_thread="yes"; break],
                                 [link_thread="no"])
  				done
                fi

            else
               for ax_lib in $ax_boost_user_thread_lib boost_thread-$ax_boost_user_thread_lib; do
				      AC_CHECK_LIB($ax_lib, toupper,
                                   [BOOST_THREAD_LIB="-l$ax_lib"; AC_SUBST(BOOST_THREAD_LIB) link_thread="yes"; break],
                                   [link_thread="no"])
                  done

            fi
			if test "x$link_thread" = "xno"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
                        else
                           case "x$build_os" in
                              *bsd* )
			        BOOST_LDFLAGS="-pthread $BOOST_LDFLAGS"
                              break;
                              ;;
                           esac

			fi
		fi

		CPPFLAGS="$CPPFLAGS_SAVED"
    	LDFLAGS="$LDFLAGS_SAVED"
	fi
])

 http://autoconf-archive.cryp.to/ax_boost_regex.html
#
# SYNOPSIS
#
#   AX_BOOST_WAVE
#
# DESCRIPTION
#
#   Test for Wave library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation
#   is available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_WAVE_LIB)
#
#   And sets:
#
#     HAVE_BOOST_WAVE
#
# LAST MODIFICATION
#
#   2007-11-22
#
# COPYLEFT
#
#   Copyright (c) 2007 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2007 Michael Tindal
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_BOOST_WAVE],
[
	AC_ARG_WITH([boost-wave],
	AS_HELP_STRING([--with-boost-wave@<:@=special-lib@:>@],
                   [use the Wave library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-wave=boost_wave-gcc-mt-d-1_33_1 ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_wave_lib=""
        else
		    want_boost="yes"
        	ax_boost_user_wave_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)
    AM_CONDITIONAL(ROSE_USE_BOOST_WAVE, test "x$want_boost" = "xyes"  )

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

		LIBS_SAVED=$LIBS
		LIBS="$LIBS $BOOST_SYSTEM_LIB $BOOST_THREAD_LIB"
		export LIBS

        AC_CACHE_CHECK(whether the Boost::Wave library is available,
					   ax_cv_boost_wave,
        [AC_LANG_PUSH([C++])
			 AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[@%:@include <boost/wave.hpp>
												]],
                                   [[boost::wave::token_category r; return 0;]]),
                   ax_cv_boost_wave=yes, ax_cv_boost_wave=no)
         AC_LANG_POP([C++])
		])
		if test "x$ax_cv_boost_wave" = "xyes"; then

			AC_DEFINE(HAVE_BOOST_WAVE,,[define if the Boost::Wave library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
            if test "x$ax_boost_user_wave_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_wave*.{so,dylib,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_wave.*\)\.so.*$;\1;' -e 's;^lib\(boost_wave.*\)\.a*$;\1;' -e 's;^lib\(boost_wave.*\)\.dylib$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_WAVE_LIB="-l$ax_lib"; AC_SUBST(BOOST_WAVE_LIB) link_wave="yes"; break],
                                 [link_wave="no"])
  				done
                if test "x$link_wave" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_wave*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_wave.*\)\.dll.*$;\1;' -e 's;^\(boost_wave.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, toupper,
                                 [BOOST_WAVE_LIB="-l$ax_lib"; AC_SUBST(BOOST_WAVE_LIB) link_wave="yes"; break],
                                 [link_wave="no"])
  				done
                fi

            else
               for ax_lib in $ax_boost_user_wave_lib boost_wave-$ax_boost_user_wave_lib; do
				      AC_CHECK_LIB($ax_lib, main,
                                   [BOOST_WAVE_LIB="-l$ax_lib"; AC_SUBST(BOOST_WAVE_LIB) link_wave="yes"; break],
                                   [link_wave="no"])
               done
            fi
			if test "x$link_wave" != "xyes"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
			fi
            # DQ (4/7/2006): build a macro to use in source code to know when WAVE is to be used.
            AC_DEFINE([USE_ROSE_BOOST_WAVE_SUPPORT],1,[Controls use of BOOST WAVE support in ROSE.])
       
		else
            AC_DEFINE([USE_ROSE_BOOST_WAVE_SUPPORT],0,[Controls use of BOOST WAVE support in ROSE.])

		fi



		CPPFLAGS="$CPPFLAGS_SAVED"
           	LDFLAGS="$LDFLAGS_SAVED"
		LIBS="$LIBS_SAVED"

	fi
])

# ===========================================================================
#              http://autoconf-archive.cryp.to/ax_lib_mysql.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_MYSQL([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   This macro provides tests of availability of MySQL client library of
#   particular version or newer.
#
#   AX_LIB_MYSQL macro takes only one argument which is optional. If there
#   is no required version passed, then macro does not run version test.
#
#   The --with-mysql option takes one of three possible values:
#
#   no - do not check for MySQL client library
#
#   yes - do check for MySQL library in standard locations (mysql_config
#   should be in the PATH)
#
#   path - complete path to mysql_config utility, use this option if
#   mysql_config can't be found in the PATH
#
#   This macro calls:
#
#     AC_SUBST(MYSQL_CFLAGS)
#     AC_SUBST(MYSQL_LDFLAGS)
#     AC_SUBST(MYSQL_VERSION)
#
#   And sets:
#
#     HAVE_MYSQL
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Mateusz Loskot <mateusz@loskot.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_LIB_MYSQL],
[
    AC_ARG_WITH([mysql],
        AC_HELP_STRING([--with-mysql=@<:@ARG@:>@],
            [use MySQL client library @<:@default=yes@:>@, optionally specify path to mysql_config]
        ),
        [
        if test "$withval" = "no"; then
            want_mysql="no"
        elif test "$withval" = "yes"; then
            want_mysql="yes"
        else
            want_mysql="yes"
            MYSQL_CONFIG="$withval"
        fi
        ],
        [want_mysql="no"] dnl JJW (7/29/2008): Default should be no MySQL
    )

    MYSQL_CFLAGS=""
    MYSQL_LDFLAGS=""
    MYSQL_VERSION=""

    dnl
    dnl Check MySQL libraries (libpq)
    dnl

    if test "$want_mysql" = "yes"; then

        if test -z "$MYSQL_CONFIG" -o test; then
            AC_PATH_PROG([MYSQL_CONFIG], [mysql_config], [no])
        fi

        if test "$MYSQL_CONFIG" != "no"; then
            AC_MSG_CHECKING([for MySQL libraries])

            MYSQL_CFLAGS="`$MYSQL_CONFIG --cflags`"
            MYSQL_LDFLAGS="`$MYSQL_CONFIG --libs`"

            MYSQL_VERSION=`$MYSQL_CONFIG --version`

            AC_DEFINE([HAVE_MYSQL], [1],
                [Define to 1 if MySQL libraries are available])

            found_mysql="yes"
            AC_MSG_RESULT([yes])
        else
            found_mysql="no"
            AC_MSG_RESULT([no])
        fi
    fi

    dnl
    dnl Check if required version of MySQL is available
    dnl


    mysql_version_req=ifelse([$1], [], [], [$1])

    if test "$found_mysql" = "yes" -a -n "$mysql_version_req"; then

        AC_MSG_CHECKING([if MySQL version is >= $mysql_version_req])

        dnl Decompose required version string of MySQL
        dnl and calculate its number representation
        mysql_version_req_major=`expr $mysql_version_req : '\([[0-9]]*\)'`
        mysql_version_req_minor=`expr $mysql_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
        mysql_version_req_micro=`expr $mysql_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$mysql_version_req_micro" = "x"; then
            mysql_version_req_micro="0"
        fi

        mysql_version_req_number=`expr $mysql_version_req_major \* 1000000 \
                                   \+ $mysql_version_req_minor \* 1000 \
                                   \+ $mysql_version_req_micro`

        dnl Decompose version string of installed MySQL
        dnl and calculate its number representation
        mysql_version_major=`expr $MYSQL_VERSION : '\([[0-9]]*\)'`
        mysql_version_minor=`expr $MYSQL_VERSION : '[[0-9]]*\.\([[0-9]]*\)'`
        mysql_version_micro=`expr $MYSQL_VERSION : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$mysql_version_micro" = "x"; then
            mysql_version_micro="0"
        fi

        mysql_version_number=`expr $mysql_version_major \* 1000000 \
                                   \+ $mysql_version_minor \* 1000 \
                                   \+ $mysql_version_micro`

        mysql_version_check=`expr $mysql_version_number \>\= $mysql_version_req_number`
        if test "$mysql_version_check" = "1"; then
            AC_MSG_RESULT([yes])
        else
            AC_MSG_RESULT([no])
        fi
    fi

    AC_SUBST([MYSQL_VERSION])
    AC_SUBST([MYSQL_CFLAGS])
    AC_SUBST([MYSQL_LDFLAGS])
])

#
# SYNOPSIS
#
#   AX_LIB_SQLITE3([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the SQLite 3 library of a particular version (or newer)
#
#   This macro takes only one optional argument, required version of
#   SQLite 3 library. If required version is not passed, 3.0.0 is used
#   in the test of existance of SQLite 3.
#
#   If no intallation prefix to the installed SQLite library is given
#   the macro searches under /usr, /usr/local, and /opt.
#
#   This macro calls:
#
#     AC_SUBST(SQLITE3_CFLAGS)
#     AC_SUBST(SQLITE3_LDFLAGS)
#     AC_SUBST(SQLITE3_VERSION)
#
#   And sets:
#
#     HAVE_SQLITE3
#
# LAST MODIFICATION
#
#   2008-03-17
#
# COPYLEFT
#
#   Copyright (c) 2008 Mateusz Loskot <mateusz@loskot.net>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_LIB_SQLITE3],
[
    AC_ARG_WITH([sqlite3],
        AC_HELP_STRING(
            [--with-sqlite3=@<:@ARG@:>@],
            [use SQLite 3 library @<:@default=yes@:>@, optionally specify the prefix for sqlite3 library]
        ),
        [
        if test "$withval" = "no"; then
            WANT_SQLITE3="no"
        elif test "$withval" = "yes"; then
            WANT_SQLITE3="yes"
            ac_sqlite3_path=""
        else
            WANT_SQLITE3="yes"
            ac_sqlite3_path="$withval"
        fi
        ],
        [WANT_SQLITE3="no"]
    )

    SQLITE3_CFLAGS=""
    SQLITE3_LDFLAGS=""
    SQLITE3_VERSION=""
    if test "x$WANT_SQLITE3" = "xyes"; then

        ac_sqlite3_header="sqlite3.h"

        sqlite3_version_req=ifelse([$1], [], [3.0.0], [$1])
        sqlite3_version_req_shorten=`expr $sqlite3_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
        sqlite3_version_req_major=`expr $sqlite3_version_req : '\([[0-9]]*\)'`
        sqlite3_version_req_minor=`expr $sqlite3_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
        sqlite3_version_req_micro=`expr $sqlite3_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$sqlite3_version_req_micro" = "x" ; then
            sqlite3_version_req_micro="0"
        fi

        sqlite3_version_req_number=`expr $sqlite3_version_req_major \* 1000000 \
                                   \+ $sqlite3_version_req_minor \* 1000 \
                                   \+ $sqlite3_version_req_micro`

        AC_MSG_CHECKING([for SQLite3 library >= $sqlite3_version_req])

        if test "$ac_sqlite3_path" != ""; then
            ac_sqlite3_ldflags="-L$ac_sqlite3_path/lib"
            ac_sqlite3_cppflags="-I$ac_sqlite3_path/include"
        else
            for ac_sqlite3_path_tmp in /usr /usr/local /opt ; do
                if test -f "$ac_sqlite3_path_tmp/include/$ac_sqlite3_header" \
                    && test -r "$ac_sqlite3_path_tmp/include/$ac_sqlite3_header"; then
                    ac_sqlite3_path=$ac_sqlite3_path_tmp
                    ac_sqlite3_cppflags="-I$ac_sqlite3_path_tmp/include"
                    ac_sqlite3_ldflags="-L$ac_sqlite3_path_tmp/lib"
                    break;
                fi
            done
        fi

        ac_sqlite3_ldflags="$ac_sqlite3_ldflags -lsqlite3"

        saved_CPPFLAGS="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS $ac_sqlite3_cppflags"

        AC_LANG_PUSH(C++)
        AC_COMPILE_IFELSE(
            [
            AC_LANG_PROGRAM([[@%:@include <sqlite3.h>]],
                [[
#if (SQLITE_VERSION_NUMBER >= $sqlite3_version_req_number)
// Everything is okay
#else
#  error SQLite version is too old
#endif
                ]]
            )
            ],
            [
            AC_MSG_RESULT([yes])
            success="yes"
            ],
            [
            AC_MSG_RESULT([not found])
            succees="no"
            ]
        )
        AC_LANG_POP([C++])

        CPPFLAGS="$saved_CPPFLAGS"

        if test "$success" = "yes"; then

            SQLITE3_CFLAGS="$ac_sqlite3_cppflags"
            SQLITE3_LDFLAGS="$ac_sqlite3_ldflags"

            ac_sqlite3_header_path="$ac_sqlite3_path/include/$ac_sqlite3_header"

            dnl Retrieve SQLite release version
            if test "x$ac_sqlite3_header_path" != "x"; then
                ac_sqlite3_version=`cat $ac_sqlite3_header_path \
                    | grep '#define.*SQLITE_VERSION.*\"' | sed -e 's/.* "//' \
                        | sed -e 's/"//'`
                if test $ac_sqlite3_version != ""; then
                    SQLITE3_VERSION=$ac_sqlite3_version
                else
                    AC_MSG_WARN([Can not find SQLITE_VERSION macro in sqlite3.h header to retrieve SQLite version!])
                fi
            fi

            AC_SUBST(SQLITE3_CFLAGS)
            AC_SUBST(SQLITE3_LDFLAGS)
            AC_SUBST(SQLITE3_VERSION)
            AC_DEFINE([HAVE_SQLITE3], [], [Have the SQLITE3 library])
        fi
    fi
])

#
# SYNOPSIS
#
#   AX_ROSE([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the ROSE C++ libraries of a particular version (or newer)
#
#   If no path to the installed rose library is given the macro
#   searchs under /usr, /usr/local, /opt and /opt/local and evaluates
#   the $ROSE_ROOT environment variable. Further documentation is
#   available at <http://randspringer.de/rose/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(ROSE_CPPFLAGS) / AC_SUBST(ROSE_LDFLAGS)
#
#   And sets:
#
#     HAVE_ROSE
#
# LAST MODIFICATION
#
#   2008-03-24
#
# COPYLEFT
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_ROSE],
[
AC_ARG_WITH([rose],
	AS_HELP_STRING([--with-rose@<:@=DIR@:>@], [use rose (default is yes) - it is possible to specify the root directory for rose (optional)]),
	[
    if test "$withval" = "no"; then
		want_rose="no"
    elif test "$withval" = "yes"; then
        want_rose="yes"
        ac_rose_path=""
    else
	    want_rose="yes"
        ac_rose_path="$withval"
	fi
    ],
    [want_rose="yes"])


AC_ARG_WITH([rose-libdir],
        AS_HELP_STRING([--with-rose-libdir=LIB_DIR],
        [Force given directory for rose libraries. Note that this will overwrite library path detection, so use this parameter only if default library detection fails and you know exactly where your rose libraries are located.]),
        [
        if test -d $withval
        then
                ac_rose_lib_path="$withval"
        else
                AC_MSG_ERROR(--with-rose-libdir expected directory name)
        fi
        ],
        [ac_rose_lib_path=""]
)

if test "x$want_rose" = "xyes"; then
	rose_lib_version_req=ifelse([$1], ,1.20.0,$1)
	rose_lib_version_req_shorten=`expr $rose_lib_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
	rose_lib_version_req_major=`expr $rose_lib_version_req : '\([[0-9]]*\)'`
	rose_lib_version_req_minor=`expr $rose_lib_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
	rose_lib_version_req_sub_minor=`expr $rose_lib_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
	if test "x$rose_lib_version_req_sub_minor" = "x" ; then
		rose_lib_version_req_sub_minor="0"
    	fi
	WANT_ROSE_VERSION=90200
	AC_MSG_CHECKING(for roselib >= $rose_lib_version_req)
	succeeded=yes

	dnl first we check the system location for rose libraries
	dnl this location ist chosen if rose libraries are installed with the --layout=system option
	dnl or if you install rose with RPM
	if test "$ac_rose_path" != ""; then
		ROSE_LDFLAGS="-L$ac_rose_path/lib"
		ROSE_CPPFLAGS="-I$ac_rose_path/include"
	else
		for ac_rose_path_tmp in /usr /usr/local /opt /opt/local ; do
			if test -d "$ac_rose_path_tmp/include/rose" && test -r "$ac_rose_path_tmp/include/rose"; then
				ROSE_LDFLAGS="-L$ac_rose_path_tmp/lib"
				ROSE_CPPFLAGS="-I$ac_rose_path_tmp/include"
				break;
			fi
		done
	fi

    dnl overwrite ld flags if we have required special directory with
    dnl --with-rose-libdir parameter
    if test "$ac_rose_lib_path" != ""; then
       ROSE_LDFLAGS="-L$ac_rose_lib_path"
    fi

	CPPFLAGS_SAVED="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $ROSE_CPPFLAGS"
	export CPPFLAGS

	LDFLAGS_SAVED="$LDFLAGS"
	LDFLAGS="$LDFLAGS $ROSE_LDFLAGS"
	export LDFLAGS

	dnl if we found no rose with system layout we search for rose libraries
	dnl built and installed without the --layout=system option or for a staged(not installed) version
	if test "x$succeeded" != "xyes"; then
		_version=0
		if test "$ac_rose_path" != ""; then
			if test -d "$ac_rose_path" && test -r "$ac_rose_path"; then
				for i in `ls -d $ac_rose_path/include/rose-* 2>/dev/null`; do
					_version_tmp=`echo $i | sed "s#$ac_rose_path##" | sed 's/\/include\/rose-//' | sed 's/_/./'`
					V_CHECK=`expr $_version_tmp \> $_version`
					if test "$V_CHECK" = "1" ; then
						_version=$_version_tmp
					fi
					VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
					ROSE_CPPFLAGS="-I$ac_rose_path/include/rose-$VERSION_UNDERSCORE"
				done
			fi
		else
			for ac_rose_path in /usr /usr/local /opt /opt/local ; do
				if test -d "$ac_rose_path" && test -r "$ac_rose_path"; then
					for i in `ls -d $ac_rose_path/include/rose-* 2>/dev/null`; do
						_version_tmp=`echo $i | sed "s#$ac_rose_path##" | sed 's/\/include\/rose-//' | sed 's/_/./'`
						V_CHECK=`expr $_version_tmp \> $_version`
						if test "$V_CHECK" = "1" ; then
							_version=$_version_tmp
	               					best_path=$ac_rose_path
						fi
					done
				fi
			done

			VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
			ROSE_CPPFLAGS="-I$best_path/include/rose-$VERSION_UNDERSCORE"
            if test "$ac_rose_lib_path" = ""
            then
               ROSE_LDFLAGS="-L$best_path/lib"
            fi

	    		if test "x$ROSE_ROOT" != "x"; then
				if test -d "$ROSE_ROOT" && test -r "$ROSE_ROOT" && test -d "$ROSE_ROOT/stage/lib" && test -r "$ROSE_ROOT/stage/lib"; then
					version_dir=`expr //$ROSE_ROOT : '.*/\(.*\)'`
					stage_version=`echo $version_dir | sed 's/rose_//' | sed 's/_/./g'`
			        	stage_version_shorten=`expr $stage_version : '\([[0-9]]*\.[[0-9]]*\)'`
					V_CHECK=`expr $stage_version_shorten \>\= $_version`
                    if test "$V_CHECK" = "1" -a "$ac_rose_lib_path" = "" ; then
						AC_MSG_NOTICE(We will use a staged rose library from $ROSE_ROOT)
						ROSE_CPPFLAGS="-I$ROSE_ROOT"
						ROSE_LDFLAGS="-L$ROSE_ROOT/stage/lib"
					fi
				fi
	    		fi
		fi

		CPPFLAGS="$CPPFLAGS $ROSE_CPPFLAGS"
		export CPPFLAGS
		LDFLAGS="$LDFLAGS $ROSE_LDFLAGS"
		export LDFLAGS

	fi

	if test "$succeeded" != "yes" ; then
		if test "$_version" = "0" ; then
			AC_MSG_ERROR([[We could not detect the rose libraries (version $rose_lib_version_req_shorten or higher). If you have a staged rose library (still not installed) please specify \$ROSE_ROOT in your environment and do not give a PATH to --with-rose option.  If you are sure you have rose installed, then check your version number looking in <rose_paths.h>. See http://randspringer.de/rose for more documentation.]])
		else
			AC_MSG_NOTICE([Your rose libraries seems to old (version $_version).])
		fi
	else
		AC_SUBST(ROSE_CPPFLAGS)
		AC_SUBST(ROSE_LDFLAGS)
		AC_DEFINE(HAVE_ROSE,,[define if the ROSE library is available])
	fi

        CPPFLAGS="$CPPFLAGS_SAVED"
       	LDFLAGS="$LDFLAGS_SAVED"
fi

])

#
# SYNOPSIS
#
#   AX_ROSE_LIB
#
# DESCRIPTION
#
#   Test for program options library from the Boost C++ libraries. The
#   macro requires a preceding call to AX_ROSE_LIB_BASE. Further
#   documentation is available at
#   <http://randspringer.de/rose/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(ROSE_LIB)
#
#   And sets:
#
#     HAVE_ROSE
#
# LAST MODIFICATION
#
#   2007-11-22
#
# COPYLEFT
#
#   Copyright (c) 2007 Thomas Porschberg <thomas@randspringer.de>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_ROSE_LIB],
[
	AC_ARG_WITH([rose-lib],
		AS_HELP_STRING([--with-rose-lib@<:@=special-lib@:>@],
                       [use the program options library from rose - it is possible to specify a certain library for the linker
                        e.g. --with-rose-lib=rose-gcc-mt-1_33_1 ]),
        [
        if test "$withval" = "no"; then
			want_rose="no"
        elif test "$withval" = "yes"; then
            want_rose="yes"
            ax_rose_user_lib=""
        else
		    want_rose="yes"
        	ax_rose_user_lib="$withval"
		fi
        ],
        [want_rose="yes"]
	)

	if test "x$want_rose" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
	    export want_rose
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $ROSE_CPPFLAGS $BOOST_CPPFLAGS "
		export CPPFLAGS
		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $ROSE_LDFLAGS $BOOST_LDFLAGS "
		export LDFLAGS
		AC_CACHE_CHECK([whether the Boost::Program_Options library is available],
					   ax_cv_rose,
					   [AC_LANG_PUSH(C++)
 		                AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[@%:@include <rose.h>
                                                          ]],
                                  [[ return 0;]]),
                           ax_cv_rose=yes, ax_cv_rose=no)
			               	AC_LANG_POP([C++])
		])
		if test "$ax_cv_rose" = yes; then
				AC_DEFINE(HAVE_ROSE,,[define if the Boost::PROGRAM_OPTIONS library is available])
                  ROSELIBDIR=`echo $ROSE_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
                if test "x$ax_rose_user_lib" = "x"; then
                for libextension in `ls $ROSELIBDIR/librose*.{so,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(rose.*\)\.so.*$;\1;' -e 's;^lib\(rose.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [ROSE_LIB="-l$ax_lib"; AC_SUBST(ROSE_LIB) link="yes"; break],
                                 [link="no"])
  				done
                if test "x$link" != "xyes"; then
                for libextension in `ls $ROSELIBDIR/rose*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(rose.*\)\.dll.*$;\1;' -e 's;^\(rose.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [ROSE_LIB="-l$ax_lib"; AC_SUBST(ROSE_LIB) link="yes"; break],
                                 [link="no"])
  				done
                fi
                else
                  for ax_lib in $ax_rose_user_lib rose-$ax_rose_user_lib; do
				      AC_CHECK_LIB($ax_lib, main,
                                   [ROSE_LIB="-l$ax_lib"; AC_SUBST(ROSE_LIB) link="yes"; break],
                                   [link="no"])
                  done
                fi
				if test "x$link" != "xyes"; then
					AC_MSG_ERROR([Could not link against [$ax_lib] !])
				fi
		fi
		CPPFLAGS="$CPPFLAGS_SAVED"
    	LDFLAGS="$LDFLAGS_SAVED"
	fi
])

# ===========================================================================
#              http://autoconf-archive.cryp.to/ax_with_prog.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_WITH_PROG([VARIABLE],[program],[VALUE-IF-NOT-FOUND],[PATH])
#
# DESCRIPTION
#
#   Locates an installed program binary, placing the result in the precious
#   variable VARIABLE. Accepts a present VARIABLE, then --with-program, and
#   failing that searches for program in the given path (which defaults to
#   the system path). If program is found, VARIABLE is set to the full path
#   of the binary; if it is not found VARIABLE is set to VALUE-IF-NOT-FOUND
#   if provided, unchanged otherwise.
#
#   A typical example could be the following one:
#
#         AX_WITH_PROG(PERL,perl)
#
#   NOTE: This macro is based upon the original AX_WITH_PYTHON macro from
#   Dustin J. Mitchell <dustin@cs.uchicago.edu>.
#
# LAST MODIFICATION
#
#   2008-05-05
#
# COPYLEFT
#
#   Copyright (c) 2008 Francesco Salvestrini <salvestrini@users.sourceforge.net>
#   Copyright (c) 2008 Dustin J. Mitchell <dustin@cs.uchicago.edu>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

AC_DEFUN([AX_WITH_PROG],[
    AC_PREREQ([2.59])

    pushdef([VARIABLE],$1)
    pushdef([EXECUTABLE],$2)
    pushdef([VALUE_IF_NOT_FOUND],$3)
    pushdef([PATH_PROG],$4)

    AC_ARG_VAR(VARIABLE,Absolute path to EXECUTABLE executable)

    AS_IF(test -z "$VARIABLE",[
    	AC_MSG_CHECKING(whether EXECUTABLE executable path has been provided)
        AC_ARG_WITH(EXECUTABLE,AS_HELP_STRING([--with-EXECUTABLE=[[[[PATH]]]]],absolute path to EXECUTABLE executable), [
	    AS_IF([test "$withval" != "yes"],[
	        VARIABLE="$withval"
		AC_MSG_RESULT($VARIABLE)
	    ],[
		VARIABLE=""
	        AC_MSG_RESULT([no])
	    ])
	],[
	    AC_MSG_RESULT([no])
	])

        AS_IF(test -z "$VARIABLE",[
	    AC_PATH_PROG([]VARIABLE[],[]EXECUTABLE[],[]VALUE_IF_NOT_FOUND[],[]PATH_PROG[])
        ])
    ])

    popdef([PATH_PROG])
    popdef([VALUE_IF_NOT_FOUND])
    popdef([EXECUTABLE])
    popdef([VARIABLE])
])

AC_DEFUN([CHOOSE_BACKEND_COMPILER],
dnl Written by Dan Quinlan, 12/17/2001
dnl This macro selects the back-end C++ compiler to use to compile output 
dnl generated by preprocessors build using ROSE.  This macro needs to be called
dnl before the GET_COMPILER_SPECIFIC_DEFINES macro is called (so that defines 
dnl for the correct back-end C++ compiler are identified for use in preprocessors 
dnl build using ROSE)
[
# Make sure that we select a backend compiler before building the backend specific header files
# AC_BEFORE([CHOOSE_BACKEND_COMPILER],[GENERATE_BACKEND_COMPILER_SPECIFIC_HEADERS])
  AC_BEFORE([CHOOSE_BACKEND_COMPILER],[GENERATE_BACKEND_CXX_COMPILER_SPECIFIC_HEADERS])
  AC_ARG_WITH(alternate_backend_Cxx_compiler,
    [  --with-alternate_backend_Cxx_compiler=<compiler name>
                                Specify an alternative C++ back-end compiler],
    [
    # Use a different compiler for the backend than for the compilation of ROSE source code
      BACKEND_CXX_COMPILER=$with_alternate_backend_Cxx_compiler

      echo "alternative back-end C++ compiler specified for generated translators to use: $BACKEND_CXX_COMPILER"
    ] ,
    [ 
    # Alternatively use the specified C++ compiler
	   BACKEND_CXX_COMPILER="$CXX"
      echo "default back-end C++ compiler for generated translators to use: $BACKEND_CXX_COMPILER"
    ])

  AC_ARG_WITH(alternate_backend_C_compiler,
    [  --with-alternate_backend_C_compiler=<compiler name>
                                Specify an alternative C back-end compiler],
    [
    # Use a different compiler for the backend than for the compilation of ROSE source code
      BACKEND_C_COMPILER=$with_alternate_backend_C_compiler
      echo "alternative back-end C compiler specified for generated translators to use: $BACKEND_C_COMPILER"
    ] ,
    [ 
    # Alternatively use the specified C compiler
	   BACKEND_C_COMPILER="$CC"
      echo "default back-end C compiler for generated translators to use: $BACKEND_C_COMPILER"
    ])

# DQ (10/3/2008): Added option to specify backend fortran compiler
  AC_ARG_WITH(alternate_backend_fortran_compiler,
    [  --with-alternate_backend_fortran_compiler=<compiler name>
                                Specify an alternative fortran back-end compiler],
    [
    # Use a different compiler for the backend than for the compilation of ROSE source code
      BACKEND_FORTRAN_COMPILER=$with_alternate_backend_fortran_compiler
      echo "alternative back-end fortran compiler specified for generated translators to use: $BACKEND_FORTRAN_COMPILER"
    ] ,
    [ 
    # Alternatively use the specified fortran compiler
	 # BACKEND_FORTRAN_COMPILER="$FC"
	   BACKEND_FORTRAN_COMPILER="gfortran"
      echo "default back-end fortran compiler for generated translators to use: $BACKEND_FORTRAN_COMPILER"
    ])

# DQ (8/29/2005): Added support for version numbering of backend compiler
  BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f1`
  BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f2`

# echo "back-end compiler for generated translators to use will be: $BACKEND_CXX_COMPILER"
  echo "     C++ back-end compiler major version number = $BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER"
  echo "     C++ back-end compiler minor version number = $BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER"

# Use this to get the major and minor version numbers for gfortran (which maps --version to -dumpversion, unlike gcc and g++)
# gfortran --version | head -1 | cut -f2 -d\) | tr -d \  | cut -d\. -f2
# Or Jeremiah suggests the alternative:
# gfortran --version | sed -n '1s/.*) //;1p'
  echo "BACKEND_FORTRAN_COMPILER = $BACKEND_FORTRAN_COMPILER"

# Testing the 4.0.x compiler
# BACKEND_FORTRAN_COMPILER="/usr/apps/gcc/4.0.2/bin/gfortran"
# echo "BACKEND_FORTRAN_COMPILER = $BACKEND_FORTRAN_COMPILER"

# DQ (9/15/2009): Normally we expect a string such as "GNU Fortran 95 (GCC) 4.1.2", but 
# the GNU 4.0.x compiler's gfortran outputs a string such as "GNU Fortran 95 (GCC 4.0.2)"
# So for this case we detect it explicitly and fill in the values directly!
  BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER=`echo|$BACKEND_FORTRAN_COMPILER --version | head -1 | cut -f2 -d\) | tr -d \  | cut -d\. -f1`
  BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER=`echo|$BACKEND_FORTRAN_COMPILER --version | head -1 | cut -f2 -d\) | tr -d \  | cut -d\. -f2`

# Test if we computed the major and minor version numbers correctly...recompute if required
  if test x$BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER == x; then
    echo "Warning: BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER = $BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER (blank) so this is likely the GNU 4.0.x version (try again to get the version number)"
    BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER=`echo|$BACKEND_FORTRAN_COMPILER --version | head -1 | sed s/"GNU Fortran 95 (GCC "//g | cut -f1 -d \) | cut -d\. -f1`
    BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER=`echo|$BACKEND_FORTRAN_COMPILER --version | head -1 | sed s/"GNU Fortran 95 (GCC "//g | cut -f1 -d \) | cut -d\. -f2`
  fi

# echo "back-end compiler for generated translators to use will be: $BACKEND_CXX_COMPILER"
  echo "     Fortran back-end compiler major version number = $BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER"
  echo "     Fortran back-end compiler minor version number = $BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER"

# Test that we have correctly evaluated the major and minor versions numbers...
  if test x$BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER == x; then
    echo "Warning: Could not compute the MAJOR version number of $BACKEND_FORTRAN_COMPILER"
  # exit 1
  fi

  if test x$BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER == x; then
    echo "Warning: Could not compute the MINOR version number of $BACKEND_FORTRAN_COMPILER"
  # exit 1
  fi

# DQ (9/16/2009): GNU gfortran 4.0 has special problems so we avoid some tests where it fails.
  gfortran_version_4_0=no
  if test x$BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER == x4; then
     if test x$BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER == x0; then
        echo "Note: we have identified version 4.0 of gfortran!"
        gfortran_version_4_0=yes
     fi
  fi
  AM_CONDITIONAL(ROSE_USING_GFORTRAN_VERSION_4_0, [test "x$gfortran_version_4_0" = "xyes"])

# DQ (9/17/2009): GNU gfortran 4.1 has special problems so we avoid some tests where it fails.
  gfortran_version_4_1=no
  if test x$BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER == x4; then
     if test x$BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER == x1; then
        echo "Note: we have identified version 4.1 of gfortran!"
        gfortran_version_4_1=yes
     fi
  fi
  AM_CONDITIONAL(ROSE_USING_GFORTRAN_VERSION_4_1, [test "x$gfortran_version_4_1" = "xyes"])

# echo "Exiting after test of backend version number support ..."
# exit 1

# We use the name of the backend C++ compiler to generate a compiler name that will be used
# elsewhere (CXX_ID might be a better name to use, instead we use basename to strip the path).
# compilerName=`basename $BACKEND_CXX_COMPILER`
  COMPILER_NAME=`basename $BACKEND_CXX_COMPILER`
# echo "default back-end compiler for generated preprocessors will be: $BACKEND_CXX_COMPILER"
# export BACKEND_CXX_COMPILER
# AC_DEFINE_UNQUOTED([CXX_COMPILER_NAME],"$BACKEND_CXX_COMPILER",[Name of backend C++ compiler.])
# echo "default back-end compiler for generated preprocessors will be: $BACKEND_CXX_COMPILER compiler name = $compilerName"

# DQ (1/15/2007): This does not work, it seems that BACKEND_C_COMPILER must be a simple name not a compound name using an option!
# Specify any option that specific backend compiler require (e.g. -restrict)
  case $COMPILER_NAME in
    gcc|g++)
      ;;
    icc|icpc)
    # BACKEND_C_COMPILER="$BACKEND_C_COMPILER -restrict"
    # BACKEND_CXX_COMPILER="$BACKEND_CXX_COMPILER -restrict"
      ;;
    "KCC --c" | mpKCC|KCC)
      ;;
    cc|CC)
    ;;
  esac

  echo "After adding (required) options BACKEND_C_COMPILER   = $BACKEND_C_COMPILER"
  echo "After adding (required) options BACKEND_CXX_COMPILER = $BACKEND_CXX_COMPILER"

  echo "default back-end compiler for generated preprocessors will be: $BACKEND_CXX_COMPILER compiler name = $COMPILER_NAME"

# export BACKEND_CXX_COMPILER
# AC_DEFINE_UNQUOTED([CXX_COMPILER_NAME],"$BACKEND_CXX_COMPILER",[Name of backend C++ compiler.])

# This will be used to select options based on which backend compiler is used (g++, xlC, icc, etc.)
# we can't use the basename of the compiler to execute because it might be link using a non-standard name (e.g. mpig++-3.4.1)
  export COMPILER_NAME
  AC_DEFINE_UNQUOTED([BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH],"$COMPILER_NAME",[Name of backend C++ compiler excluding path (used to select code generation options).])

# This will be called to execute the backend compiler (for C++)
  export BACKEND_CXX_COMPILER
  AC_DEFINE_UNQUOTED([BACKEND_CXX_COMPILER_NAME_WITH_PATH],"$BACKEND_CXX_COMPILER",[Name of backend C++ compiler including path (may or may not explicit include path; used to call backend).])

# This will be called to execute the backend compiler (for C)
  export BACKEND_C_COMPILER
  AC_DEFINE_UNQUOTED([BACKEND_C_COMPILER_NAME_WITH_PATH],"$BACKEND_C_COMPILER",[Name of backend C compiler including path (may or may not explicit include path; used to call backend).])

# This will be called to execute the backend compiler (for C)
  export BACKEND_FORTRAN_COMPILER
  AC_DEFINE_UNQUOTED([BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH],"$BACKEND_FORTRAN_COMPILER",[Name of backend Fortran compiler including path (may or may not explicit include path; used to call backend).])

# These are useful in handling differences between different versions of the backend compiler
# we assume that the C and C++ compiler version number match and only record version information 
# for the backend C++ compiler. (for example, this helps us generated different code for 
# g++ 3.3.x and 3.4.x backend compilers).
  export BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER
  AC_DEFINE_UNQUOTED([BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER],$BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER,[Major version number of backend C++ compiler.])
  export BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER
  AC_DEFINE_UNQUOTED([BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER],$BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER,[Minor version number of backend C++ compiler.])

  export BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER
  AC_DEFINE_UNQUOTED([BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER],$BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER,[Major version number of backend Fortran compiler.])
  export BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER
  AC_DEFINE_UNQUOTED([BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER],$BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER,[Minor version number of backend Fortran compiler.])
])


AC_DEFUN([GET_COMPILER_SPECIFIC_DEFINES],
dnl First written by Bobby Philip, 11/25/2001
dnl define GET_COMPILER_SPECIFIC_DEFINES.
dnl this macro tries to automatically
dnl determine the macros that are
dnl predefined by a specific compiler
[
# AC_LANG_SAVE
# AC_LANG_CPLUSPLUS
  AC_LANG_PUSH(C++)
  changequote(<<, >>) dnl For some reason the quotes '' below are being thrown away, so do this
# Use the selected back-end C++ compiler instead of $CXX
# case $CXX in

# DQ (12/2/2006): This macro is required even because it sets up the CXX_SPEC_DEF and C_SPEC_DEF macros in rose_config.h (generated by autoheader).
# echo "This GET COMPILER SPECIFIC DEFINES macro is no longer used, see create_system_headers file instead!"
# exit 1

# DQ (4/1/2005): Modified to get compiler name from path
  compilerName=`basename $BACKEND_CXX_COMPILER`
  echo "Using back-end C++ compiler = \"$BACKEND_CXX_COMPILER\" compiler name = \"$compilerName\" for processing of unparsed source files from ROSE preprocessors."

# case $BACKEND_CXX_COMPILER in
  case $compilerName in

  # Support for GNU gcc or g++ as a backend for compiling ROSE generated code
  # JJW 7/25/2008: Added mpi* in here, assuming they are like gcc
    g++|gcc|mpicc|mpic++|mpicxx|mpiCC)
             BACKEND_GCC_MAJOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f1`
             BACKEND_GCC_MINOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f2`
             BACKEND_GCC_PATCHLEVEL=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f3`
             macroString="{\"-D__GNUG__=$BACKEND_GCC_MAJOR\", \"-D__GNUC__=$BACKEND_GCC_MAJOR\", \"-D__GNUC_MINOR__=$BACKEND_GCC_MINOR\", \"-D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL\""
             if test x$enable_new_edg_interface = xyes; then
               :
             else
               macroString="${macroString}, \"--preinclude\", \"rose_edg_required_macros_and_functions.h\""
             fi
             macroString="${macroString}}"

             compilerVendorName=GNU
             ;;

  # Support for Intel icc as a backend for compiling ROSE generated code
    icpc|icc)

             tmpFile="/tmp/tmpICCMacroExtraction`uname -n`$$.C"
             echo "int main(int argc, char **argv){return 0;}" > "$tmpFile"
             extra_icc_defines=`"icpc" -# "test.C" 2>&1 |  grep "\-D" | grep "GNUG\|__GNUC_PATCHLEVEL__\|__GNUC_MINOR__\|__GNUC__"  | sed ':a; /\\$/N; s/\\\n//; ta' | sed 's/\\\//' `
             tmp_macro=""
             for macro_i in $extra_icc_defines
                 do
                    echo $macro_i
                    tmp_macro="$tmp_macro, $macro_i"
                    echo " tmp_macro  $tmp_macro"
                 done
           # macroString=" -D__PURE_INTEL_C99_HEADERS__ ${tmp_macro} --preinclude rose_edg_macros_and_functions_required_for_icc.h "
             macroString="{\"-D__PURE_INTEL_C99_HEADERS__\" ${tmp_macro}"
             if test x$enable_new_edg_interface = xyes; then
               :
             else
               macroString="${macroString}, \"--preinclude\", \"rose_edg_required_macros_and_functions.h\""
             fi
             macroString="${macroString}}"
              echo "ICC MACRO DEFS: $ICC_MACRO_DEFS"
             echo "macroString: $macroString"

             compilerVendorName=Intel

             #BACKEND_GCC_MAJOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f1`
             #BACKEND_GCC_MINOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f2`
             #BACKEND_GCC_PATCHLEVEL=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f3` 
             ;;

  # Support for ROSE "roseTranslator" as a backend for compiling ROSE generated code
  # Or support "roseAnalysis" as a backend which generates object files from the original source code.
    roseTranslator|roseAnalysis)
           # macroString=" -D__GNUG__=$BACKEND_GCC_MAJOR -D__GNUC__=$BACKEND_GCC_MAJOR -D__GNUC_MINOR__=$BACKEND_GCC_MINOR -D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL -D_GNU_SOURCE --preinclude rose_edg_macros_and_functions_required_for_gnu.h "
           # macroString="{\"-D__GNUG__=$BACKEND_GCC_MAJOR\", \"-D__GNUC__=$BACKEND_GCC_MAJOR\", \"-D__GNUC_MINOR__=$BACKEND_GCC_MINOR\", \"-D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL\", \"-D_GNU_SOURCE\""
           # if test x$enable_new_edg_interface = xyes; then
           #   :
           # else
           #   macroString="${macroString}, \"--preinclude\", \"rose_edg_required_macros_and_functions.h\""
           # fi
           # macroString="${macroString}}"
           # BACKEND_GCC_MAJOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f1`
           # BACKEND_GCC_MINOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f2`
           # BACKEND_GCC_PATCHLEVEL=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f3`
           #
           # echo "This is the case of using roseTranslator to compile ROSE.";
           # exit 1;
           # ;;
           #
           # DQ (8/22/2009): This is the same as the g++ case above (we can us that because rose acts like g++ by default).
             BACKEND_GCC_MAJOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f1`
             BACKEND_GCC_MINOR=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f2`
             BACKEND_GCC_PATCHLEVEL=`echo|$BACKEND_CXX_COMPILER -dumpversion | cut -d\. -f3`
             macroString="{\"-D__GNUG__=$BACKEND_GCC_MAJOR\", \"-D__GNUC__=$BACKEND_GCC_MAJOR\", \"-D__GNUC_MINOR__=$BACKEND_GCC_MINOR\", \"-D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL\""
             if test x$enable_new_edg_interface = xyes; then
               :
             else
               macroString="${macroString}, \"--preinclude\", \"rose_edg_required_macros_and_functions.h\""
             fi
             macroString="${macroString}}"

             compilerVendorName=GNU
             ;;

    *)
             echo "we reached here for some reason (cannot identify back-end C++ compiler \"$BACKEND_CXX_COMPILER\")";
             exit 1;
             macroString="";;
  esac

  changequote([, ])
  AC_DEFINE_UNQUOTED([CXX_SPEC_DEF],$macroString,[-D options to hand to EDG C++ front-end.])

# This is now setup in a separate macro and can be specified on the command line
# AC_DEFINE_UNQUOTED(CXX_COMPILER_NAME, "$CXX")

# This string has all compiler specific predefined macros listed
  echo "Backend compiler specific macroString = $macroString"

# DQ (9/13/2009): Add information about the backend compiler (which matches the associated header files generated).
# These values are used to control tests that are backend compiler and or compiler version dependent.
  BACKEND_COMPILER_VERSION_NAME=$compilerVendorName
  BACKEND_COMPILER_VERSION_MAJOR_NUMBER=$BACKEND_GCC_MAJOR
  BACKEND_COMPILER_VERSION_MINOR_NUMBER=$BACKEND_GCC_MINOR
  BACKEND_COMPILER_VERSION_PATCHLEVEL_NUMBER=$BACKEND_GCC_PATCHLEVEL

# DQ (9/14/2009): generate a name to use with the name of the ROSE EDG binary so that we can be version number specific.
  GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX="$BACKEND_COMPILER_VERSION_NAME-$BACKEND_COMPILER_VERSION_MAJOR_NUMBER.$BACKEND_COMPILER_VERSION_MINOR_NUMBER"

  if test x$BACKEND_COMPILER_VERSION_NAME = xGNU; then
    if test x$BACKEND_COMPILER_VERSION_MAJOR_NUMBER = x4; then
      if test x$BACKEND_COMPILER_VERSION_MINOR_NUMBER = x2; then
        ok_for_testing=true
      fi
    fi
  fi

  echo "BACKEND_COMPILER_VERSION_NAME              = $BACKEND_COMPILER_VERSION_NAME"
  echo "BACKEND_COMPILER_VERSION_MAJOR_NUMBER      = $BACKEND_COMPILER_VERSION_MAJOR_NUMBER"
  echo "BACKEND_COMPILER_VERSION_MINOR_NUMBER      = $BACKEND_COMPILER_VERSION_MINOR_NUMBER"
  echo "BACKEND_COMPILER_VERSION_PATCHLEVEL_NUMBER = $BACKEND_COMPILER_VERSION_PATCHLEVEL_NUMBER"

  echo "GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX = $GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX"

  echo "This version of backend compiler is OK for compiler and compiler version dependent testing: ok_for_testing = $ok_for_testing"

  AM_CONDITIONAL(BACKEND_COMPILER_VERSION_OK_FOR_ROSE_TESTING,test "x$ok_for_testing" = xtrue)

# DQ (9/30/2009): Exempt a set of old compiler versions from some testing
# This is an attept to define a class of compiler versions that 
# we should not test because it can generate internal compiler errors.
# Specifically I am turning off the projects/interpretation directory
# when using GNU version 3.4 (this is an example of this class).
  if test x$BACKEND_COMPILER_VERSION_NAME = xGNU; then
    if test x$BACKEND_COMPILER_VERSION_MAJOR_NUMBER = x3; then
      if test x$BACKEND_COMPILER_VERSION_MINOR_NUMBER = x4; then
        echo "Warning: This is classified as an OLD compiler to ROSE (some parts of ROSE will not be compiled)."
        old_compiler=true
      fi
    fi
  fi

  echo "This version of backend compiler is condidered too old for parts of ROSE: old_compiler = $old_compiler"

  AM_CONDITIONAL(OLD_COMPILER_VERSION,test "x$old_compiler" = xtrue)


  AC_SUBST(BACKEND_COMPILER_VERSION_NAME)
  AC_SUBST(BACKEND_COMPILER_VERSION_MAJOR_NUMBER)
  AC_SUBST(BACKEND_COMPILER_VERSION_MINOR_NUMBER)
  AC_SUBST(BACKEND_COMPILER_VERSION_PATCHLEVEL_NUMBER)

  AC_SUBST(GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX)


# AC_LANG_RESTORE
  AC_LANG_POP(C++)
])













#
# SYNOPSIS
#
#   DETERMINE_OS
#
# DESCRIPTION
#
#   Determine which OS the build is configured in.
#
#   This macro sets:
#
#     OS_LINUX
#

AC_DEFUN([DETERMINE_OS],
    [

    case $build_os in
    linux*)
      LINUX=yes;
  ;;
  cygwin)
  MINGW=yes
  ;;
  mingw*)
  MINGW32=yes
  ;;
  darwin*)
  MACOSX=yes
  ;;
  msdos*)
  MSDOS=yes
  ;;
  esac
    AM_CONDITIONAL([OS_MACOSX],[ test "x$MACOSX" = xyes ] )
    AM_CONDITIONAL([OS_LINUX], [ test "x$LINUX"  = xyes ] )
    AM_CONDITIONAL([OS_MINGW], [ test "x$MINGW"  = xyes ] )
    AM_CONDITIONAL([OS_MSDOS], [ test "x$MSDOS"  = xyes ] )

  ])


# /etc/redhat-release
# /etc/debian_version
# /etc/SuSE-release
# /etc/slackware-version
# /etc/gentoo-release
# You could do 'cat /etc/*-release' or 'cat /etc/*-version'.

# DQ (9/10/2009): This is a macro I wrote to nail down the OS vendor so that
# I could skip specific tests in ROSE based on the OS vendor as part of the
# NMI testing of ROSE on the Compile Farm.
AC_DEFUN([DETERMINE_OS_VENDOR],
[
   echo "Try to identify the OS vendor...";
   AC_CHECK_TOOL(ROSE_LSB_RELEASE, [lsb_release], [no])

   OS_vendor="ROSE_unknown_OS";
   OS_release="ROSE_unknown_OS_release";

#  if test -z "$ROSE_LSB_RELEASE"; then
   if test "x$ROSE_LSB_RELEASE" = xno; then
      echo "********************************";
      echo "* lsb_release is NOT available *";
      echo "********************************";
    # Most OS's output their name buried in /etc/issue
      ls -dl /etc/*-release /etc/*-version;
      echo "***************************";
      echo "* Output /etc/issue file: *";
      cat /etc/issue
      echo "***************************";

    # For at least Apple Mac OSX, there is no lsb_release program or /etc/*-release /etc/*-version
    # files but autoconf will guess the vendor and the OS release correctly (so use those vaules).
      echo "Autoconf computed value for cpu       = $build_cpu"
      echo "Autoconf computed value for OS vendor = $build_vendor"
      echo "Autoconf computed value for OS        = $build_os"
      echo "***************************";

    # Fix the case of Apple OSX support.
      if test "x$build_vendor" = xapple; then
         OS_vendor=$build_vendor
         case $build_os in
            darwin8*)
               OS_release=10.4
               ;;
            darwin9*)
               OS_release=10.5
               ;;
            darwin10*)
               OS_release=10.6
               ;;
            *)
             echo "Error: Apple Mac OSX version not recognized as either darwin8 or darwin9 ... (build_os = $build_os)";
             exit 1;
             OS_release="";;
         esac
         echo "Identified Apple OSX platform OS_vendor = $OS_vendor OS_release = $OS_release"
      fi
    # exit 1
   else
      echo "lsb_release IS available ROSE_LSB_RELEASE = $ROSE_LSB_RELEASE";
      OS_vendor=`lsb_release -is`
      OS_release=`lsb_release -rs`

      echo "In conditional: OS_vendor  = $OS_vendor"
      echo "In conditional: OS_release = $OS_release"
   fi

   echo "In conditional: OS_vendor  = $OS_vendor"
   echo "In conditional: OS_release = $OS_release"

   case $OS_vendor in
      Debian*)
         DEBIAN=yes
         ;;
      RedHat*)
         REDHAT=yes
         ;;
      Ubuntu*)
         UBUNTU=yes
         ;;
      CentOS*)
         CENTOS=yes
         ;;
      apple*)
         APPLE=yes
         ;;
      esac
         AM_CONDITIONAL([OS_VENDOR_DEBIAN],[ test "x$DEBIAN" = xyes ] )
         AM_CONDITIONAL([OS_VENDOR_REDHAT],[ test "x$REDHAT" = xyes ] )
         AM_CONDITIONAL([OS_VENDOR_UBUNTU],[ test "x$UBUNTU" = xyes ] )
         AM_CONDITIONAL([OS_VENDOR_CENTOS],[ test "x$CENTOS" = xyes ] )
         AM_CONDITIONAL([OS_VENDOR_APPLE],[ test "x$APPLE"  = xyes ] )

       # Added conditionals for 32-bit vs. 64-bit OS (used only in the binary analysis work -- and rarely).
         AM_CONDITIONAL([OS_32BIT],[ test "x$build_cpu" = xi686 ] )
         AM_CONDITIONAL([OS_64BIT],[ test "x$build_cpu" = xx86_64 ] )

         AM_CONDITIONAL([OS_VENDOR_REDHAT_32BIT],[ test "x$REDHAT" = xyes -a "x$build_cpu" = xi686 ] )

   echo "Leaving DETERMINE OS VENDOR: OS_vendor  = $OS_vendor"
   echo "Leaving DETERMINE OS VENDOR: OS_release = $OS_release"

   AC_SUBST(OS_vendor)
   AC_SUBST(OS_release)

  ])


dnl
dnl ACROSE_ENABLE_HPCT
dnl
dnl Must call this after AM_PATH_XML2(2.0.0, [with_xml="yes"])
dnl
dnl Checks whether the user explicitly requested that the
dnl ROSE-HPCT module be built. Sets the shell variable
dnl 'acrose_enable_rosehpct' to 'yes' or 'no', accordingly.
dnl Also defines the automake conditional,
dnl ROSE_BUILD_ROSEHPCT, and modifies CXXFLAGS and
dnl LIBS to include and link against the appropriate
dnl prerequisite libraries.
dnl  
dnl Enabled it by default if prerequisites are met, Liao, 8/7/2008 
dnl
dnl Author: Rich Vuduc <richie@llnl.gov>
dnl
dnl $Id: enable-rosehpct.m4,v 1.1 2005/10/23 07:52:04 vuduc2 Exp $
dnl

AC_DEFUN([ACROSE_ENABLE_ROSEHPCT], [

AC_MSG_CHECKING([whether user wants to build ROSE-HPCT])
AC_ARG_ENABLE([rosehpct],
  AC_HELP_STRING([--enable-rosehpct],
    [enable build of the ROSE-HPCT module]),
  [acrose_enable_rosehpct=$enableval],
  [acrose_enable_rosehpct=yes]
  )
AC_MSG_RESULT([${acrose_enable_rosehpct}])

if test x"${acrose_enable_rosehpct}" = xyes ; then
  ACROSE_CHECK_ROSEHPCT([], [acrose_enable_rosehpct=no])
fi

AM_CONDITIONAL(ROSE_BUILD_ROSEHPCT,
  test x"${acrose_enable_rosehpct}" = xyes)
])

dnl
dnl ACROSE_CHECK_ROSEHPCT([ACTION-IF-OK], [ACTION-ELSE])
dnl
dnl Checks for all the prerequisite libraries needed to
dnl build the ROSE-HPCT module. Exports the automake
dnl variables ROSEHPCT_DEP_INCLUDES and ROSEHPCT_DEP_LIBS
dnl to specify include paths and linker flags needed to
dnl build ROSE-HPCT and its example programs.
dnl
AC_DEFUN([ACROSE_CHECK_ROSEHPCT], [
AC_LANG_PUSH([C++])

AC_MSG_NOTICE([checking for ROSE-HPCT build-prerequisites])

if test x"${with_xml}" = xyes ; then
  AC_MSG_NOTICE([all known ROSE-HPCT build-prerequisites satisfied])
  $1
else  # At least one prerequisite failed
  :
  AC_MSG_NOTICE([one or more ROSE-HPCT build-prerequisites not satisfied])
  $2
fi

ROSEHPCT_DEP_INCLUDES="${XML_CPPFLAGS}"
ROSEHPCT_DEP_LIBS="${XML_LIBS}"

AC_SUBST([ROSEHPCT_DEP_INCLUDES])
AC_SUBST([ROSEHPCT_DEP_LIBS])
AC_LANG_POP([C++])
])

dnl eof

AC_DEFUN([GENERATE_BACKEND_CXX_COMPILER_SPECIFIC_HEADERS],
dnl DQ 12/17/2001 build from what Bobby put into configure.in directly 11/25/2001
dnl This builds the directories required for the back-end compiler specific header files.
dnl it depends upon the CHOOSE BACKEND COMPILER macro to have already been called.
[
 # BP : 11/20/2001, create a directory to store header files which are compiler specific
   compilerName="`basename $BACKEND_CXX_COMPILER`"
   chmod u+x "${srcdir}/config/create_system_headers"
   if test "$ROSE_CXX_HEADERS_DIR" = ""; then
      dnl AC_MSG_NOTICE([ROSE_CXX_HEADERS_DIR not set ...])
      ROSE_CXX_HEADERS_DIR="${prefix}/include/${compilerName}_HEADERS"
   else
      AC_MSG_NOTICE([ROSE_CXX_HEADERS_DIR set to: $ROSE_CXX_HEADERS_DIR])
   fi

   saveCurrentDirectory="`pwd`"
   cd "$srcdir"
   absolutePath_srcdir="`pwd`"
   cd "$saveCurrentDirectory"

 # DQ (9/1/2009): Output the absolute path
   echo "absolutePath_srcdir = ${absolutePath_srcdir}"

 # Use the full path name to generate the header from the correctly specified version of the backend compiler
   mkdir -p "./include-staging/${BACKEND_CXX_COMPILER}_HEADERS"
   "${srcdir}/config/create_system_headers" "${BACKEND_CXX_COMPILER}" "./include-staging/${BACKEND_CXX_COMPILER}_HEADERS" "${absolutePath_srcdir}"
])


AC_DEFUN([SETUP_BACKEND_CXX_COMPILER_SPECIFIC_REFERENCES],
dnl DQ 12/17/2001 build from what Bobby put into configure.in directly 11/25/2001
dnl This builds the directories required for the back-end compiler specific header files.
dnl it depends upon the CHOOSE BACKEND COMPILER macro to have already been called.
[
 # Now setup the include path that we will prepend to any user -I<dir> options so that the 
 # required compiler-specific header files can be found (these are often relocated versions 
 # of the compiler specific header files that have been processed so that EDG can read them)
 # It is unfortunate, but many compiler-specific files include compiler-specific code which
 # will not compile with a standard C++ compiler or can not be processed using a standard
 # C preprocessor (cpp) (an ugly fact of common compilers).

   chmod u+x "${srcdir}/$ROSE_HOME/config/dirincludes"

 # Include the directory with the subdirectories of header files
   if test "x$enable_new_edg_interface" = "xyes"; then
     includeString="{`${srcdir}/config/get_compiler_header_dirs ${BACKEND_CXX_COMPILER} | while read dir; do echo -n \\\"$dir\\\",\ ; done` \"/usr/include\"}"
   else
     includeString="{\"${BACKEND_CXX_COMPILER}_HEADERS\"`${srcdir}/$ROSE_HOME/config/dirincludes "./include-staging/" "${BACKEND_CXX_COMPILER}_HEADERS"`, \"/usr/include\"}"
   fi

   echo "includeString = $includeString"
   AC_DEFINE_UNQUOTED([CXX_INCLUDE_STRING],$includeString,[Include path for backend C++ compiler.])
])

AC_DEFUN([GENERATE_BACKEND_C_COMPILER_SPECIFIC_HEADERS],
[
   compilerName="`basename $BACKEND_C_COMPILER`"

   echo "C compilerName = ${compilerName}"

   chmod u+x "${srcdir}/config/create_system_headers"

   if test "$ROSE_C_HEADERS_DIR" = ""; then
      dnl AC_MSG_NOTICE([ROSE_C_HEADERS_DIR not set ...])
      ROSE_C_HEADERS_DIR="${compilerName}_HEADERS"
   else
      AC_MSG_NOTICE([ROSE_C_HEADERS_DIR set to: $ROSE_C_HEADERS_DIR])
   fi

   saveCurrentDirectory="`pwd`"
   cd "$srcdir"
   absolutePath_srcdir="`pwd`"
   cd "$saveCurrentDirectory"

 # DQ (9/1/2009): Output the absolute path
   echo "absolutePath_srcdir = ${absolutePath_srcdir}"

 # Use the full path name to generate the header from the correctly specified version of the backend compiler
   mkdir -p "./include-staging/${BACKEND_C_COMPILER}_HEADERS"
   "${srcdir}/config/create_system_headers" "${BACKEND_C_COMPILER}" "./include-staging/${BACKEND_C_COMPILER}_HEADERS" "${absolutePath_srcdir}"

   error_code=$?
   echo "error_code = $error_code"
   if test $error_code != 0; then
        echo "Error in ${srcdir}/config/create_system_headers: nonzero exit code returned to caller error_code = $error_code"
        exit 1
   fi
])


AC_DEFUN([SETUP_BACKEND_C_COMPILER_SPECIFIC_REFERENCES],
dnl DQ 12/17/2001 build from what Bobby put into configure.in directly 11/25/2001
dnl This builds the directories required for the back-end compiler specific header files.
dnl it depends upon the CHOOSE BACKEND COMPILER macro to have already been called.
[
 # Now setup the include path that we will prepend to any user -I<dir> options so that the 
 # required compiler-specific header files can be found (these are often relocated versions 
 # of the compiler specific header files that have been processed so that EDG can read them)
 # It is unfortunate, but many compiler-specific files include compiler-specific code which
 # will not compile with a standard C++ compiler or can not be processed using a standard
 # C preprocessor (cpp) (an ugly fact of common compilers).

   chmod u+x ${srcdir}/$ROSE_HOME/config/dirincludes

 # Include the directory with the subdirectories of header files
   if test "x$enable_new_edg_interface" = "xyes"; then
     includeString="{`${srcdir}/config/get_compiler_header_dirs ${BACKEND_C_COMPILER} | while read dir; do echo -n \\\"$dir\\\",\ ; done` \"/usr/include\"}"
   else
     includeString="{\"${BACKEND_C_COMPILER}_HEADERS\"`${srcdir}/$ROSE_HOME/config/dirincludes "./include-staging/" "${BACKEND_C_COMPILER}_HEADERS"`, \"/usr/include\"}"
   fi

   echo "includeString = $includeString"
   AC_DEFINE_UNQUOTED([C_INCLUDE_STRING],$includeString,[Include path for backend C compiler.])
])


# ===========================================================================
#            http://autoconf-archive.cryp.to/mdl_have_opengl.html
# ===========================================================================
#
# SYNOPSIS
#
#   MDL_HAVE_OPENGL
#
# DESCRIPTION
#
#   Search for OpenGL. We search first for Mesa (a GPL'ed version of Mesa)
#   before a vendor's version of OpenGL, unless we were specifically asked
#   not to with `--with-Mesa=no' or `--without-Mesa'.
#
#   The four "standard" OpenGL libraries are searched for: "-lGL", "-lGLU",
#   "-lGLX" (or "-lMesaGL", "-lMesaGLU" as the case may be) and "-lglut".
#
#   All of the libraries that are found (since "-lglut" or "-lGLX" might be
#   missing) are added to the shell output variable "GL_LIBS", along with
#   any other libraries that are necessary to successfully link an OpenGL
#   application (e.g. the X11 libraries). Care has been taken to make sure
#   that all of the libraries in "GL_LIBS" are listed in the proper order.
#
#   Additionally, the shell output variable "GL_CFLAGS" is set to any flags
#   (e.g. "-I" flags) that are necessary to successfully compile an OpenGL
#   application.
#
#   The following shell variable (which are not output variables) are also
#   set to either "yes" or "no" (depending on which libraries were found) to
#   help you determine exactly what was found.
#
#     have_GL
#     have_GLU
#     have_GLX
#     have_glut
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Matthew D. Langston
#   Copyright (c) 2008 Ahmet Inan <auto@ainan.org>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Macro Archive. When you make and
#   distribute a modified version of the Autoconf Macro, you may extend this
#   special exception to the GPL to apply to your modified version as well.

AC_DEFUN([MDL_HAVE_OPENGL],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

  AC_CACHE_CHECK([for OpenGL], mdl_cv_have_OpenGL,
  [
dnl Check for Mesa first, unless we were asked not to.
    AC_ARG_WITH([--with-Mesa],
                   [Prefer the Mesa library over a vendors native OpenGL library (default=yes)],
                   with_Mesa_help_string)
    AC_ARG_ENABLE(Mesa, $with_Mesa_help_string, use_Mesa=$enableval, use_Mesa=yes)

    if test x"$use_Mesa" = xyes; then
       GL_search_list="MesaGL   GL"
      GLU_search_list="MesaGLU GLU"
      GLX_search_list="MesaGLX GLX"
    else
       GL_search_list="GL  MesaGL"
      GLU_search_list="GLU MesaGLU"
      GLX_search_list="GLX MesaGLX"
    fi

    AC_LANG_SAVE
    AC_LANG_C

dnl If we are running under X11 then add in the appropriate libraries.
if test x"$no_x" != xyes; then
dnl Add everything we need to compile and link X programs to GL_X_CFLAGS
dnl and GL_X_LIBS.
  GL_CFLAGS="$X_CFLAGS"
  GL_X_LIBS="$X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
fi
    GL_save_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$GL_CFLAGS"

    GL_save_LIBS="$LIBS"
    LIBS="$GL_X_LIBS"


    # Save the "AC_MSG_RESULT file descriptor" to FD 8.
    exec 8>&AC_FD_MSG

    # Temporarily turn off AC_MSG_RESULT so that the user gets pretty
    # messages.
    exec AC_FD_MSG>/dev/null

    AC_SEARCH_LIBS(glAccum,          $GL_search_list, have_GL=yes,   have_GL=no)
    AC_SEARCH_LIBS(gluBeginCurve,   $GLU_search_list, have_GLU=yes,  have_GLU=no)
    AC_SEARCH_LIBS(glXChooseVisual, $GLX_search_list, have_GLX=yes,  have_GLX=no)
    AC_SEARCH_LIBS(glutInit,        glut,             have_glut=yes, have_glut=no)



    # Restore pretty messages.
    exec AC_FD_MSG>&8

    if test -n "$LIBS"; then
      mdl_cv_have_OpenGL=yes
      GL_LIBS="$LIBS"
      AC_SUBST(GL_CFLAGS)
      AC_SUBST(GL_LIBS)
    else
      mdl_cv_have_OpenGL=no
      GL_CFLAGS=
    fi

dnl Reset GL_X_LIBS regardless, since it was just a temporary variable
dnl and we don't want to be global namespace polluters.
    GL_X_LIBS=

    LIBS="$GL_save_LIBS"
    CPPFLAGS="$GL_save_CPPFLAGS"

    AC_LANG_RESTORE

dnl bugfix: dont forget to cache this variables, too
    mdl_cv_GL_CFLAGS="$GL_CFLAGS"
    mdl_cv_GL_LIBS="$GL_LIBS"
    mdl_cv_have_GL="$have_GL"
    mdl_cv_have_GLU="$have_GLU"
    mdl_cv_have_GLX="$have_GLX"
    mdl_cv_have_glut="$have_glut"
  ])
  GL_CFLAGS="$mdl_cv_GL_CFLAGS"
  GL_LIBS="$mdl_cv_GL_LIBS"
  have_GL="$mdl_cv_have_GL"
  have_GLU="$mdl_cv_have_GLU"
  have_GLX="$mdl_cv_have_GLX"
  have_glut="$mdl_cv_have_glut"
])
dnl endof bugfix -ainan

dnl ------------------------------------------------------------------------
dnl Find a file (or one of more files in a list of dirs)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_FIND_FILE],
[
$3=NONE
for i in $2;
do
  for j in $1;
  do    
    if test -r "$i/$j"; then
      $3=$i
      break 2
    fi
  done
done
])

AC_DEFUN([AC_FIND_FILE_EXT],
[
$3=NONE

for i in $2;
do
  for j in $1;
  do
    try="ls -l $i/$j*"
    if test=`eval $try 2> /dev/null`; then
      $3=$i
      break 2
    fi
  done
done
])


dnl ------------------------------------------------------------------------
dnl Try to find ROSE headers and libraries.
dnl ------------------------------------------------------------------------
dnl 
dnl AC_DEFUN([AC_PATH_ROSE],
dnl [
dnl    AC_MSG_CHECKING([for ROSE])
dnl 
dnl    AC_ARG_WITH(rose,
dnl     [  --with-rose=DIR   where the ROSE includes are. ],
dnl     [  ac_rose="$withval" ]
dnl    )
dnl 
dnl dnl ---------------------------[check ROSE headers]------------
dnl   rose_incdirs="$ac_rose/include"
dnl   AC_FIND_FILE(rose.h, $rose_incdirs, rose_incdir)
dnl   if test "$rose_incdir" = NONE; then
dnl      AC_MSG_ERROR([ ROSE headers not found,  use --with-rose=DIR ])
dnl   fi
dnl   ROSE_INCLUDES="-I$rose_incdir"
dnl   AC_SUBST(ROSE_INCLUDES)
dnl   
dnl dnl ---------------------------[check ROSE libraries]----------
dnl   rose_libdirs="$ac_rose/lib"
dnl   AC_FIND_FILE_EXT(librose, $rose_libdirs, rose_libdir)
dnl   if test "$rose_libdir" = NONE; then
dnl      AC_MSG_ERROR([ ROSE libraries not found,  use --with-rose=DIR ])
dnl   fi
dnl   LIB_ROSE="-lrose"
dnl   ROSE_LDFLAGS="-L$rose_libdir"
dnl   AC_SUBST(LIB_ROSE)
dnl   AC_SUBST(ROSE_LDFLAGS)
dnl   AC_MSG_RESULT([ headers $rose_incdir, libraries $rose_libdir ])
dnl ])
dnl 

dnl ------------------------------------------------------------------------
dnl Try to find the QT headers and libraries.
dnl ------------------------------------------------------------------------
AC_DEFUN([AC_PATH_QT],
[
AC_MSG_CHECKING([for Qt])

dnl DQ (9/11/2008): Modified to set the "n" so that tests without QT would work.
AC_ARG_WITH(qt,
    [  --with-qt=DIR where the QT root directory is. ],
    [  ac_qt_path="$withval"],
    [  with_qt=no ])

AC_ARG_WITH(qt-inc,
    [  --with-qt-inc=DIR   where the QT includes are. ],
    [  ac_qt_includes="$withval"
    ])

AC_ARG_WITH(qt-lib,
    [  --with-qt-lib=DIR where the QT libraries are. ],
    [  ac_qt_libraries="$withval"
    ])

AC_ARG_WITH(roseQt,
    [  --with-roseQt=Build with roseQt. yes or no. defaults to no. ],
    [  ],
    [  with_roseQt=no ])
  
dnl ---------------------------[check Qt headers]------------
  qt_incdirs="$ac_qt_includes $ac_qt_path/include /usr/include/qt4"
  AC_FIND_FILE(QtGui/qapplication.h, $qt_incdirs, qt_incdir)

dnl echo "qt_incdir = $qt_incdir"
dnl echo "with_QRose = $with_QRose"

dnl DQ (9/12/2008): Only test for failure if we have configured for QRose use.
dnl This prevents failing when we find Qt3 and we have not configured
dnl ROSE to use QRose. Also fixed in code below.
dnl if test "x$with_QRose" != xno && test "$qt_incdir" = NONE; then

dnl Gabriel (7/7/2009): Added Qt3 support for the ROSE components - we need to define QT3_SUPPORT , 
dnl                     but I am not sure where to put it, so I have placed it below with the includes
  if test "x$with_QRose" != xno; then
     if test "$qt_incdir" = NONE; then
        AC_MSG_ERROR([ Qt headers not found,  use --with-qt=DIR or --with-qt-includes=DIR])
     fi
  fi
  QT_INCLUDES="-DQT3_SUPPORT -I$qt_incdir/QtCore -I$qt_incdir/QtGui -I$qt_incdir/Qt3Support -I$qt_incdir/QtXml -I$qt_incdir"
  AC_SUBST(QT_INCLUDES)

dnl ---------------------------[check Qt Libraries]------------
  qt_libdirs="$ac_qt_libraries $ac_qt_path/lib /usr/lib"
  AC_FIND_FILE_EXT(libQtGui, $qt_libdirs, qt_libdir)
dnl if test "$with_QRose" != no && test "$qt_libdir" = NONE; then
  if test "$with_QRose" != no; then
     if test "$qt_libdir" = NONE; then
        AC_MSG_ERROR([ Qt libraries not found,  use --with-qt=DIR or --with-qt-libraries=DIR ])
     fi
  fi
dnl  LIB_QT="-lQtCore -lQtGui -lQt3Support"
  QT_LDFLAGS="-L$qt_libdir -lQtCore -lQtGui -lQt3Support -lQtXml -lQtUiTools"
dnl  AC_SUBST(LIB_QT)

  AM_CONDITIONAL(ROSE_USE_QT,test "$with_qt" != no)

  AC_SUBST(QT_LDFLAGS)
  AC_MSG_RESULT([ headers $qt_incdir, libraries $qt_libdir ])
])

dnl ------------------------------------------------------------------------
dnl Find Qt meta object compiler (moc)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_PATH_QT_MOC],
[
   AC_REQUIRE([AC_CHECK_WINDOWS])
   AC_REQUIRE([AC_PATH_QT])

   AC_ARG_WITH(qt-bin,
    [  --with-qt-bin=DIR   where the QT binaries are. ],
    [  ac_qt_bin="$withval"
    ])

dnl ------------------------[get binary]-----------------

   moc_dirs="$ac_qt_bin $ac_qt_path/bin `echo $PATH | sed s/:/\ /g`"
   if test $USING_WINDOWS = 1; then
      moc_binary="moc.exe"
   else
      moc_binary="moc"
   fi

dnl ------------------------[check if moc exists]-----------------
   AC_MSG_CHECKING([for Qt meta-object compiler])
   AC_FIND_FILE($moc_binary, $moc_dirs, moc_dir)
dnl if test "$with_QRose" != no && test "$moc_dir" = NONE; then
   if test "$with_QRose" != no; then
      if test "$moc_dir" = NONE; then
         AC_MSG_ERROR([ No Qt meta object compiler ($moc_binary) found! (should be found in PATH, or --with-qt=DIR-qt_root, or --with-qt-bin=DIR-qt_bin)])
      fi
   fi
   MOC=$moc_dir/$moc_binary
   AC_MSG_RESULT([$MOC])
dnl ------------------------[check if moc is version 4]-----------------
   AC_MSG_CHECKING([Qt meta-object compiler version])

dnl DQ (9/12/2008): Added to test for use of QRose to prevent error if Qt3 is found.
   if test "$with_QRose" != no; then
      if test "$moc_dir" != NONE; then
         try=`$MOC -v 2>&1 | grep "Qt 4."`
         if test -z "$try"; then
            AC_MSG_ERROR([ invalid version - $MOC must be version 4.x.x])
         else
            AC_MSG_RESULT([passed])
         fi
     fi
     AC_SUBST(MOC)
   fi
])


dnl MB:  05/14/09  copied and adapted MOC finding to find also uic and rcc

dnl ------------------------------------------------------------------------
dnl Find Qt ui compiler (uic)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_PATH_QT_UIC],
[
   AC_REQUIRE([AC_CHECK_WINDOWS])
   AC_REQUIRE([AC_PATH_QT])

   AC_ARG_WITH(qt-bin,
    [  --with-qt-bin=DIR   where the QT binaries are. ],
    [  ac_qt_bin="$withval"
    ])

dnl ------------------------[get binary]-----------------

   uic_dirs="$ac_qt_bin $ac_qt_path/bin `echo $PATH | sed s/:/\ /g`"
   if test $USING_WINDOWS = 1; then
      uic_binary="uic.exe"
   else
      uic_binary="uic"
   fi

dnl ------------------------[check if uic exists]-----------------
   AC_MSG_CHECKING([for Qt uic compiler])
   AC_FIND_FILE($uic_binary, $uic_dirs, uic_dir)
dnl if test "$with_QRose" != no && test "$uic_dir" = NONE; then
   if test "$with_QRose" != no; then
      if test "$uic_dir" = NONE; then
         AC_MSG_ERROR([ No Qt ui compiler ($uic_binary) found! (should be found in PATH, or --with-qt=DIR-qt_root, or --with-qt-bin=DIR-qt_bin)])
      fi
   fi
   UIC=$uic_dir/$uic_binary
   AC_MSG_RESULT([$UIC])
dnl ------------------------[check if uic is version 4]-----------------
   AC_MSG_CHECKING([Qt uic compiler version])

dnl DQ (9/12/2008): Added to test for use of QRose to prevent error if Qt3 is found.
   if test "$with_QRose" != no; then
      if test "$uic_dir" != NONE; then
         try=`$UIC -v 2>&1 | grep "Qt 4."`
         if test -z "$try"; then
            AC_MSG_ERROR([ invalid version - $UIC must be version 4.x.x])
         else
            AC_MSG_RESULT([passed])
         fi
     fi
     AC_SUBST(UIC)
   fi
])


dnl ------------------------------------------------------------------------
dnl Find Qt resource resource compiler
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_PATH_QT_RCC],
[
   AC_REQUIRE([AC_CHECK_WINDOWS])
   AC_REQUIRE([AC_PATH_QT])

   AC_ARG_WITH(qt-bin,
    [  --with-qt-bin=DIR   where the QT binaries are. ],
    [  ac_qt_bin="$withval"
    ])

dnl ------------------------[get binary]-----------------

   rcc_dirs="$ac_qt_bin $ac_qt_path/bin `echo $PATH | sed s/:/\ /g`"
   if test $USING_WINDOWS = 1; then
      rcc_binary="rcc.exe"
   else
      rcc_binary="rcc"
   fi

dnl ------------------------[check if rcc exists]-----------------
   AC_MSG_CHECKING([for Qt rcc compiler])
   AC_FIND_FILE($rcc_binary, $rcc_dirs, rcc_dir)
dnl if test "$with_QRose" != no && test "$rcc_dir" = NONE; then
   if test "$with_QRose" != no; then
      if test "$rcc_dir" = NONE; then
         AC_MSG_ERROR([ No Qt rcc compiler ($rcc_binary) found! (should be found in PATH, or --with-qt=DIR-qt_root, or --with-qt-bin=DIR-qt_bin)])
      fi
   fi
   RCC=$rcc_dir/$rcc_binary
   AC_MSG_RESULT([$RCC])
dnl ------------------------[check if rcc is version 4]-----------------
   AC_MSG_CHECKING([Qt resource compiler version])

dnl DQ (9/12/2008): Added to test for use of QRose to prevent error if Qt3 is found.
   if test "$with_QRose" != no; then
      if test "$rcc_dir" != NONE; then
         try=`$RCC -v 2>&1 | grep "Qt 4."`
         if test -z "$try"; then
            AC_MSG_ERROR([ invalid version - $RCC must be version 4.x.x])
         else
            AC_MSG_RESULT([passed])
         fi
     fi
     AC_SUBST(RCC)
   fi
])

dnl --------------------------------------------------------------------
dnl check qt version
dnl --------------------------------------------------------------------

AC_DEFUN([AC_PATH_QT_VERSION],
[
    AC_REQUIRE([AC_PATH_QT])


    if test $with_qt != no
    then
        dnl get complete version string ...
        QT_VERSION=`grep QT_VERSION_STR $qt_incdir/QtCore/qglobal.h | awk '{print $ 3}' | sed -e 's/\"//g'`
        dnl get major version number
        QT_VERSION_MAJOR=`echo $QT_VERSION | awk -F . '{print $ 1}'`
        QT_VERSION_MINOR=`echo $QT_VERSION | awk -F . '{print $ 2}'`

        dnl test for empty variables
        if test -z "$QT_VERSION_MAJOR"
        then
            QT_VERSION_MAJOR=0
        fi
        if test -z "$QT_VERSION_MINOR"
        then
            QT_VERSION_MINOR=0
        fi

        if test x"$with_roseQt" != x"no"
        then
            AC_MSG_CHECKING([Qt version compatible with roseQt])
            if test $QT_VERSION_MAJOR -ge 4 && test $QT_VERSION_MINOR -ge 4
            then
                with_roseQt=yes
                AC_MSG_RESULT([passed])
            else
                AC_MSG_ERROR([ invalid version - qt must be at least version 4.4.x in order to build with roseQt])
            fi
	dnl Comment the following lines in, to have roseQt automatically enabled if qt>=4.4.0
        dnl else
            dnl if test $QT_VERSION_MAJOR -lt 4 && test $QT_VERSION_MINOR -lt 4
            dnl then
            dnl    AC_MSG_ERROR([ invalid version - qt must be at least version 4.4.x in order to build with roseQt])
            dnl else
            dnl    with_roseQt=yes
            dnl    AC_MSG_RESULT([passed])
            dnl fi
        fi
        AC_SUBST(QT_VERSION)
        AC_SUBST(QT_VERSION_MAJOR)
        AC_SUBST(QT_VERSION_MINOR)
    fi
])
            

AC_DEFUN([AC_CHECK_WINDOWS],
[
AC_REQUIRE([AC_CANONICAL_HOST])
AC_MSG_CHECKING([for windows])
USING_MINGW32=0
if test "$host_os" = cygwin; then   
   USING_WINDOWS=1
   AC_MSG_RESULT([yes: cygwin. Use --build=mingw32 for dll library!])
else
   if test "$host_os" = mingw32; then
      USING_WINDOWS=1
      USING_MINGW32=1
      AC_MSG_RESULT([yes: mingw32])
   else   
      AC_MSG_RESULT([no: $host_os])
      USING_WINDOWS=0
   fi
fi

AC_SUBST(USING_WINDOWS)
AM_CONDITIONAL(QROSE_USE_MINGW32, [test $USING_MINGW32 = 1])
AM_CONDITIONAL(QROSE_USE_WINDOWS, [test $USING_WINDOWS = 1])
])




AC_DEFUN([ROSE_SUPPORT_GRAPHVIZ],
[
# Begin macro ROSE_SUPPORT_GRAPHVIZ.

# The purpose of this macro is to build a automake conditional that 
# communicates if "dot" is available in the user's path so that 
# the PDF files generated in the ROSE/tutorial directory can be 
# tested for their existence.  If "dot" is available then the 
# PDF should be generated, else they will not be generated
# and we should not test for their existance.  The PDF's
# are used in the generation of the ROSE documentation.
# We don't require that the documentation generated
# and since we distribute the documentation with ROSE
# (and it is available on the web) we don't want to
# require these dependences.

AC_MSG_CHECKING([for GraphViz's dot program in path])

DOT_AVAILABLE="no"
DOT_PATH="`which dot > /dev/null`"
if test $? -eq 0; then
   DOT_AVAILABLE="yes"
   AC_MSG_RESULT(yes)
else
   # Dot is not found.
   AC_MSG_RESULT([not found in user's path])
   echo "GraphViz's dot program not found in the user's path!"
fi

AM_CONDITIONAL([ROSE_USE_GRAPHVIZ_DOT],[ test "x$DOT_AVAILABLE" = xyes ] )

# End macro ROSE_SUPPORT_MAPLE.
]
)

AC_DEFUN([ROSE_SUPPORT_INTEL_PIN],
[
# Begin macro ROSE_SUPPORT_INTEL_PIN.

# Inclusion of test for INTEL PIN Dynamic Instrumentation Package and it's location.

AC_MSG_CHECKING(for IntelPin)
AC_ARG_WITH(IntelPin,
[  --with-IntelPin=PATH	Specify the prefix where Intel Pin Package is installed],
,
if test ! "$with_IntelPin" ; then
   with_IntelPin=no
fi
)

echo "In ROSE SUPPORT MACRO: with_yices $with_IntelPin"

if test "$with_IntelPin" = no; then
   # If --with-IntelPin is not specified, then don't use it.
   echo "Skipping use of Intel Pin package support!"
else
   IntelPin_path=$with_IntelPin
   echo "Setup IntelPin support in ROSE! path = $IntelPin_path"
   AC_DEFINE([USE_ROSE_INTEL_PIN_SUPPORT],1,[Controls use of ROSE support for Intel Pin Dynamic Instrumentation Package.])
fi

AC_SUBST(IntelPin_path)

# End macro ROSE_SUPPORT_INTEL_PIN.
]
)

AC_DEFUN([ROSE_SUPPORT_BDDBDDB],
[
# Begin macro ROSE_SUPPORT_BDDBDDB.

AC_REQUIRE([ROSE_SUPPORT_PYTHON])

# Inclusion of test for bddbddb and it's location.

AC_MSG_CHECKING(for bddbddb)
AC_ARG_WITH(bddbddb,
[  --with-bddbddb       Specify the use of bddbddb (distributed internally)],
,
if test ! "$with_bddbddb" ; then
  with_bddbddb=no
fi
)

if test "$with_bddbddb" = no; then
  # If bddbddb is not specified, then don't use it.
  echo "Skipping use of bddbddb!"
else
    if test -n "$python_path";
    then
        # We currently ignore the path, but it might be used
        # later if BDDBDDB is installed external to ROSE.
        bddbddb_path=$with_bddbddb
        AC_DEFINE([USE_ROSE_BDDBDDB_SUPPORT],1,[Controls use of ROSE support for bddbddb (Binary decision diagrams (BDD) BDD-Based Deductive DataBase.])
    else
        AC_MSG_ERROR([--with-bddbddb was passed but Python 2.4 or above was not found.  Try setting the --with-python option to the location of a good python interpreter: --with-python=/path/to/python/bin/python])
    fi

fi

AC_SUBST(bddbddb_path)

# End macro ROSE_SUPPORT_BDDBDDB.
]
)


AC_DEFUN([HASH_ROSE_BINARY_SQL],
[

AC_MSG_CHECKING(for binary analysis support (internal))
AC_ARG_WITH(binarysql,
   [  --with-binarysql ... Enable MySQL for Binary Analysis work],with_binarysql=yes, with_binarysql=no,
with_binarysql=yes )

echo "In ROSE SUPPORT MACRO: with_binarysql = $with_binarysql"

if test "$with_binarysql" = no; then
   # If binary analysis support is not specified, then don't use it.
   echo "Skipping use of Binary SQL Support!"
else
   echo "Setup Binary SQL support in ROSE!"
   AC_DEFINE([HASH_ROSE_USE_BINARYSQL],1,[Controls use of ROSE support for Binary SQL.])
fi


]
)


AC_DEFUN([ROSE_SUPPORT_DOXYGEN],
[
# Begin macro ROSE_SUPPORT_DOXYGEN.

# DQ Comment: We need a better name than --enable-doxygen-internal
# use: --enable-developer-docs

dnl *********************************************************************
dnl * Enable Doxygen to display internal information on ROSE project
dnl *********************************************************************
dnl 
dnl DQ (8/25/2004): Make the default to generate the developer documentation
dnl so that configure without any options will generate reasonable documentation
dnl within its construction of the documentation.
dnl
dnl AC_ARG_ENABLE(doxygen-developer-docs,
dnl   [  --enable-doxygen-developer-docs .......... Enable display of internal project detail with Doxygen.])
AC_ARG_ENABLE(doxygen-developer-docs,
  [  --enable-doxygen-developer-docs .......... Enable display of internal project detail with Doxygen.],
  [enable_doxygen_developer_docs=yes],
  [enable_doxygen_developer_docs=yes])

echo "enable_doxygen_developer_docs = $enable_doxygen_developer_docs"

dnl *********************************************************************
dnl * Enable Doxygen build documentation for Rose faster by connecting 
dnl * Sage III as a separate set of documentation to Rose using the 
dnl * Doxygen Tag file mechanism.  The documentation is generated much 
dnl * faster which simplifies the addition of documentation to both Rose 
dnl * and Sage separately, but it is not as presentable.
dnl *********************************************************************
AC_ARG_ENABLE(doxygen-generate-fast-docs,
  [  --enable-doxygen-generate-fast-docs ...... Enable faster generation of Doxygen documents using 
                                         tag file mechanism to connect Sage III documentation to 
                                         Rose documentation (documentation not as presentable).])

dnl DQ (12/15/2002) support for Doxygen specific options
if test "$enable_doxygen_developer_docs" = yes ; then
  echo "force generation of internal project detail"
  dnl AC_DEFINE(DOXYGEN_EXTRACT_ALL,"YES",[Force Doxygen to document everything (even if no documentation is explicit).])
  dnl AC_DEFINE(DOXYGEN_EXTRACT_PRIVATE,YES,[Force Doxygen to document private interfaces.])
  dnl AC_DEFINE(DOXYGEN_EXTRACT_STATIC,YES,[Force Doxygen to document static interfaces.])
  dnl AC_DEFINE(DOXYGEN_EXTRACT_INTERNAL_DOCS,YES,[Force Doxygen to output all internal project documentation.])
  DOXYGEN_EXTRACT_ALL="YES"
  DOXYGEN_EXTRACT_PRIVATE="YES"
  DOXYGEN_EXTRACT_STATIC="YES"
  DOXYGEN_INTERNAL_DOCS="YES"
  DOXYGEN_HIDE_UNDOC_MEMBERS="NO"
  DOXYGEN_HIDE_UNDOC_CLASSES="NO"
  DOXYGEN_HIDE_FRIEND_COMPOUNDS="NO"
  DOXYGEN_ENABLED_SECTIONS="documentDevelopmentVersionUsingDoxygen"
else
  echo "generation user documentation only (no internal project detail)"
  dnl AC_DEFINE([DOXYGEN_EXTRACT_ALL],[NO],[Force Doxygen to document everything (even if no documentation is explicit).])
  dnl AC_DEFINE([DOXYGEN_EXTRACT_PRIVATE],[NO],[Force Doxygen to document private interfaces.])
  dnl AC_DEFINE([DOXYGEN_EXTRACT_STATIC],[NO],[Force Doxygen to document static interfaces.])
  dnl AC_DEFINE([DOXYGEN_EXTRACT_INTERNAL_DOCS],[NO],[Force Doxygen to output all internal project documentation.])
  DOXYGEN_EXTRACT_ALL="NO"
  DOXYGEN_EXTRACT_PRIVATE="NO"
  DOXYGEN_EXTRACT_STATIC="NO"
  DOXYGEN_INTERNAL_DOCS="NO"
  DOXYGEN_HIDE_UNDOC_MEMBERS="YES"
  DOXYGEN_HIDE_UNDOC_CLASSES="YES"
  DOXYGEN_HIDE_FRIEND_COMPOUNDS="YES"
  DOXYGEN_ENABLED_SECTIONS="documentUserVersionUsingDoxygen"
fi

echo "srcdir = $srcdir"
# echo "top_srcdir = $top_srcdir"
# echo "ac_top_srcdir = $ac_top_srcdir"
# echo "enable_doxygen_generate_fast_docs = $enable_doxygen_generate_fast_docs"

# DQ (7/13/2004): Fix for compiling in the source tree.
# Doxygen is easier to use if it uses an absolute path to
# find the ROSE documentation.  So we build an absolute path
# from the srcdir variable if it is a relative path.
# Note that top_srcdir is not set if we build the compile
# tree in the source tree. So this fixes that support in ROSE.
# if (test -d /$srcdir); then
# if ( (test -d /$srcdir) && ( test $srcdir != "." ) ); then
#     echo "Absolute Source Tree Path Found"
#     topSourceDirectory=$srcdir
#   else
#     echo "Relative Source Tree Path Found (computing source directory)"
#     current_dir=$PWD
#     cd $srcdir;
#     topSourceDirectory=`pwd`
#     cd $current_dir
#     final_location_dir=$PWD
# fi

# DQ (7/25/2004): This is better code (found on the web, it sure is cute)
case $srcdir in
  /*) topSourceDirectory=$srcdir ;;
  *) topSourceDirectory=$PWD/$srcdir ;;
esac

echo "Computed topSourceDirectory (Absolute Path) = $topSourceDirectory"

dnl DQ (12/20/2002) support for Doxygen specific options
dnl We might want the default to later be to merge Rose and 
dnl Sage documentation (change it then).
if (test "$enable_doxygen_generate_fast_docs" = yes) ; then
  echo "Permit separation of Sage III doxygen generated documentation from Rose documentation uses doxygen tag file mechanism to connect the two"
  DOXYGEN_ADDED_DIRECTORY_1=""
  DOXYGEN_ADDED_DIRECTORY_2=""
  DOXYGEN_USE_TAG_FILE="@top_pwd@/docs/Rose/SageIntermediateForm/sageDoxygen.tags"
else
  echo "Permit Doxygen generation of Rose documentation including Sage III documentation as a single monolithic project; takes longer"
  DOXYGEN_ADDED_DIRECTORY_1="@top_pwd@/src/frontend/SageIII"
  DOXYGEN_ADDED_DIRECTORY_2="$topSourceDirectory/src/frontend/SageIII"
  DOXYGEN_USE_TAG_FILE=""
fi

dnl Debugging output
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_EXTRACT_ALL           = $DOXYGEN_EXTRACT_ALL"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_EXTRACT_PRIVATE       = $DOXYGEN_EXTRACT_PRIVATE"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_EXTRACT_STATIC        = $DOXYGEN_EXTRACT_STATIC"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_INTERNAL_DOCS         = $DOXYGEN_INTERNAL_DOCS"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_HIDE_UNDOC_MEMBERS    = $DOXYGEN_HIDE_UNDOC_MEMBERS"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_HIDE_UNDOC_CLASSES    = $DOXYGEN_HIDE_UNDOC_CLASSES"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_HIDE_FRIEND_COMPOUNDS = $DOXYGEN_HIDE_FRIEND_COMPOUNDS"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_ENABLED_SECTIONS      = $DOXYGEN_ENABLED_SECTIONS"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_ADDED_DIRECTORY_1     = $DOXYGEN_ADDED_DIRECTORY_1"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_ADDED_DIRECTORY_2     = $DOXYGEN_ADDED_DIRECTORY_2"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_USE_TAG_FILE          = $DOXYGEN_USE_TAG_FILE"

AC_SUBST(DOXYGEN_EXTRACT_ALL)
AC_SUBST(DOXYGEN_EXTRACT_PRIVATE)
AC_SUBST(DOXYGEN_EXTRACT_STATIC)
AC_SUBST(DOXYGEN_INTERNAL_DOCS)
AC_SUBST(DOXYGEN_HIDE_UNDOC_MEMBERS)
AC_SUBST(DOXYGEN_HIDE_UNDOC_CLASSES)
AC_SUBST(DOXYGEN_HIDE_FRIEND_COMPOUNDS)
AC_SUBST(DOXYGEN_ENABLED_SECTIONS)
AC_SUBST(DOXYGEN_ADDED_DIRECTORY_1)
AC_SUBST(DOXYGEN_ADDED_DIRECTORY_2)
AC_SUBST(DOXYGEN_USE_TAG_FILE)

# End macro ROSE_SUPPORT_DOXYGEN.
]
)








AC_DEFUN([ROSE_SUPPORT_DWARF],
[
# Begin macro ROSE_SUPPORT_DWARF.

# Inclusion of test for DWARF libdwarf library and it's location.

AC_MSG_CHECKING(for dwarf)
AC_ARG_WITH(dwarf,
[  --with-dwarf=PATH	Specify the path where libdwarf is installed],
,
if test ! "$with_dwarf" ; then
   with_dwarf=no
fi
)

echo "In ROSE SUPPORT MACRO: with_dwarf $with_dwarf"

if test "$with_dwarf" = no; then
   # If dwarf is not specified, then don't use it.
   echo "Skipping use of Dwarf (libdwarf) support!"
else
   dwarf_path=$with_dwarf
   echo "Setup Dwarf support in ROSE! path = $dwarf_path"
   AC_DEFINE([USE_ROSE_DWARF_SUPPORT],1,[Controls use of ROSE support for Dwarf (libdwarf) library.])
   DWARF_LINK="$dwarf_path/libdwarf.a -lelf"
   DWARF_INCLUDE="-I$dwarf_path"

   echo "DWARF_INCLUDE = $DWARF_INCLUDE"
   echo "DWARF_LINK    = $DWARF_LINK"
fi

AC_SUBST(dwarf_path)

# DQ (9/4/2009): Added support for fix make installcheck rule when Dwarf is tested
AC_SUBST(DWARF_LINK)
AC_SUBST(DWARF_INCLUDE)

# End macro ROSE_SUPPORT_DWARF.
]
)

AC_DEFUN([ROSE_SUPPORT_EDG_DEBUGGING],
[
# Begin macro ROSE_SUPPORT_EDG_DEBUGGING.

# This optionally permits the unions in EDG to be converted to structs to support 
# debugging of data member field accesses in the EDG/Sage III translation.

AC_MSG_CHECKING(for specification of EDG union/struct debugging support)
AC_ARG_ENABLE(edg_union_struct_debugging,
[  --enable-edg_union_struct_debugging	Specify if EDG Union/Struct debugging support is to be used],
,
if test ! "$enable_edg_union_struct_debugging" ; then
   enable_edg_union_struct_debugging=no
fi
)

echo "In ROSE SUPPORT MACRO: enable_edg_union_struct_debugging $enable_edg_union_struct_debugging"

if test "$enable_edg_union_struct_debugging" = no; then
   # If edg_union_struct_debugging is not specified, then don't use it.
   echo "Skipping use of EDG Union/Struct debugging support!"
   edg_union_struct_debugging_value="union"
else
   edg_union_struct_debugging_value="struct"
   echo "Setup EDG Union/Struct debugging support in ROSE! edg_union_struct_debugging_value = $edg_union_struct_debugging_value"
fi

AC_DEFINE([USE_ROSE_EDG_DEBUGGING_SUPPORT],1,[Controls if EDG Union/Struct debugging support is to be used.])

AC_SUBST(edg_union_struct_debugging_value)

# End macro ROSE_SUPPORT_EDG_DEBUGGING.
]
)

AC_DEFUN([ROSE_SUPPORT_GCC_OMP],
[
# Begin macro ROSE_SUPPORT_OMP.

# Inclusion of test for Omni OpenMP Runtime system and its location.

AC_MSG_CHECKING(for GCC OpenMP)
AC_ARG_WITH(gcc_omp,
[  --with-gcc_omp  Enables OMP in ROSE to run Compass thread parallel],
,
if test ! "$with_gcc_omp" ; then
   with_gcc_omp=no
fi
)

echo "In ROSE SUPPORT MACRO: with_gcc_omp $with_gcc_omp"

if test "$with_gcc_omp" = no; then
   # If omp_runtime_support is not specified, then don't use it.
   echo "Skipping use of GCC OpenMP support!"
else
   gcc_omp_path=$with_gcc_omp
   echo "Setup OpenMP support in ROSE! path = $gcc_omp_path"
   AC_DEFINE([USE_ROSE_GCC_OMP],1,[Use of GCC OpenMP in ROSE.])
   CFLAGS="$CFLAGS -fopenmp -pthread"
   LDFLAGS="$LDFLAGS -fopenmp -pthread"
   CXXFLAGS="$CXXFLAGS -fopenmp -pthread"
fi

AC_SUBST(gcc_omp_path)

# End macro ROSE_SUPPORT_OMP.
]
)

AC_DEFUN([ROSE_WITH_GOMP_OPENMP_LIBRARY],
[
# Check if gomp 4.4 OpenMP runtime library is available
# Begin macro ROSE_WITH_GOMP_OPENMP_LIBRARY.
# Inclusion of test for GCC GOMP OpenMP Runtime system and its location.

AC_MSG_CHECKING(for OpenMP using gomp runtime library)
AC_ARG_WITH(gomp_omp_runtime_library,
[  --with-gomp_omp_runtime_library=PATH	Specify the prefix where GOMP Runtime System is installed],
,
if test ! "$with_gomp_omp_runtime_library" ; then
   with_gomp_omp_runtime_library=no
fi
)

echo "In ROSE SUPPORT MACRO: with_gomp_omp_runtime_library $with_gomp_omp_runtime_library"

if test "$with_gomp_omp_runtime_library" = no; then
   # If gomp_omp_runtime_library is not specified, then don't use it.
   echo "Skipping use of GOMP OpenMP Runtime Library!"
else
   gomp_omp_runtime_library_path=$with_gomp_omp_runtime_library
   echo "Setup GCC GOMP OpenMP library in ROSE! path = $gomp_omp_runtime_library_path"
   AC_DEFINE([USE_ROSE_GOMP_OPENMP_LIBRARY],1,[Controls use of ROSE support for OpenMP Translator targeting GCC 4.4 's GOMP OpenMP RTL.])
   AC_DEFINE_UNQUOTED([GCC_GOMP_OPENMP_LIB_PATH],"$gomp_omp_runtime_library_path",[Location (unquoted) of the GCC 4.4's GOMP OpenMP runtime library.])
   AC_DEFINE_UNQUOTED([ROSE_INSTALLATION_PATH],"$prefix",[Location (unquoted) of the top directory path to which ROSE is installed.])
fi

AC_SUBST(gomp_omp_runtime_library_path)

# End macro ROSE_WITH_GOMP_OPENMP_LIBRARY.
AM_CONDITIONAL(WITH_GOMP_OPENMP_LIB,test ! "$with_gomp_omp_runtime_library" = no)

]
)

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

AC_DEFUN([ROSE_SUPPORT_IDA],
[
# Begin macro ROSE_SUPPORT_IDA.

# Inclusion of test for the Interactive Disassembler IDA Pro package and it's location.

AC_MSG_CHECKING(for ida)
AC_ARG_WITH(ida,
[  --with-ida=PATH	Specify the prefix where IDA Pro is installed],
,
if test ! "$with_ida" ; then
   with_ida=no
fi
)

echo "In ROSE SUPPORT MACRO: with_ida $with_ida"

if test "$with_ida" = no; then
   # If ida is not specified, then don't use it.
   echo "Skipping use of Ida!"
else
   ida_path=$with_ida
   echo "Setup Ida support in ROSE! path = $ida_path"
   AC_DEFINE_UNQUOTED([IDA_PRO_PATH],"$ida_path",[Prefix path for use of IDA.])
   AC_DEFINE([USE_ROSE_IDA_SUPPORT],1,[Controls use of IDA support for disassembling.])

fi

AC_SUBST(ida_path)

# End macro ROSE_SUPPORT_IDA.
]
)

AC_DEFUN([ROSE_SUPPORT_INSURE],
[
# Begin macro ROSE_SUPPORT_INSURE.

# We may be asked to guess the correct flags,
# so we must know our host and the compiler used.
AC_REQUIRE([AC_CANONICAL_HOST])
AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([BTNG_INFO_CXX_ID])
AC_REQUIRE([BTNG_CXX_AR])

dnl *********************************************************************
dnl * Use INSURE
dnl *********************************************************************
# AC_ARG_ENABLE(insure-api,
#  [  --enable-insure-api ....................... Enable insure API in code.])
AC_ARG_ENABLE(insure,
 [  --enable-insure ....................... Augment the linker with insure.])

# if test "$enable_insure_api" = yes ; then
#   AC_DEFINE([USE_INSURE],[],[Support for Insure])
#   AC_DEFINE([USE_INSURE_API],[],[Support for Insure API (Compiling with the Insure library).])
# fi

dnl DQ added so that EDG can detect use of linker and use the api 
dnl while the rest of ROSE does not.  EDG only supports use of API
dnl not just the linker option.
if test "$enable_insure" = yes ; then
  AC_DEFINE([USE_INSURE],[],[Link-time support for Insure.])
fi



dnl *********************************************************************
dnl * Specify the location of INSURE
dnl *********************************************************************
# Let user specify where to find insure.
# Specify by --with-insure= or setting insure_PREFIX.
AC_MSG_CHECKING(for insure)
AC_ARG_WITH(insure,
[  --with-insure=PATH	Specify the prefix where insure is installed],
, test "$insure_PREFIX" && with_insure="$insure_PREFIX")
test "$with_insure" && test ! "$with_insure" = no && insure_PREFIX="$with_insure"
AC_MSG_RESULT($insure_PREFIX)
echo "insure_PREFIX is $insure_PREFIX" >&5
if test "$insure_PREFIX" ; then
  insure_INCLUDES="-I$insure_PREFIX"
  insure_LIBS="$insure_PREFIX/insure_stubs.a"
  insure_BIN="$insure_PREFIX/insure"
fi
AC_SUBST(insure_PREFIX)
AC_SUBST(insure_INCLUDES)
AC_SUBST(insure_LIBS)
AC_SUBST(insure_BIN)
echo "insure_INCLUDES is $insure_INCLUDES" >&5
echo "insure_LIBS is $insure_LIBS" >&5
echo "insure_BIN is $insure_BIN" >&5



dnl Allows insure windows to be disabled, as it is too much load over the network.
# AC_ARG_ENABLE(insure-windows,
#   [  --enable-insure-windows ............... turn on use of INSURE windows option])
# test ! "$enable_insure_windows" = yes && insure_windows_option='-windows=no';



dnl *********************************************************************
dnl * Specify options for INSURE
dnl *********************************************************************
dnl  Added by DQ to support INSURE (using my favorite options and paths which might be platform specific)
dnl note that we have to use the ${} instead of $() for this to work on LINUX and IBM (other platforms do not seem to case)
dnl  INSURE_RUN_AT_EXIT="-run-at-exit=\"if %z; then echo \\\"%v: %e errors, %l+%L bytes leaked.\\\"; fi\" "
AC_ARG_WITH(insure-options,
   [  --with-insure-options=ARG ................ manually set INSURE options to ARG],
INSURE_OPTIONS=$with_insure_options,
INSURE_OPTIONS=""
)
AC_SUBST(INSURE_OPTIONS)


# if test "$enable_insure_linker" ; then
#   AUX_LINKER="${insure_BIN} ${INSURE_OPTIONS}"
# fi
# AC_SUBST(AUX_LINKER)



# if test "$enable_insure_linker" = yes ; then
# if test "$with_insure" = yes ; then
if test "$enable_insure" = yes ; then
# Setup compiler specific options for using insure
# Different compilers (e.g KCC) require options in different order.
  AUX_LINKER="${insure_BIN} ${INSURE_OPTIONS}"
  case $CXX in
    g++)
    # See comment below about syntax change for Linux
      CC="insure"
      CCLD="$CC"
      CXX="insure"
      CXXLD="$CXX"
      ;;
    KCC | mpKCC)
    # insure specific option required and clear the AUX_LINKER
      CCLD="$CC --link_command_prefix '${insure_BIN} ${INSURE_OPTIONS}' -o"
      CXXLD="$CXX --link_command_prefix '${insure_BIN} ${INSURE_OPTIONS}' -o"
      AUX_LINKER=""
      ;;
    CC)
    # No insure specific option required
    # Can't use "$(xxx)" syntax since it only works when build proecess is run on SUNs
    # Linux requires a different syntax: "$xxx" instead.
    # CCLD="$(AUX_LINKER) $(CXX)"
    # CXXLD="$(AUX_LINKER) $(CXX)"
      CCLD="$AUX_LINKER $CXX"
      CXXLD="$AUX_LINKER $CXX"
    ;;
  esac
else
     CCLD="$CC"
# Liao: 2/17/2009, enable CXXLD environment variable to be accepted during configuration,
# especially when roseTranslator is used and it is not ready to be used as a linker
      CXXLD="$CXXLD"
      if test "x$CXXLD" = x; then
         CXXLD="$CXX"
      fi 
# BP : 10/29/2001, the above two lines were originally 
#      CCLD="$(CC)"
#      CXXLD="$(CXX)"
# but on Linux systems this seems to not work
fi

AC_SUBST(AUX_LINKER)

echo "In macro ROSE SUPPORT INSURE: CC    = $CC"
echo "In macro ROSE SUPPORT INSURE: CCLD  = $CCLD"
echo "In macro ROSE SUPPORT INSURE: CXX   = $CXX"
echo "In macro ROSE SUPPORT INSURE: CXXLD = $CXXLD"

AC_SUBST(CCLD)
AC_SUBST(CXXLD)

# End macro ROSE_SUPPORT_INSURE.
]
)


AC_DEFUN([ROSE_SUPPORT_JAVA],
[
# Begin macro ROSE_SUPPORT_JAVA.

AC_MSG_CHECKING([for Java (javac first, then java, then jvm)])

AC_ARG_WITH([java],
            AS_HELP_STRING([--with-java],
                           [use Java for Fortran or Javaport support (default is YES if Java can be found)]),
            [javasetting=$withval],
            [javasetting=try])

JAVAC=$javasetting
USE_JAVA=1
#echo "JAVA_HOME = ${JAVA_HOME}"
if test "x$javasetting" = xno; then
# echo 'test "x$javasetting" = xno;'
  JAVAC=""
  USE_JAVA=0
elif test "x$javasetting" = xyes || test "x$javasetting" = xtry; then
# echo 'test x$javasetting = xyes || test x$javasetting = xtry;'
  JAVA_PATH="${JAVA_HOME}"
# echo "JAVA_PATH = ${JAVA_PATH}"
  if test "x$JAVA_PATH" = x; then
#   echo 'test x$JAVA_PATH = x;'
# DQ & PC (11/3/2009): Search for javac instead of java since we require
# the JDK and this should detect that it is missign as early as possible.
    JAVAC="`which javac`"
  # if which javac > /dev/null; then
    if test $? -eq 0; then
    # echo 'which javac;'
    # echo "java = ${JAVA}"
    # JAVAC="`which javac`"
    # echo "javac = ${JAVAC}"
# DQ & PC (11/3/2009): Fixing support for Java that causes problems for CERT and our new RedHat Release 5 systems.
      ROSE_CANON_SYMLINK(JAVAC, "${JAVAC}")
    # echo "javac = ${JAVAC}"
      JAVAC_BASENAME=`basename ${JAVAC}`
      if test x${JAVAC_BASENAME} == "xjavac"; then
         : # echo "Found Sun or IBM Java (javac)"
      else
       # This is likely the Eclipse Java (ecj).
       # DQ (11/3/2009): If this is IBM Java then it should also work with ROSE and this macro.
         AC_MSG_ERROR([This is not SUN or IBM Java found by default (likely found ecj - Eclipse Java) -- specify correct java using --with-java=<path>])
      fi
    # AS_SET_CATFILE(JAVA_PATH, "`pwd`", "`dirname ${JAVA}`/../..")
      AS_SET_CATFILE(JAVA_PATH, "`pwd`", "`dirname ${JAVAC}`/..")
    # echo "After setting value: JAVA_PATH = ${JAVA_PATH}"
    elif "x$javasetting" = "xyes"; then
      AC_MSG_ERROR([--with-java was given but "java" is not in PATH and JAVA_HOME was not set])
    else # $javasetting is "try", so it is not an error for Java to not be found
    # echo 'FALSE case: x$javasetting = xyes;'
      JAVAC=""
      USE_JAVA=0
    fi
  else
  # echo 'FALSE test x$JAVA_PATH = x;'
# DQ & PC (11/3/2009): Search for javac instead of java since we require
# the JDK and this should detect that it is missign as early as possible.
#   JAVA="${JAVA_PATH}/bin/java"
    JAVAC="${JAVA_PATH}/bin/javac"
  fi
elif test -d "${javasetting}"; then
# DQ & PC (11/3/2009): Search for javac instead of java since we require
# the JDK and this should detect that it is missign as early as possible.
  if test -x "${javasetting}/bin/javac"; then
    JAVA_PATH="${javasetting}"
  # echo "After setting value using javasetting: JAVA_PATH = ${JAVA_PATH}"
    JAVAC="${javasetting}/bin/javac"
  else
    AC_MSG_ERROR([Argument to --with-java should be either a javac executable or a top-level JDK install directory (with bin/javac present)])
  fi
elif test -x "${javasetting}"; then
  AS_SET_CATFILE(JAVA_PATH, "`pwd`", "`dirname ${javasetting}`/..")
  JAVAC="${javasetting}"
else
  AC_MSG_ERROR([Argument to --with-java should be either a javac executable or a top-level JDK install directory (with bin/javac present)])
fi

# echo "USE_JAVA = $USE_JAVA"
if test "x$USE_JAVA" = x1; then
# AC_MSG_RESULT([$JAVAC])
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT([not requested])
fi

# echo "Before checking for Java JVM: JAVA_PATH = ${JAVA_PATH}"

if test "x$USE_JAVA" = x1; then

# DQ (11/3/2009): This was moved from down below to check for java before the jvm
# Fix the case of Apple OSX support.
# echo "Before OS specific JAVA = ${JAVA}"
  if test "x$build_vendor" = xapple; then
     JAVA_BIN="${JAVA_PATH}/Commands"
  else
     JAVA_BIN="${JAVA_PATH}/bin"
  fi

  JAVA="${JAVA_BIN}/java"
# echo "JAVA = ${JAVA}"
  AC_MSG_CHECKING(for java)
  if test -x "${JAVA}"; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_ERROR([java not found in $JAVA_PATH])
  fi

  AC_MSG_CHECKING(for Java JVM include and link options)
# This is a hack, but it seems to work to find the JVM library
  if test -x /usr/bin/javaconfig; then # We are on a Mac
    JAVA_JVM_LINK="-framework JavaVM"
    JAVA_JVM_INCLUDE="-I`/usr/bin/javaconfig Headers`"
  else
    JAVA_JVM_FULL_PATH="`env _JAVA_LAUNCHER_DEBUG=x ${JAVA} 2>/dev/null | grep '^JVM path is' | cut -c 13-`" ; # Sun JVM
    JAVA_JVM_PATH=`dirname "${JAVA_JVM_FULL_PATH}"`
    if test "x$JAVA_JVM_FULL_PATH" = x; then
      JAVA_JVM_PATH="`env _JAVA_LAUNCHER_DEBUG=x ${JAVA} 2>&1 | grep '^JavaJVMDir  = ' | cut -c 15-`" # IBM J9 JVM
      if test "x$JAVA_JVM_PATH" = x; then
        AC_MSG_ERROR([Unable to find path to JVM library])
      fi
    fi
    JAVA_JVM_LINK="-L${JAVA_JVM_PATH} -ljvm"
    JAVA_JVM_INCLUDE="-I${JAVA_PATH}/include -I${JAVA_PATH}/include/linux"
  fi
  AC_MSG_RESULT([$JAVA_JVM_INCLUDE and $JAVA_JVM_LINK])

# JAR="${JAVA_PATH}/bin/jar"
  JAR="${JAVA_BIN}/jar"

  AC_MSG_CHECKING(for jar)
  if test -x "${JAR}"; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_ERROR([jar not found in $JAVA_PATH])
  fi
fi

if test $USE_JAVA; then
  AC_DEFINE([USE_ROSE_JAVA_SUPPORT],[],[Controls use of ROSE support for Java.])
fi
AC_DEFINE_UNQUOTED([JAVA_JVM_PATH],["$JAVA"],[Path to JVM executable])

AC_SUBST(JAVA_PATH)
AC_SUBST(JAVA_JVM_LINK)
AC_SUBST(JAVA_JVM_INCLUDE)
AC_SUBST(JAVA)
AC_SUBST(JAVAC)
AC_SUBST(JAR)

# End macro ROSE_SUPPORT_JAVA.
]
)

AC_DEFUN([ROSE_SUPPORT_LIBFFI],
[

# DQ (9/21/2009): Added pkg.m4 to local ROSE/aclocal
# DQ (9/19/2009): This fails on OSX (ninjai.llnl.gov) (added by Peter to support ROSE interpreter).
PKG_CHECK_MODULES([LIBFFI],[libffi],[with_libffi=yes],[with_libffi=no])

# DQ (9/19/2009): This fails on Mac OSX (this variable is needed
# for an automake conditional that will be built in configure.in)
# with_libffi=no
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $LIBFFI_CFLAGS"
AC_CHECK_HEADERS(ffi.h,[with_ffi_header=yes],[with_ffi_header=no])
CPPFLAGS="$save_CPPFLAGS"

echo "Intermediate test debugging info for libffi support: "
echo "   with_libffi     = $with_libffi"
echo "   with_ffi_header = $with_ffi_header"
echo "   LIBFFI_LIBS     = $LIBFFI_LIBS"
echo "   LIBFFI_CFLAGS   = $LIBFFI_CFLAGS"

if [test ! "$with_libffi" = no]; then
   if [test ! "$with_ffi_header" = yes]; then
      echo "Warning:  libffi found, but ffi.h header file not found! (reset with_libffi to disable LIBFFI support)"
      with_ffi_header=no
      with_libffi=no
   fi
fi

echo "Debugging info for libffi support: "
echo "   with_libffi     = $with_libffi"
echo "   with_ffi_header = $with_ffi_header"
echo "   LIBFFI_LIBS     = $LIBFFI_LIBS"
echo "   LIBFFI_CFLAGS   = $LIBFFI_CFLAGS"

]
)

AC_DEFUN([ROSE_SUPPORT_MAPLE],
[
# Begin macro ROSE_SUPPORT_MAPLE.

# Inclusion of test for Maple Symbolic Algebra Package and it's location.

AC_MSG_CHECKING(for maple)
AC_ARG_WITH(maple,
[  --with-maple=PATH	Specify the prefix where Maple is installed],
,
if test ! "$with_maple" ; then
   with_maple=no
fi
)

echo "In ROSE SUPPORT MACRO: with_maple $with_maple"

if test "$with_maple" = no; then
   # If maple is not specified, then don't use it.
   echo "Skipping use of Maple!"
else
   maple_path=$with_maple
   echo "Setup Maple support in ROSE! path = $maple_path"

   MAPLE_SYSTEM_TYPE="`$maple_path/bin/maple.system.type`"
   MAPLE_LIBDIR="$maple_path/$MAPLE_SYSTEM_TYPE"
   MAPLE_EXTRA_LIBS="-L$MAPLE_LIBDIR -Wl,-rpath,$MAPLE_LIBDIR -lrt"
   AC_CHECK_LIB(maplec,StartMaple,[:],[AC_ERROR([Unable to link to Maple!])],[$MAPLE_EXTRA_LIBS])

   MAPLE_INCLUDES="-I$maple_path/extern/include"
   # Maple includes its own old copy of libstdc++ in its bin directory so we need to override this with the system provided one
   MAPLE_LIBS="-Wl,-rpath,/usr/lib -lmaplec $MAPLE_EXTRA_LIBS"
   AC_SUBST(MAPLE_INCLUDES)
   AC_SUBST(MAPLE_LIBS)

   AC_DEFINE([USE_ROSE_MAPLE_SUPPORT],1,[Controls use of ROSE support for Maple Symbolic Algebra Package.])
fi

AC_SUBST(maple_path)

# End macro ROSE_SUPPORT_MAPLE.
]
)

AC_DEFUN([ROSE_SUPPORT_MYSQL],
[
# Begin macro ROSE_SUPPORT_MYSQL.

# Inclusion of test for MySQL and it's location.
# We have no control over where the database is located (placed by MySQL)
# so we don't need to specify that.  We do have to specify:
#  1) server name (default: localhost)
#  2) username: (default: root)
#  3) password: (default: rose)
#  4) database name: (default: rosedb)

AC_MSG_CHECKING(for MySQL)
AC_ARG_WITH(MySQL,
[  --with-MySQL=PATH	Specify the prefix where MySQL & MySQL++ is installed],
,
if test ! "$with_MySQL" ; then
   with_MySQL=no
fi
)

AC_ARG_WITH(MySQL_server,
[  --with-MySQL_server=MACHINE_NAME	Specify the MySQL database server to be used],
,
if test ! "$with_MySQL_server" ; then
#  with_MySQL_server="localhost"
   with_MySQL_server="$HOST"
fi
)

AC_ARG_WITH(MySQL_username,
[  --with-MySQL_username=USER_NAME	Specify the MySQL username to be used],
,
if test ! "$with_MySQL_username" ; then
#  with_MySQL_username="root"
   with_MySQL_username="$USERNAME"
fi
)

AC_ARG_WITH(MySQL_password,
[  --with-MySQL_password=PASSWORD	Specify the MySQL password to be used],
,
if test ! "$with_MySQL_password" ; then
   with_MySQL_password="rosepwd"
fi
)

AC_ARG_WITH(MySQL_database_name,
[  --with-MySQL_database_name=DATABASE_NAME	Specify the MySQL database name to be used],
,
if test ! "$with_MySQL_database_name" ; then
   with_MySQL_database_name="rosedb"
fi
)

echo "In ROSE SUPPORT MACRO: with_MySQL $with_MySQL"

if test "$with_MySQL" = no; then
   # If MySQL is not specified, then don't use it.
   echo "Skipping use of MySQL!"
else
   optional_DataBase_subdirs="DataBase"
 # MySQL_login_string="localhost root rose"
#  MySQL_server="localhost"
#  MySQL_username="root"
#  MySQL_password="rose"
#  MySQL_database_name="rosedb"

   MySQL_path=$with_MySQL
   MySQL_server=$with_MySQL_server
   MySQL_username=$with_MySQL_username
   MySQL_password=$with_MySQL_password
   MySQL_database_name=$with_MySQL_database_name

   echo "Setup MySQL DataBase! path          = $MySQL_path"
   echo "Setup MySQL DataBase! server        = $MySQL_server"
   echo "Setup MySQL DataBase! username      = $MySQL_username"
   echo "Setup MySQL DataBase! password      = $MySQL_password"
   echo "Setup MySQL DataBase! database name = $MySQL_database_name"
   echo "Setup MySQL DataBase! optional_DataBase_subdirs = $optional_DataBase_subdirs"

   AC_CONFIG_SUBDIRS(Projects/DataBase)
fi

AC_SUBST(optional_DataBase_subdirs)
AC_SUBST(MySQL_login_string)
AC_SUBST(MySQL_path)
AC_SUBST(MySQL_server)
AC_SUBST(MySQL_username)
AC_SUBST(MySQL_password)
AC_SUBST(MySQL_database_name)

# End macro ROSE_SUPPORT_MYSQL.
]
)

AC_DEFUN([ROSE_SUPPORT_OMNI_OPENMP],
[
# Begin macro ROSE_SUPPORT_OMNI_OPENMP.

# Inclusion of test for Omni OpenMP Runtime system and its location.

AC_MSG_CHECKING(for OpenMP using Omni runtime library)
AC_ARG_WITH(omni_omp_runtime_support,
[  --with-omni_omp_runtime_support=PATH	Specify the prefix where Omni OpenMP Runtime System is installed],
,
if test ! "$with_omni_omp_runtime_support" ; then
   with_omni_omp_runtime_support=no
fi
)

echo "In ROSE SUPPORT MACRO: with_omni_omp_runtime_support $with_omni_omp_runtime_support"

if test "$with_omni_omp_runtime_support" = no; then
   # If omni_omp_runtime_support is not specified, then don't use it.
   echo "Skipping use of Omni OpenMP Runtime support!"
else
   omni_omp_runtime_support_path=$with_omni_omp_runtime_support
   echo "Setup Omni OpenMP support in ROSE! path = $omni_omp_runtime_support_path"
   AC_DEFINE([USE_ROSE_OMNI_OPENMP_SUPPORT],1,[Controls use of ROSE support for OpenMP Translator targeting Omni RTL.])
fi

AC_SUBST(omni_omp_runtime_support_path)

# End macro ROSE_SUPPORT_OMNI_OPENMP.
]
)

AC_DEFUN([ROSE_SUPPORT_PHP],
[

AC_ARG_WITH(php,
[  --with-php=PATH	Specify the prefix where PHP (and phc) is installed],
,
if test ! "$with_php" ; then
   with_php=no
fi
)

echo "In ROSE SUPPORT MACRO: with_php $with_php"

if test "$with_php" = no; then
   echo "Skipping use of PHP support (and phc)!"
else
   php_path=$with_php
   echo "Setup PHP support in ROSE! path = $php_path"
   AC_DEFINE([USE_ROSE_PHP_SUPPORT],1,[Controls use of ROSE support for PHP.])
fi

AC_SUBST(php_path)


]
)


AC_DEFUN([ROSE_SUPPORT_PURIFY],
[
# Begin macro ROSE_SUPPORT_PURIFY.

# We may be asked to guess the correct flags,
# so we must know our host and the compiler used.
AC_REQUIRE([AC_CANONICAL_HOST])
AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([BTNG_INFO_CXX_ID])
AC_REQUIRE([BTNG_CXX_AR])

dnl *********************************************************************
dnl * Use PURIFY
dnl *********************************************************************
AC_ARG_ENABLE(purify-api,
  [  --enable-purify-api ....................... Enable purify API in code.])
AC_ARG_ENABLE(purify-linker,
  [  --enable-purify-linker ....................... Augment the linker with purify.])

if test "$enable_purify_api" = yes ; then
  AC_DEFINE([USE_PURIFY],[],[Support for Purify])
  AC_DEFINE([USE_PURIFY_API],[],[Support for Purify API (Compiling with the Purify library).])
fi

dnl DQ added so that EDG can detect use of linker and use the api 
dnl while the rest of ROSE does not.  EDG only supports use of API
dnl not just the linker option.
if test "$enable_purify_linker" = yes ; then
  AC_DEFINE([USE_PURIFY_LINKER],[],[Link-time support for Purify.])
fi



dnl *********************************************************************
dnl * Specify the location of PURIFY
dnl *********************************************************************
# Let user specify where to find purify.
# Specify by --with-purify= or setting purify_PREFIX.
AC_MSG_CHECKING(for purify)
AC_ARG_WITH(purify,
[  --with-purify=PATH	Specify the prefix where purify is installed],
, test "$purify_PREFIX" && with_purify="$purify_PREFIX")
test "$with_purify" && test ! "$with_purify" = no && purify_PREFIX="$with_purify"
AC_MSG_RESULT($purify_PREFIX)
echo "purify_PREFIX is $purify_PREFIX" >&5
if test "$purify_PREFIX" ; then
  purify_INCLUDES="-I$purify_PREFIX"
  purify_LIBS="$purify_PREFIX/purify_stubs.a"
  purify_BIN="$purify_PREFIX/purify"
fi
AC_SUBST(purify_PREFIX)
AC_SUBST(purify_INCLUDES)
AC_SUBST(purify_LIBS)
AC_SUBST(purify_BIN)
echo "purify_INCLUDES is $purify_INCLUDES" >&5
echo "purify_LIBS is $purify_LIBS" >&5
echo "purify_BIN is $purify_BIN" >&5



dnl Allows purify windows to be disabled, as it is too much load over the network.
AC_ARG_ENABLE(purify-windows,
  [  --enable-purify-windows ............... turn on use of PURIFY windows option])
test ! "$enable_purify_windows" = yes && purify_windows_option='-windows=no';



dnl *********************************************************************
dnl * Specify options for PURIFY
dnl *********************************************************************
dnl  Added by DQ to support PURIFY (using my favorite options and paths which might be platform specific)
dnl note that we have to use the ${} instead of $() for this to work on LINUX and IBM (other platforms do not seem to case)
dnl  PURIFY_RUN_AT_EXIT="-run-at-exit=\"if %z; then echo \\\"%v: %e errors, %l+%L bytes leaked.\\\"; fi\" "
AC_ARG_WITH(purify-options,
   [  --with-purify-options=ARG ................ manually set PURIFY options to ARG],
PURIFY_OPTIONS=$with_purify_options,
PURIFY_OPTIONS="$purify_windows_option -recursion-depth-limit=40000 -chain-length=24 -first-only=yes -leaks-at-exit=no -inuse-at-exit=no -always-use-cache-dir=yes -cache-dir=${HOME}/tmp -best-effort ${PURIFY_RUN_AT_EXIT}"
)
AC_SUBST(PURIFY_OPTIONS)


# if test "$enable_purify_linker" ; then
#   AUX_LINKER="${purify_BIN} ${PURIFY_OPTIONS}"
# fi
# AC_SUBST(AUX_LINKER)



if test "$enable_purify_linker" = yes ; then
# Setup compiler specific options for using purify
# Different compilers (e.g KCC) require options in different order.
  AUX_LINKER="${purify_BIN} ${PURIFY_OPTIONS}"
  case $CXX in
    g++)
    # See comment below about syntax change for Linux
      CCLD="$AUX_LINKER $CXX"
      CXXLD="$AUX_LINKER $CXX"
      ;;
    KCC | mpKCC)
    # purify specific option required and clear the AUX_LINKER
      CCLD="$CC --link_command_prefix '${purify_BIN} ${PURIFY_OPTIONS}' -o"
      CXXLD="$CXX --link_command_prefix '${purify_BIN} ${PURIFY_OPTIONS}' -o"
      AUX_LINKER=""
      ;;
    CC)
    # No purify specific option required
    # Can't use "$(xxx)" syntax since it only works when build proecess is run on SUNs
    # Linux requires a different syntax: "$xxx" instead.
    # CCLD="$(AUX_LINKER) $(CXX)"
    # CXXLD="$(AUX_LINKER) $(CXX)"
      CCLD="$AUX_LINKER $CXX"
      CXXLD="$AUX_LINKER $CXX"
    ;;
  esac
else
      CCLD="$CC"
# Liao: 2/17/2009, enable CXXLD environment variable to be accepted during configuration,
# especially when roseTranslator is used and it is not ready to be used as a linker
      CXXLD="$CXXLD"
     if test "x$CXXLD" = x; then
         CXXLD="$CXX"
      fi 
# BP : 10/29/2001, the above two lines were originally 
#      CCLD="$(CC)"
#      CXXLD="$(CXX)"
# but on Linux systems this seems to not work
fi

AC_SUBST(AUX_LINKER)
echo "In macro ROSE SUPPORT INSURE: CC    = $CC"
echo "In macro ROSE SUPPORT PURIFY: CCLD = $CCLD"
echo "In macro ROSE SUPPORT INSURE: CXX   = $CXX"
echo "In macro ROSE SUPPORT PURIFY: CXXLD = $CXXLD"

AC_SUBST(CCLD)
AC_SUBST(CXXLD)

# End macro ROSE_SUPPORT_PURIFY.
]
)








AC_DEFUN([ROSE_SUPPORT_PYTHON],
[
# Begin macro ROSE_SUPPORT_PYTHON.

# Inclusion of test for Python version 2.4 and it's location.

AC_MSG_CHECKING(for python >= 2.4.0)
AC_ARG_WITH(python,
[  --with-python       Specify the path to the Python 2.4 interpreter (the binary itself)],
,
)

echo "In ROSE SUPPORT MACRO: with_python $with_python"

if test "$with_python" = yes;
then
    with_python=""
    cat 1>&2 << EOF
Incorrect use of option --with-python ($with_python).
Please use: --with-python=/path/to/python/bin/python
EOF
fi

PYTHONGUESSES="$with_python python2.4 python2.5 python2.6 python"
echo "Python interpreter guesses: $PYTHONGUESSES"

goodPy()
{
    if which $[1] > /dev/null 2>&1;
    then
        # DQ (12/28/2006): Can't use VERSION name (changed to PYTHON_VERSION), since it conflects with autoconf VERSION macro.
        PYTHON_VERSION=`$[1] -V 2>&1 | cut -f 2 -d " "`
        MAJOR=`echo $PYTHON_VERSION | cut -f 1 -d '.'`
        MINOR=`echo $PYTHON_VERSION | cut -f 2 -d '.'`
        RELEASE=`echo $PYTHON_VERSION | cut -f 3 -d '.'`
        if (test $MAJOR -eq 2) && (test $MINOR -ge 4);
        then
            return 0;
        fi
    fi
    return 1
}

python_path=''
for PYTHON in $PYTHONGUESSES
do
    if goodPy $PYTHON;
    then
        cat << EOF
Found a compatibale version of Python: $PYTHON
EOF
        python_path=$PYTHON
        break
    fi
done

if test -z "$python_path";
then
    echo "Could not find Python >=2.4.0!"
fi

AC_SUBST(python_path)

# End macro ROSE_SUPPORT_PYTHON.
]
)


AC_DEFUN([ROSE_SUPPORT_ROSE],
[
# Begin macro ROSE_SUPPORT_ROSE.

# *********************************************************************
# This macro encapsulates the complexity of the tests required for ROSE
# to understnd the machine environment and the configure command line.
# It is represented a s single macro so that we can simplify the ROSE
# configure.in and permit other external project to call this macro as 
# a way to set up there environment and define the many macros that an
# application using ROSE might require.
# *********************************************************************

# DQ (9/26/2009): this is not a defined macro.  This is handled in the build script.
# AM_PREREQ(1.9.6)

AMTAR ?= $(TAR)

# DQ (9/9/2009): Added output to test values of am__tar and am__untar (fails on nmi:x86_sles_9).
echo "Defined: am__tar   = $am__tar"
echo "Defined: am__untar = $am__untar"
echo "Defined: AMTAR     = $AMTAR"

# DQ (9/9/2009): Added test.
if test "$am__tar" = "false"; then
   echo "am__tar set to false -- this will be a problem later."
   exit 1
fi

# DQ (9/9/2009): Added test.
if test "$am__untar" = "false"; then
   echo "am__untar set to false -- this will be a problem later."
   exit 1
fi


# JJW (10/8/2008): Make default CFLAGS, CXXFLAGS, and FFLAGS empty (from
# <URL:http://osdir.com/ml/sysutils.autoconf.general/2003-11/msg00124.html>)
true ${CFLAGS:=}
true ${CXXFLAGS:=}
true ${FFLAGS:=}

# DQ (6/5/2007): Try to make package name use upper case for ROSE (this does not work)
# AC_PACKAGE_TARNAME([ROSE])

# DQ (3/20/2009): Trying to get information about what system we are on so that I
# can detect Cygwin and OSX (and other operating systems in the future).
AC_CANONICAL_BUILD
# AC_CANONICAL_HOST
# AC_CANONICAL_TARGET
echo "Building ROSE for cpu       = $build_cpu"
echo "Building ROSE for OS vendor = $build_vendor"
echo "Building ROSE for OS        = $build_os"

DETERMINE_OS

# DQ (3/20/2009): The default is to assume Linux, so skip supporting this test.
# AM_CONDITIONAL(ROSE_BUILD_OS_IS_LINUX,  [test "x$build_os" = xlinux-gnu])
AM_CONDITIONAL(ROSE_BUILD_OS_IS_OSX,    [test "x$build_vendor" = xapple])
AM_CONDITIONAL(ROSE_BUILD_OS_IS_CYGWIN, [test "x$build_os" = xcygwin])

# DQ (9/10/2009): A more agressive attempt to identify the OS vendor
# This sets up automake conditional variables for each OS vendor name.
DETERMINE_OS_VENDOR

# exit 1

# This appears to be a problem for Java (and so the Fortran support).
# CHECK_SSL
ROSE_SUPPORT_SSL

# Need the SSL automake conditional so that libssl can be added selectively for only those
# translators that require it (since it conflicts with use of Java, and thus Fortran support).
AM_CONDITIONAL(ROSE_USE_SSL_SUPPORT, [test "x$enable_ssl" = xyes])

configure_date=`date '+%A %B %e %H:%M:%S %Y'`
AC_SUBST(configure_date)
# echo "In ROSE/con figure: configure_date = $configure_date"

# DQ (1/27/2008): Added based on suggestion by Andreas.  This allows
# the binary analysis to have more specific information. However, it
# appears that it requires version 2.61 of autoconf and we are using 2.59.
# echo "$host_cpu"
# echo "host_cpu = $host_cpu"
# echo "host_vendor = $host_vendor"
# echo "ac_cv_host = $ac_cv_host"
# echo "host = $host"
# This does not currently work - I don't know why!
# AC_DEFINE([ROSE_HOST_CPU],$host_cpu,[Machine CPU Name where ROSE was configured.])

# DQ (9/7/2006): Allow the default prefix to be the current build tree
# This does not appear to work properly
# AC_PREFIX_DEFAULT(`pwd`)

# echo "In configure: prefix = $prefix"
# echo "In configure: pwd = $PWD"

if test "$prefix" = NONE; then
   echo "Setting prefix to default: $PWD"
   prefix="$PWD"
# else
#   echo "prefix was exlicitly set to: $prefix"
fi
# echo "In configure (after testing prefix): prefix = $prefix"
# echo "In configure (after testing prefix): prefix = $libdir"

# exit 1

#AC_MSG_WARN([Exiting as a test!])
#AC_MSG_ERROR([Exiting as a test!])
#echo "Exiting as a test!"
#exit 1

# JJW: This needs to be early as things like C++ header editing are not done for the new interface
AC_ARG_ENABLE(new-edg-interface, AS_HELP_STRING([--enable-new-edg-interface], [Enable new (experimental) translator from EDG ASTs to Sage ASTs]))
AM_CONDITIONAL(ROSE_USE_NEW_EDG_INTERFACE, [test "x$enable_new_edg_interface" = xyes])
if test "x$enable_new_edg_interface" = "xyes"; then
  AC_MSG_WARN([Using newest version of interface to translate EDG to ROSE (experimental)!])
  AC_DEFINE([ROSE_USE_NEW_EDG_INTERFACE], [], [Whether to use the new interface to EDG])
fi

# DQ (12/29/2008): the default is new EDG interface is 3.10, this option permits the use
# of the newer EDG 4.0 interface (which breaks some existing work).
AC_ARG_ENABLE(edg-version4, AS_HELP_STRING([--enable-edg-version4], [Enable newest EDG version 4 (requires --enable-new-edg-interface option)]))
AM_CONDITIONAL(ROSE_USE_EDG_VERSION_4, [test "x$enable_edg_version4" = xyes])
if test "x$enable_edg_version4" = "xyes"; then
  AC_MSG_WARN([Using newest EDG version 4.x (requires new interface) to translate EDG to ROSE (experimental)!])
  AC_DEFINE([ROSE_USE_EDG_VERSION_4], [], [Whether to use the new EDG version 4.x])
fi

# DQ (1/4/2009) Added support for optional GNU language extensions in new EDG/ROSE interface.
# This value will be substituted into EDG/4.0/src/rose_lang_feat.h in the future (not used at present!)
AC_ARG_ENABLE(gnu-extensions, AS_HELP_STRING([--enable-gnu-extensions], [Enable internal support in ROSE for GNU language extensions]))
if test "x$enable_gnu_extensions" = "xyes"; then
  ROSE_SUPPORT_GNU_EXTENSIONS="TRUE"
else
  ROSE_SUPPORT_GNU_EXTENSIONS="FALSE"
fi
AC_SUBST(ROSE_SUPPORT_GNU_EXTENSIONS)

# DQ (1/4/2009) Added support for optional Microsoft language extensions in new EDG/ROSE interface.
# This value will be substituted into EDG/4.0/src/rose_lang_feat.h in the future (not used at present!)
AC_ARG_ENABLE(microsoft-extensions, AS_HELP_STRING([--enable-microsoft-extensions], [Enable internal support in ROSE for Microsoft language extensions]))
if test "x$enable_microsoft_extensions" = "xyes"; then
  ROSE_SUPPORT_MICROSOFT_EXTENSIONS="TRUE"
else
  ROSE_SUPPORT_MICROSOFT_EXTENSIONS="FALSE"
fi
AC_SUBST(ROSE_SUPPORT_MICROSOFT_EXTENSIONS)


# DQ (8/18/2009): Removed this conditional macro.
# DQ (4/23/2009): Added support for commandline specification of using new graph IR nodes.
# AC_ARG_ENABLE(newGraphNodes, AS_HELP_STRING([--enable-newGraphNodes], [Enable new (experimental) graph IR nodes]))
#AM_CONDITIONAL(ROSE_USE_NEW_GRAPH_NODES, [test "x$enable_newGraphNodes" = xyes])
#if test "x$enable_newGraphNodes" = "xyes"; then
#  AC_MSG_WARN([Using the new graph IR nodes in ROSE (experimental)!])
#  AC_DEFINE([ROSE_USE_NEW_GRAPH_NODES], [], [Whether to use the new graph IR nodes])
#fi

# DQ (5/2/2009): Added support for backward compatability of new IR nodes with older API.
AC_ARG_ENABLE(use_new_graph_node_backward_compatability,
    AS_HELP_STRING([--enable-use_new_graph_node_backward_compatability], [Enable new (experimental) graph IR nodes backward compatability API]))
AM_CONDITIONAL(ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY, [test "x$enable_use_new_graph_node_backward_compatability" = xyes])
if test "x$enable_use_new_graph_node_backward_compatability" = "xyes"; then
  AC_MSG_WARN([Using the new graph IR nodes in ROSE (experimental)!])
  AC_DEFINE([ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY], [], [Whether to use the new graph IR nodes compatability option with older API])
fi


#AM_CONDITIONAL(ROSE_USE_QROSE,test "$with_qrose" = true)

# Set up for use of bison to build dot2gml tool in directory
# src/roseIndependentSupport/dot2gml.  This is made optional
# because it seems that many don't have the correct version of bison
# to support the compilation of this tool.  This is it is a configure
# option to build it (or have the makefile system have it be built).
AC_ARG_ENABLE(dot2gml_translator,
[--enable-dot2gml_translator   Configure option to have DOT to GML translator built (bison version specific tool).],
[ echo "Setting up optional DOT-to-GML translator in directory: src/roseIndependentSupport/dot2gml"
])
AM_CONDITIONAL(DOT_TO_GML_TRANSLATOR,test "$enable_dot2gml_translator" = yes)

# exit 1

# Set the value of srcdir so that it will be an absolute path instead of a relative path
# srcdir=`dirname "$0"`
# echo "In ROSE/con figure: srcdir = $srcdir"
# echo "In ROSE/con figure: $0"
# Record the location of the build tree (so it can be substituted into ROSE/docs/Rose/rose.cfg)
# topSourceDirectory=`dirname "$0"`
# echo "In ROSE/con figure: topSourceDirectory = $topSourceDirectory"
# AC_SUBST(topSourceDirectory)

# echo "Before test for CANONICAL HOST: CC (CC = $CC)"

AC_CANONICAL_HOST

# DQ: added here to see if it would be defined for the template tests and avoid placing 
# a $(CXX_TEMPLATE_REPOSITORY_PATH) directory in the top level build directory (a minor error)
CXX_TEMPLATE_REPOSITORY_PATH='$(top_builddir)/src'

# ROSE_HOME should be relative to top_srcdir or top_builddir.
ROSE_HOME=.
# ROSE_HOME=`pwd`/$top_srcdir
AC_SUBST(ROSE_HOME)
# echo "In ROSE/configure: ROSE_HOME = $ROSE_HOME"

# This does not appear to exist any more
# Support for Gabriel's QRose GUI Library
# ROSE_SUPPORT_QROSE

AC_LANG(C++)
AX_BOOST_BASE([1.35.0], [], [echo "Boost 1.35.0 or above is required for ROSE" 1>&2; exit 1])
AC_SUBST(ac_boost_path) dnl Hack using an internal variable from AX_BOOST_BASE -- this path should only be used to set --with-boost in distcheck

# DQ (12/22/2008): Fix boost configure to handle OS with older version of Boost that will
# not work with ROSE, and use the newer version specified by the user on the configure line.
echo "In ROSE/configure: ac_boost_path = $ac_boost_path"
#AC_DEFINE([ROSE_BOOST_PATH],"$ac_boost_path",[Location of Boost specified on configure line.])
AC_DEFINE_UNQUOTED([ROSE_BOOST_PATH],"$ac_boost_path",[Location (unquoted) of Boost specified on configure line.])
#AC_DEFINE([ROSE_WAVE_PATH],"$ac_boost_path/wave",[Location of Wave specified on configure line.])
AC_DEFINE_UNQUOTED([ROSE_WAVE_PATH],"$ac_boost_path/wave",[Location (unquoted) of Wave specified on configure line.])

# DQ (11/5/2009): Added test for GraphViz's ``dot'' program
ROSE_SUPPORT_GRAPHVIZ
# exit 1

AX_BOOST_THREAD
AX_BOOST_DATE_TIME
AX_BOOST_REGEX
AX_BOOST_PROGRAM_OPTIONS
#AX_BOOST_SERIALIZATION
#AX_BOOST_ASIO
#AX_BOOST_SIGNALS
#AX_BOOST_TEST_EXEC_MONITOR
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM
AX_BOOST_WAVE

# AM_CONDITIONAL(ROSE_USE_BOOST_WAVE,test "$with_wave" = true)

AX_LIB_SQLITE3
AX_LIB_MYSQL

# DQ (9/15/2009): I have moved this to before the backend compiler selection so that
# we can make the backend selection a bit more compiler dependent. Actually we likely
# don't need this!
# DQ (9/17/2006): These should be the same for both C and C++ (else we will need separate macros)
# Setup the -D<xxx> defines required to allow EDG to take the same path through the compiler 
# specific and system specific header files as for the backend compiler.  These depend
# upon the selection of the back-end compiler.
# GET_COMPILER_SPECIFIC_DEFINES

# Test this macro here at the start to avoid long processing times (before it fails)
CHOOSE_BACKEND_COMPILER


# End macro ROSE_SUPPORT_ROSE.
]
)





AC_DEFUN([ROSE_SUPPORT_ROSE_PART_2],
[
# Begin macro ROSE_SUPPORT_ROSE.

# For testing the configure script generation this link can be commented out
# to improve performance of tests unrelated to backend compiler headr files.
# DQ (9/17/2006): This must be done for BOTH C++ and C compilers (since the
# compiler-specific header files for each can be different; as is the case 
# for GNU).
# GENERATE_BACKEND COMPILER_SPECIFIC_HEADERS
# GENERATE_BACKEND_CXX_COMPILER_SPECIFIC_HEADERS

# echo "DONE: configure.in ...(after calling: generate backend C compiler specific headers)"
# echo "Exiting in configure.in ...(after calling: generate backend C compiler specific headers)"
# exit 1

# AC_REQUIRE([AC_PROG_CXX])
AC_PROG_CXX

echo "In configure.in ... CXX = $CXX"
# exit 1

# DQ (9/17/2006): These should be the same for both C and C++ (else we will need separate macros)
# Setup the -D<xxx> defines required to allow EDG to take the same path through the compiler 
# specific and system specific header files as for the backend compiler.  These depend
# upon the selection of the back-end compiler.
GET_COMPILER_SPECIFIC_DEFINES

# DQ (1/15/2007): These are no longer used, I think!
# Setup the location of header files after building the 
# default header files based on the back-end compiler.
# ROSE_C_HEADER_OPTIONS
# ROSE_CXX_HEADER_OPTIONS

# echo "DONE: configure.in ...(after calling: rose C and Cxx header options)"
# echo "Exiting in configure.in ...(after calling: rose C and Cxx header options)"
# exit 1

# This must go after the setup of the headers options
# Setup the CXX_INCLUDE_STRING to be used by EDG to find the correct headers
# SETUP_BACKEND_COMPILER_SPECIFIC_REFERENCES
# JJW (12/10/2008): We don't preprocess the header files for the new interface,
# but we still need to use the original C++ header directories
SETUP_BACKEND_C_COMPILER_SPECIFIC_REFERENCES
SETUP_BACKEND_CXX_COMPILER_SPECIFIC_REFERENCES

# echo "DONE: configure.in ...(after calling: setup backend C and Cxx compiler specific references)"
# echo "Exiting in configure.in ...(after calling: setup backend C and Cxx compiler specific references)"
# exit 1

# echo "Before test for LEX: CC (CC = $CC)"

# DQ (1/15/2007): Check if longer internal make check rule is to be used (default is short tests)
ROSE_SUPPORT_LONG_MAKE_CHECK_RULE

# Make the use of longer test optional where it is used in some ROSE/tests directories
AM_CONDITIONAL(ROSE_USE_LONG_MAKE_CHECK_RULE,test "$with_ROSE_LONG_MAKE_CHECK_RULE" = yes)

# The libxml2 library is availabe in /usr/lib on most Linux systems, however this is not
# enough when using the Intel compilers.  So we need to turn it on explicitly when we
# expect it to work with a specific platform/compiler combination.

# JJW -- use standard version in /usr/share/aclocal, and configure XML only
# once for roseHPCT and BinaryContextLookup
with_xml="no"
AM_PATH_XML2(2.0.0, [with_xml="yes"])

# Make the use of libxml2 explicitly controlled.
AM_CONDITIONAL(ROSE_USE_XML,test "$with_xml" != no)

# DQ (10/17/2009): This is a bug introduced (again) into ROSE which disables the Java support.
# See elsewhere in this file where this macro is commented out and the reason explained in 
# more details.
# AS Check for ssl for the binary clone detection work
# CHECK_SSL

# Check for objdump for BinaryContextLookup since it doesn't normally exist on
# Mac
AC_CHECK_TOOL(ROSE_OBJDUMP_PATH, [objdump], [no])
AM_CONDITIONAL(ROSE_USE_OBJDUMP, [test "$ROSE_OBJDUMP_PATH" != "no"])
AM_CONDITIONAL(ROSE_USE_BINARYCONTEXTLOOKUP, [test "$with_xml" != "no" -a "$ROSE_OBJDUMP_PATH" != "no"])

AC_C_BIGENDIAN
AC_CHECK_HEADERS([byteswap.h machine/endian.h])

# PKG_CHECK_MODULES([VALGRIND], [valgrind], [with_valgrind=yes; AC_DEFINE([ROSE_USE_VALGRIND], 1, [Use Valgrind calls in ROSE])], [with_valgrind=no])
VALGRIND_BINARY=""
AC_ARG_WITH(valgrind, [  --with-valgrind ... Run uninitialized field tests that use Valgrind],
            [AC_DEFINE([ROSE_USE_VALGRIND], 1, [Use Valgrind calls in ROSE])
             if test "x$withval" = "xyes"; then VALGRIND_BINARY="`which valgrind`"; else VALGRIND_BINARY="$withval"; fi])

AC_ARG_WITH(wave-default, [  --with-wave-default ... Use Wave as the default preprocessor],
            [AC_DEFINE([ROSE_WAVE_DEFAULT], true, [Use Wave as default in ROSE])],
            [AC_DEFINE([ROSE_WAVE_DEFAULT], false, [Simple preprocessor as default in ROSE])]
            )

# Don't set VALGRIND here because that turns on actually running valgrind in
# many tests, as opposed to just having the path available for
# uninitializedField_tests
AC_SUBST(VALGRIND_BINARY)
AM_CONDITIONAL(USE_VALGRIND, [test "x$VALGRIND_BINARY" != "x"])

# Add --disable-binary-analysis-tests flag to turn off tests that sometimes
# sometimes break.
AC_ARG_ENABLE(binary-analysis-tests, AS_HELP_STRING([--disable-binary-analysis-tests], [Disable tests of ROSE binary analysis code]), binary_analysis_tests="$withval", binary_analysis_tests=yes)
AM_CONDITIONAL(USE_BINARY_ANALYSIS_TESTS, test "x$binary_analysis_tests" = "xyes")

# Figure out what version of lex we have available
# flex works better than lex (this gives a preference to flex (flex is gnu))
AM_PROG_LEX
AC_SUBST(LEX)
AC_PROG_YACC
AC_SUBST(YACC)

# echo "After test for LEX: CC (CC = $CC)"

# DQ (4/1/2001) Need to call this macro to avoid having "MAKE" set to "make" in the
# top level Makefile (this is important to getting gmake to be used in the "make distcheck"
# makefile rule.  (This does not seem to work, since calling "make distcheck" still fails and
# only "gmake distcheck" seems to work.  I don't know why!
AC_PROG_MAKE_SET

# Call supporting macro for MySQL (more complex than SQLite, MySQL will be removed from optional use within ROSE)
ROSE_SUPPORT_MYSQL
# echo "In configure: with_MySQL = $with_MySQL"

# Setup Automake conditional in Projects/DataBase/Makefile.am
AM_CONDITIONAL(ROSE_USE_MYSQL_DATABASE,[test "x$MYSQL_VERSION" != "x"])

# Setup Automake conditional in ROSE/projects/DataBase/Makefile.am
AM_CONDITIONAL(ROSE_USE_SQLITE_DATABASE,[test "x$SQLITE3_VERSION" != "x"])

# DQ (9/21/2009): Debugging for RH release 5
echo "Testing the value of CC: (CC = $CC)"
echo "Testing the value of CPPFLAGS: (CPPFLAGS = $CPPFLAGS)"

# exit 1

# Call supporting macro for MAPLE
ROSE_SUPPORT_MAPLE

# Setup Automake conditional in Projects/programModeling/Makefile.am
AM_CONDITIONAL(ROSE_USE_MAPLE,test ! "$with_maple" = no)

#Call supporting macro for IDA PRO
ROSE_SUPPORT_IDA

# Setup Automake conditional in projects/AstEquivalence/Makefile.am
AM_CONDITIONAL(ROSE_USE_IDA,test ! "$with_ida" = no)

# Call supporting macro to Yices Satisfiability Modulo Theories (SMT) Solver
ROSE_SUPPORT_YICES

# Setup Automake conditional in --- (not yet distributed)
AM_CONDITIONAL(ROSE_USE_YICES,test ! "$with_yices" = no)

# Call supporting macro to Intel Pin Dynamic Instrumentation
ROSE_SUPPORT_INTEL_PIN

# Setup Automake conditional in --- (not yet distributed)
AM_CONDITIONAL(ROSE_USE_INTEL_PIN,test ! "$with_IntelPin" = no)

# Call supporting macro to DWARF (libdwarf)
ROSE_SUPPORT_DWARF

# Setup Automake conditional in --- (not yet distributed)
AM_CONDITIONAL(ROSE_USE_DWARF,test ! "$with_dwarf" = no)

# Call supporting macro for libffi (Foreign Function Interface library)
# This library is used by Peter's work on the Interpreter in ROSE.
ROSE_SUPPORT_LIBFFI

# Setup Automake conditional in projects/interpreter/Makefile.am
AM_CONDITIONAL(ROSE_USE_LIBFFI,test ! "$with_libffi" = no)

TEST_SMT_SOLVER=""
AC_ARG_WITH(smt-solver,
[  --with-smt-solver=PATH	Specify the path to an SMT-LIB compatible SMT solver.  Used only for testing.],
if test "x$with_smt_solver" = "xcheck" -o "x$with_smt_solver" = "xyes"; then
  AC_ERROR([--with-smt-solver cannot be auto-detected])
fi
if test "x$with_smt_solver" != "xno"; then
  TEST_SMT_SOLVER="$with_smt_solver"
fi,
)

AM_CONDITIONAL(ROSE_USE_TEST_SMT_SOLVER,test ! "$TEST_SMT_SOLVER" = "")
AC_SUBST(TEST_SMT_SOLVER)

# DQ (3/13/2009): Trying to get Intel Pin and ROSE to both use the same version of libdwarf.
# DQ (3/10/2009): The Dwarf support in Intel Pin conflicts with the Dwarf support in ROSE.
# Maybe there is a way to fix this later, for now we want to disallow it.
# echo "with_dwarf    = $with_dwarf"
# echo "with_IntelPin = $with_IntelPin"
#if test "$with_dwarf" != no && test "$with_IntelPin" != no; then
# # echo "Support for both DWARF and Intel Pin fails, these configure options are incompatable."
#   AC_MSG_ERROR([Support for both DWARF and Intel Pin fails, these configure options are incompatable!])
#fi

ROSE_SUPPORT_PHP

AM_CONDITIONAL(ROSE_USE_PHP,test ! "$with_php" = no)

#TPS (03/13/08) introduced optional DB support for binary work
HASH_ROSE_BINARY_SQL

AM_CONDITIONAL(ROSE_USE_BINARY_SQL,test ! "$with_binarysql" = no)


# Call supporting macro for Windows Source Code Analysis
ROSE_SUPPORT_WINDOWS_ANALYSIS

# Setup Automake conditional in Projects/programModeling/Makefile.am
AM_CONDITIONAL(ROSE_USE_WINDOWS_ANALYSIS_SUPPORT,test ! "$with_wine" = no)

# Control use of debugging support to convert most unions in EDG to structs.
ROSE_SUPPORT_EDG_DEBUGGING

# Call supporting macro for Omni OpenMP
# 
ROSE_SUPPORT_OMNI_OPENMP

# Configuration commandline support for OMP project using ROSE
#AM_CONDITIONAL(ROSE_USE_OPENMP,test ! "$with_omp_runtime_support" = no)
AM_CONDITIONAL(ROSE_USE_OMNI_OPENMP,test ! "$with_omni_omp_runtime_support" = no)

# call supporting macro for GCC 4.4.x gomp OpenMP runtime library
# AM_CONDITIONAL is already included into the macro
ROSE_WITH_GOMP_OPENMP_LIBRARY

# Call supporting macro for GCC OpenMP
ROSE_SUPPORT_GCC_OMP

# Configuration commandline support for OpenMP in ROSE
AM_CONDITIONAL(ROSE_USE_GCC_OMP,test ! "$with_gcc_omp" = no)


# JJW and TP (3-17-2008) -- added MPI support
AC_ARG_WITH(mpi,
[--with-mpi                    Configure option to have MPI-based tools built.],
[ echo "Setting up optional MPI-based tools"
])
AM_CONDITIONAL(ROSE_MPI,test "$with_mpi" = yes)
AC_CHECK_TOOLS(MPICXX, [mpiCC mpic++ mpicxx])


# TPS (2-11-2009) -- added PCH Support
AC_ARG_WITH(pch,
[--with-pch                    Configure option to have pre-compiled header support enabled.],
[ echo "Enabling precompiled header"
])
AM_CONDITIONAL(ROSE_PCH,test "$with_pch" = yes)
if test "x$with_pch" = xyes; then
  CPPFLAGS="-U_REENTRANT $CPPFLAGS";
  AC_MSG_NOTICE( "PCH enabled: You got the following CPPFLAGS: $CPPFLAGS" );
if test "x$with_mpi" = xyes; then
  AC_MSG_ERROR( "PCH Support cannot be configured together with MPI support" );
fi
if test "x$with_gcc_omp" = xyes; then
  AC_MSG_ERROR( "PCH Support cannot be configured together with GCC_OMP support" );
fi
else
  AC_MSG_NOTICE( "PCH disabled: No Support for PCH." );
fi


# TP (2-27-2009) -- support for RTED
ROSE_SUPPORT_RTED

AM_CONDITIONAL(ROSE_USE_RTED,test ! "$with_rted" = no)

# TP SUPPORT FOR OPENGL
AC_PATH_X dnl We need to do this by hand for some reason
MDL_HAVE_OPENGL
# echo "have_GL = '$have_GL' and have_glut = '$have_glut'"
AM_CONDITIONAL(ROSE_USE_OPENGL, test ! "x$have_GL" = xno -a ! "x$have_glut" = xno)

# Call supporting macro for python
ROSE_SUPPORT_PYTHON

AC_CHECK_PROGS(PERL, [perl])

# DQ (9/4/2009): Added checking for indent command (common in Linux, but not on some platforms).
# This command is used in the tests/roseTests/astInterfaceTests/Makefile.am file.
AC_CHECK_PROGS(INDENT, [indent])
AM_CONDITIONAL(ROSE_USE_INDENT, [test "x$INDENT" = "xindent"])
echo "value of INDENT variable = $INDENT"

# DQ (9/30/2009): Added checking for tclsh command (common in Linux, but not on some platforms).
# This command is used in the src/frontend/BinaryDisassembly/Makefile.am file.
AC_CHECK_PROGS(TCLSH, [tclsh])
AM_CONDITIONAL(ROSE_USE_TCLSH, [test "x$TCLSH" = "xtclsh"])
echo "value of TCLSH variable = $TCLSH"

# DQ & PC (11/3/2009): Debugging the Java support.
if false; then
if test "x$JAVA_HOME" = "x"; then
  JAVA="`which javac`"
  if test -f /usr/bin/javaconfig; then # Mac Java
    :
  else
    while test `readlink "$JAVA"` ; do 
      JAVA=`readlink "$JAVA"` ; 
   done

   if test $JAVA = "gcj"; then 
      AC_MSG_ERROR( "Error: gcj not supported. Please configure sun java as javac" );
   fi

  fi
  JAVA_HOME="`dirname $JAVA`/.."
fi
fi
# Call supporting macro for the Java path required by the Open Fortran Parser (for Fortran 2003 support)
# Use our classpath in case the user's is messed up
AS_SET_CATFILE([ABSOLUTE_SRCDIR], [`pwd`], [${srcdir}])
CLASSPATH=${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/antlr-2.7.7.jar:${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/antlr-3.0.1.jar:${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/antlr-runtime-3.0.1.jar:${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/stringtemplate-3.1b1.jar:.
export CLASSPATH
AC_SUBST(CLASSPATH)
ROSE_SUPPORT_JAVA # This macro uses JAVA_HOME

OPEN_FORTRAN_PARSER_PATH="${ac_top_builddir}/src/3rdPartyLibraries/fortran-parser" # For the one rule that uses it
AC_SUBST(OPEN_FORTRAN_PARSER_PATH)

AX_WITH_PROG(GFORTRAN_PATH, [gfortran], [])

ofp_enabled=no
AC_MSG_CHECKING([whether Fortran support can be used])
if test "x$USE_JAVA" = x1; then
  CPPFLAGS="$CPPFLAGS $JAVA_JVM_INCLUDE"
  if test "x$GFORTRAN_PATH" != "x"; then
    AC_DEFINE([USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT], [1], [Always enable Fortran support whenever Java and gfortran are present])
    ofp_enabled=yes
    AC_MSG_RESULT([yes])
    AC_DEFINE([USE_GFORTRAN_IN_ROSE], [1], [Mark that GFORTRAN is available])

  # Test that we have correctly evaluated the major and minor versions numbers...
    if test x$BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER == x; then
      echo "Error: Could not compute the MAJOR version number of $BACKEND_FORTRAN_COMPILER"
      exit 1
    fi
    if test x$BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER == x; then
      echo "Error: Could not compute the MINOR version number of $BACKEND_FORTRAN_COMPILER"
      exit 1
    fi
  else
    AC_MSG_RESULT([no ... gfortran cannot be found (try --with-gfortran)])
  fi
else
  AC_MSG_RESULT([no ... Java cannot be found (try --with-java)])
fi
AM_CONDITIONAL(ROSE_USE_OPEN_FORTRAN_PARSER, [test "x$ofp_enabled" = "xyes"])
AC_SUBST(GFORTRAN_PATH)

AC_PROG_SWIG(1.3.31)
SWIG_ENABLE_CXX
#AS (10/23/07): introduced conditional use of javaport
AC_ARG_WITH(javaport,
   [  --with-javaport ... Enable Java bindings using Swig],
   [with_javaport=yes],
   [with_javaport=no])
AM_CONDITIONAL(ENABLE_JAVAPORT,test "$with_javaport" = yes)

if test "x$with_javaport" = "xyes"; then
  if test "x$USE_JAVA" = "x0"; then
    AC_MSG_ERROR([Trying to enable --with-javaport without --with-java also being set])
  fi
  if /bin/sh -c "$SWIG -version" >& /dev/null; then
    :
  else
    AC_MSG_ERROR([Trying to enable --with-javaport without SWIG installed])
  fi
  AC_MSG_WARN([Enabling Java binding support -- SWIG produces invalid C++ code, so -fno-strict-aliasing is being added to CXXFLAGS to work around this issue.  If you are not using GCC as a compiler, this flag will need to be changed.])
  CXXFLAGS="$CXXFLAGS -fno-strict-aliasing"
fi

# Call supporting macro for Haskell
ROSE_SUPPORT_HASKELL

# Call supporting macro for bddbddb
ROSE_SUPPORT_BDDBDDB

# Setup Automake conditional in Projects/DatalogAnalysis/Makefile.am
AM_CONDITIONAL(ROSE_USE_BDDBDDB,test ! "$with_bddbddb" = no)

# Call supporting macro for VISUALIZATION (FLTK and GraphViz)
ROSE_SUPPORT_VISUALIZATION

# if ((test ! "$with_FLTK_include" = no) || (test ! "$with_FLTK_libs" = no) || (test ! "$with_GraphViz_include" = no) || (test ! "$with_GraphViz_libs" = no)); then
#   echo "Skipping visualization support!"
# else
#   echo "Setting up visualization support!"
# fi

# Setup Automake conditional in src/roseIndependentSupport/visualization/Makefile.am
AM_CONDITIONAL(ROSE_USE_VISUALIZATION,(test ! "$with_FLTK_include" = no) || (test ! "$with_FLTK_libs" = no) || (test ! "$with_GraphViz_include" = no) || (test ! "$with_GraphViz_libs" = no))

# allow either user or developer level documentation using Doxygen
ROSE_SUPPORT_DOXYGEN

# DQ (8/25/2004): Disabled fast docs option.
# Setup Automake conditional to allow use of Doxygen Tag file to speedup
# generation of Rose documentation this does not however provide the
# best organized documentation so we use it as an option to speed up
# the development of the documenation and then alternatively build the 
# final documentation.
# AM_CONDITIONAL(DOXYGEN_GENERATE_FAST_DOCS,test "$enable_doxygen_generate_fast_docs" = yes)
# echo "In configure.in: enable_doxygen_generate_fast_docs = $enable_doxygen_generate_fast_docs"

# Test for setup of document merge of Sage docs with Rose docs
# Causes document build process to take longer but builds better documentation
if (test "$enable_doxygen_generate_fast_docs" = yes) ; then
   AC_MSG_NOTICE([Generate Doxygen documentation faster (using tag file mechanism) ...])
else
   AC_MSG_NOTICE([Generate Doxygen documentation slower (reading all of Sage III and Rose together) ...])
fi

AC_PROG_CXXCPP
dnl AC_PROG_RANLIB
# echo "In configure.in (before libtool win32 setup): libtool test for 64 bit libs = `/usr/bin/file conftest.o`"
dnl AC_LIBTOOL_WIN32_DLL -- ROSE is probably not set up for this

# echo "In configure.in (before libtool setup): disabling static libraries by default (use --enable-static or --enable-static= to override)"
AC_DISABLE_STATIC

# echo "In configure.in (before libtool setup): libtool test for 64 bit libs = `/usr/bin/file conftest.o`"
LT_AC_PROG_SED dnl This seems to not be called, even though it is needed in the other macros
m4_pattern_allow([LT_LIBEXT])dnl From http://www.mail-archive.com/libtool-commit@gnu.org/msg01369.html
AC_PROG_LIBTOOL
AC_LIBLTDL_CONVENIENCE dnl We need to use our version because libtool can't handle when we use libtool v2 but the v1 libltdl is installed on a system
AC_SUBST(LTDLINCL)
AC_SUBST(LIBLTDL)
AC_LIBTOOL_DLOPEN
AC_LIB_LTDL(recursive)
dnl AC_LT DL_SHLIBPATH dnl Get the environment variable like LD_LIBRARY_PATH for the Fortran support to use
dnl This seems to be an internal variable, set by different macros in different
dnl Libtool versions, but with the same name
AC_DEFINE_UNQUOTED(ROSE_SHLIBPATH_VAR, ["$shlibpath_var"], [Variable like LD_LIBRARY_PATH])

echo 'int i;' > conftest.$ac_ext
AC_TRY_EVAL(ac_compile);
# echo "In configure.in (after libtool setup): libtool test for 64 bit libs = `/usr/bin/file conftest.o`"

# Various functions for finding the location of librose.* (used to make the
# ROSE executables relocatable to different locations without recompilation on
# some platforms)
AC_CHECK_HEADERS([dlfcn.h], [have_dladdr=yes], [have_dladdr=no])
if test "x$have_dladdr" = "xyes"; then
  AC_CHECK_LIB([dl], [dladdr], [], [have_dladdr=no])
fi
if test "x$have_dladdr" = "xyes"; then
  AC_DEFINE([HAVE_DLADDR], [], [Whether <dlfcn.h> and -ldl contain dladdr()])
  use_rose_in_build_tree_var=no
else
  AC_MSG_WARN([ROSE cannot find the locations of loaded shared libraries using your dynamic linker.  ROSE can only be used with the given build directory or prefix, and the ROSE_IN_BUILD_TREE environment variable must be used to distinguish the two cases.])
  use_rose_in_build_tree_var=yes
fi
AM_CONDITIONAL(USE_ROSE_IN_BUILD_TREE_VAR, [test "x$use_rose_in_build_tree_var" = "xyes"])

# exit 1

# Figure out what version of lex we have available
# flex works better than lex (this gives a preference to flex (flex is gnu))
dnl AM_PROG_LEX
dnl AC_SUBST(LEX)
# This will work with flex and lex (but flex will not set LEXLIB to -ll unless it finds the gnu
# flex library which is not often installed (and at any rate not installed on our system at CASC)).
# Once the lex file contains its own version of yywrap then we will not need this set explicitly.

# next two lines commented out by BP : 10/29/2001,
# the flex library IS installed on our systems, setting it to -ll causes problems on
# Linux systems
# echo "Setting LEXLIB explicitly to -ll (even if flex is used: remove this once lex file contains it's own version of yywrap)"
# dnl LEXLIB='-ll'
# dnl AC_SUBST(LEXLIB)

# Determine what C++ compiler is being used.
AC_MSG_CHECKING(what the C++ compiler $CXX really is)
BTNG_INFO_CXX_ID
AC_MSG_RESULT($CXX_ID-$CXX_VERSION)

# Define various C++ compiler options.
# echo "Before ROSE_FLAG _ CXX_OPTIONS macro"
ROSE_FLAG_CXX_OPTIONS
# echo "Outside of ROSE_FLAG _ CXX_OPTIONS macro: CXX_DEBUG= $CXX_DEBUG"

# Enable turning on purify and setting its options, etc.
ROSE_SUPPORT_PURIFY
# echo "In ROSE/configure: AUX_LINKER = $AUX_LINKER" 

# Enable turning on Insure and setting its options, etc.
ROSE_SUPPORT_INSURE
# echo "In ROSE/configure: AUX_LINKER = $AUX_LINKER" 

# DQ (7/8/2004): Added support for shared libraries using Brian's macros
# ROSE_TEST_LIBS="-L`pwd`/src"

# DQ (9/7/2006): build the directory where libs will be placed.
# mkdir -p $prefix/libs
# echo "Before calling \"mkdir -p $prefix/lib\": prefix = $prefix"
# mkdir -p $prefix/lib

# DQ (1/14/2007): I don't think this is required any more!
# ROSE_TEST_LIBS="-L$prefix/lib"

# DQ (1/14/2007): I don't know if this is required, but too many people are resetting this variable!
# LIBS_WITH_RPATH="$(WAVE_LIBRARIES)"

dnl PC (09/15/2006): None of the following should not be relevant any more
dnl
dnl echo "Calling LIBS_ADD_RPATH ROSE_TEST_LIBS = $ROSE_TEST_LIBS"
dnl # Macro copied from Brian Gummey's implementation and turned on by default.
dnl ROSE_LIBS_ADD_RPATH(ROSE_TEST_LIBS,LIBS_WITH_RPATH,0)
dnl 
dnl # This is part of support for Boost-Wave (CPP Preprocessor Library)
dnl # Only add the Boost-Wave library to rpath if it has been set
dnl if (test "$with_boost_wave" = yes); then
dnl    MY_WAVE_PATH="-L$wave_libraries"
dnl    ROSE_LIBS_ADD_RPATH(MY_WAVE_PATH,LIBS_WITH_RPATH,0)
dnl fi
dnl 
dnl echo "DONE: MY_WAVE_PATH                   = $MY_WAVE_PATH"
dnl echo "DONE: LIBS_ADD_RPATH ROSE_TEST_LIBS  = $ROSE_TEST_LIBS"
dnl echo "DONE: LIBS_ADD_RPATH LIBS_WITH_RPATH = $LIBS_WITH_RPATH"
dnl 
dnl # exit 1
dnl 
dnl # This is part of support for QRose (specification of QT Graphics Library)
dnl # Only add the QT library to rpath if it has been set
dnl if (test "$ac_qt_libraries"); then
dnl    MY_QT_PATH="-L$ac_qt_libraries"
dnl    ROSE_LIBS_ADD_RPATH(MY_QT_PATH,LIBS_WITH_RPATH,0)
dnl fi
dnl 
dnl echo "DONE: MY_QT_PATH                     = $MY_QT_PATH"
dnl echo "DONE: LIBS_ADD_RPATH ROSE_TEST_LIBS  = $ROSE_TEST_LIBS"
dnl echo "DONE: LIBS_ADD_RPATH LIBS_WITH_RPATH = $LIBS_WITH_RPATH"

AC_SUBST(LIBS_WITH_RPATH)

# DQ (exit to test rpath macro)
# exit 1

# Determine how to create C++ libraries.
AC_MSG_CHECKING(how to create C++ libraries)
BTNG_CXX_AR
AC_MSG_RESULT($CXX_STATIC_LIB_UPDATE and $CXX_DYNAMIC_LIB_UPDATE)

# DQ (6/23/2004) Commented out due to warning in running build
# I do not know why in this case, INCLUDES is not generically
# defined and automatically substituted.  It usually is.  BTNG.
# INCLUDES='-I. -I$(srcdir) -I$(top_builddir)'
# AC_SUBST(INCLUDES)

# We don't need to select between SAGE 2 and SAGE 3 anymore (must use SAGE 3)
# SAGE_VAR_INCLUDES_AND_LIBS

# Let user specify where to find A++P++ installation.
# Specify by --with-AxxPxx= or setting AxxPxx_PREFIX.
# Note that the prefix specified should be that specified
# when installing A++P++.  The prefix appendages are also
# added here.
# BTNG.
AC_MSG_CHECKING(for A++P++)
AC_ARG_WITH(AxxPxx,
[  --with-AxxPxx=PATH	Specify the prefix where A++P++ is installed],
,
if test "$AxxPxx_PREFIX" ; then 
   with_AxxPxx="$AxxPxx_PREFIX"
else
   with_AxxPxx=no
fi
)
test "$with_AxxPxx" && test "$with_AxxPxx" != no && AxxPxx_PREFIX="$with_AxxPxx"
AC_MSG_RESULT($AxxPxx_PREFIX)
if test "$AxxPxx_PREFIX" ; then
  # Note that the prefix appendages are added to AxxPxx_PREFIX to find A++ and P++.
  AC_MSG_RESULT(using $AxxPxx_PREFIX as path to A++ Library)
  Axx_INCLUDES="-I$AxxPxx_PREFIX/A++/lib/include"
  Axx_LIBS="-L$AxxPxx_PREFIX/A++/lib/lib -lApp -lApp_static -lApp"
  Pxx_INCLUDES="-I$AxxPxx_PREFIX/P++/lib/include"
  Pxx_LIBS="-L$AxxPxx_PREFIX/P++/lib/lib -lApp -lApp_static -lApp"
  # optional_AxxPxxSpecificExample_subdirs="EXAMPLES"
  # we will want to setup subdirectories in the TESTS directory later so set it up now
  # optional_AxxPxxSpecificTest_subdirs="A++Tests"
else
  AC_MSG_RESULT(No path specified for A++ Library)
fi
AC_SUBST(Axx_INCLUDES)
AC_SUBST(Axx_LIBS)
AC_SUBST(Pxx_INCLUDES)
AC_SUBST(Pxx_LIBS)
# AC_SUBST(optional_AxxPxxSpecificExample_subdirs)
# AC_SUBST(optional_AxxPxxSpecificTest_subdirs)
# Do not append to INCLUDES and LIBS because Axx is not needed everywhere.
# It is only needed in EXAMPLES.
# Set up A++/P++ directories that require A++/P++ Libraries (EXAMPLES)
AM_CONDITIONAL(AXXPXX_SPECIFIC_TESTS,test ! "$with_AxxPxx" = no)

# BTNG_CHOOSE_STL defines STL_DIR and STL_INCLUDES
# BTNG_CHOOSE_STL
# echo "STL_INCLUDE = $STL_INCLUDE"
# AC _SUB ST(STL_INCLUDES)
# AC _SUB ST(STL_DIR)

# We no longer want to have the ROSE configure.in setup the PerformanceTests/Makefile
# PerformanceTests/Makefile
AC_ARG_WITH(PERFORMANCE_TESTS,
   [  --with-PERFORMANCE_TESTS ... compile and run performance tests within both A++ and P++],, with_PERFORMANCE_TESTS=no )
# BTNG_AC_LOG(with_PERFORMANCE_TESTS is $with_PERFORMANCE_TESTS)
# with_PERFORMANCE_TESTS variable is exported so that other packages
# (e.g. indirect addressing) can set 
# themselves up dependent upon the use/non-use of PADRE
export with_PERFORMANCE_TESTS;

# Inclusion of PerformanceTests and/or its sublibraries.
# if test "$with_PERFORMANCE_TESTS" = no; then
#   # If PerformanceTests is not specified, then don't use it.
#     echo "Skipping PerformanceTests!"
# else
#   # If PERFORMANCE_TESTS is specified, then configure in PERFORMANCE_TESTS
#   # without regard to its sublibraries.
#   # subdir_list="BenchmarkBase $subdir_list"
#   # optional_PERFORMANCE_subdirs="TESTS/PerformanceTests/BenchmarkBase"
#   # optional_PERFORMANCE_subdirs="TESTS/PerformanceTests"
#   optional_PERFORMANCE_subdirs="PerformanceTests"
#   # echo "Setup PerformanceTests! optional_PERFORMANCE_subdirs = $optional_PERFORMANCE_subdirs"
#   AC_CONFIG_SUBDIRS(TESTS/PerformanceTests/BenchmarkBase)
# fi

dnl # PC (8/16/2006): Now we test for GCJ since MOPS uses it
dnl AC_ARG_WITH([gcj],
dnl [  --with-gcj .................. Specify use of Java (gcj must be in path, required for use with ROSE/projects/FiniteStateModelChecker which uses MOPS internally)], [
dnl    AM_PROG_GCJ
dnl    echo "GCJ = '$GCJ'"
dnl    if test "x$GCJ" == "x" ; then
dnl      echo "gcj not found in path; please add gcj to path or omit --with-gcj option"
dnl      exit 1
dnl    fi
dnl    with_gcj=yes
dnl ],[
dnl    _AM_IF_OPTION([no-dependencies],, [_AM_DEPENDENCIES(GCJ)])
dnl ]) 
with_gcj=no ; # JJW 5-22-2008 The code that was here before broke if gcj was not present, even if the --with-gcj flag was absent
AM_CONDITIONAL(USE_GCJ,test "$with_gcj" = yes)

AC_SEARCH_LIBS(clock_gettime, [rt], [
  RT_LIBS="$LIBS"
  LIBS=""
],[
  RT_LIBS=""
])
AC_SUBST(RT_LIBS)

# DQ (9/11/2006): Removed performance tests conditional, the performance tests were
# removed previously, but we still have the tests/PerformanceTests directory.
# AM_CONDITIONAL(ROSE_PERFORMANCE_TESTS,test ! "$with_PERFORMANCE_TESTS" = no)

# DQ (9/11/2006): skipping use of optional_PERFORMANCE_subdirs
# There is no configure.in in TESTS/PerformanceTests (only in TESTS/PerformanceTests/BenchmarkBase)
# AC_CONFIG_SUBDIRS(TESTS/PerformanceTests)
# AC_CONFIG_SUBDIRS(TESTS/PerformanceTests/BenchmarkBase)
# AC_SUBST(optional_PERFORMANCE_subdirs)

# Set up for Dan Quinlan's development test directory.
AC_ARG_ENABLE(dq-developer-tests,
[--enable-dq-developer-tests   Development option for Dan Quinlan (disregard).],
[ echo "Setting up optional ROSE/developersScratchSpace/Dan directory"
if test -d ${srcdir}/developersScratchSpace; then
  :
else
  echo "This is a non-developer version of ROSE (source distributed with EDG binary)"
  enable_dq_developer_tests=no
fi
])
AM_CONDITIONAL(DQ_DEVELOPER_TESTS,test "$enable_dq_developer_tests" = yes)

AC_DEFINE([HAVE_EXPLICIT_TEMPLATE_INSTANTIATION],[],[Use explicit template instantiation.])

# Copied from the P++/configure.in
# Determine how to build a C++ library.
AC_MSG_CHECKING(how to build C++ libraries)
BTNG_CXX_AR
if test "$CXX_ID" = ibm; then
  # IBM does not have a method for supporting shared libraries
  # Here is a kludge.
  CXX_SHARED_LIB_UPDATE="`cd ${srcdir}/../config && pwd`/mklib.aix -o"
  BTNG_AC_LOG(CXX_SHARED_LIB_UPDATE changed to $CXX_SHARED_LIB_UPDATE especially for the IBM)
fi
AC_MSG_RESULT($CXX_STATIC_LIB_UPDATE and $CXX_SHARED_LIB_UPDATE)
AC_SUBST(CXX_STATIC_LIB_UPDATE)
AC_SUBST(CXX_SHARED_LIB_UPDATE)

# The STL tests use the CC command line which specifies -ptr$(CXX_TEMPLATE_REPOSITORY_PATH) but this
# is not defined in the shell so no substitution is done and a directory named
# $(CXX_TEMPLATE_REPOSITORY_PATH) is built in the top level directory.  The least we can do is
# delete it if we can't stop it from being generated.
# AC_MSG_RESULT(deleting temporary template directory built during STL tests.)
# rm -rf '$(CXX_TEMPLATE_REPOSITORY_PATH)'
rm -rf Templates.DB

SETUP_EDG

# Setup Automake conditional in Projects/DataBase/Makefile.am
# AM_CONDITIONAL(ROSE_USE_EDG_3_3,test "$with_EDG_3_3" = yes)

# Find md5 or md5sum and create a signature for ROSE binary compatibility
AC_CHECK_PROGS(MD5, [md5 md5sum], [false])
AC_SUBST(MD5)
if test -e ${srcdir}/src/frontend/CxxFrontend/EDG/EDG_SAGE_Connection; then
  has_edg_source=yes
  if test "x$MD5" = "xfalse"; then
    AC_MSG_WARN([Could not find either md5 or md5sum -- building binary EDG tarballs is disabled])
    binary_edg_tarball_enabled=no
  else
    binary_edg_tarball_enabled=yes
  fi
else
  has_edg_source=no
  binary_edg_tarball_enabled=no # This is a binary release version of ROSE anyway
fi

AM_CONDITIONAL(ROSE_HAS_EDG_SOURCE, [test "x$has_edg_source" = "xyes"])
AM_CONDITIONAL(BINARY_EDG_TARBALL_ENABLED, [test "x$binary_edg_tarball_enabled" = "xyes"])

#The build_triplet_without_redhat variable is used only in src/frontend/CxxFrontend/Makefile.am to determine the binary edg name
build_triplet_without_redhat=`${srcdir}/config/cleanConfigGuessOutput "$build" "$build_cpu" "$build_vendor"`
AC_SUBST(build_triplet_without_redhat) dnl This is done even with EDG source, since it is used to determine the binary to make in roseFreshTest

dnl ---------------------------------------------------------------------
dnl (8/29/2007): This was added to provide more portable times upon the 
dnl suggestion of Matt Sottile at LANL.
dnl ---------------------------------------------------------------------
AC_C_INLINE
AC_HEADER_TIME
AC_CHECK_HEADERS([sys/time.h c_asm.h intrinsics.h mach/mach_time.h])

AC_CHECK_TYPE([hrtime_t],[AC_DEFINE(HAVE_HRTIME_T, 1, [Define to 1 if hrtime_t is defined in <sys/time.h>])],,[#if HAVE_SYS_TIME_H 
#include <sys/time.h> 
#endif])

AC_CHECK_FUNCS([gethrtime read_real_time time_base_to_time clock_gettime mach_absolute_time])

dnl Cray UNICOS _rtc() (real-time clock) intrinsic
AC_MSG_CHECKING([for _rtc intrinsic])
rtc_ok=yes
AC_TRY_LINK([#ifdef HAVE_INTRINSICS_H
#include <intrinsics.h>
#endif], [_rtc()], [AC_DEFINE(HAVE__RTC,1,[Define if you have the UNICOS _rtc() intrinsic.])], [rtc_ok=no])
AC_MSG_RESULT($rtc_ok)
dnl ---------------------------------------------------------------------


# Record the location of the build tree (so it can be substituted into ROSE/docs/Rose/rose.cfg)
top_pwd=$PWD
AC_SUBST(top_pwd)
# echo "In ROSE/con figure: top_pwd = $top_pwd"

absolute_path_srcdir="`cd $srcdir; pwd`"
AC_SUBST(absolute_path_srcdir)

# This is silly, but it is done to hide an include command (in
# projects/compass/Makefile.am, including compass-makefile.inc in the build
# tree) from Automake because the needed include file does not exist when
# automake is run
INCLUDE_COMPASS_MAKEFILE_INC="include compass_makefile.inc"
AC_SUBST(INCLUDE_COMPASS_MAKEFILE_INC)

# ROSE-HPCT module -- note that this needs the XML check to have already
# happened
ACROSE_ENABLE_ROSEHPCT

# PC (08/20/2009): Symbolic links need to be resolved for the callgraph analysis tests
res_top_pwd=$(cd "$top_pwd" && pwd -P)

# DQ (11/10/2007): Add paths defined by automake to the generated rose.h.in and rose.h
# header files so that this information will be available at compile time. Unclear
# which syntax is best for the specification of these paths.
AC_DEFINE_UNQUOTED([ROSE_COMPILE_TREE_PATH],"$res_top_pwd",[Location of ROSE Compile Tree.])

# This block turns off features of libharu that don't work with Java
with_png=no
export with_png
with_zlib=no
export with_zlib

# GMY (9/3/2008) QT4 & QROSE Optional Packages
AC_ARG_WITH(QRose,
	[--with-QRose=PATH	prefix of QRose installation],
	[QROSE_PREFIX=$with_QRose],
	[with_QRose=no])

AC_SUBST(QROSE_PREFIX)
AM_CONDITIONAL(ROSE_USE_QROSE,test "$with_QRose" != no)

#AM_CONDITIONAL(USE_QROSE, test "$with_QRose" != no)
#QROSE_LDFLAGS="-L${QROSE_PREFIX}/lib -lqrose"
#QROSE_CXXFLAGS="-I${QROSE_PREFIX}/include"
#AC_SUBST(QROSE_LDFLAGS)
#AC_SUBST(QROSE_CXXFLAGS)

echo "with_QRose = $with_QRose"

AC_PATH_QT
AC_PATH_QT_MOC
AC_PATH_QT_RCC
AC_PATH_QT_UIC
AC_PATH_QT_VERSION

# echo "with_qt = $with_qt"

AM_CONDITIONAL(ROSE_USE_QT,test x"$with_qt" != x"no")
AM_CONDITIONAL(ROSE_WITH_ROSEQT,test x"$with_roseQt" != x"no")

AC_ARG_ENABLE(assembly-semantics, AS_HELP_STRING([--enable-assembly-semantics], [Enable semantics-based analysis of assembly code]))
AM_CONDITIONAL(ROSE_USE_ASSEMBLY_SEMANTICS, [test "x$enable_assembly_semantics" = xyes])

# Xen and Ether [RPM 2009-10-28]
AC_ARG_WITH(ether,
        [  --with-ether=PATH   prefix of Xen/Ether installation
                      Xen is a hypervisor for running virtual machines (http://www.xen.org)
                      Ether is a layer on top of Xen for accessing Windows XP OS-level data
                      structures (http://ether.gtisc.gatech.edu)],
        [AC_DEFINE(ROSE_USE_ETHER, 1, [Defined if Ether from Georgia Tech is available.])
	 if test "$with_ether" = "yes"; then ETHER_PREFIX=/usr; else ETHER_PREFIX="$with_ether"; fi],
        [with_ether=no])
AC_SUBST(ETHER_PREFIX)
AM_CONDITIONAL(ROSE_USE_ETHER,test "$with_ether" != "no")


# PC (7/10/2009): The Haskell build system expects a fully numeric version number.
PACKAGE_VERSION_NUMERIC=`echo $PACKAGE_VERSION | sed -e 's/\([[a-z]]\+\)/\.\1/; y/a-i/1-9/'`
AC_SUBST(PACKAGE_VERSION_NUMERIC)

# DQ (9/21/2009): Debugging for RH release 5
echo "Testing the value of CC: (CC = $CC)"
echo "Testing the value of CPPFLAGS: (CPPFLAGS = $CPPFLAGS)"

echo "subdirs $subdirs"
AC_CONFIG_SUBDIRS([libltdl src/3rdPartyLibraries/libharu-2.1.0])

# End macro ROSE_SUPPORT_ROSE_PART_2.
]
)


AC_DEFUN([ROSE_SUPPORT_RTED],
[
# Begin macro ROSE_SUPPORT_RTED.

AC_MSG_CHECKING(for rted)
AC_ARG_WITH(rted,
[--with-rted=PATH              Configure option to have RTED enabled.],
,
if test ! "$with_rted" ; then
   with_rted=no
fi
)

echo "In ROSE SUPPORT MACRO: with_rted $with_rted"

if test "$with_rted" = no; then
   # If dwarf is not specified, then don't use it.
   echo "Skipping use of RTED support!"
else
   rted_path=$with_rted
   echo "Setup RTED support in ROSE! path = $rted_path"
   AC_DEFINE([USE_ROSE_RTED_SUPPORT],1,[Controls use of ROSE support for RTED library.])
fi

AC_SUBST(rted_path)

]
)

AC_DEFUN([ROSE_SUPPORT_SSL],
[
# Begin macro ROSE_SUPPORT_SSL.

# Adding support for the SSL library to provide access to MD5 checksum support for binary library identification.

AC_MSG_CHECKING(for ssl)
AC_ARG_ENABLE(ssl,
[  --enable-ssl ................................ Enable use of SSL library (MD5 checksums)],
,
if test ! "$enable_ssl" ; then
   enable_ssl=no
fi
)

#if test "$enable_ssl" = yes ; then
#  AC_DEFINE([USE_ROSE_SSL_SUPPORT],[],[Support for ssl])
#fi

echo "In ROSE SSL SUPPORT MACRO: enable_ssl = $enable_ssl"

if test "$enable_ssl" = no; then
   # If SSL is not specified, then don't use it.
   echo "Skipping use of SSL (libssl) support!"
else
   echo "Setup SSL support in ROSE! path = $enable_ssl"
   AC_DEFINE([USE_ROSE_SSL_SUPPORT],1,[Controls use of ROSE support for SSL (libssl -- MD5) library (incompatable with Java, so disables Fortran support).])
fi

# AC_SUBST(dwarf_path)

# End macro ROSE_SUPPORT_DWARF.
]
)

AC_DEFUN([ROSE_SUPPORT_VISUALIZATION],
[
# Begin macro ROSE_SUPPORT_VISUALIZATION.

# Inclusion of test for FLTK and GraphViz in support of the visualization 
# provided by dotgl (Nils' work)
#  1) FLTK location path (include and libs path)
#  2) GraphViz location path (include and libs path)

# Note that due to the way in which these libraries are installed in Nils
# directory we have to specify the include and libs paths separately for
# each library.  I don't know if this mechanism has be always be this general.

AC_MSG_CHECKING(for FLTK library include path)
AC_ARG_WITH(FLTK_include,
[  --with-FLTK_include=PATH	Specify the prefix where FLTK include files is installed],
,
if test ! "$with_FLTK_include" ; then
   with_FLTK_include=no
fi
)

AC_MSG_CHECKING(for FLTK library libs path)
AC_ARG_WITH(FLTK_libs,
[  --with-FLTK_libs=PATH	Specify the prefix where FLTK libraries are installed],
,
if test ! "$with_FLTK_libs" ; then
   with_FLTK_libs=no
fi
)

AC_MSG_CHECKING(for GraphViz library include path)
AC_ARG_WITH(GraphViz_include,
[  --with-GraphViz_include=PATH	Specify the prefix where GraphViz include files are installed],
,
if test ! "$with_GraphViz_include" ; then
   with_GraphViz_include=no
fi
)

AC_MSG_CHECKING(for GraphViz library libs path)
AC_ARG_WITH(GraphViz_libs,
[  --with-GraphViz_libs=PATH	Specify the prefix where GraphViz libraries are installed],
,
if test ! "$with_GraphViz_libs" ; then
   with_GraphViz_libs=no
fi
)

# DQ (9/1/2005): Added support for GLUT library (FreeGlut)
AC_MSG_CHECKING(for GLUT library include path)
AC_ARG_WITH(GLUT_include,
[  --with-GLUT_include=PATH	Specify the prefix where GLUT include files are installed])

if test ! "$with_GLUT_include" ; then
   with_GLUT_include=no
fi


# DQ (9/1/2005): Added support for GLUT library (FreeGlut)
AC_MSG_CHECKING(for GLUT library libs path)
AC_ARG_WITH(GLUT_libs,
[  --with-GLUT_libs=PATH	Specify the prefix where GLUT libraries are installed])

if test ! "$with_GLUT_libs" ; then
   with_GLUT_libs=no
fi

echo "In ROSE SUPPORT MACRO: with_FLTK_include     $with_FLTK_include"
echo "In ROSE SUPPORT MACRO: with_FLTK_libs        $with_FLTK_libs"
echo "In ROSE SUPPORT MACRO: with_GraphViz_include $with_GraphViz_include"
echo "In ROSE SUPPORT MACRO: with_GraphViz_libs    $with_GraphViz_libs"

if (test "$with_FLTK_include" = no) || (test "$with_FLTK_libs" = no) || 
   (test "$with_GraphViz_include" = no) || (test "$with_GraphViz_libs" = no) ||
   (test "$with_GLUT_include" = no) || (test "$with_GLUT_libs" = no); then
   # If none of these are specified, then don't use visualization (don't build dotGL).
   echo "Skipping use of dotgl Visualization!"
else
 # optional_Visualization_subdirs="visualization"
   FLTK_include_path=$with_FLTK_include
   FLTK_libs_path=$with_FLTK_libs
   GraphViz_include_path=$with_GraphViz_include
   GraphViz_libs_path=$with_GraphViz_libs
   GLUT_include_path=$with_GLUT_include
   GLUT_libs_path=$with_GLUT_libs

   echo "Setup VIZUALIZATION! FLTK include path     = $FLTK_include_path"
   echo "Setup VIZUALIZATION! FLTK libs path        = $FLTK_libs_path"
   echo "Setup VIZUALIZATION! GraphViz include path = $GraphViz_include_path"
   echo "Setup VIZUALIZATION! GraphViz libs path    = $GraphViz_libs_path"
   echo "Setup VIZUALIZATION! GLUT include path     = $GLUT_include_path"
   echo "Setup VIZUALIZATION! GLUT libs path        = $GLUT_libs_path"

 # echo "Setup VIZUALIZATION! optional_Visualization_subdirs = $optional_Visualization_subdirs"

#  AC_CONFIG_SUBDIRS(src/roseIndependentSupport/visualization)
fi

# These are all used to buld dotGL
AC_SUBST(optional_Visualization_subdirs)
AC_SUBST(FLTK_include_path)
AC_SUBST(FLTK_libs_path)
AC_SUBST(GraphViz_include_path)
AC_SUBST(GraphViz_libs_path)
AC_SUBST(GLUT_include_path)
AC_SUBST(GLUT_libs_path)

# End macro ROSE_SUPPORT_VISUALIZATION.
]
)





AC_DEFUN([ROSE_SUPPORT_WINDOWS_ANALYSIS],
[
# Begin macro ROSE_SUPPORT_WINDOWS_ANALYSIS.

AC_MSG_CHECKING(for Wine -- Wine is not an emulator)
AC_ARG_WITH(wine,
[  --with-wine=PATH	Specify the prefix where Wine is installed],
,
if test ! "$with_wine" ; then
   with_wine=no
fi
)

echo "In ROSE SUPPORT MACRO: with_wine $with_wine"

if test "$with_wine" = no; then
   # If wine is not specified, then don't use it.
   echo "Skipping use of Wine for Windows Binary Analysis!"
else
   wine_path=$with_wine
   echo "Setup Wine support in ROSE! path = $wine_path"
   AC_DEFINE([USE_ROSE_WINDOWS_ANALYSIS_SUPPORT],1,[Wine provides access to Windows header files for analysis of windows applications under Linux.])
fi

AC_SUBST(wine_path)

# End macro ROSE_SUPPORT_WINDOWS_BINARY_ANALYSIS.
]
)

AC_DEFUN([ROSE_SUPPORT_YICES],
[
# Begin macro ROSE_SUPPORT_YICES.

# Inclusion of test for YICES SMT Solver Package and it's location.

AC_MSG_CHECKING(for yices)
AC_ARG_WITH(yices,
[  --with-yices=PATH	Specify the prefix where Yices SMT solver is installed],
,
if test ! "$with_yices" ; then
   with_yices=no
fi
)

echo "In ROSE SUPPORT MACRO: with_yices $with_yices"

if test "$with_yices" = no; then
   # If yices is not specified, then don't use it.
   echo "Skipping use of Yices SMT Solver support!"
else
   yices_path=$with_yices
   echo "Setup Yices support in ROSE! path = $yices_path"
   AC_DEFINE([USE_ROSE_YICES_SUPPORT],1,[Controls use of ROSE support for Yices Satisfiability Modulo Theories (SMT) Solver Package.])
fi

AC_SUBST(yices_path)

# End macro ROSE_SUPPORT_YICES.
]
)

#
# SYNOPSIS
#
#   SWIG_ENABLE_CXX
#
# DESCRIPTION
#
#   Enable SWIG C++ support. This affects all invocations of $(SWIG).
#
# LAST MODIFICATION
#
#   2006-10-22
#
# COPYLEFT
#
#   Copyright (c) 2006 Sebastian Huber <sebastian-huber@web.de>
#   Copyright (c) 2006 Alan W. Irwin <irwin@beluga.phys.uvic.ca>
#   Copyright (c) 2006 Rafael Laboissiere <rafael@laboissiere.net>
#   Copyright (c) 2006 Andrew Collier <colliera@ukzn.ac.za>
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 2 of the
#   License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
#   02111-1307, USA.
#
#   As a special exception, the respective Autoconf Macro's copyright
#   owner gives unlimited permission to copy, distribute and modify the
#   configure scripts that are the output of Autoconf when processing
#   the Macro. You need not follow the terms of the GNU General Public
#   License when using or distributing such scripts, even though
#   portions of the text of the Macro appear in them. The GNU General
#   Public License (GPL) does govern all other use of the material that
#   constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the
#   Autoconf Macro released by the Autoconf Macro Archive. When you
#   make and distribute a modified version of the Autoconf Macro, you
#   may extend this special exception to the GPL to apply to your
#   modified version as well.

AC_DEFUN([SWIG_ENABLE_CXX],[
        AC_REQUIRE([AC_PROG_SWIG])
        AC_REQUIRE([AC_PROG_CXX])
        SWIG="$SWIG -c++"
])

