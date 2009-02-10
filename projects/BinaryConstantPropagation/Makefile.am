include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS =

include_HEADERS = findConstants.h flowEquations.h

bin_PROGRAMS = findConstants

findConstants_SOURCES = findConstants.C

findConstants_LDADD = $(ROSE_LIBS) -lrt

INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I$(top_srcdir)/projects/assemblyToSourceAst -I.

CLEANFILES = 

EXTRA_DIST =

