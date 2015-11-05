##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_BOOST([])
#
# DESCRIPTION
#
#   Handle Boost C++ Library settings.
#
#   This macro calls:
#
#     AC_SUBST(ac_boost_path)
#
#   And sets:
#
#     Automake conditionals:
#     AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_X_XX)
#
#     CPP #defines:
#     AC_DEFINE_UNQUOTED(ROSE_BOOST_PATH)
#     AC_DEFINE_UNQUOTED(ROSE_WAVE_PATH)
#
# LAST MODIFICATION
#
#   2013-07-31
#
# COPYLEFT
#
#   Copyright (c) 2013 Justin Too <too1@llnl.gov>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([ROSE_SUPPORT_BOOST],
[
  ROSE_CONFIGURE_SECTION([Boost C++ Libraries])


  #============================================================================
  # --with-x10=/path/to/x10-trunk/x10.dist/
  #============================================================================
  ROSE_ARG_ENABLE(
    [boost-version-check],
    [if we should validate your Boost version],
    [validate version of Boost C++ libraries (default: yes)],
    [yes]
  )

AX_BOOST_BASE([1.45.0], [], [echo "Boost 1.45.0 - 1.47.0 is required for ROSE" 1>&2; exit 1])

dnl Hack using an internal variable from AX_BOOST_BASE -- this path should only
dnl be used to set --with-boost in distcheck.
AC_SUBST(ac_boost_path)

rose_boost_version=`grep "#define BOOST_VERSION " ${ac_boost_path}/include/boost/version.hpp | cut -d" " -f 3 | tr -d '\r'`

AC_MSG_NOTICE([rose_boost_version = '$rose_boost_version'])

if test "x$rose_boost_version" = "x"; then
  ROSE_MSG_ERROR([Unable to compute the version of your Boost C++ libraries: '$ac_boost_path'])
fi

# DQ (10/22/2015): Added more tests (1.55 through 1.62)
# Define macros for conditional compilation of parts of ROSE based on version of boost
# (this ONLY happens for the tests in tests/CompilerOptionsTests/testWave)
#
# !! We don't want conditional compilation or code in ROSE based on version numbers of Boost. !!
#
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_35, test "x$rose_boost_version" = "x103500" -o "x$_version" = "x1.35")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_36, test "x$rose_boost_version" = "x103600" -o "x$_version" = "x1.36")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_37, test "x$rose_boost_version" = "x103700" -o "x$_version" = "x1.37")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_38, test "x$rose_boost_version" = "x103800" -o "x$_version" = "x1.38")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_39, test "x$rose_boost_version" = "x103900" -o "x$_version" = "x1.39")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_40, test "x$rose_boost_version" = "x104000" -o "x$_version" = "x1.40")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_41, test "x$rose_boost_version" = "x104100" -o "x$_version" = "x1.41")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_42, test "x$rose_boost_version" = "x104200" -o "x$_version" = "x1.42")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_43, test "x$rose_boost_version" = "x104300" -o "x$_version" = "x1.43")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_44, test "x$rose_boost_version" = "x104400" -o "x$_version" = "x1.44")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_45, test "x$rose_boost_version" = "x104500" -o "x$_version" = "x1.45")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_46, test "x$rose_boost_version" = "x104600" -o "x$_version" = "x1.46")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_46, test "x$rose_boost_version" = "x104601" -o "x$_version" = "x1.46")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_47, test "x$rose_boost_version" = "x104700" -o "x$_version" = "x1.47")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_48, test "x$rose_boost_version" = "x104800" -o "x$_version" = "x1.48")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_49, test "x$rose_boost_version" = "x104900" -o "x$_version" = "x1.49")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_50, test "x$rose_boost_version" = "x105000" -o "x$_version" = "x1.50")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_51, test "x$rose_boost_version" = "x105100" -o "x$_version" = "x1.51")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_52, test "x$rose_boost_version" = "x105200" -o "x$_version" = "x1.52")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_53, test "x$rose_boost_version" = "x105300" -o "x$_version" = "x1.53")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_54, test "x$rose_boost_version" = "x105400" -o "x$_version" = "x1.54")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_55, test "x$rose_boost_version" = "x105500" -o "x$_version" = "x1.55")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_56, test "x$rose_boost_version" = "x105600" -o "x$_version" = "x1.56")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_57, test "x$rose_boost_version" = "x105700" -o "x$_version" = "x1.57")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_58, test "x$rose_boost_version" = "x105800" -o "x$_version" = "x1.58")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_59, test "x$rose_boost_version" = "x105900" -o "x$_version" = "x1.59")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_60, test "x$rose_boost_version" = "x106000" -o "x$_version" = "x1.60")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_61, test "x$rose_boost_version" = "x106100" -o "x$_version" = "x1.61")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_62, test "x$rose_boost_version" = "x106200" -o "x$_version" = "x1.62")

