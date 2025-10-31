


########### next target ###############


########### install files ###############

install(FILES  functionNames.h DistributedMemoryAnalysis.h DistributedMemoryAnalysisImplementation.h functionLevelTraversal.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#if ROSE_MPI
#
#INCLUDES =  $(ROSE_INCLUDES) -DROSE_MPI 
#
##LTCXXCOMPILE = \
##	$(LIBTOOL) --tag=CXX --mode=compile $(MPICXX) $(DEFS) \
##        $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
##        $(AM_CXXFLAGS) $(CXXFLAGS)
##CXXCOMPILE = \
##	$(MPICXX) $(DEFS) \
##        $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
##        $(AM_CXXFLAGS) $(CXXFLAGS)
##CXXLINK = $(LIBTOOL) --tag=CXX --mode=link $(MPICXX) $(AM_CXXFLAGS) \
##        $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
#
#noinst_LTLIBRARIES = libdistributedMemoryAnalysis.la
#libdistributedMemoryAnalysis_la_SOURCES = DistributedMemoryAnalysis.C functionNames.C
#
#include_HEADERS =  functionNames.h DistributedMemoryAnalysis.h DistributedMemoryAnalysisImplementation.h functionLevelTraversal.h
#
#endif
#
#EXTRA_DIST = DistributedMemoryAnalysis.C functionNames.C functionNames.h \
#             DistributedMemoryAnalysis.h \
#	     DistributedMemoryAnalysisImplementation.h functionLevelTraversal.h
