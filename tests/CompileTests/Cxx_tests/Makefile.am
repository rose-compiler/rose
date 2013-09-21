include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/input_codes/cxx/Makefile_variables

# Import list of C++ test cases that should pass:
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT
#


# EXAMPLE_TESTCODES_REQUIRED_TO_PASS = \
#  $(TESTCODE_CURRENTLY_FAILING)

# EXAMPLE_TESTCODES_REQUIRED_TO_PASS = $(NEVER_PASING_TESTCODE_CURRENTLY_FAILING)

# TOO1 (2013-08-05): Refactored testcode specification into separate global file:
#
#                      $ROSE/tests/input_codes/cxx/Makefile_variables
#
#                  This way all $ROSE/tests can utilize the same growing suite
#                  of ROSE testcodes.
#
EXAMPLE_TESTCODES_REQUIRED_TO_PASS = $(ROSE__CXX_TESTS)

EXAMPLE_TESTCODES_REQUIRED_TO_PASS += \
   lulesh.C \
   luleshTALC.C \
   test2013_253.C \
   test2013_255.C \
   test2013_256.C \
   test2013_257.C \
   test2013_258.C \
   test2013_259.C \
   test2013_260.C

# DQ (9/10/2013): Test failing for switch without case statement (made up example, 
# but we will want it to pass as part of further switch statement work later).
# test2013_254.C

# DQ (7/31/2013): This is a failing test code for operator->().
# test2013_249.C 


# $(TESTCODE_CURRENTLY_FAILING)
# DQ (2/15/2012): These test codes fail on OSX only.
TESTCODE_CURRENTLY_FAILING_ON_OSX = \
test2011_06.C \
test2004_35.C 

if !ROSE_USE_EDG_VERSION_4_7
# DQ (6/12/2013): This fails to compile with EDG 4.7 (EDG complains: line 13: error: constructor or destructor may not have its address taken)
# Skip this test code while we report the problem to EDG (and investigate more in internally).
EXAMPLE_TESTCODES_REQUIRED_TO_PASS += test2012_68.C
endif

# DQ (5/26/2013): Currently faling tests:
CURRENTLY_FAILING_TESTS = \
test2013_195.C \
test2013_197.C

# DQ (7/3/2013): This is a failing Qt example code (unparsed code is not correct).
# test2013_245.C 


# DQ (5/23/2013): This is a failing test because we fixup the symbol tables and then compute 
# the name qualification (and with the fixed up symbol tables we get the name qualification wrong).
# Not clear how to fix this at the moment, it was a made up test that I will work on later.
# test2013_185.C 


# DQ (4/21/2013): failing regression tests (result of fix for operator vs. non-operator syntax in function calls).
# test2013_117.C 

# DQ (4/9/2013): failing regression tests (result of fix for operator vs. non-operator syntax in function calls).


# DQ (4/7/2013): new tests codes from Doxygen work, a few still fail.
# test2013_84.C
# test2013_85.C
# test2013_96.C


# DQ (2/16/2013): Failing test codes as a result of name qualification on friend class declaration
# test2013_62.C 


# DQ (1/21/2013): Failing test codes:
# test2013_13.C 
# test2013_14.C 
# test2013_15.C 
# test2013_19.C 
# test2013_30.C 
# test2013_35.C 
# test2013_40.C  // Demonstrates expression precedence problem.
# test2013_41.C  // Demonstrates expression precedence problem.
# test2013_42.C  // Demonstrates expression precedence problem.
# test2013_43.C  // Demonstrates expression precedence problem.


# Variation of regression test that fails.
# test2012_252.C 

# DQ (12/29/2011): This passes but I had to handle a template constant parameter incorrectly in the AST to get past a problem in EDG (or my lack of understanding of an aspect of EDG).
# test2011_191.C 
# test2012_79.C 
# test2012_81.C 

# DQ (11/3/2012): Tests failing after redesign of source position information.
# Accumulate these here until we see what size problem we have...(first two are the same issue)...
# test2005_06.C 
# test2012_57.C
# test2012_59.C

# TV (06/17/13): [TODO] This was used for EDG 3.3. We now have the long list in this file...
# include $(srcdir)/Makefile-pass.inc


# TV (08/02/12): Added a list of none previously referenced test codes, code to be sorted
TESTCODE_TO_BE_SORTED = \
test2011_178.C \
test2011_196.C \
test2011_197.C \
test2011_198.C \
test2011_199.C \
test2011_200.C \
test2011_201.C \
test2011_202.C \
test2011_203.C \
test2012_133.C \
test2012_134.C \
test2012_135.C \
test2012_35.C \
test2012_79.C

