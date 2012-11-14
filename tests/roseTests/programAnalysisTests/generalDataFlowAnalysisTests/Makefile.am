# This makefile is written in such a way that each section (delimited by "#" characters) describes a single category of
# tests. In order to disable that category of test all one needs to do is comment out that section.

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Forward declarations for "+="
MOSTLYCLEANFILES =
EXTRA_DIST =
CLEAN_TARGETS =
C_CHECK_TARGETS =
FORTRAN_CHECK_TARGETS =

LDADD = $(ROSE_LIBS) 

SAF_SRC_ROOT = $(top_srcdir)/src/midend/programAnalysis/genericDataflow/
INCLUDES = 					\
	$(ROSE_INCLUDES)			\
	-I$(srcdir)				\
        -I$(SAF_SRC_ROOT)			\
        -I$(SAF_SRC_ROOT)/analysis		\
        -I$(SAF_SRC_ROOT)/arrIndexLabeler	\
        -I$(SAF_SRC_ROOT)/cfgUtils		\
        -I$(SAF_SRC_ROOT)/lattice		\
        -I$(SAF_SRC_ROOT)/rwAccessLabeler	\
        -I$(SAF_SRC_ROOT)/simpleAnalyses	\
        -I$(SAF_SRC_ROOT)/state			\
        -I$(SAF_SRC_ROOT)/variables

bin_PROGRAMS = taintAnalysisTest constantPropagationTest taintedFlowAnalysisTest liveDeadVarAnalysisTest pointerAliasAnalysisTest
EXTRA_DIST += constantPropagation.h taintedFlowAnalysis.h pointerAliasAnalysis.h

taintAnalysisTest_SOURCES = taintAnalysisTest.C
liveDeadVarAnalysisTest_SOURCES = liveDeadVarAnalysisTest.C
constantPropagationTest_SOURCES = constantPropagation.C constantPropagationTest.C
taintedFlowAnalysisTest_SOURCES = taintedFlowAnalysis.C taintedFlowAnalysisTest.C
pointerAliasAnalysisTest_SOURCES = pointerAliasAnalysis.C pointerAliasAnalysisTest.C

CONST_PROP = ./constantPropagationTest
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status



###############################################################################################################################
### Taint Analysis tests ("cxxta" unique prefix)
###############################################################################################################################

TAINT_ANALYSIS_SPECIMENS = taint_input0.C taint_input1.C taint_input2.C taint_input3.C taint_input4.C taint_input5.C taint_input6.C
EXTRA_DIST += $(TAINT_ANALYSIS_SPECIMENS) $(addsuffix .ans, $(TAINT_ANALYSIS_SPECIMENS)) taintAnalysisTest.conf

TAINT_ANALYSIS_TESTS = $(addprefix cxxta_, $(addsuffix .passed, $(TAINT_ANALYSIS_SPECIMENS)))
$(TAINT_ANALYSIS_TESTS): cxxta_%.passed: $(srcdir)/% taintAnalysisTest taintAnalysisTest.conf
	@$(RTH_RUN) INPUT=$< ANSWER=$<.ans $(srcdir)/taintAnalysisTest.conf $@

C_CHECK_TARGETS += check-taint-analysis
.PHONY: check-taint-analysis
check-taint-analysis: $(TAINT_ANALYSIS_TESTS)

CLEAN_TARGETS += clean-taint-analysis
.PHONY: clean-taint-analysis
clean-taint-analysis:
	rm -f $(TAINT_ANALYSIS_TESTS) $(TAINT_ANALYSIS_TESTS:.passed=.failed)
	rm -f $(TAINT_ANALYSIS_TESTS:.passed=.out) $(TAINT_ANALYSIS_TESTS:.passed=.err)
	rm -f detail.html index.html summary.html



###############################################################################################################################
### C++ constant propagation tests on local specimens ("cxxcpls" unique prefix)
###############################################################################################################################

CXX_CONSTPROP_LOCAL_SPECIMENS = cp_test1.C
EXTRA_DIST += $(CXX_CONSTPROP_LOCAL_SPECIMENS)
CXX_CONSTPROP_LOCAL_TESTS = $(addprefix cxxcpls_, $(addsuffix .passed, $(CXX_CONSTPROP_LOCAL_SPECIMENS)))
$(CXX_CONSTPROP_LOCAL_TESTS): cxxcpls_%.passed: $(srcdir)/% $(TEST_EXIT_STATUS) $(CONST_PROP)
	@$(RTH_RUN) CMD="$(CONST_PROP) $(ROSE_FLAGS) -c $<" $(TEST_EXIT_STATUS) $@

C_CHECK_TARGETS += check-cxx-constprop-local
.PHONY: check-cxx-constprop-local
check-cxx-constprop-local: $(CXX_CONSTPROP_LOCAL_TESTS)

CLEAN_TARGETS += clean-cxx-constprop-local
.PHONY: clean-cxx-constprop-local
clean-cxx-constprop-local:
	rm -f $(CXX_CONSTPROP_LOCAL_TESTS) $(CXX_CONSTPROP_LOCAL_TESTS:.passed=.failed)
	rm -f $(CXX_CONSTPROP_LOCAL_TESTS:.passed=.out) $(CXX_CONSTPROP_LOCAL_TESTS:.passed=.err)
	rm -f $(patsubst cxxcpls_%.passed, %*.dot, $(CXX_CONSTPROP_LOCAL_TESTS))
	rm -f detail.html index.html summary.html



###############################################################################################################################
### C++ constant propagation tests on specimens from the CXX_TESTDIR directory ("cxxcp" unique prefix)
###############################################################################################################################

