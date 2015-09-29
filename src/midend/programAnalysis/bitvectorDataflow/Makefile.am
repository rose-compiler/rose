include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../CFG -I$(top_srcdir)/src/util -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/util/support 

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

noinst_LTLIBRARIES = libbitvectorDataflow.la

libbitvectorDataflow_la_SOURCES = DataFlowAnalysis.C  DefUseChain.C  ReachingDefinition.C ReachingDefinitionFacade.C

#libdataflowAnalysisSources = DataFlowAnalysis.C  ReachingDefinition.C

# lib_LTLIBRARIES = libdataflowAnalysis.a
# libdataflowAnalysis_a_SOURCES  = $(libdataflowAnalysisSources)


clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

pkginclude_HEADERS = DataFlowAnalysis.h DefUseChain.h ReachingDefinition.h ReachingDefinitionFacade.h DefUseChain.C









EXTRA_DIST = CMakeLists.txt
