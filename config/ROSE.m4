
AC_DEFUN([SAGE_VAR_INCLUDES_AND_LIBS],
dnl Define SAGE_INCLUDES and SAGE_LIBS.
dnl Do not use this macro in ROSE top level.
dnl The relative paths are meant for packages
dnl just under the top level.
[
# Begin macro SAGE_VAR_INCLUDES_AND_LIBS

# set up enable-SAGE2 stuff.
# By specifying --enable-SAGE2 on the top configure command,
# we will configure and make the ROSE libraries.
# AC_ARG_ENABLE(SAGE2,
#   [  --enable-SAGE2         also configure SAGE2 (SAGE3 is default)],
#   , enable_SAGE2=no)

# Note: we expect SAGE_REL_PATH is set to the SAGE directory from either
# top_srcdir or top_builddir, whatever the case may be.
# AC_MSG_CHECKING(whether SAGE2 or SAGE3 should be used)
# if test "$enable_SAGE2" = "no"; then
#   AC_MSG_RESULT(SAGE3)
#   AC_DEFINE([USE_SAGE3],[1],[Support for SAGE 3 over SAGE 2.])
#   SAGE_INCLUDES='-I$(top_srcdir)/../SAGE'
#   SAGE_LIBS='$(top_builddir)/../SAGE/libsage3.a -ll'
# else
#   AC_MSG_RESULT(SAGE2)
#   SAGE_INCLUDES='-I$SAGE_HOME/newCCplusplus/sage2/src/baseclasses -I$SAGE_HOME/SUN_built/sage2/src/generated'
#   SAGE_LIBS="$(SAGE_HOME)/SUN_built/lib/libedg.a $(SAGE_HOME)/SUN_built/lib/libsage2.a -ll"
# fi

# AC_DEFINE([USE_SAGE3],[1],[Support for SAGE 3 over SAGE 2.])
# AC_SUBST(SAGE_INCLUDES)
# AC_SUBST(SAGE_LIBS)

# End macro SAGE_VAR_INCLUDES_AND_LIBS
])dnl



dnl
dnl SETUP_COMPILERS_THE_WAY_I_WANT sets the CC and CXX variables.
dnl
AC_DEFUN([SETUP_COMPILERS_THE_WAY_I_WANT], [
# Begin macro SETUP_COMPILERS_THE_WAY_I_WANT.

dnl DQ I added these so that the CC and CXX variable will be set using meaningful defaults
dnl    I'm aware that Brian will want to discuss this :-).

AC_ARG_WITH(CC,
   [  --with-CC=ARG ............................ manually set C compiler to ARG],
   CC=$withval,
   [
# echo "Before test of CC (CC = $CC)"
if test ! "$CC"; then
# echo "Resetting CC based on host = $host"
dnl BTNG added the if test to bypass the look-up table if CC is defined in the
dnl enironment.  Using the environment is the expected behavior of AC_PROG_CC.
dnl The look-up table is due to DQ.
case $host_os in
      hpux*)
        CC=cc
      ;;
      sun*| solaris*)
        CC=cc
      ;;
      osf*)
        CC=cc
      ;;
      aix*)
        CC=mpcc
      ;;
      irix*)
        CC=cc
      ;;
      linux*)
        CC=gcc

dnl Solution from http://root.cern.ch/root/roottalk/roottalk02/3156.html
        GCC_MAJOR=`echo|gcc -dumpversion | cut -d\. -f1`
        GCC_MINOR=`echo|gcc -dumpversion | cut -d\. -f2`

dnl     GCC_MAJOR    := $(shell $(CXX) -dumpversion 2>&1 | cut -d'.' -f1)
dnl     GCC_MINOR    := $(shell $(CXX) -dumpversion 2>&1 | cut -d'.' -f2)

        echo "gnu version GCC_MAJOR = $GCC_MAJOR GCC_MINOR = $GCC_MINOR"
      ;;
      *)

dnl Try to find a C compiler to use (gcc is the default here)
        CC=gcc
# AC_PROG_CC

dnl Initialize CFLAGS to null string
        CFLAGS=""
   esac
else
    echo "CC was previously set to an explicit value so don't reset based on host = $host"
fi
]  )

# echo "Inside of COMPILERS_THE_WAY_I_WANT (before call to ac prog cc): CFLAGS = $CFLAGS"
# echo "     compiler version numbers major = $GCC_MAJOR  minor = $GCC_MINOR"

AC_PROG_CC($CC)

# setup the similar options for the C compiler also
ROSE_FLAG_C_OPTIONS

AC_ARG_WITH(CXX,
   [  --with-CXX=ARG ........................... manually set C++ compiler to ARG],
   CXX=$withval,
   [
# echo "Before test of CXX (CXX = $CXX)"
if test ! "$CXX"; then
# echo "Resetting CXX based on host = $host"
dnl BTNG added the if test to bypass the look-up table if CXX is defined in the
dnl enironment.  Using the environment is the expected behavior of AC_PROG_CXX.
dnl The look-up table is due to DQ.
case $host_os in
      hpux*)
        CXX=aCC
        CFLAGS=""
        CXXFLAGS=""
      ;;
      sun* | solaris*)
        CXX=CC
        CFLAGS=""
        CXXFLAGS=""
      ;;
      osf*)
        CXX=cxx
        CFLAGS=""
        CXXFLAGS=""
      ;;
      irix*)
        CXX=CC
        CFLAGS=""
        CXXFLAGS=""
      ;;
      aix*)
        CXX=mpCC
        CFLAGS=""
        CXXFLAGS=""
      ;;
      linux*)
        CXX=g++
        CFLAGS="-fPIC"
        CXXFLAGS="-fPIC"

