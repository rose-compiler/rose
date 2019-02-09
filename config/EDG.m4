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
# George Vulov: This macro seems to cause nothing but trouble and it's only used for ancient compilers
# AC_C_CONST

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
[  --enable-purify        use memory managment that purify can understand],
[if test $enable_purify = yes; then
     AC_DEFINE([USING_PURIFY],[],[Support for PURIFY debugging software.])
 fi])

# AC_ARG_ENABLE(ccpp,
# [  --disable-ccpp don't support CC++ extensions],
# [if test $enable_cc++ = yes; then
#      AC_DEFINE([CCPP_EXTENSIONS_ALLOWED],[],[Support for CC++.])
#  fi],[AC_DEFINE([CCPP_EXTENSIONS_ALLOWED],[],[Support for CC++.])])

consistency_checking_define_value=0
AC_ARG_ENABLE(checking,
[  --disable-checking     don't do EDG specific consistency checking in parser],
[if test $enable_checking = yes; then
    # AC_DEFINE([CHECKING],[1],[Internal consistency tests.])
      consistency_checking_define_value=0
 fi],[
 # AC_DEFINE([CHECKING],[0],[Internal consistency tests.])
   consistency_checking_define_value=1
])

# DQ (1/6/2009): Made the comment more clear that this is an EDG feature.
AC_DEFINE_UNQUOTED([CHECKING],[$consistency_checking_define_value],[Internal EDG specific consistency tests.])

AC_ARG_ENABLE(stand-alone,
	[  --enable-stand-alone compile standalone edgcpfe],
	[standalone="$enableval"],
	[standalone="no"]
)

AC_ARG_ENABLE(cp-backend,
	[  --enable-cp-backend  generate c++ code as output],
	[cp_backend="$enableval"],
	[cp_backend="no"]
)

AC_ARG_ENABLE(sage-backend,
	[  --enable-sage-backend generate sage++ tree],
	[sage_backend="$enableval"],
	[sage_backend="no"]
)

if test "$cp_backend" = "yes" -a "$sage_backend" = "yes"; then 
  AC_MSG_ERROR(cannot have both sage and c++ backend)
fi

# Initialized values
sage_backend_defined_value=0
cp_backend_defined_value=0
build_parser_as_lib_defined_value=0

build_stand_alone_C_generator_defined_value=0
build_stand_alone_CP_generator_defined_value=0
build_stand_alone_SAGE_generator_defined_value=0

if test "$cp_backend" = "yes"; then 
  # AC_DEFINE([BACK_END_IS_CP_GEN_BE],1,[Define use of C++ generating backend (not used except for debugging).])
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

])