# This list comes from $(CXX_TESTDIR)/Makefile-pass.inc as of 2011-??-14 so we can remove failing tests.  Do not add these
# names to EXTRA_DIST since they are in some other directory.  Additional comments about the failing tests can be found
# scattered through out commit c8f4d3c of this file.
CXX_TESTDIR = $(top_srcdir)/tests/CompileTests/Cxx_tests
CXX_CONSTPROP_SPECIMENS =													 \
	X.C lexPhase2003_01.C math.C test2001_01.C test2001_02.C test2001_05.C test2001_06.C test2001_07.C test2001_08.C	 \
	test2001_09.C test2001_10.C test2001_11.C test2001_12.C test2001_13.C test2001_15.C test2001_16.C test2001_17.C		 \
	test2001_18.C test2001_19.C test2001_20.C test2001_21.C test2001_22.C test2001_23.C test2001_24.C test2001_25.C		 \
	test2001_27.C test2001_29.C test2002_01.C test2003_02.C test2003_03.C test2003_04.C test2003_05.C test2003_06.C		 \
	test2003_08.C test2003_09.C test2003_10.C test2003_11.C test2003_12.C test2003_13.C test2003_14.C test2003_16.C		 \
	test2003_17.C test2003_20.C test2003_21.C test2003_23.C test2003_25.C test2003_26.C test2003_27.C test2003_28.C		 \
	test2003_29.C test2003_30.C test2003_31.C test2003_37.C test2003_38.C test2004_01.C test2004_03.C test2004_04.C		 \
	test2004_05.C test2004_06.C test2004_07.C test2004_08.C test2004_09.C test2004_100.C test2004_101.C test2004_102.C	 \
	test2004_103.C test2004_104.C test2004_105.C test2004_106.C test2004_107.C test2004_108.C test2004_109.C test2004_110.C	 \
	test2004_111.C test2004_112.C test2004_113.C test2004_114.C test2004_115.C test2004_117.C test2004_118.C test2004_119.C	 \
	test2004_12.C test2004_121.C test2004_122.C test2004_123.C test2004_124.C test2004_125.C test2004_128.C test2004_13.C	 \
	test2004_131.C test2004_132.C test2004_133.C test2004_134.C test2004_135.C test2004_136.C test2004_137.C test2004_138.C	 \
	test2004_140.C test2004_144.C test2004_146.C test2004_147.C test2004_148.C test2004_150.C test2004_151.C test2004_152.C	 \
	test2004_153.C test2004_154.C test2004_155.C test2004_156.C test2004_158.C test2004_16.C test2004_160.C test2004_163.C	 \
	test2004_17.C test2004_19.C test2004_23.C test2004_24.C test2004_26.C test2004_27.C test2004_28.C test2004_29.C		 \
	test2004_30.C test2004_31.C test2004_32.C test2004_33.C test2004_34.C test2004_37.C test2004_38.C test2004_39.C		 \
	test2004_40.C test2004_42.C test2004_44.C test2004_49.C test2004_50.C test2004_51.C test2004_53.C test2004_54.C		 \
	test2004_56.C test2004_58.C test2004_59.C test2004_61.C test2004_62.C test2004_64.C test2004_65.C test2004_67.C		 \
	test2004_68.C test2004_69.C test2004_70.C test2004_71.C test2004_73.C test2004_74.C test2004_76.C test2004_78.C		 \
	test2004_79.C test2004_80.C test2004_81.C test2004_83.C test2004_84.C test2004_86.C test2004_87.C test2004_89.C		 \
	test2004_90.C test2004_91.C test2004_93.C test2004_94.C test2004_95.C test2004_96.C test2004_97.C test2004_98.C		 \
	test2004_99.C test2005_01.C test2005_02.C test2005_03.C test2005_05.C test2005_105.C test2005_107.C test2005_109.C	 \
	test2005_11.C test2005_110.C test2005_111.C test2005_113.C test2005_116.C test2005_117.C test2005_119.C test2005_120.C	 \
	test2005_121.C test2005_123.C test2005_124.C test2005_125.C test2005_126.C test2005_127.C test2005_13.C test2005_134.C	 \
	test2005_135.C test2005_14.C test2005_140.C test2005_144.C test2005_145.C test2005_148.C test2005_149.C test2005_15.C	 \
	test2005_150.C test2005_153.C test2005_154.C test2005_157.C test2005_159.C test2005_16.C test2005_160.C test2005_162.C	 \
	test2005_163.C test2005_165.C test2005_169.C test2005_170.C test2005_171.C test2005_172.C test2005_173.C test2005_174.C	 \
	test2005_175.C test2005_176.C test2005_179.C test2005_180.C test2005_182.C test2005_183.C test2005_184.C test2005_185.C	 \
	test2005_189.C test2005_190.C test2005_191.C test2005_192.C test2005_195.C test2005_196.C test2005_197.C test2005_198.C	 \
	test2005_20.C test2005_203.C test2005_206.C test2005_207.C test2005_208.C test2005_209.C test2005_21.C test2005_22.C	 \
	test2005_23.C test2005_24.C test2005_25.C test2005_26.C test2005_30.C test2005_31.C test2005_32.C test2005_33.C		 \
	test2005_36.C test2005_40.C test2005_41.C test2005_45.C test2005_47.C test2005_48.C test2005_49.C test2005_50.C		 \
	test2005_51.C test2005_52.C test2005_53.C test2005_55.C test2005_57.C test2005_58.C test2005_59.C test2005_60.C		 \
	test2005_61.C test2005_62.C test2005_64.C test2005_65.C test2005_66.C test2005_67.C test2005_68.C test2005_70.C		 \
	test2005_72.C test2005_73.C test2005_74.C test2005_78.C test2005_79.C test2005_80.C test2005_81.C test2005_83.C		 \
	test2005_84.C test2005_86.C test2005_87.C test2005_88.C test2005_89.C test2005_90.C test2005_91.C test2005_92.C		 \
	test2005_94.C test2005_95.C test2005_96.C test2005_97.C test2005_99.C test2006_01.C test2006_02.C test2006_03.C		 \
	test2006_06.C test2006_07.C test2006_08.C test2006_10.C test2006_101.C test2006_103.C test2006_104.C test2006_105.C	 \
	test2006_106.C test2006_107.C test2006_108.C test2006_109.C test2006_111.C test2006_113.C test2006_114.C test2006_116.C	 \
	test2006_117.C test2006_118.C test2006_12.C test2006_120.C test2006_121.C test2006_122.C test2006_123.C test2006_126.C	 \
	test2006_129.C test2006_13.C test2006_14.C test2006_140.C test2006_142.C test2006_143.C test2006_144.C test2006_145.C	 \
	test2006_146.C test2006_149.C test2006_150.C test2006_151.C test2006_152.C test2006_153.C test2006_155.C test2006_156.C	 \
	test2006_157.C test2006_158.C test2006_16.C test2006_161.C test2006_162.C test2006_163.C test2006_164.C test2006_165.C	 \
	test2006_17.C test2006_177.C test2006_178.C test2006_179.C test2006_18.C test2006_181.C test2006_183.C test2006_19.C	 \
	test2006_22.C test2006_23.C test2006_25.C test2006_26.C test2006_34.C test2006_37.C test2006_38.C test2006_41.C		 \
	test2006_42.C test2006_44.C test2006_46.C test2006_52.C test2006_53.C test2006_55.C test2006_56.C test2006_57.C		 \
	test2006_59.C test2006_61.C test2006_62.C test2006_63.C test2006_66.C test2006_67.C test2006_68.C test2006_69.C		 \
	test2006_70.C test2006_71.C test2006_72.C test2006_73.C test2006_74.C test2006_75.C test2006_76.C test2006_77.C		 \
	test2006_80.C test2006_81.C test2006_82.C test2006_83.C test2006_86.C test2006_87.C test2006_88.C test2006_91.C		 \
	test2006_94.C test2006_95.C test2006_96.C test2006_97.C test2006_98.C test2006_99.C test2007_01.C test2007_02.C		 \
	test2007_03.C test2007_04.C test2007_05.C test2007_06.C test2007_07.C test2007_08.C test2007_09.C test2007_10.C		 \
	test2007_102.C test2007_103.C test2007_12.C test2007_128.C test2007_13.C test2007_130.C test2007_133.C test2007_137.C	 \
	test2007_14.C test2007_140.C test2007_142.C test2007_147.C test2007_15.C test2007_150.C test2007_151.C test2007_152.C	 \
	test2007_154.C test2007_157.C test2007_16.C test2007_160.C test2007_161.C test2007_162.C test2007_163.C test2007_165.C	 \
	test2007_167.C test2007_169.C test2007_17.C test2007_172.C test2007_173.C test2007_21.C test2007_23.C test2007_24.C	 \
	test2007_25.C test2007_28.C test2007_29.C test2007_38.C test2007_41.C test2007_43.C test2007_46.C test2007_47.C		 \
	test2007_48.C test2007_49.C test2007_50.C test2007_51.C test2007_52.C test2007_53.C test2007_55.C test2007_57.C		 \
	test2007_59.C test2007_62.C test2007_70.C test2007_72.C test2007_75.C test2007_77.C test2007_78.C test2007_81.C		 \
	test2007_82.C test2007_83.C test2007_84.C test2007_85.C test2007_90.C test2007_98.C test2008_01.C test2008_03.C		 \
	test2008_04.C test2008_05.C test2008_06.C test2008_09.C test2008_10.C test2008_11.C test2008_12.C test2008_13.C		 \
	test2008_14.C test2009_01.C test2009_02.C test2009_03.C test2009_04.C test2009_05.C test2009_06.C test2009_08.C		 \
	test2009_09.C test2009_10.C test2009_12.C test2009_14.C test2009_15.C test2009_18.C test2009_19.C test2009_20.C		 \
	test2009_21.C test2009_24.C test2009_27.C test2009_28.C test2009_30.C test2009_31.C test2009_39.C test2010_18.C		 \
	test2010_25.C test2011_01.C test2011_02.C test2011_05.C test2011_07.C test2011_100.C test2011_111.C test2011_115.C	 \
	test2011_117.C test2011_118.C test2011_119.C test2011_120.C test2011_19.C test2011_20.C test2011_21.C test2011_22.C	 \
	test2011_23.C test2011_24.C test2011_25.C test2011_26.C test2011_27.C test2011_28.C test2011_29.C test2011_30.C		 \
	test2011_31.C test2011_32.C test2011_33.C test2011_34.C test2011_35.C test2011_36.C test2011_37.C test2011_38.C		 \
	test2011_40.C test2011_41.C test2011_42.C test2011_43.C test2011_44.C test2011_45.C test2011_46.C test2011_47.C		 \
	test2011_48.C test2011_49.C test2011_50.C test2011_51.C test2011_52.C test2011_53.C test2011_54.C test2011_55.C		 \
	test2011_56.C test2011_57.C test2011_58.C test2011_59.C test2011_60.C test2011_61.C test2011_62.C test2011_63.C		 \
	test2011_64.C test2011_65.C test2011_67.C test2011_68.C test2011_69.C test2011_70.C test2011_71.C test2011_73.C		 \
	test2011_74.C test2011_75.C test2011_76.C test2011_77.C test2011_78.C test2011_79.C test2011_80.C test2011_81.C		 \
	test2011_82.C test2011_83.C test2011_84.C test2011_85.C test2011_86.C test2011_87.C test2011_88.C test2011_89.C		 \
	test2011_90.C test2011_91.C test2011_92.C test2011_93.C test2011_94.C test2011_95.C test2011_96.C test2011_97.C		 \
	test2011_98.C test_CplusplusMacro_Cpp.C test_int_lexemes.C test_int_lexemes_donot_pass.C test_simple_int.C test_wchars.C
