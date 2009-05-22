include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Import list of C++ test cases that should pass:
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT
#
include $(srcdir)/Makefile-pass.inc

# Import list of C++ test cases that are known to fail:
#   TESTCODE_CURRENTLY_FAILING
#
include $(srcdir)/Makefile-fail.inc

# Configure-time automake conditional
if ROSE_USE_LONG_MAKE_CHECK_RULE

# This is the long test of ROSE (takes a while!)
TESTCODES_REQUIRED_TO_PASS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)

else # !ROSE_USE_LONG_MAKE_CHECK_RULE

# Shorten the list of code to compile to simplify checks using "make distcheck"
# Note that the tests test2001_02.C and test2001_03.C are required because the
# tests in ../C_subset_of_Cxx use them and that test2004_116.C is required 
# because the tests in ROSE/tests/roseTests/programTransformationTests use it.
TESTCODES_REQUIRED_TO_PASS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT)

endif # ROSE_USE_LONG_MAKE_CHECK_RULE

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) 

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0

# Some of these test codes reference A++ header fiels as part of their tests
# Include the path to A++ and the transformation specification
TESTCODE_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

# A number of tests require the path to the A++ include directory 
# and a number of other tests require a path to the source directory.
# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.C)
$(TEST_Objects): ../../testTranslator
	../../testTranslator $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)

# This requires a special makefile rule
test2005_56.o: ../../testTranslator
	../../testTranslator $(ROSE_FLAGS) -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\" -c $(srcdir)/test2005_56.C -o test2005_56.o

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

CURRENT_DIRECTORY = `pwd`
QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

$(QMTEST_Objects): ../../testTranslator
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.qmt=.C)

# This must be placed after the more general rule to work properly!
test2005_56.qmt: ../../testTranslator
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\" -I$(srcdir) -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# Make sure that all the test codes end up in the distribution
EXTRA_DIST = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING) test_CplusplusMacro_C.C test2006_137.C \
     confdefs.h  test2001_05.h  test2001_11.h test2001_18A.h  test2003_08.h  test2003_12.h  test2003_24.h  test2004_40.h \
     test2005_154.h  test2005_158.h  test2005_15.h   test2005_26.h  test2005_81.h  test2006_33.h foo.h \
     test2001_15.h  test2001_18B.h  test2003_09.h  test2003_14.h  test2003_25.h  test2005_133.h  test2005_157.h \
     test2005_159.h  test2005_160.h  test2005_75.h  test2005_88.h  test2006_42.h test2007_08.h test2007_41.h \
     test2007_42.h test2007_47.h test2007_48.h test2007_137.h test2007_160.h test2007_160a.h test2007_167.h \
     LogicalPath.cpp

# DQ (6/18/2008): All of these should be present...
# confdefs.h     test2001_11.h   test2001_18B.h  test2003_12.h  test2003_25.h   test2005_154.h  test2005_159.h  test2005_26.h  test2005_88.h    test2006_187b.h  test2006_42.h   test2007_113.h  test2007_137.h   test2007_167.h  test2007_42.h  test2007_97.h
# foo.h          test2001_15.h   test2003_08.h   test2003_14.h  test2004_40.h   test2005_157.h  test2005_15.h   test2005_75.h  test2006_136.h   test2006_33.h    test2007_08.h   test2007_114.h  test2007_160a.h  test2007_40.h   test2007_47.h
# test2001_05.h  test2001_18A.h  test2003_09.h   test2003_24.h  test2005_133.h  test2005_158.h  test2005_160.h  test2005_81.h  test2006_187a.h  test2006_36.h    test2007_101.h  test2007_136.h  test2007_160.h   test2007_41.h   test2007_48.h

check-local:
	@echo "Tests of C++."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Cxx_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC]
	rm -rf QMTest
