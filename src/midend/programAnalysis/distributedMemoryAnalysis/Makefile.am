include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_MPI

AM_CPPFLAGS =  $(ROSE_INCLUDES) -DROSE_MPI 

#LTCXXCOMPILE = \
#	$(LIBTOOL) --tag=CXX --mode=compile $(MPICXX) $(DEFS) \
#        $(DEFAULT_AM_CPPFLAGS) $(AM_CPPFLAGS) $(AM_CPPFLAGS) $(CPPFLAGS) \
#        $(AM_CXXFLAGS) $(CXXFLAGS)
#CXXCOMPILE = \
#	$(MPICXX) $(DEFS) \
#        $(DEFAULT_AM_CPPFLAGS) $(AM_CPPFLAGS) $(AM_CPPFLAGS) $(CPPFLAGS) \
#        $(AM_CXXFLAGS) $(CXXFLAGS)
#CXXLINK = $(LIBTOOL) --tag=CXX --mode=link $(MPICXX) $(AM_CXXFLAGS) \
#        $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@

noinst_LTLIBRARIES = libdistributedMemoryAnalysis.la
libdistributedMemoryAnalysis_la_SOURCES = DistributedMemoryAnalysis.C functionNames.C

pkginclude_HEADERS =  functionNames.h DistributedMemoryAnalysis.h DistributedMemoryAnalysisImplementation.h functionLevelTraversal.h

endif

EXTRA_DIST = CMakeLists.txt DistributedMemoryAnalysis.C functionNames.C functionNames.h \
             DistributedMemoryAnalysis.h \
	     DistributedMemoryAnalysisImplementation.h functionLevelTraversal.h
