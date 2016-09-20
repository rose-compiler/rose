include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


EXAMPLE_TESTCODES_REQUIRED_TO_PASS = \
test2012_01.C \
test2012_02.C \
test2012_03.C \
test2012_04.C \
test2012_05.C \
test2012_06.C \
test2012_10.C \
test2012_11.C \
test2012_12.C \
test2012_13.C \
test2012_15.C \
test2012_16.C \
test2012_19.C \
test2012_21.C \
test2012_22.C \
test2012_23.C \
test2012_24.C \
test2012_25.C \
test2012_26.C \
test2012_27.C \
test2012_28.C \
test2012_29.C \
test2012_30.C \
test2012_31.C \
test2012_32.C \
test2012_33.C \
test2012_34.C \
test2012_36.C \
test2012_37.C \
test2012_38.C \
test2012_39.C \
test2012_40.C \
test2012_41.C \
test2012_42.C \
test2012_43.C \
test2012_44.C \
test2012_45.C \
test2012_46.C \
test2012_47.C \
test2012_48.C \
test2012_49.C \
test2012_50.C \
test2012_51.C \
test2012_52.C \
test2012_53.C \
test2012_54.C \
test2012_55.C \
test2012_56.C \
test2012_57.C \
test2012_58.C \
test2012_59.C \
test2012_60.C \
test2012_61.C \
test2012_62.C \
test2012_63.C \
test2012_64.C \
test2012_65.C \
test2012_66.C \
test2012_67.C \
test2012_69.C \
test2012_70.C \
test2012_71.C \
test2012_72.C \
test2012_73.C \
test2012_76.C \
test2012_77.C \
test2012_78.C \
test2012_80.C \
test2012_81.C \
test2012_82.C \
test2012_83.C \
test2012_84.C \
test2012_85.C \
test2012_87.C \
test2012_88.C \
test2012_91.C \
test2012_92.C \
test2012_93.C \
test2012_94.C \
test2012_95.C \
test2012_96.C \
test2012_97.C \
test2012_98.C \
test2013_01.C \
test2013_02.C \
test2013_03.C \
test2013_04.C \
test2013_05.C \
test2013_06.C \
test2013_08.C \
test2013_10.C \
test2013_11.C \
test2013_12.C \
test2013_16.C \
test2013_17.C \
test2013_18.C \
test2013_20.C \
test2013_21.C \
test2013_22.C \
test2013_23.C \
test2013_25.C \
test2013_26.C \
test2013_27.C \
test2013_28.C \
test2013_29.C \
test2013_31.C \
test2013_32.C \
test2013_34.C \
test2013_36.C \
test2013_37.C \
test2013_44.C \
test2013_46.C \
test2013_47.C \
test2013_48.C \
test2013_50.C \
test2013_51.C \
test2013_52.C \
test2013_53.C \
test2013_54.C \
test2013_55.C \
test2013_59.C \
test2013_60.C \
test2013_61.C \
test2013_64.C \
test2013_65.C \
test2013_66.C \
test2013_67.C \
test2013_69.C \
test2013_70.C \
test2013_71.C \
test2013_72.C \
test2013_73.C \
test2013_74.C \
test2013_75.C \
test2013_77.C \
test2013_78.C \
test2013_79.C \
test2013_80.C \
test2013_81.C \
test2013_82.C \
test2013_83.C \
test2013_85.C \
test2013_86.C \
test2013_87.C \
test2013_88.C \
test2013_89.C \
test2013_90.C \
test2013_91.C \
test2013_92.C \
test2013_93.C \
test2013_94.C \
test2013_95.C \
test2014_01.C \
test2014_02.C \
test2014_03.C \
test2014_04.C \
test2014_05.C \
test2014_06.C 

if ROSE_USE_MICROSOFT_EXTENSIONS
# Linux support does not include Microsoft support (at the same time), so these 
# tests fail unless we have configured with --enable-microsoft-extensions.
EXAMPLE_TESTCODES_REQUIRED_TO_PASS += \
test2014_07.C \
test2014_08.C \
test2014_09.C \
test2014_10.C \
test2014_11.C \
test2014_14.C \
test2014_15.C \
test2014_17.C \
test2014_18.C \
test2014_23.C \
test2014_12_nouuid.C \
test2014_13_nouuid.C \
test2014_16_nouuid.C \
test2014_19_nouuid.C \
test2014_20_nouuid.C \
test2014_21_nouuid.C \
test2014_22_nouuid.C 


