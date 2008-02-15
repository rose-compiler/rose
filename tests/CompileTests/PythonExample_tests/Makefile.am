include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###################################################
# These are Python testcodes (C++ using Python.h) #
###################################################

# Shorten the list of code to compile to simplify checks using "make distcheck"
TESTCODES_REQUIRED_TO_PASS = \
     test2004_92.C

EXTRA_TESTCODES_REQUIRED_TO_PASS = \
     test2004_92.C \
     test2005_18.C \
     test2005_54.C

TESTCODE_CURRENTLY_FAILING = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
ROSE_FLAGS = --edg:no_warnings -w --edg:restrict

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.C)
$(TEST_Objects): ../../testTranslator
	@echo "Calling TEST_Objects rule: "
	../../testTranslator $(ROSE_FLAGS) -I$(srcdir)/pythonDir -c $(srcdir)/$(@:.o=.C)

QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): ../../testTranslator $(srcdir)/$(@:.qmt=.C)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(srcdir)/pythonDir -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

EXTRA_DIST = $(ALL_TESTCODES) pythonDir

check-local:
	@echo "Tests for Python examples (tests python header files as used by SWIG)."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/PythonExample_tests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC]
	rm -rf QMTest

