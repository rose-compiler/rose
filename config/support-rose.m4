
#-----------------------------------------------------------------------------
AC_DEFUN([ROSE_SUPPORT_ROSE_PART_1],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_1.

# *********************************************************************
# This macro encapsulates the complexity of the tests required for ROSE
# to understnd the machine environment and the configure command line.
# It is represented a s single macro so that we can simplify the ROSE
# configure.in and permit other external project to call this macro as
# a way to set up there environment and define the many macros that an
# application using ROSE might require.
# *********************************************************************

# DQ (2/11/2010): Jeremiah reported this as bad syntax, I think he is correct.
# I'm not sure how this made it into this file.
# AMTAR ?= $(TAR)
AMTAR="$TAR"

# DQ (9/9/2009): Added test.
if test "$am__tar" = "false"; then
   AC_MSG_FAILURE([am__tar set to false])
fi

# DQ (9/9/2009): Added test.
if test "$am__untar" = "false"; then
   AC_MSG_FAILURE([am__untar set to false])
fi

# DQ (3/20/2009): Trying to get information about what system we are on so that I
# can detect Cygwin and OSX (and other operating systems in the future).
AC_CANONICAL_BUILD
# AC_CANONICAL_HOST
# AC_CANONICAL_TARGET

AC_MSG_CHECKING([machine hardware cpu])
AC_MSG_RESULT([$build_cpu])

AC_MSG_CHECKING([operating system vendor])
AC_MSG_RESULT([$build_vendor])

AC_MSG_CHECKING([operating system])
AC_MSG_RESULT([$build_os])

DETERMINE_OS

# DQ (3/20/2009): The default is to assume Linux, so skip supporting this test.
# AM_CONDITIONAL(ROSE_BUILD_OS_IS_LINUX,  [test "x$build_os" = xlinux-gnu])
AM_CONDITIONAL(ROSE_BUILD_OS_IS_OSX,    [test "x$build_vendor" = xapple])
AM_CONDITIONAL(ROSE_BUILD_OS_IS_CYGWIN, [test "x$build_os" = xcygwin])

# DQ (9/10/2009): A more agressive attempt to identify the OS vendor
# This sets up automake conditional variables for each OS vendor name.
DETERMINE_OS_VENDOR

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
   AC_MSG_NOTICE([setting prefix to default: "$PWD"])
   prefix="$PWD"
fi

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
        AC_MSG_ERROR([gcj not supported; please configure sun java as javac])
     fi

    fi
    JAVA_HOME="`dirname $JAVA`/.."
  fi
fi

# Call supporting macro for the Java path required by the Open Fortran Parser (for Fortran 2003 support)
# Use our classpath in case the user's is messed up
AS_SET_CATFILE([ABSOLUTE_SRCDIR], [`pwd`], [${srcdir}])

# Check for Java support used internally to support both the Fortran language (OFP fortran parser) and Java language (ECJ java parser).
ROSE_SUPPORT_JAVA # This macro uses JAVA_HOME

ROSE_CONFIGURE_SECTION([Checking GNU Fortran])
# DQ (10/18/2010): Check for gfortran (required for syntax checking and semantic analysis of input Fortran codes)
AX_WITH_PROG(GFORTRAN_PATH, [gfortran], [])
AC_SUBST(GFORTRAN_PATH)

# DQ (11/17/2016): We need to make sure that --without-gfortran does not set USE_GFORTRAN_IN_ROSE to true.
# if test "x$GFORTRAN_PATH" != "x"; then
if test "x$GFORTRAN_PATH" != "x" -a "$GFORTRAN_PATH" != "no"; then
   AC_DEFINE([USE_GFORTRAN_IN_ROSE], [1], [Mark that GFORTRAN is available])
else
   AC_DEFINE([USE_GFORTRAN_IN_ROSE], [0], [Mark that GFORTRAN is not available])
fi

AC_MSG_NOTICE([GFORTRAN_PATH = "$GFORTRAN_PATH"])

# Call supporting macro for X10 language compiler path

#########################################################################################
##

  ROSE_SUPPORT_X10()

  ROSE_SUPPORT_LANGUAGE_CONFIG_OPTIONS

  ROSE_CONFIGURE_SECTION([])
  AC_CHECK_LIB([curl], [Curl_connect], [HAVE_CURL=yes], [HAVE_CURL=no])
  AM_CONDITIONAL([HAS_LIBRARY_CURL], [test "x$HAVE_CURL" = "xyes"])

  ROSE_SUPPORT_UPC
  ROSE_SUPPORT_COMPASS2
  ROSE_SUPPORT_GMP
  ROSE_SUPPORT_ISL
  ROSE_SUPPORT_MPI
  ROSE_SUPPORT_SPOT

##
#########################################################################################


## Rasmussen (10/24/2017): Support for linking with the GnuCOBOL parse-tree library.
#
ROSE_SUPPORT_COBOL
#########################################################################################

## Rasmussen (11/19/2017): Support for using the modified GNU Octave parser.
#
ROSE_SUPPORT_OCTAVE
#########################################################################################


# *******************************************************
# ROSE/projects directory compilation & testing
# *******************************************************
ROSE_ARG_ENABLE(
  [projects-directory],
  [if we should enable the ROSE/projects directory],
  [Toggle compilation and testing of the the ROSE/projects directory (disabled by default)],
  [no])

if test "x$ROSE_ENABLE_PROJECTS_DIRECTORY" = "xyes"; then
   AC_DEFINE([ROSE_BUILD_PROJECTS_DIRECTORY_SUPPORT], [], [Build ROSE projects directory])
fi
AM_CONDITIONAL(ROSE_BUILD_PROJECTS_DIRECTORY_SUPPORT, [test "x$ROSE_ENABLE_PROJECTS_DIRECTORY" = "xyes"])

# ****************************************************
# ROSE/tests directory compilation & testing
# ****************************************************
AC_MSG_CHECKING([if we should build & test the ROSE/tests directory])
AC_ARG_ENABLE([tests-directory],AS_HELP_STRING([--disable-tests-directory],[Disable compilation and testing of the ROSE/tests directory]),[],[enableval=yes])
support_tests_directory=yes
if test "x$enableval" = "xyes"; then
   support_tests_directory=yes
   AC_MSG_RESULT(enabled)
   AC_DEFINE([ROSE_BUILD_TESTS_DIRECTORY_SUPPORT], [], [Build ROSE tests directory])
else
   support_tests_directory=no
   AC_MSG_RESULT(disabled)
fi
AM_CONDITIONAL(ROSE_BUILD_TESTS_DIRECTORY_SUPPORT, [test "x$support_tests_directory" = xyes])
# *******************************************************
# ROSE/tutorial directory compilation & testing
# *******************************************************
AC_MSG_CHECKING([if we should build & test the ROSE/tutorial directory])
AC_ARG_ENABLE([tutorial-directory],AS_HELP_STRING([--disable-tutorial-directory],[Disable compilation and testing of the ROSE/tutorial directory]),[],[enableval=yes])
support_tutorial_directory=yes
if test "x$enableval" = "xyes"; then
   support_tutorial_directory=yes
   AC_MSG_RESULT(enabled)
   AC_DEFINE([ROSE_BUILD_TUTORIAL_DIRECTORY_SUPPORT], [], [Build ROSE tutorial directory])
else
   support_tutorial_directory=no
   AC_MSG_RESULT(disabled)
fi
AM_CONDITIONAL(ROSE_BUILD_TUTORIAL_DIRECTORY_SUPPORT, [test "x$support_tutorial_directory" = xyes])

# ************************************************************
# Option to turn on a special mode of memory pools: no reuse of deleted memory.
# This is useful to track AST nodes during transformation, otherwise the same memory may be reused
# by multiple different AST nodes.
# Liao 8/13/2014
# ************************************************************

AC_ARG_ENABLE(memoryPoolNoReuse, AS_HELP_STRING([--enable-memory-pool-no-reuse], [Enable special memory pool model: no reuse of deleted memory (default is to reuse memory)]))
AM_CONDITIONAL(ROSE_USE_MEMORY_POOL_NO_REUSE, [test "x$enable_memory_pool_no_reuse" = xyes])
if test "x$enable_memory_pool_no_reuse" = "xyes"; then
  AC_MSG_WARN([turn on a special mode in memory pools: no reuse of deleted memory blocks])
  AC_DEFINE([ROSE_USE_MEMORY_POOL_NO_REUSE], [], [Whether to use a special no-reuse mode of memory pools])
fi


# ************************************************************
# Option to control the size of the generated files by ROSETTA
# ************************************************************

# DQ (12/29/2009): This is part of optional support to reduce the sizes of some of the ROSETTA generated files.
AC_ARG_ENABLE(smallerGeneratedFiles, AS_HELP_STRING([--enable-smaller-generated-files], [ROSETTA generates smaller files (but more of them so it takes longer to compile)]))
AM_CONDITIONAL(ROSE_USE_SMALLER_GENERATED_FILES, [test "x$enable_smaller_generated_files" = xyes])
if test "x$enable_smaller_generated_files" = "xyes"; then
  AC_MSG_WARN([using optional ROSETTA mechanism to generate numerous but smaller files for the ROSE IR])
  AC_DEFINE([ROSE_USE_SMALLER_GENERATED_FILES], [], [Whether to use smaller (but more numerous) generated files for the ROSE IR])
fi

# DQ (11/14/2011): Added new configure mode to support faster development of langauge specific
# frontend support (e.g. for work on new EDG 4.3 front-end integration into ROSE).
AC_ARG_ENABLE(internalFrontendDevelopment, AS_HELP_STRING([--enable-internalFrontendDevelopment], [Enable development mode to reduce files required to support work on language frontends]))
AM_CONDITIONAL(ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT, [test "x$enable_internalFrontendDevelopment" = xyes])
if test "x$enable_internalFrontendDevelopment" = "xyes"; then
  AC_MSG_WARN([using reduced set of files to support faster development of language frontend work; e.g. new EDG version 4.3 to translate EDG to ROSE (internal use only)!])

# DQ (11/14/2011): It is not good enough for this to be processed here (added to the rose_config.h file)
# since it is seen too late in the process.
# AC_DEFINE([ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT], [], [Whether to use internal reduced mode to support integration of the new EDG version 4.x])
fi

# This is the support for using EDG as the frontend in ROSE.
ROSE_SUPPORT_EDG

# This is the support for using Clang as a frontend in ROSE not the support for Clang as a compiler to compile ROSE source code.
ROSE_SUPPORT_CLANG

# Support for using F18/Flang as a Fortran frontend in ROSE
ROSE_SUPPORT_FLANG

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
  AC_DEFINE([ROSE_USE_MICROSOFT_EXTENSIONS], [], [Controls use of Microsoft MSVC features])
else
  ROSE_SUPPORT_MICROSOFT_EXTENSIONS="FALSE"
fi
AC_SUBST(ROSE_SUPPORT_MICROSOFT_EXTENSIONS)
AM_CONDITIONAL(ROSE_USE_MICROSOFT_EXTENSIONS, [test "x$enable_microsoft_extensions" = xyes])

# TV (12/31/2018): Defining macro to detect the support of __float128 in EDG
#   Only valid if compiling ROSE using GNU compiler (depends on -lquadmath)
AC_LANG(C++)
AX_COMPILER_VENDOR

ac_save_LIBS="$LIBS"
LIBS="$ac_save_LIBS -lquadmath"
AC_LINK_IFELSE([
            AC_LANG_PROGRAM([[#include <quadmath.h>]])],
            [rose_use_edg_quad_float=yes],
            [rose_use_edg_quad_float=no])
LIBS="$ac_save_LIBS"

if test "$ROSE_SUPPORT_MICROSOFT_EXTENSIONS" == "TRUE"; then
  rose_use_edg_quad_float=no
fi

if test "x$rose_use_edg_quad_float" == "xyes"; then
  AC_DEFINE([ROSE_USE_EDG_QUAD_FLOAT], [], [Enables support for __float80 and __float128 in EDG.])
fi
AC_SUBST(ROSE_USE_EDG_QUAD_FLOAT)
AM_CONDITIONAL(ROSE_USE_EDG_QUAD_FLOAT, [ test $rose_use_edg_quad_float == yes ])
unset ax_cv_cxx_compiler_vendor

# DQ (9/16/2012): Added support for debugging output of new EDG/ROSE connection.  More specifically
# if this is not enabled then it skips the use of output spew in the new EDG/ROSE connection code.
AC_ARG_ENABLE(debug_output_for_new_edg_interface,
    AS_HELP_STRING([--enable-debug_output_for_new_edg_interface], [Enable debugging output (spew) of new EDG/ROSE connection]))
AM_CONDITIONAL(ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION, [test "x$enable_debug_output_for_new_edg_interface" = xyes])
if test "x$enable_debug_output_for_new_edg_interface" = "xyes"; then
  AC_MSG_WARN([using this mode causes large volumes of output spew (internal debugging only)!])
  AC_DEFINE([ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION], [], [Controls large volumes of output spew useful for debugging new EDG/ROSE connection code])
fi

# DQ (6/7/2013): Added support for new Fortran front-end development.
AC_ARG_ENABLE(experimental_fortran_frontend,
    AS_HELP_STRING([--enable-experimental_fortran_frontend], [Enable experimental fortran frontend development]))
AM_CONDITIONAL(ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION, [test "x$enable_experimental_fortran_frontend" = xyes])
if test "x$enable_experimental_fortran_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode enables experimental fortran front-end (internal development only)!])
  AC_DEFINE([ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION], [], [Enables development of experimental fortran frontend])
fi

# DQ (6/7/2013): Added support for debugging new Fortran front-end development.
AC_ARG_ENABLE(debug_output_for_experimental_fortran_frontend,
    AS_HELP_STRING([--enable-debug_output_for_experimental_fortran_frontend], [Enable debugging output (spew) of new OFP/ROSE connection]))
AM_CONDITIONAL(ROSE_DEBUG_EXPERIMENTAL_OFP_ROSE_CONNECTION, [test "x$enable_debug_output_for_experimental_fortran_frontend" = xyes])
if test "x$enable_debug_output_for_experimental_fortran_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode causes large volumes of output spew (internal debugging only)!])
  AC_DEFINE([ROSE_DEBUG_EXPERIMENTAL_OFP_ROSE_CONNECTION], [], [Controls large volumes of output spew useful for debugging new OFP/ROSE connection code])
fi

# Added support for Fortran front-end development using the flang (F18) compiler [Rasmussen 8/12/2019]
AC_ARG_ENABLE(experimental_flang_frontend,
    AS_HELP_STRING([--enable-experimental_flang_frontend], [Enable experimental fortran frontend development using flang]))
AM_CONDITIONAL(ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION, [test "x$enable_experimental_flang_frontend" = xyes])
if test "x$enable_experimental_flang_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode enables the experimental fortran flang front-end (internal development only)!])
  AC_DEFINE([ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION], [], [Enables development of experimental fortran flang frontend])
fi

# DQ (8/23/2017): Added support for new csharp front-end development.
AC_ARG_ENABLE(experimental_csharp_frontend,
    AS_HELP_STRING([--enable-experimental_csharp_frontend], [Enable experimental csharp frontend development]))
AM_CONDITIONAL(ROSE_EXPERIMENTAL_CSHARP_ROSE_CONNECTION, [test "x$enable_experimental_csharp_frontend" = xyes])
if test "x$enable_experimental_csharp_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode enables experimental csharp front-end (internal development only)!])
  AC_DEFINE([ROSE_EXPERIMENTAL_CSHARP_ROSE_CONNECTION], [], [Enables development of experimental csharp frontend])

# DQ (8/23/2017): Need to review now to get the MONO_HOME and ROSLYN_HOME environment variables.
# mono_home=$MONO_HOME
# rosyln_home=$ROSYLN_HOME

  mono_home=`roslyn-config mono-home`
  roslyn_home=`roslyn-config csharp-home`

  AC_MSG_NOTICE([Mono home   = "$mono_home"])
  AC_MSG_NOTICE([Roslyn home = "$rosyln_home"])

  AC_DEFINE_UNQUOTED([ROSE_EXPERIMENTAL_CSHARP_ROSE_CONNECTION_MONO_HOME], ["$mono_home"], [Path to Mono Csharp compiler used in development of experimental csharp frontend])
  AC_DEFINE_UNQUOTED([ROSE_EXPERIMENTAL_CSHARP_ROSE_CONNECTION_ROSLYN_HOME], ["$roslyn_home"], [Path to Roslyn Csharp library used in development of experimental csharp frontend])

  AC_SUBST(mono_home)
  AC_SUBST(rosyln_home)
fi

# DQ (6/7/2013): Added support for debugging new csharp front-end development.
AC_ARG_ENABLE(debug_output_for_experimental_csharp_frontend,
    AS_HELP_STRING([--enable-debug_output_for_experimental_csharp_frontend], [Enable debugging output (spew) of new CSHARP/ROSE connection]))
AM_CONDITIONAL(ROSE_DEBUG_EXPERIMENTAL_CSHARP_ROSE_CONNECTION, [test "x$enable_debug_output_for_experimental_csharp_frontend" = xyes])
if test "x$enable_debug_output_for_experimental_csharp_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode causes large volumes of output spew (internal debugging only)!])
  AC_DEFINE([ROSE_DEBUG_EXPERIMENTAL_CSHARP_ROSE_CONNECTION], [], [Controls large volumes of output spew useful for debugging new CSHARP/ROSE connection code])
fi

# DQ (8/23/2017): Added support for new Ada front-end development.
AC_ARG_ENABLE(experimental_ada_frontend,
    AS_HELP_STRING([--enable-experimental_ada_frontend], [Enable experimental ada frontend development]))
AM_CONDITIONAL(ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION, [test "x$enable_experimental_ada_frontend" = xyes])
if test "x$enable_experimental_ada_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode enables experimental ada front-end (internal development only)!])
  AC_DEFINE([ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION], [], [Enables development of experimental ada frontend])
fi

# DQ (6/7/2013): Added support for debugging new ada front-end development.
AC_ARG_ENABLE(debug_output_for_experimental_ada_frontend,
    AS_HELP_STRING([--enable-debug_output_for_experimental_ada_frontend], [Enable debugging output (spew) of new ADA/ROSE connection]))
AM_CONDITIONAL(ROSE_DEBUG_EXPERIMENTAL_ADA_ROSE_CONNECTION, [test "x$enable_debug_output_for_experimental_ada_frontend" = xyes])
if test "x$enable_debug_output_for_experimental_ada_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode causes large volumes of output spew (internal debugging only)!])
  AC_DEFINE([ROSE_DEBUG_EXPERIMENTAL_ADA_ROSE_CONNECTION], [], [Controls large volumes of output spew useful for debugging new ADA/ROSE connection code])
fi

# DQ (6/8/2017): Adding ADA support using GNAT ASIS tool chain.
ROSE_SUPPORT_GNAT

# Setup Automake conditional.
AM_CONDITIONAL(ROSE_USE_GNAT,test ! "$with_gnat" = no)

if test "x$enable_experimental_ada_frontend" = "xyes"; then
  gnat_home=$with_gnat
  AC_MSG_NOTICE([GNAT home = "$gnat_home"])
  AC_DEFINE_UNQUOTED([ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION_GNAT_HOME], ["$with_gnat"], [Path to GNAT Ada compiler used in development of experimental ada frontend])
  AC_SUBST(gnat_home)
fi

# DQ (8/23/2017): Added support for new Jovial front-end development.
AC_ARG_ENABLE(experimental_jovial_frontend,
    AS_HELP_STRING([--enable-experimental_jovial_frontend], [Enable experimental jovial frontend development]))
AM_CONDITIONAL(ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION, [test "x$enable_experimental_jovial_frontend" = xyes])
if test "x$enable_experimental_jovial_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode enables experimental jovial front-end (internal development only)!])
  AC_DEFINE([ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION], [], [Enables development of experimental jovial frontend])
fi

# DQ (8/23/2017): Added support for debugging new jovial front-end development.
AC_ARG_ENABLE(debug_output_for_experimental_jovial_frontend,
    AS_HELP_STRING([--enable-debug_output_for_experimental_jovial_frontend], [Enable debugging output (spew) of new JOVIAL/ROSE connection]))
AM_CONDITIONAL(ROSE_DEBUG_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION, [test "x$enable_debug_output_for_experimental_jovial_frontend" = xyes])
if test "x$enable_debug_output_for_experimental_jovial_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode causes large volumes of output spew (internal debugging only)!])
  AC_DEFINE([ROSE_DEBUG_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION], [], [Controls large volumes of output spew useful for debugging new JOVIAL/ROSE connection code])
fi

# DQ (8/23/2017): Added support for new Cobol front-end development.
AC_ARG_ENABLE(experimental_cobol_frontend,
    AS_HELP_STRING([--enable-experimental_cobol_frontend], [Enable experimental cobol frontend development]))
AM_CONDITIONAL(ROSE_EXPERIMENTAL_COBOL_ROSE_CONNECTION, [test "x$enable_experimental_cobol_frontend" = xyes])
if test "x$enable_experimental_cobol_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode enables experimental cobol front-end (internal development only)!])
  AC_DEFINE([ROSE_EXPERIMENTAL_COBOL_ROSE_CONNECTION], [], [Enables development of experimental cobol frontend])
fi

# DQ (8/23/2017): Added support for debugging new Cobol front-end development.
AC_ARG_ENABLE(debug_output_for_experimental_cobol_frontend,
    AS_HELP_STRING([--enable-debug_output_for_experimental_cobol_frontend], [Enable debugging output (spew) of new COBOL/ROSE connection]))
AM_CONDITIONAL(ROSE_DEBUG_EXPERIMENTAL_COBOL_ROSE_CONNECTION, [test "x$enable_debug_output_for_experimental_cobol_frontend" = xyes])
if test "x$enable_debug_output_for_experimental_cobol_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode causes large volumes of output spew (internal debugging only)!])
  AC_DEFINE([ROSE_DEBUG_EXPERIMENTAL_COBOL_ROSE_CONNECTION], [], [Controls large volumes of output spew useful for debugging new COBOL/ROSE connection code])
fi

# Rasmussen (10/30/2017): Added support for new Octave/Matlab front-end development.
AC_ARG_ENABLE([experimental_matlab_frontend],
    AS_HELP_STRING([--enable-experimental_matlab_frontend], [Enable experimental Octave/Matlab frontend development (default=no)]))
AM_CONDITIONAL(ROSE_EXPERIMENTAL_MATLAB_ROSE_CONNECTION, [test "x$enable_experimental_matlab_frontend" = xyes])
if test "x$enable_experimental_matlab_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode enables experimental Octave/Matlab front-end (internal development only)!])
  AC_DEFINE([ROSE_EXPERIMENTAL_MATLAB_ROSE_CONNECTION], [], [Enables development of experimental Octave/Matlab frontend])
fi

# Rasmussen (10/30/2017): Added support for debugging new Octave/Matlab front-end development.
AC_ARG_ENABLE(debug_output_for_experimental_matlab_frontend,
    AS_HELP_STRING([--enable-debug_output_for_experimental_matlab_frontend], [Enable debugging output (spew) of new Octave/Matlab ROSE connection]))
AM_CONDITIONAL(ROSE_DEBUG_EXPERIMENTAL_MATLAB_ROSE_CONNECTION, [test "x$enable_debug_output_for_experimental_matlab_frontend" = xyes])
if test "x$enable_debug_output_for_experimental_matlab_frontend" = "xyes"; then
  AC_MSG_WARN([using this mode causes large volumes of output spew (internal debugging only)!])
  AC_DEFINE([ROSE_DEBUG_EXPERIMENTAL_MATLAB_ROSE_CONNECTION], [], [Controls large volumes of output spew useful for debugging new Octave/Matlab ROSE connection code])
fi

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
  AC_MSG_WARN([using the new graph IR nodes in ROSE (experimental)!])
  AC_DEFINE([ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY], [], [Whether to use the new graph IR nodes compatability option with older API])
fi


# Set up for use of bison to build dot2gml tool in directory
# src/roseIndependentSupport/dot2gml.  This is made optional
# because it seems that many don't have the correct version of bison
# to support the compilation of this tool.  This is it is a configure
# option to build it (or have the makefile system have it be built).
AC_ARG_ENABLE(dot2gml_translator,
[  --enable-dot2gml_translator   Configure option to have DOT to GML translator built (bison version specific tool).],
[ AC_MSG_NOTICE([setting up optional DOT-to-GML translator in directory: src/roseIndependentSupport/dot2gml])
])
AM_CONDITIONAL(DOT_TO_GML_TRANSLATOR,test "$enable_dot2gml_translator" = yes)

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

# *****************************************************************

# DQ (12/3/2016): Added support for specification of specific warnings a for those specific warnings to be treated as errors.
# ROSE_SUPPORT_FATAL_WARNINGS

# *****************************************************************

# DQ (3/21/2017): Moved this to here (earlier than where is it used below) so that
# the warnings options can use the compiler vendor instead of the compiler name.
AC_LANG(C++)

# Get frontend compiler vendor
AX_COMPILER_VENDOR
FRONTEND_CXX_COMPILER_VENDOR="$ax_cv_cxx_compiler_vendor"

# echo "_AC_LANG_ABBREV              = $_AC_LANG_ABBREV"
# echo "ax_cv_c_compiler_vendor      = $ax_cv_c_compiler_vendor"
# echo "ax_cv_cxx_compiler_vendor    = $ax_cv_cxx_compiler_vendor"
AC_MSG_NOTICE([FRONTEND_CXX_COMPILER_VENDOR = "$FRONTEND_CXX_COMPILER_VENDOR"])

unset ax_cv_cxx_compiler_vendor

# Setup default options for C and C++ compilers compiling ROSE source code.
ROSE_FLAG_C_OPTIONS
ROSE_FLAG_CXX_OPTIONS

# echo "Exiting after computing the frontend compiler vendor"
# exit 1

# *****************************************************************

# DQ (11/14/2011): This is defined here since it must be seen before any processing of the rose_config.h file.
if test "x$enable_internalFrontendDevelopment" = "xyes"; then
  AC_MSG_NOTICE([adding -D to command line to support faster development of language frontend work])
  CFLAGS+=" -DROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT"
  CXXFLAGS+=" -DROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT"
fi

AC_MSG_NOTICE([CFLAGS   = "$CFLAGS"])
AC_MSG_NOTICE([CXXFLAGS = "$CXXFLAGS"])
AC_MSG_NOTICE([CPPFLAGS = "$CPPFLAGS"])

# *****************************************************************
#    Option to define a uniform debug level for ROSE development
# *****************************************************************

# DQ (10/17/2010): This defines an advanced level of uniform support for debugging and compiler warnings in ROSE.
AC_MSG_CHECKING([for enabled advanced warning support])
# Default is that advanced warnings is off, but this can be changed later so that advanced warnings would have to be explicitly turned off.
AC_ARG_ENABLE(advanced_warnings, AS_HELP_STRING([--enable-advanced-warnings], [Support for an advanced uniform warning level for ROSE development]),[enableval=yes],[enableval=no])
AM_CONDITIONAL(ROSE_USE_UNIFORM_ADVANCED_WARNINGS_SUPPORT, [test "x$enable_advanced_warnings" = xyes])
if test "x$enable_advanced_warnings" = "xyes"; then
  AC_MSG_WARN([using an advanced uniform warning level for ROSE development])
  AC_DEFINE([ROSE_USE_UNIFORM_ADVANCED_WARNINGS_SUPPORT], [], [Support for an advanced uniform warning level for ROSE development])

# Suggested C++ specific flags (used to be run before Hudson, but fail currently).
  CXX_ADVANCED_WARNINGS+=" -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_DEBUG"

# Additional flag (suggested by George).
  CXX_ADVANCED_WARNINGS+=" -D_GLIBCXX_DEBUG_PEDANTIC"

# Incrementally add the advanced options
  if test "$CXX_ADVANCED_WARNINGS"; then CXXFLAGS="$CXXFLAGS $CXX_ADVANCED_WARNINGS"; fi
fi
# ROSE_USE_UNIFORM_DEBUG_SUPPORT=7
AC_SUBST(ROSE_USE_UNIFORM_ADVANCED_WARNINGS_SUPPORT)

AC_MSG_NOTICE([after processing --enable-advanced-warnings: CXX_ADVANCED_WARNINGS = "${CXX_ADVANCED_WARNINGS}"])
AC_MSG_NOTICE([after processing --enable-advanced-warnings: CXX_WARNINGS = "${CXX_WARNINGS}"])
AC_MSG_NOTICE([after processing --enable-advanced-warnings: C_WARNINGS   = "${C_WARNINGS}"])

AC_MSG_NOTICE([CFLAGS   = "$CFLAGS"])
AC_MSG_NOTICE([CXXFLAGS = "$CXXFLAGS"])
AC_MSG_NOTICE([CPPFLAGS = "$CPPFLAGS"])

# echo "Exiting in support after enabled advanced warnings"
# exit 1

# *****************************************************************

# DQ: added here to see if it would be defined for the template tests and avoid placing
# a $(CXX_TEMPLATE_REPOSITORY_PATH) directory in the top level build directory (a minor error)
CXX_TEMPLATE_REPOSITORY_PATH='$(top_builddir)/src'

# *****************************************************************

AC_ARG_ENABLE(assertion-behavior,
    AS_HELP_STRING([--enable-assertion-behavior[=MODE]],
        [Specifies the default behavior for failing ROSE assertions. This behavior can be changed at runtime either
         via ROSE command-line switches or the rose API. Most developers (the ROSE team and users that
         are developing transformations) will probably want "abort" since this gives the most useful post-mortem
         information. On the other hand, end users usually don't need or expect post-mortem capabilities and sometimes
         even perceive them as low code quality, in which case "exit" with non-zero status is the best behavior. Finally,
         the "throw" behavior can be used as a compromise for tool developers to more gracefully recover from a ROSE
         error that would otherwise be fatal.  When --enable-assertion-behavior is not specified then "exit" is used.
         Some assertions can be disabled altogether (e.g., when an optimized library is desired) by defining NDEBUG.
         Caveats: this switch affects the behavior of the ROSE_ASSERT macro and the Sawyer ASSERT_* macros, but not
         plain old "assert"; the NDEBUG define applies to Sawyer ASSERT_* macros and plain old "assert" but not
         ROSE_ASSERT.]))

case "$enable_assertion_behavior" in
    abort)    assertion_behavior=ROSE_ASSERTION_ABORT ;;
    exit|"")  assertion_behavior=ROSE_ASSERTION_EXIT ;;
    throw)    assertion_behavior=ROSE_ASSERTION_THROW ;;
    *)
        AC_MSG_ERROR([--enable-assertion-behavior should be "abort", "exit", or "throw"])
        ;;
esac

AC_DEFINE_UNQUOTED([ROSE_ASSERTION_BEHAVIOR], [$assertion_behavior], [Determines how failed assertions should behave.])

# *****************************************************************

# ********************************************************************************
#    Option support for the Address Sanitizer and other related Sanitizer tools.
# ********************************************************************************

ROSE_SUPPORT_SANITIZER

# *****************************************************************


# ********************************************************************************
#    Option support for the Linux Coverage Test tools.
# ********************************************************************************

ROSE_SUPPORT_LCOV

# *****************************************************************
# ROSE_HOME should be relative to top_srcdir or top_builddir.
ROSE_HOME=.
# ROSE_HOME=`pwd`/$top_srcdir
AC_SUBST(ROSE_HOME)
# echo "In ROSE/configure: ROSE_HOME = $ROSE_HOME"

# This does not appear to exist any more (not distributed in ROSE)
# Support for Gabriel's QRose GUI Library
# ROSE_SUPPORT_QROSE
#AM_CONDITIONAL(ROSE_USE_QROSE,test "$with_qrose" = true)

AC_LANG(C++)

# Python check must occur before boost check since -lboost_python depends on python
ROSE_SUPPORT_PYTHON

ROSE_SUPPORT_BOOST

# Rasmussen (12/16/2017): Added test for Bison version (Mac OSX Bison version may be too old)
ROSE_SUPPORT_BISON

# DQ (11/5/2009): Added test for GraphViz's ``dot'' program
ROSE_SUPPORT_GRAPHVIZ

AX_LIB_SQLITE3
AX_LIB_MYSQL
AM_CONDITIONAL(ROSE_USE_MYSQL,test "$found_mysql" = yes)

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

# *****************************************************************

# Calling available macro from Autoconf (test by optionally pushing C language onto the internal autoconf language stack).
# This function must be called from this support-rose file (error in ./build if called from the GET COMPILER SPECIFIC DEFINES macro.
# AC_LANG_PUSH(C)

# Get frontend compiler vendor
AX_COMPILER_VENDOR
FRONTEND_CXX_COMPILER_VENDOR="$ax_cv_cxx_compiler_vendor"
unset ax_cv_cxx_compiler_vendor

# Get backend compiler vendor
  saved_compiler_name=$CXX
  CXX=$BACKEND_CXX_COMPILER
  AC_MSG_NOTICE([after resetting CXX to be the backend compiler: CXX = "$CXX"])

  AX_COMPILER_VENDOR
# returns string ax_cv_cxx_compiler_vendor if this is the C++ compiler else returns
# the vendor for the C compiler in ax_cv_c_compiler_vendor for the C compiler.
# CcompilerVendorName= $ax_cv_c_compiler_vendor
# CxxcompilerVendorName= $ax_cv_cxx_compiler_vendor
# echo "Output the names of the vendor for the C or C++ backend compilers."
# echo "Using back-end C   compiler = \"$BACKEND_CXX_COMPILER\" compiler vendor name = $ax_cv_c_compiler_vendor   for processing of unparsed source files from ROSE preprocessors."
  AC_MSG_NOTICE([using back-end C++ compiler = "$BACKEND_CXX_COMPILER" compiler vendor name = $ax_cv_cxx_compiler_vendor for processing of unparsed source files from ROSE preprocessors])
  BACKEND_CXX_COMPILER_VENDOR="$ax_cv_cxx_compiler_vendor"

  CXX=$saved_compiler_name
  AC_MSG_NOTICE([after resetting CXX to be the saved name of the original compiler: CXX = "$CXX"])

AC_MSG_NOTICE([FRONTEND_CXX_COMPILER_VENDOR = "$FRONTEND_CXX_COMPILER_VENDOR"])

# echo "Exiting after computing the backend compiler vendor"
# exit 1

# *****************************************************************

# DQ (2/27/2016): Added version 4.9.x to supported compilers.
AC_MSG_CHECKING([whether your compiler is a GNU compiler and the version that is supported by ROSE (4.0.x - 6.3.x)])
AC_ARG_ENABLE([gcc-version-check],AS_HELP_STRING([--disable-gcc-version-check],[Disable GCC version 4.0.x - 6.3.x verification check]),,[enableval=yes])
if test "x$FRONTEND_CXX_COMPILER_VENDOR" = "xgnu" ; then
if test "x$enableval" = "xyes" ; then
      AC_LANG_PUSH([C])
      # http://www.gnu.org/s/hello/manual/autoconf/Running-the-Compiler.html
      AC_COMPILE_IFELSE([
        AC_LANG_SOURCE([[
          #if (__GNUC__ >= 4 && __GNUC_MINOR__ <= 9)
            int rose_supported_gcc;
          #else
            not gcc, or gcc version is not supported by rose
          #endif
        ]])
       ],
       [AC_MSG_RESULT([done])],
       gcc_version=`gcc -dumpversion`
       [AC_MSG_FAILURE([your GCC $gcc_version version is currently NOT supported by ROSE; GCC 4.0.x to 4.8.x is supported now])])
      AC_LANG_POP([C])
else
    AC_MSG_RESULT([skipping])
fi
else
    AC_MSG_RESULT([not a GNU compiler])
fi

# *****************************************************************

# DQ (2/7/17): This is a problem reported by Robb (sometimes gcc is not installed).
# This is used in EDG (host_envir.h)  Test by building a bad version of gcc
# use shell script called gcc with "exit 1" inside.
if test "x$FRONTEND_CXX_COMPILER_VENDOR" = "xgnu" ; then
   GCC_VERSION=`gcc -dumpversion | cut -d\. -f1`
   GCC_MINOR_VERSION=`gcc -dumpversion | cut -d\. -f2`

   AC_MSG_NOTICE([initial compiler version test: GCC_VERSION = "$GCC_VERSION"])
   AC_MSG_NOTICE([initial compiler version test: GCC_MINOR_VERSION = "$GCC_MINOR_VERSION"])

   AC_SUBST(GCC_VERSION)
   AC_SUBST(GCC_MINOR_VERSION)
else
 # DQ (2/8/2017): Default configuration of EDG will behave like GNU 4.8.x (unclear if this is idea).
   GCC_VERSION=4
   GCC_MINOR_VERSION=8
fi

# echo "Exiting after test for GNU compiler and setting the version info for EDG (GCC_VERSION and GCC_MINOR_VERSION)."
# exit 1

# *****************************************************************

# DQ (2/7/2017): These macros test for C++11 and C++14 features and
# the default behavior of the CXX compiler.  Unfortunately the also
# modify the CXX value so we have to save it and reset it after the
# macros are called.  We modified the macros as well to save the
# default behavior of the CXX compiler so that we can detect C++11
# mode within the frontend compiler used to compile ROSE.  Thi is used
# mostly so far to just disable some test that are causing GNU g++
# version 4.8.x internal errors (because the C++11 support is new).

AC_MSG_NOTICE([before checking C++11 support: CXX = $CXX CXXCPP = "$CXXCPP"])

AC_MSG_NOTICE([calling AX CXX COMPILE STDCXX 11 macro])
save_CXX="$CXX"
AX_CXX_COMPILE_STDCXX_11(, optional)

AC_MSG_NOTICE([after checking C++11 support: CXX = "$CXX", CXXCPP = "$CXXCPP"])

AC_MSG_NOTICE([rose_frontend_compiler_default_is_cxx11_success = "$rose_frontend_compiler_default_is_cxx11_success"])
AC_MSG_NOTICE([gcc_version_4_8                                 = "$gcc_version_4_8"])

AM_CONDITIONAL(ROSE_USING_GCC_VERSION_4_8_CXX11, [test "x$gcc_version_4_8" = "xyes" && test "x$rose_frontend_compiler_default_is_cxx11_success" = "xyes"])

AC_MSG_NOTICE([calling AX CXX COMPILE STDCXX 14 macro])
AX_CXX_COMPILE_STDCXX_14(, optional)

AC_MSG_NOTICE([after checking C++14 support: CXX = "$CXX", CXXCPP = "$CXXCPP"])
CXX="$save_CXX"

AC_MSG_NOTICE([after restoring the saved value of CXX: CXX = "$CXX", CXXCPP = "$CXXCPP"])

# echo "Exiting in support-rose after computing the C++ mode (c++11, and c++14 modes)"
# exit 1

# *****************************************************************

# DQ (12/7/2016): Added support for specification of specific warnings a for those specific warnings to be treated as errors.
ROSE_SUPPORT_FATAL_WARNINGS

# *****************************************************************

# echo "Exiting in support-rose after computing the compiler vendor name for the C and C++ compilers."
# exit 1

# End macro ROSE_SUPPORT_ROSE_PART_1.
]
)