CXX_CONSTPROP_SPECIMENS_FAILING =											\
	inputBug317.C inputBug327.C inputForLoopLocator.C test2001_04.C test2001_14.C test2001_28.C test2003_01.C	\
	test2003_15.C test2003_18.C test2003_32.C test2003_35.C test2003_36.C test2004_02.C test2004_10.C test2004_11.C	\
	test2004_120.C test2004_127.C test2004_129.C test2004_130.C test2004_139.C test2004_14.C test2004_141.C		\
	test2004_142.C test2004_143.C test2004_145.C test2004_149.C test2004_15.C test2004_157.C test2004_159.C		\
	test2004_161.C test2004_162.C test2004_164.C test2004_18.C test2004_35.C test2004_36.C test2004_43.C		\
	test2004_45.C test2004_47.C test2004_48.C test2004_52.C test2004_55.C test2004_60.C test2004_63.C test2004_75.C	\
	test2004_77.C test2004_85.C test2004_88.C test2005_06.C test2005_07.C test2005_08.C test2005_09.C test2005_10.C	\
	test2005_100.C test2005_101.C test2005_102.C test2005_103.C test2005_104.C test2005_106.C test2005_108.C	\
	test2005_112.C test2005_118.C test2005_12.C test2005_122.C test2005_128.C test2005_129.C test2005_130.C		\
	test2005_131.C test2005_132.C test2005_133.C test2005_136.C test2005_137.C test2005_138.C test2005_139.C	\
	test2005_141.C test2005_142.C test2005_143.C test2005_146.C test2005_152.C test2005_155.C test2005_156.C	\
	test2005_158.C test2005_161.C test2005_164.C test2005_166.C test2005_167.C test2005_168.C test2005_17.C		\
	test2005_177.C test2005_178.C test2005_181.C test2005_187.C test2005_19.C test2005_193.C test2005_194.C		\
	test2005_199.C test2005_202.C test2005_27.C test2005_28.C test2005_29.C test2005_34.C test2005_35.C		\
	test2005_37.C test2005_38.C test2005_39.C test2005_42.C test2005_43.C test2005_44.C test2005_46.C test2005_63.C	\
	test2005_71.C test2005_75a.C test2005_75b.C test2005_76.C test2005_77.C test2005_82.C test2005_93.C		\
	test2005_98.C test2006_04.C test2006_05.C test2006_102.C test2006_112.C test2006_115.C test2006_119.C		\
	test2006_124.C test2006_125.C test2006_141.C test2006_154.C test2006_159.C test2006_160.C test2006_166.C	\
	test2006_185.C test2006_189.C test2006_20.C test2006_28.C test2006_39.C test2006_40.C test2006_47.C		\
	test2006_50.C test2006_54.C test2006_58.C test2006_78.C test2006_79.C test2006_84.C test2006_85.C test2006_89.C	\
	test2006_90.C test2006_92.C test2007_11.C test2007_135.C test2007_141.C test2007_155.C test2007_158.C		\
	test2007_159.C test2007_164.C test2007_166.C test2007_170.C test2007_171.C test2007_26.C test2007_40.C		\
	test2007_44.C test2007_94.C test2008_08.C test2008_16.C test2009_11.C test2009_37.C test2009_40.C test2009_41.C	\
	test2009_42.C test2010_03.C test2010_04.C test2010_05.C test2010_06.C test2010_19.C test2010_24.C test2011_06.C	\
	test2011_101.C test2011_110.C test2011_121.C test2011_133.C test2011_39.C test2011_72.C test2011_99.C
