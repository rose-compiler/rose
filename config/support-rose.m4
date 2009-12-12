AC_DEFUN([ROSE_SUPPORT_ROSE_PART_1],
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

# For testing the configure script generation this link can be commented out
# to improve performance of tests unrelated to backend compiler headr files.
# DQ (9/17/2006): This must be done for BOTH C++ and C compilers (since the
# compiler-specific header files for each can be different; as is the case 
# for GNU).
# GENERATE_BACKEND COMPILER_SPECIFIC_HEADERS
# GENERATE_BACKEND_CXX_COMPILER_SPECIFIC_HEADERS


# End macro ROSE_SUPPORT_ROSE.
]
)





AC_DEFUN([ROSE_SUPPORT_ROSE_PART_2],
[
# Begin macro ROSE_SUPPORT_ROSE.

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

## This should be set after a complex test (turn it on as default)
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

# End macro ROSE_SUPPORT_ROSE_PART_2.
]
)



AC_DEFUN([ROSE_SUPPORT_ROSE_PART_3],
[
# Begin macro ROSE_SUPPORT_ROSE.

## Setup the EDG specific stuff
SETUP_EDG

# Setup Automake conditional in Projects/DataBase/Makefile.am
# AM_CONDITIONAL(ROSE_USE_EDG_3_3,test "$with_EDG_3_3" = yes)

# Find md5 or md5sum and create a signature for ROSE binary compatibility
AC_CHECK_PROGS(MD5, [md5 md5sum], [false])
AC_SUBST(MD5)
if test -e ${srcdir}/src/frontend/CxxFrontend/EDG/Makefile.am; then
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

# End macro ROSE_SUPPORT_ROSE_PART_3.
]
)


AC_DEFUN([ROSE_SUPPORT_ROSE_PART_4],
[
# Begin macro ROSE_SUPPORT_ROSE.

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

# End macro ROSE_SUPPORT_ROSE_PART_3.
]
)

