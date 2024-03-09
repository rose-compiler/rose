include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

FLANG_INCLUDES = -I$(FLANG_INSTALL_PATH)/include

AM_CPPFLAGS = $(ROSE_INCLUDES) $(FLANG_INCLUDES) -I$(top_srcdir)/src/3rdPartyLibraries/flang-parser -I$(top_srcdir)/src/frontend/Experimental_General_Language_Support

noinst_LTLIBRARIES = libexperimentalRoseFlang.la

if ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION
  libexperimentalRoseFlang_la_SOURCES = \
      fortran_flang_support.C           \
      unparse-sage.cpp                  \
      sage-build.C
endif

pkginclude_HEADERS = fortran_flang_support.h

noinst_HEADERS =   \
  sage-build.h
