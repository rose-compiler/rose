include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Import list of C++ test cases that should pass:
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT
#

if ROSE_USE_EDG_VERSION_4_3

EXAMPLE_TESTCODES_REQUIRED_TO_PASS = \
test_CplusplusMacro_Cpp.C \
test2001_01.C \
test2001_02.C \
test2001_03.C \
test2001_04.C \
test2001_05.C \
test2001_06.C \
test2001_07.C \
test2001_08.C \
test2001_09.C \
test2001_10.C \
test2001_11.C \
test2001_12.C \
test2001_13.C \
test2001_14.C \
test2001_15.C \
test2001_16.C \
test2001_17.C \
test2001_18.C \
test2001_19.C \
test2001_20.C \
test2001_21.C \
test2001_22.C \
test2001_23.C \
test2001_24.C \
test2001_25.C \
test2001_26.C \
test2001_27.C \
test2001_28.C \
test2001_29.C \
test2002_01.C \
test2003_01.C \
test2003_02.C \
test2003_03.C \
test2003_04.C \
test2003_05.C \
test2003_06.C \
test2003_08.C \
test2003_09.C \
test2003_10.C \
test2003_11.C \
test2003_12.C \
test2003_13.C \
test2003_14.C \
test2003_15.C \
test2003_16.C \
test2003_17.C \
test2003_18.C \
test2003_19.C \
test2003_20.C \
test2003_21.C \
test2003_23.C \
test2003_25.C \
test2003_26.C \
test2003_27.C \
test2003_28.C \
test2003_29.C \
test2003_30.C \
test2003_31.C \
test2003_32.C \
test2003_35.C \
test2003_36.C \
test2003_37.C \
test2003_38.C \
test2004_01.C \
test2004_02.C \
test2004_03.C \
test2004_04.C \
test2004_05.C \
test2004_06.C \
test2004_07.C \
test2004_08.C \
test2004_09.C \
test2004_10.C \
test2004_11.C \
test2004_12.C \
test2004_13.C \
test2004_14.C \
test2004_15.C \
test2004_16.C \
test2004_17.C \
test2004_18.C \
test2004_19.C \
test2004_23.C \
test2004_24.C \
test2004_26.C \
test2004_27.C \
test2004_28.C \
test2004_29.C \
test2004_30.C \
test2004_31.C \
test2004_32.C \
test2004_33.C \
test2004_34.C \
test2004_42.C \
test2004_44.C \
test2004_49.C \
test2004_50.C \
test2004_51.C \
test2004_52.C \
test2004_53.C \
test2004_54.C \
test2004_55.C \
test2004_56.C \
test2004_59.C \
test2004_60.C \
test2004_61.C \
test2004_62.C \
test2004_65.C \
test2004_71.C \
test2004_72.C \
test2004_73.C \
test2004_74.C \
test2004_87.C \
test2004_90.C \
test2004_93.C \
test2004_94.C \
test2004_95.C \
test2004_96.C \
test2004_97.C \
test2004_99.C \
test2011_149.C \
test2011_150.C \
test2011_151.C \
test2011_152.C \
test2011_153.C \
test2011_154.C \
test2011_155.C \
test2011_157.C \
test2011_158.C \
test2011_159.C \
test2011_160.C \
test2011_161.C \
test2011_162.C \
test2011_163.C \
test2011_164.C \
test2011_165.C \
test2011_166.C \
test2011_167.C \
test2011_168.C \
test2011_169.C \
test2011_170.C \
test2011_171.C \
test2011_172.C \
test2011_173.C \
test2011_174.C \
test2011_175.C \
test2011_176.C 


else
include $(srcdir)/Makefile-pass.inc
endif


# Import list of C++ test cases that are known to fail:
#   TESTCODE_CURRENTLY_FAILING
#
if ROSE_USE_EDG_VERSION_4_3
TESTCODE_CURRENTLY_FAILING = \
test2004_35.C \
test2004_36.C \
test2004_37.C \
test2004_38.C \
test2004_39.C \
test2004_40.C \
test2004_43.C \
test2004_45.C \
test2004_47.C \
test2004_48.C \
test2004_58.C \
test2004_63.C \
test2004_64.C \
test2004_67.C \
test2004_68.C \
test2004_69.C \
test2004_70.C \
test2004_75.C \
test2004_76.C \
test2004_77.C \
test2004_78.C \
test2004_79.C \
test2004_80.C \
test2004_81.C \
test2004_83.C \
test2004_84.C \
test2004_85.C \
test2004_86.C \
test2004_88.C \
test2004_89.C \
test2004_91.C \
test2004_98.C \
test2004_100.C \
test2011_156.C 

