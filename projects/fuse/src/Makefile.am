include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = fuse

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH)

INCLUDES = $(ROSE_INCLUDES)

fuse_CXXFLAGS =
fuse_LDADD = -lrose

fuse_SOURCES = \
	analysis/abstract_object.C \
	analysis/abstract_object_map.C \
	analysis/abstract_object_set.C \
	analysis/analysis.C \
	analysis/analysis_tester.C \
	analysis/compose.C \
	analysis/composed_analysis.C \
	analysis/factor_trans_system.C \
	analysis/graphIterator.C \
	analysis/partitions.C \
	analysis/stx_analysis.C \
	cfgUtils/CallGraphTraverse.C \
	cfgUtils/cfgUtils.C \
	cfgUtils/DataflowCFG.C \
	cfgUtils/VirtualCFGIterator.C \
	dbglog/dbglog.C \
	dbglog/widgets.C \
	dbglog/widgets/ats_graph.C \
	lattice/lattice.C \
	simpleAnalyses/call_context_sensitivity_analysis.C \
	simpleAnalyses/const_prop_analysis.C \
	simpleAnalyses/dead_path_elim_analysis.C \
	simpleAnalyses/live_dead_analysis.C \
	simpleAnalyses/ortho_array_analysis.C \
	simpleAnalyses/pointsToAnalysis.C \
	simpleAnalyses/printAnalysisStates.C \
	simpleAnalyses/saveDotAnalysis.C \
	simpleAnalyses/taintAnalysis.C \
	state/nodeState.C \
	compositionalTestAnalysis.C

fuse_include_HEADERS =\
	analysis/abstract_object.h \
	analysis/abstract_object_map.h \
	analysis/abstract_object_set.h \
	analysis/analysis.h \
	analysis/analysis_tester.h \
	analysis/compose.h \
	analysis/composed_analysis.h \
        analysis/data_structures.h \
	analysis/factor_trans_system.h \
	analysis/graphIterator.h \
	analysis/partitions.h \
	analysis/stx_analysis.h \
	cfgUtils/CallGraphTraverse.h \
	cfgUtils/cfgUtils.h \
	cfgUtils/DataflowCFG.h \
	cfgUtils/VirtualCFGIterator.h \
	dbglog/dbglog.h \
	dbglog/widgets.h \
	dbglog/widgets/ats_graph.h \
	lattice/lattice.h \
	lattice/latticeFull.h \
	simpleAnalyses/call_context_sensitivity_analysis.h \
	simpleAnalyses/const_prop_analysis.h \
	simpleAnalyses/dead_path_elim_analysis.h \
	simpleAnalyses/live_dead_analysis.h \
	simpleAnalyses/ortho_array_analysis.h \
	simpleAnalyses/pointsToAnalysis.h \
	simpleAnalyses/printAnalysisStates.h \
	simpleAnalyses/saveDotAnalysis.h \
	simpleAnalyses/taintAnalysis.h \
	simpleAnalyses/VariableStateTransfer.h \
	state/nodeState.h

EXTRA_DIST = \
	img/divDL.gif \
	img/divGO.gif