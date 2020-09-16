include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)


TESTCODES_REQUIRED_TO_PASS = \
   test2020_03.C 

TESTCODE_CURRENTLY_FAILING = \
   test2020_01.C \
   test2020_02.C 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
   $(TESTCODES_REQUIRED_TO_PASS) 

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
   $(TESTCODES_REQUIRED_TO_PASS) \
   $(TESTCODE_CURRENTLY_FAILING)


TESTCODE_INCLUDES =

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
# VALGRIND =

# DQ (11/5/2016): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
VALGRIND = $(NON_RTH_ADDRESS_SANITIZER_OPTIONS)
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

TEST_TRANSLATOR=../../testTranslator

#########
# ROSE_FLAGS

# LANG_FLAGS = -std=c++11
LANG_FLAGS = -std=c++03

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
EXTRA_DIST = $(TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

check-local:
if USING_GNU_COMPILER
if ROSE_USING_GCC_VERSION_LATER_4_8
	@echo "C++03 tests require configuration with either EDG version 4.9 and GNU version 4.8 or backend compiler or EDG 4.12 and 4.8 or any later GNU compiler."
	@echo "C++03 tests using EDG 4.12 and any supported backend compiler are allowed."
	@$(MAKE) $(PASSING_TEST_Source_passed)
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
	@echo "C++03 tests not allowed with EDG 4.9: Clang backend compiler using EDG 4.12 or greater frontend."
# DQ (3/22/2020): Builtins are now mostly supported, allowing tests so that we can debug Mac support during COVID-19 work at home.
# DQ (2/21/2017): Too many failing files due to builtin functions that are missing.
#	@$(MAKE) $(PASSING_TEST_Objects)
	@$(MAKE) $(PASSING_TEST_Source_passed)
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
	@echo "C++03 tests using EDG 4.12 Intel backend compiler are allowed."
	@$(MAKE) $(PASSING_TEST_Source_passed)
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
	@echo "*********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Cxx03_tests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************************"


clean-local:
	rm -f a.out *.o rose_*.[cC] roseTestProgram.C *.dot
	rm -rf QMTest
	rm -f *.int.c
	rm -f *.failed *.passed

status:
	@$(MAKE) $(TEST_Source_passed)