AC_DEFUN([ROSE_SUPPORT_ROSE_BUILD_INCLUDE_FILES],
[
# Begin macro ROSE_SUPPORT_ROSE_BUILD_INCLUDE_FILES.

AC_MSG_NOTICE([in ROSE SUPPORT ROSE BUILD INCLUDE FILES: Using back-end C++ compiler = "$BACKEND_CXX_COMPILER" compiler vendor name = "$ax_cv_cxx_compiler_vendor" for processing of unparsed source files from ROSE preprocessors])

# Note that this directory name is not spelled correctly, is this a typo?
# JJW (12/10/2008): We don't preprocess the header files for the new interface
rm -rf ./include-stagin

if test x$enable_clang_frontend = xyes; then
  INSTALL_CLANG_SPECIFIC_HEADERS
else
  # DQ (11/1/2011): I think that we need these for more complex header file
  # requirements than we have seen in testing C code to date.  Previously
  # in testing C codes with the EDG 4.x we didn't need as many header files.
  GENERATE_BACKEND_C_COMPILER_SPECIFIC_HEADERS
  GENERATE_BACKEND_CXX_COMPILER_SPECIFIC_HEADERS
fi

# End macro ROSE_SUPPORT_ROSE_BUILD_INCLUDE_FILES.
]
)


