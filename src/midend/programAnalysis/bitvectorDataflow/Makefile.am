include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../CFG -I$(top_srcdir)/src/util -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/util/support 

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

noinst_LTLIBRARIES = libbitvectorDataflow.la

# DQ (2/9/2014): This code does not compile using GNU g++ version 4.7 and later compilers.
# This requires more investigation to decide on a fix, but we might eliminate 
# this code in favor of more recent analysis work so it might not be work fixing).
# libbitvectorDataflow_la_SOURCES = DataFlowAnalysis.C  DefUseChain.C  ReachingDefinition.C ReachingDefinitionFacade.C
if ROSE_USING_GCC_VERSION_LATER_4_6
  libbitvectorDataflow_la_SOURCES = 
else
  libbitvectorDataflow_la_SOURCES = DataFlowAnalysis.C  DefUseChain.C  ReachingDefinition.C ReachingDefinitionFacade.C
endif

#libdataflowAnalysisSources = DataFlowAnalysis.C  ReachingDefinition.C

# lib_LTLIBRARIES = libdataflowAnalysis.a
# libdataflowAnalysis_a_SOURCES  = $(libdataflowAnalysisSources)


clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = DataFlowAnalysis.h DefUseChain.h ReachingDefinition.h ReachingDefinitionFacade.h DefUseChain.C









EXTRA_DIST = CMakeLists.txt
