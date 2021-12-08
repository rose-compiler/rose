dnl
dnl ROSE_FLAG_CXX set the C++ compiler flags.
dnl This macro modifies CXXFLAGS.  Secondarily,
dnl it defines variables such as CXX_DEBUG, CXX_OPTIMIZE, etc.
dnl
AC_DEFUN([ROSE_FLAG_OPTIONS], [

AC_REQUIRE([AC_CANONICAL_HOST])
AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([AC_PROG_CXX])

# Given the C++ compiler command-line, create output variables HOST_CXX_VENDOR, HOST_CXX_VERSION, and HOST_CXX_LANGUAGE
# that contain the vendor (gnu, llvm, or intel), the version (as defined by CPP macros, not the --version output), and
# the language dialect (c++17, gnu++11, etc.).
ROSE_COMPILER_FEATURES([c++], [$CXX $CPPFLAGS $CXXFLAGS], [HOST_CXX_])

# Debug Flags

AC_ARG_WITH(DEBUG, AS_HELP_STRING([--with-DEBUG], [manually set the compiler debug flags]))

has_debug_flag=yes
if test "$with_DEBUG" = "yes"; then
  C_DEBUG="-g"
  CXX_DEBUG="-g"
elif test "$with_DEBUG" = "no"; then
  C_DEBUG=""
  CXX_DEBUG=""
elif test "$with_DEBUG"; then
  C_DEBUG="$with_DEBUG"
  CXX_DEBUG="$with_DEBUG"
else
  has_debug_flag=no
  C_DEBUG=""
  CXX_DEBUG=""
fi

AC_SUBST(C_DEBUG)
AC_SUBST(CXX_DEBUG)

# Optimization Flags

AC_ARG_WITH(OPTIMIZE, AS_HELP_STRING([--with-OPTIMIZE], [manually set the compiler optimization flags]))

has_optimize_flag=yes
if test "$with_OPTIMIZE" = yes; then
  C_OPTIMIZE="-O2"
  CXX_OPTIMIZE="-O2"
elif test "$with_OPTIMIZE" = debug; then
  C_OPTIMIZE="-Og"
  CXX_OPTIMIZE="-Og"
elif test "$with_OPTIMIZE" = no; then
  C_OPTIMIZE="-O0"
  CXX_OPTIMIZE="-O0"
elif test "$with_OPTIMIZE"; then
  C_OPTIMIZE=$with_OPTIMIZE
  CXX_OPTIMIZE=$with_OPTIMIZE
else
  has_optimize_flag=no
  C_OPTIMIZE="-O2"
  CXX_OPTIMIZE="-O2"
fi

AC_SUBST(C_OPTIMIZE)
AC_SUBST(CXX_OPTIMIZE)

# Warning Flags

AC_ARG_WITH(WARNINGS, AS_HELP_STRING([--with-WARNINGS], [manually set the compiler warning flags]))

# Large set of warnings
rose_basic_warnings="-Wall -Wextra"
# Even more warnings
rose_extra_warnings="-Wshadow -Wdouble-promotion -Wformat=2 -Wformat-truncation -Wundef -Wconversion"
# Some warnings that cannot be made errors in ROSE (last 3 should get fixed in EDG -- probably with pragma)
rose_no_error_warnings="-Wno-error=cpp -Wno-error=multichar -Wno-error=shift-count-overflow -Wno-error=stringop-overflow="

# Evolving list of warnings that we want to fix in ROSE
#warnings_of_interrest="uninitialized maybe-uninitialized format format-overflow format-truncation"
warnings_of_interrest="uninitialized"
warnings_causing_failure=""
develop_warn_on="$(for w in $warnings_of_interrest; do echo -W$w; done)"
release_fail_on="$(for w in $warnings_of_interrest; do echo -Werror=$w; done)"
develop_fail_on="$(for w in $warnings_causing_failure; do echo -Werror=$w; done)"

has_warning_flag=yes
if test "$with_WARNINGS" = none; then
  # Ensure no warnings
  C_WARNINGS=""
  CXX_WARNINGS=""
elif test "$with_WARNINGS" = basic; then
  # Basic warnings used by most projects
  C_WARNINGS="$rose_basic_warnings"
  CXX_WARNINGS="$rose_basic_warnings"
elif test "$with_WARNING" = all; then
  # All warnings that we could think of
  C_WARNINGS="$rose_basic_warnings $rose_extra_warnings"
  CXX_WARNINGS="$rose_basic_warnings $rose_extra_warnings"
elif test "$with_WARNINGS" = develop; then
  # Only warnings that we want to enforce in `develop` mode
  C_WARNINGS="$develop_warn_on $develop_fail_on"
  CXX_WARNINGS="$develop_warn_on $develop_fail_on"
elif test "$with_WARNINGS" = candidate; then
  # All warnings plus failure on important warnings
  C_WARNINGS="$develop_fail_on $release_fail_on"
  CXX_WARNINGS="$develop_fail_on $release_fail_on"
elif test "$with_WARNINGS" = release; then
  # All warnings plus failure on important warnings
  C_WARNINGS="$rose_basic_warnings $rose_extra_warnings $develop_fail_on $release_fail_on"
  CXX_WARNINGS="$rose_basic_warnings $rose_extra_warnings $develop_fail_on $release_fail_on"
elif test "$with_WARNING" = pedantic; then
  # Fails on any warnings (of `all`) but a few special ones
  C_WARNINGS="-Werror $rose_basic_warnings $rose_extra_warnings $rose_no_error_warnings"
  CXX_WARNINGS="-Werror $rose_basic_warnings $rose_extra_warnings $rose_no_error_warnings"
elif test "$with_WARNINGS"; then
  # User defined
  C_WARNINGS=$with_WARNINGS
  CXX_WARNINGS=$with_WARNINGS
else
  has_warning_flag=no
  C_WARNINGS=""
  CXX_WARNINGS=""
fi

AC_SUBST(C_WARNINGS)
AC_SUBST(CXX_WARNINGS)

CXX_TEMPLATE_REPOSITORY_PATH='$(top_builddir)/src'
CXX_TEMPLATE_OBJECTS= # A bunch of Makefile.ams use this
AC_SUBST(CXX_TEMPLATE_REPOSITORY_PATH)
AC_SUBST(CXX_TEMPLATE_OBJECTS)

AC_MSG_NOTICE([Initial flags:])
AC_MSG_NOTICE([CFLAGS       = "$CFLAGS"])
AC_MSG_NOTICE([CXXFLAGS     = "$CXXFLAGS"])

AC_MSG_NOTICE([Found Flags:])
AC_MSG_NOTICE([C_DEBUG      = "$C_DEBUG"])
AC_MSG_NOTICE([CXX_DEBUG    = "$CXX_DEBUG"])
AC_MSG_NOTICE([C_OPTIMIZE   = "$C_OPTIMIZE"])
AC_MSG_NOTICE([CXX_OPTIMIZE = "$CXX_OPTIMIZE"])
AC_MSG_NOTICE([C_WARNINGS   = "$C_WARNINGS"])
AC_MSG_NOTICE([CXX_WARNINGS = "$CXX_WARNINGS"])

if test "$has_debug_flag" = yes; then
  CFLAGS="$CFLAGS $C_DEBUG"
  CXXFLAGS="$CXXFLAGS $CXX_DEBUG"
fi
if test "$has_optimize_flag" = yes; then
  CFLAGS="$CFLAGS $C_OPTIMIZE"
  CXXFLAGS="$CXXFLAGS $CXX_OPTIMIZE"
fi
if test "$has_warning_flag" = yes; then
  CFLAGS="$CFLAGS $C_WARNINGS"
  CXXFLAGS="$CXXFLAGS $CXX_WARNINGS"
fi

AC_MSG_NOTICE([Final Flags:])
AC_MSG_NOTICE([CFLAGS       = "$CFLAGS"])
AC_MSG_NOTICE([CXXFLAGS     = "$CXXFLAGS"])

])

