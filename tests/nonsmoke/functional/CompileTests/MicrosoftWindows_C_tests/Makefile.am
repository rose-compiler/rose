include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USING_ALTERNATE_BACKEND_C_COMPILER
# TOO (2/10/11): set the correct linker and linker-flags for the alternate
# backend C compiler.
override CCLD=@BACKEND_C_COMPILER@
override LIBS=

# TOO (2/10/11): don't run these tests if they were cross-compiled, otherwise
# they will just fail.
# check-TESTS:
# 	@echo "Skipping execution of cross-compiled tests: $(TESTS)"
endif

##################################################################################
# These are C testcodes being used for testing Microsoft Language Support.
##################################################################################


TESTCODES_REQUIRED_TO_PASS = \
   test2003_12.c  test2009_01.c   test2009_21.c   test2012_110.c  test2012_153.c \
   test2012_24.c  test2012_48.c   test2012_68.c   test2012_85.c   test2013_111.c \
   test2013_37.c  test2013_69.c   test2013_86.c   test2013_94.c   test2014_13.c \
   test2014_26.c  test2014_48.c   test2014_86.c   test2004_20.c   test2009_02.c \
   test2009_22.c  test2012_113.c  test2012_154.c  test2012_26.c   test2012_49.c \
   test2012_69.c  test2012_86.c   test2013_113.c  test2013_39.c   test2013_71.c \
   test2013_87.c  test2013_95.c   test2014_14.c   test2014_34.c   test2014_49.c  \
   test2014_92.c  test2005_172.c  test2009_04.c   test2010_01.c   test2012_11.c \
   test2012_158.c test2012_27.c   test2012_61.c   test2012_70.c   test2012_87.c \
   test2013_24.c  test2013_72.c   test2013_89.c   test2013_96.c \
   test2014_15.c  test2014_37.c   test2014_58.c   test2014_94.c   test2007_177.c \
   test2009_07.c  test2010_10.c   test2012_127.c  test2012_165.c  test2012_34.c \
   test2012_62.c  test2012_81.c   test2012_93.c   test2013_26.c   test2013_55.c \
   test2013_78.c  test2013_91.c   test2014_01.c   test2014_16.c   test2014_38.c  \
   test2014_70.c  test2008_01.c   test2009_08.c   test2010_17.c   test2012_140.c \
   test2012_170.c test2012_35.c   test2012_65.c   test2012_83.c   test2012_99.c \
   test2013_29.c  test2013_62.c   test2013_82.c   test2013_92.c   test2014_05.c \
   test2014_17.c  test2014_39.c   test2014_83.c   test2008_02.c   test2009_16.c  \
   test2011_05.c  test2012_148.c  test2012_22.c   test2012_37.c   test2012_67.c \
   test2012_84.c  test2013_100.c  test2013_30.c   test2013_64.c   test2013_83.c \
   test2013_93.c  test2014_12.c   test2014_21.c   test2014_40.c   test2014_84.c \
   hello.c

TESTCODE_CURRENTLY_FAILING = 

TESTCODES_REQUIRED_TO_RUN =

TESTCODES = \
   $(TESTCODES_REQUIRED_TO_PASS) 

ALL_TESTCODES = \
   $(TESTCODES_REQUIRED_TO_PASS) \
   $(TESTCODE_CURRENTLY_FAILING)

# File option to accumulate performance information about the compilation
# PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/C_ROSE_PERFORMANCE_DATA.csv

# TOO1 (2013-09-17): Required by test2005_168.o to find "rosedll.h",
#                    introduced by CMake work.
TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII/

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

LANG_FLAGS = -std=c89

ROSE_FLAGS = -pedantic-errors
if !ROSE_USE_CLANG_FRONTEND
   ROSE_FLAGS += --edg:no_warnings --edg:restrict
endif
ROSE_FLAGS += -w -rose:verbose 0 -rose:skip_unparse_asm_commands -rose:skipfinalCompileStep

#########
# RTH test targets

$(TEST_Source_passed): %.passed: $(TEST_TRANSLATOR) $(top_srcdir)/scripts/test_exit_status
	@$(RTH_RUN) \
	   CMD="$(TEST_TRANSLATOR) $(LANG_FLAGS) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.c.passed=.c)" \
	   TITLE="TEST: $(TEST_TRANSLATOR) $(@:.c.passed=.c)" \
	   $(top_srcdir)/scripts/test_exit_status $@

#
############

EXTRA_DIST = $(ALL_TESTCODES)

check-local:
if ROSE_USING_GCC_VERSION_LATER_4_4
	@echo "Tests for C."
	@$(MAKE) $(PASSING_TEST_Source_passed)
else
	@echo "Microsoft C tests require GNU version 4.4 (or greater) backend compilers."
endif
	@echo "**************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/MicrosoftWindows_C_tests: make check rule complete (terminated normally) ******"
	@echo "**************************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] rose_performance_report_lockfile.lock *.out *.dot
	rm -rf QMTest
	rm -rf test_directory
	rm -rf test2013_76_unparse_headers conftest_configure_test else_case_disambiguation_test.c
	rm -f *.err *.passed *.failed
	rm -f token_trailing_*.c token_leading_*.c

status:
	@$(MAKE) $(TEST_Source_passed)

