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

# DQ (9/26/2015): Commented out to supress warning in aclocal.
# AC_REQUIRE([BTNG_INFO_CXX_ID])

# Given the C++ compiler command-line, create output variables HOST_CXX_VENDOR, HOST_CXX_VERSION, and HOST_CXX_LANGUAGE
# that contain the vendor (gnu, llvm, or intel), the version (as defined by CPP macros, not the --version output), and
# the language dialect (c++17, gnu++11, etc.).
ROSE_COMPILER_FEATURES([c++], [$CXX $CPPFLAGS $CXXFLAGS], [HOST_CXX_])

AC_MSG_NOTICE([in c++ option setting: FRONTEND_CXX_COMPILER_VENDOR = "$FRONTEND_CXX_COMPILER_VENDOR"])

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

# echo "Setting with_CXX_DEBUG to withval = $withval"
# with_CXX_DEBUG=$withval

echo "OS_VENDOR_APPLE = $OS_VENDOR_APPLE"
echo "OS_vendor = $OS_vendor"

if "x$OS_VENDOR_APPLE" == "xapple"; then
    echo "Automake conditional OS_VENDOR_APPLE: DEFINED"
else
    echo "Automake conditional OS_VENDOR_APPLE: NOT defined"
fi

echo "FRONTEND_CXX_VERSION_MAJOR = $FRONTEND_CXX_VERSION_MAJOR"

# if test $FRONTEND_CXX_COMPILER_VENDOR == "gnu" && test $FRONTEND_CXX_VERSION_MAJOR -ge 5; then
# if test "x$FRONTEND_CXX_COMPILER_VENDOR" = "xgnu" && test $FRONTEND_CXX_VERSION_MAJOR -ge 5; then
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
    # DQ (9/18/2020): The reason we can't use these options is because the MacOSX compiler is clang,
    # but it calls itself g++, and we need to figure out how to tell with the g++ is acutally clang 
    # so that we can suppress these options in just that case.
    # DQ (9/17/2020): We can't use these option until we use more modern compilers than 4.8.5 in Jenkins.
    # DQ (9/16/2020): Adding -fno-var-tracking-asignment -fno-misleading-indentation -wfatal-errors
    # CXX_DEBUG="-g"
    # CXX_DEBUG="-g -fno-var-tracking-asignment -fno-misleading-indentation"
    # CXX_DEBUG="-g -fno-misleading-indentation"
    # CXX_DEBUG="-g -Wno-misleading-indentation"
    # CXX_DEBUG="-g -fno-var-tracking-assignments"
    # CXX_DEBUG="-g -fno-var-tracking-assignments -Wno-misleading-indentation"
    # CXX_DEBUG="-g -fno-var-tracking-assignments -Wno-misleading-indentation"
    # CXX_DEBUG="-g"
    # DQ (9/20/20): We need to make sure that where the Clang compiler on MacOSX pretends to be 
    # the GNU compiler that we don't use some GNU options that are an error when used with Clang.
    # if test $FRONTEND_CXX_COMPILER_VENDOR == "gnu" && test $FRONTEND_CXX_VERSION_MAJOR -ge 5; then
    # if test $FRONTEND_CXX_COMPILER_VENDOR == "gnu" ; then
      if test "x$FRONTEND_CXX_COMPILER_VENDOR" == "xgnu" && test $FRONTEND_CXX_VERSION_MAJOR -ge 5; then
         AC_MSG_NOTICE([using additional GNU compiler options: -fno-var-tracking-assignments -Wno-misleading-indentation])
         CXX_DEBUG="-g -fno-var-tracking-assignments -Wno-misleading-indentation"
      else
         AC_MSG_NOTICE([skip using additional GNU compiler options (likely clang trying to look like g++ on MaxOSX): -fno-var-tracking-assignments -Wno-misleading-indentation])
         CXX_DEBUG="-g"
      fi
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

echo "Am I set: CXX_DEBUG= $CXX_DEBUG"
echo "CXXFLAGS currently set to $CXXFLAGS"

exit 1

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

