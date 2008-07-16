dnl -*- shell-script -*-
dnl 
dnl OFP_SETUP_JAVA
dnl
dnl defines:
dnl OFP_JAVA 

# Much of this is taken by example from CCAIN 
# (http://sourceforge.net/projects/ccain).

AC_DEFUN([OFP_SETUP_JAVA],[

ofp_show_subtitle "Java VM" 

if test -n "$1" ; then
    # The user specified a java vm with --with-java to configure so try it.

    # This means check for program (basename of $1 (e.g., java)) (second arg) 
    # in the path (arg 5) given by everything up to the basename of $1
    # and if found, set OFP_JAVA to to $1 (third arg) and if not, set it to 
    # no (arg 4).  
    TMP_JAVA_BASE_NAME="`basename $1`"
    TMP_PATH=`echo $1 | sed "s/\/$TMP_JAVA_BASE_NAME//"`
    AC_CHECK_PROG(OFP_JAVA, $TMP_JAVA_BASE_NAME, $1, no, $TMP_PATH)
else
    AC_PATH_PROG(OFP_JAVA, java, no)
fi

if test "$OFP_JAVA" = "no" ; then
    # We must have the java VM to run OFP, so print error message 
    # and exit configure.
    AC_MSG_ERROR([Must have Java VM (e.g., java)])
else
    AC_MSG_CHECKING([that $OFP_JAVA can run Java program])
    # Create a test file
    cat > ConfTest.java <<EOF
public class ConfTest {
   public static void main(String[[]] args) throws Exception {
   }
}
EOF

    # We know we can compile since that was already tested (or should have
    # been, if configure.ac sets up OFP_JAVAC first).
    $OFP_JAVAC -classpath . ConfTest.java
    # The '-s' means see if the file exists with size > 0.  Not that result
    # to see if the compiler failed (doesn't exist or size == 0).
    if test ! -s ConfTest.class ; then
	rm -f ConfTest.java
	# Print error message and tell configure to exit.
	AC_MSG_ERROR([$OFP_JAVAC cannot compile Java source])
    else
	# Successfully compiled so make sure we can run it.
	$OFP_JAVA -classpath . ConfTest
	if test $? != 0 ; then
	    rm -f ConfTest.class ConfTest.java
	    # Failed -- print error message and exit.
	    AC_MSG_ERROR([$OFP_JAVA cannot run Java program])
	fi
    fi

    # If we got here, we successfully compiled and ran the Java program.
    AC_MSG_RESULT([yes])
    rm -f ConfTest.class ConfTest.java
fi

AC_SUBST(OFP_JAVA)    
AC_DEFINE_UNQUOTED(OFP_JAVA, "$OFP_JAVA", [OFP underlying Java VM])
])

