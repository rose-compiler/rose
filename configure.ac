dnl DQ (1/11/2010): Consider handing configure options as specified at:
dnl    https://projects.coin-or.org/BuildTools/wiki/user-configure

dnl The version number in the AC_INIT expression has been placed there automatically by the "build" script. We cannot
dnl do the replacement here in the configure file because AC_INIT's second argument must be a literal string, not a
dnl shell variable.
AC_INIT([ROSE], [0.9.6.14])
AC_PREREQ([2.59])

dnl +--------------------------------------------------------------------------+
dnl | Utilities
dnl +--------------------------------------------------------------------------+
rose_shtool="$srcdir/scripts/shtool"
T_BM="$("$rose_shtool" echo -n -e %B)" dnl "terminal bold mode"
T_NM="$("$rose_shtool" echo -n -e %b)" dnl "terminal normal mode"

dnl +--------------------------------------------------------------------------+
dnl | Source tree build - disabled
dnl +--------------------------------------------------------------------------+
AC_CONFIG_SRCDIR([README])
if test -e "README"; then
  AC_MSG_FAILURE(
    [
      ROSE cannot be built in the source tree '$(dirname "$0")' yet.
      Please run $0 in a different directory.
    ]
  )
fi

dnl +--------------------------------------------------------------------------+
dnl | ROSE versioning
dnl +--------------------------------------------------------------------------+
ROSE_SUPPORT_GIT_VERSIONING
ROSE_CONFIGURE_SECTION([Configuring ${T_BM}ROSE${T_NM}, version ${T_BM}0.9.6a ($GIT_HEAD_HASH)${T_NM}])

dnl +--------------------------------------------------------------------------+
dnl | Autotools setup
dnl +--------------------------------------------------------------------------+
AC_CONFIG_AUX_DIR([config])

# DQ (9/26/2015): Added option to eliminate large number of automake warnings (subdir-objects)
# RPM (9/29/2015): "subdir-objects" is causing $ROSE_SRC to be modified by "make"
# DQ (10/2/2015): Note that not using "foreign" option causes a LOT of warnings.
# AM_INIT_AUTOMAKE([foreign tar-pax 1.9.5])
# AM_INIT_AUTOMAKE([foreign tar-pax 1.9.5 subdir-objects])
# AM_INIT_AUTOMAKE([tar-pax 1.9.5])
# AM_INIT_AUTOMAKE([foreign tar-pax 1.9.5])
AM_INIT_AUTOMAKE([foreign tar-pax 1.9.5])

AM_CONFIG_HEADER([rose_config.h])

# TOO1 (9/23/2013): Use the builtin verbose mechanism of Automake:
#
#                   ```bash
#                   $ make V=0
#                   $ make V=1
#                   ```
#
# We put this in an ifdef for backward compatibility since this
# macro (AM_SILENT_RULES) is only supported for Automake v1.11+.
#
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AM_PROG_CC_C_O

# JJW (10/8/2008): Make default CFLAGS, CXXFLAGS, and FFLAGS empty (from
# <URL:http://osdir.com/ml/sysutils.autoconf.general/2003-11/msg00124.html>)
true ${CFLAGS:=}
true ${CXXFLAGS:=}
true ${FFLAGS:=}

dnl Liao 8/17/2010. A work around a undefined SED on NERSC hopper.
dnl I had to promote it here instead of put it into ROSE_SUPPORT_ROSE_PART_2
dnl since the expansion order there is wrong.
test -z "$SED" && SED=sed

dnl +--------------------------------------------------------------------------+
dnl | ROSE Support
dnl +--------------------------------------------------------------------------+

dnl This macro calls all of the other macros that used to be in the body of
dnl this configure.in file.  This permits other projects to call this same
dnl macro and setup the variables and macros that an external project may
dnl also require.
ROSE_SUPPORT_ROSE_PART_1
ROSE_SUPPORT_ROSE_BUILD_INCLUDE_FILES
CONFIGURE_POET
ROSE_SUPPORT_ROSE_PART_2
ROSE_SUPPORT_ROSE_PART_3
ROSE_SUPPORT_ROSE_PART_4
ROSE_SUPPORT_ROSE_PART_5
ROSE_OPTIONAL_PROJECTS
ROSE_SUPPORT_ROSE_PART_6
ROSE_SUPPORT_ROSE_PART_7

dnl Now generate all the output files (specified above)
ROSE_CONFIGURE_SECTION([Generating files])
AC_OUTPUT

dnl +--------------------------------------------------------------------------+

cat <<X
${T_BM}
+--------------------------------+
| Configure Terminated Normally! |
+--------------------------------+
${T_NM}
Now type 'make' and then 'make install'.

Thank you for using ROSE.

X

