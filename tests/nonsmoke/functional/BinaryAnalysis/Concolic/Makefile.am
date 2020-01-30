include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/nonsmoke/specimens/binary/SpecimenSets.inc

EXTRA_DIST =

# Forward declarations
TEST_TARGETS=
noinst_PROGRAMS=
MOSTLYCLEANFILES=
PHONIES=

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

# To keep compile commands tidy, please ONLY put required include flags here.
AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

# Tests are disabled if SQLite3 is not installed since that's the only database driver they currently support.
if ROSE_USE_SQLITE_DATABASE
DISABLED =
else
DISABLED = no supported database driver
endif

########################################################################################################################
# Tests
########################################################################################################################

#------------------------------------------------------------------------------------------------------------------------
# Test concolic database operations

#
# utilities and sample codes

# a.1) sample program to load and store in DB
noinst_PROGRAMS += sampleExecutable
sampleExecutable_SOURCES = sampleExecutable.C
sampleExecutable_LDADD =

# a.2) sample program to test the execution monitor
noinst_PROGRAMS += test-execution-monitor
test_execution_monitor_SOURCES = test-execution-monitor.C
test_execution_monitor_LDADD =

# a.3) simplest execution monitor to count unique instruction
noinst_PROGRAMS += rose-execution-monitor-zero
rose_execution_monitor_zero_SOURCES = rose-execution-monitor-zero.C
rose_execution_monitor_zero_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)

# a.4) test compiled programs.
noinst_PROGRAMS += testConcolicDB
testConcolicDB_SOURCES = testConcolicDB.C configDB.h
testConcolicDB_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)

# a.5) execution wrapper for perf to intercept signals
#~ noinst_PROGRAMS += rose-perf-execution-wrapper
#~ rose_perf_execution_wrapper_SOURCES = rose-perf-execution-wrapper.c
#~ rose_perf_execution_wrapper_LDADD =

# a.6) reads perf-report output and computes unique instruction count
#~ AM_YFLAGS = -d -t

#~ noinst_PROGRAMS += rose-perf-analyzer
#~ rose_perf_analyzer_SOURCES = PerfOutputAnalyzer/parser.ypp PerfOutputAnalyzer/lexer.lpp \
#~                              PerfOutputAnalyzer/analyzer.cpp 
#~ rose_perf_analyzer_CPPFLAGS = $(ROSE_INCLUDES)
#~ rose_perf_analyzer_LDFLAGS = $(ROSE_RPATHS)
#~ rose_perf_analyzer_LDADD = $(ROSE_LIBS)

# a.7) the script that organizes perf's execution

#~ noinst_SCRIPTS = rose-execmon-linux-perf-intel-x86-64.sh

#~ rose-execmon-linux-perf-intel-x86-64.sh: $(srcdir)/rose-execution-monitor-linux-perf-intel-x86_64.sh rose-perf-analyzer
#~ 	cp $< $@
#~ 	chmod u+x $@

#	
# targets	

TEST_TARGETS += testConcolicDB.passed

testConcolicDB.passed: $(TEST_EXIT_STATUS) testConcolicDB
	@$(RTH_RUN) \
		TITLE="concolic database [$@]" \
		CMD="./testConcolicDB" \
		ANS="no" \
		DISABLED="$(DISABLED)" \
		$< $@

# b) test various executions

noinst_PROGRAMS += testOpenDb
testOpenDb_SOURCES = testOpenDb.C
testOpenDb_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testOpenDb.passed
testOpenDb.passed: $(TEST_EXIT_STATUS) testOpenDb
	@$(RTH_RUN) \
		TITLE="concolic database openDB [$@]" \
		CMD="./testOpenDb" \
		DISABLED="$(DISABLED)" \
		$< $@

noinst_PROGRAMS += testDefineTests
testDefineTests_SOURCES = testDefineTests.C
testDefineTests_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testDefineTests.passed
testDefineTests.passed: $(TEST_EXIT_STATUS) testDefineTests
	@$(RTH_RUN) \
		TITLE="concolic database defineTests [$@]" \
		CMD="./testDefineTests" \
		DISABLED="$(DISABLED)" \
		$< $@


noinst_PROGRAMS += testRun1
testRun1_SOURCES = testRun1.C
testRun1_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testRun1.passed
testRun1.passed: $(TEST_EXIT_STATUS) testRun1
	@$(RTH_RUN) \
		TITLE="concolic database run1 [$@]" \
		CMD="./testRun1" \
		DISABLED="$(DISABLED)" \
		$< $@

noinst_PROGRAMS += testRun10
testRun10_SOURCES = testRun10.C
testRun10_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testRun10.passed
testRun10.passed: $(TEST_EXIT_STATUS) testRun10
	@$(RTH_RUN) \
		TITLE="concolic database run10 [$@]" \
		CMD="./testRun10" \
		DISABLED="$(DISABLED)" \
		$< $@

noinst_PROGRAMS += testDefineFail
testDefineFail_SOURCES = testDefineFail.C
testDefineFail_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testDefineFail.passed
testDefineFail.passed: $(TEST_EXIT_STATUS) testDefineFail
	@$(RTH_RUN) \
		TITLE="concolic database defineFail [$@]" \
		CMD="./testDefineFail" \
		DISABLED="$(DISABLED)" \
		$< $@

noinst_PROGRAMS += testCombined
testCombined_SOURCES = testCombined.C
testCombined_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testCombined.passed
testCombined.passed: $(TEST_EXIT_STATUS) testCombined
	@$(RTH_RUN) \
		TITLE="concolic database combined [$@]" \
		CMD="./testCombined" \
		DISABLED="$(DISABLED)" \
		$< $@