# Import list of C++ test cases that are known to fail:
#   TESTCODE_CURRENTLY_FAILING
#
TESTCODE_CURRENTLY_FAILING = 

# DQ (8/12/2012): This test code appears to demonstrate a loss of the template arguments in the name qualification.
# test2012_189.C

# This source files are listed as unused source files and thus violate 
# the unused source file ploicy so let's list them explicitly here:
UNMENTIONED_SOURCE_FILES = \
c++trick.C \
sgnodee.hpp \
test2006_136.h \
test2007_113.h \
test2007_114.h \
test2007_40.h \
test2007_95.C \
test2008_07.C \
test2008_15.C \
test2009_23.C \
test2009_26.C \
test2010_20.C \
test2010_21.C \
test2010_23.C \
test2011_102.C \
test2011_103.C \
test2011_104.C \
test2011_105.C \
test2011_106.C \
test2011_107.C \
test2011_108.C \
test2011_109.C \
test2011_112.C \
test2011_122.C \
test2011_123.C \
test2011_124.C \
test2011_125.C \
test2011_126.C \
test2011_127.C \
test2011_128.C \
test2011_129.C \
test2011_130.C \
test2011_131.C \
test2011_132.C \
test2011_134.C \
test2011_144.C \
test2011_145.C \
test2011_148.C \
test2012_139.C \
test2012_140.C \
test2012_178.h \
test2012_189.C \
test2013_13.C \
test2013_14.C \
test2013_15.C \
test2013_16.C \
test2013_17.C \
test2013_19.C \
test2013_24.C \
test2013_30.C \
test2013_35.C \
test2013_41.C \
test2013_42.C \
test2013_43.C \
test2013_45.C \
test2013_49.C \
test_gnu.C 




# DQ (8/6/2012): These are the previously failing test with the older version of ROSE (115 of the 183 old failing tests pass now: 68 left...).
PASSING_PREVIOUSLY_FAILING_TESTS = \
certSignalExample.C \
longFile.C \
test2005_151.C \
test2005_200.C \
test2005_204.C \
test2005_69.C \
test2005_85.C \
test2006_09.C \
test2006_127.C \
test2006_15.C \
test2006_167.C \
test2006_168.C \
test2006_169.C \
test2006_172.C \
test2006_174.C \
test2006_175.C \
test2006_176.C \
test2006_180.C \
test2006_182.C \
test2006_186.C \
test2006_188.C \
test2006_27.C \
test2006_64.C \
test2007_100.C \
test2007_104.C \
test2007_106.C \
test2007_107.C \
test2007_110.C \
test2007_111.C \
test2007_114.C \
test2007_116.C \
test2007_117.C \
test2007_118.C \
test2007_119.C \
test2007_120.C \
test2007_121.C \
test2007_122.C \
test2007_126.C \
test2007_127.C \
test2007_129.C \
test2007_131.C \
test2007_132.C \
test2007_134.C \
test2007_136.C \
test2007_143.C \
test2007_145.C \
test2007_148.C \
test2007_149.C \
test2007_156.C \
test2007_168.C \
test2007_174.C \
test2007_176.C \
test2007_18.C \
test2007_19.C \
test2007_20.C \
test2007_22.C \
test2007_27.C \
test2007_31.C \
test2007_32.C \
test2007_33.C \
test2007_34.C \
test2007_36.C \
test2007_37.C \
test2007_39.C \
test2007_45.C \
test2007_51.C \
test2007_52.C \
test2007_56.C \
test2007_58.C \
test2007_61.C \
test2007_63.C \
test2007_67.C \
test2007_68.C \
test2007_71.C \
test2007_73.C \
test2007_74.C \
test2007_76.C \
test2007_80.C \
test2007_86.C \
test2007_87.C \
test2007_88.C \
test2007_89.C \
test2007_91.C \
test2007_96.C \
test2007_97.C \
test2007_99.C \
test2009_07.C \
test2009_13.C \
test2009_22.C \
test2009_33.C \
test2009_34.C \
test2009_38.C \
test2009_44.C \
test2009_46.C \
test2009_47.C \
test2010_07.C \
test2010_09.C \
test2010_10.C \
test2010_11.C \
test2010_15.C



