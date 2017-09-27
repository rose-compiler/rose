include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/Experimental_Cobol_ROSE_Connection

noinst_LTLIBRARIES = libexperimentalRoseCobol.la

# DQ (8/23/2017): I'm not clear if we need makefile conditionals, since the directory is already conditionally processed in the parent directory.

if ROSE_EXPERIMENTAL_COBOL_ROSE_CONNECTION
  libexperimentalRoseCobol_la_SOURCES = cobol_support.C 
else
# Compile using a function that represents the API of the cobol support.
  libexperimentalRoseCobol_la_SOURCES = dummy_cobol_main.C
endif

pkginclude_HEADERS = cobol_support.h
