dnl -*- shell-script -*-
dnl 
dnl OFP_SETUP_JAVAC
dnl
dnl defines:
dnl OFP_JAVAC 

# Much of this is taken by example from CCAIN 
# (http://sourceforge.net/projects/ccain).

AC_DEFUN([OFP_SETUP_JAVAC],[

ofp_show_subtitle "Java compiler" 

if test -n "$1" ; then
    # The user specified a java compiler with --with-javac to configure.

    # This means check for program (basename of $1 (e.g., javac)) (second arg) 
    # in the path (arg 5) given by everything up to the basename of $1
    # and if found, set OFP_JAVAC to to $1 (third arg) and if not, set it to 
    # no (arg 4).  
    TMP_JAVAC_BASE_NAME="`basename $1`"
    TMP_PATH=`echo $1 | sed "s/\/$TMP_JAVAC_BASE_NAME//"`
    AC_CHECK_PROG(OFP_JAVAC, $TMP_JAVAC_BASE_NAME, $1, no, $TMP_PATH)
else
    AC_PATH_PROGS(OFP_JAVAC, javac, no)
fi

if test "$OFP_JAVAC" = "no" ; then
    # We must have the java compiler to build OFP, so print error message 
    # and exit configure.
    AC_MSG_ERROR([Must have Java compiler (e.g., javac)])
else
    AC_MSG_CHECKING([that $OFP_JAVAC can compile Java source])

    # Make sure we can compile something with the javac command.
    cat > ConfTest.java <<EOF
public class ConfTest {
   public static void main(String[[]] args) throws Exception {
   }
}
EOF

    $OFP_JAVAC -classpath . ConfTest.java
    # The '-s' means see if the file exists with size > 0.  Not that result
    # to see if the compiler failed (doesn't exist or size == 0).
    if test ! -s ConfTest.class ; then
	rm -f ConfTest.java
	# Print error message and tell configure to exit.
	AC_MSG_ERROR([$OFP_JAVAC cannot compile Java source])
    else
	# Successfully compiled so clean up tmp files.
	rm -f ConfTest.java ConfTest.class
    fi

    # If we get here, we successfully compiled.
    AC_MSG_RESULT([yes])
fi

AC_SUBST(OFP_JAVAC)    
AC_DEFINE_UNQUOTED(OFP_JAVAC, "$OFP_JAVAC", [OFP underlying Java compiler])
])

