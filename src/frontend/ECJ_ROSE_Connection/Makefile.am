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
  lib_LTLIBRARIES = libjava_ecj_parser_c_jni_JavaParserActionJNI.la
endif

# DQ (10/21/2007): build (JJW: either empty or non-empty) lib file so that ROSE/src/Makefile.am need not be conditional on Fortran.
noinst_LTLIBRARIES = libroseJava.la

if ROSE_USE_OPEN_FORTRAN_PARSER
  libroseFortran_la_SOURCES = JavaTraversal.c
#  libroseFortran_la_SOURCES = 
else
# Compile using a function that represents the API of the Open Fortran Parser
  libroseFortran_la_SOURCES = dummy_JavaParser_main.C
endif

if ROSE_USE_OPEN_FORTRAN_PARSER
OFP_DIR = $(top_builddir)/src/3rdPartyLibraries/java-parser
libjava_ecj_parser_c_jni_JavaParserActionJNI_la_SOURCES = JavaParserActionROSE.C $(OFP_DIR)/token.c $(OFP_DIR)/jni_token_funcs.c $(OFP_DIR)/c_actions_jni.c

# DQ (10/11/2010): Put any required header files here.
include_HEADERS = 

OFP_SOURCE_FILES =

$(OFP_SOURCE_FILES): extract_rose_required_ecj_source_files

JavaParserActionROSE.C: $(OFP_DIR)/token.h $(OFP_DIR)/ActionEnums.h $(OFP_DIR)/jni_token_funcs.h

extract_rose_required_ecj_source_files:
	cd ${top_builddir}/src/3rdPartyLibraries/java-parser; $(MAKE) extract_rose_required_ecj_source_files
endif