#-----------------------------------------------------------------------------

AC_DEFUN([ROSE_SUPPORT_ROSE_PART_2],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_2.

# DQ (9/26/2015): Since the config/ltdl.m4 file in regenerated, we can't edit it easily.
# So make this a requirement so that it will not be expanded there.
m4_require([_LT_SYS_DYNAMIC_LINKER])

# AC_REQUIRE([AC_PROG_CXX])
AC_PROG_CXX

AC_MSG_NOTICE([in configure.in ... CXX = "$CXX"])

# DQ (9/17/2006): These should be the same for both C and C++ (else we will need separate macros)
# Setup the -D<xxx> defines required to allow EDG to take the same path through the compiler
# specific and system specific header files as for the backend compiler.  These depend
# upon the selection of the back-end compiler.
GET_COMPILER_SPECIFIC_DEFINES
ROSE_CONFIG_TOKEN="$ROSE_CONFIG_TOKEN $FRONTEND_CXX_COMPILER_VENDOR-$FRONTEND_CXX_VERSION_MAJOR.$FRONTEND_CXX_VERSION_MINOR"

# This must go after the setup of the headers options
# Setup the CXX_INCLUDE_STRING to be used by EDG to find the correct headers
# SETUP_BACKEND_COMPILER_SPECIFIC_REFERENCES
# JJW (12/10/2008): We don't preprocess the header files for the new interface,
# but we still need to use the original C++ header directories
ROSE_CONFIGURE_SECTION([Checking backend C/C++ compiler specific references])
SETUP_BACKEND_C_COMPILER_SPECIFIC_REFERENCES
SETUP_BACKEND_CXX_COMPILER_SPECIFIC_REFERENCES

# echo "In configure.in ... CXX = $CXX : exiting after call to setup backend C and C++ compilers specific references."
# exit 1

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

# Check for availability of wget (used for downloading the EDG binaries used in ROSE).
AC_CHECK_TOOL(ROSE_WGET_PATH, [wget], [no])
AM_CONDITIONAL(ROSE_USE_WGET, [test "$ROSE_WGET_PATH" != "no"])
if test "$ROSE_WGET_PATH" = "no"; then
   AC_MSG_FAILURE([wget was not found; ROSE requires wget to download EDG binaries automatically])
else
   # Not clear if we really should have ROSE configure automatically do something like this.
   AC_MSG_NOTICE([ROSE might use wget to automatically download EDG binaries as required during the build])
fi
# Check for availability of ps2pdf, part of ghostscript (used for generating pdf files).
AC_CHECK_TOOL(ROSE_PS2PDF_PATH, [ps2pdf], [no])
AM_CONDITIONAL(ROSE_USE_PS2PDF, [test "$ROSE_PS2PDF_PATH" != "no"])
if test "$ROSE_PS2PDF_PATH" = "no"; then
   AC_MSG_FAILURE([ps2pdf was not found; ROSE requires ps2pdf (part of ghostscript) to generate pdf files])
fi

AC_C_BIGENDIAN
AC_CHECK_HEADERS([byteswap.h machine/endian.h])

ROSE_SUPPORT_VALGRIND

AC_ARG_WITH(wave-default, [  --with-wave-default     Use Wave as the default preprocessor],
            [AC_DEFINE([ROSE_WAVE_DEFAULT], true, [Use Wave as default in ROSE])],
            [AC_DEFINE([ROSE_WAVE_DEFAULT], false, [Simple preprocessor as default in ROSE])]
            )

# Add --disable-binary-analysis-tests flag to turn off tests that sometimes
# sometimes break.
# Pei-Hung (10/24/2016) use only ROSE_BUILD_BINARY_ANALYSIS_SUPPORT to control binary analysis tests
# AC_ARG_ENABLE(binary-analysis-tests, AS_HELP_STRING([--disable-binary-analysis-tests], [Disable tests of ROSE binary analysis code]), binary_analysis_tests="$withval", binary_analysis_tests=yes)
# AM_CONDITIONAL(USE_BINARY_ANALYSIS_TESTS, test "x$binary_analysis_tests" = "xyes")

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

# DQ (9/21/2009): Debugging for RH release 5
AC_MSG_NOTICE([testing the value of CC: (CC = "$CC")])
AC_MSG_NOTICE([testing the value of CPPFLAGS: (CPPFLAGS = "$CPPFLAGS")])

# Call supporting macro for MAPLE
ROSE_SUPPORT_MAPLE

# Setup Automake conditional in Projects/programModeling/Makefile.am
AM_CONDITIONAL(ROSE_USE_MAPLE,test ! "$with_maple" = no)

# DQ (5/21/2017): I don't think we use this anymore.
#ROSE_SUPPORT_VXWORKS

# Setup Automake conditional.
AM_CONDITIONAL(ROSE_USE_VXWORKS,test ! "$with_vxworks" = no)

# DQ (5/23/2017): I don't think we use this anymore.
# DQ (4/10/2010): Added configure support for Backstroke project.
# ROSE_SUPPORT_BACKSTROKE

#Call supporting macro for IDA PRO
ROSE_SUPPORT_IDA

# Setup Automake conditional in projects/AstEquivalence/Makefile.am
AM_CONDITIONAL(ROSE_USE_IDA,test ! "$with_ida" = no)

# Call supporting macro for libffi (Foreign Function Interface library)
# This library is used by Peter's work on the Interpreter in ROSE.
ROSE_SUPPORT_LIBFFI

# Setup Automake conditional in projects/interpreter/Makefile.am
AM_CONDITIONAL(ROSE_USE_LIBFFI,test ! "$with_libffi" = no)


# DQ (3/13/2009): Trying to get Intel Pin and ROSE to both use the same version of libdwarf.
# DQ (3/10/2009): The Dwarf support in Intel Pin conflicts with the Dwarf support in ROSE.
# Maybe there is a way to fix this later, for now we want to disallow it.
# echo "with_dwarf    = $with_dwarf"
# echo "with_IntelPin = $with_IntelPin"
#if test "$with_dwarf" != no && test "$with_IntelPin" != no; then
# # echo "Support for both DWARF and Intel Pin fails, these configure options are incompatable."
#   AC_MSG_ERROR([Support for both DWARF and Intel Pin fails, these configure options are incompatable!])
#fi

# DQ (3/14/2013): Adding support for Aterm library use in ROSE.
ROSE_SUPPORT_ATERM

# DQ (1/22/2016): Added support for stratego (need to know the path to sglri executable for Experimental Fortran support).
ROSE_SUPPORT_STRATEGO

# RASMUSSEN (11/16/2017): Removed check for OFP Stratego tools binary installation (Experimental Fortran support).
# Now assumes that the OFP Fortran parse table (Fortran.tbl) is stored in the source directory.
# RASMUSSEN (2/22/2017): Added support for OFP Stratego tools binary installation (Experimental Fortran support).
# This assumes that OFP is installed from an OFP release and not imported and built with ROSE directly.

#ROSE_SUPPORT_OFP_STRATEGO

if test "x$enable_experimental_fortran_frontend" = "xyes"; then
   if test "x$ATERM_LIBRARY_PATH" = "x"; then
      AC_MSG_ERROR([support for experimental_fortran_frontend requires Aterm library support, --with-aterm=PATH must be specified])
   fi
   if test "x$STRATEGO_LIBRARY_PATH" = "x"; then
      AC_MSG_ERROR([support for experimental_fortran_frontend requires Stratego library support, --with-stratego=PATH must be specified])
   fi
fi

# Rasmussen (10/24/2017): ATerm and Stratego/XT tools binary installation required for Jovial support.
if test "x$enable_experimental_jovial_frontend" = "xyes"; then
   if test "x$ATERM_LIBRARY_PATH" = "x"; then
      AC_MSG_ERROR([support for experimental_jovial_frontend requires Aterm library support, --with-aterm=PATH must be specified!])
   fi
   if test "x$STRATEGO_LIBRARY_PATH" = "x"; then
      AC_MSG_ERROR([support for experimental_jovial_frontend requires Stratego library support, --with-stratego=PATH must be specified!])
   fi
fi

# Rasmussen (10/24/2017): GnuCobol parse-tree library installation required for Cobol support.
if test "x$enable_experimental_cobol_frontend" = "xyes"; then
   if test "x$COBPT_LIBRARY_PATH" = "x"; then
      AC_MSG_ERROR([support for experimental_cobol_frontend requires GnuCobol parse-tree library support, --with-cobpt=PATH must be specified!])
   fi
fi

# Rasmussen (11/19/2017): Octave/Matlab parser installation required for Matlab support.
if test "x$enable_experimental_matlab_frontend" = "xyes"; then
   if test "x$OCTAVE_PARSER_INSTALL_TARFILE" = "x"; then
      AC_MSG_ERROR([support for experimental_matlab_frontend requires the modified GNU Octave parser, --with-octave-parser=PATH must be specified!])
   fi
fi


ROSE_SUPPORT_MINT

ROSE_SUPPORT_VECTORIZATION

# Pei-Hung (12/17/2014): Adding support for POCC.
ROSE_SUPPORT_POCC

ROSE_SUPPORT_LIBHARU

ROSE_SUPPORT_PHP

AM_CONDITIONAL(ROSE_USE_PHP,test ! "$with_php" = no)

#ASR
ROSE_SUPPORT_LLVM

AM_CONDITIONAL(ROSE_USE_LLVM,test ! "$with_llvm" = no)

# Call supporting macro for Windows Source Code Analysis
ROSE_SUPPORT_WINDOWS_ANALYSIS

# Setup Automake conditional in Projects/programModeling/Makefile.am
AM_CONDITIONAL(ROSE_USE_WINDOWS_ANALYSIS_SUPPORT,test ! "$with_wine" = no)

# Control use of debugging support to convert most unions in EDG to structs.
ROSE_SUPPORT_EDG_DEBUGGING

# Call supporting macro for Omni OpenMP
#
ROSE_SUPPORT_OMNI_OPENMP

# call supporting macro for GCC 4.4.x gomp OpenMP runtime library
# AM_CONDITIONAL is already included into the macro
ROSE_WITH_GOMP_OPENMP_LIBRARY

# Call supporting macro for GCC OpenMP
ROSE_SUPPORT_GCC_OMP

# Configuration commandline support for OpenMP in ROSE
AM_CONDITIONAL(ROSE_USE_GCC_OMP,test ! "$with_parallel_ast_traversal_omp" = no)


# JJW and TP (3-17-2008) -- added MPI support
AC_ARG_WITH(parallel_ast_traversal_mpi,
[  --with-parallel_ast_traversal_mpi     Enable AST traversal in parallel using MPI.],
[ AC_MSG_NOTICE([setting up optional MPI-based tools])
])
AM_CONDITIONAL(ROSE_MPI,test "$with_parallel_ast_traversal_mpi" = yes)
AC_CHECK_TOOLS(MPICXX, [mpiCC mpic++ mpicxx])


# TPS (2-11-2009) -- added PCH Support
AC_ARG_WITH(pch,
[  --with-pch                    Configure option to have pre-compiled header support enabled.],
[ AC_MSG_NOTICE([enabling precompiled header])
])
AM_CONDITIONAL(ROSE_PCH,test "$with_pch" = yes)
if test "x$with_pch" = xyes; then
  AC_MSG_NOTICE(["PCH enabled: CPPFLAGS = "$CPPFLAGS"])
  if test "x$with_parallel_ast_traversal_mpi" = xyes; then
    AC_MSG_ERROR([PCH support cannot be configured together with MPI support])
  fi
  if test "x$with_parallel_ast_traversal_omp" = xyes; then
    AC_MSG_ERROR([PCH Support cannot be configured together with GCC_OMP support])
  fi
else
  AC_MSG_NOTICE("PCH disabled: no Support for PCH")
fi





# TP (2-27-2009) -- support for RTED
ROSE_SUPPORT_RTED

AM_CONDITIONAL(ROSE_USE_RTED,test ! "$with_rted" = no)

# TP SUPPORT FOR OPENGL
#AC_DEFINE([openGL],1,[By default OpenGL is disabled.])
AC_ARG_ENABLE([rose-openGL],
  [  --enable-rose-openGL  enable openGL],
  [  rose_openGL=${enableval}
AC_PATH_X dnl We need to do this by hand for some reason

# DQ (9/26/2015): Using more recent autoconf macro to avoid warnings.
# MDL_HAVE_OPENGL
AC_FIND_OPENGL

AC_MSG_NOTICE([have_GL = "$have_GL" and have_glut = "$have_glut" and rose_openGL = "$rose_openGL"])
#AM_CONDITIONAL(ROSE_USE_OPENGL, test ! "x$have_GL" = xno -a ! "x$openGL" = xno)
if test ! "x$rose_openGL" = xno; then
   AC_MSG_NOTICE([checking OpenGL dependencies..."])
  if test "x$have_GL" = xyes; then
    AC_MSG_NOTICE([OpenGL enabled: found OpenGL])
  else
    AC_MSG_ERROR([OpenGL not found])
  fi
 if test "x$have_glut" = xyes; then
    AC_MSG_NOTICE([OpenGL enabled: found GLUT])
 else
   AC_MSG_NOTICE([OpenGL GLUT not found; please use --with-glut])
 fi
fi
], [ rose_openGL=no
  AC_MSG_NOTICE([OpenGL disabled])
])
AM_CONDITIONAL(ROSE_USE_OPENGL, test ! "x$have_GL" = xno -a ! "x$rose_openGL" = xno)


AM_CONDITIONAL(USE_ROSE_GLUT_SUPPORT, false)

AC_ARG_WITH(glut,
[  --with-glut=PATH     Configure option to have GLUT enabled.],
,
if test ! "$with_glut" ; then
   with_glut=no
fi
)

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_glut = "$with_glut"])

if test "$with_glut" = no; then
   # If dwarf is not specified, then don't use it.
   AC_MSG_NOTICE([skipping use of GLUT support])
else
   AM_CONDITIONAL(USE_ROSE_GLUT_SUPPORT, true)
   glut_path=$with_glut
   AC_MSG_NOTICE([setup GLUT support in ROSE. path = "$glut_path"])
   AC_DEFINE([USE_ROSE_GLUT_SUPPORT],1,[Controls use of ROSE support for GLUT library.])
fi


AC_SUBST(glut_path)



AC_CHECK_PROGS(PERL, [perl])

# DQ (9/4/2009): Added checking for indent command (common in Linux, but not on some platforms).
# This command is used in the tests/nonsmoke/functional/roseTests/astInterfaceTests/Makefile.am file.
AC_CHECK_PROGS(INDENT, [indent])
AM_CONDITIONAL(ROSE_USE_INDENT, [test "x$INDENT" = "xindent"])
AC_MSG_NOTICE([INDENT = "$INDENT"])

# DQ (9/30/2009): Added checking for tclsh command (common in Linux, but not on some platforms).
AC_CHECK_PROGS(TCLSH, [tclsh])
AM_CONDITIONAL(ROSE_USE_TCLSH, [test "x$TCLSH" = "xtclsh"])
AC_MSG_NOTICE([TCLSH = "$TCLSH"])

# Call supporting macro for OFP
ROSE_SUPPORT_OFP

# DQ (3/6/2013): The major version number must match or the ac_pkg_swig.m4 will report
# we are using the wrong version of swig (likely we need a newer version of this m4 script).
# AC_PROG_SWIG(1.3.31)
AC_PROG_SWIG(2.0.0)
SWIG_ENABLE_CXX
#AS (10/23/07): introduced conditional use of javaport
AC_ARG_WITH([javaport],
   [  --with-javaport ... Enable generation of Java bindings for ROSE using Swig],
   [with_javaport=$withval],
   [with_javaport=no])
AM_CONDITIONAL(ENABLE_JAVAPORT,test "$with_javaport" = yes)

if test "x$with_javaport" = "xyes"; then
  if test "x$USE_JAVA" = "x0"; then
    AC_MSG_ERROR([trying to enable --with-javaport without --with-java also being set])
  fi
  if /bin/sh -c "$SWIG -version" >& /dev/null; then
    :
  else
    AC_MSG_ERROR([trying to enable --with-javaport without SWIG installed])
  fi
  AC_MSG_WARN([enabling Java binding support -- SWIG produces invalid C++ code, so -fno-strict-aliasing is being added to CXXFLAGS to work around this issue.  If you are not using GCC as a compiler, this flag will need to be changed.])
  CXXFLAGS="$CXXFLAGS -fno-strict-aliasing"

# DQ (3/6/2013): Added support to permit conditional compilation for use of SWIG.
# SWIG has restricted support for C++ and so we need to tailor ROSE to fix into
# the subset of C++ that SWIG can support.  We only want to turn this ON when SWIG
# is processing the ROSE source code.  So it need not generate an entry in rose_config.h.
# AC_DEFINE([ROSE_USE_SWIG_SUPPORT], [], [Whether to use SWIG support or not within ROSE])
fi

# Call supporting macro for Haskell
ROSE_SUPPORT_HASKELL

ROSE_SUPPORT_CUDA

# if swi-prolog is available
ROSE_SUPPORT_SWIPL

# Call support macro for Z3

ROSE_SUPPORT_Z3

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

# *********************************************************************
# Option to control internal support of PPL (Parma Polyhedron Library)
# *********************************************************************

# TV (05/25/2010): Check for Parma Polyhedral Library (PPL)

AC_ARG_WITH(
        [ppl],
        AS_HELP_STRING([--with-ppl@<:@=DIR@:>@], [use Parma Polyhedral Library (PPL)]),
        [
        if test "$withval" = "no"; then
      AC_MSG_FAILURE([--with-ppl=PATH must be specified to use option --with-ppl (a valid Parma Polyhedral Library (PPL) intallation)])
        elif test "$withval" = "yes"; then
      AC_MSG_FAILURE([--with-ppl=PATH must be specified to use option --with-ppl (a valid Parma Polyhedral Library (PPL) intallation)])
        else
            has_ppl_path="yes"
            ppl_path="$withval"
        fi
        ],
        [has_ppl_path="no"]
)

AC_ARG_ENABLE(
        ppl,
        AS_HELP_STRING(
                [--enable-ppl],
                [Support for Parma Polyhedral Library (PPL)]
        )
)
AM_CONDITIONAL(
        ROSE_USE_PPL,
        [test "x$enable_ppl" = "xyes"])
if test "x$enable_ppl" = "xyes"; then
        if test "x$has_ppl_path" = "xyes"; then
                PPL_PATH="$ppl_path"
                AC_DEFINE([ROSE_USE_PPL], [], [Whether to use Parma Polyhedral Library (PPL) support or not within ROSE])
        fi
fi
AC_SUBST(ROSE_USE_PPL)
AC_SUBST(PPL_PATH)

# *********************************************************************************
# Option to control internal support of Cloog (Code generator for Polyhedral Model)
# *********************************************************************************

AC_ARG_WITH(
        [cloog],
        AS_HELP_STRING([--with-cloog@<:@=DIR@:>@], [use Cloog]),
        [
        if test "$withval" = "no"; then
      AC_MSG_FAILURE([--with-cloog=PATH must be specified to use option --with-cloog (a valid Cloog intallation)])
        elif test "$withval" = "yes"; then
            AC_MSG_FAILURE([--with-cloog=PATH must be specified to use option --with-cloog (a valid Cloog intallation)])
        else
            has_cloog_path="yes"
            cloog_path="$withval"
        fi
        ],
        [has_cloog_path="no"]
)

AC_ARG_ENABLE(
        cloog,
        AS_HELP_STRING(
                [--enable-cloog],
                [Support for Cloog]
        )
)
AM_CONDITIONAL(
        ROSE_USE_CLOOG,
        [test "x$enable_cloog" = "xyes"])
if test "x$enable_cloog" = "xyes"; then
        if test "x$has_cloog_path" = "xyes"; then
                CLOOG_PATH="$cloog_path"
                AC_DEFINE([ROSE_USE_CLOOG], [], [Whether to use Cloog support or not within ROSE])
        fi
fi
AC_SUBST(ROSE_USE_CLOOG)
AC_SUBST(CLOOG_PATH)

# **************************************************************************************
# Option to control internal support of ScopLib (A classic library for Polyhedral Model)
# **************************************************************************************

AC_ARG_WITH(
        [scoplib],
        AS_HELP_STRING([--with-scoplib@<:@=DIR@:>@], [use ScopLib]),
        [
        if test "$withval" = "no"; then
      AC_MSG_FAILURE([--with-scoplib=PATH must be specified to use option --with-scoplib (a valid ScopLib intallation)])
        elif test "$withval" = "yes"; then
      AC_MSG_FAILURE([--with-scoplib=PATH must be specified to use option --with-scoplib (a valid ScopLib intallation)])
        else
            has_scoplib_path="yes"
            scoplib_path="$withval"
        fi
        ],
        [has_scoplib_path="no"]
)

AC_ARG_ENABLE(
        scoplib,
        AS_HELP_STRING(
                [--enable-scoplib],
                [Support for ScopLib]
        )
)
AM_CONDITIONAL(
        ROSE_USE_SCOPLIB,
        [test "x$enable_scoplib" = "xyes"])
if test "x$enable_scoplib" = "xyes"; then
        if test "x$has_scoplib_path" = "xyes"; then
                SCOPLIB_PATH="$scoplib_path"
                AC_DEFINE([ROSE_USE_SCOPLIB], [], [Whether to use ScopLib support or not within ROSE])
        fi
fi
AC_SUBST(ROSE_USE_SCOPLIB)
AC_SUBST(SCOPLIB_PATH)

# *************************************************************************************
# Option to control internal support of Candl (Dependency analysis in Polyhedral Model)
# *************************************************************************************

AC_ARG_WITH(
        [candl],
        AS_HELP_STRING([--with-candl@<:@=DIR@:>@], [use Candl]),
        [
        if test "$withval" = "no"; then
            AC_MSG_FAILURE([--with-candl=PATH must be specified to use option --with-candl (a valid Candl intallation)])
        elif test "$withval" = "yes"; then
            AC_MSG_FAILURE([--with-candl=PATH must be specified to use option --with-candl (a valid Candl intallation)])
        else
            has_candl_path="yes"
            candl_path="$withval"
        fi
        ],
        [has_candl_path="no"]
)

AC_ARG_ENABLE(
        candl,
        AS_HELP_STRING(
                [--enable-candl],
                [Support for Candl]
        )
)
AM_CONDITIONAL(
        ROSE_USE_CANDL,
        [test "x$enable_candl" = "xyes"])
if test "x$enable_candl" = "xyes"; then
        if test "x$has_candl_path" = "xyes"; then
                CANDL_PATH="$candl_path"
                AC_DEFINE([ROSE_USE_CANDL], [], [Whether to use Candl support or not within ROSE])
        fi
fi
AC_SUBST(ROSE_USE_CANDL)
AC_SUBST(CANDL_PATH)

# *****************************************************************
#            Accelerator Support (CUDA, OpenCL)
# *****************************************************************

# Check: --with-cuda-inc, --with-cuda-lib, and  --with-cuda-bin
ROSE_CHECK_CUDA
# Check: --with-opencl-inc, --with-opencl-lib
ROSE_CHECK_OPENCL

# *****************************************************************
#            Option to define DOXYGEN SUPPORT
# *****************************************************************

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
   AC_MSG_NOTICE([generate Doxygen documentation faster (using tag file mechanism)])
else
   AC_MSG_NOTICE([generate Doxygen documentation slower (reading all of Sage III and Rose together)])
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

# Liao 8/17/2010. Tried to work around a undefined SED on NERSC hopper.
# But this line is expanded after AC_PROG_LIBTOOL.
# I had to promote it to configure.in, right before calling  ROSE_SUPPORT_ROSE_PART_2
#test -z "$SED" && SED=sed

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

#echo 'int i;' > conftest.$ac_ext
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
# ROSE_FLAG_C_OPTIONS
# ROSE_FLAG_CXX_OPTIONS
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
[  --with-AxxPxx=PATH   Specify the prefix where A++P++ is installed],
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

ROSE_CONFIGURE_SECTION([Checking system capabilities])

AC_SEARCH_LIBS(clock_gettime, [rt], [
  RT_LIBS="$LIBS"
  LIBS=""
],[
  RT_LIBS=""
])
AC_SUBST(RT_LIBS)

# DQ (9/11/2006): Removed performance tests conditional, the performance tests were
# removed previously, but we still have the tests/nonsmoke/functional/PerformanceTests directory.
# AM_CONDITIONAL(ROSE_PERFORMANCE_TESTS,test ! "$with_PERFORMANCE_TESTS" = no)

# DQ (9/11/2006): skipping use of optional_PERFORMANCE_subdirs
# There is no configure.in in TESTS/PerformanceTests (only in TESTS/PerformanceTests/BenchmarkBase)
# AC_CONFIG_SUBDIRS(TESTS/PerformanceTests)
# AC_CONFIG_SUBDIRS(TESTS/PerformanceTests/BenchmarkBase)
# AC_SUBST(optional_PERFORMANCE_subdirs)

# DQ (12/16/2009): This option is now removed since the developersScratchSpace has been
# removed from the ROSE's git repository and it is a separate git repository that can be
# checked out internally by ROSE developers.
# Set up for Dan Quinlan's development test directory.
# AC_ARG_ENABLE(dq-developer-tests,
# [--enable-dq-developer-tests   Development option for Dan Quinlan (disregard).],
# [ echo "Setting up optional ROSE/developersScratchSpace/Dan directory"
# if test -d ${srcdir}/developersScratchSpace; then
#   :
# else
#   echo "This is a non-developer version of ROSE (source distributed with EDG binary)"
#   enable_dq_developer_tests=no
# fi
# ])
# AM_CONDITIONAL(DQ_DEVELOPER_TESTS,test "$enable_dq_developer_tests" = yes)

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

#-----------------------------------------------------------------------------


AC_DEFUN([ROSE_SUPPORT_ROSE_PART_3],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_3.

## Setup the EDG specific stuff
SETUP_EDG


ROSE_ARG_ENABLE(
  [alternate-edg-build-cpu],
  [for alternate EDG build cpu],
  [allows you to generate EDG binaries with a different CPU type in the name string]
)

#The build_triplet_without_redhat variable is used only in src/frontend/CxxFrontend/Makefile.am to determine the binary edg name
build_triplet_without_redhat=`${srcdir}/config/cleanConfigGuessOutput "$build" "$build_cpu" "$build_vendor"`
if test "x$CONFIG_HAS_ROSE_ENABLE_ALTERNATE_EDG_BUILD_CPU" = "xyes"; then
  # Manually modify the build CPU <build_cpu>-<build_vendor>-<build>
  build_triplet_without_redhat="$(echo "$build_triplet_without_redhat" | sed 's/^[[^-]]*\(.*\)/'$ROSE_ENABLE_ALTERNATE_EDG_BUILD_CPU'\1/')"
fi
AC_SUBST(build_triplet_without_redhat) dnl This is done even with EDG source, since it is used to determine the binary to make in roseFreshTest

# End macro ROSE_SUPPORT_ROSE_PART_3.
])

#-----------------------------------------------------------------------------

AC_DEFUN([ROSE_SUPPORT_ROSE_PART_4],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_4.

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

# Liao 6/20/2011, store source path without symbolic links, used to have consistent source and compile paths for ROSE
# when call graph analysis tests are used.
res_top_src=$(cd "$srcdir" && pwd -P)
AC_DEFINE_UNQUOTED([ROSE_SOURCE_TREE_PATH],"$res_top_src",[Location of ROSE Source Tree.])

# kelly64 (6/26/2013): Compass2 xml configuration files require fully-resolved
#                      absolute paths.
AC_SUBST(res_top_src)

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

# DQ (4/11/2010): This seems to have to appear before the Qt macros
# because the "AC PATH QT" are defined in config/qrose_indigo_1.m4.
# *****************************************************
#  Support for QRose Qt GUI (work at Imperial College)
# *****************************************************

# GMY (9/3/2008) QT4 & QROSE Optional Packages
AC_ARG_WITH(QRose, [  --with-QRose=PATH     prefix of QRose installation],
   [QROSE_PREFIX=$with_QRose
    if test "x$with_QRose" = xyes; then
       AC_MSG_FAILURE([--with-QRose=PATH must be specified to use option --with-QRose (a valid QRose intallation)])
    fi
    if test "x$with_QRose" = x; then
       AC_MSG_FAILURE([empty path used in --with-QRose=PATH must be specified to use option --with-QRose (a valid Qt intallation)])
    fi
   ],
        [with_QRose=no])

AC_SUBST(QROSE_PREFIX)
AM_CONDITIONAL(ROSE_USE_QROSE,test "x$with_QRose" != xno)

AC_MSG_NOTICE([with_QRose = "$with_QRose"])

#AM_CONDITIONAL(USE_QROSE, test "$with_QRose" != no)
#QROSE_LDFLAGS="-L${QROSE_PREFIX}/lib -lqrose"
#QROSE_CXXFLAGS="-I${QROSE_PREFIX}/include"
#AC_SUBST(QROSE_LDFLAGS)
#AC_SUBST(QROSE_CXXFLAGS)


# DQ (4/11/2010): Organized the Qt configure support.
# ****************************************************
#         Support for Qt (General GUI support)
# ****************************************************

# These are defined in config/qrose_indigo_1.m4, they
# are not standard AC macros.
AC_PATH_QT
AC_PATH_QT_MOC
AC_PATH_QT_RCC
AC_PATH_QT_UIC

# The code to set ROSEQT is in this macro's definition.
AC_PATH_QT_VERSION

AC_MSG_NOTICE([with_qt     = "$with_qt"])
AM_CONDITIONAL(ROSE_USE_QT,test x"$with_qt" != x"no")
if test "x$with_qt" = xyes; then
   AC_MSG_FAILURE([path to Qt not specified (usage: --with-qt=PATH)])
fi

# If QRose was specified then make sure that Qt was specified.
if test "x$with_QRose" != xno; then
   if test "x$with_qt" = xno; then
      AC_MSG_FAILURE([QRose requires valid specification of Qt installation (requires option: --with-qt=PATH)])
   fi
fi

# *****************************************************
#   Support for RoseQt GUI (ROSE specific Qt widgets)
# *****************************************************

# ROSE_SUPPORT_ROSEQT
# echo "with_roseQt = $with_roseQt"
# AM_CONDITIONAL(ROSE_WITH_ROSEQT,test x"$with_roseQt" != x"no")

# ****************************************************
#   Support for Assembly Semantics (binary analysis)
ROSE_SUPPORT_BINARY
# ****************************************************

ROSE_SUPPORT_PYTHON_API

# Added support for detection of libnuma, a NUMA aware memory allocation mechanism for many-core optimizations.
AC_CHECK_HEADERS(numa.h, [found_libnuma=yes])

if test "x$found_libnuma" = xyes; then
  AC_DEFINE([HAVE_NUMA_H],[],[Support for libnuma a NUMA memory allocation library for many-core optimizations])
fi

AM_CONDITIONAL(ROSE_USE_LIBNUMA, [test "x$found_libnuma" = xyes])


# PC (7/10/2009): The Haskell build system expects a fully numeric version number.
PACKAGE_VERSION_NUMERIC=`echo $PACKAGE_VERSION | sed -e 's/\([[a-z]]\+\)/\.\1/; y/a-i/1-9/'`
AC_SUBST(PACKAGE_VERSION_NUMERIC)

# This CPP symbol is defined so we can check whether rose_config.h is included into a public header file.  It serves
# no other purpose.  The name must not begin with "ROSE_" but must have a high probability of being globally unique (which
# is why it ends with "_ROSE").
AC_DEFINE(CONFIG_ROSE, 1, [Always defined and used for checking whether global CPP namespace is polluted])

# End macro ROSE_SUPPORT_ROSE_PART_4.
]
)




dnl ---------------------------------------------------------------
dnl CLASSPATH_COND_IF(COND, SHELL-CONDITION, [IF-TRUE], [IF-FALSE])
dnl ---------------------------------------------------------------
dnl Automake 1.11 can emit conditional rules for AC_CONFIG_FILES,
dnl using AM_COND_IF.  This wrapper uses it if it is available,
dnl otherwise falls back to code compatible with Automake 1.9.6.
AC_DEFUN([CLASSPATH_COND_IF],
[m4_ifdef([AM_COND_IF],
  [AM_COND_IF([$1], [$3], [$4])],
  [if $2; then
     m4_default([$3], [:])
   else
     m4_default([$4], [:])
   fi
])])

#-----------------------------------------------------------------------------

AC_DEFUN([ROSE_SUPPORT_ROSE_PART_5],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_5.

# DQ (9/21/2009): Debugging for RH release 5
AC_MSG_NOTICE([CC = "$CC"])
AC_MSG_NOTICE([CPPFLAGS = "$CPPFLAGS"])

AC_MSG_NOTICE([subdirs = "$subdirs"])
AC_CONFIG_SUBDIRS([libltdl])

# This list should be the same as in build (search for Makefile.in)

CLASSPATH_COND_IF([ROSE_HAS_EDG_SOURCE], [test "x$has_edg_source" = "xyes"], [
AC_CONFIG_FILES([
src/frontend/CxxFrontend/EDG/Makefile
src/frontend/CxxFrontend/EDG/EDG_4.12/Makefile
src/frontend/CxxFrontend/EDG/EDG_4.12/misc/Makefile
src/frontend/CxxFrontend/EDG/EDG_4.12/src/Makefile
src/frontend/CxxFrontend/EDG/EDG_4.12/src/disp/Makefile
src/frontend/CxxFrontend/EDG/EDG_4.12/lib/Makefile
src/frontend/CxxFrontend/EDG/EDG_5.0/Makefile
src/frontend/CxxFrontend/EDG/EDG_5.0/misc/Makefile
src/frontend/CxxFrontend/EDG/EDG_5.0/src/Makefile
src/frontend/CxxFrontend/EDG/EDG_5.0/src/disp/Makefile
src/frontend/CxxFrontend/EDG/EDG_5.0/lib/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.0/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.0/misc/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.0/src/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.0/src/disp/Makefile
src/frontend/CxxFrontend/EDG/EDG_6.0/lib/Makefile
src/frontend/CxxFrontend/EDG/edgRose/Makefile
])], [])

# End macro ROSE_SUPPORT_ROSE_PART_5.
]
)


