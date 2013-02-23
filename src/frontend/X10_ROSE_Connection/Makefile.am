include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/X10_ROSE_Connection -I$(X10_DIR) $(X10_JVM_INCLUDE)

# DQ (2/14/2013): This needs to point to an X10 installation.
# X10_DIR = $(top_builddir)/src/3rdPartyLibraries/java-parser
X10_DIR = $(top_builddir)/src/3rdPartyLibraries

EXTRA_DIST = CMakeLists.txt
DISTCLEANFILES = 

if ROSE_BUILD_X10_LANGUAGE_SUPPORT
# To build a similar library using libtool we indicate that we want an installed 
# library (which will then be made into a dynamic shared library).
  lib_LTLIBRARIES = libx10Traversal.la
endif

noinst_LTLIBRARIES = libroseX10.la

x10Traversal.C: x10Traversal.h

# Make this dependent on files that will be unpacked (untared) 
# from the src/3rdPartyLibraries/java-parser/ecjROSE.jar file.
# X10ActionROSE.C: X10Traversal.h

# This rule will build both: $(ECJ_DIR)/JavaTraversal.h and $(ECJ_DIR)/JavaParser.h
# in the event that they don't already exist in $(ECJ_DIR).
#X10Traversal.h:
#	cd ${top_builddir}/src/3rdPartyLibraries/java-parser; $(MAKE) extract_rose_required_ecj_source_files

if ROSE_BUILD_X10_LANGUAGE_SUPPORT
   libroseX10_la_SOURCES = x10_main.C x10.C

# if !ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
# if !ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT && !ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
#if !ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT 
#   if !ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
    # Handle case of Java language support specified and Fortran language support NOT specified.
#      libroseX10_la_SOURCES += $(srcdir)/../OpenFortranParser_SAGE_Connection/jserver.C
#   endif
# endif

else
 # Compile using a function that represents the API of the Open Fortran Parser
   libroseX10_la_SOURCES = dummy_x10_main.C
endif

if ROSE_BUILD_X10_LANGUAGE_SUPPORT
# libx10Traversal_la_SOURCES = x10ActionROSE.C x10Traversal.C token.C jni_token.C x10SourceCodePosition.C jni_x10SourceCodePosition.C VisitorContext.C jni_utils.C x10_support.C
  libx10Traversal_la_SOURCES = x10ActionROSE.C x10Traversal.C x10_token.C jni_x10_token.C x10SourceCodePosition.C jni_x10SourceCodePosition.C jni_x10_utils.C x10_support.C

# include_HEADERS = x10.h x10_support.h token.h jni_token.h x10SourceCodePosition.h jni_x10SourceCodePosition.h jni_utils.h VisitorContext.h
  include_HEADERS = x10.h x10_support.h x10_token.h jni_x10_token.h x10SourceCodePosition.h jni_x10SourceCodePosition.h jni_x10_utils.h

# extract_rose_required_x10_source_files:
#	cd ${top_builddir}/src/3rdPartyLibraries/java-parser; $(MAKE) extract_rose_required_ecj_source_files
endif

