dnl -*- shell-script -*-
dnl 
dnl OFP_SETUP_JAVAH
dnl
dnl defines:
dnl    OFP_ENABLE_C_ACTIONS
dnl    OFP_CFLAGS
dnl    OFP_DY_CFLAGS=

# Much of this is taken by example from CCAIN 
# (http://sourceforge.net/projects/ccain).

AC_DEFUN([OFP_SETUP_JAVAH],[

ofp_show_subtitle "JNI" 

OFP_DY_CFLAGS=

OFP_CURR_JDK_DIR="$OFP_JAVA_PATH"

if test -n "$1" ; then
    # the user supplied the include flags to configure with --with-java-incs
    OFP_CFLAGS="$1"
    OFP_ENABLE_C_ACTIONS="yes"
else
    case "${host}" in
	i?86-*-linux* | x86_64*linux*)
	    if test -d "$OFP_JAVA_PATH/include" ; then
		AC_CHECK_FILE([$OFP_JAVA_PATH/include/jni.h], have_jni_h=yes, 
		    have_jni_h=no)
		OFP_CFLAGS="$OFP_CFLAGS -I$OFP_JAVA_PATH/include"
		AC_MSG_CHECKING([for $OFP_JAVA_PATH/include/linux])
		if test -d "$OFP_JAVA_PATH/include/linux" ; then
		    OFP_CFLAGS="$OFP_CFLAGS -I$OFP_JAVA_PATH/include/linux"
		    AC_MSG_RESULT([yes])
		else
		    AC_MSG_RESULT([no])
		fi
		OFP_ENABLE_C_ACTIONS=$have_jni_h
	    else
		OFP_ENABLE_C_ACTIONS="no"
		AC_MSG_RESULT([no])
	    fi
            OFP_DY_CFLAGS=-shared
	    ;;
	powerpc-*-darwin* | i?86-*-darwin*)
	# Look in the *typical* place that darwin puts the java jdk.
	    AC_MSG_CHECKING([for jni.h])
	    have_jni_h=no
	    if test -d "/System/Library/Frameworks/JavaVM.framework" ; then
		JVM_FRWK_DIR="/System/Library/Frameworks/JavaVM.framework"
		if test -d "$JVM_FRWK_DIR/Versions/CurrentJDK" ; then
		    OFP_CURR_JDK_DIR="$JVM_FRWK_DIR/Versions/CurrentJDK"
		    if test -f "$OFP_CURR_JDK_DIR/Headers/jni.h" ; then
			OFP_CFLAGS="$OFP_CFLAGS -I$OFP_CURR_JDK_DIR/Headers"
			AC_MSG_RESULT([$OFP_CURR_JDK_DIR/Headers/jni.h])
			have_jni_h=yes
		    else
			AC_MSG_RESULT([no])
		    fi
		fi
	    fi
	    
            OFP_DY_CFLAGS=-dynamiclib
	    OFP_ENABLE_C_ACTIONS=$have_jni_h
	    ;;	    
    esac
fi

# The OFP_CURR_JDK_DIR is used to store a reference to where we found the 
# CurrentJDK directory on a Darwin box since this will be useful in 
# ofp_setup_libjvm
AC_SUBST(OFP_CURR_JDK_DIR)
AC_SUBST(OFP_CFLAGS)
AC_SUBST(OFP_DY_CFLAGS)
AC_SUBST(OFP_ENABLE_C_ACTIONS)    
AC_DEFINE_UNQUOTED(OFP_ENABLE_C_ACTIONS, "$OFP_ENABLE_C_ACTIONS", 
    [OFP Java jni.h])
])