noinst_PROGRAMS += testExport
testExport_SOURCES = testExport.C
testExport_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testExport.passed
testExport.passed: $(TEST_EXIT_STATUS) testExport sampleExecutable
	@$(RTH_RUN) \
		TITLE="concolic database testExport [$@]" \
		CMD="./testExport" \
		DISABLED="$(DISABLED)" \
		$< $@

noinst_PROGRAMS += testAddressRandomization
testAddressRandomization_SOURCES = testAddressRandomization.C
testAddressRandomization_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testAddressRandomization.passed
testAddressRandomization.passed: $(TEST_EXIT_STATUS) testAddressRandomization sampleExecutable
	@$(RTH_RUN) \
		TITLE="concolic database testAddressRandomization [$@]" \
		CMD="./testAddressRandomization" \
		DISABLED="$(DISABLED)" \
		$< $@

# This test has a problem: both this test and testPerfExecutionMonitor create test-execution-manager, which is a parallel race.
# Commenting out both in order to consistent with the Tup build.
#TEST_TARGETS += testExecutionMonitor.passed
#testExecutionMonitor.passed: $(srcdir)/defaultcrsh.conf crsh/crsh rose-execution-monitor-zero sampleExecutable test-execution-monitor
#	@$(RTH_RUN) \
#		TITLE="concolic database testExecutionMonitor [$@]" \
#		CMD="./crsh/crsh $(srcdir)/testExecutionMonitor.crsh" \
#		ANS="no" \
#		DISABLED="$(DISABLED)" \
#		$< $@
#
#~ TEST_TARGETS += testPerfExecutionMonitor.passed
#
#~ testPerfExecutionMonitor.passed: $(srcdir)/defaultcrsh.conf crsh/crsh rose-execmon-linux-perf-intel-x86-64.sh sampleExecutable test-execution-monitor
#~ 	@$(RTH_RUN) \
#~ 		TITLE="concolic database testPerfExecutionMonitor [$@]" \
#~ 		CMD="./crsh/crsh $(srcdir)/testPerfExecutionMonitor.crsh" \
#~ 		ANS="no" \
#~		DISABLED="$(DISABLED)" \
#~ 		$< $@

noinst_PROGRAMS += testConnectNonExisting
testConnectNonExisting_SOURCES = testConnectNonExisting.C
testConnectNonExisting_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testConnectNonExisting.passed
testConnectNonExisting.passed: $(TEST_EXIT_STATUS) testConnectNonExisting
	@$(RTH_RUN) \
		TITLE="concolic database testConnectNonExisting [$@]" \
		CMD="./testConnectNonExisting" \
		DISABLED="$(DISABLED)" \
		$< $@

noinst_PROGRAMS += testCreateOverwrites
testCreateOverwrites_SOURCES = testCreateOverwrites.C
testCreateOverwrites_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testCreateOverwrites.passed
testCreateOverwrites.passed: $(TEST_EXIT_STATUS) testCreateOverwrites
	@$(RTH_RUN) \
		TITLE="concolic database testCreateOverwrites [$@]" \
		CMD="./testCreateOverwrites" \
		DISABLED="$(DISABLED)" \
		$< $@

noinst_PROGRAMS += testConnect
testConnect_SOURCES = testConnect.C
testConnect_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)
TEST_TARGETS += testConnect.passed
testConnect.passed: $(TEST_EXIT_STATUS) testConnect
	@$(RTH_RUN) \
		TITLE="concolic database testConnect [$@]" \
		CMD="./testConnect" \
		DISABLED="$(DISABLED)" \
		$< $@

noinst_PROGRAMS += testTestCase
testTestCase_SOURCES = testTestCase.C
testTestCase_LDADD = $(ROSE_SEPARATE_LIBS)
MOSTLYCLEANFILES += testTestCase.db
TEST_TARGETS += testTestCase.passed
testTestCase.passed: $(TEST_EXIT_STATUS) testTestCase
	@$(RTH_RUN)				\
		TITLE="test test cases [$@]"	\
		CMD="./testTestCase"		\
		DISABLED="$(DISABLED)"		\
		$< $@

#------------------------------------------------------------------------------------------------------------------------
# Test concolic executor

noinst_PROGRAMS += testConcolicExecutor
testConcolicExecutor_SOURCES = testConcolicExecutor.C
testConcolicExecutor_LDADD = $(ROSE_SEPARATE_LIBS)
testConcolicExecutor_specimen = $(top_srcdir)/tests/nonsmoke/specimens/binary/concolic-specimen-01
testConcolicExecutor_flags = --log 'Rose::BinaryAnalysis::Concolic(debug)' --database 'testConcolicExecutor-1.db'
MOSTLYCLEANFILES += testConcolicExecutor-1.db

TEST_TARGETS += testConcolicExecutor.passed

testConcolicExecutor.passed: $(TEST_EXIT_STATUS) testConcolicExecutor $(testConcolicExecutor_specimen)
	@$(RTH_RUN)												\
	    TITLE="concolic executor [$@]"									\
	    CMD="$$(pwd)/testConcolicExecutor $(testConcolicExecutor_flags) $(testConcolicExecutor_specimen)"	\
	    DISABLED="$(DISABLED)"										\
	    $< $@

###############################################################################################################################
# Standard boilerplate
###############################################################################################################################

check-local: $(TEST_TARGETS)
else
# Binary analysis is not enabled
check-local:
	@echo "  TESTING (disabled: binary frontend is not enabled)"
endif

# Don't delete things indiscriminately--sometimes developers have other related files that should not be touched!
# Rather than use one big MOSTLYCLEANFILES, split things into smaller parts so we don't get command lines that are too big.
clean-local:
	rm -f $(MOSTLYCLEANFILES)

.PHONY: $(PHONIES)
