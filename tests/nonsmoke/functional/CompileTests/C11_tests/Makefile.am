include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


EXAMPLE_TESTCODES_REQUIRED_TO_PASS = \
test2014_03.c \
test2014_06.c \
test2014_07.c \
test2014_08.c \
test2014_09.c \
test2014_10.c \
test2014_11.c \
test2014_12.c \
test2014_13.c \
test2014_14.c \
test2014_15.c \
test2014_16.c \
test2014_17.c \
test2014_18.c \
test2014_19.c \
test2014_20.c \
test2014_21.c \
test2014_22.c \
test2014_23.c \
test2014_24.c \
test2014_25.c \
test2014_26.c \
test2014_27.c \
test2014_28.c \
test2014_29.c \
test2014_30.c \
last.c

TESTCODE_CURRENTLY_FAILING =

if !USING_INTEL_COMPILER
EXAMPLE_TESTCODES_REQUIRED_TO_PASS += \
   test2014_01.c \
   test2014_02.c \
   test2014_04.c \
   test2014_05.c 
else
TESTCODE_CURRENTLY_FAILING += \
   test2014_01.c \
   test2014_02.c \
   test2014_04.c \
   test2014_05.c 
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

#########
# Targets lists

PASSING_TEST_Source_passed = ${TESTCODES:.c=.c.passed}
TEST_Source_passed = ${ALL_TESTCODES:.c=.c.passed}

#########
# Translator to be used: calling "make TEST_TRANSLATOR=../../testBackend" uses the backend compiler only

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

../../testBackend:
	cd ../..; $(MAKE) testBackend

testCompiler:
	ln -s `which @BACKEND_C_COMPILER@` testCompiler

TEST_TRANSLATOR=../../testTranslator

#########
# ROSE_FLAGS

LANG_FLAGS = -std=c11

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
		CMD="$(TEST_TRANSLATOR) $(LANG_FLAGS) $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.c.passed=.c)" \
		TITLE="TEST: $(TEST_TRANSLATOR) $(@:.c.passed=.c)" \
		$(top_srcdir)/scripts/test_exit_status $@
else
	$(VALGRIND) $(TEST_TRANSLATOR) $(LANG_FLAGS) $(ROSE_FLAGS) -I$(srcdir) -c $(@:.o=.C)
endif

#
############

# Make sure that all the test codes end up in the distribution
EXTRA_DIST = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

check-local:
# DQ (2/19/2017): NEW VERSION OF LOGIC
# DQ (2/18/2017): More specific logic required for portability.
# When using EDG 4.9 these files don't compile on later versions of GNU, CLANG, and Intel compilers 
# but we still want them tests as we transition to EDG 4.12 (where they all work fine independent of 
# the compiler vendor and GNU compiler version).
if ROSE_USE_EDG_VERSION_4_9
	@echo "ROSE_USE_EDG_VERSION_4_9 == true"
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
if ROSE_USING_GCC_VERSION_LATER_4_8
	@echo "ROSE_USING_GCC_VERSION_LATER_4_8 == true"
	@echo "C11 tests using GNU version 4.8 backend compiler."
	@$(MAKE) $(PASSING_TEST_Source_passed)
else  # NOT ROSE_USING_GCC_VERSION_LATER_4_8
	@echo "ROSE_USING_GCC_VERSION_LATER_4_8 == false"
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else  # NOT USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
else  # NOT ROSE_USE_EDG_VERSION_4_9
# Using EDG 4.12 or later (run all tests independent of the compiler).
	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
if ROSE_USING_GCC_VERSION_LATER_4_8
	@echo "ROSE_USING_GCC_VERSION_LATER_4_8 == true"
	@echo "C11 tests using GNU version 4.8 backend compiler."
	@$(MAKE) $(PASSING_TEST_Source_passed)
else  # NOT ROSE_USING_GCC_VERSION_LATER_4_8
	@echo "ROSE_USING_GCC_VERSION_LATER_4_8 == false"
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else  # NOT USING_GNU_COMPILER
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
# DQ (1/18/2017): EDG 4.9 is the default now, and EDG 4.12 will be the default shortly.
# # if ROSE_USE_EDG_VERSION_4_9
# if ROSE_USING_GCC_VERSION_LATER_4_8
#	@echo "C11 tests using GNU version 4.8 backend compiler."
#	@$(MAKE) $(PASSING_TEST_Source_passed)
# else
# if ROSE_USING_GCC_VERSION_LATER_4_9
# 	@echo "C11 tests using GNU version 4.9 backend compiler."
#	@$(MAKE) $(PASSING_TEST_Source_passed)
# endif
# endif
# else
# 	@echo "C11 tests require configuration with EDG version 4.9 and GNU version 4.8 or 4.9 backend compilers."
# endif
	@echo "*******************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/C11_tests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************************"

clean-local:
	rm -f a.out *.o rose_*.[cC] roseTestProgram.C *.dot
	rm -f *.int.c

status:
	@$(MAKE) $(TEST_Source_passed)