AC_MSG_NOTICE([after initialization: with_CXX_OPTIMIZE = "$with_CXX_OPTIMIZE"])
AC_MSG_NOTICE([after initialization: CXX_OPTIMIZE = "$CXX_OPTIMIZE"])

# echo "After processing CXX_OPTIMIZE: Exiting as a test!"
# exit 1

dnl *********************************************************************
dnl * Set the C++ compiler flags in CXX_WARNINGS
dnl *********************************************************************
dnl This should use the AC_ARG_ENABLE not AC_ARC_WITH!

AC_MSG_NOTICE([setup CXX_WARNING CXX = "$CXX"])

# AC_ARG_ENABLE(warnings, AS_HELP_STRING([--enable-warnings], [Support for a uniform warning level for ROSE development]),[enableval=yes],[enableval=yes])
# AC_ARG_WITH(CXX_WARNINGS, [  --with-CXX_WARNINGS=ARG   manually set the C++ compiler warning flags to ARG (leave blank to choose automatically)])
AC_ARG_WITH(CXX_WARNINGS, AS_HELP_STRING([--with-CXX_WARNINGS], [Support for a uniform warning level for ROSE development]),[withval=yes],[withval=yes])
# AC_ARG_WITH(CXX_WARNINGS, AS_HELP_STRING([--with-CXX_WARNINGS], [Support for a uniform warning level for ROSE development]),[with_CXX_WARNINGS=yes],[with_CXX_WARNINGS=yes])

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

# echo "Setting with_CXX_WARNINGS to withval = $withval"
# with_CXX_WARNINGS=$withval

if test "x$with_CXX_WARNINGS" = "xyes"; then
# DQ (12/3/2016): Add these options to what may have been specified using enable_fatal_rose_warnings.
# CXX_WARNINGS was activated but not specified, so set it.
  AC_MSG_NOTICE([using default options for maximal warnings (true case)])
# case $CXX in
  case $FRONTEND_CXX_COMPILER_VENDOR in
#   g++)
    gnu)
    # cc1plus: warning: command line option "-Wstrict-prototypes" is valid for Ada/C/ObjC but not for C++
    # cc1plus: warning: command line option "-Wmissing-prototypes" is valid for Ada/C/ObjC but not for C++
    # CXX_WARNINGS="-Wall -Wstrict-prototypes -Wmissing-prototypes"
    #  CXX_WARNINGS="-Wall"
    # DQ (4/5/2017): Ignore some warnings (GNU feature only meaningful for small codes).
    # CXX_WARNINGS+="-Wall -Wnomisleading-indentation -Wnonnull-compare"
    # DQ (4/5/2017): Note: specification of "-Wnomisleading-indentation" causes errors downstream in boost configuration.
    # CXX_WARNINGS+="-Wall -Wnomisleading-indentation"
      CXX_WARNINGS+="-Wall"
      ;;
#   icpc)
    intel)
    # For Intel turn on 64bit migration/portability warnings
    # CXX_WARNINGS="-w1 -Wall -Wcheck -Wp64"
    # CXX_WARNINGS+="-w1 -Wall -Wcheck -Wp64"
      CXX_WARNINGS+="-Wall"
      ;;
    clang)
    # For Intel turn on 64bit migration/portability warnings
    # CXX_WARNINGS="-w1 -Wall -Wcheck -Wp64"
    # CXX_WARNINGS+="-w1 -Wall -Wcheck -Wp64"
      CXX_WARNINGS+="-Wall -Wno-unused-local-typedefs -Wno-attributes"
      ;;
#   KCC | mpKCC)
    kai)
    # CXX_WARNINGS="--for_init_diff_warning --new_for_init -w"
      CXX_WARNINGS+="--for_init_diff_warning --new_for_init -w"
      ;;
#   CC)
    sun)
      case $host_os in
        solaris*| sun4*)
        # CXX_WARNINGS=""
          CXX_WARNINGS+=""
    esac
    ;;
  esac
elif test "x$with_CXX_WARNINGS" = "xno"; then

  CXX_WARNINGS=''
# DQ (1/15/2007): turn on warnings by default.
  AC_MSG_NOTICE([using at least some default (minimal) options for warnings (false case)])
