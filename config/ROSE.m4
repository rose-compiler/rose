
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
# AC_ARG_WITH(CXX_DEBUG, [  --with-CXX_DEBUG=ARG    manually set the C++ compiler debug flags to ARG (leave blank to choose automatically)])
AC_ARG_WITH(CXX_DEBUG, AS_HELP_STRING([--with-CXX_DEBUG], [manually set the C++ compiler debug flags to ARG (leave blank to choose automatically)]),[withval=yes],[withval=yes])

# if test "$with_CXX_DEBUG" = yes; then
#  # CXX_DEBUG was activated but not specified, so set it.
#  case $CXX in
#    *) CXX_DEBUG='-g' ;;
#  esac
#elif test "$with_CXX_DEBUG" = no; then
#  CXX_DEBUG=''
#else
#  CXX_DEBUG=$with_CXX_DEBUG
#fi

echo "withval = $withval"
echo "with_CXX_DEBUG = $with_CXX_DEBUG"

if test "x$with_CXX_DEBUG" = "x"; then
   if test "x$withval" = "xyes"; then
      with_CXX_DEBUG=$withval
   else 
      if test "x$withval" = "xno"; then
         with_CXX_DEBUG=$withval
      fi
   fi
else
   echo "with_CXX_DEBUG is explictly set to: $with_CXX_DEBUG"
fi

echo "After initialization: with_CXX_DEBUG = $with_CXX_DEBUG"

# echo "Setting with_CXX_DEBUG to withval = $withval"
# with_CXX_DEBUG=$withval

if test "x$with_CXX_DEBUG" = "xyes"; then
# CXX_DEBUG was activated but not specified, so set it.
  echo "Using default options for maximal debug (true case)"
  case $CXX in
    g++)
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
  echo "Using at least some default (minimal) options for debug flags (currently the same as above) (false case)"
  case $CXX in
    g++)
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
  echo "Using user provided options for CXX_DEBUG..."
  CXX_DEBUG=$with_CXX_DEBUG
fi

AC_SUBST(CXX_DEBUG)
if test -n "$CXX_DEBUG"; then CXXFLAGS="$CXXFLAGS $CXX_DEBUG"; fi

# echo "Am I set: CXX_DEBUG= $CXX_DEBUG"
# echo "CXXFLAGS currently set to $CXXFLAGS"


dnl *********************************************************************
dnl * Set the C++ compiler optimization flags in CXXOPT
dnl *********************************************************************
dnl This should use the AC_ARG_ENABLE not AC_ARC_WITH!

echo "Setup CXX_OPTIMIZE"

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

# AC_ARG_ENABLE(warnings, AS_HELP_STRING([--enable-warnings], [Support for a uniform warning level for ROSE development]),[enableval=yes],[enableval=yes])
# AC_ARG_WITH(CXX_WARNINGS, [  --with-CXX_WARNINGS=ARG   manually set the C++ compiler warning flags to ARG (leave blank to choose automatically)])
AC_ARG_WITH(CXX_WARNINGS, AS_HELP_STRING([--with-CXX_WARNINGS], [Support for a uniform warning level for ROSE development]),[withval=yes],[withval=yes])
# AC_ARG_WITH(CXX_WARNINGS, AS_HELP_STRING([--with-CXX_WARNINGS], [Support for a uniform warning level for ROSE development]),[with_CXX_WARNINGS=yes],[with_CXX_WARNINGS=yes])

echo "withval = $withval"
echo "with_CXX_WARNINGS = $with_CXX_WARNINGS"

if test "x$with_CXX_WARNINGS" = "x"; then
   if test "x$withval" = "xyes"; then
      with_CXX_WARNINGS=$withval
   else 
      if test "x$withval" = "xno"; then
         with_CXX_WARNINGS=$withval
      fi
   fi
else
   echo "with_CXX_WARNINGS is explictly set to: $with_CXX_WARNINGS"
fi

echo "After initialization: with_CXX_WARNINGS = $with_CXX_WARNINGS"

# echo "Setting with_CXX_WARNINGS to withval = $withval"
# with_CXX_WARNINGS=$withval

