include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (3/13/2010): Moved OFP specific source files to src/3rdPartyLibraries/fortran-parser
# These are automatically extracted from the OFP jar file.
INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/ECJ_ROSE_Connection -I${top_builddir}/src/3rdPartyLibraries/java-parser

EXTRA_DIST = CMakeLists.txt
DISTCLEANFILES = 

# DQ (10/11/2010): Note that building the lib is just:
# g++ -fPIC -shared $(ROSE_INCLUDES) JavaTraversal.c -o libJavaTraversal.so

# For now use Java whenever we use Fortran (since both require Java support).
if ROSE_USE_OPEN_FORTRAN_PARSER
# To build a similar library using libtool we indicate that we want an installed 
# library (which will then be made into a dynamic shared library).
# lib_LTLIBRARIES = libjava_ecj_parser_c_jni_JavaParserActionJNI.la
  lib_LTLIBRARIES = libJavaTraversal.la
endif

# DQ (10/21/2007): build (JJW: either empty or non-empty) lib file so that ROSE/src/Makefile.am need not be conditional on Fortran.
# noinst_LTLIBRARIES = libroseJava.la
# noinst_LTLIBRARIES = libJavaTraversal.la
noinst_LTLIBRARIES = libroseJava.la

JavaTraversal.C: JavaTraversal.h

JavaTraversal.h:
	cd ${top_builddir}/src/3rdPartyLibraries/parser-parser; $(MAKE) extract_rose_required_ecj_source_files

if ROSE_USE_OPEN_FORTRAN_PARSER
 # libroseJava_la_SOURCES = JavaTraversal.c
 # libJavaTraversal_la_SOURCES = JavaTraversal.C
 # libJavaTraversal_la_SOURCES = openJavaParser_main.C 
 # libroseJava_la_SOURCES = 
   libroseJava_la_SOURCES = openJavaParser_main.C ecj.C
else
 # Compile using a function that represents the API of the Open Fortran Parser
 # libroseJava_la_SOURCES = dummy_JavaParser_main.C
 # libJavaTraversal_la_SOURCES = dummy_JavaParser_main.C
   libroseJava_la_SOURCES = dummy_JavaParser_main.C
endif

if ROSE_USE_OPEN_FORTRAN_PARSER
ECJ_DIR = $(top_builddir)/src/3rdPartyLibraries/java-parser
# libjava_ecj_parser_c_jni_JavaParserActionJNI_la_SOURCES = JavaTraversal.C $(ECJ_DIR)/token.c $(ECJ_DIR)/jni_token_funcs.c $(ECJ_DIR)/c_actions_jni.c
# libjava_ecj_parser_c_jni_JavaParserActionJNI_la_SOURCES = JavaTraversal.C 
libJavaTraversal_la_SOURCES = JavaTraversal.C 

# DQ (10/11/2010): Put any required header files here.
include_HEADERS = 

# OFP_SOURCE_FILES =
# $(OFP_SOURCE_FILES): extract_rose_required_ecj_source_files

JavaParserActionROSE.C: $(ECJ_DIR)/token.h $(ECJ_DIR)/ActionEnums.h $(ECJ_DIR)/jni_token_funcs.h

extract_rose_required_ecj_source_files:
	cd ${top_builddir}/src/3rdPartyLibraries/java-parser; $(MAKE) extract_rose_required_ecj_source_files
endif

