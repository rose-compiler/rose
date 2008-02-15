include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#################################################################
# These are C++ testcodes targeting use of expression templates #
#################################################################

# Shorten the list of code to compile to simplify checks using "make distcheck"
TESTCODES_REQUIRED_TO_PASS = 

TESTCODES_THAT_TAKE_TOO_LONG = \
     test2006_30.C \
     test2006_31.C \
     test2006_32.C

# The test2006_29.C file is a classic expression template example code and it
# works fine but takes too long to be practical to compile within "make check".
# It appears to take about 60 minutes to compile (likely part of a bug in ROSE).
# It is likely dues to the current inefficency of computing names (particualrly 
# mangled names) within ROSE.  This will be fixed later.
TESTCODE_CURRENTLY_FAILING = \
     test2006_29.C 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

$(TEST_Objects): ../../testTranslator
	@echo "Calling TEST_Objects rule: "
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.C)

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
	@echo "Tests for Expression Template examples."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*********************************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/ExpressionTemplateExample_tests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************************"

