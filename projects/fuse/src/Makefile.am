include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = fuse

fuse_CXXFLAGS =
fuse_LDADD = -lrose
fuse_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH)

AM_CPPFLAGS = \
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
	analysis/comp_shared_ptr.h \
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

# Liao, 1/16/2014, comment out missing files to pass make dist
#	dbglog/index.html  # used by the dbglog system
#	src/factor_trans_systemTest.C 
EXTRA_DIST = \
	dbglog/dbgLogGraphTester.C \
	dbglog/dbgLogTester.C \
	img/divDL.gif \
	img/divGO.gif

FUSE_TESTS_DIR = $(top_srcdir)/projects/fuse/src/tests

# for .C files
FUSE_C_TESTS = \
	compositionalTestInput1b.C \
	compositionalTestInput2b.C \
	compositionalTestInput3.C \
	compositionalTestInput4.C \
	compositionalTestInput5.C \
	compositionalTestInput5b.C \
	compositionalTestInput6.C \
	compositionalTestInput7.C \
	compositionalTestInput.jisheng.C \
	cp_test1.C 

FUSE_C_TESTS_TIMING_OUT=\
	compositionalTestInput1.C \
	compositionalTestInput2.C

# for .cpp files
FUSE_CPP_TESTS = \
	simplePtrTest.cpp \
	sas13Fig1Example.cpp

#FUSE_C_CHECKS = ${FUSE_C_TESTS:.C=.C-o}
#FUSE_CPP_CHECKS = ${FUSE_CPP_TESTS:.cpp=.cpp-o}
#$(FUSE_C_CHECKS): fuse $(FUSE_TESTS_DIR)/$(@:.C=.C-o)
#	./fuse -c --edg:no_warnings $(@:.C-o=.C)

#$(FUSE_CPP_CHECKS):fuse $(FUSE_TESTS_DIR)/$(@:.cpp=.cpp-o)
#	./fuse -c --edg:no_warnings $(@:.cpp-o=.cpp)

FUSE_C_CHECKS = $(addprefix fuse_C_, $(addsuffix .passed, $(FUSE_C_TESTS)))
FUSE_CPP_CHECKS = $(addprefix fuse_cpp_, $(addsuffix .passed, $(FUSE_CPP_TESTS)))

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status
FUSE_FLAGS = 
FUSE = ./fuse


$(FUSE_C_CHECKS): fuse_C_%.passed: $(FUSE_TESTS_DIR)/% $(TEST_EXIT_STATUS) $(FUSE)
	@$(RTH_RUN) CMD="./fuse  -I$(FUSE_TESTS_DIR) -c $<" $(TEST_EXIT_STATUS) $@

$(FUSE_CPP_CHECKS): fuse_cpp_%.passed: $(FUSE_TESTS_DIR)/% $(TEST_EXIT_STATUS) $(FUSE)
	@$(RTH_RUN) CMD="./fuse  -I$(FUSE_TESTS_DIR) -c $<" $(TEST_EXIT_STATUS) $@


.PHONY: check-fuse
check-fuse: $(FUSE_C_CHECKS) $(FUSE_CPP_CHECKS)

