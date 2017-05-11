include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

EXAMPLE_TESTCODES_REQUIRED_TO_PASS = \
test2016_02.C \
test2016_04.C \
test2016_05.C \
test2016_07.C \
test2016_08.C \
test2016_09.C \
test2016_10.C \
test2016_14.C \
test2016_15.C \
test2016_19.C \
test2016_24.C \
test2016_26.C \
last.C

# DQ (2/21/2017): Some compilers are not handling the C++14 support (Intel v15 and earlier).
if USING_GNU_COMPILER
EXAMPLE_TESTCODES_REQUIRED_TO_PASS += test2016_03.C test2016_11.C test2016_25.C 
else # NOT USING_GNU_COMPILER
if USING_CLANG_COMPILER
EXAMPLE_TESTCODES_REQUIRED_TO_PASS += test2016_03.C test2016_11.C test2016_25.C 
else # NOT USING_CLANG_COMPILER
if USING_INTEL_COMPILER
# Intel v16 can not handle this C++14 support.
# EXAMPLE_TESTCODES_REQUIRED_TO_PASS += test2016_25.C 
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER

# These are incomplete non-compilable code fragments
TESTCODE_CURRENTLY_FAILING = \
test2016_01.C \
test2016_12.C \
test2016_13.C \
test2016_16.C \
test2016_17.C \
test2016_18.C \
test2016_20.C \
test2016_21.C \
test2016_22.C \
test2016_23.C \
test2017_01.C \
last.C

# DQ (11/19/2016): This test is failing in the unparsed code (a RAJA example) at least for the GNU 6.1 compiler and using EDG 4.11).
# test2016_40.C 
if ROSE_USE_EDG_VERSION_4_9
EXAMPLE_TESTCODES_REQUIRED_TO_PASS += \
   test2016_06.C 
endif

# Note: last.C just simplifies the Makefile.am it
# avoids common makefle errors in adding new files.

# This is the long test of ROSE (takes a while!)
TESTCODES_REQUIRED_TO_PASS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) 

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

SKIP_UNPARSE_C_ASM_STATEMENTS = -rose:skip_unparse_asm_commands

# Use options to turn on C++14 and process using just the frontend for now.
# CXX11_OPTIONS = -rose:Cxx11 -rose:skip_translation_from_edg_ast_to_rose_ast
CXX11_OPTIONS = -rose:Cxx14

# DQ (5/8/2011): Adding -rose:skipfinalCompileStep so that we can test the new name qualification support that is not yet hooked up to the unparser.
ROSE_FLAGS = $(CXX11_OPTIONS) --edg:no_warnings -w -rose:verbose 0 $(SKIP_UNPARSE_C_ASM_STATEMENTS)
# ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 -rose:skipfinalCompileStep $(SKIP_UNPARSE_C_ASM_STATEMENTS)

TESTCODE_INCLUDES =

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

# DQ (9/18/2011): Added testing for new option to use folded constants or the original express trees.
TEST_Objects_FoldedConstants = ${EXAMPLE_TESTCODES_REQUIRED_TO_PASS_FOLDED_CONSTANTS:.C=.foldedConstants-o}
TEST_Objects_UnfoldedConstants = ${ALL_TESTCODES:.C=.unfoldedConstants-o}

# VALGRIND_OPTIONS = -v --num-callers=12 --leak-check=yes --error-limit=no --show-reachable=yes
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
# VALGRIND =

# DQ (11/5/2016): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
VALGRIND = $(ADDRESS_SANITIZER_OPTIONS)
else
VALGRIND =
endif

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

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# Make sure that all the test codes end up in the distribution
EXTRA_DIST = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

print_testcodes:
	echo $(TESTCODES)

check-local:
# DQ (2/19/2017): NEW VERSION OF LOGIC
# DQ (2/18/2017): More specific logic required for portability.
# When using EDG 4.9 these files don't compile on later versions of GNU, CLANG, and Intel compilers 
# but we still want them tests as we transition to EDG 4.12 (where they all work fine independent of 
# the compiler vendor and GNU compiler version).
if ROSE_USE_EDG_VERSION_4_9
	@echo "ROSE_USE_EDG_VERSION_4_9 == true: C++14 support is not available with EDG 4.9"
else  # NOT ROSE_USE_EDG_VERSION_4_9
# Using EDG 4.12 or later (run all tests independent of the compiler).
	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
if ROSE_USING_GCC_VERSION_LATER_4_9
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == true"
# Exclude these tests on GNU 4.9 and later compilers).
if ROSE_USING_GCC_VERSION_LATER_5_1
	@echo "ROSE_USING_GCC_VERSION_LATER_5_1 == true"
	@$(MAKE) $(PASSING_TEST_Objects)
else # NOT ROSE_USING_GCC_VERSION_LATER_5_1
	@echo "ROSE_USING_GCC_VERSION_LATER_5_1 == true (requires GNU 5.1 or later version of GNU compiler)"
endif # ROSE_USING_GCC_VERSION_LATER_5_1
else  # NOT ROSE_USING_GCC_VERSION_LATER_4_9
# If this is not the CLANG or Intel compilers and an earlier version of GNU than GNU 4.9 compiler, so include these files.
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == false (requires GNU 5.1 or later version of GNU compiler)"
endif # ROSE_USING_GCC_VERSION_LATER_4_9
else  # NOT USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
# DQ (2/28/2017): Skip these tests until we can return to this problem later.
#	@$(MAKE) $(PASSING_TEST_Objects)
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
	@$(MAKE) $(PASSING_TEST_Objects)
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
endif # ROSE_USE_EDG_VERSION_4_9
	@echo "*********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Cxx14_tests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************************"


old_rule:
if !ROSE_USE_EDG_VERSION_4_9
if ROSE_USING_GCC_VERSION_LATER_5_2
	@echo "C++14 tests using GNU version 5.2 backend compiler."
	@$(MAKE) $(PASSING_TEST_Objects)
else
if USING_INTEL_COMPILER
	@echo "C++14 tests using Intel backend compiler."
	@$(MAKE) $(PASSING_TEST_Objects)
else
if USING_CLANG_COMPILER
	@echo "C++14 tests using Clang backend compiler."
	@$(MAKE) $(PASSING_TEST_Objects)
endif
endif
else
	@echo "C++14 tests require configuration with EDG version 4.12 and GNU version 5.2 or later backend compilers (compiler too old)."
endif
else
	@echo "C++14 tests require configuration with EDG version 4.12 and GNU version 5.2 or later backend compilers (EDG version too old)."
endif

clean-local:
	rm -f a.out *.o rose_*.[cC] roseTestProgram.C *.dot
	rm -f *.foldedConstants-o
	rm -f *.unfoldedConstants-o
	rm -rf QMTest
	rm -f *.int.c