# case $CXX in
  case $FRONTEND_CXX_COMPILER_VENDOR in
#   g++)
    gnu)
    # CXX_WARNINGS="-Wall -Wstrict-prototypes -Wmissing-prototypes"
      CXX_WARNINGS="-Wall"
      ;;
#   icpc)
    intel)
    # For Intel turn on 64bit migration/portability warnings
      CXX_WARNINGS="-w1 -Wall -Wcheck -Wp64"
      ;;
    clang)
    # CXX_WARNINGS="-Wall -Wstrict-prototypes -Wmissing-prototypes"
      CXX_WARNINGS="-Wall -Wno-unused-local-typedefs -Wno-attributes"
      ;;
#   "KCC --c" | mpKCC)
    kai)
      CXX_WARNINGS="--for_init_diff_warning --new_for_init -w"
      ;;
#   CC)
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
  CXX_WARNINGS+=$with_CXX_WARNINGS
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

# AC_ARG_WITH(C_WARNINGS, [  --with-C_WARNINGS=ARG   manually set the C compiler warning flags to ARG (leave blank to choose automatically)])
# AC_ARG_WITH(C_WARNINGS, AS_HELP_STRING([--with-C_WARNINGS], [Support for a uniform warning level for ROSE development]),[with_C_WARNINGS=yes],[with_C_WARNINGS=yes])
AC_ARG_WITH(C_WARNINGS, AS_HELP_STRING([--with-C_WARNINGS], [Support for a uniform warning level for ROSE development]),[withval=yes],[withval=yes])

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
  # C_WARNINGS was activated but not specified, so set it.
# DQ (3/21/2017): Base the C language warning specification on the vendor for the C++ compiler (same as vendor for C compiler).
# case $CC in
  case $FRONTEND_CXX_COMPILER_VENDOR in
#   gcc)
    gnu)
    # C_WARNINGS="-Wall -Wstrict-prototypes -Wmissing-prototypes"
      C_WARNINGS+="-Wall -Wstrict-prototypes -Wmissing-prototypes"
      ;;
#   icc)
    intel)
    # For Intel turn on 64bit migration/portability warnings
    # C_WARNINGS="-w -Wall -Wcheck -Wp64"
      C_WARNINGS+="-w -Wall -Wcheck -Wp64"
      ;;
    clang)
    # C_WARNINGS="-Wall -Wstrict-prototypes -Wmissing-prototypes"
      C_WARNINGS+="-Wall -Wno-unused-local-typedefs -Wno-attributes"
      ;;
#   "KCC --c" | mpKCC)
    kai)
    # C_WARNINGS="--for_init_diff_warning --new_for_init -w"
      C_WARNINGS+="--for_init_diff_warning --new_for_init -w"
      ;;
#   cc)
    sun)
      case $host_os in
        solaris*| sun4*)
        # C_WARNINGS=""
          C_WARNINGS+=""
    esac
    ;;
  esac
elif test "x$with_C_WARNINGS" = "xno"; then
# DQ (12/3/2016): Ony use default warning when enable_fatal_rose_warnings is not used.
  C_WARNINGS=''
# DQ (1/15/2007): turn on warnings by default.
# case $CC in
  case $FRONTEND_CXX_COMPILER_VENDOR in
#   gcc)
    gnu)
    # C_WARNINGS="-Wall -Wstrict-prototypes -Wmissing-prototypes"
      C_WARNINGS="-Wall"
      ;;
#   icc)
    intel)
    # For Intel turn on 64bit migration/portability warnings
      C_WARNINGS="-w1 -Wall -Wcheck -Wp64"
      ;;
    clang)
    # C_WARNINGS="-Wall -Wstrict-prototypes -Wmissing-prototypes"
      C_WARNINGS="-Wall -Wno-unused-local-typedefs -Wno-attributes"
      ;;
#   "KCC --c" | mpKCC)
    kai)
      C_WARNINGS="--for_init_diff_warning --new_for_init -w"
      ;;