dnl Solution from http://root.cern.ch/root/roottalk/roottalk02/3156.html
        GXX_MAJOR=`echo|g++ -dumpversion | cut -d\. -f1`
        GXX_MINOR=`echo|g++ -dumpversion | cut -d\. -f2`
      ;;
      *)
        CXX=g++
dnl DQ(2/7/2000) reordered the different defaults to give 
dnl vendor compilers a higher priority than KCC C++ compiler
dnl but a higher priority than the GNU g++ compiler.
      # CCC="CC cxx xlC mpKCC KCC g++"
dnl Try to find a compiler to use
      # AC_PROG_CXX
dnl Initialize CXXFLAGS to null string
      # CXXFLAGS=""
        CFLAGS="-fPIC"
        CXXFLAGS="-fPIC"
        ;;
   esac
fi
]  )

# echo "Inside of COMPILERS_THE_WAY_I_WANT (before call to ac prog cxx): CXXFLAGS = $CXXFLAGS"
# echo "     compiler version numbers major = $GXX_MAJOR  minor = $GXX_MINOR"

AC_PROG_CXX($CXX)

# echo "Inside of COMPILERS_THE_WAY_I_WANT: CC = $CC"
# echo "Inside of COMPILERS_THE_WAY_I_WANT: CFLAGS = $CFLAGS"
# echo "Inside of COMPILERS_THE_WAY_I_WANT: CXX = $CXX"
# echo "Inside of COMPILERS_THE_WAY_I_WANT: CXXFLAGS = $CXXFLAGS"

# echo "Resetting CFLAGS to null string: CFLAGS = $CFLAGS"
# CFLAGS=""
# echo "After reset: CFLAGS = $CFLAGS"

# echo "Resetting CXXFLAGS to null string: CXXFLAGS = $CXXFLAGS"
# CXXFLAGS=""
# echo "After reset: CXXFLAGS = $CXXFLAGS"

# End macro SETUP_COMPILERS_THE_WAY_I_WANT
])dnl







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



dnl *********************************************************************
dnl * Set the C++ compiler flags in CXX_TEMPLATES (Options for templates)
dnl *********************************************************************

echo "Setup CXX_TEMPLATES"

# Cause configure to guess by default
AC_ARG_WITH(CXX_TEMPLATES, [  --with-CXX_TEMPLATES=ARG manually set CXX_TEMPLATES to ARG.
                           (leave blank to choose automatically)],, with_CXX_TEMPLATES=yes; )

# DQ (1/15/2007): This macro should not longer be used (depricated).
# This is a macro that is not used anymore.  Such compiler specific flags are setup
# internally and not as configure options (since they make the setup of ROSE overly 
# complex for users).

if test "$with_CXX_TEMPLATES" = yes; then
  # Guess what the correct C++ templates flag should be.
  echo "Guessing at template flags ..."
  case $CXX_ID in
dnl    gnu) CXX_TEMPLATES="-fno-implicit-templates" ;;
dnl    gnu) CXX_TEMPLATES="-frepo" ;;
    gnu) CXX_TEMPLATES="" ;;
    intel) CXX_TEMPLATES="" ;;
    kai) CXX_TEMPLATES="--one_instantiation_per_object --for_init_diff_warning --new_for_init" ;;
    sunpro) CXX_TEMPLATES='-ptv -ptr$(CXX_TEMPLATE_REPOSITORY_PATH)'
      case "$CXX_VERSION" in
        0x4*) CXX_TEMPLATE_OBJECTS='$(CXX_TEMPLATE_REPOSITORY_PATH)/Templates.DB/*.o' ;;
        5*) CXX_TEMPLATE_OBJECTS= ;;
      esac
    ;;
    *) CXX_TEMPLATES="-pt used -prelink"
    ;;
  esac
else
  CXX_TEMPLATES=$with_CXX_TEMPLATES
fi

# echo "debugging: CXX_ID               = $CXX_ID"
# echo "debugging: CXX_VERSION          = $CXX_VERSION"
# echo "debugging: CXX_TEMPLATE_OBJECTS = $CXX_TEMPLATE_OBJECTS"

CXX_TEMPLATE_REPOSITORY_PATH='$(top_builddir)/src'

AC_SUBST(CXX_TEMPLATES)
AC_SUBST(CXX_TEMPLATE_REPOSITORY_PATH)
AC_SUBST(CXX_TEMPLATE_OBJECTS)

## Don't do this here since we have to use the unmodified CXXFLAGS variable for the 
## configure compilation tests (and we want don't want those templates in our repository)
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
