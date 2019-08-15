include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USING_ALTERNATE_BACKEND_C_COMPILER
# TOO (2/10/11): set the correct linker and linker-flags for the alternate
# backend C compiler.
override CCLD=@BACKEND_C_COMPILER@
override LIBS=

# TOO (2/10/11): don't run these tests if they were cross-compiled, otherwise
# they will just fail.
check-TESTS:
	@echo "Skipping execution of cross-compiled tests: $(TESTS)"
endif

##################################################################################
# These are C testcodes targeting aspects of C that might not be a subset of C++ #
##################################################################################

TESTCODES_REQUIRED_TO_PASS = \
test2003_12.c \
test2004_20.c \
test2005_172.c \
test2006_132.c \
test2006_48.c \
test2007_177.c \
test2008_01.c \
test2008_02.c \
test2009_08.c \
test2009_07.c \
test2009_02.c \
test2009_22.c \
test2009_01.c \
test2009_21.c \
test2009_04.c \
test2009_16.c \
test2010_14.c \
test2010_10.c \
test2010_01.c \
test2011_05.c \
test2010_15.c \
test2010_17.c \
test2010_11.c \
test2012_07.c \
test2012_153.c \
test2012_30.c \
test2012_51.c \
test2012_68.c \
test2012_84.c \
test2012_05.c \
test2012_148.c \
test2012_175.c \
test2012_49.c \
test2012_67.c \
test2012_83.c \
test2012_99.c \
test2012_11.c \
test2012_158.c \
test2012_35.c \
test2012_61.c \
test2012_70.c \
test2012_86.c \
test2012_127.c \
test2012_165.c \
test2012_47.c \
test2012_62.c \
test2012_71.c \
test2012_87.c \
test2012_140.c \
test2012_170.c \
test2012_48.c \
test2012_65.c \
test2012_81.c \
test2012_93.c \
test2012_08.c \
test2012_154.c \
test2012_34.c \
test2012_53.c \
test2012_69.c \
test2012_85.c \
test2013_05.c \
test2013_06.c \
test2013_22.c \
test2013_24.c \
test2013_25.c \
test2013_26.c \
test2013_29.c \
test2013_30.c \
test2013_37.c \
test2013_38.c \
test2013_39.c \
test2013_52.c \
test2013_55.c \
test2013_62.c \
test2013_64.c \
test2013_67.c \
test2013_68.c \
test2013_69.c \
test2013_71.c \
test2013_72.c \
test2013_78.c \
commentTest.c \
testAnsiC.c \
gconv_info.c \
test_CplusplusMacro_C.c \
math.c \
testCvsCpp.c \
stdio.c \
YardenPragmaPackExample.c 

# DQ (9/25/2013): All of these before test2013_78.c were a part fo the 
# C_tests directory.  This subset compiles using the -std=c89 where as
# all the rest had required the -std=gnu89 option (default for GNU gcc).

TESTCODE_CURRENTLY_FAILING = 


# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = $(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

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

ROSE_FLAGS =
if !ROSE_USE_CLANG_FRONTEND
   ROSE_FLAGS += --edg:no_warnings --edg:restrict
endif
ROSE_FLAGS += -w -rose:verbose 0

#########
# RTH test targets

$(TEST_Source_passed): %.passed: $(srcdir)/% $(TEST_TRANSLATOR) $(top_srcdir)/scripts/test_exit_status
if !ROSE_USE_INSURE
	@$(RTH_RUN) \
		CMD="$(TEST_TRANSLATOR) $(LANG_FLAGS) $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.c.passed=.c)" \
		TITLE="TEST: $(TEST_TRANSLATOR) $(@:.c.passed=.c)" \
		$(top_srcdir)/scripts/test_exit_status $@
else
	$(VALGRIND) $(TEST_TRANSLATOR) $(LANG_FLAGS) $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.c.passed=.c)
endif

#
############

# builtin-types.def, Liao, 1/15/2014, no reference to this file at all. remove it from the list
EXTRA_DIST = $(ALL_TESTCODES) c-common.def \
             predict.def test2010_15.h test2013_05.h test2013_06.h

copyFiles:
	cp $(srcdir)/*.h $(top_srcdir)/tests/nonsmoke/functional/CompileTests/C_tests
	cp $(srcdir)/*.c $(top_srcdir)/tests/nonsmoke/functional/CompileTests/C_tests

check-local:
	@echo "Tests for C."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Source_passed)
	@echo "*******************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/C89_std_c89_tests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] rose_performance_report_lockfile.lock *.out *.dot
	rm -rf QMTest
	rm -rf test_directory
	rm -rf test2013_76_unparse_headers conftest_configure_test else_case_disambiguation_test.c
	rm -f *.err *.passed *.failed

status:
	@$(MAKE) $(TEST_Source_passed)

