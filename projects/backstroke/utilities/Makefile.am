include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build a library of all the reverser code
noinst_LTLIBRARIES =  libBackstrokeUtilities.la

libBackstrokeUtilities_ladir = .
libBackstrokeUtilities_la_DEPENDENCIES =
libBackstrokeUtilities_la_SOURCES = Utilities.C
include_HEADERS = Utilities.h types.h
noinst_HEADERS = CPPDefinesAndNamespaces.h
libBackstrokeUtilities_la_LIBADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


EXTRA_DIST = 

CLEANFILES = 

ALL_TESTCODES =

check-local:

