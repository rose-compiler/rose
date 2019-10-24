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
# COPYLEFT
#
#   Copyright (c) 2013 Justin Too <too1@llnl.gov>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([ROSE_SUPPORT_BOOST],
[
  ROSE_CONFIGURE_SECTION([Checking Boost])


  #============================================================================
  # --enable-boost-version-check
  #============================================================================
  ROSE_ARG_ENABLE(
    [boost-version-check],
    [if we should validate your Boost version],
    [validate version of Boost C++ libraries (default: yes)],
    [yes]
  )

AX_BOOST_BASE([1.51.0], [], [echo "Boost 1.51.0 - 1.61.0 except 1.54 is required for ROSE" 1>&2; exit 1])

dnl Hack using an internal variable from AX_BOOST_BASE -- this path should only
dnl be used to set --with-boost in distcheck.
AC_SUBST(ac_boost_path)

BOOST_VERSION_HEADER="${ac_boost_path}/include/boost/version.hpp"

# Liao, 2019/3/12. Check file existence before calling grep
if test -f "$BOOST_VERSION_HEADER" ; then
  rose_boost_version=`grep "#define BOOST_VERSION " ${ac_boost_path}/include/boost/version.hpp | cut -d" " -f 3 | tr -d '\r'`
else
  ROSE_MSG_ERROR([Unable to find $ac_boost_path/include/boost/version.hpp . Please specify the right boost installation path with --with-boost=/path/to/boost])
fi

AC_MSG_NOTICE([rose_boost_version = '$rose_boost_version'])
ROSE_CONFIG_TOKEN="$ROSE_CONFIG_TOKEN boost-$rose_boost_version"

if test "x$rose_boost_version" = "x"; then
  ROSE_MSG_ERROR([Unable to compute the version of your Boost C++ libraries from '$ac_boost_path'/include/boost/version.hpp . Please make sure the file exists or specify the right path with --with-boost])
fi

# DQ (10/22/2015): Added more tests (1.55 through 1.62)
# Define macros for conditional compilation of parts of ROSE based on version of boost
# (this ONLY happens for the tests in tests/nonsmoke/functional/CompilerOptionsTests/testWave)
#
# !! We don't want conditional compilation or code in ROSE based on version numbers of Boost. !!
#
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_35, test "$rose_boost_version" = "103500" -o "$_version" = "1.35")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_36, test "$rose_boost_version" = "103600" -o "$_version" = "1.36")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_37, test "$rose_boost_version" = "103700" -o "$_version" = "1.37")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_38, test "$rose_boost_version" = "103800" -o "$_version" = "1.38")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_39, test "$rose_boost_version" = "103900" -o "$_version" = "1.39")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_40, test "$rose_boost_version" = "104000" -o "$_version" = "1.40")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_41, test "$rose_boost_version" = "104100" -o "$_version" = "1.41")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_42, test "$rose_boost_version" = "104200" -o "$_version" = "1.42")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_43, test "$rose_boost_version" = "104300" -o "$_version" = "1.43")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_44, test "$rose_boost_version" = "104400" -o "$_version" = "1.44")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_45, test "$rose_boost_version" = "104500" -o "$_version" = "1.45")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_46, test "$rose_boost_version" = "104600" -o "$_version" = "1.46")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_46, test "$rose_boost_version" = "104601" -o "$_version" = "1.46")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_47, test "$rose_boost_version" = "104700" -o "$_version" = "1.47")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_48, test "$rose_boost_version" = "104800" -o "$_version" = "1.48")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_49, test "$rose_boost_version" = "104900" -o "$_version" = "1.49")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_50, test "$rose_boost_version" = "105000" -o "$_version" = "1.50")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_51, test "$rose_boost_version" = "105100" -o "$_version" = "1.51")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_52, test "$rose_boost_version" = "105200" -o "$_version" = "1.52")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_53, test "$rose_boost_version" = "105300" -o "$_version" = "1.53")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_54, test "$rose_boost_version" = "105400" -o "$_version" = "1.54")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_55, test "$rose_boost_version" = "105500" -o "$_version" = "1.55")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_56, test "$rose_boost_version" = "105600" -o "$_version" = "1.56")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_57, test "$rose_boost_version" = "105700" -o "$_version" = "1.57")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_58, test "$rose_boost_version" = "105800" -o "$_version" = "1.58")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_59, test "$rose_boost_version" = "105900" -o "$_version" = "1.59")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_60, test "$rose_boost_version" = "106000" -o "$_version" = "1.60")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_61, test "$rose_boost_version" = "106100" -o "$_version" = "1.61")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_62, test "$rose_boost_version" = "106200" -o "$_version" = "1.62")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_63, test "$rose_boost_version" = "106300" -o "$_version" = "1.63")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_64, test "$rose_boost_version" = "106400" -o "$_version" = "1.64")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_65, test "$rose_boost_version" = "106500" -o "$_version" = "1.65")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_66, test "$rose_boost_version" = "106600" -o "$_version" = "1.66")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_67, test "$rose_boost_version" = "106700" -o "$_version" = "1.67")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_68, test "$rose_boost_version" = "106800" -o "$_version" = "1.68")
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_1_69, test "$rose_boost_version" = "106900" -o "$_version" = "1.69")

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
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_63, test $rose_boost_version -ge 106300)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_64, test $rose_boost_version -ge 106400)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_65, test $rose_boost_version -ge 106500)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_66, test $rose_boost_version -ge 106600)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_67, test $rose_boost_version -ge 106700)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_68, test $rose_boost_version -ge 106800)
AM_CONDITIONAL(ROSE_USING_BOOST_VERSION_GE_1_69, test $rose_boost_version -ge 106900)

