AC_DEFUN([ROSE_SUPPORT_JAVA],
[
# Begin macro ROSE_SUPPORT_JAVA.

AC_MSG_CHECKING(for Java)

AC_ARG_WITH([java],
            AS_HELP_STRING([--with-java],
                           [use Java for Fortran or Javaport support (default is YES if Java can be found)]),
            [javasetting=$withval],
            [javasetting=try])

JAVA=$javasetting
USE_JAVA=1
if test "x$javasetting" = xno; then
  JAVA=""
  USE_JAVA=0
elif test "x$javasetting" = xyes || test "x$javasetting" = xtry; then
  JAVA_PATH="${JAVA_HOME}"
  if test "x$JAVA_PATH" = x; then
    if which java; then
      JAVA="`which java`"
      AS_SET_CATFILE(JAVA_PATH, "`pwd`", "`dirname ${JAVA}`/../..")
    elif "x$javasetting" = "xyes"; then
      AC_MSG_ERROR([--with-java was given but "java" is not in PATH and JAVA_HOME was not set])
    else # $javasetting is "try", so it is not an error for Java to not be found
      JAVA=""
      USE_JAVA=0
    fi
  else
    JAVA="${JAVA_PATH}/bin/java"
  fi
elif test -d "${javasetting}"; then
  if test -x "${javasetting}/bin/java"; then
    JAVA_PATH="${javasetting}"
    JAVA="${javasetting}/bin/java"
  else
    AC_MSG_ERROR([Argument to --with-java should be either a java executable or a top-level JDK install directory (with bin/java present)])
  fi
elif test -x "${javasetting}"; then
  AS_SET_CATFILE(JAVA_PATH, "`pwd`", "`dirname ${javasetting}`/..")
  JAVA="${javasetting}"
else
  AC_MSG_ERROR([Argument to --with-java should be either a java executable or a top-level JDK install directory (with bin/java present)])
fi

if test "x$USE_JAVA" = x1; then
  AC_MSG_RESULT([$JAVA])
else
  AC_MSG_RESULT([not requested])
fi

if test "x$USE_JAVA" = x1; then
  AC_MSG_CHECKING(for Java JVM include and link options)
# This is a hack, but it seems to work to find the JVM library
  if test -x /usr/bin/javaconfig; then # We are on a Mac
    JAVA_JVM_LINK="-framework JavaVM"
    JAVA_JVM_INCLUDE="-I`/usr/bin/javaconfig Headers`"
  else
    JAVA_JVM_FULL_PATH="`env _JAVA_LAUNCHER_DEBUG=x ${JAVA} | grep '^JVM path is' | cut -c 13-`"
    if test "x$JAVA_JVM_FULL_PATH" = x; then
      AC_MSG_ERROR([Unable to find path to JVM library])
    fi
    JAVA_JVM_PATH=`dirname "${JAVA_JVM_FULL_PATH}"`
    JAVA_JVM_LINK="-L${JAVA_JVM_PATH} -ljvm"
    JAVA_JVM_INCLUDE="-I${JAVA_PATH}/include -I${JAVA_PATH}/include/linux"
  fi
  AC_MSG_RESULT([$JAVA_JVM_INCLUDE and $JAVA_JVM_LINK])

  JAVAC="${JAVA_PATH}/bin/javac"
  JAR="${JAVA_PATH}/bin/jar"

  AC_MSG_CHECKING(for javac)
  if test -x "${JAVAC}"; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_ERROR([javac not found in $JAVA_PATH])
  fi

  AC_MSG_CHECKING(for jar)
  if test -x "${JAR}"; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_ERROR([jar not found in $JAVA_PATH])
  fi
fi

AC_DEFINE([USE_ROSE_JAVA_SUPPORT],[$USE_JAVA],[Controls use of ROSE support for Java.])
AC_DEFINE_UNQUOTED([JAVA_JVM_PATH],["$JAVA"],[Path to JVM executable])

AC_SUBST(JAVA_PATH)
AC_SUBST(JAVA_JVM_LINK)
AC_SUBST(JAVA_JVM_INCLUDE)
AC_SUBST(JAVA)
AC_SUBST(JAVAC)
AC_SUBST(JAR)

# End macro ROSE_SUPPORT_JAVA.
]
)
