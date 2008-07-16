# Copyright (c) 2005, 2006 Los Alamos National Security, LLC.  This
# material was produced under U.S. Government contract DE-
# AC52-06NA25396 for Los Alamos National Laboratory (LANL), which is
# operated by the Los Alamos National Security, LLC (LANS) for the
# U.S. Department of Energy. The U.S. Government has rights to use,
# reproduce, and distribute this software. NEITHER THE GOVERNMENT NOR
# LANS MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
# LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified to
# produce derivative works, such modified software should be clearly
# marked, so as not to confuse it with the version available from
# LANL.

# Additionally, this program and the accompanying materials are made
# available under the terms of the Eclipse Public License v1.0 which
# accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html

# Much of this is taken from CCAIN (http://sourceforge.net/projects/ccain).

############################################################################
# Initialization, version number, and other random setup/init stuff
############################################################################

# Init autoconf

AC_INIT([OpenFortranParser],[0.6])
AC_PREREQ(2.58)
AC_CONFIG_AUX_DIR(./config)

OFP_ROOT_DIR="`pwd`"

# Set the version of OFP that we are installing
OFP_MAJOR_VERSION="0"
OFP_MINOR_VERSION="6"

# Initialize variables
OFP_CFLAGS="$CFLAGS"
OFP_LDFLAGS="$LDFLAGS"
OFP_ENABLE_C_ACTIONS="no"
OFP_ENABLE_C_MAIN="no"
OFP_ENABLE_JNI="no"
OFP_JAVA=$JAVA
OFP_JAVAC=$JAVAC
OFP_JAR=$JAR
OFP_CC=$CC
INSTALL=""
RANLIB=""

OFP_VERSION=$OFP_MAJOR_VERSION.$OFP_MINOR_VERSION

AC_SUBST(OFP_MAJOR_VERSION)
AC_SUBST(OFP_MINOR_VERSION)
AC_SUBST(OFP_VERSION)

AC_DEFINE_UNQUOTED(OFP_MAJOR_VERSION, $OFP_MAJOR_VERSION,
    [Major release number of OpenFortranParser])
AC_DEFINE_UNQUOTED(OFP_MINOR_VERSION, $OFP_MINOR_VERSION,
    [Minor release number of OpenFortranParser])


OFP_CONFIGURE_SETUP
ofp_show_title "Configuring OFP version $OFP_VERSION"
ofp_show_subtitle "Initialization, setup"


############################################################################
# Configuration options
############################################################################

OFP_CONFIGURE_OPTIONS

#
# GNU C and autotools are inconsistent about whether this is defined
# so let's make it true everywhere for now...
#
AC_GNU_SOURCE

AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PROG_CC

OFP_CC=$CC

# Determine the host, target, build of the machine we're on.
AC_CANONICAL_SYSTEM
AC_MSG_CHECKING([host])
AC_MSG_RESULT([${host}])
AC_MSG_CHECKING([host_cpu])
AC_MSG_RESULT([${host_cpu}])
AC_MSG_CHECKING([host_os])
AC_MSG_RESULT([${host_os}])
AC_MSG_CHECKING([host_vendor])
AC_MSG_RESULT([${host_vendor}])

# defines OFP_JAVAC
OFP_SETUP_JAVAC($with_javac)
# defines OFP_JAVA
OFP_SETUP_JAVA($with_java)
# defines OFP_JAR
OFP_SETUP_JAR($with_jar)

# Determine the root directory of the java install.
# Figure out the name of the java command, without the full path
AC_MSG_CHECKING([for root directory of Java install])
OFP_JAVA_BASE_NAME=`basename $OFP_JAVA`
# Strip the bin/java off the full java command so we should be left 
# with the root directory for the java install.
OFP_JAVA_PATH=`echo $OFP_JAVA | sed "s/\/bin\/$OFP_JAVA_BASE_NAME//"`
AC_MSG_RESULT([$OFP_JAVA_PATH])
AC_SUBST(OFP_JAVA_PATH)

# defines OFP_ENABLE_C_ACTIONS, OFP_CFLAGS
if test "$enable_c_actions" = "yes" ; then 
   if test -n "$with_java_incs" ; then
      OFP_SETUP_JAVAH([$with_java_incs])
   else 
      OFP_SETUP_JAVAH([])
   fi
   AC_MSG_RESULT([OFP_CFLAGS is $OFP_CFLAGS])
# added OFP_DY_CFLAGS so dynamic libraries will build properly on Macs.
   AC_MSG_RESULT([OFP_DY_CFLAGS is $OFP_DY_CFLAGS])
   AC_MSG_RESULT([OFP_ENABLE_C_ACTIONS is $OFP_ENABLE_C_ACTIONS])
else
   ofp_show_subtitle "C actions disabled"
fi

# Defines OFP_ENABLE_C_MAIN, OFP_LDFLAGS
if test "$enable_c_main" = "yes" ; then
   if test -n "$with_java_lib_dir" ; then
      OFP_SETUP_LIBJVM([$with_java_lib_dir])
   else
      OFP_SETUP_LIBJVM([])
   fi
   AC_MSG_RESULT([OFP_LDFLAGS is $OFP_LDFLAGS])
   AC_MSG_RESULT([OFP_ENABLE_C_MAIN is $OFP_ENABLE_C_MAIN])
else
   ofp_show_subtitle "C main() disabled"
fi

if test "$OFP_ENABLE_C_ACTIONS" = "yes" -o "$OFP_ENABLE_C_MAIN" = "yes" ; then 
   OFP_ENABLE_JNI="yes"
fi

# Final steps
AC_SUBST(OFP_ENABLE_JNI)
AC_SUBST(OFP_CFLAGS)
AC_SUBST(OFP_DY_CFLAGS)
AC_SUBST(OFP_LDFLAGS)
AC_SUBST(OFP_ROOT_DIR)
AC_SUBST(INSTALL)
AC_SUBST(RANLIB)
AC_SUBST(OFP_CC)

ofp_show_subtitle "Final output" 

AC_CONFIG_FILES([
    make.inc
])
AC_OUTPUT()

