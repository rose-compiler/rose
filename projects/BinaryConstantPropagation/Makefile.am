include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS =

# DQ (2/10/2009): Added support for automake conditional
# The constant propogation analysis depends upon the assembly semantics
# so we need to reflect this in the Makefile.am
if ROSE_USE_ASSEMBLY_SEMANTICS

include_HEADERS = findConstants.h flowEquations.h

bin_PROGRAMS = findConstants

findConstants_SOURCES = findConstants.C

findConstants_LDADD = $(ROSE_LIBS) -lrt

INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) -I$(top_srcdir)/projects/assemblyToSourceAst -I.

endif

CLEANFILES = 

EXTRA_DIST =