CXX_TESTDIR = $(top_srcdir)/tests/CompileTests/Cxx_tests
CXX_TESTS_REQUIRED_TO_PASS = \
test2001_01.C \
test2001_02.C \
test2001_05.C \
test2001_06.C \
test2001_07.C \
test2001_08.C \
test2001_09.C \
test2001_10.C \
test2001_11.C \
test2001_12.C \
test2001_13.C \
test2001_15.C \
test2001_16.C \
test2001_17.C \
test2001_18.C \
test2001_19.C \
test2001_20.C \
test2001_21.C \
test2001_22.C \
test2001_23.C \
test2001_24.C \
test2001_25.C \
test2001_27.C \
test2001_29.C \
test2002_01.C \
test2003_02.C \
test2003_03.C \
test2003_04.C \
test2003_05.C \
test2003_06.C \
test2003_08.C \
test2003_09.C \
test2003_10.C \
test2003_11.C \
test2003_12.C \
test2003_13.C \
test2003_14.C \
test2003_16.C \
test2003_17.C \
test2003_20.C \
test2003_21.C \
test2003_23.C \
test2003_25.C \
test2003_26.C \
test2003_27.C \
test2003_28.C \
test2003_29.C \
test2003_30.C \
test2003_31.C \
test2003_37.C \
test2003_38.C \
test2004_01.C \
test2004_03.C \
test2004_04.C \
test2004_05.C \
test2004_06.C \
test2004_07.C \
test2004_08.C \
test2004_09.C \
test2004_100.C \
test2004_101.C \
test2004_102.C \
test2004_103.C \
test2004_104.C \
test2004_105.C \
test2004_106.C \
test2004_107.C \
test2004_108.C \
test2004_109.C \
test2004_110.C \
test2004_111.C \
test2004_112.C \
test2004_113.C \
test2004_114.C \
test2004_115.C \
test2004_117.C \
test2004_118.C \
test2004_119.C \
test2004_121.C \
test2004_122.C \
test2004_123.C \
test2004_124.C \
test2004_125.C \
test2004_128.C \
test2004_12.C \
test2004_131.C \
test2004_132.C \
test2004_133.C \
test2004_134.C \
test2004_135.C \
test2004_136.C \
test2004_137.C \
test2004_138.C \
test2004_13.C \
test2004_140.C \
test2004_144.C \
test2004_146.C \
test2004_147.C \
test2004_148.C \
test2004_150.C \
test2004_151.C \
test2004_152.C \
test2004_153.C \
test2004_154.C \
test2004_155.C \
test2004_156.C \
test2004_158.C \
test2004_163.C \
test2004_16.C \
test2004_17.C \
test2004_23.C \
test2004_24.C \
test2004_26.C \
test2004_27.C \
test2004_28.C \
test2004_29.C \
test2004_31.C \
test2004_32.C \
test2004_33.C \
test2004_34.C \
test2004_37.C \
test2004_38.C \
test2004_39.C \
test2004_40.C \
test2004_42.C \
test2004_49.C \
test2004_50.C \
test2004_51.C \
test2004_53.C \
test2004_54.C \
test2004_56.C \
test2004_58.C \
test2004_59.C \
test2004_61.C \
test2004_62.C \
test2004_64.C \
test2004_65.C \
test2004_67.C \
test2004_68.C \
test2004_69.C \
test2004_70.C \
test2004_71.C \
test2004_73.C \
test2004_74.C \
test2004_78.C \
test2004_79.C \
test2004_80.C \
test2004_81.C \
test2004_83.C \
test2004_84.C \
test2004_86.C \
test2004_87.C \
test2004_89.C \
test2004_91.C \
test2004_93.C \
test2004_94.C \
test2004_95.C \
test2004_96.C \
test2004_97.C \
test2004_98.C \
test2004_99.C \
test2005_01.C \
test2005_02.C \
test2005_03.C \
test2005_05.C \
test2005_105.C \
test2005_107.C \
test2005_109.C \
test2005_110.C \
test2005_111.C \
test2005_113.C \
test2005_116.C \
test2005_117.C \
test2005_119.C \
test2005_11.C \
test2005_121.C \
test2005_124.C \
test2005_125.C \
test2005_126.C \
test2005_127.C \
test2005_135.C \
test2005_13.C \
test2005_140.C \
test2005_144.C \
test2005_145.C \
test2005_148.C \
test2005_149.C \
test2005_14.C \
test2005_150.C \
test2005_153.C \
test2005_154.C \
test2005_157.C \
test2005_159.C \
test2005_15.C \
test2005_160.C \
test2005_162.C \
test2005_169.C \
test2005_16.C \
test2005_170.C \
test2005_171.C \
test2005_172.C \
test2005_173.C \
test2005_175.C \
test2005_176.C \
test2005_179.C \
test2005_180.C \
test2005_182.C \
test2005_183.C \
test2005_184.C \
test2005_185.C \
test2005_189.C \
test2005_190.C \
test2005_191.C \
test2005_192.C \
test2005_195.C \
test2005_196.C \
test2005_197.C \
test2005_198.C \
test2005_203.C \
test2005_206.C \
test2005_207.C \
test2005_208.C \
test2005_209.C \
test2005_20.C \
test2005_21.C \
test2005_22.C \
test2005_23.C \
test2005_24.C \
test2005_25.C \
test2005_26.C \
test2005_30.C \
test2005_31.C \
test2005_36.C \
test2005_40.C \
test2005_41.C \
test2005_45.C \
test2005_47.C \
test2005_48.C \
test2005_49.C \
test2005_50.C \
test2005_51.C \
test2005_52.C \
test2005_55.C \
test2005_58.C \
test2005_59.C \
test2005_60.C \
test2005_61.C \
test2005_62.C \
test2005_64.C \
test2005_65.C \
test2005_66.C \
test2005_67.C \
test2005_68.C \
test2005_70.C \
test2005_72.C \
test2005_78.C \
test2005_80.C \
test2005_83.C \
test2005_84.C \
test2005_86.C \
test2005_87.C \
test2005_88.C \
test2005_89.C \
test2005_90.C \
test2005_91.C \
test2005_92.C \
test2005_94.C \
test2005_95.C \
test2005_99.C \
test2006_01.C \
test2006_02.C \
test2006_03.C \
test2006_06.C \
test2006_07.C \
test2006_08.C \
test2006_101.C \
test2006_104.C \
test2006_105.C \
test2006_106.C \
test2006_107.C \
test2006_108.C \
test2006_109.C \
test2006_10.C \
test2006_111.C \
test2006_113.C \
test2006_114.C \
test2006_116.C \
test2006_117.C \
test2006_118.C \
test2006_120.C \
test2006_121.C \
test2006_122.C \
test2006_123.C \
test2006_129.C \
test2006_12.C \
test2006_13.C \
test2006_140.C \
test2006_142.C \
test2006_143.C \
test2006_144.C \
test2006_145.C \
test2006_146.C \
test2006_149.C \
test2006_14.C \
test2006_150.C \
test2006_151.C \
test2006_152.C \
test2006_153.C \
test2006_155.C \
test2006_156.C \
test2006_157.C \
test2006_158.C \
test2006_161.C \
test2006_162.C \
test2006_163.C \
test2006_164.C \
test2006_165.C \
test2006_16.C \
test2006_177.C \
test2006_178.C \
test2006_179.C \
test2006_17.C \
test2006_181.C \
test2006_183.C \
test2006_18.C \
test2006_19.C \
test2006_22.C \
test2006_23.C \
test2006_25.C \
test2006_26.C \
test2006_34.C \
test2006_37.C \
test2006_38.C \
test2006_41.C \
test2006_42.C \
test2006_44.C \
test2006_46.C \
test2006_52.C \
test2006_53.C \
test2006_55.C \
test2006_56.C \
test2006_57.C \
test2006_59.C \
test2006_62.C \
test2006_63.C \
test2006_66.C \
test2006_67.C \
test2006_68.C \
test2006_69.C \
test2006_70.C \
test2006_71.C \
test2006_72.C \
test2006_73.C \
test2006_75.C \
test2006_76.C \
test2006_77.C \
test2006_80.C \
test2006_81.C \
test2006_82.C \
test2006_83.C \
test2006_86.C \
test2006_87.C \
test2006_94.C \
test2006_95.C \
test2006_96.C \
test2006_97.C \
test2006_98.C \
test2006_99.C \
test2007_01.C \
test2007_02.C \
test2007_03.C \
test2007_04.C \
test2007_05.C \
test2007_06.C \
test2007_07.C \
test2007_08.C \
test2007_09.C \
test2007_102.C \
test2007_103.C \
test2007_10.C \
test2007_12.C \
test2007_130.C \
test2007_133.C \
test2007_137.C \
test2007_13.C \
test2007_140.C \
test2007_142.C \
test2007_147.C \
test2007_14.C \
test2007_150.C \
test2007_151.C \
test2007_152.C \
test2007_154.C \
test2007_157.C \
test2007_15.C \
test2007_160.C \
test2007_161.C \
test2007_162.C \
test2007_163.C \
test2007_165.C \
test2007_167.C \
test2007_169.C \
test2007_16.C \
test2007_172.C \
test2007_173.C \
test2007_17.C \
test2007_21.C \
test2007_23.C \
test2007_24.C \
test2007_25.C \
test2007_29.C \
test2007_41.C \
test2007_43.C \
test2007_46.C \
test2007_47.C \
test2007_48.C \
test2007_49.C \
test2007_50.C \
test2007_51.C \
test2007_52.C \
test2007_55.C \
test2007_57.C \
test2007_59.C \
test2007_62.C \
test2007_70.C \
test2007_72.C \
test2007_75.C \
test2007_77.C \
test2007_78.C \
test2007_81.C \
test2007_82.C \
test2007_83.C \
test2007_84.C \
test2007_85.C \
test2007_98.C \
test2008_01.C \
test2008_03.C \
test2008_04.C \
test2008_05.C \
test2008_06.C \
test2008_09.C \
test2008_10.C \
test2008_11.C \
test2008_12.C \
test2008_13.C \
test2008_14.C \
test2009_01.C \
test2009_02.C \
test2009_03.C \
test2009_04.C \
test2009_05.C \
test2009_06.C \
test2009_08.C \
test2009_09.C \
test2009_10.C \
test2009_12.C \
test2009_14.C \
test2009_15.C \
test2009_18.C \
test2009_21.C \
test2009_24.C \
test2009_27.C \
test2009_30.C \
test2009_39.C \
test2010_18.C \
test2010_25.C \
test2011_01.C \
test2011_02.C \
test2011_05.C \
test2011_07.C \
test2011_100.C \
test2011_111.C \
test2011_115.C \
test2011_117.C \
test2011_118.C \
test2011_119.C \
test2011_120.C \
test2011_19.C \
test2011_20.C \
test2011_21.C \
test2011_22.C \
test2011_23.C \
test2011_24.C \
test2011_25.C \
test2011_26.C \
test2011_27.C \
test2011_28.C \
test2011_29.C \
test2011_30.C \
test2011_31.C \
test2011_32.C \
test2011_33.C \
test2011_34.C \
test2011_35.C \
test2011_36.C \
test2011_37.C \
test2011_38.C \
test2011_40.C \
test2011_41.C \
test2011_42.C \
test2011_43.C \
test2011_44.C \
test2011_45.C \
test2011_46.C \
test2011_47.C \
test2011_48.C \
test2011_49.C \
test2011_50.C \
test2011_51.C \
test2011_52.C \
test2011_53.C \
test2011_54.C \
test2011_55.C \
test2011_56.C \
test2011_57.C \
test2011_58.C \
test2011_59.C \
test2011_60.C \
test2011_61.C \
test2011_62.C \
test2011_63.C \
test2011_64.C \
test2011_65.C \
test2011_67.C \
test2011_68.C \
test2011_69.C \
test2011_70.C \
test2011_71.C \
test2011_73.C \
test2011_74.C \
test2011_75.C \
test2011_78.C \
test2011_79.C \
test2011_80.C \
test2011_81.C \
test2011_82.C \
test2011_83.C \
test2011_84.C \
test2011_85.C \
test2011_86.C \
test2011_87.C \
test2011_88.C \
test2011_89.C \
test2011_90.C \
test2011_91.C \
test2011_92.C \
test2011_93.C \
test2011_94.C \
test2011_95.C \
test2011_96.C \
test2011_97.C \
test2011_98.C \
test_CplusplusMacro_Cpp.C \
test_int_lexemes.C \
test_int_lexemes_donot_pass.C \
test_simple_int.C \
test_wchars.C

