include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_MPI

INCLUDES =  $(ROSE_INCLUDES) -DROSE_MPI 

LTCXXCOMPILE = \
	$(LIBTOOL) --tag=CXX --mode=compile $(MPICXX) $(DEFS) \
        $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
        $(AM_CXXFLAGS) $(CXXFLAGS)
CXXCOMPILE = \
	$(MPICXX) $(DEFS) \
        $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
        $(AM_CXXFLAGS) $(CXXFLAGS)
CXXLINK = $(LIBTOOL) --tag=CXX --mode=link $(MPICXX) $(AM_CXXFLAGS) \
        $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@

noinst_LTLIBRARIES = libdistributedMemoryAnalysis.la
libdistributedMemoryAnalysis_la_SOURCES = DistributedMemoryAnalysis.C functionNames.C
libdistributedMemoryAnalysis_la_LIBADD = $(LIBS_WITH_RPATH) #$(ROSE_LIBS) 

libdistributedMemoryAnalysis.la: $(libdistributedMemoryAnalysis_la_OBJECTS) $(libdistributedMemoryAnalysis_la_DEPENDENCIES) 
	$(CXXLINK) -rpath $(libdir) $(libdistributedMemoryAnalysis_la_LDFLAGS) $(libdistributedMemoryAnalysis_la_OBJECTS) $($(COMPASS_CHECKER_la_LIBADD) $(LIBS)

DistributedMemoryAnalysis.C:
	$(LIBTOOL) --tag=CXX --mode=link $(MPICXX) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o DistributedMemoryAnalysis.o

functionNames.C:
	$(LIBTOOL) --tag=CXX --mode=link $(MPICXX) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o functionNames.o

clean-local:

distclean-local:

include_HEADERS =  functionNames.h DistributedMemoryAnalysis.h DistributedMemoryAnalysisImplementation.h functionLevelTraversal.h

endif