if ROSE_USE_EDG_VERSION_4_9
# These test code require EDG version 4.9 for the better Microsoft support for the declspec(uuid).
EXAMPLE_TESTCODES_REQUIRED_TO_PASS += \
test2014_12.C \
test2014_13.C \
test2014_16.C \
test2014_19.C \
test2014_20.C \
test2014_21.C \
test2014_22.C 
endif

endif

# Waiting for commit from Zack.
# hello.C

TESTCODE_CURRENTLY_FAILING = 


# DQ (8/6/2012): This is the list of test code that were failing before the move to EDG 4.3 (failing with the use of EDG 3.3)
# DQ (8/6/2012): Original count is 183 failing tests that have never passed previously.
NEVER_PASING_TESTCODE_CURRENTLY_FAILING = 

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

# DQ (2/4/2014): Unparse the asm commands as a test.
SKIP_UNPARSE_C_ASM_STATEMENTS = -rose:skip_unparse_asm_commands
# SKIP_UNPARSE_C_ASM_STATEMENTS = 

# DQ (8/18/2014): When unparsing Microsoft extensions we can't pass this the generated code to the GNU g++ compiler.
# DQ (5/8/2011): Adding -rose:skipfinalCompileStep so that we can test the new name qualification support that is not yet hooked up to the unparser.
# ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 $(SKIP_UNPARSE_C_ASM_STATEMENTS)
# ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 -rose:skipfinalCompileStep $(SKIP_UNPARSE_C_ASM_STATEMENTS)
# ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 $(SKIP_UNPARSE_C_ASM_STATEMENTS) -pedantic-errors -std=c++98
ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 -rose:skipfinalCompileStep $(SKIP_UNPARSE_C_ASM_STATEMENTS) -pedantic-errors -std=c++98

# DQ (3/10/2013): Add the Boost path so that the case where Boost is not installed in /usr/include is covered.
# Some of these test codes reference A++ header files as part of their tests
# Include the path to A++ and the transformation specification
# TESTCODE_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code
# TESTCODE_INCLUDES = $(ROSE_BOOST_PREINCLUDE_PATH) -I$(top_srcdir)/tests/CompileTests/A++Code
# TESTCODE_INCLUDES = $(ROSE_BOOST_NORMAL_INCLUDE_PATH) -I$(top_srcdir)/tests/CompileTests/A++Code
TESTCODE_INCLUDES = $(ROSE_BOOST_NORMAL_INCLUDE_PATH)
APP_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code

PASSING_TEST_Objects = ${TESTCODES:.C=.o} ${ROSE__CXX_TESTS__BOOST:.C=.o}
PASSING_TEST_Sources = ${TESTCODES:.C=.C.passed} ${ROSE__CXX_TESTS__BOOST:.C=.C.passed}
TEST_Objects = ${ALL_TESTCODES:.C=.o}
TEST_Sources = ${ALL_TESTCODES:.C=.C.passed}

# DQ (9/18/2011): Added testing for new option to use folded constants or the original express trees.
TEST_Objects_FoldedConstants = ${EXAMPLE_TESTCODES_REQUIRED_TO_PASS_FOLDED_CONSTANTS:.C=.foldedConstants-o}
TEST_Objects_UnfoldedConstants = ${ALL_TESTCODES:.C=.unfoldedConstants-o}

# VALGRIND_OPTIONS = -v --num-callers=12 --leak-check=yes --error-limit=no --show-reachable=yes
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

TEST_TRANSLATOR = ../../testTranslator

# A number of tests require the path to the A++ include directory 
# and a number of other tests require a path to the source directory.
# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): $(TEST_TRANSLATOR) $(srcdir)/$(@:.o=.C)
# $(TEST_Objects):
# $(TEST_Objects): %.o: $(ROSE__CXX_TESTS_DIR)/%.C ../../testTranslator
# if !ROSE_USE_INSURE
# 	$(VALGRIND) $(TEST_TRANSLATOR) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c "$<"
# #	$(VALGRIND) ../../testParser $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)
# #	$(VALGRIND) ../../testFrontEnd $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)
# else
# #	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
# #	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; $(TEST_TRANSLATOR) --help > /dev/null; echo "Fixed"; fi
# #	cp ../../.libs/lt-testTranslator testTranslator_$(@:.o=)
# #	./testTranslator_$(@:.o=) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)
# #	echo "Removing the just built copy of lt-testTranslator to save space..."
# #	rm -f testTranslator_$(@:.o=)
# 	$(VALGRIND) $(TEST_TRANSLATOR) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(@:.o=.C)
# endif

