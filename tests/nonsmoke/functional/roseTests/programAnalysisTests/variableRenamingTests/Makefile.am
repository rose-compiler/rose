include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =

###############################################################################################################################
# Test specimens: inputs to the test program
###############################################################################################################################

# Location of input specimens (so we only need to list the file base names in the rest of the makefile).
SPECIMEN_DIR = $(top_srcdir)/tests/nonsmoke/functional/CompileTests/Cxx_tests

# These specimens were commented out in the edg3 branch with the comment that they "should be fixed (as opposed to testcodes
# that are expected to fail)" and which currently also fail in the edg4x branch.
FAILURE_TO_PASS = test2004_02.C test2005_114.C test2005_187.C test2006_47.C

# These specimens fail the test but were also not tested in the edg3x release branch.
NOT_TESTED_IN_EDG3 = test2006_54.C test2011_78.C

# Do not run the test against these input specimens.
EXCLUDE_SPECIMENS = $(FAILURE_TO_PASS) $(NOT_TESTED_IN_EDG3)

# All the input specimens against which we should run the test.  The EXAMPLE_TESTCODES_REQUIRED_TO_PASS comes from
# the following included file (BTW, it doesn't work to include it as "$(SPECIMEN_DIR)/Makefile-pass.inc").
include ../../../CompileTests/Cxx_tests/Makefile-pass.inc
SPECIMENS = $(filter-out $(EXCLUDE_SPECIMENS), $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS))

# We test only the exit status of the testVariableRenaming executable.
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

###############################################################################################################################
# The test executable
###############################################################################################################################

noinst_PROGRAMS += testVariableRenaming
testVariableRenaming_SOURCES = testVariableRenaming.C
testVariableRenaming_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)
testVariableRenaming_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

###############################################################################################################################
# The individual tests: the test executable run against each individual input specimen.
###############################################################################################################################

testVariableRenamingSwitches = --edg:no_warnings -w -rose:verbose -I$(SPECIMEN_DIR)/../A++Code -I$(SPECIMEN_DIR)
testVariableRenamingTests = $(addprefix tvr_, $(addsuffix .passed, $(SPECIMENS)))
$(testVariableRenamingTests): tvr_%.passed: $(SPECIMEN_DIR)/% $(TEST_EXIT_STATUS) testVariableRenaming
	@$(RTH_RUN) CMD="./testVariableRenaming $(testVariableRenamingSwitches) -c $<" $(TEST_EXIT_STATUS) $@


###############################################################################################################################
# Automake targets
###############################################################################################################################

check-local: $(testVariableRenamingTests)
	@echo "*****************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/roseTests/programAnalysisTests/variableRenamingTests: make check rule complete (terminated normally) ******"
	@echo "*****************************************************************************************************************************"

clean-local:
	rm -f $(testVariableRenamingTests)
	rm -f $(testVariableRenamingTests:.passed=.failed)