#   cc)
    sun)
      case $host_os in
        solaris*| sun4*)
          C_WARNINGS=""
    esac
    ;;
  esac
else
  AC_MSG_NOTICE([adding explicitly specified warnings to be used.])
  C_WARNINGS+=$with_C_WARNINGS
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

AC_MSG_NOTICE([setup ROSE_CXX_HEADERS_DIR])
# echo "GCC_MAJOR = $GCC_MAJOR"

AC_ARG_WITH(GNU_CXX_HEADERS, [  --with-GNU_CXX_HEADERS            use the the directory of included GNU header files in the ROSE source tree])

# For the g++ 3.x compilers we can generally use the 2.96 header files. However
# it does not always work (using namespace std requires a previously defined 
# namespace std which is in the 3.x headers but NOT in the 2.96 headers).
if test "$with_GNU_CXX_HEADERS" = yes; then
# if ((test "$with_GNU_HEADERS" = yes) || (test "$GCC_MAJOR" = 3)); then
  # GNU_HEADERS was activated but not specified, so set it.
    AC_MSG_NOTICE([with-GNU_CXX_HEADERS = yes (use default GNU headers (preprocessed) distributed with ROSE)])
    ROSE_CXX_HEADERS_DIR="${srcdir}/GNU_CXX_HEADERS"
else
    AC_MSG_NOTICE([with_GNU_CXX_HEADERS = "$with_GNU_CXX_HEADERS"])
fi

# echo "with-GNU_CXX_HEADERS = $with_GNU_CXX_HEADERS"
# echo "ROSE_CXX_HEADERS_DIR = $ROSE_CXX_HEADERS_DIR"

AC_ARG_WITH(ROSE_INTERNAL_HEADERS, [  --with-ROSE_INTERNAL_HEADERS=ARG  manually set the directory of header files used internally)])
if test "$with_ROSE_INTERNAL_HEADERS" = ""; then
    AC_MSG_NOTICE([with-ROSE_INTERNAL_HEADERS not set])
else
    AC_MSG_NOTICE([with-ROSE_INTERNAL_HEADERS is a user defined directory])
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

AC_MSG_NOTICE([setup ROSE_C_HEADERS_DIR])
# echo "GCC_MAJOR = $GCC_MAJOR"

AC_ARG_WITH(GNU_C_HEADERS, [  --with-GNU_C_HEADERS              use the the directory of included GNU header files in the ROSE source tree])

# For the g++ 3.x compilers we can generally use the 2.96 header files. However
# it does not always work (using namespace std requires a previously defined 
# namespace std which is in the 3.x headers but NOT in the 2.96 headers).
if test "$with_GNU_C_HEADERS" = yes; then
# if ((test "$with_GNU_HEADERS" = yes) || (test "$GCC_MAJOR" = 3)); then
  # GNU_HEADERS was activated but not specified, so set it.
    AC_MSG_NOTICE([with-GNU_C_HEADERS = yes (use default GNU headers (preprocessed) distributed with ROSE)])
    ROSE_C_HEADERS_DIR="${srcdir}/GNU_C_HEADERS"
else
    AC_MSG_NOTICE([with-GNU_C_HEADERS = "$with_GNU_C_HEADERS"])
fi

# echo "with-GNU_C_HEADERS = $with_GNU_C_HEADERS"
# echo "ROSE_C_HEADERS_DIR = $ROSE_C_HEADERS_DIR"

AC_ARG_WITH(ROSE_INTERNAL_HEADERS, [  --with-ROSE_INTERNAL_HEADERS=ARG  manually set the directory of header files used internally)])
if test "$with_ROSE_INTERNAL_HEADERS" = ""; then
    AC_MSG_NOTICE([with-ROSE_INTERNAL_HEADERS not set])
else
    AC_MSG_NOTICE([with-ROSE_INTERNAL_HEADERS is a user defined directory])
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
    AC_MSG_NOTICE([with_ROSE_LONG_MAKE_CHECK_RULE set (long testing used for "make check" rule)])
else
    AC_MSG_NOTICE([with_ROSE_LONG_MAKE_CHECK_RULE not set (short testing used by default)])
fi

])
