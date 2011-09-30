AC_DEFUN([ROSE_SUPPORT_OFP],
[
# Begin macro ROSE_SUPPORT_OFP.

# Default OFP version, this should be changed each time the OFP jar file is updated.
#
#default_ofp_version=20110825
#
#ofp_major_version_number=0
#ofp_minor_version_number=8
#ofp_patch_version_number=3

# DQ (10/18/2010): If fortran is enabled then OFP must be allowed and this was already check previously.
# This step is taken to simplify how we handle OFP and Fortran.  Of course OFP is enabled if Fortran is
# a valid langauge option, else we could not process the Fortran code with out OFP.  So we want to move
# to having only a single option for Fortran as a language. All reference to conditionals based on OFP can
# be replaced with reference to Fortran being processed.  The following code is dead (but not deleted yet).
if false; then
# DQ (10/18/2010): Only test if we should make OFP active if fortran support is enabled.
echo "Before testing for gfortran to enable OFP: support_fortran_language = $support_fortran_language"
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
        echo "Error: Could not compute the MAJOR version number of $BACKEND_FORTRAN_COMPILER"
        exit 1
      fi
      if test x$BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER == x; then
        echo "Error: Could not compute the MINOR version number of $BACKEND_FORTRAN_COMPILER"
        exit 1
      fi
    else
      AC_MSG_RESULT([no ... gfortran cannot be found (try --with-gfortran=<path>)])
    fi
  else
    AC_MSG_RESULT([no ... Java cannot be found (try --with-java=<path>)])
  fi
else
  echo "Fortran is not enabled so OFG is disabled."
fi
fi
# AM_CONDITIONAL(ROSE_USE_OPEN_FORTRAN_PARSER, [test "x$ofp_enabled" = "xyes"])


OPEN_FORTRAN_PARSER_PATH="${ac_top_builddir}/src/3rdPartyLibraries/fortran-parser" # For the one rule that uses it
AC_SUBST(OPEN_FORTRAN_PARSER_PATH)


# DQ (2/2/2010): New code to control use of different versions of OFP within ROSE.
AC_ARG_ENABLE(ofp-version,
[  --enable-ofp-version    major.minor.patch version number for OFP (e.g. 0.7.2, 0.8.0, ...).],
[ echo "Setting up OFP version"
])

# DQ (7/31/2010): Changed the default version of OFP to 0.8.1 (now distributed with ROSE).
echo "enable_ofp_version = $enable_ofp_version"
if test "x$enable_ofp_version" = "x"; then
   echo "Default version of OFP used (0.8.3)"
   ofp_major_version_number=0
   ofp_minor_version_number=8
 # DQ (9/26/2010): Changed default version to 0.8.2
 # CER (6/2/2011): Changed default version to 0.8.3
   ofp_patch_version_number=3
else
   ofp_major_version_number=`echo $enable_ofp_version | cut -d\. -f1`
   ofp_minor_version_number=`echo $enable_ofp_version | cut -d\. -f2`
   ofp_patch_version_number=`echo $enable_ofp_version | cut -d\. -f3`
fi

echo "ofp_major_version_number = $ofp_major_version_number"
echo "ofp_minor_version_number = $ofp_minor_version_number"
echo "ofp_patch_version_number = $ofp_patch_version_number"

ofp_jar_file_contains_java_file=false
if test "x$ofp_major_version_number" = "x0"; then
   echo "Recognized an accepted major version number."
   if test "x$ofp_minor_version_number" = "x8"; then
      echo "Recognized an accepted minor version number (any 0.8.x version is allowed)."
#     echo "Recognized an accepted minor version number."
      if test "x$ofp_patch_version_number" = "x0"; then
         echo "Recognized an accepted patch version number (very old version of OFP)."
      else
         if test "x$ofp_patch_version_number" = "x1"; then
            echo "Recognized an older but accepted patch version number ONLY for testing."
         else
            ofp_jar_file_contains_java_file=true
            if test "x$ofp_patch_version_number" = "x2"; then
               echo "Recognized an accepted patch version number."
            elif test "x$ofp_patch_version_number" = "x3"; then
               echo "Recognized an accepted patch version number."
            else
#              echo "ERROR: Could not identify the OFP patch version number."
               echo "Recognized an accepted patch version number (later than default)."
               exit 1
            fi
         fi
#       # exit 1
      fi
   else
      if test "x$ofp_minor_version_number" = "x7"; then
       # We accept any patch level with minor version number 7 releases. 
         echo "Recognized an accepted minor version number using ofp_patch_version_number = $ofp_patch_version_number."
      else
         echo "ERROR: Could not identify the OFP minor version number."
         exit 1
      fi
   fi
else
   if test "x$ofp_major_version_number" = "x1"; then
      echo "Recognized an accepted major version number (but this is not supported yet)."
      exit 1
   else
      echo "ERROR: Could not identify the OFP major version number."
      exit 1
   fi
fi

# DQ (9/28/2010): Newer versions of the OFP jar file contains fortran/ofp/parser/java/IFortranParserAction.java
# we need this to maintain backward compatability.
AM_CONDITIONAL(ROSE_OFP_CONTAINS_JAVA_FILE, [test "x$ofp_jar_file_contains_java_file" = true])

AC_DEFINE_UNQUOTED([ROSE_OFP_MAJOR_VERSION_NUMBER], $ofp_major_version_number , [OFP major version number])
AC_DEFINE_UNQUOTED([ROSE_OFP_MINOR_VERSION_NUMBER], $ofp_minor_version_number , [OFP minor version number])
AC_DEFINE_UNQUOTED([ROSE_OFP_PATCH_VERSION_NUMBER], $ofp_patch_version_number , [OFP patch version number])

ROSE_OFP_MAJOR_VERSION_NUMBER=$ofp_major_version_number
ROSE_OFP_MINOR_VERSION_NUMBER=$ofp_minor_version_number
ROSE_OFP_PATCH_VERSION_NUMBER=$ofp_patch_version_number

AC_SUBST(ROSE_OFP_MAJOR_VERSION_NUMBER)
AC_SUBST(ROSE_OFP_MINOR_VERSION_NUMBER)
AC_SUBST(ROSE_OFP_PATCH_VERSION_NUMBER)

# echo "Testing OFP version number specification..."
# exit 1

# DQ (4/5/2010): Moved the specification of CLASSPATH to after the specification 
# of OFP version number so that we can use it to set the class path.
# DQ (3/11/2010): Updating to new Fortran OFP version 0.7.2 with Craig.
# CLASSPATH=${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/antlr-2.7.7.jar:${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/antlr-3.0.1.jar:${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/antlr-runtime-3.0.1.jar:${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/stringtemplate-3.1b1.jar:.
# CLASSPATH=${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/antlr-3.2.jar:${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/fortran-parser/lib/OpenFortranParser-0.7.2.jar:.
# CLASSPATH=${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/antlr-3.2.jar:${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/fortran-parser/OpenFortranParser-0.7.2.jar:.
# CLASSPATH=${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/antlr-3.2.jar:${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/fortran-parser/OpenFortranParser-${ROSE_OFP_MAJOR_VERSION_NUMBER}.${ROSE_OFP_MINOR_VERSION_NUMBER}.${ROSE_OFP_PATCH_VERSION_NUMBER}.jar:.
CLASSPATH=${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/antlr-3.2.jar:${ABSOLUTE_SRCDIR}${OPEN_FORTRAN_PARSER_PATH}/OpenFortranParser-${ROSE_OFP_MAJOR_VERSION_NUMBER}.${ROSE_OFP_MINOR_VERSION_NUMBER}.${ROSE_OFP_PATCH_VERSION_NUMBER}.jar:.

#
# OFP version 0.8.3 and antlr 3.3 are the defaults
#
if test "x$ofp_minor_version_number" = "x8"; then
   if test "x$ofp_patch_version_number" = "x3"; then
      CLASSPATH=${ABSOLUTE_SRCDIR}/src/3rdPartyLibraries/antlr-jars/antlr-3.3-complete.jar:${ABSOLUTE_SRCDIR}${OPEN_FORTRAN_PARSER_PATH}/OpenFortranParser-${ROSE_OFP_MAJOR_VERSION_NUMBER}.${ROSE_OFP_MINOR_VERSION_NUMBER}.${ROSE_OFP_PATCH_VERSION_NUMBER}.jar:.
   fi
fi

export CLASSPATH
AC_SUBST(CLASSPATH)
# ROSE_SUPPORT_JAVA # This macro uses JAVA_HOME

AC_DEFINE_UNQUOTED([ROSE_OFP_CLASSPATH], $CLASSPATH , [OFP class path for Jave Virtual Machine])
# AC_DEFINE([ROSE_OFP_CLASSPATH], $CLASSPATH , [OFP class path for Jave Virtual Machine])

echo "    OFP CLASSPATH=$CLASSPATH"

# End macro ROSE_SUPPORT_OFP.
]
)
