include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ATERM_INCLUDE_DIR = $(ATERM_INSTALL_PATH)/include

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(ATERM_INCLUDE_DIR) -I$(top_srcdir)/src/frontend/Experimental_OpenFortranParser_ROSE_Connection -I${top_srcdir}/src/3rdPartyLibraries/experimental-fortran-parser/aterm_traversal

noinst_LTLIBRARIES = libexperimentalRoseFortran.la

# DQ (6/7/2013): I'm not clear if we need makefile conditionals, since the directory is already conditionally processed in the parent directory.

# if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
  libexperimentalRoseFortran_la_SOURCES = fortran_support.C ATermToUntypedFortranTraversal.C UntypedTraversal.C UntypedConverter.C
else
# Compile using a function that represents the API of the Open Fortran Parser
  libexperimentalRoseFortran_la_SOURCES = dummy_openFortranParser_main.C
endif

OFP_DIR = $(top_builddir)/src/3rdPartyLibraries/experimental-fortran-parser

pkginclude_HEADERS = fortran_support.h ATermToUntypedFortranTraversal.h UntypedTraversal.h UntypedConverter.h

OFP_SOURCE_FILES =

# DQ (3/13/2010): Moved OFP specific files to 3rdPartyLibraries/fortran-parser
$(OFP_SOURCE_FILES): extract_rose_required_ofp_source_files

# extract_rose_required_ofp_source_files:
#	cd ${top_builddir}/src/3rdPartyLibraries/experimental-fortran-parser; $(MAKE) extract_rose_required_ofp_source_files
