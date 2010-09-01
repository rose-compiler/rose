AC_DEFUN([ROSE_SUPPORT_JAVA],
[
# Begin macro ROSE_SUPPORT_JAVA.

AC_MSG_CHECKING([for Java (javac first, then java, then jvm)])

AC_ARG_WITH([java],
            AS_HELP_STRING([--with-java],
                           [use Java for Fortran or Javaport support (default is YES if Java can be found)]),
            [javasetting=$withval],
            [javasetting=try])

JAVAC=$javasetting
USE_JAVA=1
#echo "JAVA_HOME = ${JAVA_HOME}"
if test "x$javasetting" = xno; then
# echo 'test "x$javasetting" = xno;'
  JAVAC=""
  USE_JAVA=0
elif test "x$javasetting" = xyes || test "x$javasetting" = xtry; then
# echo 'test x$javasetting = xyes || test x$javasetting = xtry;'
  JAVA_PATH="${JAVA_HOME}"
# echo "JAVA_PATH = ${JAVA_PATH}"
  if test "x$JAVA_PATH" = x; then
#   echo 'test x$JAVA_PATH = x;'
# DQ & PC (11/3/2009): Search for javac instead of java since we require
# the JDK and this should detect that it is missign as early as possible.
    JAVAC="`which javac`"
  # if which javac > /dev/null; then
    if test $? -eq 0; then
    # echo 'which javac;'
    # echo "java = ${JAVA}"
    # JAVAC="`which javac`"
    # echo "javac = ${JAVAC}"
# DQ & PC (11/3/2009): Fixing support for Java that causes problems for CERT and our new RedHat Release 5 systems.
      ROSE_CANON_SYMLINK(JAVAC, "${JAVAC}")
    # echo "javac = ${JAVAC}"
      JAVAC_BASENAME=`basename ${JAVAC}`
      if test x${JAVAC_BASENAME} == "xjavac"; then
         : # echo "Found Sun or IBM Java (javac)"
      else
       # This is likely the Eclipse Java (ecj).
       # DQ (11/3/2009): If this is IBM Java then it should also work with ROSE and this macro.
         AC_MSG_ERROR([This is not SUN or IBM Java found by default (likely found ecj - Eclipse Java) -- specify correct java using --with-java=<path>])
      fi
    # AS_SET_CATFILE(JAVA_PATH, "`pwd`", "`dirname ${JAVA}`/../..")
      AS_SET_CATFILE(JAVA_PATH, "`pwd`", "`dirname ${JAVAC}`/..")
      
      # George Vulov (Aug. 25, 2010) On OS X the binaries found under /System/Library/Frameworks/JavaVM.framework/Versions/Current/Commands/
      # aren't the true Java binaries; instead they check the java preferences app and then instantiate whatever version of Java
      # is specified there. The actual java binaries are located in /System/Library/Frameworks/JavaVM.framework/Versions/CurrentJDK/Home
      if test "x$build_vendor" = xapple; then
      	if (( `echo ${JAVA_PATH} | grep -c "/Commands/.."` > 0 )); then
      		AS_SET_CATFILE(JAVA_PATH, "`pwd`", "`dirname ${JAVAC}`/../../CurrentJDK/Home")
      	fi
	  fi
      
    # echo "After setting value: JAVA_PATH = ${JAVA_PATH}"
    elif "x$javasetting" = "xyes"; then
      AC_MSG_ERROR([--with-java was given but "java" is not in PATH and JAVA_HOME was not set])
    else # $javasetting is "try", so it is not an error for Java to not be found
    # echo 'FALSE case: x$javasetting = xyes;'
      JAVAC=""
      USE_JAVA=0
    fi
  else
  # echo 'FALSE test x$JAVA_PATH = x;'
# DQ & PC (11/3/2009): Search for javac instead of java since we require
# the JDK and this should detect that it is missign as early as possible.
#   JAVA="${JAVA_PATH}/bin/java"
    JAVAC="${JAVA_PATH}/bin/javac"
  fi
elif test -d "${javasetting}"; then
# DQ & PC (11/3/2009): Search for javac instead of java since we require
# the JDK and this should detect that it is missign as early as possible.
  if test -x "${javasetting}/bin/javac"; then
    JAVA_PATH="${javasetting}"
  # echo "After setting value using javasetting: JAVA_PATH = ${JAVA_PATH}"
    JAVAC="${javasetting}/bin/javac"
  else
    AC_MSG_ERROR([Argument to --with-java should be either a javac executable or a top-level JDK install directory (with bin/javac present)])
  fi
elif test -x "${javasetting}"; then
  AS_SET_CATFILE(JAVA_PATH, "`pwd`", "`dirname ${javasetting}`/..")
  JAVAC="${javasetting}"
else
  AC_MSG_ERROR([Argument to --with-java should be either a javac executable or a top-level JDK install directory (with bin/javac present)])
fi

# echo "USE_JAVA = $USE_JAVA"
if test "x$USE_JAVA" = x1; then
# AC_MSG_RESULT([$JAVAC])
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT([not requested])
fi

# echo "Before checking for Java JVM: JAVA_PATH = ${JAVA_PATH}"

if test "x$USE_JAVA" = x1; then

  JAVA_BIN="${JAVA_PATH}/bin"
  JAVA="${JAVA_BIN}/java"
# echo "JAVA = ${JAVA}"
  AC_MSG_CHECKING(for java)
  if test -x "${JAVA}"; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_ERROR([java not found in $JAVA_PATH])
  fi

  AC_MSG_CHECKING(for Java JVM include and link options)
# This is a hack, but it seems to work to find the JVM library
  if test -x /usr/bin/javaconfig; then # We are on a Mac
    JAVA_JVM_LINK="-framework JavaVM"
  else
    JAVA_JVM_FULL_PATH="`env _JAVA_LAUNCHER_DEBUG=x ${JAVA} 2>/dev/null | grep '^JVM path is' | cut -c 13-`" ; # Sun JVM
    JAVA_JVM_PATH=`dirname "${JAVA_JVM_FULL_PATH}"`
    if test "x$JAVA_JVM_FULL_PATH" = x; then
      JAVA_JVM_PATH="`env _JAVA_LAUNCHER_DEBUG=x ${JAVA} 2>&1 | grep '^JavaJVMDir  = ' | cut -c 15-`" # IBM J9 JVM
      if test "x$JAVA_JVM_PATH" = x; then
        AC_MSG_ERROR([Unable to find path to JVM library])
      fi
    fi
    JAVA_JVM_LINK="-L${JAVA_JVM_PATH} -ljvm"
  fi
  
  JAVA_JVM_INCLUDE="-I${JAVA_PATH}/include -I${JAVA_PATH}/include/linux"
  
  AC_MSG_RESULT([$JAVA_JVM_INCLUDE and $JAVA_JVM_LINK])

# JAR="${JAVA_PATH}/bin/jar"
  JAR="${JAVA_BIN}/jar"

  AC_MSG_CHECKING(for jar)
  if test -x "${JAR}"; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_ERROR([jar not found in $JAVA_PATH])
  fi
fi

if test $USE_JAVA; then
  AC_DEFINE([USE_ROSE_JAVA_SUPPORT],[],[Controls use of ROSE support for Java.])
fi
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
