include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
MOSTLYCLEANFILES =
EXTRA_DIST =

SUBDIRS = 					\
	testCallGraphAnalysis			\
	defUseAnalysisTests			\
	variableLivenessTests			\
	variableRenamingTests			\
	generalDataFlowAnalysisTests		\
	systemDependenceGraphTests		\
	typeTraitTests

# DQ (4/4/2017): Note that Boost 1.62 version is problematic and Boost suggests using an older version.
# There is nothing that ROSE can do about this except skip some tests that require problem features of Boost 1.62.
if !ROSE_USING_BOOST_VERSION_1_62
SUBDIRS += staticInterproceduralSlicingTests   \
        ssa_UnfilteredCfg_Test                 \
	staticSingleAssignmentTests
endif

# ROSE Test Harness scripts
#CHECK_ANSWER = $(top_srcdir)/scripts/test_against_answer
CHECK_ANSWER = $(top_srcdir)/scripts/test_with_answer
CHECK_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status


AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/programAnalysis -I$(top_srcdir)/src/midend/programAnalysis/pointerAnal -I${top_srcdir}/src/midend/programAnalysis/CFG -I${top_srcdir}/src/midend/programAnalysis/bitvectorDataflow -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/midend/programAnalysis/ 
#-I$(top_srcdir)/src/midend/programAnalysis/ptrTrace/GAI/ICFG/

AM_LDFLAGS = $(ROSE_RPATHS)


#-------------------------------------------------------------------------------------------------------------------------------
# Executables

noinst_PROGRAMS += CFGTest
# DQ (2/10/2010): ROSE currently fails to generate code for this file (it does however compile for analysis only).
# CFGTest_SOURCES = CFGTest.C
CFGTest_SOURCES =
if !ROSE_USING_ROSE_CODE_GENERATION
     CFGTest_SOURCES += CFGTest.C
endif
CFGTest_LDADD = $(ROSE_LIBS)


noinst_PROGRAMS += DataFlowTest
DataFlowTest_SOURCES = DataFlowTest.C
DataFlowTest_LDADD = $(ROSE_LIBS)


noinst_PROGRAMS += StmtRefTest
StmtRefTest_SOURCES = StmtRefTest.C
StmtRefTest_LDADD = $(ROSE_LIBS)


noinst_PROGRAMS += PtrAnalTest
PtrAnalTest_SOURCES = PtrAnalTest.C
PtrAnalTest_LDADD = $(ROSE_LIBS)


noinst_PROGRAMS += steensgaardTest1
steensgaardTest1_SOURCES = steensgaardTest1.C
steensgaardTest1_LDADD = $(ROSE_LIBS)


noinst_PROGRAMS += steensgaardTest2
steensgaardTest2_SOURCES = steensgaardTest2.C
steensgaardTest2_LDADD = $(ROSE_LIBS)


noinst_PROGRAMS += VirtualFunctionAnalysisTest
VirtualFunctionAnalysisTest_SOURCES = VirtualFunctionAnalysisTest.C
VirtualFunctionAnalysisTest_LDADD = $(ROSE_LIBS)


noinst_PROGRAMS += ReachingDefinitionFacadeTest
ReachingDefinitionFacadeTest_SOURCES = ReachingDefinitionFacadeTest.C
ReachingDefinitionFacadeTest_LDADD = $(ROSE_LIBS)

#-------------------------------------------------------------------------------------------------------------------------------
# VirtualFunctionAnalysisTest tests using a variety of specimens from the CompileTests/Cxx_tests directory.
# Using the ROSE Test Harness for consistency with other tests

# Note: "include $(VFA_SPECIMEN_DIR)/Makefile-pass.inc" doesn't work
VFA_SPECIMEN_DIR = $(top_srcdir)/tests/nonsmoke/functional/CompileTests/Cxx_tests
include $(srcdir)/../../CompileTests/Cxx_tests/Makefile-pass.inc

if ROSE_USE_LONG_MAKE_CHECK_RULE
    VFA_TEST_SPECIMENS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)
else
    VFA_TEST_SPECIMENS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT)
endif

VFA_TEST_NAMES = $(basename $(VFA_TEST_SPECIMENS))
VFA_TEST_TARGETS = $(addprefix vfa_, $(addsuffix .passed, $(VFA_TEST_NAMES)))
VFA_TEST_SWITCHES = --edg:no_warnings -w -rose:verbose 0 --edg:restrict
VFA_TEST_INCLUDES = -I$(VFA_SPECIMEN_DIR) -I$(top_srcdir)/tests/nonsmoke/functional/CompileTests/A++Code

$(VFA_TEST_TARGETS): vfa_%.passed: $(VFA_SPECIMEN_DIR)/%.C VirtualFunctionAnalysisTest $(CHECK_EXIT_STATUS)
	@$(RTH_RUN) CMD="./VirtualFunctionAnalysisTest $(VFA_TEST_SWITCHES) $(VFA_TEST_INCLUDES) -c $<" $(CHECK_EXIT_STATUS) $@

