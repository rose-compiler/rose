include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

####################################################################################
# These are ROSE translators and test the compilation ROSE applications using ROSE #
####################################################################################


if ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
   TESTCODES_REQUIRED_TO_PASS = \
testRoseHeaders_01.C

# Test code that I am working one still (tests ROSE compiling ROSE header files).
# This test can't be required to pass yet.
# testRoseHeaders_01.C

else
# Shorten the list of code to compile to simplify checks using "make distcheck"
TESTCODES_REQUIRED_TO_PASS = 
endif

EXTRA_TESTCODES_REQUIRED_TO_PASS = \
     test2003_33.C \
     test2003_34.C \
     test2004_46.C \
     test2006_43.C 

TESTCODE_CURRENTLY_FAILING = \
     $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

ROSE_FLAGS = --edg:no_warnings -w --edg:restrict 

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

$(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.C)
	../../testTranslator $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/$(@:.o=.C)

QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): ../../testTranslator $(srcdir)/$(@:.qmt=.C)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/$(@:.qmt=.C)

# DQ (9/13/2005): Test the compilation of ROSE using ROSE!
# This is just a test of compiling the Sage III IR (the largest file; most but not all of Sage III)
$(top_builddir)/src/frontend/SageIII/Cxx_Grammar.o: ../../testTranslator
	../../testTranslator -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C

$(top_builddir)/src/frontend/SageIII/Cxx_Grammar.qmt: ../../testTranslator
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# Exclude the Cxx_Grammar.C file from being part of the distribution
EXTRA_DIST = $(TESTCODES)

check-local:
	@echo "Tests of ROSE example translators (compiling ROSE using ROSE)."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*******************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/RoseExample_tests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC]
	rm -rf QMTest
