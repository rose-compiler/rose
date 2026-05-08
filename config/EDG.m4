AC_DEFUN([SETUP_EDG], [

# SAGE_VAR_INCLUDES_AND_LIBS
AC_DEFINE([SAGE_TRANSFORM],[1],[Control use of SAGE within EDG.])

# Checks for header files.
AC_HEADER_DIRENT
AC_CHECK_HEADERS(limits.h sys/time.h unistd.h)
AC_DEFINE([STDC_HEADERS], [1], [Define to 1 if you have ANSI C headers.])

AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_STRUCT_TM

AC_C_CHAR_UNSIGNED

AC_CHECK_TYPES([long double])

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

consistency_checking_define_value=0
AC_ARG_ENABLE(checking,
[  --disable-checking     don't do EDG specific consistency checking in parser],
[if test $enable_checking = yes; then
      consistency_checking_define_value=0
 fi],[
   consistency_checking_define_value=1
])

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
  GEN_BE="cp_gen_be$OBJ"
  cp_backend_defined_value=1
else
  GEN_BE="sage_gen_be$OBJ"
  sage_backend_defined_value=1
  if test "$standalone"="no"; then
     build_parser_as_lib_defined_value=1
  fi
fi

AC_SUBST(GEN_BE)

])










