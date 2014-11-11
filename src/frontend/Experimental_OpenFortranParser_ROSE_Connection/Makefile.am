include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ATERM_LIB_DIR = $(ATERM_INSTALL_PATH)/lib
ATERM_INCLUDE_DIR = $(ATERM_INSTALL_PATH)/include

# INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/Experimental_OpenFortranParser_ROSE_Connection -I${top_builddir}/src/3rdPartyLibraries/experimental_fortran-parser
# INCLUDES = $(ROSE_INCLUDES) -I$(ATERM_INCLUDE_DIR) -I$(top_srcdir)/src/frontend/Experimental_OpenFortranParser_ROSE_Connection -I${top_srcdir}/src/3rdPartyLibraries/experimental-fortran-parser -I${top_builddir}/src/3rdPartyLibraries/experimental-fortran-parser
INCLUDES = $(ROSE_INCLUDES) -I$(ATERM_INCLUDE_DIR) -I$(top_srcdir)/src/frontend/Experimental_OpenFortranParser_ROSE_Connection -I${top_srcdir}/src/3rdPartyLibraries/experimental-fortran-parser/aterm_traversal

#EXTRA_DIST = CMakeLists.txt preproc-fortran.ll preproc-fortran-fixed.ll
#DISTCLEANFILES = preproc-fortran.cc preproc-fortran-fixed.cc

noinst_LTLIBRARIES = libexperimentalRoseFortran.la

# DQ (6/7/2013): I'mnot clear if we need makefile conditionals, since the directory is already conditionally processed in the parent directory.

# if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
  libexperimentalRoseFortran_la_SOURCES = fortran_support.C $(srcdir)/UntypedASTBuilder.cpp 
# fortran_support.C fortran_error_handler.C openFortranParser_main.C jserver.C ofp.C FortranModuleInfo.C FortranParserState.C preproc-fortran.ll preproc-fortran-fixed.ll
else
# Compile using a function that represents the API of the Open Fortran Parser
  libexperimentalRoseFortran_la_SOURCES = dummy_openFortranParser_main.C
endif

OFP_DIR = $(top_builddir)/src/3rdPartyLibraries/experimental-fortran-parser

# if ROSE_USE_OPEN_FORTRAN_PARSER
# if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
# libfortran_ofp_parser_c_jni_FortranParserActionJNI_la_SOURCES = FortranParserActionROSE.C $(OFP_DIR)/token.c $(OFP_DIR)/jni_token_funcs.c $(OFP_DIR)/c_actions_jni.c

# DQ (3/13/2010): Moved OFP specific files to 3rdPartyLibraries/fortran-parser
pkginclude_HEADERS = fortran_support.h
# rose_fortran_token_maps.h  rose_token_defs.h fortran_support.h jserver.h ofp.h FortranModuleInfo.h FortranParserState.h fortran_error_handler.h

OFP_SOURCE_FILES =

$(OFP_SOURCE_FILES): extract_rose_required_ofp_source_files
# FortranParserActionROSE.C: $(OFP_DIR)/ActionEnums.h $(OFP_DIR)/token.h $(OFP_DIR)/jni_token_funcs.h

# extract_rose_required_ofp_source_files:
#	cd ${top_builddir}/src/3rdPartyLibraries/experimental-fortran-parser; $(MAKE) extract_rose_required_ofp_source_files
else

# Make a rule so that --enable-only-c "make dist" will not have problems.
$(OFP_DIR)/token.c:
	touch $(OFP_DIR)/token.c

$(OFP_DIR)/jni_token_funcs.c:
	touch $(OFP_DIR)/jni_token_funcs.c

$(OFP_DIR)/c_actions_jni.c:
	touch $(OFP_DIR)/c_actions_jni.c

# libfortran_ofp_parser_c_jni_FortranParserActionJNI_la_SOURCES = FortranParserActionROSE.C 
# FortranParserActionROSE.C: 

endif