.PHONY: check-vfa
check-vfa: $(VFA_TEST_TARGETS)

# Clean this way because argument lists are too long
.PHONY:
clean-vfa:
	rm -f $(VFA_TEST_TARGETS)
	rm -f $(VFA_TEST_TARGETS:.passed=.failed)


#-------------------------------------------------------------------------------------------------------------------------------
# Tests.  This once used the $(srcdir)/TestDriver script, but that had a bunch of problems that are avoided by using the ROSE
# Test Harness (see "scripts/rth_run.pl --help" for details).  Those problems were:
#  1. The tests could not be run in parallel
#  2. The tests could not all run when one failed (even with "make -k"), which is useful for knowing all that fail.
#  3. A single test could not be run by itself, which is useful when debugging a failing test
#  4. Tests didn't properly use makefile dependencies
#  5. Running "make check" re-ran all the tests even when their results were up-to-date.
#  6. The tests generated output even when they were passing, which makes failure output hard to distinguish
#  7. The output wasn't suitable for "build --quiet" mode (no progress reporting)
#  8. A failing test produced a backward diff; Using RTH normalizes the diff output across all of ROSE
#  9. Command output was placed in unusual file names; RTH output is always in *.passed or *.failed files
# 10. Command-lines were not printed, which made it difficult to figure out exactly what command failed.
# We now use the ROSE Test Harness and split each test from TestDriver into its own makefile target. [RPM 2013-02-25]

# DQ (1/20/2018): Updated reference files against which we diff after verifying output by hand.
# DQ (1/8/2018): Some of these tests are failing due to changes in the support for labels and case/default statements to support duff's device).
# DQ (8/23/2013): The Makefiles have an error that preventing this from running on my system.
# This needs to be discussed.
# EXTRA_TEST_NAMES = ptr_01 cfg_01 cfg_02 cfg_03 df_01 df_02 df_03 df_04 sr_01 sr_02 sr_03 vf_01 vf_02 vf_03 vf_04 vf_05
# EXTRA_TEST_NAMES = ptr_01 cfg_01 cfg_03 df_03 df_04 sr_03 vf_01 vf_02 vf_03 vf_04 vf_05
# EXTRA_TEST_NAMES += cfg_02 df_01 df_02 sr_01 sr_02 
EXTRA_TEST_NAMES = ptr_01 cfg_01 cfg_02 cfg_03 df_01 df_02 df_03 df_04 sr_01 sr_02 sr_03 vf_01 vf_02 vf_03 vf_04 vf_05

EXTRA_TEST_TARGETS = $(addsuffix .passed, $(EXTRA_TEST_NAMES))

.PHONY: check-extra
check-extra: $(EXTRA_TEST_TARGETS)

# Pointer analysis tests
ptr_01.passed: $(CHECK_ANSWER) PtrAnalTest $(srcdir)/testPtr2.C $(srcdir)/PtrAnalTest.out2
	@$(RTH_RUN) CMD="./PtrAnalTest $(srcdir)/testPtr2.C" ANS=$(srcdir)/PtrAnalTest.out2 $< $@

# Control flow graph tests
cfg_01.passed: $(CHECK_ANSWER) CFGTest $(srcdir)/testfile1.c $(srcdir)/testfile1.c.cfg
	@$(RTH_RUN) CMD="./CFGTest -I$(srcdir) $(srcdir)/testfile1.c" ANS=$(srcdir)/testfile1.c.cfg $< $@
cfg_02.passed: $(CHECK_ANSWER) CFGTest $(srcdir)/testfile2.c $(srcdir)/testfile2.c.cfg
	@$(RTH_RUN) CMD="./CFGTest -I$(srcdir) $(srcdir)/testfile2.c" ANS=$(srcdir)/testfile2.c.cfg $< $@
cfg_03.passed: $(CHECK_ANSWER) CFGTest $(srcdir)/testfile3.c $(srcdir)/testfile3.c.cfg
	@$(RTH_RUN) CMD="./CFGTest -I$(srcdir) $(srcdir)/testfile3.c" ANS=$(srcdir)/testfile3.c.cfg $< $@

# Data flow tests
df_01.passed: $(CHECK_ANSWER) DataFlowTest $(srcdir)/testfile1.c $(srcdir)/testfile1.c.du
	@$(RTH_RUN) CMD="./DataFlowTest -I$(srcdir) $(srcdir)/testfile1.c" ANS=$(srcdir)/testfile1.c.du $< $@
df_02.passed: $(CHECK_ANSWER) DataFlowTest $(srcdir)/testfile2.c $(srcdir)/testfile2.c.du
	@$(RTH_RUN) CMD="./DataFlowTest -I$(srcdir) $(srcdir)/testfile2.c" ANS=$(srcdir)/testfile2.c.du $< $@
df_03.passed: $(CHECK_ANSWER) DataFlowTest $(srcdir)/testfile3.c $(srcdir)/testfile3.c.du
	@$(RTH_RUN) CMD="./DataFlowTest -I$(srcdir) $(srcdir)/testfile3.c" ANS=$(srcdir)/testfile3.c.du $< $@
df_04.passed: $(CHECK_ANSWER) DataFlowTest $(srcdir)/testfile4.c $(srcdir)/testfile4.c.du
	@$(RTH_RUN) CMD="./DataFlowTest -I$(srcdir) $(srcdir)/testfile4.c" ANS=$(srcdir)/testfile4.c.du $< $@

# Statement ref tests
sr_01.passed: $(CHECK_ANSWER) StmtRefTest $(srcdir)/testfile1.c $(srcdir)/testfile1.c.ref
	@$(RTH_RUN) CMD="./StmtRefTest -I$(srcdir) $(srcdir)/testfile1.c" ANS=$(srcdir)/testfile1.c.ref $< $@
sr_02.passed: $(CHECK_ANSWER) StmtRefTest $(srcdir)/testfile2.c $(srcdir)/testfile2.c.ref
	@$(RTH_RUN) CMD="./StmtRefTest -I$(srcdir) $(srcdir)/testfile2.c" ANS=$(srcdir)/testfile2.c.ref $< $@
sr_03.passed: $(CHECK_ANSWER) StmtRefTest $(srcdir)/testfile3.c $(srcdir)/testfile3.c.ref
	@$(RTH_RUN) CMD="./StmtRefTest -I$(srcdir) $(srcdir)/testfile3.c" ANS=$(srcdir)/testfile3.c.ref $< $@

# Virtual function analysis tests (these are only tested for their exit status)
MOSTLYCLEANFILES += call_graph.dot full_call_graph.dot init_call_graph.dot test_vfa[0-5].ti
vf_01.passed: $(CHECK_EXIT_STATUS) VirtualFunctionAnalysisTest $(srcdir)/test_vfa1.C
	@$(RTH_RUN) CMD="./VirtualFunctionAnalysisTest -I$(srcdir) $(srcdir)/test_vfa1.C" $< $@
vf_02.passed: $(CHECK_EXIT_STATUS) VirtualFunctionAnalysisTest $(srcdir)/test_vfa2.C
	@$(RTH_RUN) CMD="./VirtualFunctionAnalysisTest -I$(srcdir) $(srcdir)/test_vfa2.C" $< $@
vf_03.passed: $(CHECK_EXIT_STATUS) VirtualFunctionAnalysisTest $(srcdir)/test_vfa3.C
	@$(RTH_RUN) CMD="./VirtualFunctionAnalysisTest -I$(srcdir) $(srcdir)/test_vfa3.C" $< $@
vf_04.passed: $(CHECK_EXIT_STATUS) VirtualFunctionAnalysisTest $(srcdir)/test_vfa4.C
	@$(RTH_RUN) CMD="./VirtualFunctionAnalysisTest -I$(srcdir) $(srcdir)/test_vfa4.C" $< $@
vf_05.passed: $(CHECK_EXIT_STATUS) VirtualFunctionAnalysisTest $(srcdir)/test_vfa5.C
	@$(RTH_RUN) CMD="./VirtualFunctionAnalysisTest -I$(srcdir) $(srcdir)/test_vfa5.C" $< $@

MOSTLYCLEANFILES +=				\
	$(EXTRA_TEST_TARGETS)			\
	$(EXTRA_TEST_TARGETS:.passed=.failed)

EXTRA_DIST +=								\
	testfile1.c testfile1.c.cfg testfile1.c.du testfile1.c.ref	\
	testfile2.c testfile2.c.cfg testfile2.c.du testfile2.c.ref	\
	testfile3.c testfile3.c.cfg testfile3.c.du testfile3.c.ref	\
	testfile4.c testfile4.c.du  testPtr1.C testPtr2.C		\
	PtrAnalTest.out1  steensgaardTest1.outx   steensgaardTest2.out2	\
	PtrAnalTest.out2  steensgaardTest2.out1				\
	test_vfa1.C test_vfa2.C test_vfa3.C test_vfa4.C test_vfa5.C

# Not used
#AnnotationLanguageParserTestRule: trustedAnnotationOutput
#	./AnnotationLanguageParserTest $(srcdir)/annotationLanguageParserInputFile.ann > annotationOutput
#	diff annotationOutput trustedAnnotationOutput || exit


#-------------------------------------------------------------------------------------------------------------------------------
# Automake boilerplate

check-local: check-vfa check-extra
	@echo "*******************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/roseTests/programAnalysisTests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************"


clean-local: clean-vfa
	rm -f $(MOSTLYCLEANFILES)