else
include $(srcdir)/Makefile-fail.inc
endif

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

# Some of these test codes reference A++ header fiels as part of their tests
# Include the path to A++ and the transformation specification
TESTCODE_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

# DQ (9/18/2011): Added testing for new option to use folded constants or the original express trees.
TEST_Objects_FoldedConstants = ${EXAMPLE_TESTCODES_REQUIRED_TO_PASS_FOLDED_CONSTANTS:.C=.foldedConstants-o}
TEST_Objects_UnfoldedConstants = ${ALL_TESTCODES:.C=.unfoldedConstants-o}

# VALGRIND_OPTIONS = -v --num-callers=12 --leak-check=yes --error-limit=no --show-reachable=yes
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

# A number of tests require the path to the A++ include directory 
# and a number of other tests require a path to the source directory.
# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.C)
$(TEST_Objects): ../../testTranslator
if !ROSE_USE_INSURE
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)
#	$(VALGRIND) ../../testParser $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)
#	$(VALGRIND) ../../testFrontEnd $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)
else
#	DQ (4/22/2011): For Insure++ we want to get the filename into the names used for the report files.
#	@if test ! -f "../../.libs/lt-testTranslator"; then echo "Can NOT find ../../.libs/lt-testTranslator"; ../../testTranslator --help > /dev/null; echo "Fixed"; fi
#	cp ../../.libs/lt-testTranslator testTranslator_$(@:.o=)
#	./testTranslator_$(@:.o=) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)
#	echo "Removing the just built copy of lt-testTranslator to save space..."
#	rm -f testTranslator_$(@:.o=)
	$(VALGRIND) ../../testTranslator $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)
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
test2005_56.o: ../../testTranslator
	../../testTranslator $(ROSE_FLAGS) -DTEST_STRING_MACRO=\""Thu Apr 14 08:18:33 PDT 2005"\" -c $(srcdir)/test2005_56.C -o test2005_56.o

test2010_01.o: ../../testTranslator
	echo "Running boost specific test."
	../../testTranslator $(ROSE_FLAGS) $(ROSE_INCLUDES) -c $(srcdir)/test2010_01.C -o test2010_01.o

test2010_02.o: ../../testTranslator
	echo "Running boost specific test."
	../../testTranslator $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2010_02.C -o test2010_02.o

test2010_07.o: ../../testTranslator
	echo "Running boost specific test."
#	../../testTranslator $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2010_07.C -o test2010_07.o
	../../testTranslator $(ROSE_FLAGS) -I$(top_builddir) -I$(top_srcdir)/src -I$(top_srcdir)/src/util -c $(srcdir)/test2010_07.C -o test2010_07.o

test2010_08.o: ../../testTranslator
	echo "Running boost specific test."
	../../testTranslator $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2010_08.C -o test2010_08.o

cpp_test2010_07: ../../testTranslator
	echo "Running boost specific test."
	g++ -E -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/test2010_07.C

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

../../testTranslatorFoldedConstants:
	cd ../..; $(MAKE) testTranslatorFoldedConstants

../../testTranslatorUnfoldedConstants:
	cd ../..; $(MAKE) testTranslatorUnfoldedConstants


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
     test2005_159.h  test2005_160.h  test2005_75.h  test2005_88.h  test2006_36.h test2006_42.h test2006_187a.h \
     test2006_187b.h test2007_08.h test2007_41.h \
     test2007_42.h test2007_47.h test2007_48.h test2007_97.h test2007_101.h test2007_136.h test2007_137.h test2007_160.h test2007_160a.h test2007_167.h \
     test2008_11.h LogicalPath.cpp test2005_56.C test2010_25.h