# DQ (10/22/2015): Added acceptable case 1.54.
# DQ (8/2/2016): Added acceptable case 1.55 through 1.61.
# [Matzke, 2016-09-19] Acceptable cases now 1.49-1.53, 1.55-1.60 decided at Jun 17 meeting, but leaving in 1.61
# [Matzke, 2019-01-31] Acceptable versions no 1.57-1.69. We decided to support only latest 10 versions from now on.
boost_minimum_version=105700

if test "$rose_boost_version" -lt "$boost_minimum_version"; then
   if test "$ROSE_ENABLE_BOOST_VERSION_CHECK" = "yes"; then
       AC_MSG_FAILURE([boost version $rose_boost_version is unsupported (need $boost_minimum_version or later)])
   else      
       AC_MSG_WARN([boost version $rose_boost_version is unsupported (need $boost_minimum_version or later)])
   fi
else
   AC_MSG_NOTICE([reasonable version of Boost found: $rose_boost_version])
fi

# DQ (12/22/2008): Fix boost configure to handle OS with older version of Boost that will
# not work with ROSE, and use the newer version specified by the user on the configure line.
AC_MSG_NOTICE([in ROSE/configure: ac_boost_path = "$ac_boost_path"])
#AC_DEFINE([ROSE_BOOST_PATH],"$ac_boost_path",[Location of Boost specified on configure line.])
AC_DEFINE_UNQUOTED([ROSE_BOOST_PATH],"$ac_boost_path",[Location (unquoted) of Boost specified on configure line.])
#AC_DEFINE([ROSE_WAVE_PATH],"$ac_boost_path/wave",[Location of Wave specified on configure line.])
AC_DEFINE_UNQUOTED([ROSE_WAVE_PATH],"$ac_boost_path/wave",[Location (unquoted) of Wave specified on configure line.])

AC_DEFINE_UNQUOTED([ROSE_BOOST_VERSION], $rose_boost_version, [Version of Boost specified on configure line.])

AX_BOOST_SYSTEM
AX_BOOST_THREAD
AX_BOOST_DATE_TIME
AX_BOOST_CHRONO
AX_BOOST_RANDOM dnl needed by Wt and other libraries that use boost
AX_BOOST_REGEX
AX_BOOST_PROGRAM_OPTIONS
#AX_BOOST_SERIALIZATION
#AX_BOOST_ASIO
#AX_BOOST_SIGNALS
#AX_BOOST_TEST_EXEC_MONITOR
AX_BOOST_FILESYSTEM
AX_BOOST_WAVE
#AX_BOOST_PYTHON

# AM_CONDITIONAL(ROSE_USE_BOOST_WAVE,test "$with_wave" = true)

# End macro ROSE_SUPPORT_BOOST.
])

