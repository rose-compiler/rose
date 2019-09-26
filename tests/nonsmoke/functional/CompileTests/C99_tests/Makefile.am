include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###########################################################################################
# These are C99 testcodes targeting aspects of C99 that might not be a subset of C or C++ #
###########################################################################################

TESTCODES_REQUIRED_TO_PASS = \
     test2005_186.c \
     test2006_127.c \
     test2006_143.c \
     test2008_01.c \
     test2012_20.c \
     test2012_23.c \
     test2012_25.c \
     test2012_58.c \
     test2012_100.c \
     test2012_104.c \
     test2012_105.c \
     test2013_01.c \
     test2013_02.c \
     test2013_03.c \
     test2013_04.c \
     test2014_01.c \
     complex_01.c \
     complex_03.c \
     bool.c \
     constants.c

TESTCODE_CURRENTLY_FAILING = \
     complex_02.c \
     complex_04.c \
     test2012_106.c

# DQ (7/3/2013): This demonstrates the GNU gcc does not default to C99, at least for gcc 
# version 4.4.5. ROSE was modified in March 2013 to default to C99, but this might have 
# to be version number dependent (on GNU gcc).  For now comment this file out so we can 
# figure out what the versions numbers of GNU gcc default to which C language standards.
#    test2013_01.c 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

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

LANG_FLAGS = -std=c99

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

EXTRA_DIST = $(ALL_TESTCODES)

check-local:
	@echo "Tests for C99 examples."
	@$(MAKE) $(PASSING_TEST_Source_passed)
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/C99_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC]

status:
	@$(MAKE) $(TEST_Source_passed)

