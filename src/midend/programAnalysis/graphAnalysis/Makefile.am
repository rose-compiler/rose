include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


INCLUDES =  $(ROSE_INCLUDES) 

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .


noinst_LTLIBRARIES = libSB_GraphAnalysis.la

# DQ (11/8/2007): The runTest.cpp file was moved to tests/roseTests/programAnalysisTests/defUseAnalysisTests/runTest.C by Thomas.
# libDefUseAnalysis_la_SOURCES = $(srcdir)/GlobalVarAnalysis.cpp $(srcdir)/DefUseAnalysis.cpp $(srcdir)/DefUseAnalysis_perFunction.cpp $(srcdir)/dfaToDot.cpp $(srcdir)/runTest.cpp
libSB_GraphAnalysis_la_SOURCES = $(srcdir)/CallGraphAnalysis.C $(srcdir)/RoseBin_DotGraph.cpp \
	$(srcdir)/RoseBin_Graph.cpp $(srcdir)/RoseBin_GmlGraph.cpp $(srcdir)/SB_Graph.C $(srcdir)/RoseSrc_CallGraphAnalysis.C 




clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates *.la *.lo

distclean-local:
	rm -rf Templates.DB

include_HEADERS =  CallGraphAnalysis.h RoseBin_DotGraph.h RoseBin_Graph.h RoseBin_GmlGraph.h SB_Graph.h RoseSrc_CallGraphAnalysis.h

