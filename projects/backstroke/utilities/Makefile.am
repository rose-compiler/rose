include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Build a library of all the reverser code
noinst_LIBRARIES =  libBackstrokeUtilities.a

libBackstrokeUtilities_ladir = .
libBackstrokeUtilities_a_DEPENDENCIES =
libBackstrokeUtilities_a_SOURCES = utilities.C
libBackstrokeUtilities_a_include_HEADERS = utilities.h types.h
libBackstrokeUtilities_a_includedir = $(includedir)/backstroke
noinst_HEADERS = cppDefinesAndNamespaces.h


EXTRA_DIST = 

CLEANFILES = 

ALL_TESTCODES =

check-local:

