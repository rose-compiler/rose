include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#####################################################################################
# These are UPC testcodes targeting aspects of UPCC that might not be a subset of C #
#####################################################################################

# affinity &arr[i] is parsed to arr+i, may hurt the later translation
# Laks: add the test files here
TESTCODES_REQUIRED_TO_PASS = \
     caf-decl.f90  \
     caf-allocate.f90 \
     caf-team.f90 \
     caf.f90 \
     caf-derived_type.caf \
     caf-topology.caf 

TESTCODE_CURRENTLY_FAILING = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) 

ROSE_FLAGS = -rose:skip_syntax_check -rose:skipfinalCompileStep

PASSING_TEST_Objects = ${TESTCODES:.f90=.o}
TEST_Objects = ${ALL_TESTCODES:.f90=.o}

$(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.f90)
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.f90)

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

# QMTEST_Objects = ${ALL_TESTCODES:.f90=.qmt}

# Make rule to build the QMTest database files
#CURRENT_DIRECTORY = `pwd`
#$(QMTEST_Objects): ../../testTranslator $(srcdir)/$(@:.qmt=.f90)
#	@echo "Calling QMTEST_Objects rule: "
#	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.f90)

# Include makefile rules specific to QMTest
# include $(top_srcdir)/config/QMTest_makefile.inc

# put the headers here for now. TODO: Move them to a better place
EXTRA_DIST = $(ALL_TESTCODES) 

check-local:
	@echo "Tests for CAF examples."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/CAF_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_*.* 

