include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

####################################################################################
# These are ROSE translators and test the compilation ROSE applications using ROSE #
####################################################################################

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

# Build variable to GNAT include directory.
# GNAT_INCLUDES = -I$(srcdir)/include
GNAT_INCLUDES = -I@GNAT_INCLUDES@

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

# PASSING_TEST_Objects = ${TESTCODES:.C=.o}
# PASSING_TEST_Sources = ${TESTCODES:.C=.C.passed}
# TEST_Objects = ${ALL_TESTCODES:.C=.o}
# TEST_Sources = ${ALL_TESTCODES:.C=.C.passed}

TEST_TRANSLATOR = ../../testTranslator

# DQ (6/8/2017): Make all of the tests codes
$(TEST_TRANSLATOR):
	cd ../..; $(MAKE) test

# $(TEST_Sources): %.passed: $(TEST_TRANSLATOR) $(srcdir)/$(@:.C.passed=.C)
#	@$(RTH_RUN) CMD="$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(GNAT_INCLUDES) -c $(srcdir)/$(@:.C.passed=.C)" $(top_srcdir)/scripts/test_exit_status $@

# ****************************************************************************************************
#     Test codes or selected ROSE code that can compile using ROSE or is the subject of current work 
# ****************************************************************************************************

# Test program for GNAT
# Possible syntax we need to use: ${tool_name} -f ${target_unit} "$@"
test_gnat: $(TEST_TRANSLATOR)
	$(TEST_TRANSLATOR) -c $(srcdir)/unit_2.adb

EXTRA_DIST = $(TESTCODES) input_hello_world.cpp
# input_hello_world.h

check-local:
	@echo "Tests of GNAT examples."
	@$(MAKE) $(PASSING_TEST_Sources)
	@$(MAKE) test_gnat
	@echo "********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/gnat_tests: make check rule complete (terminated normally) ******"
	@echo "********************************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot
	rm -rf QMTest
	rm -f *.passed *.failed
