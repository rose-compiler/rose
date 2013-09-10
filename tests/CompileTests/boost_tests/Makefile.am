include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

############################################################################################
# These are C++ testcodes using the Boost Library (if these are comented out it is because #
# they are currently time consuming to run, interested user may run them directly)         #
############################################################################################

TESTCODES_REQUIRED_TO_PASS =

# DQ (11/3/2011): Can't find "boost/spirit/phoenix/core.hpp"
# test_boost_phoenix_v2.C

# The file test_boost_lambda.C and test_boost_phoenix_v1.C do not fail, but take too long to be included in frequent tests
TESTCODE_CURRENTLY_FAILING = \
test_boost_lambda.C \
test_boost_phoenix_v1.C \
test_boost_phoenix_v2.C

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) 

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict --edg:export
ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

TRANSLATOR = ../../testTranslator

$(TEST_Objects): $(TRANSLATOR) $(srcdir)/$(@:.o=.C)
	$(TRANSLATOR) $(ROSE_FLAGS) -I$(srcdir)/usr/local/mstools/include -c $(srcdir)/$(@:.o=.C)

# This test code currently fails due to mangled names being too long!
test_boost_phoenix_v2.o: ../../testTranslator
	$(VALGRIND) $(TRANSLATOR) -rose:verbose 0 -I$(srcdir)/usr/local/mstools/include -I$(srcdir)/usr/local/mstools/include/phoenix_v2 -c $(srcdir)/test_boost_phoenix_v2.C


CURRENT_DIRECTORY = `pwd`
QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
# A number of tests require the path to the A++ include directory 
# and a number of other tests require a path to the source directory.
# $(QMTEST_Objects): preprocessor $(srcdir)/$(@:.qmt=.C)
$(QMTEST_Objects): ../../testTranslator
#	@echo "Calling QMTEST_Objects rule for Cpp_tests directory: "
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(srcdir)/usr/local/mstools/include -c $(srcdir)/$(@:.qmt=.C)

test_boost_phoenix_v2.qmt: ../../testTranslator
	$(VALGRIND) ../preprocessor -rose:verbose 0 -I$(srcdir)/usr/local/mstools/include -I$(srcdir)/usr/local/mstools/include/phoenix_v2 -c $(srcdir)/test_boost_phoenix_v2.C
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(srcdir)/usr/local/mstools/include -I$(srcdir)/usr/local/mstools/include/phoenix_v2 -c $(srcdir)/$(@:.qmt=.C)


# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

EXTRA_DIST = $(ALL_TESTCODES)

check-local:
	@echo "Tests of Boost Library examples."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/boost_tests: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC]
	rm -rf QMTest

