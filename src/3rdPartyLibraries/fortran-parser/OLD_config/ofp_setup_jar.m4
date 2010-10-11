dnl -*- shell-script -*-
dnl 
dnl OFP_SETUP_JAR
dnl
dnl defines:
dnl OFP_JAR

# Much of this is taken by example from CCAIN 
# (http://sourceforge.net/projects/ccain).

AC_DEFUN([OFP_SETUP_JAR],[

ofp_show_subtitle "Java jar" 

if test -n "$1" ; then
    # The user specified a java archiver with --with-jar to configure.

    # This means check for program (basename of $1 (e.g., jar)) (second arg) 
    # in the path (arg 5) given by everything up to the basename of $1
    # and if found, set OFP_JAR to to $1 (third arg) and if not, set it to 
    # no (arg 4).  
    TMP_JAR_BASE_NAME="`basename $1`"
    TMP_PATH=`echo $1 | sed "s/\/$TMP_JAR_BASE_NAME//"`
    AC_CHECK_PROG(OFP_JAR, $TMP_JAR_BASE_NAME, $1, no, $TMP_PATH)
else
    AC_PATH_PROGS(OFP_JAR, jar, no)
fi

if test "$OFP_JAR" = "no" ; then
    # We must have the java archiver to build OFP, so print error message 
    # and exit configure.
    AC_MSG_ERROR([Must have Java archiver (e.g., jar)])
else
    AC_MSG_CHECKING([$OFP_JAR can archive Java class])
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
	# Successfully compiled so try archiving.
	$OFP_JAR cf ConfTest.jar ConfTest.class
	if test $? != 0 -o ! -f ConfTest.jar ; then
	    rm -f ConfTest.class ConfTest.java 
	    # Failed -- print error message and exit.
	    AC_MSG_ERROR([$OFP_JAR cannot archive Java class])
	fi
    fi

    # If we got here, we successfully compiled and archived the Java class.
    AC_MSG_RESULT([yes])
    rm -f ConfTest.class ConfTest.java ConfTest.jar
fi

AC_SUBST(OFP_JAR)    
AC_DEFINE_UNQUOTED(OFP_JAR, "$OFP_JAR", [OFP underlying Java jar])
])