if test "x$with_CXX_WARNINGS" = "xyes"; then
# CXX_WARNINGS was activated but not specified, so set it.
  echo "Using default options for maximal warnings (true case)"
  case $CXX in
    g++)
    # cc1plus: warning: command line option "-Wstrict-prototypes" is valid for Ada/C/ObjC but not for C++
    # cc1plus: warning: command line option "-Wmissing-prototypes" is valid for Ada/C/ObjC but not for C++
    # CXX_WARNINGS="-Wall -Wstrict-prototypes -Wmissing-prototypes"
      CXX_WARNINGS="-Wall"
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
elif test "x$with_CXX_WARNINGS" = "xno"; then
  CXX_WARNINGS=''
# DQ (1/15/2007): turn on warnings by default.
  echo "Using at least some default (minimal) options for warnings (false case)"
  case $CXX in
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
# Settings specified explicitly by the user.
  echo "Using user provided options for CXX_WARNINGS..."
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

echo "C_DEBUG   = $C_DEBUG"
echo "CXX_DEBUG = $CXX_DEBUG"

echo "C_WARNINGS   = $C_WARNINGS"
echo "CXX_WARNINGS = $CXX_WARNINGS"

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

echo "withval = $withval"
echo "with_C_DEBUG = $with_C_DEBUG"

if test "x$with_C_DEBUG" = "x"; then
   if test "x$withval" = "xyes"; then
      with_C_DEBUG=$withval
   else 
      if test "x$withval" = "xno"; then
         with_C_DEBUG=$withval
      fi
   fi
else
   echo "with_C_DEBUG is explictly set to: $with_C_DEBUG"
fi

echo "After initialization: with_C_DEBUG = $with_C_DEBUG"

# echo "Setting with_C_DEBUG to withval = $withval"
# with_C_DEBUG=$withval

if test "x$with_C_DEBUG" = "xyes"; then
# C_DEBUG was activated but not specified, so set it.
  echo "Using default options for maximal debug (true case)"
  case $CC in
    gcc)
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
  echo "Using at least some default (minimal) options for debug flags (currently the same as above) (false case)"
  case $CC in
    gcc)
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
  echo "Using user provided options for C_DEBUG..."
  C_DEBUG=$with_C_DEBUG
fi

AC_SUBST(C_DEBUG)
if test -n "$C_DEBUG"; then CFLAGS="$CFLAGS $C_DEBUG"; fi

# echo "Am I set: C_DEBUG= $C_DEBUG"


dnl *********************************************************************
dnl * Set the C compiler optimization flags in C_OPTIMIZE
dnl *********************************************************************
dnl This should use the AC_ARG_ENABLE not AC_ARC_WITH!

echo "Setup C_OPTIMIZE"

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

# AC_ARG_WITH(C_WARNINGS, [  --with-C_WARNINGS=ARG   manually set the C compiler warning flags to ARG (leave blank to choose automatically)])
# AC_ARG_WITH(C_WARNINGS, AS_HELP_STRING([--with-C_WARNINGS], [Support for a uniform warning level for ROSE development]),[with_C_WARNINGS=yes],[with_C_WARNINGS=yes])
AC_ARG_WITH(C_WARNINGS, AS_HELP_STRING([--with-C_WARNINGS], [Support for a uniform warning level for ROSE development]),[withval=yes],[withval=yes])

echo "withval = $withval"
echo "with_C_WARNINGS = $with_C_WARNINGS"

if test "x$with_C_WARNINGS" = "x"; then
   if test "x$withval" = "xyes"; then
      with_C_WARNINGS=$withval
   else 
      if test "x$withval" = "xno"; then
         with_C_WARNINGS=$withval
      fi
   fi
else
   echo "with_C_WARNINGS is explictly set to: $with_C_WARNINGS"
fi

echo "After initialization: with_C_WARNINGS = $with_C_WARNINGS"

if test "x$with_C_WARNINGS" = "xyes"; then
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
elif test "x$with_C_WARNINGS" = "xno"; then
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

echo "C_DEBUG   = $C_DEBUG"
echo "CXX_DEBUG = $CXX_DEBUG"

echo "C_WARNINGS   = $C_WARNINGS"
echo "CXX_WARNINGS = $CXX_WARNINGS"

# echo "Exiting at the base of ROSE FLAG C OPTIONS..."
# exit 1;

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
