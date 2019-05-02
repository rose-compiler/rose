include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

EXAMPLE_TESTCODES_REQUIRED_TO_PASS = \
   test2018_01.C \
   test2018_04.C \
   test2018_19.C

TESTCODE_CURRENTLY_FAILING = \
   test2018_02.C \
   test2018_05.C \
   test2018_06.C \
   test2018_07.C \
   test2018_08.C \
   test2018_09.C \
   test2018_10.C \
   test2018_11.C \
   test2018_13.C \
   test2018_14.C \
   test2018_15.C \
   test2018_16.C \
   test2018_17.C \
   test2018_18.C \
   test2018_20.C \
   test2018_21.C \
   test2018_22.C \
   test2018_23.C \
   test2018_24.C \
   test2018_25.C \
   test2018_26.C \
   test2018_27.C \
   test2018_28.C \
   test2018_29.C \
   test2018_30.C \
   test2018_31.C \
   test2018_32.C \
   test2018_33.C \
   test2018_34.C \
   test2018_35.C \
   test2018_36.C \
   test2018_37.C \
   test2018_38.C \
   test2018_39.C \
   test2018_40.C \
   test2018_41.C \
   test2018_42.C \
   test2018_43.C \
   test2018_44.C \
   test2018_45.C \
   test2018_46.C \
   test2018_47.C \
   test2018_48.C \
   test2018_49.C \
   test2018_50.C \
   test2018_51.C \
   test2018_52.C \
   test2018_53.C \
   test2018_54.C \
   test2018_55.C \
   test2018_56.C \
   test2018_57.C \
   test2018_58.C \
   test2018_59.C \
   test2018_60.C \
   test2018_61.C \
   test2018_62.C \
   test2018_63.C \
   test2018_64.C \
   test2018_65.C \
   test2018_66.C \
   test2018_67.C \
   test2018_68.C \
   test2018_69.C \
   test2018_70.C \
   test2018_71.C \
   test2018_72.C \
   test2018_73.C

if USING_GNU_COMPILER
#  EXAMPLE_TESTCODES_REQUIRED_TO_PASS +=
else
if USING_CLANG_COMPILER
#    EXAMPLE_TESTCODES_REQUIRED_TO_PASS +=
else
#    TESTCODE_CURRENTLY_FAILING +=
endif
endif

if USING_INTEL_COMPILER
   TESTCODE_CURRENTLY_FAILING += \
     test2018_03.C \
     test2018_12.C
else
   EXAMPLE_TESTCODES_REQUIRED_TO_PASS += \
     test2018_03.C \
     test2018_12.C
endif

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

LANG_FLAGS = -std=c++17

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
endif # ROSE_USE_EDG_VERSION_4_9
	@echo "*********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Cxx17_tests: make check rule complete (terminated normally) ******"
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

