include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_GCC_OMP
INCLUDES_OMP = -DROSE_GCC_OMP 
endif

INCLUDES =  $(ROSE_INCLUDES) \
   -I$(srcdir)/.. -I$(srcdir)/../bitvectorDataflow -I$(srcdir)/../CFG -I$(top_srcdir)/src/midend/loopProcessing \
   -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/midend/astUtil/astInterface \
   -I$(top_srcdir)/src/midend/astUtil/annotation -I$(top_srcdir)/src/midend/astUtil/symbolicVal  \
   -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs \
   $(INCLUDES_OMP)

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .


noinst_LTLIBRARIES = libDefUseAnalysis.la

# DQ (11/8/2007): The runTest.cpp file was moved to tests/roseTests/programAnalysisTests/defUseAnalysisTests/runTest.C by Thomas.
# libDefUseAnalysis_la_SOURCES = $(srcdir)/GlobalVarAnalysis.cpp $(srcdir)/DefUseAnalysis.cpp $(srcdir)/DefUseAnalysis_perFunction.cpp $(srcdir)/dfaToDot.cpp $(srcdir)/runTest.cpp
libDefUseAnalysis_la_SOURCES = $(srcdir)/GlobalVarAnalysis.cpp $(srcdir)/DefUseAnalysis.cpp $(srcdir)/DefUseAnalysis_perFunction.cpp $(srcdir)/dfaToDot.cpp $(srcdir)/LivenessAnalysis.cpp $(srcdir)/DefUseAnalysisAbstract.cpp



clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS =  DefUseAnalysis.h  BottomUpTraversalLiveness.h DefUseAnalysis_perFunction.h  DFAFilter.h  DFAnalysis.h  dfaToDot.h  GlobalVarAnalysis.h  support.h LivenessAnalysis.h DefUseAnalysisAbstract.h

EXTRA_DIST = CMakeLists.txt