CXX_CONSTPROP_SPECIMENS_HANGING = \
	test2005_114.C

CXX_CONSTPROP_TESTS = $(addprefix cxxcp_, $(addsuffix .passed, $(CXX_CONSTPROP_SPECIMENS)))
$(CXX_CONSTPROP_TESTS): cxxcp_%.passed: $(CXX_TESTDIR)/% $(TEST_EXIT_STATUS) $(CONST_PROP)
	@$(RTH_RUN) CMD="$(CONST_PROP) $(ROSE_FLAGS) -I$(CXX_TESTDIR) -c $<" $(TEST_EXIT_STATUS) $@

C_CHECK_TARGETS += check-cxx-constprop
.PHONY: check-cxx-constprop
check-cxx-constprop: $(CXX_CONSTPROP_TESTS)

CLEAN_TARGETS += clean-cxx-constprop
.PHONY: clean-cxx-constprop
clean-cxx-constprop:
	rm -f $(CXX_CONSTPROP_TESTS) $(CXX_CONSTPROP_TESTS:.passed=.failed)
	rm -f $(CXX_CONSTPROP_TESTS:.passed=.out) $(CXX_CONSTPROP_TESTS:.passed=.err)
	rm -f $(patsubst cxxcp_%.passed, %*.dot, $(CXX_CONSTPROP_TESTS))
	rm -f detail.html index.html summary.html
	rm -f exception_L4R__L5R_cfg.dot exception_L6R__L7R_cfg.dot test2004_40.h_fooAfter___Fb_v_Gb__Fe___L2R_cfg.dot
	rm -f test2004_40.h_fooBefore___Fb_v_Gb__Fe___L2R_cfg.dot test2005_81.h_L1R_cfg.dot
	rm -f test2005_81.h_L2R_cfg.dot test2005_88.h_L2R_cfg.dot test2005_88.h_L3R_cfg.dot