AC_DEFUN([ROSE_FLAG_CXX_OPTIONS], [
# Begin macro ROSE_FLAG_CXX.

# We may be asked to guess the correct flags,
# so we must know our host and the compiler used.
AC_REQUIRE([AC_CANONICAL_HOST])
AC_REQUIRE([AC_PROG_CXX])

# Given the C++ compiler command-line, create output variables HOST_CXX_VENDOR, HOST_CXX_VERSION, and HOST_CXX_LANGUAGE
# that contain the vendor (gnu, llvm, or intel), the version (as defined by CPP macros, not the --version output), and
# the language dialect (c++17, gnu++11, etc.).
ROSE_COMPILER_FEATURES([c++], [$CXX $CPPFLAGS $CXXFLAGS], [HOST_CXX_])

AC_MSG_NOTICE([in c++ option setting: FRONTEND_CXX_COMPILER_VENDOR = "$FRONTEND_CXX_COMPILER_VENDOR"])

dnl *********************************************************************
dnl * Set up the Preprocessor -D options CXXDEBUG and ARCH_DEFINES
dnl *********************************************************************
AC_ARG_WITH(CXX_DEBUG, AS_HELP_STRING([--with-CXX_DEBUG], [manually set the C++ compiler debug flags to ARG (leave blank to choose automatically)]),[withval=yes],[withval=yes])

AC_MSG_NOTICE([with_CXX_DEBUG = "$with_CXX_DEBUG"])

if test "x$with_CXX_DEBUG" = "x"; then
   if test "x$withval" = "xyes"; then
      with_CXX_DEBUG=$withval
   else 
      if test "x$withval" = "xno"; then
         with_CXX_DEBUG=$withval
      fi
   fi
else
   AC_MSG_NOTICE([with_CXX_DEBUG is explictly set to "$with_CXX_DEBUG"])
fi

AC_MSG_NOTICE([after initialization: with_CXX_DEBUG = "$with_CXX_DEBUG"])

if test "x$FRONTEND_CXX_COMPILER_VENDOR" = "xgnu" ; then
   AC_MSG_NOTICE([using additional GNU compiler options: -fno-var-tracking-assignments -Wno-misleading-indentation])
else
   AC_MSG_NOTICE([skip using additional GNU compiler options: -fno-var-tracking-assignments -Wno-misleading-indentation])
fi

if test "x$with_CXX_DEBUG" = "xyes"; then
# CXX_DEBUG was activated but not specified, so set it.
  AC_MSG_NOTICE([using default options for maximal debug (true case)])
  case $CXX in
    g++)
      CXX_DEBUG="-g"
      ;;
    clang++)
      CXX_DEBUG="-g"
      ;;
    icpc)
      CXX_DEBUG="-g"
      ;;
    KCC | mpKCC)
      CXX_DEBUG="-g"
      ;;
    CC)
      case $host_os in
        solaris*| sun4*)
          CXX_DEBUG="-g"
    esac
    ;;
  esac
