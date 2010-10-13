include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##################################################################################
# These are C testcodes targeting aspects of C that might not be a subset of C++ #
##################################################################################

TESTCODES_REQUIRED_TO_PASS = \
     simple.java

TESTCODE_CURRENTLY_FAILING = 

noinst_headers = 


# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# File option to accumulate performance information about the compilation
# PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/C_ROSE_PERFORMANCE_DATA.csv

ROSE_FLAGS = -rose:java

PASSING_TEST_Objects = ${TESTCODES:.java=.o}
TEST_Objects = ${ALL_TESTCODES:.java=.o}

testTranslator=../../testTranslator

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.java)
$(TEST_Objects): ../../testTranslator
	$(testTranslator) $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.java)

EXTRA_DIST = $(ALL_TESTCODES)

check-local:
	@echo "Tests for Java."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
#	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Java_tests: make check rule complete (terminated normally) ******"
	@echo "************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[java] rose_performance_report_lockfile.lock *.out