###############################################################################################################################
# C constant propagation tests for specimens in the $(CXX_TESTDIR) directory ("ccp" unique prefix).
###############################################################################################################################

# These names are in the $(CXX_TESTDIR) directory, so do not add them to $(EXTRA_DIST).  Additional comments about the failing
# tests can be found scattered through out commit c8f4d3c of this file.
C_CONSTPROP_SPECIMENS = 												 \
	X.C test2001_02.C test2001_05.C test2001_07.C test2001_08.C test2001_09.C test2001_17.C test2001_23.C		 \
	test2001_24.C test2001_25.C test2002_01.C test2003_04.C test2003_05.C test2003_06.C test2003_10.C test2003_11.C	 \
	test2003_13.C test2003_14.C test2003_16.C test2003_20.C test2003_21.C test2003_23.C test2003_24.C test2003_25.C	 \
	test2003_26.C test2003_29.C test2003_30.C test2003_31.C test2004_03.C test2004_04.C test2004_05.C test2004_102.C \
	test2004_105.C test2004_107.C test2004_108.C test2004_114.C test2004_118.C test2004_13.C test2004_131.C		 \
	test2004_152.C test2004_153.C test2004_154.C test2004_163.C test2004_19.C test2004_23.C test2004_28.C		 \
	test2004_29.C test2004_49.C test2004_53.C test2004_61.C test2004_74.C test2004_94.C test2004_96.C test2005_111.C \
	test2005_116.C test2005_125.C test2005_157.C test2005_159.C test2005_16.C test2005_165.C test2005_169.C		 \
	test2005_170.C test2005_176.C test2005_180.C test2005_182.C test2005_184.C test2005_191.C test2005_198.C	 \
	test2005_20.C test2005_209.C test2005_21.C test2005_22.C test2005_23.C test2005_26.C test2005_48.C test2005_62.C \
	test2005_84.C test2005_92.C test2005_95.C test2006_07.C test2006_10.C test2006_103.C test2006_104.C		 \
	test2006_105.C test2006_106.C test2006_107.C test2006_108.C test2006_109.C test2006_111.C test2006_113.C	 \
	test2006_116.C test2006_12.C test2006_137.C test2006_14.C test2006_140.C test2006_142.C test2006_145.C		 \
	test2006_146.C test2006_149.C test2006_150.C test2006_151.C test2006_152.C test2006_153.C test2006_154.C	 \
	test2006_16.C test2006_181.C test2006_23.C test2006_34.C test2006_52.C test2006_53.C test2006_55.C test2006_56.C \
	test2006_57.C test2006_62.C test2006_69.C test2006_75.C test2006_76.C test2006_82.C test2006_83.C test2006_88.C	 \
	test2006_91.C test2006_94.C test2006_97.C test2006_99.C test_CplusplusMacro_C.C test_int_lexemes.C		 \
	test_int_lexemes_donot_pass.C test_simple_int.C test_wchars.C
C_CONSTPROP_SPECIMENS_FAILING =											     \
	lastTest.C test2001_14.C test2003_15.C test2003_18.C test2003_32.C test2003_35.C test2003_36.C test2004_14.C \
	test2004_60.C test2005_100.C test2005_164.C test2005_187.C test2005_202.C test2006_40.C test2006_50.C

# The EXAMPLE_C_TESTCODES_VERIFIED_TO_PASS have ".C" extentions, not ".c", so we need to first copy each one to a new name.
C_CONSTPROP_TESTS = $(addprefix ccp_, $(addsuffix .passed, $(C_CONSTPROP_SPECIMENS)))
$(C_CONSTPROP_TESTS): ccp_%.passed: $(CXX_TESTDIR)/% $(TEST_EXIT_STATUS) $(CONST_PROP)
	cp $< ${@:.C.passed=.c}
	@$(RTH_RUN) CMD="$(CONST_PROP) $(ROSE_FLAGS) -I$(CXX_TESTDIR) -c ${@:.C.passed=.c}" $(TEST_EXIT_STATUS) $@

C_CHECK_TARGETS += check-c-constprop
.PHONY: check-c-constprop
check-c-constprop: $(C_CONSTPROP_TESTS)

CLEAN_TARGETS += clean-c-constprop
.PHONY: clean-c-constprop
clean-c-constprop:
	rm -f $(C_CONSTPROP_TESTS) $(C_CONSTPROP_TESTS:.passed=.failed)
	rm -f $(C_CONSTPROP_TESTS:.passed=.out) $(C_CONSTPROP_TESTS:.passed=.err)
	rm -f $(C_CONSTPROP_TESTS:.C.passed=.c) $(C_CONSTPROP_TESTS:.C.passed=.c*.dot)
	rm -f detail.html index.html summary.html



###############################################################################################################################
### C99 constant propagation tests for specimens in the $(C99_TESTDIR) directory ("c99cp" unique prefix).
###############################################################################################################################

