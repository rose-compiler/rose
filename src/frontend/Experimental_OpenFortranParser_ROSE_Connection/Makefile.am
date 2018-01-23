include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ATERM_INCLUDE_DIR = $(ATERM_INSTALL_PATH)/include

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(ATERM_INCLUDE_DIR) -I$(top_srcdir)/src/frontend/Experimental_OpenFortranParser_ROSE_Connection -I${top_srcdir}/src/frontend/Experimental_General_Language_Support

noinst_LTLIBRARIES = libexperimentalRoseFortran.la

# DQ (6/7/2013): I'm not clear if we need makefile conditionals, since the directory is already conditionally processed in the parent directory.

if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
  libexperimentalRoseFortran_la_SOURCES = fortran_support.C ATermToUntypedFortranTraversal.C UntypedFortranTraversal.C UntypedFortranConverter.C
else
# Compile using a function that represents the API of the Open Fortran Parser
  libexperimentalRoseFortran_la_SOURCES = dummy_openFortranParser_main.C
endif

pkginclude_HEADERS = fortran_support.h

noinst_HEADERS = \
  ATermToUntypedFortranTraversal.h  \
  UntypedFortranTraversal.h         \
  UntypedFortranConverter.h
