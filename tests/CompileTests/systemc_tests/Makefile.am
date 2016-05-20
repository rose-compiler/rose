include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

####################################################################################
# These are ROSE translators and test the compilation ROSE applications using ROSE #
####################################################################################

# Test translator contributed by Tim.
noinst_PROGRAMS = member_function

member_function_SOURCES = member_function.cpp

AM_CPPFLAGS = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build variable to SystemC include directory.
SYSTEM_C_INCLUDES = -I$(srcdir)/include

# Files that represent header files from ROSE that we can now compile with ROSE.
# However we don't want to distribute the System C include files, so we can't 
# include the test code to be compiled (except for internal testing).
# TESTCODES_REQUIRED_TO_PASS = input_hello_world.C
TESTCODES_REQUIRED_TO_PASS = 

EXTRA_TESTCODES_REQUIRED_TO_PASS = 

TESTCODE_CURRENTLY_FAILING = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

ROSE_FLAGS = --edg:no_warnings -w --edg:restrict 

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
PASSING_TEST_Sources = ${TESTCODES:.C=.C.passed}
TEST_Objects = ${ALL_TESTCODES:.C=.o}
TEST_Sources = ${ALL_TESTCODES:.C=.C.passed}

TEST_TRANSLATOR = ../../testTranslator

# DQ (2/10/2013): Make all of the tests codes
$(TEST_TRANSLATOR):
	cd ../..; $(MAKE) test

$(TEST_Sources): %.passed: $(TEST_TRANSLATOR) $(srcdir)/$(@:.C.passed=.C)
	@$(RTH_RUN) CMD="$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(SYSTEM_C_INCLUDES) -c $(srcdir)/$(@:.C.passed=.C)" $(top_srcdir)/scripts/test_exit_status $@

QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): $(TEST_TRANSLATOR) $(srcdir)/$(@:.qmt=.C)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(top_builddir) $(SYSTEM_C_INCLUDES) -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# ****************************************************************************************************
#     Test codes or selected ROSE code that can compile using ROSE or is the subject of current work 
# ****************************************************************************************************

# DQ (4/10/2016): Added test code from project working on systemc (but this was not really specific to systemc support).
test_member_function: member_function
	./member_function -c -I$(srcdir) $(srcdir)/input_member_function.cpp
#	./member_function -DSKIP_ROSE_BUILTIN_DECLARATIONS -c -I$(srcdir) $(srcdir)/input_member_function.cpp

EXTRA_DIST = $(TESTCODES) input_member_function.cpp input_member_function.h

check-local:
	@echo "Tests of System C examples."
	@$(MAKE) $(PASSING_TEST_Sources)
	@$(MAKE) test_member_function
	@echo "**************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/sytemc_tests: make check rule complete (terminated normally) ******"
	@echo "**************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot
	rm -rf QMTest
	rm -f *.passed *.failed
