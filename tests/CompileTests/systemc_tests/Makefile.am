include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

####################################################################################
# These are ROSE translators and test the compilation ROSE applications using ROSE #
####################################################################################

ROSE_INCLUDES = -I$(srcdir)/include

# Files that represent header files from ROSE that we can now compilie with ROSE.
# TESTCODES_REQUIRED_TO_PASS = input_hello_world.C
TESTCODES_REQUIRED_TO_PASS = 

EXTRA_TESTCODES_REQUIRED_TO_PASS = 

TESTCODE_CURRENTLY_FAILING = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

ROSE_FLAGS = --edg:no_warnings -w --edg:restrict 

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
PASSING_TEST_Sources = ${TESTCODES:.C=.C.passed}
TEST_Objects = ${ALL_TESTCODES:.C=.o}
TEST_Sources = ${ALL_TESTCODES:.C=.C.passed}

TEST_TRANSLATOR = ../../testTranslator

# DQ (2/10/2013): Make all of the tests codes
$(TEST_TRANSLATOR):
	cd ../..; $(MAKE) test

# $(TEST_Objects): $(TEST_TRANSLATOR) $(srcdir)/$(@:.o=.C)
#	../../testTranslator $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/$(@:.o=.C)

$(TEST_Sources): %.passed: $(TEST_TRANSLATOR) $(srcdir)/$(@:.C.passed=.C)
#	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/$(@:.o=.C)
	@$(RTH_RUN) CMD="$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/$(@:.C.passed=.C)" $(top_srcdir)/scripts/test_exit_status $@

QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): $(TEST_TRANSLATOR) $(srcdir)/$(@:.qmt=.C)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc


# ****************************************************************************************************
#     Test codes or selected ROSE code that can compile using ROSE or is the subject of current work 
# ****************************************************************************************************

../../testGraphGeneration:
	$(MAKE) testGraphGeneration -C $(top_builddir)/tests

# This tests the long ROSE command line of ROSE compiling ROSE (the file is empty).
test0: $(srcdir)/testRoseHeaders_00.C ../../testGraphGeneration
	../../testGraphGeneration $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRoseHeaders_00.C

# We don't want to check any of these in to the ROSE repository (internal testing only).
# EXTRA_DIST = $(TESTCODES)
EXTRA_DIST = $(TESTCODES)

check-local:
	@echo "Tests of SystemC examples."
	@$(MAKE) $(PASSING_TEST_Sources)
	@echo "**************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/sytemc_tests: make check rule complete (terminated normally) ******"
	@echo "**************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot
	rm -rf QMTest
	rm -f *.passed *.failed
