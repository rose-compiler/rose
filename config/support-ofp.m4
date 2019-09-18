AC_DEFUN([ROSE_SUPPORT_OFP],
[
# Begin macro ROSE_SUPPORT_OFP.

# Default OFP version and jar file, these should be changed each time the OFP jar file is updated.
# Used in ../src/3rdPartyLibraries/fortran-parser/Makefile.am
# and     ../src/frontend/SageIII/sageSupport.C
#
default_ofp_version=20190822

ofp_major_version_number=0
ofp_minor_version_number=8
ofp_patch_version_number=6

# DQ (10/18/2010): If fortran is enabled then OFP must be allowed and this was already check previously.
# This step is taken to simplify how we handle OFP and Fortran.  Of course OFP is enabled if Fortran is
# a valid langauge option, else we could not process the Fortran code with out OFP.  So we want to move
# to having only a single option for Fortran as a language. All reference to conditionals based on OFP can
# be replaced with reference to Fortran being processed.  The following code is dead (but not deleted yet).
if false; then
# DQ (10/18/2010): Only test if we should make OFP active if fortran support is enabled.
AC_MSG_NOTICE([before testing for gfortran to enable OFP: support_fortran_language = "$support_fortran_language"])
ofp_enabled=no

if test "x$support_fortran_language" = "xyes"; then
  AC_MSG_CHECKING([for gfortran to test whether Fortran support can be used])
  if test "x$USE_JAVA" = x1; then
    CPPFLAGS="$CPPFLAGS $JAVA_JVM_INCLUDE"
    if test "x$GFORTRAN_PATH" != "x"; then
    # AC_DEFINE([USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT], [1], [Always enable Fortran support whenever Java and gfortran are present])
    # AC_DEFINE([USE_ROSE_INTERNAL_JAVA_SUPPORT], [1], [Always enable Fortran support whenever Java and gfortran are present])
      ofp_enabled=yes
      AC_MSG_RESULT([yes])
      AC_DEFINE([USE_GFORTRAN_IN_ROSE], [1], [Mark that GFORTRAN is available])

    # Test that we have correctly evaluated the major and minor versions numbers...
      if test x$BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER == x; then
        AC_MSG_FAILURE([could not compute the major version number of "$BACKEND_FORTRAN_COMPILER"])
      fi
      if test x$BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER == x; then
        AC_MSG_FAILURE([could not compute the minor version number of "$BACKEND_FORTRAN_COMPILER"])
      fi
    else
      AC_MSG_RESULT([no ... gfortran cannot be found (try --with-gfortran=<path>)])
    fi
  else
    AC_MSG_RESULT([no ... Java cannot be found (try --with-java=<path>)])
  fi
else
  AC_MSG_NOTICE([Fortran is not enabled so OFP is disabled])
fi
fi
# AM_CONDITIONAL(ROSE_USE_OPEN_FORTRAN_PARSER, [test "x$ofp_enabled" = "xyes"])


OPEN_FORTRAN_PARSER_PATH="${ac_top_builddir}/src/3rdPartyLibraries/fortran-parser" # For the one rule that uses it
AC_SUBST(OPEN_FORTRAN_PARSER_PATH)


# DQ (2/2/2010): New code to control use of different versions of OFP within ROSE.
AC_ARG_ENABLE(ofp-version,
[  --enable-ofp-version    version number for OFP Java-based parser (e.g. 20190822).],
[ AC_MSG_NOTICE([setting up OFP version])
])

# CER (8/24/2011): Changed the version numbering system of the OFP jar file
# distributed with ROSE to use the date rather than the OFP release version.
#
if test "x$enable_ofp_version" = "x"; then
   AC_MSG_NOTICE([using default version of OFP: "$default_ofp_version"])
   ofp_version_number=$default_ofp_version
else
   ofp_version_number=$enable_ofp_version
fi

AC_MSG_NOTICE([ofp_version_number = "$ofp_version_number"])

ofp_jar_file_contains_java_file=true

# DQ (9/28/2010): Newer versions of the OFP jar file contains fortran/ofp/parser/java/IFortranParserAction.java
# we need this to maintain backward compatability.
AM_CONDITIONAL(ROSE_OFP_CONTAINS_JAVA_FILE, [test "x$ofp_jar_file_contains_java_file" = true])

AC_DEFINE_UNQUOTED([ROSE_OFP_VERSION_NUMBER], $ofp_version_number , [OFP version number])
AC_DEFINE_UNQUOTED([ROSE_OFP_MAJOR_VERSION_NUMBER], $ofp_major_version_number , [OFP major version number])
AC_DEFINE_UNQUOTED([ROSE_OFP_MINOR_VERSION_NUMBER], $ofp_minor_version_number , [OFP minor version number])
AC_DEFINE_UNQUOTED([ROSE_OFP_PATCH_VERSION_NUMBER], $ofp_patch_version_number , [OFP patch version number])

ROSE_OFP_VERSION_NUMBER=$ofp_version_number
ROSE_OFP_MAJOR_VERSION_NUMBER=$ofp_major_version_number
ROSE_OFP_MINOR_VERSION_NUMBER=$ofp_minor_version_number
ROSE_OFP_PATCH_VERSION_NUMBER=$ofp_patch_version_number

AC_SUBST(ROSE_OFP_VERSION_NUMBER)
AC_SUBST(ROSE_OFP_MAJOR_VERSION_NUMBER)
AC_SUBST(ROSE_OFP_MINOR_VERSION_NUMBER)
AC_SUBST(ROSE_OFP_PATCH_VERSION_NUMBER)

ofp_jar_file="OpenFortranParser-${ofp_version_number}.jar"
AC_MSG_NOTICE([ofp_jar_file = "$ofp_jar_file"])

ROSE_OFP_JAR_FILE=$ofp_jar_file
AC_SUBST(ROSE_OFP_JAR_FILE)
AC_DEFINE_UNQUOTED([ROSE_OFP_JAR_FILE], $ofp_jar_file , [OFP jar file])

# DQ (4/5/2010): Moved the specification of CLASSPATH to after the specification 
# of OFP version number so that we can use it to set the class path.

CLASSPATH=${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/antlr-3.5.2-complete.jar:${ABSOLUTE_SRCDIR}${OPEN_FORTRAN_PARSER_PATH}/${ROSE_OFP_JAR_FILE}

export CLASSPATH
AC_SUBST(CLASSPATH)
AC_DEFINE_UNQUOTED([ROSE_OFP_CLASSPATH], $CLASSPATH , [OFP class path for Jave Virtual Machine])

AC_MSG_NOTICE([OFP CLASSPATH = "$CLASSPATH"])

# End macro ROSE_SUPPORT_OFP.
]
)