elif test "x$with_CXX_DEBUG" = "xno"; then
  CXX_DEBUG=''
# DQ (10/22/2010): turn on debug flags by default.
  AC_MSG_NOTICE([using at least some default (minimal) options for debug flags (currently the same as above) (false case)])
  case $CXX in
    g++)
      CXX_DEBUG=""
      ;;
    clang++)
      CXX_DEBUG=""
      ;;
    icpc)
      CXX_DEBUG=""
      ;;
    "KCC --c" | mpKCC)
      CXX_DEBUG=""
      ;;
    CC)
      case $host_os in
        solaris*| sun4*)
          CXX_DEBUG=""
    esac
    ;;
  esac
else
# Settings specified explicitly by the user.
  AC_MSG_NOTICE([using user provided options for CXX_DEBUG])
  CXX_DEBUG=$with_CXX_DEBUG
fi

AC_SUBST(CXX_DEBUG)
if test -n "$CXX_DEBUG"; then CXXFLAGS="$CXXFLAGS $CXX_DEBUG"; fi

# echo "Am I set: CXX_DEBUG= $CXX_DEBUG"
AC_MSG_NOTICE([CXXFLAGS currently set to $CXXFLAGS])

dnl *********************************************************************
dnl * Set the C++ compiler optimization flags in CXXOPT
dnl *********************************************************************
dnl This should use the AC_ARG_ENABLE not AC_ARC_WITH!