# $(TEST_Sources_passed): %.passed: $(TEST_TRANSLATOR) $(top_srcdir)/scripts/test_exit_status
# $(TEST_Sources): %.passed: $(ROSE__CXX_TESTS_DIR)/%.C ../../testTranslator
$(TEST_Sources): %.passed: $(TEST_TRANSLATOR)
if !ROSE_USE_INSURE
#	$(VALGRIND) $(TEST_TRANSLATOR) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c "$<"
	@$(RTH_RUN) CMD="$(TEST_TRANSLATOR) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.C.passed=.C)" $(top_srcdir)/scripts/test_exit_status $@
#	$(VALGRIND) ../../testParser $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)
#	$(VALGRIND) ../../testFrontEnd $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)
else
#	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
#	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; $(TEST_TRANSLATOR) --help > /dev/null; echo "Fixed"; fi
#	cp ../../.libs/lt-testTranslator testTranslator_$(@:.o=)
#	./testTranslator_$(@:.o=) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)
#	echo "Removing the just built copy of lt-testTranslator to save space..."
#	rm -f testTranslator_$(@:.o=)
	$(VALGRIND) $(TEST_TRANSLATOR) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(@:.o=.C)
endif

# DQ (9/18/2011): This is testing the option to process the AST to use the constant folded values (contained in the AST).
$(TEST_Objects_FoldedConstants): ../../testTranslatorFoldedConstants
	$(VALGRIND) ../../testTranslatorFoldedConstants $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.foldedConstants-o=.C) && touch $@
#	$(VALGRIND) ../../testFrontEnd $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)

# DQ (9/18/2011): This is the equivalent to the default using testTranslator (so not critical to run this).
$(TEST_Objects_UnfoldedConstants): ../../testTranslatorUnfoldedConstants
	$(VALGRIND) ../../testTranslatorUnfoldedConstants $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.unfoldedConstants-o=.C) && touch $@

# DQ (4/6/2010): Added test for parsing only (tested here for C++ code)
testParser:../../testParser
	$(VALGRIND) ../../testParser $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/test2001_01.C

$(TEST_TRANSLATOR):
	cd ../..; $(MAKE) testTranslator

../../testTranslatorFoldedConstants:
	cd ../..; $(MAKE) testTranslatorFoldedConstants

../../testTranslatorUnfoldedConstants:
	cd ../..; $(MAKE) testTranslatorUnfoldedConstants


CURRENT_DIRECTORY = `pwd`
QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

$(QMTEST_Objects): $(TEST_TRANSLATOR)
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/$(TEST_TRANSLATOR) NULL $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# Make sure that all the test codes end up in the distribution
EXTRA_DIST = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)

# DQ (6/18/2008): All of these should be present...
# confdefs.h     test2001_11.h   test2001_18B.h  test2003_12.h  test2003_25.h   test2005_154.h  test2005_159.h  test2005_26.h  test2005_88.h    test2006_187b.h  test2006_42.h   test2007_113.h  test2007_137.h   test2007_167.h  test2007_42.h  test2007_97.h
# foo.h          test2001_15.h   test2003_08.h   test2003_14.h  test2004_40.h   test2005_157.h  test2005_15.h   test2005_75.h  test2006_136.h   test2006_33.h    test2007_08.h   test2007_114.h  test2007_160a.h  test2007_40.h   test2007_47.h
# test2001_05.h  test2001_18A.h  test2003_09.h   test2003_24.h  test2005_133.h  test2005_158.h  test2005_160.h  test2005_81.h  test2006_187a.h  test2006_36.h    test2007_101.h  test2007_136.h  test2007_160.h   test2007_41.h   test2007_48.h

# This was a bug specific to the normalization of source file names to absolute paths
# DQ (12/11/2009): At the moment it is an failing test because "thisIsAnError" will be 
# interpreted to be a file.
testUsingBadCommandline: $(TEST_TRANSLATOR)
	echo "class A { int x; float y;}; int main() { int x=0; return x; }" > roseTestProgram.C; \
	$(TEST_TRANSLATOR) -DthisIsOK -D thisIsAnError -rose:verbose 0 roseTestProgram.C

