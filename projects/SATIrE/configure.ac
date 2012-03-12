#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.67)
AC_INIT(SATIrE, 0.9.1, schordan@technikum-wien.at or adrian@llnl.gov)
AC_CONFIG_SRCDIR([README])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign no-installinfo no-texinfo.tex tar-pax 1.9.5])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_YACC
AS_IF([test "x`which $YACC`" = x],[AC_MSG_FAILURE([Please install bison])])
AC_PROG_CXX
AS_IF([test "x`which $CXX`" = x],[AC_MSG_FAILURE([Please install g++])])
AC_PROG_CC
AS_IF([test "x`which $CC`" = x],[AC_MSG_FAILURE([Please install gcc])])
AC_PROG_LEX
AS_IF([test "x`which $LEX`" = x],[AC_MSG_FAILURE([Please install flex])])
AC_PROG_SED
AS_IF([test "x`which $SED`" = x],[AC_MSG_FAILURE([Please install sed])])
AC_PROG_LIBTOOL
AS_IF([test "x$LIBTOOL" = x],[AC_MSG_FAILURE([Please install libtool])])

# ROSE PATH
ROSEDIR='/opt/rose'
AC_ARG_WITH(rosedir,
  [  --with-rosedir=DIR           ROSE install path (default in /opt/rose)],
  [ROSEDIR="$withval"])
AC_SUBST(ROSEDIR)

ROSE_INCLUDE_DIR="$ROSEDIR/include"
AC_ARG_WITH(rose_includedir,
  [  --with-rose-includedir=DIR   ROSE include path (default in /opt/rose/include)],
  [ROSE_INCLUDE_DIR="$withval"])
AC_SUBST(ROSE_INCLUDE_DIR)

ROSE_LIB_DIR="$ROSEDIR/lib"
AC_ARG_WITH(rose_libdir,
  [  --with-rose-libdir=DIR       ROSE library path (default in /opt/rose/lib)],
  [ROSE_LIB_DIR="$withval"])
AC_SUBST(ROSE_LIB_DIR)

# PAG PATH
AC_ARG_WITH([pag],
   [AS_HELP_STRING([--without-pag],
          [disable support for PAG])],
   [],
   [with_pag=yes])

PAGDIR='/opt/pag'
AC_ARG_WITH(pagdir,
  [  --with-pagdir=DIR            PAG install path (default in /opt/pag)],
  [with_pag=yes
   PAGDIR="$withval"])
AC_SUBST(PAGDIR)

PAG_INCLUDE_DIR="$PAGDIR/include"
AC_ARG_WITH(pag_includedir,
  [  --with-pag-includedir=DIR    PAG include path (default in /opt/pag/include)],
  [with_pag=yes
   PAG_INCLUDE_DIR="$withval"])
AC_SUBST(PAG_INCLUDE_DIR)

PAG_LIB_DIR="$PAGDIR/lib/pag"
AC_ARG_WITH(pag_libdir,
  [  --with-pag-libdir=DIR        PAG library path (default in /opt/pag/lib)],
  [with_pag=yes
   PAG_LIB_DIR="$withval"])
AC_SUBST(PAG_LIB_DIR)

# BOOST PATH
BOOSTDIR='/usr'
AC_ARG_WITH(boostdir,
  [  --with-boostdir=DIR          Boost install path (default in /usr)],
  [BOOSTDIR="$withval"])
AC_SUBST(BOOSTDIR)

BOOST_INCLUDE_DIR="$BOOSTDIR/include"
AC_ARG_WITH(boost_includedir,
  [  --with-boost-includedir=DIR  Boost include path (default in /usr/include)],
  [BOOST_INCLUDE_DIR="$withval"])
AC_SUBST(BOOST_INCLUDE_DIR)

BOOST_LIB_DIR="$BOOSTDIR/lib"
AC_ARG_WITH(boost_libdir,
  [  --with-boost-libdir=DIR      Boost library path (default in /usr/lib)],
  [BOOST_LIB_DIR="$withval"])
AC_SUBST(BOOST_LIB_DIR)

# additional Boost stuff: it encodes the compiler version into its lib names
BOOST_COMPILER_STRING=""
AC_ARG_WITH(boost_compiler_string,
  [  --with-boost-compiler-string=STR    Boost compiler string (default: "")],
  [BOOST_COMPILER_STRING="$withval"])
AC_SUBST(BOOST_COMPILER_STRING)


export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:${prefix}/lib/pkgconfig

