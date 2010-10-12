dnl -*- shell-script -*-
dnl 
dnl OFP_SETUP_LIBJVM
dnl
dnl defines:
dnl    OFP_JVM_LIB_DIR
dnl    OFP_LDFLAGS
dnl    OFP_DY_CFLAGS

# Much of this is taken by example from CCAIN 
# (http://sourceforge.net/projects/ccain).

AC_DEFUN([OFP_SETUP_LIBJVM],[

ofp_show_subtitle "libjvm" 

OFP_JVM_LIB_DIR=
OFP_JVM_LIB=
OFP_JVM_FWRK=

have_libjvm_so=no

case "${host}" in
     i?86-*-linux* | x86_64*linux*)
	if test -n "$1" ; then
	    OFP_JVM_LIB_DIR="$1"
	else
	    OFP_JVM_LIB_DIR="$OFP_JAVA_PATH/jre/lib/i386/client"
	fi
	OFP_JVM_LIB="libjvm.so"
        OFP_DY_CFLAGS="-shared"
	;;
    powerpc-*-darwin* | i?86-*-darwin*)
	if test -n "$1" ; then
	    OFP_JVM_LIB_DIR="$1"
	else
	    OFP_JVM_LIB_DIR="$OFP_CURR_JDK_DIR/Libraries"
	fi
	OFP_JVM_LIB="libjvm.dylib"
        OFP_JVM_FWRK="-framework JavaVM"
        OFP_DY_CFLAGS="-dynamiclib"
	;;	    
esac

if test -d "$OFP_JVM_LIB_DIR" ; then
    AC_CHECK_FILE([$OFP_JVM_LIB_DIR/$OFP_JVM_LIB], have_libjvm_so=yes, 
	have_libjvm_so=no)
    OFP_LDFLAGS="$OFP_LDFLAGS -L$OFP_JVM_LIB_DIR -ljvm $OFP_JVM_FWRK"
fi

# use the same variable as was used in searching for jni.h 
# because if either jni.h or libjvm.so can't be found, we fail.
OFP_ENABLE_C_MAIN=$have_libjvm_so

AC_SUBST(OFP_JVM_LIB_DIR)
AC_SUBST(OFP_JVM_LIB)
AC_SUBST(OFP_ENABLE_C_MAIN)
AC_SUBST(OFP_LDFLAGS)

])