# These names are in the $(C99_TESTDIR) directory, so do not add them to $(EXTRA_DIST).  Additional comments about the failing
# tests can be found scattered through out commit c8f4d3c of this file.
C99_TESTDIR = $(top_srcdir)/tests/CompileTests/C99_tests
C99_CONSTPROP_SPECIMENS = bool.c complex_01.c complex_03.c constants.c test2005_186.c test2006_127.c test2006_143.c 
C99_CONSTPROP_SPECIMENS_FAILING = test2008_01.c

C99_CONSTPROP_TESTS = $(addprefix c99cp_, $(addsuffix .passed, $(C99_CONSTPROP_SPECIMENS)))
$(C99_CONSTPROP_TESTS): c99cp_%.passed: $(C99_TESTDIR)/% $(TEST_EXIT_STATUS) $(CONST_PROP)
	@$(RTH_RUN) CMD="$(CONST_PROP) $(ROSE_FLAGS) -I$(C99_TESTDIR) -c $<" $(TEST_EXIT_STATUS) $@

C_CHECK_TARGETS += check-c99-constprop
.PHONY: check-c99-constprop
check-c99-constprop: $(C99_CONSTPROP_TESTS)

CLEAN_TARGETS += clean-c99-constprop
.PHONY: clean-c99-constprop
clean-c99-constprop:
	rm -f $(C99_CONSTPROP_TESTS) $(C99_CONSTPROP_TESTS:.passed=.failed)
	rm -f $(C99_CONSTPROP_TESTS:.passed=.out) $(C99_CONSTPROP_TESTS:.passed=.err)
	rm -f $(patsubst c99cp_%.passed, %*.dot, $(C99_CONSTPROP_TESTS))
	rm -f detail.html index.html summary.html



###############################################################################################################################
### FORTRAN 77 constant propagation tests for specimens in the $(F77_TESTDIR) directory ("f77cp" unique prefix).
###############################################################################################################################

# These names are in the $(F77_TESTDIR) directory, so do not add them to $(EXTRA_DIST).  Additional comments about the failing
# tests can be found scattered through out commit c8f4d3c of this file.
F77_TESTDIR = $(top_srcdir)/tests/CompileTests/Fortran_tests
F77_CONSTPROP_SPECIMENS = test2007_222.f 
F77_CONSTPROP_SPECIMENS_FAILING =										 \
	lastTest.f test2007_129.f test2007_132.f test2007_148.f test2007_150.f test2007_151.f test2007_153.f	 \
	test2007_154.f test2007_155.f test2007_156.f test2007_162.f test2007_163.f test2007_164.f test2007_175.f \
	test2007_198.f test2007_200.f test2007_201.f test2007_202.f test2007_204.f test2007_205.f test2007_206.f \
	test2007_207.f test2007_208.f test2007_211.f test2007_212.f test2007_213.f test2007_214.f test2007_215.f \
	test2007_216.f test2007_217.f test2007_218.f test2007_220.f test2007_221.f test2007_223.f test2007_224.f \
	test2007_226.f test2007_228.f test2007_229.f test2007_230.f test2007_231.f test2007_232.f test2007_234.f \
	test2007_242.f test2007_243.f test2007_245.f test2007_246.f test2007_247.f test2007_251.f test2007_253.f \
	test2007_259.f test2007_260.f test2007_261.f
F77_CONSTPROP_SPECIMENS_FAILING_CFG =										 \
	lastCode.f test2007_124.f test2007_125.f test2007_128.f test2007_145.f test2007_157.f test2007_183.f	 \
	test2007_184.f test2007_185.f test2007_186.f test2007_187.f test2007_188.f test2007_189.f test2007_191.f \
	test2007_192.f test2007_193.f test2007_194.f test2007_195.f test2007_196.f test2007_197.f test2007_209.f \
	test2007_227.f test2007_256.f test2007_258.f

F77_CONSTPROP_TESTS = $(addprefix f77cp_, $(addsuffix .passed, $(F77_CONSTPROP_SPECIMENS)))
$(F77_CONSTPROP_TESTS): f77cp_%.passed: $(F77_TESTDIR)/% $(TEST_EXIT_STATUS) $(CONST_PROP)
	@$(RTH_RUN) CMD="$(CONST_PROP) $(ROSE_FLAGS) -rose:f77 -c $<" $(TEST_EXIT_STATUS) $@

FORTRAN_CHECK_TARGETS += check-f77-constprop
.PHONY: check-f77-constprop
check-f77-constprop: $(F77_CONSTPROP_TESTS)

CLEAN_TARGETS += clean-f77-constprop
.PHONY: clean-f77-constprop
clean-f77-constprop:
	rm -f $(F77_CONSTPROP_TESTS) $(F77_CONSTPROP_TESTS:.passed=.failed)
	rm -f $(F77_CONSTPROP_TESTS:.passed=.out) $(F77_CONSTPROP_TESTS:.passed=.err)
	rm -f $(patsubst f77cp_%.passed, %*.dot, $(F77_CONSTPROP_TESTS))
	rm -f detail.html index.html summary.html



###############################################################################################################################
### Fortran 90 constant propagation tests for specimens in the $(F90_TESTDIR) directory ("f90cp" unique prefix)
###############################################################################################################################