# DQ (10/22/2015): Added more tests (1.55 through 1.62)
# TOO1 (3/16/2015):
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_35, test $rose_boost_version -ge 103500)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_36, test $rose_boost_version -ge 103600)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_37, test $rose_boost_version -ge 103700)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_38, test $rose_boost_version -ge 103800)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_39, test $rose_boost_version -ge 103900)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_40, test $rose_boost_version -ge 104000)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_41, test $rose_boost_version -ge 104100)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_42, test $rose_boost_version -ge 104200)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_43, test $rose_boost_version -ge 104300)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_44, test $rose_boost_version -ge 104400)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_45, test $rose_boost_version -ge 104500)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_46, test $rose_boost_version -ge 104600)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_46, test $rose_boost_version -ge 104601)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_47, test $rose_boost_version -ge 104700)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_48, test $rose_boost_version -ge 104800)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_49, test $rose_boost_version -ge 104900)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_50, test $rose_boost_version -ge 105000)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_51, test $rose_boost_version -ge 105100)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_52, test $rose_boost_version -ge 105200)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_53, test $rose_boost_version -ge 105300)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_54, test $rose_boost_version -ge 105400)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_55, test $rose_boost_version -ge 105500)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_56, test $rose_boost_version -ge 105600)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_57, test $rose_boost_version -ge 105700)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_58, test $rose_boost_version -ge 105800)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_59, test $rose_boost_version -ge 105900)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_60, test $rose_boost_version -ge 106000)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_61, test $rose_boost_version -ge 106100)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_62, test $rose_boost_version -ge 106200)

# DQ (10/22/2015): Added acceptable case 1.54.
if test \
      "x$rose_boost_version" = "x104500" -o "x$_version" = "x1.45" \
   -o "x$rose_boost_version" = "x104600" -o "x$_version" = "x1.46" \
   -o "x$rose_boost_version" = "x104601" -o "x$_version" = "x1.46" \
   -o "x$rose_boost_version" = "x104700" -o "x$_version" = "x1.47" \
   -o "x$rose_boost_version" = "x104800" -o "x$_version" = "x1.48" \
   -o "x$rose_boost_version" = "x104900" -o "x$_version" = "x1.49" \
   -o "x$rose_boost_version" = "x105000" -o "x$_version" = "x1.50" \
   -o "x$rose_boost_version" = "x105100" -o "x$_version" = "x1.51" \
   -o "x$rose_boost_version" = "x105200" -o "x$_version" = "x1.52" \
   -o "x$rose_boost_version" = "x105300" -o "x$_version" = "x1.53" \
   -o "x$rose_boost_version" = "x105400" -o "x$_version" = "x1.54"
then
    echo "Reasonable version of Boost found!"
else
  if test "x$ROSE_ENABLE_BOOST_VERSION_CHECK" = "xyes"; then
    ROSE_MSG_ERROR([Unsupported version of Boost: '$rose_boost_version'. Only 1.45 to 1.54 is currently supported.])
  else
    AC_MSG_WARN([Unsupported version of Boost is being used])
  fi
fi

# DQ (12/22/2008): Fix boost configure to handle OS with older version of Boost that will
# not work with ROSE, and use the newer version specified by the user on the configure line.
echo "In ROSE/configure: ac_boost_path = $ac_boost_path"
#AC_DEFINE([ROSE_BOOST_PATH],"$ac_boost_path",[Location of Boost specified on configure line.])
AC_DEFINE_UNQUOTED([ROSE_BOOST_PATH],"$ac_boost_path",[Location (unquoted) of Boost specified on configure line.])
#AC_DEFINE([ROSE_WAVE_PATH],"$ac_boost_path/wave",[Location of Wave specified on configure line.])
AC_DEFINE_UNQUOTED([ROSE_WAVE_PATH],"$ac_boost_path/wave",[Location (unquoted) of Wave specified on configure line.])

AC_DEFINE_UNQUOTED([ROSE_BOOST_VERSION], $rose_boost_version, [Version of Boost specified on configure line.])

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

# End macro ROSE_SUPPORT_BOOST.
])