# This was a bug specific to the normalization of source file names to absolute paths
testBuildUsingNoPath: $(TEST_TRANSLATOR)
	echo "class A { int x; float y;}; int main() { int x=0; return x; }" > roseTestProgram.C; \
	$(TEST_TRANSLATOR) -rose:verbose 0 roseTestProgram.C

# This was a bug specific to the normalization of source file names to absolute paths
testBuildUsingRelativePath: $(TEST_TRANSLATOR)
	echo "class A { int x; float y;}; int main() { int x=0; return x; }" > roseTestProgram.C; \
	$(TEST_TRANSLATOR) -rose:verbose 0 ../Cxx_tests/roseTestProgram.C

# This was always worked but is the alternative to the normalization of source file names to absolute paths
testBuildUsingAbsolutePath: $(TEST_TRANSLATOR)
	echo "class A { int x; float y;}; int main() { int x=0; return x; }" > roseTestProgram.C; \
	$(TEST_TRANSLATOR) -rose:verbose 0 `pwd`/roseTestProgram.C

test_msvc_1:
#	@$(RTH_RUN) CMD="$(TEST_TRANSLATOR) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.C.passed=.C)" $(top_srcdir)/scripts/test_exit_status $@
#	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -c $(srcdir)/cl_preprocessed_hello2.cxx
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -c $(srcdir)/cl_preprocessed_hello2_noline.cxx


# DQ (10/23/2010): Added MPI tests that I can at least run locally to debug when Hudson MPI tests fail.
MPI_LOCATION = /home/dquinlan/ROSE/ROSE_TESTS/MPI_header_tests/MPI_header_tests_64bit
test_MPI_LAM:
	$(TEST_TRANSLATOR) -rose:verbose 0 -I$(MPI_LOCATION)/LAM_MPI/include -c $(MPI_LOCATION)/tests/input.C

test_multiple_files: $(srcdir)/test2012_246a.C $(srcdir)/test2012_246b.C
	../../testTranslator -rose:verbose 1 -c $(srcdir)/test2012_246a.C $(srcdir)/test2012_246b.C 

# DQ (7/24/2011): These test fail the AST IO but pass the compilation tests.  Debug these for AST File I/O separately.
failingAST_IO_tests: test2011_113.o test2011_114.o test2011_116.o

print_testcodes:
	echo $(TESTCODES)

# Added performance test for ROSE evaluation:
performance_timing:
	../../testGraphGeneration --edg:no_warnings -w -rose:verbose 3 -rose:skip_unparse_asm_commands -rose:skipAstConsistancyTests -rose:skip_commentsAndDirectives -rose:skip_unparse -I$(srcdir) -c $(srcdir)/testPerformance.C

# DQ (11/19/2012): HPCToolKit use.  This makes some significant assumptions on how we have setup HPCToolKit.
hpctoolkit:
	hpcrun -e WALLCLOCK -- ../../testTranslator --edg:no_warnings -w -rose:verbose 0 -rose:skip_unparse_asm_commands -rose:skipAstConsistancyTests -rose:skip_commentsAndDirectives -rose:skip_unparse -I/home/dquinlan/ROSE/git-dq-edg4x-rc/tests/CompileTests/Cxx_tests -c /home/dquinlan/ROSE/git-dq-edg4x-rc/tests/CompileTests/Cxx_tests/testPerformance.C
#	hpcprof hpctoolkit-lt-testTranslator-measurements/lt-testTranslator-000000-000-0f802c45-7612-0.hpcrun
	hpcprof hpctoolkit-lt-testTranslator-measurements/lt-testTranslator-000000-000-0f802c45-3676-0.hpcrun
#	hpcviewer /home/dquinlan/ROSE/ROSE_CompileTree/git-LINUX-64bit-4.2.4-dq-edg4x-rc-debug_mode_off/tests/CompileTests/Cxx_tests/hpctoolkit-lt-testTranslator-database-11205 &


check-local:
if ROSE_USING_GCC_VERSION_LATER_4_4
	@$(MAKE) $(PASSING_TEST_Sources)
else
	@echo "Microsoft C++ tests require GNU version 4.4 (or greater) backend compilers."