# These names are in the $(F90_TESTDIR) directory, so do not add them to $(EXTRA_DIST).  Additional comments about the failing
# tests can be found scattered through out commit c8f4d3c of this file.
F90_TESTDIR = $(top_srcdir)/tests/CompileTests/Fortran_tests
F90_CONSTPROP_SPECIMENS =
F90_CONSTPROP_SPECIMENS_FAILING =											\
	canonicalExample.f90 test2007_01.f90 test2007_02.f90 test2007_03.f90 test2007_04.f90 test2007_05.f90		\
	test2007_06.f90 test2007_07.f90 test2007_08.f90 test2007_09.f90 test2007_10.f90 test2007_100.f90		\
	test2007_102.f90 test2007_103.f90 test2007_104.f90 test2007_105.f90 test2007_106.f90 test2007_11.f90		\
	test2007_110.f90 test2007_117.f90 test2007_118.f90 test2007_119.f90 test2007_12.f90 test2007_121.f90		\
	test2007_122.f90 test2007_126.f90 test2007_13.f90 test2007_135.f90 test2007_136.f90 test2007_137.f90		\
	test2007_138.f90 test2007_139.f90 test2007_14.f90 test2007_140.f90 test2007_141.f90 test2007_143.f90		\
	test2007_149.f90 test2007_15.f90 test2007_158.f90 test2007_159.f90 test2007_160.f90 test2007_161.f90		\
	test2007_169.f90 test2007_17.f90 test2007_178.f90 test2007_179.f90 test2007_181.f90 test2007_182.f90		\
	test2007_190.f90 test2007_20.f90 test2007_21.f90 test2007_22.f90 test2007_23.f90 test2007_236.f90		\
	test2007_238.f90 test2007_239.f90 test2007_24.f90 test2007_244.f90 test2007_248.f90 test2007_249.f90		\
	test2007_25.f90 test2007_250.f90 test2007_26.f90 test2007_262.f90 test2007_27.f90 test2007_28.f90		\
	test2007_29.f90 test2007_32.f90 test2007_33.f90 test2007_34.f90 test2007_36.f90 test2007_38.f90 test2007_39.f90	\
	test2007_40.f90 test2007_48.f90 test2007_49.f90 test2007_50.f90 test2007_51.f90 test2007_52.f90 test2007_53.f90	\
	test2007_54.f90 test2007_55.f90 test2007_56.f90 test2007_57.f90 test2007_58.f90 test2007_59.f90 test2007_60.f90	\
	test2007_62.f90 test2007_63.f90 test2007_64.f90 test2007_65.f90 test2007_66.f90 test2007_67.f90 test2007_68.f90	\
	test2007_69.f90 test2007_71.f90 test2007_72.f90 test2007_73.f90 test2007_74.f90 test2007_75.f90 test2007_76.f90	\
	test2007_77.f90 test2007_78.f90 test2007_79.f90 test2007_80.f90 test2007_89.f90 test2007_90.f90 test2007_91.f90	\
	test2007_92.f90 test2007_93.f90 test2007_95.f90 test2007_96.f90 test2007_97.f90
F90_CONSTPROP_SPECIMENS_FAILING_CFG =									   \
	lastTest.f90 test2007_101.f90 test2007_109.f90 test2007_111.f90 test2007_127.f90 test2007_16.f90   \
	test2007_168.f90 test2007_19.f90 test2007_219.f90 test2007_263.f90 test2007_61.f90 test2007_94.f90 \
	test2007_99.f90

F90_CONSTPROP_TESTS = $(addprefix f90cp_, $(addsuffix .passed, $(F90_CONSTPROP_SPECIMENS)))
$(F90_CONSTPROP_TESTS): f90cp_%.passed: $(F90_TESTDIR)/% $(TEST_EXIT_STATUS) $(CONST_PROP)
	@$(RTH_RUN) CMD="$(CONST_PROP) $(ROSE_FLAGS) -rose:f90 -c $<" $(TEST_EXIT_STATUS) $@

FORTRAN_CHECK_TARGETS += check-f90-constprop
.PHONY: check-f90-constprop
check-f90-constprop: $(F90_CONSTPROP_TESTS)

CLEAN_TARGETS += clean-f90-constprop
.PHONY: clean-f90-constprop
clean-f90-constprop:
	rm -f $(F90_CONSTPROP_TESTS) $(F90_CONSTPROP_TESTS:.passed=.failed)
	rm -f $(F90_CONSTPROP_TESTS:.passed=.out) $(F90_CONSTPROP_TESTS:.passed=.err)
	rm -f $(patsubst f90cp_%.passed, %*.dot, $(F90_CONSTPROP_TESTS))
	rm -f detail.html index.html summary.html



###############################################################################################################################
### Fortran 2003 constant propagation tests for specimens in the $(F03_TESTDIR) directory ("f03cp" unique prefix)
###############################################################################################################################

# These names are in the $(F03_TESTDIR) directory, so do not add them to $(EXTRA_DIST).  Additional comments about the failing
# tests can be found scattered through out commit c8f4d3c of this file.
F03_TESTDIR = $(top_srcdir)/tests/CompileTests/Fortran_tests
F03_CONSTPROP_SPECIMENS = test2007_37.f03
F03_CONSTPROP_SPECIMENS_FAILING =											 \
	test2007_112.f03 test2007_113.f03 test2007_114.f03 test2007_120.f03 test2007_123.f03 test2007_134.f03		 \
	test2007_174.f03 test2007_31.f03 test2007_41.f03 test2007_42.f03 test2007_44.f03 test2007_47.f03 test2007_98.f03
F03_CONSTPROP_SPECIMENS_FAILING_CFG =									  \
	lastTest.f03 test2007_115.f03 test2007_116.f03 test2007_130.f03 test2007_133.f03 test2007_176.f03 \
	test2007_30.f03 test2007_35.f03 test2007_43.f03 test2007_45.f03

F03_CONSTPROP_TESTS = $(addprefix f03cp_, $(addsuffix .passed, $(F03_CONSTPROP_SPECIMENS)))
$(F03_CONSTPROP_TESTS): f03cp_%.passed: $(F03_TESTDIR)/% $(TEST_EXIT_STATUS) $(CONST_PROP)
	@$(RTH_RUN) CMD="$(CONST_PROP) $(ROSE_FLAGS) -rose:skip_syntax_check -rose:skipfinalCompileStep -rose:f2003 -c $<" \
		$(TEST_EXIT_STATUS) $@

