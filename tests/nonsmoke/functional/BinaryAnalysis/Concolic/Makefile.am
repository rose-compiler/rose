include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/nonsmoke/specimens/binary/SpecimenSets.inc

SUBDIRS =

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

noinst_PROGRAMS += testConcolicDB
testConcolicDB_SOURCES = testConcolicDB.C
testConcolicDB_LDADD = $(ROSE_LIBS_WITH_PATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testConcolicDB.passed

testConcolicDB.passed: $(top_srcdir)/tests/nonsmoke/functional/BinaryAnalysis/Concolic/concolicDB.conf testConcolicDB # sampleExecutable
	@$(RTH_RUN) \
		TITLE="concolic database [$@]" \
		CMD="./testConcolicDB" \
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
