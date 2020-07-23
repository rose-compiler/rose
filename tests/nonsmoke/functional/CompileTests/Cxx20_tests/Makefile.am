include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

EXAMPLE_TESTCODES_REQUIRED_TO_PASS = \
   trivial.C \
   test2020_04.C \
   test2020_08.C \
   test2020_09.C \
   test2020_15.C \
   test2020_17.C \
   test2020_18.C \
   test2020_19.C \
   test2020_20.C \
   test2020_23.C \
   test2020_27.C \
   test2020_39.C \
   test2020_40.C \
   test2020_70.C \
   test2020_72.C \
   test2020_74.C \
   test2020_77.C \
   test2020_78.C \
   test2020_79.C \
   test2020_80.C \
   test2020_81.C \
   test2020_83.C \
   test2020_84.C \
   test2020_85.C \
   test2020_86.C \
   test2020_87.C \
   test2020_88.C \
   test2020_89.C \
   test2020_90.C \
   test2020_91.C \
   test2020_92.C \
   test2020_93.C \
   test2020_94.C \
   test2020_95.C \
   test2020_96.C \
   test2020_97.C \
   test2020_98.C \
   test2020_100.C \
   test2020_100.C \
   test2020_101.C \
   test2020_102.C \
   test2020_103.C \
   test2020_104.C \
   test2020_110.C \
   test2020_111.C \
   test2020_116.C \
   test2020_118.C

TESTCODE_CURRENTLY_FAILING = \
   test2020_01.C \
   test2020_02.C \
   test2020_03.C \
   test2020_05.C \
   test2020_06.C \
   test2020_07.C \
   test2020_10.C \
   test2020_11.C \
   test2020_12.C \
   test2020_13.C \
   test2020_14.C \
   test2020_16.C \
   test2020_21.C \
   test2020_22.C \
   test2020_24.C \
   test2020_25.C \
   test2020_26.C \
   test2020_28.C \
   test2020_29.C \
   test2020_30.C \
   test2020_31.C \
   test2020_32.C \
   test2020_33.C \
   test2020_34.C \
   test2020_35.C \
   test2020_36.C \
   test2020_37.C \
   test2020_38.C \
   test2020_41.C \
   test2020_42.C \
   test2020_43.C \
   test2020_44.C \
   test2020_45.C \
   test2020_46.C \
   test2020_47.C \
   test2020_48.C \
   test2020_49.C \
   test2020_50.C \
   test2020_51.C \
   test2020_52.C \
   test2020_53.C \
   test2020_54.C \
   test2020_55.C \
   test2020_56.C \
   test2020_57.C \
   test2020_58.C \
   test2020_59.C \
   test2020_60.C \
   test2020_61.C \
   test2020_62.C \
   test2020_63.C \
   test2020_64.C \
   test2020_65.C \
   test2020_66.C \
   test2020_67.C \
   test2020_68.C \
   test2020_69.C \
   test2020_71.C \
   test2020_73.C \
   test2020_75.C \
   test2020_76.C \
   test2020_82.C \
   test2020_99.C \
   test2020_105.C \
   test2020_106.C \
   test2020_107.C \
   test2020_108.C \
   test2020_109.C \
   test2020_112.C \
   test2020_113.C \
   test2020_114.C \
   test2020_115.C \
   test2020_117.C 

# DQ (7/21/2020): These test codes have not yet been fixed up to be valid test codes for C++20 support in ROSE.
# They may fail for issues unrelated to C++20 support in EDG 6.0.
UNEVALUATED_TEST_CODES = 

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
   TESTCODE_CURRENTLY_FAILING += 
else
   EXAMPLE_TESTCODES_REQUIRED_TO_PASS += 
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

# DQ (7/202/2020): The -std=c++20 option is not supported on the only backend compilers I have for testing this at the moment.
# But it is supported properly in ROSE, so we just need to use the correct version of the backend compiler.
# LANG_FLAGS = -std=c++17
if ROSE_USING_GCC_VERSION_LATER_10_1
LANG_FLAGS = -std=c++20
else
LANG_FLAGS = -std=c++17
endif

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
if ROSE_USING_GCC_VERSION_LATER_10_1
	@echo "ROSE_USING_GCC_VERSION_LATER_5_1 == true"
	@$(MAKE) $(PASSING_TEST_Source_passed)
else # NOT ROSE_USING_GCC_VERSION_LATER_5_1
	@echo "ROSE_USING_GCC_VERSION_LATER_10_1 == true (requires GNU 10.1 or later version of GNU compiler)"
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
#	@$(MAKE) $(PASSING_TEST_Source_passed)
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
	@echo "*********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Cxx20_tests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************************"

clean-local:
	rm -f a.out *.o rose_*.[cC] roseTestProgram.C *.dot
	rm -f *.foldedConstants-o
	rm -f *.unfoldedConstants-o
	rm -rf QMTest
	rm -f *.int.c
	rm -f *.passed
	rm -f *.failed

status:
	@$(MAKE) $(TEST_Source_passed)