# Check for SWI-Prolog (optional)
#-----------------------------------
#
# This is so convoluted because SWI prolog comes with many different names
PKG_CHECK_MODULES(PL, [pl >= 5.8.0],
    [AC_SUBST([SWIPL_PREFIX], [`$PKG_CONFIG pl --variable prefix`])
     AS_IF([test -x $SWIPL_PREFIX/bin/swipl],
           [AC_SUBST([SWIPL], ["${SWIPL_PREFIX}/bin/swipl"])],
           [AC_SUBST([SWIPL], ["${SWIPL_PREFIX}/bin/pl"])])
     AC_SUBST([SWIPL_VERSION], [`${SWIPL} --version | awk '{print $3}'`])
     AC_MSG_RESULT([using ${SWIPL}])
    ],
    [# alternate pkg-config name
     PKG_CHECK_MODULES(SWIPL, [swipl >= 5.8.0], 
        [AC_SUBST([SWIPL_PREFIX], [`$PKG_CONFIG swipl --variable prefix`])
         AC_SUBST([SWIPL], ["${SWIPL_PREFIX}/bin/swipl"])
         AC_SUBST([SWIPL_VERSION], [`${SWIPL} --version | awk '{print $3}'`])
	],
	[AC_MSG_NOTICE([Did not find SWI-Prolog version 5.8 or later! 
Please remember to set PKG_CONFIG_PATH accordingly.])
         AC_SUBST([SWIPL], [""])
        ])
    ])

AC_ARG_WITH([swi-prolog],
   [AS_HELP_STRING([--with-swi-prolog],
     [enable SWI-Prolog in Termite (recommended)  @<:@default=check@:>@])],
   :, [with_swi_prolog=check])
 
AS_IF([test "x$with_swi_prolog" != xno], dnl yes or autodetect
      [AS_IF([test -n "$SWIPL" -a -x "$SWIPL"], 
             [with_swi_prolog=yes],
      	     [AS_IF([test "x$with_swi_prolog" != xcheck],
                    [AC_MSG_FAILURE([--with-swi_prolog was given, but test for SWI Prolog failed])])],
                    [with_swi_prolog=no])])

AS_IF([test "x$with_swi_prolog" = xyes],
      [eval `$SWIPL --dump-runtime-variables |grep -v '^CC='`
       AC_SUBST([SWI_PL], ["$SWIPL"])
       AC_SUBST([SWI_PROLOG_LIBS], ["$PLLIBS"])
       AC_SUBST([SWI_PROLOG_LDADD], ["-L'$PLBASE/lib/$PLARCH' -Wl,-rpath '$PLBASE/lib/$PLARCH' $PLLIB $PLLIBS"])
       AC_SUBST([SWI_PROLOG_HOME_DIR], ["$PLBASE"])
       AC_SUBST([SWI_PROLOG_INCLUDE_DIR], ["$PLBASE/include"])
       AC_SUBST([SWI_PLC], ["$SWIPL -q -t main -O -L0 -G0 -T0"])
       AC_DEFINE([HAVE_SWI_PROLOG], [1], [Define if you have SWI Prolog])
       AC_MSG_RESULT([Termite will be built with SWI-Prolog support.])
       have_swi_prolog=1],
      [AC_SUBST([SWI_PL], ["false"])
       AC_SUBST([SWI_PROLOG_LDADD], [""])  # set a safe dummy default path
       AC_SUBST([SWI_PROLOG_INCLUDE_DIR], ["/usr/include"])
       AC_DEFINE([HAVE_SWI_PROLOG], [0], [Define if you have SWI Prolog])
       AC_MSG_RESULT([Termite will be built WITHOUT SWI-Prolog support.])
       have_swi_prolog=0])

AM_CONDITIONAL(WITH_SWI_PROLOG, [test "x$with_swi_prolog" = xyes])
AC_SUBST(have_swi_prolog)


# Check for CBMC model checker (optional)
#-----------------------------------
AC_ARG_WITH([cbmc],
   [AS_HELP_STRING([--with-cbmc],
     [enable CBMC model checker (optional) @<:@default=check@:>@])],
   [],
   [with_cbmc=check])
 
