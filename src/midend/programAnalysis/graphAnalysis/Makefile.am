include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


INCLUDES =  $(ROSE_INCLUDES) 

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

noinst_LTLIBRARIES = libSB_GraphAnalysis.la

sourse_files = \
   $(srcdir)/CallGraphAnalysis.C \
   $(srcdir)/SB_Graph.C \
   $(srcdir)/RoseSrc_CallGraphAnalysis.C 

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   sourse_files += \
      $(srcdir)/RoseBin_DotGraph.cpp \
      $(srcdir)/RoseBin_Graph.cpp \
      $(srcdir)/RoseBin_GmlGraph.cpp
endif

# DQ (5/2/2009): I wonder why we can't form predicates this way (does not work in Automake).
#if ROSE_USE_NEW_GRAPH_NODES && ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
# if ROSE_USE_NEW_GRAPH_NODES
#if ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY
#libSB_GraphAnalysis_la_SOURCES = $(sourse_files)
#else
#libSB_GraphAnalysis_la_SOURCES = 
#endif
#else
libSB_GraphAnalysis_la_SOURCES = $(sourse_files)
#endif


clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates *.la *.lo

distclean-local:
	rm -rf Templates.DB

include_HEADERS =  CallGraphAnalysis.h RoseBin_DotGraph.h RoseBin_Graph.h RoseBin_GmlGraph.h SB_Graph.h RoseSrc_CallGraphAnalysis.h


EXTRA_DIST = CMakeLists.txt