# DQ (8/6/2012): This is the list of test code that were failing before the move to EDG 4.3 (failing with the use of EDG 3.3)
# DQ (8/6/2012): Original count is 183 failing tests that have never passed previously.
NEVER_PASING_TESTCODE_CURRENTLY_FAILING = \
lexPhase2003_02.C \
test2004_25.C \
test2005_69.C \
test2005_85.C \
test2005_115.C \
test2005_151.C \
test2005_188.C \
test2005_200.C \
test2005_201.C \
test2005_204.C \
test2005_205.C \
test2006_09.C \
test2006_11.C \
test2006_15.C \
test2006_21.C \
test2006_24.C \
test2006_27.C \
test2006_33.C \
test2006_35.C \
test2006_36.C \
test2006_49.C \
test2006_60.C \
test2006_64.C \
test2006_127.C \
test2006_128.C \
test2006_130.C \
test2006_131.C \
test2006_138.C \
test2006_167.C \
test2006_168.C \
test2006_169.C \
test2006_170.C \
test2006_171.C \
test2006_172.C \
test2006_173.C \
test2006_174.C \
test2006_175.C \
test2006_176.C \
test2006_180.C \
test2006_182.C \
test2006_184.C \
test2006_186.C \
test2006_187.C \
test2006_188.C \
test2007_18.C \
test2007_19.C \
test2007_20.C \
test2007_22.C \
test2007_27.C \
test2007_30.C \
test2007_31.C \
test2007_32.C \
test2007_33.C \
test2007_34.C \
test2007_35.C \
test2007_36.C \
test2007_37.C \
test2007_39.C \
test2007_42.C \
test2007_45.C \
test2007_51.C \
test2007_52.C \
test2007_54.C \
test2007_56.C \
test2007_58.C \
test2007_60.C \
test2007_61.C \
test2007_63.C \
test2007_64.C \
test2007_65.C \
test2007_66.C \
test2007_67.C \
test2007_68.C \
test2007_69.C \
test2007_71.C \
test2007_73.C \
test2007_74.C \
test2007_76.C \
test2007_79.C \
test2007_80.C \
test2007_86.C \
test2007_87.C \
test2007_88.C \
test2007_89.C \
test2007_91.C \
test2007_92.C \
test2007_93.C \
test2007_96.C \
test2007_97.C \
test2007_99.C \
test2007_100.C \
test2007_101.C \
test2007_104.C \
test2007_105.C \
test2007_106.C \
test2007_107.C \
test2007_108.C \
test2007_109.C \
test2007_110.C \
test2007_111.C \
test2007_112.C \
test2007_113.C \
test2007_114.C \
test2007_115.C \
test2007_116.C \
test2007_117.C \
test2007_118.C \
test2007_119.C \
test2007_120.C \
test2007_121.C \
test2007_122.C \
test2007_123.C \
test2007_124.C \
test2007_125.C \
test2007_126.C \
test2007_127.C \
test2007_129.C \
test2007_131.C \
test2007_132.C \
test2007_134.C \
test2007_136.C \
test2007_138.C \
test2007_139.C \
test2007_143.C \
test2007_144.C \
test2007_145.C \
test2007_146.C \
test2007_148.C \
test2007_149.C \
test2007_153.C \
test2007_156.C \
test2007_168.C \
test2007_174.C \
test2007_175.C \
test2007_176.C \
test2009_07.C \
test2009_13.C \
test2009_17.C \
test2009_21.C \
test2009_22.C \
test2009_29.C \
test2009_32.C \
test2009_33.C \
test2009_34.C \
test2009_35.C \
test2009_36.C \
test2009_38.C \
test2009_43.C \
test2009_44.C \
test2009_45.C \
test2009_46.C \
test2009_47.C \
test2010_01.C \
test2010_02.C \
test2010_07.C \
test2010_08.C \
test2010_09.C \
test2010_10.C \
test2010_11.C \
test2010_12.C \
test2010_15.C \
test2010_16.C \
test2010_17.C \
test2010_22.C \
test2010_27.C \
test2011_01.C \
test2011_02.C \
test2011_05.C \
test2011_06.C \
test2011_07.C \
test2011_08.C \
test2011_09.C \
test2011_10.C \
test2011_11.C \
test2011_12.C \
test2011_13.C \
test2011_14.C \
test2011_15.C \
test2011_16.C \
test2011_17.C \
test2011_18.C \
longFile.C \
certSignalExample.C \
test2010_26.C



