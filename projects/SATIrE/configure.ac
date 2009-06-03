#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(SATIrE, 0.8.5, markus@complang.tuwien.ac.at)
AC_CONFIG_SRCDIR([README])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE


# Checks for programs.
AC_PROG_YACC
AS_IF([test "x$YACC" = x],[AC_MSG_FAILURE([Please install bison])])
AC_PROG_CXX
AS_IF([test "x$CXX" = x],[AC_MSG_FAILURE([Please install g++])])
AC_PROG_CC
AS_IF([test "x$CC" = x],[AC_MSG_FAILURE([Please install gcc])])
AC_PROG_LEX
AS_IF([test "x$LEX" = x],[AC_MSG_FAILURE([Please install flex])])
dnl AC_PROG_RANLIB
dnl AS_IF([test "x$RANLIB" = x],[AC_MSG_FAILURE([Please install ranlib])])
AC_PROG_SED
AS_IF([test "x$SED" = x],[AC_MSG_FAILURE([Please install sed])])
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
BOOST_COMPILER_STRING="gcc42"
AC_ARG_WITH(boost_compiler_string,
           [  --with-boost-compiler-string=STR    Boost compiler string (default: gcc42)],
           [BOOST_COMPILER_STRING="$withval"])


# Check for SWI-Prolog (optional)
#-----------------------------------
if pl --version |grep -q SWI-Prolog; then 
   PL=pl    # Linux
elif xpce --version |grep -q SWI-Prolog; then
   PL=xpce  # Ubuntu (with GUI support, PCE profiling library)
elif swipl --version |grep -q SWI-Prolog; then
   PL=swipl # OS X, Ubuntu (without GUI or PCE)
elif prolog --version |grep -q SWI-Prolog; then
   PL=prolog  # Ubuntu (without GUI or PCE)
fi
PLBASE=`$PL --dump-runtime-variables |grep PLBASE |sed 's/"/ /g' |awk '{print $2}'`
PLARCH=`$PL --dump-runtime-variables |grep PLARCH |sed 's/"/ /g' |awk '{print $2}'`
PLLIBS=`$PL --dump-runtime-variables |grep PLLIBS |sed 's/"/ /g' |awk -F= '{print $2}' |sed 's/;//g'`
PLCFLAGS=`$PL --dump-runtime-variables |grep PLCFLAGS |sed 's/"/ /g' |awk '{print $2}'`
PLLIB="$PLBASE/lib/$PLARCH"
AC_ARG_WITH([swi-prolog],
            [AS_HELP_STRING([--with-swi-prolog],
              [enable SWI-Prolog in Termite (recommended)  @<:@default=check@:>@])],
            [],
            [with_swi_prolog=check])
          
AS_IF([test "x$with_swi_prolog" != xno],
      [AC_CHECK_LIB([pl], [PL_toplevel],
                    [with_swi_prolog=yes],
                    [if test "x$with_swi_prolog" != xcheck; then
                       AC_MSG_FAILURE([--with-swi_prolog was given, but test for SWI Prolog failed])
                     else
		       with_swi_prolog=no
                     fi],
                    [$PLCFLAGS $PLLIBS -L$PLLIB])],
      [with_swi_prolog=no])

AS_IF([test "x$with_swi_prolog" = xyes],
      [AC_SUBST([SWI_PL], ["$PL"])
       AC_SUBST([SWI_PROLOG_LIBS], ["$PLLIBS"])
       AC_SUBST([SWI_PROLOG_LDADD], ["-Wl,-rpath $PLLIB -L$PLLIB -lpl"])
       AC_SUBST([SWI_PROLOG_HOME_DIR], ["$PLBASE"])
       AC_SUBST([SWI_PROLOG_INCLUDE_DIR], ["$PLBASE/include"])
       AC_DEFINE([HAVE_SWI_PROLOG], [1], [Define if you have SWI Prolog])
       AC_MSG_RESULT([Termite will be built with SWI-Prolog support.])
       have_swi_prolog=1],
      [AC_SUBST([SWI_PL], ["pl"])
       AC_SUBST([SWI_PROLOG_LDADD], [""])  # set a safe dummy default path
       AC_SUBST([SWI_PROLOG_INCLUDE_DIR], ["/usr/include"])
       AC_DEFINE([HAVE_SWI_PROLOG], [0], [Define if you have SWI Prolog])
       AC_MSG_RESULT([Termite will be built WITHOUT SWI-Prolog support.])
       have_swi_prolog=0])

AM_CONDITIONAL(WITH_SWI_PROLOG, [test "x$with_swi_prolog" = xyes])
AC_SUBST(have_swi_prolog)


# Check for CBMC model checker (optional)
#-----------------------------------
CBMC=`which cbmc-2-9-i686-linux`
CBMCHOME=`dirname $CBMC`/..

AC_ARG_WITH([cbmc],
            [AS_HELP_STRING([--with-cbmc],
              [enable CBMC model checker (optional) @<:@default=check@:>@])],
            [],
            [with_cbmc=check])
          
AS_IF([test "x$with_cbmc" != xno],
      AS_IF([test "x$CBMC" != x],
            [with_cbmc=yes],
            [if test "x$with_cbmc" != xcheck; then
             AC_MSG_FAILURE([--with-cbmc was given, but test for CBMC failed])
	     else
	       with_cbmc=no
             fi]),
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
      [AC_MSG_RESULT([skipping check for PAG])])
AM_CONDITIONAL(WITH_PAG, [test "x$with_pag" = xyes])

AC_CHECK_FILE(${BOOST_LIB_DIR}/libboost_filesystem-${BOOST_COMPILER_STRING}-mt.so,
             AC_MSG_RESULT([found Boost (hopefully)]),
             [echo "Could not find 'libboost_filesystem-${BOOST_COMPILER_STRING}-mt.so' in '${BOOST_LIB_DIR}'."
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
src/analyzer/annotation/Makefile
src/analyzer/astaccess/Makefile
src/analyzer/astnumbering/Makefile
src/analyzer/commandlineinterface/Makefile
src/analyzer/icfgbuilder/Makefile
src/analyzer/skeletons/Makefile
src/analyzer/support/Makefile
src/analyzer/provided/pointsto/Makefile
src/analyzer/provided/dataflow/Makefile
src/analyzer/provided/Makefile
src/analyzer/Makefile
src/grato/Makefile
src/pig/Makefile
src/Makefile
examples/Makefile
tools/Makefile
tools/icfgbuilder/Makefile
tools/loopbounds/Makefile
tools/loopbounds/lb.sh
])


AC_OUTPUT

# SUMMARY

SWI_STATUS="disabled"
if [[ x$with_swi_prolog = xyes ]]; then
    SWI_STATUS="enabled"
fi

PAG_STATUS="disabled"
if [[ x$with_pag = xyes ]]; then
    PAG_STATUS="enabled"
fi

CBMC_STATUS="disabled"
if [[ x$with_cbmc = xyes ]]; then
    CBMC_STATUS="enabled"
fi

echo
echo "------------------------------------------------------------------------"
echo "  SATIrE configuration summary:"
echo "    PAG .................." $PAG_STATUS
echo "    SWI-Prolog............" $SWI_STATUS
echo "    CBMC ................." $CBMC_STATUS
echo "------------------------------------------------------------------------"
