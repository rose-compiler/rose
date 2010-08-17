include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Build a library of all the reverser code
noinst_LIBRARIES =  libBackstrokeUtilities.a

libBackstrokeUtilities_ladir = .
libBackstrokeUtilities_a_DEPENDENCIES =
libBackstrokeUtilities_a_SOURCES = Utilities.C
libBackstrokeUtilities_a_include_HEADERS = Utilities.h types.h
libBackstrokeUtilities_a_includedir = $(includedir)/backstroke
noinst_HEADERS = CPPDefinesAndNamespaces.h


EXTRA_DIST = 

CLEANFILES = 

ALL_TESTCODES =

check-local:

