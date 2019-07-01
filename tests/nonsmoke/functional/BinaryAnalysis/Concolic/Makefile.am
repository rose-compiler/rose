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

# a) sample program to load and store in DB
noinst_PROGRAMS += sampleExecutable
sampleExecutable_SOURCES = sampleExecutable.C
sampleExecutable_LDADD =

# a) test compiled programs
noinst_PROGRAMS += testConcolicDB
testConcolicDB_SOURCES = testConcolicDB.C configDB.h
testConcolicDB_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)

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
		ANS="yes" \
		$< $@
	rm -f openDB.db

TEST_TARGETS += testDefineTests.passed

testDefineTests.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database defineTests [$@]" \
		CMD="./crsh/crsh $(srcdir)/defineTests.crsh" \
		ANS="yes" \
		$< $@
	rm -f defineTests.db

TEST_TARGETS += testRun1.passed

testRun1.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database run1 [$@]" \
		CMD="./crsh/crsh $(srcdir)/run1.crsh" \
		ANS="yes" \
		$< $@
	rm -f run1.db


TEST_TARGETS += testRun10.passed

testRun10.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database run10 [$@]" \
		CMD="./crsh/crsh $(srcdir)/run10.crsh" \
		ANS="yes" \
		$< $@
	rm -f run10.db


TEST_TARGETS += testDefineFail.passed

testDefineFail.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database defineFail [$@]" \
		CMD="./crsh/crsh $(srcdir)/defineFail.crsh" \
		ANS="yes" \
		$< $@
	rm -f defineFail.db

TEST_TARGETS += testCombined.passed

testCombined.passed: $(srcdir)/defaultcrsh.conf crsh/crsh
	@$(RTH_RUN) \
		TITLE="concolic database combined [$@]" \
		CMD="./crsh/crsh $(srcdir)/combined.crsh" \
		ANS="yes" \
		$< $@
	rm -f combined.db

TEST_TARGETS += testExport.passed

testExport.passed: $(srcdir)/defaultcrsh.conf crsh/crsh sampleExecutable
	@$(RTH_RUN) \
		TITLE="concolic database testExport [$@]" \
		CMD="./crsh/crsh $(srcdir)/testExport.crsh" \
		ANS="yes" \
		$< $@
	rm -f testExport.db



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