FORTRAN_CHECK_TARGETS += check-f03-constprop
.PHONY: check-f03-constprop
check-f03-constprop: $(F03_CONSTPROP_TESTS)

CLEAN_TARGETS += clean-f03-constprop
.PHONY: clean-f03-constprop
clean-f03-constprop:
	rm -f $(F03_CONSTPROP_TESTS) $(F03_CONSTPROP_TESTS:.passed=.failed)
	rm -f $(F03_CONSTPROP_TESTS:.passed=.out) $(F03_CONSTPROP_TESTS:.passed=.err)
	rm -f $(patsubst f03cp_%.passed, %*.dot, $(F03_CONSTPROP_TESTS))
	rm -f $(patsubst f03cp_%.f03.passed, %*.rmod, $(F03_CONSTPROP_TESTS))
	rm -f detail.html index.html summary.html



###############################################################################################################################
### C++ liveness analysis tests for local specimens ("cxxla" unique prefix)
###############################################################################################################################

CXX_LIVENESS_SPECIMENS = test1.C test4.C test5.C test6.C 
EXTRA_DIST += $(CXX_LIVENESS_SPECIMENS)

CXX_LIVENESS_TESTS = $(addprefix cxxla_, $(addsuffix .passed, $(CXX_LIVENESS_SPECIMENS)))
$(CXX_LIVENESS_TESTS): cxxla_%.passed: $(srcdir)/% $(TEST_EXIT_STATUS) liveDeadVarAnalysisTest
	@$(RTH_RUN) CMD="./liveDeadVarAnalysisTest $(ROSE_FLAGS) -c $<" $(TEST_EXIT_STATUS) $@

C_CHECK_TARGETS += check-cxx-liveness
.PHONY: check-cxx-liveness
check-cxx-liveness: $(CXX_LIVENESS_TESTS)

CLEAN_TARGETS += clean-cxx-liveness
.PHONY: clean-cxx-liveness
clean-cxx-liveness:
	rm -f $(CXX_LIVENESS_TESTS) $(CXX_LIVENESS_TESTS:.passed=.failed)
	rm -f $(CXX_LIVENESS_TESTS:.passed=.out) $(CXX_LIVENESS_TESTS:.passed=.err)
	rm -f $(patsubst cxxla_%.passed, %*.dot, $(CXX_LIVENESS_TESTS))
	rm -f detail.html index.html summary.html



###############################################################################################################################
### C++ pointer alias analysis tests for local specimens ("cxxpa" unique prefix)
###############################################################################################################################

CXX_POINTER_ALIAS_SPECIMENS = test_ptr1.C test_ptr2.C	test_ptr3.C test_ptr4.C test_ptr5.C
EXTRA_DIST += $(CXX_POINTER_ALIAS_SPECIMENS)

CXX_POINTER_ALIAS_TESTS = $(addprefix cxxpa_, $(addsuffix .passed, $(CXX_POINTER_ALIAS_SPECIMENS)))
$(CXX_POINTER_ALIAS_TESTS): cxxpa_%.passed: $(srcdir)/% $(TEST_EXIT_STATUS) ./pointerAliasAnalysisTest
	@$(RTH_RUN) CMD="./pointerAliasAnalysisTest $(ROSE_FLAGS) -c $<" $(TEST_EXIT_STATUS) $@

C_CHECK_TARGETS += check-cxx-pointer-alias
.PHONY: check-cxx-pointer-alias
check-cxx-pointer-alias: $(CXX_POINTER_ALIAS_TESTS)

CLEAN_TARGETS += clean-cxx-pointer-alias
.PHONY: clean-cxx-pointer-alias
clean-cxx-pointer-alias:
	rm -f $(CXX_POINTER_ALIAS_TESTS) $(CXX_POINTER_ALIAS_TESTS:.passed=.failed)
	rm -f $(CXX_POINTER_ALIAS_TESTS:.passed=.out) $(CXX_POINTER_ALIAS_TESTS:.passed=.err)
	rm -f $(patsubst cxxpa_%.passed, %*.dot, $(CXX_POINTER_ALIAS_TESTS))
	rm -f detail.html index.html summary.html



###############################################################################################################################
### Automake check and clean rules
###############################################################################################################################

# These two are defined based on what languages ROSE was compiled to support.
.PHONY: check-c-languages check-fortran-languages

if !ROSE_BUILD_OS_IS_OSX
# Note: C++ language support implies C, C++, and C99 support.
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
check-c-languages: $(C_CHECK_TARGETS)
else
check-c-languages:
endif
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
check-fortran-languages: $(FORTRAN_CHECK_TARGETS)
else
check-fortran-languages:
endif
else
check-c-languages:
	@echo "Disabled on OS X. DQ (9/30/2011): OSX testing is problematic, too many fail (so look into this later)." >&2
check-fortran-languages:
	@echo "Disabled on OS X. DQ (9/30/2011): OSX testing is problematic, too many fail (so look into this later)." >&2
endif


check-local: check-c-languages check-fortran-languages

# Clean only files that are created by the build system (i.e., no wild cards, because that tends to delete files that
# developers are working with).  Also, the MOSTLYCLEANFILES list gets too long for the shell, so we use phony targets to
# clean each test individually.
clean-local: $(CLEAN_TARGETS)
	rm -rf dbg_imgs
	rm -f $(MOSTLYCLEANFILES)
