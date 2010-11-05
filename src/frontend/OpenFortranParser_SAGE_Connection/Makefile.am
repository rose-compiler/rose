include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (11/3/2010): Include the jni.h file by specifying the location explicitly. Note that
# on systems where the "jni.h" file is installed in the gcc includes
# (e.g. at LLNL in: /nfs/apps/gcc/4.2.4/lib/gcc/x86_64-unknown-linux-gnu/4.2.4/include/jni.h)
# this will not prevent the wrong jni.h from being included.
# INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/OpenFortranParser_SAGE_Connection -I${top_builddir}/src/3rdPartyLibraries/fortran-parser
INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/OpenFortranParser_SAGE_Connection -I${top_builddir}/src/3rdPartyLibraries/fortran-parser $(JAVA_JVM_INCLUDE)

EXTRA_DIST = CMakeLists.txt preproc-fortran.ll preproc-fortran-fixed.ll
DISTCLEANFILES = preproc-fortran.cc preproc-fortran-fixed.cc

# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
# To build a similar library using libtool we indicate that we want an installed 
# library (which will then be made into a dynamic shared library).
  lib_LTLIBRARIES = libfortran_ofp_parser_c_jni_FortranParserActionJNI.la
endif

# DQ (10/21/2007): build (JJW: either empty or non-empty) lib file so that ROSE/src/Makefile.am need not be conditional on Fortran.
noinst_LTLIBRARIES = libroseFortran.la

# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  libroseFortran_la_SOURCES = fortran_support.C fortran_error_handler.C openFortranParser_main.C jserver.C ofp.C FortranModuleInfo.C FortranParserState.C preproc-fortran.ll preproc-fortran-fixed.ll
else
# Compile using a function that represents the API of the Open Fortran Parser
  libroseFortran_la_SOURCES = dummy_openFortranParser_main.C
endif

OFP_DIR = $(top_builddir)/src/3rdPartyLibraries/fortran-parser

# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
libfortran_ofp_parser_c_jni_FortranParserActionJNI_la_SOURCES = FortranParserActionROSE.C $(OFP_DIR)/token.c $(OFP_DIR)/jni_token_funcs.c $(OFP_DIR)/c_actions_jni.c

# DQ (3/13/2010): Moved OFP specific files to 3rdPartyLibraries/fortran-parser
include_HEADERS = rose_fortran_token_maps.h  rose_token_defs.h fortran_support.h jserver.h ofp.h FortranModuleInfo.h FortranParserState.h fortran_error_handler.h

# DQ (3/13/2010): Extract the required OFP source files needed for ROSE Fortran support.
#OFP_SOURCE_FILES = \
#   $(OFP_DIR)/FortranParserAction.h \
#   $(OFP_DIR)/c_actions_jni.c \
#   $(OFP_DIR)/token.h \
#   $(OFP_DIR)/token.c \
#   $(OFP_DIR)/ActionEnums.h \
#   $(OFP_DIR)/jni_token_funcs.h \
#   $(OFP_DIR)/jni_token_funcs.c

OFP_SOURCE_FILES =


# DQ (3/13/2010): The OFP files are extracted when we build the sageSupport.C file
# (well before buildign the files here).  We don't want to specify the rules twice 
# since that is a problem for the "make distcheck" rule.

$(OFP_SOURCE_FILES): extract_rose_required_ofp_source_files
FortranParserActionROSE.C: $(OFP_DIR)/ActionEnums.h $(OFP_DIR)/token.h $(OFP_DIR)/jni_token_funcs.h

extract_rose_required_ofp_source_files:
	cd ${top_builddir}/src/3rdPartyLibraries/fortran-parser; $(MAKE) extract_rose_required_ofp_source_files
else

# Make a rule so that --enable-only-c "make dist" will not have problems.
$(OFP_DIR)/token.c:
	touch $(OFP_DIR)/token.c

$(OFP_DIR)/jni_token_funcs.c:
	touch $(OFP_DIR)/jni_token_funcs.c

$(OFP_DIR)/c_actions_jni.c:
	touch $(OFP_DIR)/c_actions_jni.c

libfortran_ofp_parser_c_jni_FortranParserActionJNI_la_SOURCES = FortranParserActionROSE.C 
FortranParserActionROSE.C: 

endif