#-----------------------------------------------------------------------------
AC_DEFUN([ROSE_SUPPORT_ROSE_PART_6],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_6.

# RV 9/14/2005: Removed src/3rdPartyLibraries/PDFLibrary/Makefile
# JJW 1/30/2008: Removed rose_paths.h as it is now built by a separate Makefile included from $(top_srcdir)/Makefile.am
AC_CONFIG_FILES([
Makefile
config/Makefile
docs/Makefile
docs/Rose/Makefile
docs/Rose/ROSE_DemoGuide.tex
docs/Rose/ROSE_DeveloperInstructions.tex
docs/Rose/ROSE_Exam.tex
docs/Rose/ROSE_InstallationInstructions.tex
docs/Rose/Tutorial/Makefile
docs/Rose/Tutorial/gettingStarted.tex
docs/Rose/Tutorial/tutorial.tex
docs/Rose/footer.html
docs/Rose/gettingStarted.tex
docs/Rose/leftmenu.html
docs/Rose/manual.tex
docs/Rose/rose-install-demo.cfg
docs/Rose/rose.cfg
docs/Rose/roseQtWidgets.doxygen
docs/Rose/sage.cfg
docs/testDoxygen/Makefile
docs/testDoxygen/test.cfg
exampleTranslators/AstCopyReplTester/Makefile
exampleTranslators/DOTGenerator/Makefile
exampleTranslators/Makefile
exampleTranslators/PDFGenerator/Makefile
exampleTranslators/defaultTranslator/Makefile
exampleTranslators/documentedExamples/AstRewriteExamples/Makefile
exampleTranslators/documentedExamples/Makefile
exampleTranslators/documentedExamples/dataBaseExamples/Makefile
exampleTranslators/documentedExamples/simpleTranslatorExamples/Makefile
exampleTranslators/documentedExamples/simpleTranslatorExamples/exampleMakefile
LicenseInformation/Makefile
projects/ArithmeticMeasureTool/Makefile
projects/BinaryCloneDetection/Makefile
projects/BinaryCloneDetection/compression/Makefile
projects/BinaryCloneDetection/semantic/Makefile
projects/BinaryCloneDetection/syntactic/Makefile
projects/BinaryCloneDetection/syntactic/gui/Makefile
projects/CertSecureCodeProject/Makefile
projects/CloneDetection/Makefile
projects/RaaS/Makefile
projects/RaaS/src/Makefile
projects/RaaS/include/nlohmann/Makefile
projects/RaaS/examples/demo/Makefile
projects/RaaS/examples/demo/compile_commands.json
projects/CompilationDB/Makefile
projects/CompilationDB/scripts/Makefile
projects/CompilationDB/examples/demo/Makefile
projects/CompilationDB/examples/demo/compile_commands.json
projects/CompilationDB/examples/kripke/Makefile
projects/CompilationDB/examples/doxygen/Makefile
projects/CompilationDB/static/js/Makefile
projects/CompilationDB/static/css/Makefile
projects/EditDistanceMetric/Makefile
projects/Fortran_to_C/Makefile
projects/Fortran_to_C/src/Makefile
projects/Fortran_to_C/tests/Makefile
projects/Jovial_to_C/Makefile
projects/Jovial_to_C/src/Makefile
projects/Jovial_to_C/tests/Makefile
projects/HeaderFilesInclusion/HeaderFilesGraphGenerator/Makefile
projects/HeaderFilesInclusion/HeaderFilesNotIncludedList/Makefile
projects/HeaderFilesInclusion/Makefile
projects/MPI_Tools/MPICodeMotion/Makefile
projects/MPI_Tools/MPIDeterminismAnalysis/Makefile
projects/MPI_Tools/Makefile
projects/Makefile
projects/ManyCoreRuntime/Makefile
projects/ManyCoreRuntime/docs/Makefile
projects/MapleDSL/Makefile
projects/QtDesignerPlugins/Makefile
projects/RTED/CppRuntimeSystem/DebuggerQt/Makefile
projects/RTED/CppRuntimeSystem/Makefile
projects/RTED/Makefile
projects/RoseBlockLevelTracing/Makefile
projects/RoseBlockLevelTracing/src/Makefile
projects/RoseQt/AstViewer/Makefile
projects/RoseQt/Makefile
projects/SMTPathFeasibility/Makefile
projects/ShiftCalculus4/Makefile
projects/TemplateAnalysis/Makefile
projects/TemplateAnalysis/include/ROSE/Analysis/Template/Makefile
projects/TemplateAnalysis/include/ROSE/proposed/Makefile
projects/TemplateAnalysis/include/nlohmann/Makefile
projects/TemplateAnalysis/lib/ROSE/Analysis/Template/Makefile
projects/TemplateAnalysis/lib/ROSE/proposed/Makefile
projects/TemplateAnalysis/src/Makefile
projects/TemplateAnalysis/examples/Makefile
projects/TemplateAnalysis/examples/inspect-autovar/Makefile
projects/TileK/Makefile
projects/TileK/doc/Makefile
projects/TileK/doc/dlx.doxy
projects/TileK/doc/index.html
projects/TileK/doc/klt-rtl.doxy
projects/TileK/doc/klt.doxy
projects/TileK/doc/mdcg.doxy
projects/TileK/doc/mfb.doxy
projects/TileK/doc/tilek-basic.doxy
projects/TileK/doc/tilek-cuda.doxy
projects/TileK/doc/tilek-opencl.doxy
projects/TileK/doc/tilek-rtl-basic.doxy
projects/TileK/doc/tilek-rtl-cuda.doxy
projects/TileK/doc/tilek-rtl-opencl.doxy
projects/TileK/doc/tilek-rtl-threads.doxy
projects/TileK/doc/tilek-threads.doxy
projects/TileK/include/DLX/TileK/Makefile
projects/TileK/include/RTL/Host/Makefile
projects/TileK/include/RTL/Kernel/CUDA/Makefile
projects/TileK/include/RTL/Kernel/OpenCL/Makefile
projects/TileK/lib/Makefile
projects/TileK/src/Makefile
projects/TileK/tests/accelerator/CUDA/Makefile
projects/TileK/tests/accelerator/Makefile
projects/TileK/tests/accelerator/OpenCL/Makefile
projects/TileK/tests/basic/Makefile
projects/TileK/tests/rtl/Makefile
projects/TileK/tests/threads/Makefile
projects/UpcTranslation/Makefile
projects/UpcTranslation/tests/Makefile
projects/Viz/Makefile
projects/Viz/examples/Makefile
projects/Viz/include/Makefile
projects/Viz/include/Viz/Makefile
projects/Viz/include/Viz/Traversals/Makefile
projects/Viz/lib/Makefile
projects/Viz/src/Makefile
projects/Viz/tools/Makefile
projects/amrShiftDSLcompiler/AMRShift/Makefile
projects/amrShiftDSLcompiler/Makefile
projects/arrayDSLcompiler/Makefile
projects/POET_ROSE/Makefile
projects/POET_ROSE/test/Makefile
projects/POET_ROSE/manycore-stencil/Makefile
projects/POET_ROSE/manycore-stencil/include/Makefile
projects/POET_ROSE/manycore-stencil/benchmarks/Makefile
projects/arrayOptimization/Makefile
projects/arrayOptimization/test/Makefile
projects/autoParallelization/Makefile
projects/autoParallelization/tests/Makefile
projects/autoParallelization/difftests/Makefile
projects/autoTuning/Makefile
projects/autoTuning/doc/Makefile
projects/autoTuning/tests/Makefile
projects/compass/Makefile
projects/compass/src/Makefile
projects/compass/src/compassSupport/Makefile
projects/compass/src/util/C-API/Makefile
projects/compass/src/util/MPIAbstraction/Makefile
projects/compass/src/util/MPIAbstraction/alt-mpi-headers/Makefile
projects/compass/src/util/MPIAbstraction/alt-mpi-headers/mpich-1.2.7p1/Makefile
projects/compass/src/util/MPIAbstraction/alt-mpi-headers/mpich-1.2.7p1/include/Makefile
projects/compass/src/util/MPIAbstraction/alt-mpi-headers/mpich-1.2.7p1/include/mpi2c++/Makefile
projects/compass/src/util/Makefile
projects/compass/tools/Makefile
projects/compass/tools/compass/Makefile
projects/compass/tools/compass/buildInterpreter/Makefile
projects/compass/tools/compass/doc/Makefile
projects/compass/tools/compass/doc/compass.tex
projects/compass/tools/compass/gui/Makefile
projects/compass/tools/compass/gui2/Makefile
projects/compass/tools/compass/tests/Compass_C_tests/Makefile
projects/compass/tools/compass/tests/Compass_Cxx_tests/Makefile
projects/compass/tools/compass/tests/Compass_OpenMP_tests/Makefile
projects/compass/tools/compass/tests/Makefile
projects/compass/tools/compassVerifier/Makefile
projects/compass/tools/sampleCompassSubset/Makefile
projects/dsl_infrastructure/Makefile
projects/extractMPISkeleton/Makefile
projects/extractMPISkeleton/src/Makefile
projects/extractMPISkeleton/tests/Makefile
projects/fuse/Makefile
projects/fuse/src/Makefile
projects/fuse/tests/Makefile
projects/interpreter/Makefile
projects/palette/Makefile
projects/pragmaParsing/Makefile
projects/xgenTranslator/Makefile
python/Makefile
python/Rose/Makefile
python/Rose/BinaryAnalysis/Makefile
python/Rose/BinaryAnalysis/Partitioner2/Makefile
python/Rose/BinaryAnalysis/Unparser/Makefile
scripts/Makefile
src/3rdPartyLibraries/MSTL/Makefile
src/3rdPartyLibraries/Makefile
src/3rdPartyLibraries/antlr-jars/Makefile
src/3rdPartyLibraries/flang-parser/Makefile
src/3rdPartyLibraries/fortran-parser/Makefile
src/3rdPartyLibraries/java-parser/Makefile
src/3rdPartyLibraries/qrose/Components/Common/Makefile
src/3rdPartyLibraries/qrose/Components/Common/icons/Makefile
src/3rdPartyLibraries/qrose/Components/Makefile
src/3rdPartyLibraries/qrose/Components/QueryBox/Makefile
src/3rdPartyLibraries/qrose/Components/SourceBox/Makefile
src/3rdPartyLibraries/qrose/Components/TreeBox/Makefile
src/3rdPartyLibraries/qrose/Framework/Makefile
src/3rdPartyLibraries/qrose/Makefile
src/3rdPartyLibraries/qrose/QRoseLib/Makefile
src/3rdPartyLibraries/qrose/Widgets/Makefile
src/Makefile
src/ROSETTA/Makefile
src/ROSETTA/src/Makefile
src/backend/Makefile
src/frontend/BinaryFormats/Makefile
src/frontend/BinaryLoader/Makefile
src/frontend/CxxFrontend/Clang/Makefile
src/frontend/CxxFrontend/Makefile
src/frontend/DLX/Makefile
src/frontend/DLX/include/DLX/Core/Makefile
src/frontend/DLX/lib/core/Makefile
src/frontend/Disassemblers/Makefile
src/frontend/ECJ_ROSE_Connection/Makefile
src/frontend/Experimental_General_Language_Support/Makefile
src/frontend/Experimental_General_Language_Support/ATerm/Makefile
src/frontend/Experimental_OpenFortranParser_ROSE_Connection/Makefile
src/frontend/Experimental_Flang_ROSE_Connection/Makefile
src/frontend/Experimental_Csharp_ROSE_Connection/Makefile
src/frontend/Experimental_Ada_ROSE_Connection/Makefile
src/frontend/Experimental_Jovial_ROSE_Connection/Makefile
src/frontend/Experimental_Cobol_ROSE_Connection/Makefile
src/frontend/Experimental_Matlab_ROSE_Connection/Makefile
src/frontend/Makefile
src/frontend/OpenFortranParser_SAGE_Connection/Makefile
src/frontend/PHPFrontend/Makefile
src/frontend/Partitioner2/Makefile
src/frontend/PythonFrontend/Makefile
src/frontend/SageIII/GENERATED_CODE_DIRECTORY_Cxx_Grammar/Makefile
src/frontend/SageIII/Makefile
src/frontend/SageIII/astFileIO/Makefile
src/frontend/SageIII/astFixup/Makefile
src/frontend/SageIII/astFromString/Makefile
src/frontend/SageIII/astHiddenTypeAndDeclarationLists/Makefile
src/frontend/SageIII/astMerge/Makefile
src/frontend/SageIII/astPostProcessing/Makefile
src/frontend/SageIII/astTokenStream/Makefile
src/frontend/SageIII/astVisualization/Makefile
src/frontend/SageIII/includeDirectivesProcessing/Makefile
src/frontend/SageIII/sage.docs
src/frontend/SageIII/sageInterface/Makefile
src/frontend/SageIII/virtualCFG/Makefile
src/frontend/X10_ROSE_Connection/Makefile
src/midend/KLT/Makefile
src/midend/KLT/include/KLT/Core/Makefile
src/midend/KLT/include/KLT/DLX/Makefile
src/midend/KLT/include/KLT/MDCG/Makefile
src/midend/KLT/include/KLT/RTL/Makefile
src/midend/KLT/lib/core/Makefile
src/midend/KLT/lib/mdcg/Makefile
src/midend/KLT/lib/rtl/Makefile
src/midend/MDCG/Makefile
src/midend/MDCG/include/MDCG/Model/Makefile
src/midend/MDCG/include/MDCG/Tools/Makefile
src/midend/MDCG/lib/model/Makefile
src/midend/MDCG/lib/tools/Makefile
src/midend/MFB/Makefile
src/midend/MFB/include/MFB/KLT/Makefile
src/midend/MFB/include/MFB/Makefile
src/midend/MFB/include/MFB/Sage/Makefile
src/midend/MFB/lib/klt/Makefile
src/midend/MFB/lib/sage/Makefile
src/midend/MFB/lib/utils/Makefile
src/midend/Makefile
src/midend/BinaryAnalysis/Makefile
src/midend/programAnalysis/Makefile
src/midend/programAnalysis/ssaUnfilteredCfg/Makefile
src/midend/programAnalysis/staticSingleAssignment/Makefile
src/midend/programAnalysis/systemDependenceGraph/Makefile
src/midend/programTransformation/extractFunctionArgumentsNormalization/Makefile
src/midend/programTransformation/loopProcessing/Makefile
src/midend/programTransformation/singleStatementToBlockNormalization/Makefile
src/roseExtensions/Makefile
src/roseExtensions/dataStructureTraversal/Makefile
src/roseExtensions/failSafe/Makefile
src/roseExtensions/highLevelGrammar/Makefile
src/roseExtensions/qtWidgets/AsmInstructionBar/Makefile
src/roseExtensions/qtWidgets/AsmView/Makefile
src/roseExtensions/qtWidgets/AstBrowserWidget/Makefile
src/roseExtensions/qtWidgets/AstGraphWidget/Makefile
src/roseExtensions/qtWidgets/AstProcessing/Makefile
src/roseExtensions/qtWidgets/BeautifiedAst/Makefile
src/roseExtensions/qtWidgets/FlopCounter/Makefile
src/roseExtensions/qtWidgets/InstructionCountAnnotator/Makefile
src/roseExtensions/qtWidgets/KiviatView/Makefile
src/roseExtensions/qtWidgets/Makefile
src/roseExtensions/qtWidgets/MetricFilter/Makefile
src/roseExtensions/qtWidgets/MetricsConfig/Makefile
src/roseExtensions/qtWidgets/MetricsKiviat/Makefile
src/roseExtensions/qtWidgets/NodeInfoWidget/Makefile
src/roseExtensions/qtWidgets/ProjectManager/Makefile
src/roseExtensions/qtWidgets/PropertyTreeWidget/Makefile
src/roseExtensions/qtWidgets/QCodeEditWidget/Makefile
src/roseExtensions/qtWidgets/QCodeEditWidget/QCodeEdit/Makefile
src/roseExtensions/qtWidgets/QCodeEditWidget/QCodeEdit/document/Makefile
src/roseExtensions/qtWidgets/QCodeEditWidget/QCodeEdit/qnfa/Makefile
src/roseExtensions/qtWidgets/QCodeEditWidget/QCodeEdit/widgets/Makefile
src/roseExtensions/qtWidgets/QtGradientEditor/Makefile
src/roseExtensions/qtWidgets/RoseCodeEdit/Makefile
src/roseExtensions/qtWidgets/RoseFileSelector/Makefile
src/roseExtensions/qtWidgets/SrcBinView/Makefile
src/roseExtensions/qtWidgets/TaskSystem/Makefile
src/roseExtensions/qtWidgets/TreeModel/Makefile
src/roseExtensions/qtWidgets/WidgetCreator/Makefile
src/roseExtensions/qtWidgets/util/Makefile
src/roseExtensions/roseHPCToolkit/Makefile
src/roseExtensions/roseHPCToolkit/docs/Makefile
src/roseExtensions/roseHPCToolkit/include/Makefile
src/roseExtensions/roseHPCToolkit/include/rosehpct/Makefile
src/roseExtensions/roseHPCToolkit/include/rosehpct/gprof/Makefile
src/roseExtensions/roseHPCToolkit/include/rosehpct/profir/Makefile
src/roseExtensions/roseHPCToolkit/include/rosehpct/profir2sage/Makefile
src/roseExtensions/roseHPCToolkit/include/rosehpct/sage/Makefile
src/roseExtensions/roseHPCToolkit/include/rosehpct/util/Makefile
src/roseExtensions/roseHPCToolkit/include/rosehpct/xml-xercesc/Makefile
src/roseExtensions/roseHPCToolkit/include/rosehpct/xml/Makefile
src/roseExtensions/roseHPCToolkit/include/rosehpct/xml2profir/Makefile
src/roseExtensions/roseHPCToolkit/src/Makefile
src/roseExtensions/roseHPCToolkit/src/gprof/Makefile
src/roseExtensions/roseHPCToolkit/src/profir/Makefile
src/roseExtensions/roseHPCToolkit/src/profir2sage/Makefile
src/roseExtensions/roseHPCToolkit/src/sage/Makefile
src/roseExtensions/roseHPCToolkit/src/util/Makefile
src/roseExtensions/roseHPCToolkit/src/xml-xercesc/Makefile
src/roseExtensions/roseHPCToolkit/src/xml/Makefile
src/roseExtensions/roseHPCToolkit/src/xml2profir/Makefile
src/roseExtensions/sqlite3x/Makefile
src/roseIndependentSupport/Makefile
src/roseIndependentSupport/dot2gml/Makefile
src/roseSupport/Makefile
src/util/Makefile
src/util/commandlineProcessing/Makefile
src/util/graphs/Makefile
src/util/Sawyer/Makefile
src/util/stringSupport/Makefile
src/util/support/Makefile
stamp-h
tests/Makefile
tests/nonsmoke/ExamplesForTestWriters/Makefile
tests/nonsmoke/Makefile
tests/nonsmoke/acceptance/Makefile
tests/nonsmoke/functional/BinaryAnalysis/Dwarf_tests/Makefile
tests/nonsmoke/functional/BinaryAnalysis/Makefile
tests/nonsmoke/functional/BinaryAnalysis/Pin_tests/Makefile
tests/nonsmoke/functional/BinaryAnalysis/libraryIdentification_tests/Makefile
tests/nonsmoke/functional/BinaryAnalysis/Concolic/Makefile
tests/nonsmoke/functional/CompileTests/A++Code/Makefile
tests/nonsmoke/functional/CompileTests/A++Tests/Makefile
tests/nonsmoke/functional/CompileTests/C_tests/Makefile
tests/nonsmoke/functional/CompileTests/C_subset_of_Cxx_tests/Makefile
tests/nonsmoke/functional/CompileTests/C89_std_c89_tests/Makefile
tests/nonsmoke/functional/CompileTests/C99_tests/Makefile
tests/nonsmoke/functional/CompileTests/C11_tests/Makefile
tests/nonsmoke/functional/CompileTests/CudaTests/Makefile
tests/nonsmoke/functional/CompileTests/Cxx_tests/Makefile
tests/nonsmoke/functional/CompileTests/Cxx11_tests/Makefile
tests/nonsmoke/functional/CompileTests/Cxx14_tests/Makefile
tests/nonsmoke/functional/CompileTests/Cxx17_tests/Makefile
tests/nonsmoke/functional/CompileTests/ElsaTestCases/Makefile
tests/nonsmoke/functional/CompileTests/ElsaTestCases/ctests/Makefile
tests/nonsmoke/functional/CompileTests/ElsaTestCases/gnu/Makefile
tests/nonsmoke/functional/CompileTests/ElsaTestCases/kandr/Makefile
tests/nonsmoke/functional/CompileTests/ElsaTestCases/std/Makefile
tests/nonsmoke/functional/CompileTests/ExpressionTemplateExample_tests/Makefile
tests/nonsmoke/functional/CompileTests/FailSafe_tests/Makefile
tests/nonsmoke/functional/CompileTests/Fortran_tests/LANL_POP/Makefile
tests/nonsmoke/functional/CompileTests/Fortran_tests/Makefile
tests/nonsmoke/functional/CompileTests/Fortran_tests/experimental_frontend_tests/Makefile
tests/nonsmoke/functional/CompileTests/Fortran_tests/gfortranTestSuite/Makefile
tests/nonsmoke/functional/CompileTests/Fortran_tests/gfortranTestSuite/gfortran.dg/Makefile
tests/nonsmoke/functional/CompileTests/Fortran_tests/gfortranTestSuite/gfortran.fortran-torture/Makefile
tests/nonsmoke/functional/CompileTests/Java_tests/Makefile
tests/nonsmoke/functional/CompileTests/Java_tests/unit_tests/Makefile
tests/nonsmoke/functional/CompileTests/experimental_csharp_tests/Makefile
tests/nonsmoke/functional/CompileTests/experimental_ada_tests/Makefile
tests/nonsmoke/functional/CompileTests/experimental_fortran_tests/Makefile
tests/nonsmoke/functional/CompileTests/experimental_jovial_tests/Makefile
tests/nonsmoke/functional/CompileTests/experimental_cobol_tests/Makefile
tests/nonsmoke/functional/CompileTests/experimental_matlab_tests/Makefile
tests/nonsmoke/functional/CompileTests/Makefile
tests/nonsmoke/functional/CompileTests/MicrosoftWindows_C_tests/Makefile
tests/nonsmoke/functional/CompileTests/MicrosoftWindows_Cxx_tests/Makefile
tests/nonsmoke/functional/CompileTests/MicrosoftWindows_Java_tests/Makefile
tests/nonsmoke/functional/CompileTests/MicrosoftWindows_tests/Makefile
tests/nonsmoke/functional/CompileTests/OpenClTests/Makefile
tests/nonsmoke/functional/CompileTests/OpenACC_tests/Makefile
tests/nonsmoke/functional/CompileTests/OpenACC_tests/fortran/Makefile
tests/nonsmoke/functional/CompileTests/OpenMP_tests/Makefile
tests/nonsmoke/functional/CompileTests/OpenMP_tests/cvalidation/Makefile
tests/nonsmoke/functional/CompileTests/OpenMP_tests/fortran/Makefile
tests/nonsmoke/functional/CompileTests/OvertureCode/Makefile
tests/nonsmoke/functional/CompileTests/P++Tests/Makefile
tests/nonsmoke/functional/CompileTests/PythonExample_tests/Makefile
tests/nonsmoke/functional/CompileTests/Python_tests/Makefile
tests/nonsmoke/functional/CompileTests/RoseExample_tests/Makefile
tests/nonsmoke/functional/CompileTests/STL_tests/Makefile
tests/nonsmoke/functional/CompileTests/UPC_tests/Makefile
tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/Makefile
tests/nonsmoke/functional/CompileTests/UnparseHeadersUsingTokenStream_tests/Makefile
tests/nonsmoke/functional/CompileTests/boost_tests/Makefile
tests/nonsmoke/functional/CompileTests/colorAST_tests/Makefile
tests/nonsmoke/functional/CompileTests/copyAST_tests/Makefile
tests/nonsmoke/functional/CompileTests/frontend_integration/Makefile
tests/nonsmoke/functional/CompileTests/hiddenTypeAndDeclarationListTests/Makefile
tests/nonsmoke/functional/CompileTests/mergeAST_tests/Makefile
tests/nonsmoke/functional/CompileTests/mixLanguage_tests/Makefile
tests/nonsmoke/functional/CompileTests/nameQualificationAndTypeElaboration_tests/Makefile
tests/nonsmoke/functional/CompileTests/sizeofOperation_tests/Makefile
tests/nonsmoke/functional/CompileTests/sourcePosition_tests/Makefile
tests/nonsmoke/functional/CompileTests/staticCFG_tests/Makefile
tests/nonsmoke/functional/CompileTests/systemc_tests/Makefile
tests/nonsmoke/functional/CompileTests/vxworks_tests/Makefile
tests/nonsmoke/functional/CompileTests/uninitializedField_tests/Makefile
tests/nonsmoke/functional/CompileTests/unparseToString_tests/Makefile
tests/nonsmoke/functional/CompileTests/virtualCFG_tests/Makefile
tests/nonsmoke/functional/CompileTests/x10_tests/Makefile
tests/nonsmoke/functional/CompileTests/unparse_template_from_ast/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/collectAllCommentsAndDirectives_tests/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/preinclude_tests/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testCpreprocessorOption/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testFileNamesAndExtensions/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testForSpuriousOutput/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testGenerateSourceFileNames/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testGnuOptions/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testHeaderFileOutput/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testIncludeOptions/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testOutputFileOption/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/testWave/Makefile
tests/nonsmoke/functional/CompilerOptionsTests/tokenStream_tests/Makefile
tests/nonsmoke/functional/Makefile
tests/nonsmoke/functional/moveDeclarationTool/Makefile
tests/nonsmoke/functional/PerformanceTests/Makefile
tests/nonsmoke/functional/RunTests/A++Tests/Makefile
tests/nonsmoke/functional/RunTests/AstDeleteTests/Makefile
tests/nonsmoke/functional/RunTests/FortranTests/LANL_POP/Makefile
tests/nonsmoke/functional/RunTests/FortranTests/Makefile
tests/nonsmoke/functional/RunTests/Makefile
tests/nonsmoke/functional/RunTests/PythonTests/Makefile
tests/nonsmoke/functional/UnitTests/Makefile
tests/nonsmoke/functional/UnitTests/Rose/Makefile
tests/nonsmoke/functional/UnitTests/Rose/SageBuilder/Makefile
tests/nonsmoke/functional/Utility/Makefile
tests/nonsmoke/functional/roseTests/Makefile
tests/nonsmoke/functional/roseTests/PHPTests/Makefile
tests/nonsmoke/functional/roseTests/ROSETTA/Makefile
tests/nonsmoke/functional/roseTests/abstractMemoryObjectTests/Makefile
tests/nonsmoke/functional/roseTests/astFileIOTests/Makefile
tests/nonsmoke/functional/roseTests/astInliningTests/Makefile
tests/nonsmoke/functional/roseTests/astInterfaceTests/Makefile
tests/nonsmoke/functional/roseTests/astInterfaceTests/typeEquivalenceTests/Makefile
tests/nonsmoke/functional/roseTests/astInterfaceTests/unitTests/Makefile
tests/nonsmoke/functional/roseTests/astLValueTests/Makefile
tests/nonsmoke/functional/roseTests/astMergeTests/Makefile
tests/nonsmoke/functional/roseTests/astOutliningTests/Makefile
tests/nonsmoke/functional/roseTests/astPerformanceTests/Makefile
tests/nonsmoke/functional/roseTests/astProcessingTests/Makefile
tests/nonsmoke/functional/roseTests/astQueryTests/Makefile
tests/nonsmoke/functional/roseTests/astRewriteTests/Makefile
tests/nonsmoke/functional/roseTests/astSnippetTests/Makefile
tests/nonsmoke/functional/roseTests/astSymbolTableTests/Makefile
tests/nonsmoke/functional/roseTests/astTokenStreamTests/Makefile
tests/nonsmoke/functional/roseTests/fileLocation_tests/Makefile
tests/nonsmoke/functional/roseTests/graph_tests/Makefile
tests/nonsmoke/functional/roseTests/loopProcessingTests/Makefile
tests/nonsmoke/functional/roseTests/mergeTraversal_tests/Makefile
tests/nonsmoke/functional/roseTests/ompLoweringTests/Makefile
tests/nonsmoke/functional/roseTests/ompLoweringTests/fortran/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/defUseAnalysisTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/generalDataFlowAnalysisTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/ssa_UnfilteredCfg_Test/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/staticInterproceduralSlicingTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/staticSingleAssignmentTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/systemDependenceGraphTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/testCallGraphAnalysis/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/typeTraitTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/variableLivenessTests/Makefile
tests/nonsmoke/functional/roseTests/programAnalysisTests/variableRenamingTests/Makefile
tests/nonsmoke/functional/roseTests/programTransformationTests/Makefile
tests/nonsmoke/functional/roseTests/programTransformationTests/extractFunctionArgumentsTest/Makefile
tests/nonsmoke/functional/roseTests/programTransformationTests/singleStatementToBlockNormalization/Makefile
tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/Makefile
tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/01/ANALYSIS/Makefile
tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/01/Makefile
tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/01/PROFILE/Makefile
tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/01/PROGRAM/Makefile
tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/02/Makefile
tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/02/PROFILE/Makefile
tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/02/struct_ls/Makefile
tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/03/Makefile
tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/03/PROFILE/Makefile
tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/03/struct_ls/Makefile
tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/Makefile
tests/nonsmoke/functional/roseTests/varDeclNorm/Makefile
tests/nonsmoke/functional/testSupport/Makefile
tests/nonsmoke/functional/testSupport/gtest/Makefile
tests/nonsmoke/functional/translatorTests/Makefile
tests/nonsmoke/specimens/Makefile
tests/nonsmoke/specimens/binary/Makefile
tests/nonsmoke/specimens/c++/Makefile
tests/nonsmoke/specimens/c/Makefile
tests/nonsmoke/specimens/fortran/Makefile
tests/nonsmoke/specimens/java/Makefile
tests/nonsmoke/unit/Makefile
tests/nonsmoke/unit/SageInterface/Makefile
tests/roseTests/Makefile
tests/roseTests/ompLoweringTests/Makefile
tests/roseTests/programAnalysisTests/Makefile
tests/roseTests/programAnalysisTests/typeTraitTests/Makefile
tests/smoke/ExamplesForTestWriters/Makefile
tests/smoke/Makefile
tests/smoke/functional/BinaryAnalysis/Makefile
tests/smoke/functional/Fortran/Makefile
tests/smoke/functional/Makefile
tests/smoke/specimens/Makefile
tests/smoke/specimens/binary/Makefile
tests/smoke/specimens/c++/Makefile
tests/smoke/specimens/c/Makefile
tests/smoke/specimens/fortran/Makefile
tests/smoke/specimens/java/Makefile
tests/smoke/unit/BinaryAnalysis/Makefile
tests/smoke/unit/Boost/Makefile
tests/smoke/unit/Makefile
tests/smoke/unit/Sawyer/Makefile
tests/smoke/unit/Utility/Makefile
tools/Makefile
tools/globalVariablesInLambdas/Makefile
tools/classMemberVariablesInLambdas/Makefile
tools/checkFortranInterfaces/Makefile
tutorial/Makefile
tutorial/binaryAnalysis/Makefile
tutorial/exampleMakefile
tutorial/intelPin/Makefile
tutorial/outliner/Makefile
tutorial/roseHPCT/Makefile
])

# DQ (3/8/2017): Removed these directories from testing (pre-smoke and pre-nonsmoke test directories.
# tests/CompileTests/Makefile
# tests/CompileTests/OpenMP_tests/Makefile
# tests/CompileTests/x10_tests/Makefile

# DQ (11/14/2017): Removed GNAT test directory since it is redundant with the ADA test directory.
# tests/nonsmoke/functional/CompileTests/gnat_tests/Makefile

# Liao, 1/16/2014, comment out a few directories which are turned off for EDG 4.x upgrade
#projects/BinaryDataStructureRecognition/Makefile
#tests/nonsmoke/functional/CompileTests/CAF2_tests/Makefile


# DQ (10/27/2010): New Fortran tests (from gfortan test suite).
# tests/nonsmoke/functional/CompileTests/Fortran_tests/gfortranTestSuite/Makefile
# tests/nonsmoke/functional/CompileTests/Fortran_tests/gfortranTestSuite/gfortran.fortran-torture/Makefile
# tests/nonsmoke/functional/CompileTests/Fortran_tests/gfortranTestSuite/gfortran.dg/Makefile

# DQ (8/12/2010): We want to get permission to distribute these files as test codes.
# tests/nonsmoke/functional/CompileTests/Fortran_tests/LANL_POP/Makefile

# DQ (10/24/2009): We don't need to support EDG 3.10 anymore.
# src/frontend/CxxFrontend/EDG_3.10/Makefile
# src/frontend/CxxFrontend/EDG_3.10/misc/Makefile
# src/frontend/CxxFrontend/EDG_3.10/src/Makefile
# src/frontend/CxxFrontend/EDG_3.10/src/disp/Makefile
# src/frontend/CxxFrontend/EDG_3.10/lib/Makefile

# DQ (12/31/2008): Skip these, since we don't have SPEC and NAS benchmarks setup yet.
# developersScratchSpace/Dan/Fortran_tests/NPB3.2-SER/Makefile
# developersScratchSpace/Dan/Fortran_tests/NPB3.2-SER/BT/Makefile
# developersScratchSpace/Dan/SpecCPU2006/Makefile
# developersScratchSpace/Dan/SpecCPU2006/config/Makefile
# developersScratchSpace/Dan/SpecCPU2006/config/rose.cfg

# DQ (9/12/2008): Removed older version of QRose (now an external project)
# src/roseIndependentSupport/graphicalUserInterface/Makefile
# src/roseIndependentSupport/graphicalUserInterface/src/Makefile
# src/roseIndependentSupport/graphicalUserInterface/src/QRTree/Makefile
# src/roseIndependentSupport/graphicalUserInterface/src/QRCodeBox/Makefile
# src/roseIndependentSupport/graphicalUserInterface/src/QRGui/Makefile
# src/roseIndependentSupport/graphicalUserInterface/src/QRGui/icons22/Makefile
# src/roseIndependentSupport/graphicalUserInterface/src/QRQueryBox/Makefile
# exampleTranslators/graphicalUserInterfaceExamples/Makefile
# exampleTranslators/graphicalUserInterfaceExamples/slicing/Makefile
# exampleTranslators/graphicalUserInterfaceExamples/attributes/Makefile
# exampleTranslators/graphicalUserInterfaceExamples/query/Makefile
# exampleTranslators/graphicalUserInterfaceExamples/layout/Makefile

# End macro ROSE_SUPPORT_ROSE_PART_6.
]
)


