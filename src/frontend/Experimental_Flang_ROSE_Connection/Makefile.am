include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

FLANG_INCLUDES = -I$(FLANG_SOURCE_PATH)/lib/parser

# NOTE: This doesn't work because CXXFLAGS comes after AM_CXXFLAGS.  Usage of CXXFLAGS in ROSE
# should be changed to AM_CXXFLAGS so that user's CXXFLAGS can be changed without affecting entire project Makefiles!
# At the moment These files need to be build by hand:
#   1. fortran_flang_support.C requires normal CXXFLAGS
#   2. external-build.C and external-build-with-symbols.C require std=c++17
AM_CPPFLAGS = $(ROSE_INCLUDES) $(FLANG_INCLUDES) -I$(top_srcdir)/src/3rdPartyLibraries/flang-parser -I$(top_srcdir)/src/frontend/Experimental_General_Language_Support
# Temporary: uncomment the language standard option in Makefile after fortran_flang_support.C is compiled
AM_CXXFLAGS = $(CXXFLAGS) # -std=c++17 (replaced by .cc.lo specialization below)
CXXFLAGS =

noinst_LTLIBRARIES = libexperimentalRoseFlang.la

if ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION
  libexperimentalRoseFlang_la_SOURCES = \
      fortran_flang_support.C           \
      sage-build.cc
endif

pkginclude_HEADERS = fortran_flang_support.h

noinst_HEADERS =   \
  sage-build.h

# specialize for compilation of .cc files (in this case files following C++17 standard)
.cc.lo:
	$(AM_V_CXX)$(LTCXXCOMPILE) -std=c++17 -c -o $@ $<
