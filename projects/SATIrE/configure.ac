#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(SATIrE, 0.7.8, markus@complang.tuwien.ac.at)
AC_CONFIG_SRCDIR([README])

AM_INIT_AUTOMAKE


# Checks for programs.
AC_PROG_YACC
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LEX
AC_PROG_RANLIB

# ROSE PATH
ROSEDIR='/opt/rose'
AC_ARG_WITH(rosedir, [  --with-rosedir=DIR             ROSE install path (default in /opt/rose)], [ROSEDIR="$withval"])
AC_SUBST(ROSEDIR)

ROSE_INCLUDE_DIR="$ROSEDIR/include"
AC_ARG_WITH(rose_includedir, [  --with-rose-includedir=DIR     ROSE include path (default in /opt/rose/include)], [ROSE_INCLUDE_DIR="$withval"])
AC_SUBST(ROSE_INCLUDE_DIR)

ROSE_LIB_DIR="$ROSEDIR/lib"
AC_ARG_WITH(rose_libdir, [  --with-rose-libdir=DIR         ROSE library path (default in /opt/rose/lib)], [ROSE_LIB_DIR="$withval"])
AC_SUBST(ROSE_LIB_DIR)

# PAG PATH
PAGDIR='/opt/pag'
AC_ARG_WITH(pagdir, [  --with-pagdir=DIR             PAG install path (default in /opt/pag)], [PAGDIR="$withval"])
AC_SUBST(PAGDIR)

PAG_INCLUDE_DIR="$PAGDIR/include"
AC_ARG_WITH(pag_includedir, [  --with-pag-includedir=DIR     PAG include path (default in /opt/pag/include)], [PAG_INCLUDE_DIR="$withval"])
AC_SUBST(PAG_INCLUDE_DIR)

PAG_LIB_DIR="$PAGDIR/lib/pag"
AC_ARG_WITH(pag_libdir, [  --with-pag-libdir=DIR         PAG library path (default in /opt/pag/lib)], [PAG_LIB_DIR="$withval"])
AC_SUBST(PAG_LIB_DIR)


# Checks for libraries.
# FIXME: Replace `main' with a function in `-ledg':
#AC_CHECK_LIB([edg], [main])
AC_SEARCH_LIBS([fcosf], [m])
AC_SEARCH_LIBS([clock_gettime], [rt])
AC_SEARCH_LIBS([yywrap], [fl])
AC_CHECK_LIB([pag], [pag_version], [echo "Found PAG!"], 
             [echo "Could not find '-lpag' in '-L${PAG_LIB_DIR}'."
              echo "Please consider using one of the --with-pag* options."
              exit -1 ],
              [-L${PAG_LIB_DIR}])
AC_CHECK_LIB([rose], [PDF_close], [echo "Found ROSE!"], 
             [echo "Could not find '-lrose' in '-L${ROSE_LIB_DIR}'."
	      echo "Please consider using one of the --with-rose* options."
	      exit -1 ], 
             [-L${ROSE_LIB_DIR} -lfl])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_STRUCT_TM
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([
Makefile
src/termite/Makefile
src/analyzer/annotation/Makefile
src/analyzer/astaccess/Makefile
src/analyzer/commandlineinterface/Makefile
src/analyzer/icfgbuilder/Makefile
src/analyzer/skeletons/Makefile
src/analyzer/Makefile
src/grato/Makefile
src/pig/Makefile
src/Makefile
examples/Makefile
tools/Makefile
tools/icfgbuilder/Makefile
])


AC_OUTPUT