AC_MSG_NOTICE([setup CXX_OPTIMIZE])

AC_ARG_WITH(CXX_OPTIMIZE, [  --with-CXX_OPTIMIZE=ARG   manually set the C++ compiler optimization
                           to ARG (leave blank to choose automatically)])
if test "$with_CXX_OPTIMIZE" = yes; then
# CXX_OPTIMIZE was activated but not specified, so set it.
  case $CXX in
    KCC | mpKCC) CXX_OPTIMIZE='-g +K0 --no_exceptions --no_rtti --keep_gen_c' ;;
    *) CXX_OPTIMIZE='' ;;
  esac
elif test "$with_CXX_OPTIMIZE" = no; then
# use defaults for any compiler compiling ROSE (for g++ this means no optimization).
  CXX_OPTIMIZE=''
# else
elif test "$with_CXX_OPTIMIZE"; then
  CXX_OPTIMIZE=$with_CXX_OPTIMIZE
else
# DQ (6/30/2013): Select optimization level -O2 to be the default for ROSE distributions.
  CXX_OPTIMIZE='-O2'
fi
AC_SUBST(CXX_OPTIMIZE)
if test "$CXX_OPTIMIZE"; then CXXFLAGS="$CXXFLAGS $CXX_OPTIMIZE"; fi

dnl *********************************************************************
dnl * Set the C++ compiler flags in CXX_WARNINGS
dnl *********************************************************************
dnl This should use the AC_ARG_ENABLE not AC_ARC_WITH!

AC_MSG_NOTICE([setup CXX_WARNING CXX = "$CXX"])

AC_ARG_WITH(CXX_WARNINGS, AS_HELP_STRING([--with-CXX_WARNINGS], [Support for a uniform warning level for ROSE development]),[withval=yes],[withval=no])

AC_MSG_NOTICE([with_CXX_WARNINGS = "$with_CXX_WARNINGS"])

if test "x$with_CXX_WARNINGS" = "x"; then
   if test "x$withval" = "xyes"; then
      with_CXX_WARNINGS=$withval
   else 
      if test "x$withval" = "xno"; then
         with_CXX_WARNINGS=$withval
      fi
   fi
else
   AC_MSG_NOTICE([with_CXX_WARNINGS is explictly set to "$with_CXX_WARNINGS"])
fi

AC_MSG_NOTICE([after initialization: with_CXX_WARNINGS = "$with_CXX_WARNINGS"])

if test "x$with_CXX_WARNINGS" = "xyes"; then
  AC_MSG_NOTICE([using default options for maximal warnings (true case)])
  case $FRONTEND_CXX_COMPILER_VENDOR in
    gnu)
      CXX_WARNINGS="-Wall"
      ;;
    intel)
      CXX_WARNINGS="-Wall"
      ;;
    clang)
      CXX_WARNINGS="-Wall -Wno-unused-local-typedefs -Wno-attributes"
      ;;
    kai)
      CXX_WARNINGS="--for_init_diff_warning --new_for_init -w"
      ;;
    sun)
      case $host_os in
        solaris*| sun4*)
          CXX_WARNINGS=""
    esac
    ;;
  esac
elif test "x$with_CXX_WARNINGS" = "xno"; then
  AC_MSG_NOTICE([using at least some default (minimal) options for warnings (false case)])
  case $FRONTEND_CXX_COMPILER_VENDOR in
    gnu)
      CXX_WARNINGS=""
      ;;
    intel)
      CXX_WARNINGS=""
      ;;
    clang)
      CXX_WARNINGS=""
      ;;
    kai)
      CXX_WARNINGS=""
      ;;
    sun)
      case $host_os in
        solaris*| sun4*)
          CXX_WARNINGS=""
    esac
    ;;
  esac
else
# Settings specified explicitly by the user.
  AC_MSG_NOTICE([adding explicitly specified warnings to be used for CXX_WARNINGS])
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