# DQ (6/18/2008): All of these should be present...
# confdefs.h     test2001_11.h   test2001_18B.h  test2003_12.h  test2003_25.h   test2005_154.h  test2005_159.h  test2005_26.h  test2005_88.h    test2006_187b.h  test2006_42.h   test2007_113.h  test2007_137.h   test2007_167.h  test2007_42.h  test2007_97.h
# foo.h          test2001_15.h   test2003_08.h   test2003_14.h  test2004_40.h   test2005_157.h  test2005_15.h   test2005_75.h  test2006_136.h   test2006_33.h    test2007_08.h   test2007_114.h  test2007_160a.h  test2007_40.h   test2007_47.h
# test2001_05.h  test2001_18A.h  test2003_09.h   test2003_24.h  test2005_133.h  test2005_158.h  test2005_160.h  test2005_81.h  test2006_187a.h  test2006_36.h    test2007_101.h  test2007_136.h  test2007_160.h   test2007_41.h   test2007_48.h

# This was a bug specific to the normalization of source file names to absolute paths
# DQ (12/11/2009): At the moment it is an failing test because "thisIsAnError" will be 
# interpreted to be a file.
testUsingBadCommandline: ../../testTranslator
	echo "class A { int x; float y;}; int main() { int x=0; return x; }" > roseTestProgram.C; \
	../../testTranslator -DthisIsOK -D thisIsAnError -rose:verbose 0 roseTestProgram.C

# This was a bug specific to the normalization of source file names to absolute paths
testBuildUsingNoPath: ../../testTranslator
	echo "class A { int x; float y;}; int main() { int x=0; return x; }" > roseTestProgram.C; \
	../../testTranslator -rose:verbose 0 roseTestProgram.C

# This was a bug specific to the normalization of source file names to absolute paths
testBuildUsingRelativePath: ../../testTranslator
	echo "class A { int x; float y;}; int main() { int x=0; return x; }" > roseTestProgram.C; \
	../../testTranslator -rose:verbose 0 ../Cxx_tests/roseTestProgram.C

# This was always worked but is the alternative to the normalization of source file names to absolute paths
testBuildUsingAbsolutePath: ../../testTranslator
	echo "class A { int x; float y;}; int main() { int x=0; return x; }" > roseTestProgram.C; \
	../../testTranslator -rose:verbose 0 `pwd`/roseTestProgram.C

# DQ (10/23/2010): Added MPI tests that I can at least run locally to debug when Hudson MPI tests fail.
MPI_LOCATION = /home/dquinlan/ROSE/ROSE_TESTS/MPI_header_tests/MPI_header_tests_64bit
test_MPI_LAM:
	../../testTranslator -rose:verbose 0 -I$(MPI_LOCATION)/LAM_MPI/include -c $(MPI_LOCATION)/tests/input.C

# DQ (7/24/2011): These test fail the AST IO but pass the compilation tests.  Debug these for AST File I/O separately.
failingAST_IO_tests: test2011_113.o test2011_114.o test2011_116.o

print_testcodes:
	echo $(TESTCODES)

check-local:
if !ROSE_USE_NEW_EDG_INTERFACE
#	@$(MAKE) testUsingBadCommandline
	@$(MAKE) testBuildUsingNoPath
	@$(MAKE) testBuildUsingRelativePath
	@$(MAKE) testBuildUsingAbsolutePath
	@echo "Tests of C++."
	@$(MAKE) failingAST_IO_tests
endif
	@$(MAKE) $(PASSING_TEST_Objects)
if !ROSE_USE_NEW_EDG_INTERFACE
#	DQ (9/18/2011): Added tests for folded constant option.
	@$(MAKE) $(TEST_Objects_FoldedConstants)
#	DQ (4/6/2010): This fails on Hudson, but I don't know why!
#	@$(MAKE) testParser
#	DQ (5/8/2011): Special tests are now moved to the end.
	@$(MAKE) test2005_56.o
	@$(MAKE) test2010_07.o
endif
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Cxx_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"

clean-local:
	rm -f a.out *.o rose_*.[cC] roseTestProgram.C *.dot
	rm -f *.foldedConstants-o
	rm -f *.unfoldedConstants-o
	rm -rf QMTest
	rm -f *.int.c
