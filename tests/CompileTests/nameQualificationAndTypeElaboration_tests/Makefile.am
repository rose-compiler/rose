include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

######################################################
# These are C+ testcodes targeting aspects of both C #
# and C++ that can be compiled using a C++ compiler  #
######################################################

# DQ (2/10/2013): Fixed makefile to build required testTranslator before running tests.
# TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator 
TEST_TRANSLATOR = ../../testTranslator
$(TEST_TRANSLATOR):
	cd ../..; $(MAKE) testTranslator

TESTCODES_REQUIRED_TO_PASS = \
testNameQalTypeElab_01.C \
testNameQalTypeElab_02.C \
testNameQalTypeElab_03.C \
testNameQalTypeElab_04.C \
testNameQalTypeElab_05.C \
testNameQalTypeElab_06.C \
testNameQalTypeElab_07.C \
testNameQalTypeElab_08.C \
testNameQalTypeElab_09.C \
testNameQalTypeElab_10.C \
testNameQalTypeElab_11.C \
testNameQalTypeElab_12.C \
testNameQalTypeElab_13.C \
testNameQalTypeElab_14.C \
testNameQalTypeElab_15.C \
testNameQalTypeElab_16.C \
testNameQalTypeElab_17.C \
testNameQalTypeElab_18.C \
testNameQalTypeElab_22.C \
testNameQalTypeElab_23.C \
testNameQalTypeElab_24.C \
testNameQalTypeElab_25.C \
testNameQalTypeElab_26.C \
testNameQalTypeElab_27.C \
testNameQalTypeElab_28.C \
testNameQalTypeElab_29.C \
testNameQalTypeElab_30.C \
testNameQalTypeElab_31.C \
testNameQalTypeElab_32.C \
testNameQalTypeElab_33.C \
testNameQalTypeElab_34.C \
testNameQalTypeElab_35.C \
testNameQalTypeElab_36.C \
testNameQalTypeElab_37.C \
testNameQalTypeElab_38.C \
testNameQalTypeElab_39.C \
testNameQalTypeElab_40.C

# DQ (11/7/2007): These both work now!
# DQ (10/24/2007): This used to pass but not now!
# testNameQalTypeElab_17.C
# DQ (11/4/2007): This used to pass but not now (need to fix)!
# testNameQalTypeElab_22.C

# DQ (8/28/2006): Verified non-working code (some are here because they represent source positions tests)
TESTCODE_CURRENTLY_FAILING = \
testNameQalTypeElab_19.C \
testNameQalTypeElab_20.C \
testNameQalTypeElab_21.C

# DQ (9/6/2008): This should not be listed twice
# testNameQalTypeElab_35.C

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) 

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

EXTRA_DIST = CMakeLists.txt $(ALL_TESTCODES)

# File option to accumulate performance information about the compilation
PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/Cxx_ROSE_PERFORMANCE_DATA.csv

#ROSE_FLAGS = -rose:verbose 2 --edg:restrict --edg:export
#ROSE_FLAGS = -rose:verbose 2 --edg:restrict --edg:export $(PERFORMANCE_REPORT_OPTION)
# Suppress all warnings (in EDG: --no_warnings) (in g++: -w)
ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 $(PERFORMANCE_REPORT_OPTION)

## Include the path to A++ and the transformation specification
TESTCODE_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
#VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=yes --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
# VALGRIND = valgrind $(VALGRIND_OPTIONS)

# DQ (12/2/2006): This is an alternative to valgrind to test ROSE (from Jeremiah) (does not appear to work properly for me)
# VALGRIND = libtool gdb -batch -x $(srcdir)/../dump_crash_locs.gdb --args
VALGRIND =

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

# A number of tests require the path to the A++ include directory 
# and a number of other tests require a path to the source directory.
# $(TEST_Objects): preprocessor $(srcdir)/$(@:.o=.C)
$(TEST_Objects): $(TEST_TRANSLATOR)
	$(VALGRIND) $(TEST_TRANSLATOR) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)

CURRENT_DIRECTORY = `pwd`
QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
# A number of tests require the path to the A++ include directory 
# and a number of other tests require a path to the source directory.
# $(QMTEST_Objects): preprocessor $(srcdir)/$(@:.qmt=.C)
$(QMTEST_Objects): $(TEST_TRANSLATOR) 
#	@echo "Calling QMTEST_Objects rule for Cxx_tests directory: "
	qm.sh f rose.RoseTest $(TEST_TRANSLATOR) NULL $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.qmt=.C)

# This must be placed after the more general rule to work properly!
test2005_56.qmt: $(TEST_TRANSLATOR)
	qm.sh f rose.RoseTest $(TEST_TRANSLATOR) NULL $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\" -I$(srcdir) -c $(srcdir)/$(@:.qmt=.C)
#	$(VALGRIND) ../preprocessor -rose:verbose 2 -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\" $(srcdir)/test2005_56.C -o test2005_56

# This is a rule to add the current files into a QMTest database
build_qmtest_database: $(QMTEST_Objects)
#	@echo "Building QMTest database using file "
	rm -rf QMTest;
	mkdir -p QMTest;
	rm -rf QMTest/*.qmt
	mv *.qmt QMTest
	cd QMTest; qmtest -D`pwd` create-tdb

run_qm_tests:
	cd QMTest; qmtest run -o TestResults.qmr -j4

run_qm_gui:
	cd QMTest; qmtest gui

check-local:
	@echo "Development tests."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*******************************************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/nameQualificationAndTypeElaboration_tests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.pdf *~ *.ps *.out X rose_performance_report_lockfile.lock
	rm -rf QMTest


gprof: $(TEST_TRANSLATOR) $(srcdir)/test2001_11.C
#	gprof ../preprocessor $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/test2001_11.C