#-----------------------------------------------------------------------------


AC_DEFUN([ROSE_SUPPORT_ROSE_PART_7],
[
# Begin macro ROSE_SUPPORT_ROSE_PART_7.

AC_CONFIG_COMMANDS([default],
    [
     AC_MSG_NOTICE([ensuring Grammar in the compile tree (assuming source tree is not the same as the compile tree)])
     pathToSourceDir="`cd $srcdir && pwd`"
     test -d src/ROSETTA/Grammar || ( rm -rf src/ROSETTA/Grammar && ln -s "$pathToSourceDir/src/ROSETTA/Grammar" src/ROSETTA/Grammar )
    ],
    [])

# Generate rose_paths.C
AC_CONFIG_COMMANDS([rose_paths.C], [
    AC_MSG_NOTICE([building src/util/rose_paths.C])
    make src/util/rose_paths.C
])

# Generate public config file from private config file. The public config file adds "ROSE_" to the beginning of
# certain symbols. See scripts/publicConfiguration.pl for details.
AC_CONFIG_COMMANDS([rosePublicConfig.h],[
    AC_MSG_NOTICE([building rosePublicConfig.h])
    make rosePublicConfig.h
])

# [TOO1, 2014-04-22]
# TODO: Re-enable once we phase out support for older version of Autotools.
#       Specifically, Pontetec is using Autoconf 2.59 and Automake 1.9.6.
# Rewrite the definitions for srcdir, top_srcdir, builddir, and top_builddir so they use the "abs_" versions instead.
#AC_CONFIG_COMMANDS([absoluteNames],
#[[
#       echo "rewriting makefiles to use absolute paths for srcdir, top_srcdir, builddir, and top_builddir..."
#       find . -name Makefile | xargs sed -i~ \
#           -re 's/^(srcdir|top_srcdir|builddir|top_builddir) = \..*/\1 = $(abs_\1)/'
#]])



# End macro ROSE_SUPPORT_ROSE_PART_7.
]
)

