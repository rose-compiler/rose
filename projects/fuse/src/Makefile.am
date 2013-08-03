include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = fuse

fuse_CXXFLAGS =
fuse_LDADD = -lrose
fuse_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH)

INCLUDES = \
	$(ROSE_INCLUDES) \
	-I$(srcdir)/analysis \
	-I$(srcdir)/cfgUtils \
	-I$(srcdir)/dbglog \
	-I$(srcdir)/dbglog/widgets \
	-I$(srcdir)/lattice \
	-I$(srcdir)/simpleAnalyses \
	-I$(srcdir)/state

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

include_HEADERS =\
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

FUSE_TESTS_DIR = $(top_srcdir)/projects/fuse/src/tests

# for .C files
FUSE_C_TESTS = \
	$(FUSE_TESTS_DIR)/compositionalTestInput1.C \
	$(FUSE_TESTS_DIR)/compositionalTestInput1b.C \
	$(FUSE_TESTS_DIR)/compositionalTestInput2.C \
	$(FUSE_TESTS_DIR)/compositionalTestInput2b.C \
	$(FUSE_TESTS_DIR)/compositionalTestInput3.C \
	$(FUSE_TESTS_DIR)/compositionalTestInput4.C \
	$(FUSE_TESTS_DIR)/compositionalTestInput5b.C \
	$(FUSE_TESTS_DIR)/compositionalTestInput6.C \
	$(FUSE_TESTS_DIR)/compositionalTestInput7.C \
	$(FUSE_TESTS_DIR)/compositionalTestInput.jisheng.C \
	$(FUSE_TESTS_DIR)/cp_test1.C 

# for .cpp files
FUSE_CPP_TESTS = \
	$(FUSE_TESTS_DIR)/simplePtrTest.cpp \
	$(FUSE_TESTS_DIR)/sas13Fig1Example.cpp

FUSE_C_CHECKS = ${FUSE_C_TESTS:.C=.C-o}
FUSE_CPP_CHECKS = ${FUSE_CPP_TESTS:.cpp=.cpp-o}

$(FUSE_C_CHECKS): fuse $(FUSE_TESTS_DIR)/$(@:.C=.C-o)
	./fuse -c --edg:no_warnings $(@:.C-o=.C)

$(FUSE_CPP_CHECKS):fuse $(FUSE_TESTS_DIR)/$(@:.cpp=.cpp-o)
	./fuse -c --edg:no_warnings $(@:.cpp-o=.cpp)

check-local: $(FUSE_C_CHECKS) $(FUSE_CPP_CHECKS)

clean-local:
	rm -rf *.dot *.png dbg/

EXTRA_DIST = \
	img/divDL.gif \
	img/divGO.gif