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
   -o "x$rose_boost_version" = "x105300" -o "x$_version" = "x1.53"
then
    echo "Reasonable version of Boost found!"
else
  if test "x$ROSE_ENABLE_BOOST_VERSION_CHECK" = "xyes"; then
    ROSE_MSG_ERROR([Unsupported version of Boost: '$rose_boost_version'. Only 1.45 to 1.53 is currently supported.])
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

