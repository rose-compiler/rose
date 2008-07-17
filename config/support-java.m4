AC_DEFUN([ROSE_SUPPORT_JAVA],
[
# Begin macro ROSE_SUPPORT_JAVA.

AC_MSG_CHECKING(for Java)

JAVA_PATH=$JAVA_HOME
# This is a hack, but it seems to work to find the JVM library
JAVA_JVM_FULL_PATH="`env _JAVA_LAUNCHER_DEBUG=x ${JAVA_PATH}/bin/java | grep '^JVM path is' | cut -c 13-`"

AC_DEFINE([USE_ROSE_JAVA_SUPPORT],1,[Controls use of ROSE support for Java (required for the Open Fortran Parser from LANL).])

JAVA_JVM_PATH=`dirname "${JAVA_JVM_FULL_PATH}"`

AC_SUBST(JAVA_PATH)
AC_SUBST(JAVA_JVM_PATH)

# End macro ROSE_SUPPORT_FORTRAN.
]
)
