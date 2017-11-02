include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

COBPT_INCLUDE_PATH = $(COBPT_INSTALL_PATH)/include

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(COBPT_INCLUDE_PATH) -I$(GMP_INCLUDE_PATH) -I$(top_srcdir)/src/frontend/Experimental_Cobol_ROSE_Connection

noinst_LTLIBRARIES = libexperimentalRoseCobol.la

# DQ (8/23/2017): I'm not clear if we need makefile conditionals, since the directory is already conditionally processed in the parent directory.

if ROSE_EXPERIMENTAL_COBOL_ROSE_CONNECTION
  libexperimentalRoseCobol_la_SOURCES = cobol_support.C rose_convert_cobol.C
else
# Compile using a function that represents the API of the cobol support.
  libexperimentalRoseCobol_la_SOURCES = dummy_cobol_main.C
endif

pkginclude_HEADERS = rose_convert_cobol.h
