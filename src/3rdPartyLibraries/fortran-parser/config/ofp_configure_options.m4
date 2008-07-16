dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2004-2005 The Trustees of Indiana University.
dnl                         All rights reserved.
dnl Copyright (c) 2004-2005 The Trustees of the University of Tennessee.
dnl                         All rights reserved.
dnl Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
dnl                         University of Stuttgart.  All rights reserved.
dnl Copyright (c) 2004-2005 The Regents of the University of California.
dnl                         All rights reserved.
dnl $COPYRIGHT$
dnl 
dnl Additional copyrights may follow
dnl 
dnl $HEADER$
dnl

# Much of this is basically taken from CCAIN 
# (http://sourceforge.net/projects/ccain).

AC_DEFUN([OFP_CONFIGURE_OPTIONS],[
ofp_show_subtitle "Configuration options"

#
# Java JNI for C actions
#
AC_MSG_CHECKING([if want OFP C actions])
AC_ARG_ENABLE(c-actions, 
    AC_HELP_STRING([--enable-c-actions],
                   [enable OFP C actions (default: disabled)]))
# the hyphen in c-actions is converted to an underscore.
if test "$enable_c_actions" = "yes"; then
    AC_MSG_RESULT([yes])
    OFP_ENABLE_C_ACTIONS="yes"
else
    AC_MSG_RESULT([no])
    OFP_ENABLE_C_ACTIONS="no"
fi


#
# Java JNI for C main()
#
AC_MSG_CHECKING([if want OFP C main()])
AC_ARG_ENABLE(c-main, 
    AC_HELP_STRING([--enable-c-main],
                   [enable OFP C main (default: disabled)]))
# the hyphen in c-actions is converted to an underscore.
if test "$enable_c_main" = "yes"; then
    AC_MSG_RESULT([yes])
    OFP_ENABLE_C_MAIN="yes"
else
    AC_MSG_RESULT([no])
    OFP_ENABLE_C_MAIN="no"
fi


#
# Java VM
#
AC_MSG_CHECKING([if specified Java VM])
AC_ARG_WITH([java], 
    [AC_HELP_STRING([--with-java=JAVA],
	    [full name to Java VM (/usr/local/jdk1.6.0/bin/java)])])
if test -n "$with_java" ; then
    AC_MSG_RESULT([$with_java])
else
    AC_MSG_RESULT([no])
fi

#
# Java compiler
#
AC_MSG_CHECKING([if specified Java compiler])
AC_ARG_WITH([javac], 
    [AC_HELP_STRING([--with-javac=JAVAC],
	    [full name to Java compiler (/usr/local/jdk1.6.0/bin/javac)])])
if test -n "$with_javac" ; then
    AC_MSG_RESULT([$with_javac])
else
    AC_MSG_RESULT([no])
fi

#
# Java archiver (JAR)
#
AC_MSG_CHECKING([if specified Java archiver])
AC_ARG_WITH([jar], 
    [AC_HELP_STRING([--with-jar=JAR],
	    [full name to Java archiver (/usr/local/jdk1.6.0/bin/jar)])])
if test -n "$with_jar" ; then
    AC_MSG_RESULT([$with_jar])
else
    AC_MSG_RESULT([no])
fi

#
# Java include dir(s)
#
AC_MSG_CHECKING([if specified Java include dir(s)])
AC_ARG_WITH([java-incs], 
    [AC_HELP_STRING([--with-java-incs=FLAG(S)],
	    [C include flag(s) for JNI (-I/usr/local/jdk1.6.0/include)])])
if test -n "$with_java_incs" ; then
    AC_MSG_RESULT([$with_java_incs])
else
    AC_MSG_RESULT([no])
fi

#
# Java lib dir
#
AC_MSG_CHECKING([if specified Java lib dir])
AC_ARG_WITH([java-lib-dir], 
    [AC_HELP_STRING([--with-java-lib-dir=DIR],
	    [dir for libjvm (/usr/local/jdk1.6.0/jre/lib/i386/client)])])
if test -n "$with_java_lib_dir" ; then
    AC_MSG_RESULT([$with_java_lib_dir])
else
    AC_MSG_RESULT([no])
fi

])