CXX_TESTS_FAILING = \
test2004_160.C \
test2004_19.C \
test2004_30.C \
test2004_44.C \
test2004_76.C \
test2004_90.C \
test2005_120.C \
test2005_123.C \
test2005_134.C \
test2005_165.C \
test2005_174.C \
test2005_32.C \
test2005_33.C \
test2005_53.C \
test2005_57.C \
test2005_73.C \
test2005_74.C \
test2005_79.C \
test2005_81.C \
test2005_96.C \
test2005_97.C \
test2006_103.C \
test2006_126.C \
test2006_61.C \
test2006_74.C \
test2006_88.C \
test2006_91.C \
test2007_128.C \
test2007_28.C \
test2007_38.C \
test2007_53.C \
test2007_90.C \
test2009_19.C \
test2009_20.C \
test2009_28.C \
test2011_76.C 

CXX_TESTS_HANGING = \
	test2005_114.C

# DQ (6/1/2014): Failing test.
# test2005_163.C 
# test2011_77.C 


#FUSE_CXX_TESTS_REQUIRED_TO_PASS = ${CXX_TESTS_REQUIRED_TO_PASS:.C=.C-o}

#$(FUSE_CXX_TESTS_REQUIRED_TO_PASS): fuse $(CXX_TESTDIR)/$(@:.C=.C-o)
#	./fuse -c --edg:no_warnings $(ROSE_FLAGS) $(FUSE_FLAGS) -I$(top_srcdir)/tests/CompileTests/Cxx_tests $(CXX_TESTDIR)/$(@:.C-o=.C)


# command to run fuse ./fuse -fuse:"lc(cp)"
# runs constant propagation on all cxx_ tests
CXX_FUSE_TESTS = $(addprefix cxx_, $(addsuffix .passed, $(CXX_TESTS_REQUIRED_TO_PASS)))
$(CXX_FUSE_TESTS): cxx_%.passed: $(CXX_TESTDIR)/% $(TEST_EXIT_STATUS) $(FUSE)
	@$(RTH_RUN) CMD="./fuse -fuse:\"lc(cp)\" -I$(CXX_TESTDIR) -c $<" $(TEST_EXIT_STATUS) $@

.PHONY: check-cxx
check-cxx: $(CXX_FUSE_TESTS)

check-local: check-fuse check-cxx

clean-local:
	rm -rf *.dot *.png dbg/ *.passed *.failed
