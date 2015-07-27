dnl **************************************************************************************************************************
dnl * This m4 file encapsulates the prerequisites and optional libraries needed for binary analysis. Additional dependencies
dnl * that are required also for other parts of ROSE are not present in this file.
dnl *
dnl * We assume that $support_binaries_frontend is set to "yes" if binary analysis support is enabled in ROSE.  See
dnl * $ROSE/config/support-languages.m4 for details.
dnl **************************************************************************************************************************


dnl Support function for printing messages
dnl   usage: binary_support_requires WHAT_IS_REQUIRED, ERROR_MSG, HOW_TO_INSTALL
dnl   example: binary_suport_requries([encryption],[libgcrypt not found],[See http://nowhere for instructions.])
m4_define([binary_support_requires],[
    AC_MSG_NOTICE([error: $2

*** Binary analysis support in ROSE requires $1.
*** You can either install the prerequisite, or disable binary analysis
*** support in ROSE.  Binary analysis is one of ROSE's frontend languages.
*** If you commonly only work with source languages (C, C++, etc.) then
*** you can safely disable binary analysis support.
***
*** Option 1: $3
***
*** Option 2: To configure ROSE without binary analysis support, rerun the
***           configuration command you just used, but this time add
***           the "--enable-binary-analysis=no" switch.
    ])
    AC_MSG_ERROR([$2])
])


AC_DEFUN([ROSE_SUPPORT_BINARY],[
ROSE_CONFIGURE_SECTION([Binary analysis support])

dnl  ==================================================================================
dnl   The following tests check prerequisites for binary analysis in librose.
dnl  ==================================================================================

# Boost iostreams library is required
AX_BOOST_IOSTREAMS
if test "$support_binaries_frontend" == "yes" -a "$link_iostreams" != "yes"; then
    binary_support_requires(
        [the Boost iostreams library],
        [could not link against libboost_iostreams],
        [To install Boost's iostreams library, follow the directions at
***           http://www.boost.org. It is fairly common to need the
***           "-sNO_COMPRESSION=1" switch when configuring boost.])
fi


dnl  ==================================================================================
dnl   Check for optional packages that binary analysis in librose can use if available
dnl  ==================================================================================

dnl The libmagic library allows us to check for magic numbers in memory without having to copy the memory
dnl out to a temporary file and run the Unix file(1) command on it, which is way, way slower when we have
dnl to test for lots of magic numbers.  Used by src/midend/binaryAnalyses/BinaryMagic.h. [2015-01]
ROSE_SUPPORT_LIBMAGIC




dnl  Some of the following need to be reevaluated to see whether they're actually used. [Matzke 2015-01-21]


# Call supporting macro to Yices Satisfiability Modulo Theories (SMT) Solver
ROSE_SUPPORT_YICES

# Is the C++ libyaml available? [https://code.google.com/p/yaml-cpp]
ROSE_SUPPORT_YAML

# Is the GNU readline library available?  This is used by some projects to allow users to edit inputs. E.g., simulator2
# has an interactive debugger that uses readline to read debugger commands.
ROSE_SUPPORT_READLINE

# Call supporting macro to check for "--enable-i386" switch
ROSE_SUPPORT_I386

# Call supporting macro to internal Satisfiability (SAT) Solver
ROSE_SUPPORT_SAT

# Setup Automake conditional in --- (not yet ready for use)
echo "with_sat = $with_sat"
AM_CONDITIONAL(ROSE_USE_SAT,test ! "$with_sat" = no)

# Call supporting macro to Intel Pin Dynamic Instrumentation
ROSE_SUPPORT_INTEL_PIN

# Setup Automake conditional in --- (not yet distributed)
AM_CONDITIONAL(ROSE_USE_INTEL_PIN,test ! "$with_IntelPin" = no)

# Call supporting macro to DWARF (libdwarf)
ROSE_SUPPORT_DWARF

# Setup Automake conditional in --- (not yet distributed)
AM_CONDITIONAL(ROSE_USE_DWARF,test ! "$with_dwarf" = no)

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

# libgcrypt is used for computing SHA1 hashes of binary basic block semantics, among other things. [RPM 2010-05-12]
AC_CHECK_HEADERS(gcrypt.h,,[HAVE_GCRYPT=yes],[HAVE_GCRYPT=no])
AC_CHECK_LIB(gpg-error,gpg_strerror) dnl needed by statically linked libgcrypt
AC_CHECK_LIB(gcrypt,gcry_check_version)
AM_CONDITIONAL([HAS_LIBRARY_GCRYPT], [test "x$HAVE_GCRYPT" = "xyes"])

# Check for POSIX threads.  Just because we have POSIX threads does not necessarily mean that the user wants ROSE
# to be compiled with multi-thread support.  See also "--with-boost-thread" configure switch.
AC_CHECK_HEADERS(pthread.h)

# Check for the __thread keyword.  This type qualifier creates objects that are thread local.
AC_MSG_CHECKING([for thread local storage type qualifier])
AC_COMPILE_IFELSE([struct S {int a, b;}; static __thread struct S x;],
        [AC_DEFINE(ROSE_THREAD_LOCAL_STORAGE, __thread, [Define to __thread keyword for thread local storage.])
         AC_MSG_RESULT([__thread])],
        [AC_MSG_RESULT([not supported])])

# These headers and types are needed by projects/simulator [matzke 2009-07-02]
AC_CHECK_HEADERS([asm/ldt.h elf.h linux/types.h linux/dirent.h linux/unistd.h])
AC_CHECK_HEADERS([sys/types.h sys/mman.h sys/stat.h sys/uio.h sys/wait.h sys/utsname.h sys/ioctl.h sys/sysinfo.h sys/socket.h])
AC_CHECK_HEADERS([termios.h grp.h syscall.h])
AC_CHECK_FUNCS(pipe2)
AC_CHECK_TYPE(user_desc,
              AC_DEFINE(HAVE_USER_DESC, [], [Defined if the user_desc type is declared in <asm/ldt.h>]),
              [],
              [#include <asm/ldt.h>])

# Check whether PostgreSQL is supported
AC_CHECK_HEADERS([pqxx/version.hxx])
AC_CHECK_LIB(pqxx, PQconnectdb)

TEST_SMT_SOLVER=""
AC_ARG_WITH(smt-solver,
[  --with-smt-solver=PATH       Specify the path to an SMT-LIB compatible SMT solver.  Used only for testing.],
if test "x$with_smt_solver" = "xcheck" -o "x$with_smt_solver" = "xyes"; then
  AC_ERROR([--with-smt-solver cannot be auto-detected])
fi
if test "x$with_smt_solver" != "xno"; then
  TEST_SMT_SOLVER="$with_smt_solver"
fi,
)

AM_CONDITIONAL(ROSE_USE_TEST_SMT_SOLVER,test ! "$TEST_SMT_SOLVER" = "")
AC_SUBST(TEST_SMT_SOLVER)

dnl A blank line to separate binary analysis from some miscellaneous tests in support-rose.m4 that don't have a heading.
AC_MSG_NOTICE([All seems good for binary analysis if it's enabled.])
echo
])