endif
	@echo "****************************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/MicrosoftWindows_Cxx_tests: make check rule complete (terminated normally) ******"
	@echo "****************************************************************************************************************"

clean-local:
	rm -f a.out *.o rose_*.[cC] roseTestProgram.C *.dot *.ti
	rm -f *.foldedConstants-o
	rm -f *.unfoldedConstants-o
	rm -rf QMTest
	rm -f *.int.c
	rm -f test2013_260_unparse_headers conftest_configure_test*
	rm -f conftest.C
	rm -f *.passed *.failed *.err

#------------------------------------------------------------------------------
#                           Performance Timing Tests
#------------------------------------------------------------------------------
## Purpose
#
#  Enforce a time limit constraint on testcode compilation.
#
### Make Targets
#
#  Note: A testcode's object files are removed before running the individual
#  steps in these make targets.
#
#### Default
#
#  $ make <testcode>.timed
#
#    Compiles <testcode> with a $TIMEOUT limit
#
#    E.g.  $ make test2005_35.timed
#
#  $ make check-performance-timing
#
#    Compiles all testcodes with a $TIMEOUT limit
#
#### Threshold
#
#  $ make <testcode>.timed_with_threshold
#
#    Compiles <testcode> with g++ and then uses the g++ compile time
#    as a $TIMEOUT threshold to compile <testcode> with ROSE.
#
#    E.g.  $ make test2005_35.timed_with_threshold
#
#  $ make check-performance-timing-with-threshold
#
#    Compiles all testcodes with a $TIMEOUT limit calculated from
#    the g++ compile time threshold.
#
### Environment Variables
#
#  TIMEOUT              Maximum time limit for the compilation of a <testcode>
#  TIMEOUT_TOLERANCE    $gnu_time * $TIMEOUT_TOLERANCE (threshold tests only)
#------------------------------------------------------------------------------

# DQ (6/30/2013): Increased this to support compiling Boost example tests. We 
# need to rethink how we want to handl this. I suggest restricting this to a 
# few tests instead of each test. Compiling with optimization also makes about
# a factor of four difference for test2013_241.C (boost example test code).
# Since Jenkins testing is currently done without optimization we need the 
# larger timeout value just to handle the newer boost test codes.
# TIMEOUT            ?= 60  # Default timeout value
TIMEOUT            ?= 420  # Default timeout value
TIMEOUT_TOLERANCE  ?= 20  # Default timeout tolerance value for threshold
                          # tests = $gnu_time * $TIMEOUT_TOLERANCE

check-performance-timing:
	@$(MAKE) $(TEST_Sources:.C.passed=.timed)

check-performance-timing-with-threshold:
	@$(MAKE) $(TEST_Sources:.C.passed=.timed_with_threshold)

$(TEST_Sources:.C.passed=.timed):
	rm -f $(@:.timed=.C.passed) &&                                                 \
	$(top_srcdir)/scripts/timeout.sh $(TIMEOUT) $(MAKE) $(@:.timed=.C.passed) ||   \
			(                                                                          \
				test $$? -eq 124 &&                                                      \
				echo "[ERROR] '$@' compilation timed out after '$(TIMEOUT)' seconds" &&  \
				exit 1                                                                   \
			)

# 1. Time and Compile testcode with g++
# 2. Compile with ROSE, using TIMEOUT relative to time results from g++
#
# Uses $(TESTCODES_REQUIRED_TO_PASS)
#
$(TEST_Sources:.C.passed=.timed_with_threshold):
	rm -f $(@:.timed_with_threshold=.C.passed) &&                                            \
	SECONDS=0 &&                                                                             \
			$(MAKE) TEST_TRANSLATOR=g++ ROSE_FLAGS=  $(@:.timed_with_threshold=.C.passed) &&     \
	gnu_time="$$((($$SECONDS + 1)))" &&                                                      \
	echo "[INFO] '$@' compiled with GNU in '$$gnu_time' seconds" &&                          \
	rm -f $(@:.timed_with_threshold=.C.passed) &&                                            \
	TIMEOUT="$$((($$gnu_time * $(TIMEOUT_TOLERANCE))))" &&                                   \
	echo "[INFO] '$@' performance test is set to timeout after '$$TIMEOUT' seconds" &&       \
	$(MAKE) TIMEOUT="$$TIMEOUT" $(@:.timed_with_threshold=.timed)

