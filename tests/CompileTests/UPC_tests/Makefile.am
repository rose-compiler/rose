include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#####################################################################################
# These are UPC testcodes targeting aspects of UPCC that might not be a subset of C #
#####################################################################################

TESTCODES_REQUIRED_TO_PASS = \
     test2004_22.C

TESTCODE_CURRENTLY_FAILING = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

ROSE_FLAGS = --edg:no_warnings -w --edg:restrict --edg:upc

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

$(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.C)
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.C)

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): ../../testTranslator $(srcdir)/$(@:.qmt=.C)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

EXTRA_DIST = $(ALL_TESTCODES)

check-local:
	@echo "Tests for UPC examples."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/UPC_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC]
	rm -rf QMTest

