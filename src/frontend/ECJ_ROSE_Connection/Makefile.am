include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/ECJ_ROSE_Connection -I${top_builddir}/src/3rdPartyLibraries/java-parser $(JAVA_JVM_INCLUDE)

ECJ_DIR = $(top_builddir)/src/3rdPartyLibraries/java-parser

EXTRA_DIST = CMakeLists.txt
DISTCLEANFILES = 

noinst_LTLIBRARIES = libroseJava.la

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
# To build a similar library using libtool we indicate that we want an installed 
# library (which will then be made into a dynamic shared library).
lib_LTLIBRARIES = libJavaTraversal.la

JavaTraversal.C: JavaTraversal.h

# Make this dependent on files that will be unpacked (untared) 
# from the src/3rdPartyLibraries/java-parser/ecjROSE.jar file.
JavaParserActionROSE.C: JavaTraversal.h

# This rule will build both: $(ECJ_DIR)/JavaTraversal.h and $(ECJ_DIR)/JavaParser.h
# in the event that they don't already exist in $(ECJ_DIR).
JavaTraversal.h:
	cd ${top_builddir}/src/3rdPartyLibraries/java-parser; $(MAKE) extract_rose_required_ecj_source_files

libroseJava_la_SOURCES = openJavaParser_main.C ecj.C

if !ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
 # Handle case of Java language support specified and Fortran language support NOT specified.
   libroseJava_la_SOURCES += $(srcdir)/../OpenFortranParser_SAGE_Connection/jserver.C
endif

libJavaTraversal_la_SOURCES = JavaParserActionROSE.C JavaTraversal.C token.C jni_token.C JavaSourceCodePosition.C jni_JavaSourceCodePosition.C VisitorContext.C jni_utils.C java_support.C

include_HEADERS = ecj.h java_support.h token.h jni_token.h JavaSourceCodePosition.h jni_JavaSourceCodePosition.h jni_utils.h VisitorContext.h

extract_rose_required_ecj_source_files:
	cd ${top_builddir}/src/3rdPartyLibraries/java-parser; $(MAKE) extract_rose_required_ecj_source_files
else
 # Compile using a function that represents the API of the Open Fortran Parser
   libroseJava_la_SOURCES = dummy_openJavaParser_main.C
endif
