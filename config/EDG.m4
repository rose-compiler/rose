AC_DEFUN([SETUP_EDG], [

# SAGE_VAR_INCLUDES_AND_LIBS
AC_DEFINE([SAGE_TRANSFORM],[1],[Control use of SAGE within EDG.])

# AC_AIX

# Checks for programs.

# LAC_SAGE_PATH
# LAC_COMPILERS_ARGS
# LAC_CXX_TYPE
# LAC_CXX_VERSION
# LAC_COMPILERS_TARGET(no)

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h sys/time.h unistd.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# This screws up the C++ compiler, it tests to see if the C compiler supports the inline keyword
# (usually not) and then defines inline to be empty which interferes with the processing of inline
# code used with the C++ compiler. So comment it out and handle it within EDG if it is an issue.
# This is also listed as an obsolete construct within the new autoconf 2.52.
# AC_C_INLINE

AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_STRUCT_TM

# AC_DIAGNOSE([obsolete],[instead of using `AC_LANG', `AC_LANG_SAVE',
# and `AC_LANG_RESTORE', you should use `AC_LANG_PUSH' and `AC_LANG_POP'.])

AC_C_CHAR_UNSIGNED
AC_C_LONG_DOUBLE

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(void *)
AC_CHECK_SIZEOF(long double)
AC_CHECK_SIZEOF(long long)

# Checks for library functions.
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_SETVBUF_REVERSED
AC_TYPE_SIGNAL
AC_FUNC_STRCOLL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(getcwd getwd mktime strcspn strerror strftime strspn strstr strtod strtol strtoul)

# CC++ specific macros
OBJ=".o"
AC_SUBST(OBJ)

EXE=""
AC_SUBST(EXE)

AC_ARG_ENABLE(purify,
[  --enable-purify use memory managment that purify can understand],
[if test $enable_purify = yes; then
     AC_DEFINE([USING_PURIFY],[],[Support for PURIFY debugging software.])
 fi])

# AC_ARG_ENABLE(ccpp,
# [  --disable-ccpp don't support CC++ extensions],
# [if test $enable_cc++ = yes; then
#      AC_DEFINE([CCPP_EXTENSIONS_ALLOWED],[],[Support for CC++.])
#  fi],[AC_DEFINE([CCPP_EXTENSIONS_ALLOWED],[],[Support for CC++.])])

consistancy_checking_define_value=0
AC_ARG_ENABLE(checking,
[  --disable-checking don't do consistancy checking in parser],
[if test $enable_checking = yes; then
    # AC_DEFINE([CHECKING],[1],[Internal consistancy tests.])
      consistancy_checking_define_value=0
 fi],[
 # AC_DEFINE([CHECKING],[0],[Internal consistancy tests.])
   consistancy_checking_define_value=1
])

AC_DEFINE_UNQUOTED([CHECKING],[$consistancy_checking_define_value],[Internal consistancy tests.])

AC_ARG_ENABLE(stand-alone,
	[  --enable-stand-alone compile standalone edgcpfe],
	[standalone="$enableval"],
	[standalone="no"]
)

AC_ARG_ENABLE(cp-backend,
	[  --enable-cp-backend generate c++ code as output],
	[cp_backend="$enableval"],
	[cp_backend="no"]
)

AC_ARG_ENABLE(sage-backend,
	[  --enable-sage-backend generate sage++ tree],
	[sage_backend="$enableval"],
	[sage_backend="no"]
)

if test "$cp_backend" = "yes" -a "$sage_backend" = "yes"; then 
  AC_MSG_ERROR(Cannot have both sage and c++ backend)
fi

# Initialized values
sage_backend_defined_value=0
cp_backend_defined_value=0
build_parser_as_lib_defined_value=0

build_stand_alone_C_generator_defined_value=0
build_stand_alone_CP_generator_defined_value=0
build_stand_alone_SAGE_generator_defined_value=0

if test "$cp_backend" = "yes"; then 
  # AC_DEFINE([BACK_END_IS_CP_GEN_BE],1,[Define use of C++ generating backend (not used ecxcept for debugging).])
  GEN_BE="cp_gen_be$OBJ"
  cp_backend_defined_value=1
else
  # AC_DEFINE([BACK_END_IS_SAGE_GEN_BE],1,[Define use of SAGE specific backend to generate the SAGE AST.])
  GEN_BE="sage_gen_be$OBJ"
  sage_backend_defined_value=1
  if test "$standalone"="no"; then
     # AC_DEFINE([BUILDING_PARSER_AS_LIB],[1],[Build parser as a standalone tool (not connected to SAGE).])
     build_parser_as_lib_defined_value=1
  fi
fi

AC_SUBST(GEN_BE)

dnl *********************************************************************
dnl * Set up to use new EDG within ROSE
dnl *********************************************************************
dnl AC_ARG_WITH(EDG_1_4, [  --with-EDG_1_4    use old version of EDG version 1.4 within ROSE (used within SAGE II)])
dnl if test "$with_EDG_1_4" = yes; then
dnl   # EDG_3_3 was not activated so use EDG 1.4 (used in SAGE II)
dnl   EDG_LIBS='$(top_builddir)/src/frontend/EDG/EDG_1.4/src/libedg14.a'
dnl else
dnl   # EDG_3_3 was activated (use this newest version of EDG (as of 1/8/2004))
dnl   # EDG_LIBS='$(top_builddir)/EDG_3.3/src/libedg.a $(top_builddir)/src/EDG_Connection/libedgSage.a'
dnl   EDG_LIBS='$(top_builddir)/src/frontend/EDG/EDG_3.3/src/libedg33.a $(top_builddir)/src/frontend/EDG_SAGE_Connection/libedgSage.a'
dnl fi


# EDG_3_3 was activated (use this newest version of EDG (as of 1/8/2004) it is now the default)
# EDG_LIBS='$(top_builddir)/src/frontend/EDG/EDG_3.3/src/libedg33.a $(top_builddir)/src/frontend/EDG_SAGE_Connection/libedgSage.a'

# DQ (1/14/2007): I am not clear if this is used now that we use libtool.
EDG_LIBS='$(top_builddir)/src/frontend/EDG/EDG_3.3/util/libedgutil.a $(top_builddir)/src/frontend/EDG/EDG_3.3/src/libedg33.a $(top_builddir)/src/frontend/EDG_SAGE_Connection/libedgSage.a'

AC_SUBST(EDG_LIBS)
if test -n "$EDG_1_4"; then EDG_1_4="$EDG_1_4"; fi
if test -n "$EDG_LIBS"; then EDG_LIBS="$EDG_LIBS"; fi

# echo "Am I set: EDG_1_4 = $EDG_1_4"
# echo "Am I set: EDG_LIBS = $EDG_LIBS"

# AC_DEFINE_UNQUOTED([BACK_END_IS_CP_GEN_BE],
#          [$cp_backend_defined_value],
#          [Define use of C++ generating backend (not used ecxcept for debugging).])
# AC_DEFINE_UNQUOTED([BACK_END_IS_SAGE_GEN_BE],
#          [$sage_backend_defined_value],
#          [Define use of SAGE specific backend to generate the SAGE AST.])
# AC_DEFINE_UNQUOTED([BACK_END_IS_C_GEN_BE],[0],[Control use of backen to generate C code.])
# AC_DEFINE_UNQUOTED([BUILDING_PARSER_AS_LIB],
#          [$build_parser_as_lib_defined_value],
#          [Build parser as a standalone tool (not connected to SAGE).])

# AC_DEFINE_UNQUOTED([STANDALONE_C_GEN_BE],[$build_stand_alone_C_generator_defined_value],[Build standalone parser.])
# AC_DEFINE_UNQUOTED([STANDALONE_CP_GEN_BE],[$build_stand_alone_CP_generator_defined_value],[Build standalone parser.])
# AC_DEFINE_UNQUOTED([STANDALONE_SAGE_GEN_BE],[$build_stand_alone_SAGE_generator_defined_value],[Build standalone parser.])

# AC_DEFINE([AUTOMATIC_TEMPLATE_INSTANTIATION],[1],[template support])
# AC_DEFINE([CLASS_TEMPLATE_INSTANTIATIONS_IN_SOURCE_SEQUENCE_LISTS],[1],[include template info in AST])
# AC_DEFINE([NONCLASS_TEMPLATE_INSTANTIATIONS_IN_SOURCE_SEQUENCE_LISTS],[1],[include template info in AST])
# AC_DEFINE([USE_LONG_LONG],[1],[use long long integers])
# AC_DEFINE([RESTRICT_ALLOWED],[1],[restrict support])

])