## Don't do this here since we have to use the unmodified CXXFLAGS variable for the 
## configure compilation tests (and we want don't want those templates in our repository)
dnl if test "$CXX_TEMPLATES"; then CXXFLAGS="$CXXFLAGS $CXX_TEMPLATES"; fi

AC_MSG_NOTICE([C_DEBUG      = "$C_DEBUG"])
AC_MSG_NOTICE([CXX_DEBUG    = "$CXX_DEBUG"])
AC_MSG_NOTICE([C_WARNINGS   = "$C_WARNINGS"])
AC_MSG_NOTICE([CXX_WARNINGS = "$CXX_WARNINGS"])

# echo "Exiting at the base of ROSE FLAG CXX OPTIONS..."
# exit 1;

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

AC_MSG_NOTICE([in c option setting: FRONTEND_CXX_COMPILER_VENDOR = "$FRONTEND_CXX_COMPILER_VENDOR"])

dnl *********************************************************************
dnl * Set up the C compiler options C_DEBUG
dnl *********************************************************************
# AC_ARG_WITH(C_DEBUG, [  --with-C_DEBUG=ARG    manually set the C compiler debug flags to ARG (leave blank to choose automatically)])
AC_ARG_WITH(C_DEBUG, AS_HELP_STRING([--with-C_DEBUG], [manually set the C compiler debug flags to ARG (leave blank to choose automatically)]),[withval=yes],[withval=yes])

# if test "$with_C_DEBUG" = yes; then
#   # C_DEBUG was activated but not specified, so set it.
#   case $CC in
#     *) C_DEBUG='-g' ;;
#   esac
# elif test "$with_C_DEBUG" = no; then
#   C_DEBUG=''
# else
#   C_DEBUG=$with_C_DEBUG
# fi

AC_MSG_NOTICE([with_C_DEBUG = "$with_C_DEBUG"])

if test "x$with_C_DEBUG" = "x"; then
   if test "x$withval" = "xyes"; then
      with_C_DEBUG=$withval
   else 
      if test "x$withval" = "xno"; then
         with_C_DEBUG=$withval
      fi
   fi
else
   AC_MSG_NOTICE([with_C_DEBUG is explictly set to "$with_C_DEBUG"])
fi

AC_MSG_NOTICE([after initialization: with_C_DEBUG = "$with_C_DEBUG"])

# echo "Setting with_C_DEBUG to withval = $withval"
# with_C_DEBUG=$withval

if test "x$with_C_DEBUG" = "xyes"; then
# C_DEBUG was activated but not specified, so set it.
  AC_MSG_NOTICE([using default options for maximal debug (true case)])
  case $CC in
    gcc)
      C_DEBUG="-g"
      ;;
    clang)
      C_DEBUG="-g"
      ;;
    icc)
      C_DEBUG="-g"
      ;;
    KCC | mpKCC)
      C_DEBUG="-g"
      ;;
    cc)
      case $host_os in
        solaris*| sun4*)
          C_DEBUG="-g"
    esac
    ;;
  esac
elif test "x$with_C_DEBUG" = "xno"; then
  C_DEBUG=''
# DQ (10/22/2010): turn on debug flags by default.
  AC_MSG_NOTICE([using at least some default (minimal) options for debug flags (currently the same as above) (false case)])
  case $CC in
    gcc)
      C_DEBUG=""
      ;;
    clang)
      C_DEBUG=""
      ;;
    icc)
      C_DEBUG=""
      ;;
    "KCC --c" | mpKCC)
      C_DEBUG=""
      ;;
    cc)
      case $host_os in
        solaris*| sun4*)
          C_DEBUG=""
    esac
    ;;
  esac
else
# Settings specified explicitly by the user.
  AC_MSG_NOTICE([using user provided options for C_DEBUG])
  C_DEBUG=$with_C_DEBUG
fi

AC_SUBST(C_DEBUG)
if test -n "$C_DEBUG"; then CFLAGS="$CFLAGS $C_DEBUG"; fi

# echo "Am I set: C_DEBUG= $C_DEBUG"


dnl *********************************************************************
dnl * Set the C compiler optimization flags in C_OPTIMIZE
dnl *********************************************************************
dnl This should use the AC_ARG_ENABLE not AC_ARC_WITH!

