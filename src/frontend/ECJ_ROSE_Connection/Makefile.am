include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/ECJ_ROSE_Connection -I${top_builddir}/src/3rdPartyLibraries/java-parser $(JAVA_JVM_INCLUDE)

ECJ_DIR = $(top_builddir)/src/3rdPartyLibraries/java-parser

EXTRA_DIST = CMakeLists.txt
DISTCLEANFILES = 

noinst_LTLIBRARIES = libroseJava.la

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
# To build a similar library using libtool we indicate that we want an installed 
# library (which will then be made into a dynamic shared library).
lib_LTLIBRARIES = libJavaTraversal.la

# The file JavaTraversal.C is no longer used.  It #includes JavaTraversal.h which can
# no longer be built with openjdk (at least by version 12.0).  Thus JavaTraversal.{h,C}
# are removed from the build system.  The file $(ECJ_DIR)JavaParser.h still needs to
# be build [Rasmussen 2019.05.05].

# Use BUILT_SOURCES to ensure JavaParser.h is built before any other target
BUILT_SOURCES = JavaParser.h

# This rule will build both: $(ECJ_DIR)/JavaTraversal.h and $(ECJ_DIR)/JavaParser.h
# in the event that they don't already exist in $(ECJ_DIR).
JavaParser.h:
	cd ${top_builddir}/src/3rdPartyLibraries/java-parser; $(MAKE) extract_rose_required_ecj_source_files

libroseJava_la_SOURCES = \
	ecj.C \
	jserver.C \
	openJavaParser_main.C

libJavaTraversal_la_SOURCES = JavaParserActionROSE.C token.C jni_token.C JavaSourceCodePosition.C jni_JavaSourceCodePosition.C VisitorContext.C jni_utils.C java_support.C

pkginclude_HEADERS = ecj.h jserver.h java_support.h token.h jni_token.h JavaSourceCodePosition.h jni_JavaSourceCodePosition.h jni_utils.h VisitorContext.h

extract_rose_required_ecj_source_files:
	cd ${top_builddir}/src/3rdPartyLibraries/java-parser; $(MAKE) extract_rose_required_ecj_source_files
else
 # Compile using a function that represents the API of the Open Java Parser
   libroseJava_la_SOURCES = dummy_openJavaParser_main.C
endif