# DQ (2/19/2012): These appear to fail only sometimes and were noticed just before going on travel (test with valgrind).
STATISTICALLY_FAILING_TESTS = \
test2007_62.C \
math.C 

# DQ (5/17/2012): These are new tests which I deamed up and which fail.
# they likely failed in the older version of EDG support, but were not 
# tested there.  Since the current focus is on the previous level of
# robustness. These are considered to be NEW failing tests to work on 
# later (when the previously passing tests are fixed in the newer EDG 
# support).
NEW_FAILING_TESTS = \
test2012_86.C

# TV (06/17/13): [TODO] This was used for EDG 3.3. We now have the long list in this file...
# include $(srcdir)/Makefile-fail.inc

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

SKIP_UNPARSE_C_ASM_STATEMENTS = -rose:skip_unparse_asm_commands

# DQ (5/8/2011): Adding -rose:skipfinalCompileStep so that we can test the new name qualification support that is not yet hooked up to the unparser.
ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 $(SKIP_UNPARSE_C_ASM_STATEMENTS)
# ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 -rose:skipfinalCompileStep $(SKIP_UNPARSE_C_ASM_STATEMENTS)

# DQ (3/10/2013): Add the Boost path so that the case where Boost is not installed in /usr/include is covered.
# Some of these test codes reference A++ header files as part of their tests
# Include the path to A++ and the transformation specification
# TESTCODE_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code
# TESTCODE_INCLUDES = $(ROSE_BOOST_PREINCLUDE_PATH) -I$(top_srcdir)/tests/CompileTests/A++Code
# TESTCODE_INCLUDES = $(ROSE_BOOST_NORMAL_INCLUDE_PATH) -DBOOST_REGEX_MATCH_EXTRA -I$(top_srcdir)/tests/CompileTests/A++Code
TESTCODE_INCLUDES = $(ROSE_BOOST_NORMAL_INCLUDE_PATH) -DBOOST_REGEX_MATCH_EXTRA
APP_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code

PASSING_TEST_Objects = ${TESTCODES:.C=.o} ${ROSE__CXX_TESTS__BOOST:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

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
$(TEST_Objects): %.o: $(ROSE__CXX_TESTS_DIR)/%.C ../../testTranslator
if !ROSE_USE_INSURE
	$(VALGRIND) $(TEST_TRANSLATOR) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c "$<"
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

# This requires a special makefile rule
test2005_56.o: $(TEST_TRANSLATOR)
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\" -c $(srcdir)/test2005_56.C -o test2005_56.o

test2010_01.o: $(TEST_TRANSLATOR)
	echo "Running boost specific test."
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) $(ROSE_INCLUDES) -c $(srcdir)/test2010_01.C -o test2010_01.o

test2010_02.o: $(TEST_TRANSLATOR)
	echo "Running boost specific test."
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2010_02.C -o test2010_02.o

test2010_07.o: $(TEST_TRANSLATOR)
	echo "Running boost specific test."
#	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2010_07.C -o test2010_07.o
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) -I$(top_srcdir)/src -I$(top_srcdir)/src/util -c $(srcdir)/test2010_07.C -o test2010_07.o

test2010_08.o: $(TEST_TRANSLATOR)
	echo "Running boost specific test."
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2010_08.C -o test2010_08.o

$(ROSE__CXX_TESTS_DIR)/test2013_09.o: $(TEST_TRANSLATOR)
	echo "Running boost specific test."
#	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(APP_INCLUDES) $(ROSE_INCLUDES) -c $(srcdir)/test2010_08.C -o test2010_08.o
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(APP_INCLUDES) -c $(srcdir)/test2013_09.C -o test2013_09.o

cpp_test2010_07: $(TEST_TRANSLATOR)
	echo "Running boost specific test."
	g++ -E -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2010_07.C

$(ROSE__CXX_TESTS_DIR)/test2013_235.o: $(TEST_TRANSLATOR)
	echo "Running boost specific test."
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2013_235.C -o test2013_235.o

$(ROSE__CXX_TESTS_DIR)/test2013_236.o: $(TEST_TRANSLATOR)
	echo "Running boost specific test."
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2013_236.C -o test2013_236.o

# DQ (9/13/2013): Added new test for -rose:unparse_includes (which has a bug in how it unparses C++ style comments
# and should maybe not be unparsing declarations that are front-end specific (e.g. builtin function declarations).
test_unparse_includes_option: $(TEST_TRANSLATOR)
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -rose:unparse_includes -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2013_257.C -o test2013_257_unparse_headers.o

