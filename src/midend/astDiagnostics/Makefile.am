include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libastDiagnostics.la

libastDiagnostics_la_SOURCES = AstConsistencyTests.C AstWarnings.C AstStatistics.C AstPerformance.C

# DQ (3/7/2010): This code does not appear to be used or even distributed with ROSE any more.
# DQ (12/8/2006): Linux memory support used in ROSE
# ramustMemoryUsageObjs = \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/memcheck.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/filemap.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/processes.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/mem_totals.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/cleanup_memory.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/procps/alloc.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/procps/devname.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/procps/escape.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/procps/ksym.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/procps/pwcache.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/procps/readproc.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/procps/sig.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/procps/slab.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/procps/sysinfo.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/procps/version.o \
#  $(top_builddir)/src/3rdPartyLibraries/ramust/procps/whattime.o

# DQ (12/8/2006): Added to support memory useage under Linux
# libastDiagnostics_la_OBJECTS = AstConsistencyTests.o AstWarnings.o AstStatistics.o AstPerformance.o $(ramustMemoryUsageObjs)

include_HEADERS = AstDiagnostics.h AstConsistencyTests.h AstWarnings.h AstStatistics.h AstPerformance.h

clean-local:
	rm -rf Templates.DB ii_files ti_files core

EXTRA_DIST = CMakeLists.txt AstDiagnostics.docs