#MS:requires autoconf 2.61 (we want 2.59 to be sufficient)         
#AX_WITH_PROG(CBMC, cbmc-3-0-i686-linux)
AS_IF([test "x$with_cbmc" != xno],
      [AS_IF([test -x "$CBMC"],
            [with_cbmc=yes; CBMCHOME=`dirname $CBMC`/..],
            [AS_IF([test "x$with_cbmc" != xcheck],
                   [AC_MSG_FAILURE([--with-cbmc was given, but test for CBMC failed])],
                   [with_cbmc=no])])],
      [with_cbmc=no])

AS_IF([test "x$with_cbmc" = xyes],
      [AC_SUBST([CBMC], ["$CBMC"])
       AC_SUBST([CBMCHOME], ["$CBMCHOME"])
       AC_DEFINE([HAVE_CBMC], [1], [Define if you have CBMC])
       AC_MSG_RESULT([The CBMC Model Checker will be available to verify analysis results])
       have_cbmc=1],
      [AC_SUBST([CBMC], ["pcbmc.opt"])
       AC_SUBST([CBMCHOME], ["."])  # set a safe dummy default path
       AC_DEFINE([HAVE_CBMC], [0], [Define if you have CBMC])
       AC_MSG_RESULT([CBMC Model Checker support disabled.])
       have_cbmc=0])

AM_CONDITIONAL(WITH_CBMC, [test "x$with_cbmc" = xyes])
AC_SUBST(have_cbmc)


# Checks for libraries.
#-----------------------------------
AS_IF([test "x$with_pag" != xno],
      [AC_CHECK_LIB([pag], [pag_version],
           [AC_MSG_RESULT([found PAG])
            AC_DEFINE([HAVE_PAG], [1], [Define if you have PAG])],
           [AC_MSG_FAILURE(
      [PAG test failed (--without-pag to disable, --with-pag* to fix paths)])],
           [-L${PAG_LIB_DIR}])],
      [AC_DEFINE([HAVE_PAG], [0], [Define if you have PAG])
       AC_MSG_RESULT([skipping check for PAG])])
AM_CONDITIONAL(WITH_PAG, [test "x$with_pag" = xyes])

AC_CHECK_FILE(${BOOST_LIB_DIR}/libboost_filesystem${BOOST_COMPILER_STRING}.so,
    AC_MSG_RESULT([found Boost (hopefully)]),
    [echo "Could not find 'libboost_filesystem${BOOST_COMPILER_STRING}.so' in '${BOOST_LIB_DIR}'."
     echo "Please consider using one of the --with-boost* options."
     exit -1 ])

AC_CHECK_LIB([rose], [ROSE_NUMERIC_VERSION], AC_MSG_RESULT([found ROSE]),
    [echo "Could not find '-lrose' in '-L${ROSE_LIB_DIR}'."
     echo "Please consider using one of the --with-rose* options."
     exit -1 ],
    [-Wl,--rpath -Wl,${BOOST_LIB_DIR} -L${ROSE_LIB_DIR}])

AC_CONFIG_FILES([
Makefile
docs/Makefile
docs/manual/manual_vars.tex
src/termite/Makefile
src/termite/satire/termite.h
src/termite/clang/BUILD-clang.sh
src/analyzer/annotation/Makefile
src/analyzer/astaccess/Makefile
src/analyzer/astnumbering/Makefile
src/analyzer/commandlineinterface/Makefile
src/analyzer/icfgbuilder/Makefile
src/analyzer/skeletons/Makefile
src/analyzer/support/Makefile
src/analyzer/provided/pointsto/Makefile
src/analyzer/provided/dataflow/Makefile
src/analyzer/provided/aslan/Makefile
src/analyzer/provided/Makefile
src/analyzer/Makefile
src/grato/Makefile
src/pig/Makefile
src/satire/satire_rose.h
src/aral/Makefile
src/aral/src/Makefile
src/Makefile
examples/Makefile
tools/Makefile
tools/annotcheck/Makefile
tools/icfgbuilder/Makefile
tools/loopbounds/Makefile
tools/optimizer/Makefile
tools/profiler/Makefile
tools/unparser/Makefile
tools/wcetcc/Makefile
])

AC_OUTPUT

# SUMMARY
SWI_STATUS="no"
if [[ x$with_swi_prolog = xyes ]]; then
    SWI_STATUS="yes ($SWIPL_VERSION in $SWI_PROLOG_HOME_DIR)"
fi

echo
echo "------------------------------------------------------------------------"
echo "  SATIrE configuration summary:"
echo "    PAG .................." $with_pag
echo "    SWI-Prolog............" $SWI_STATUS
echo "    CBMC ................." $with_cbmc
echo "------------------------------------------------------------------------"
