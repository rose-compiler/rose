include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

EXAMPLE_TESTCODES_REQUIRED_TO_PASS = \
   test2016_07.C \
   test2016_08.C \
   test2016_09.C \
   test2016_14.C \
   test2016_26.C \
   test2018_01.C

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
   test2016_40.C 

if USING_GNU_COMPILER
  EXAMPLE_TESTCODES_REQUIRED_TO_PASS += \
     test2016_03.C \
     test2016_11.C \
     test2016_25.C
else
if USING_CLANG_COMPILER
    EXAMPLE_TESTCODES_REQUIRED_TO_PASS += \
       test2016_03.C \
       test2016_11.C \
       test2016_25.C
else
    TESTCODE_CURRENTLY_FAILING += \
       test2016_03.C \
       test2016_11.C \
       test2016_25.C
endif
endif

if USING_INTEL_COMPILER
   TESTCODE_CURRENTLY_FAILING += \
     test2016_02.C \
     test2016_04.C \
     test2016_05.C \
     test2016_10.C \
     test2016_15.C \
     test2016_19.C \
     test2016_24.C
else
   EXAMPLE_TESTCODES_REQUIRED_TO_PASS += \
     test2016_02.C \
     test2016_04.C \
     test2016_05.C \
     test2016_10.C \
     test2016_15.C \
     test2016_19.C \
     test2016_24.C
endif

# DQ (11/19/2016): This test is failing in the unparsed code (a RAJA example) at least for the GNU 6.1 compiler and using EDG 4.11).
TESTCODE_CURRENTLY_FAILING += \
   test2016_06.C 

# Note: last.C just simplifies the Makefile.am it
# avoids common makefle errors in adding new files.

# This is the long test of ROSE (takes a while!)
TESTCODES_REQUIRED_TO_PASS = \
   $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
   $(TESTCODES_REQUIRED_TO_PASS) 

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
   $(TESTCODES_REQUIRED_TO_PASS) \
   $(TESTCODE_CURRENTLY_FAILING)

TESTCODE_INCLUDES =

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

#########
# Targets lists

PASSING_TEST_Source_passed = ${TESTCODES:.C=.C.passed}
TEST_Source_passed = ${ALL_TESTCODES:.C=.C.passed}

#########
# Translator to be used: calling "make TEST_TRANSLATOR=../../testBackend" uses the backend compiler only

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

../../testBackend:
	cd ../..; $(MAKE) testBackend

testCompiler:
	ln -s `which @BACKEND_CXX_COMPILER@` testCompiler

TEST_TRANSLATOR=../../testTranslator

#########
# ROSE_FLAGS

LANG_FLAGS = -std=c++14

ROSE_FLAGS =
if !ROSE_USE_CLANG_FRONTEND
   ROSE_FLAGS += --edg:no_warnings --edg:restrict
endif
ROSE_FLAGS += -w -rose:verbose 0 -rose:skip_unparse_asm_commands

#########
# RTH test targets

$(TEST_Source_passed): %.passed: $(srcdir)/% $(TEST_TRANSLATOR) $(top_srcdir)/scripts/test_exit_status
if !ROSE_USE_INSURE
	@$(RTH_RUN) \
	   CMD="$(TEST_TRANSLATOR) $(LANG_FLAGS) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.C.passed=.C)" \
	   TITLE="TEST: $(TEST_TRANSLATOR) $(@:.C.passed=.C)" \
	   $(top_srcdir)/scripts/test_exit_status $@
else
	$(VALGRIND) $(TEST_TRANSLATOR) $(LANG_FLAGS) $(ROSE_FLAGS)  $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.C.passed=.C)
endif

#
############

# Make sure that all the test codes end up in the distribution
EXTRA_DIST = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

print_testcodes:
	echo $(TESTCODES)

check-local:
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
if ROSE_USING_GCC_VERSION_LATER_4_9
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == true"
# Exclude these tests on GNU 4.9 and later compilers).
if ROSE_USING_GCC_VERSION_LATER_5_1
	@echo "ROSE_USING_GCC_VERSION_LATER_5_1 == true"
	@$(MAKE) $(PASSING_TEST_Source_passed)
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
#	@$(MAKE) $(PASSING_TEST_Source_passed)
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
	@$(MAKE) $(PASSING_TEST_Source_passed)
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
	@echo "*********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Cxx14_tests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************************"

clean-local:
	rm -f a.out *.o rose_*.[cC] roseTestProgram.C *.dot
	rm -f *.foldedConstants-o
	rm -f *.unfoldedConstants-o
	rm -rf QMTest
	rm -f *.int.c
	rm -f *.passed

status:
	@$(MAKE) $(TEST_Source_passed)