# DQ (9/16/2013): Test linking the resulting translated file. Phil Miller reported an error and the issue
# is that va_start() and va_end() must be unparsed to be __builtin_va_start() and __builtin_va_end() (at least for GNU).
test_unparse_includes_option_with_link: $(TEST_TRANSLATOR)
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -rose:unparse_includes -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES) $(srcdir)/test2013_260.C -o test2013_260_unparse_headers

# DQ (9/17/2013): Added autoconf style link test.
test_common_configure_test_with_link_part_1: $(TEST_TRANSLATOR)
	@cp $(srcdir)/conftest.C conftest.C
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -rose:verbose 0 -I$(srcdir) conftest.C -o conftest_configure_test_part_1

test_common_configure_test_with_link_part_2: $(TEST_TRANSLATOR)
	@cp $(srcdir)/test2013_261.C conftest.C
	$(TEST_TRANSLATOR) $(ROSE_FLAGS) -rose:verbose 0 -I$(srcdir) conftest.C -o conftest_configure_test_part_2

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

# This must be placed after the more general rule to work properly!
test2005_56.qmt: $(TEST_TRANSLATOR)
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/$(TEST_TRANSLATOR) NULL $(ROSE_FLAGS) -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\" -I$(srcdir) -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# Make sure that all the test codes end up in the distribution
EXTRA_DIST = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING) test_CplusplusMacro_C.C test2006_137.C \
     confdefs.h  test2001_05.h  test2001_11.h test2001_18A.h  test2003_08.h  test2003_12.h  test2003_24.h  test2004_40.h \
     test2005_154.h  test2005_158.h  test2005_15.h   test2005_26.h  test2005_81.h  test2006_33.h foo.h \
     test2001_15.h  test2001_18B.h  test2003_09.h  test2003_14.h  test2003_25.h  test2005_133.h  test2005_157.h \
     test2005_159.h  test2005_160.h  test2005_75.h  test2005_88.h  test2006_36.h test2006_42.h test2006_187a.h \
     test2006_187b.h test2007_08.h test2007_41.h  test2007_42.h test2007_47.h test2007_48.h test2007_97.h \
     test2007_101.h test2007_136.h test2007_137.h test2007_160.h test2007_160a.h test2007_167.h \
     test2008_11.h LogicalPath.cpp test2005_56.C test2010_25.h test2012_242.h test2012_268.h

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
	@$(MAKE) $(PASSING_TEST_Objects)
	@$(MAKE) test_unparse_includes_option
	@$(MAKE) test_unparse_includes_option_with_link
	@$(MAKE) test_common_configure_test_with_link_part_1
	@$(MAKE) test_common_configure_test_with_link_part_2
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Cxx_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"

clean-local:
	rm -f a.out *.o rose_*.[cC] roseTestProgram.C *.dot
	rm -f *.foldedConstants-o
	rm -f *.unfoldedConstants-o
	rm -rf QMTest
	rm -f *.int.c
	rm -f test2013_260_unparse_headers conftest_configure_test*

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
	@$(MAKE) $(PASSING_TEST_Objects:.o=.timed)

check-performance-timing-with-threshold:
	@$(MAKE) $(PASSING_TEST_Objects:.o=.timed_with_threshold)

$(PASSING_TEST_Objects:.o=.timed):
	rm -f $(@:.timed=.o) &&                                                        \
	$(top_srcdir)/scripts/timeout.sh $(TIMEOUT) $(MAKE) $(@:.timed=.o) ||          \
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
$(PASSING_TEST_Objects:.o=.timed_with_threshold):
	rm -f $(@:.timed_with_threshold=.o) &&                                                   \
	SECONDS=0 &&                                                                             \
			$(MAKE) TEST_TRANSLATOR=g++ ROSE_FLAGS=  $(@:.timed_with_threshold=.o) &&            \
	gnu_time="$$((($$SECONDS + 1)))" &&                                                      \
	echo "[INFO] '$@' compiled with GNU in '$$gnu_time' seconds" &&                          \
	rm -f $(@:.timed_with_threshold=.o) &&                                                   \
	TIMEOUT="$$((($$gnu_time * $(TIMEOUT_TOLERANCE))))" &&                                   \
	echo "[INFO] '$@' performance test is set to timeout after '$$TIMEOUT' seconds" &&       \
	$(MAKE) TIMEOUT="$$TIMEOUT" $(@:.timed_with_threshold=.timed)