AC_MSG_NOTICE([setup C_OPTIMIZE])

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
# else
elif test "$with_C_OPTIMIZE"; then
  C_OPTIMIZE=$with_C_OPTIMIZE
else
# DQ (6/30/2013): Select optimization level -O2 to be the default for ROSE distributions.
  C_OPTIMIZE='-O2'
fi
AC_SUBST(C_OPTIMIZE)
if test "$C_OPTIMIZE"; then CFLAGS="$CFLAGS $C_OPTIMIZE"; fi

AC_MSG_NOTICE([after initialization: with_C_OPTIMIZE = "$with_C_OPTIMIZE"])
AC_MSG_NOTICE([after initialization: C_OPTIMIZE = "$C_OPTIMIZE"])

dnl *********************************************************************
dnl * Set the C++ compiler flags in C_WARNINGS
dnl *********************************************************************
dnl This should use the AC_ARG_ENABLE not AC_ARC_WITH!

AC_MSG_NOTICE([setup C_WARNINGS CC = "$CC"])

AC_ARG_WITH(C_WARNINGS, AS_HELP_STRING([--with-C_WARNINGS], [Support for a uniform warning level for ROSE development]),[withval=yes],[withval=no])

AC_MSG_NOTICE([with_C_WARNINGS = "$with_C_WARNINGS"])

if test "x$with_C_WARNINGS" = "x"; then
   if test "x$withval" = "xyes"; then
      with_C_WARNINGS=$withval
   else 
      if test "x$withval" = "xno"; then
         with_C_WARNINGS=$withval
      fi
   fi
else
   AC_MSG_NOTICE([with_C_WARNINGS is explictly set to "$with_C_WARNINGS"])
fi

AC_MSG_NOTICE([after initialization: with_C_WARNINGS = "$with_C_WARNINGS"])

if test "x$with_C_WARNINGS" = "xyes"; then
  case $FRONTEND_CXX_COMPILER_VENDOR in
    gnu)
      C_WARNINGS="-Wall -Wstrict-prototypes -Wmissing-prototypes"
      ;;
    intel)
      C_WARNINGS="-w -Wall -Wcheck -Wp64"
      ;;
    clang)
      C_WARNINGS="-Wall -Wno-unused-local-typedefs -Wno-attributes"
      ;;
    kai)
      C_WARNINGS="--for_init_diff_warning --new_for_init -w"
      ;;
    sun)
      case $host_os in
        solaris*| sun4*)
          C_WARNINGS=""
    esac
    ;;
  esac
elif test "x$with_C_WARNINGS" = "xno"; then
  case $FRONTEND_CXX_COMPILER_VENDOR in
    gnu)
      C_WARNINGS=""
      ;;
    intel)
      C_WARNINGS=""
      ;;
    clang)
      C_WARNINGS=""
      ;;
    kai)
      C_WARNINGS=""
      ;;
    sun)
      case $host_os in
        solaris*| sun4*)
          C_WARNINGS=""
    esac
    ;;
  esac
else
  AC_MSG_NOTICE([adding explicitly specified warnings to be used.])
  C_WARNINGS=$with_C_WARNINGS
fi

AC_SUBST(C_WARNINGS)

if test "$C_WARNINGS"; then CFLAGS="$CFLAGS $C_WARNINGS"; fi

AC_MSG_NOTICE([C_DEBUG      = "$C_DEBUG"])
AC_MSG_NOTICE([CXX_DEBUG    = "$CXX_DEBUG"])
AC_MSG_NOTICE([C_WARNINGS   = "$C_WARNINGS"])
AC_MSG_NOTICE([CXX_WARNINGS = "$CXX_WARNINGS"])

# echo "Exiting at the base of ROSE FLAG C OPTIONS..."
# exit 1;

# End macro ROSE_FLAG_C_OPTIONS.
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
    AC_MSG_NOTICE([with_ROSE_LONG_MAKE_CHECK_RULE set (long testing used for "make check" rule)])
else
    AC_MSG_NOTICE([with_ROSE_LONG_MAKE_CHECK_RULE not set (short testing used by default)])
fi

])
