include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Build a library of all the reverser code
noinst_LTLIBRARIES =  libBackstrokeUtilities.la

libBackstrokeUtilities_ladir = .
libBackstrokeUtilities_la_DEPENDENCIES =
libBackstrokeUtilities_la_SOURCES = Utilities.C
libBackstrokeUtilities_la_include_HEADERS = Utilities.h types.h
libBackstrokeUtilities_la_includedir = $(includedir)/backstroke
noinst_HEADERS = CPPDefinesAndNamespaces.h


EXTRA_DIST = 

CLEANFILES = 

ALL_TESTCODES =

check-local:

