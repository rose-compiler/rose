include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/ECJ_ROSE_Connection -I${top_builddir}/src/3rdPartyLibraries/java-parser $(JAVA_JVM_INCLUDE)

ECJ_DIR = $(top_builddir)/src/3rdPartyLibraries/java-parser

EXTRA_DIST = CMakeLists.txt
DISTCLEANFILES = 

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
# To build a similar library using libtool we indicate that we want an installed 
# library (which will then be made into a dynamic shared library).
  lib_LTLIBRARIES = libJavaTraversal.la
endif

noinst_LTLIBRARIES = libroseJava.la

JavaTraversal.C: JavaTraversal.h

# Make this dependent on files that will be unpacked (untared) 
# from the src/3rdPartyLibraries/java-parser/ecjROSE.jar file.
# JavaParserActionROSE.C: JavaTraversal.h $(ECJ_DIR)/JavaParser.h
JavaParserActionROSE.C: JavaTraversal.h

# This rule will build both: $(ECJ_DIR)/JavaTraversal.h and $(ECJ_DIR)/JavaParser.h
# in the event that they don't already exist in $(ECJ_DIR).
JavaTraversal.h:
	cd ${top_builddir}/src/3rdPartyLibraries/java-parser; $(MAKE) extract_rose_required_ecj_source_files

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
 # libroseJava_la_SOURCES = JavaParserActionROSE.C java_support.C openJavaParser_main.C ecj.C
   libroseJava_la_SOURCES = java_support.C openJavaParser_main.C ecj.C
 # libroseJava_la_SOURCES = java_support.C openJavaParser_main.C ecj.C

if !ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
 # Handle case of Java language support specified and Fortran language support NOT specified.
   libroseJava_la_SOURCES += $(srcdir)/../OpenFortranParser_SAGE_Connection/jserver.C
endif

else
 # Compile using a function that represents the API of the Open Fortran Parser
   libroseJava_la_SOURCES = dummy_openJavaParser_main.C
endif

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
 # libJavaTraversal_la_SOURCES = JavaTraversal.C 
   libJavaTraversal_la_SOURCES = JavaParserActionROSE.C JavaTraversal.C 

 # DQ (10/11/2010): Put any required header files here.
   include_HEADERS = ecj.h java_support.h

# JavaParserActionROSE.C: $(ECJ_DIR)/token.h $(ECJ_DIR)/ActionEnums.h $(ECJ_DIR)/jni_token_funcs.h
# JavaParserActionROSE.C:

extract_rose_required_ecj_source_files:
	cd ${top_builddir}/src/3rdPartyLibraries/java-parser; $(MAKE) extract_rose_required_ecj_source_files
endif

