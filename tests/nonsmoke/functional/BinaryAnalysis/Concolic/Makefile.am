include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/nonsmoke/specimens/binary/SpecimenSets.inc

SUBDIRS = crsh

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

# a.3) execution monitor to produce quality score
noinst_PROGRAMS += ExecutionMonitor
ExecutionMonitor_SOURCES = ExecutionMonitor.C
ExecutionMonitor_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)

# a.4) test compiled programs.
noinst_PROGRAMS += testConcolicDB
testConcolicDB_SOURCES = testConcolicDB.C configDB.h
testConcolicDB_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)

# a.5) execution wrapper for perf to intercept signals
noinst_PROGRAMS += rose-perf-execution-wrapper
rose_perf_execution_wrapper_SOURCES = rose-perf-execution-wrapper.c
rose_perf_execution_wrapper_LDADD =

# a.6) reads perf-report output and computes unique instruction count
AM_YFLAGS = -d -t

noinst_PROGRAMS += rose-perf-analyzer
rose_perf_analyzer_SOURCES = PerfOutputAnalyzer/parser.ypp PerfOutputAnalyzer/lexer.lpp \
                             PerfOutputAnalyzer/analyzer.cpp 
rose_perf_analyzer_CPPFLAGS = $(ROSE_INCLUDES)
rose_perf_analyzer_LDFLAGS = $(ROSE_RPATHS)
rose_perf_analyzer_LDADD = $(ROSE_LIBS)

# a.7) the script that organizes perf's execution

noinst_SCRIPTS = rose-execmon-linux-perf-intel-x86-64.sh

rose-execmon-linux-perf-intel-x86-64.sh: $(srcdir)/rose-execution-monitor-linux-perf-intel-x86_64.sh rose-perf-analyzer
	cp $< $@
	chmod u+x $@
	
#	
# targets	

TEST_TARGETS += testConcolicDB.passed

testConcolicDB.passed: $(srcdir)/concolicDB.conf testConcolicDB
	@$(RTH_RUN) \
		TITLE="concolic database [$@]" \
		CMD="./testConcolicDB" \
		ANS="no" \
		$< $@

# b) test various executions

TEST_TARGETS += testOpenDB.passed

testOpenDB.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database openDB [$@]" \
		CMD="./crsh/crsh $(srcdir)/openDB.crsh" \
		ANS="no" \
		$< $@
	rm -f openDB.db

TEST_TARGETS += testDefineTests.passed

testDefineTests.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database defineTests [$@]" \
		CMD="./crsh/crsh $(srcdir)/defineTests.crsh" \
		ANS="no" \
		$< $@
	rm -f defineTests.db

TEST_TARGETS += testRun1.passed

testRun1.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database run1 [$@]" \
		CMD="./crsh/crsh $(srcdir)/run1.crsh" \
		ANS="no" \
		$< $@
	rm -f run1.db


TEST_TARGETS += testRun10.passed

testRun10.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database run10 [$@]" \
		CMD="./crsh/crsh $(srcdir)/run10.crsh" \
		ANS="no" \
		$< $@
	rm -f run10.db


TEST_TARGETS += testDefineFail.passed

testDefineFail.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database defineFail [$@]" \
		CMD="./crsh/crsh $(srcdir)/defineFail.crsh" \
		ANS="no" \
		$< $@
	rm -f defineFail.db

TEST_TARGETS += testCombined.passed

testCombined.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database combined [$@]" \
		CMD="./crsh/crsh $(srcdir)/combined.crsh" \
		ANS="no" \
		$< $@
	rm -f combined.db

TEST_TARGETS += testExport.passed

testExport.passed: $(srcdir)/defaultcrsh.conf crsh/crsh sampleExecutable
	@$(RTH_RUN) \
		TITLE="concolic database testExport [$@]" \
		CMD="./crsh/crsh $(srcdir)/testExport.crsh" \
		ANS="no" \
		$< $@
	rm -f testExport.db


TEST_TARGETS += testAddressRandomization.passed

testAddressRandomization.passed: $(srcdir)/defaultcrsh.conf crsh/crsh sampleExecutable
	@$(RTH_RUN) \
		TITLE="concolic database testAddressRandomization [$@]" \
		CMD="./crsh/crsh $(srcdir)/testAddressRandomization.crsh" \
		ANS="no" \
		$< $@

TEST_TARGETS += testExecutionMonitor.passed

testExecutionMonitor.passed: $(srcdir)/defaultcrsh.conf crsh/crsh ExecutionMonitor sampleExecutable test-execution-monitor
	@$(RTH_RUN) \
		TITLE="concolic database testExecutionMonitor [$@]" \
		CMD="./crsh/crsh $(srcdir)/testExecutionMonitor.crsh" \
		ANS="no" \
		$< $@

TEST_TARGETS += testPerfExecutionMonitor.passed

testPerfExecutionMonitor.passed: $(srcdir)/defaultcrsh.conf crsh/crsh rose-execmon-linux-perf-intel-x86-64.sh sampleExecutable test-execution-monitor
	@$(RTH_RUN) \
		TITLE="concolic database testPerfExecutionMonitor [$@]" \
		CMD="./crsh/crsh $(srcdir)/testPerfExecutionMonitor.crsh" \
		ANS="no" \
		$< $@

TEST_TARGETS += testConnectNonExisting.passed

testConnectNonExisting.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database testConnectNonExisting [$@]" \
		CMD="./crsh/crsh $(srcdir)/testConnectNonExisting.crsh" \
		ANS="no" \
		$< $@

TEST_TARGETS += testCreateOverwrites.passed

testCreateOverwrites.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database testCreateOverwrites [$@]" \
		CMD="./crsh/crsh $(srcdir)/testCreateOverwrites.crsh" \
		ANS="no" \
		$< $@

TEST_TARGETS += testConnect.passed

testConnect.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database testConnect [$@]" \
		CMD="./crsh/crsh $(srcdir)/testConnect.crsh" \
		ANS="no" \
		$< $@



#------------------------------------------------------------------------------------------------------------------------
# Test concolic executor

noinst_PROGRAMS += testConcolicExecutor
testConcolicExecutor_SOURCES = testConcolicExecutor.C
testConcolicExecutor_LDADD = $(ROSE_SEPARATE_LIBS)
testConcolicExecutor_specimen = $(top_srcdir)/tests/nonsmoke/specimens/binary/concolic-specimen-01
testConcolicExecutor_flags = --log 'Rose::BinaryAnalysis::Concolic(debug)'

TEST_TARGETS += testConcolicExecutor.passed

testConcolicExecutor.passed: $(TEST_EXIT_STATUS) testConcolicExecutor $(testConcolicExecutor_specimen)
	@$(RTH_RUN)												\
	    TITLE="concolic executor [$@]"									\
	    CMD="$$(pwd)/testConcolicExecutor $(testConcolicExecutor_flags) $(testConcolicExecutor_specimen)"	\
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
