include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

####################################################################################
# These are ROSE translators and test the compilation ROSE applications using ROSE #
####################################################################################

# Test translator.
# noinst_PROGRAMS = vxworksc
# vxworksc_SOURCES = vxworksc.cpp

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

# Build variable to SystemC include directory.
# VXWORKS_C_INCLUDES = -I$(srcdir)/include
VXWORKS_C_INCLUDES = -I@VXWORKS_INCLUDES@

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
	@$(RTH_RUN) CMD="$(TEST_TRANSLATOR) $(ROSE_FLAGS) -I$(top_builddir) $(VXWORKS_INCLUDES) -c $(srcdir)/$(@:.C.passed=.C)" $(top_srcdir)/scripts/test_exit_status $@

# QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
# CURRENT_DIRECTORY = `pwd`
# $(QMTEST_Objects): $(TEST_TRANSLATOR) $(srcdir)/$(@:.qmt=.C)
#	@echo "Calling QMTEST_Objects rule: "
#	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(top_builddir) $(SYSTEM_C_INCLUDES) -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# ****************************************************************************************************
#     Test codes or selected ROSE code that can compile using ROSE or is the subject of current work 
# ****************************************************************************************************

# Test program for VX Works using C
test_hello_world: $(TEST_TRANSLATOR)
	$(TEST_TRANSLATOR) -c -I$(srcdir) $(VXWORKS_INCLUDES) $(srcdir)/input_hello_world.c

# Test program for VX Works using C++
test_pond: $(TEST_TRANSLATOR)
	$(TEST_TRANSLATOR) -c -I$(srcdir) $(VXWORKS_INCLUDES) $(srcdir)/pond.cc

EXTRA_DIST = $(TESTCODES) input_hello_world.cpp
# input_hello_world.h

check-local:
	@echo "Tests of VX Works C examples."
	@$(MAKE) $(PASSING_TEST_Sources)
#	@$(MAKE) test_vxworks
	@echo "***********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/vxworks_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot
	rm -rf QMTest
	rm -f *.passed *.failed
