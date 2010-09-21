include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##################################################################################
# These are C testcodes targeting aspects of C that might not be a subset of C++ #
##################################################################################

TESTCODES_REQUIRED_TO_PASS = \
     test_CplusplusMacro_C.c \
     gconv_info.c \
     stdio.c \
     testCvsCpp.c \
     testAnsiC.c \
     YardenPragmaPackExample.c \
     commentTest.c \
     test2003_12.c \
     test2004_20.c \
     test2004_21.c \
     test2005_172.c \
     test2005_168.c \
     test2005_186.c \
     test2006_48.c  \
     test2006_65.c  \
     test2006_110.c \
     test2006_114.c \
     test2006_132.c \
     test2006_133.c \
     test2006_135.c \
     test2008_01.c \
     test2008_02.c \
     test2009_01.c \
     test2009_02.c \
     test2009_03.c \
     test2009_04.c \
     test2009_05.c \
     test2009_07.c \
     test2009_08.c \
     test2009_16.c \
     test2009_21.c \
     test2009_22.c \
     test2010_01.c \
     test2010_04.c \
     test2010_06.c \
     math.c \
     complex_01.c \
     constants.c

# DQ (9/26/2009): Conditionally compile this file (fails in OSX due to lack of OSX
# support for thread local storage). Liao know more about the details of this test.
# test2009_19.c

if !OS_MACOSX
   TESTCODES_REQUIRED_TO_PASS += test2009_19.c
endif

#Liao, 5/16/2009 add a set of self-verifiable test code with main() 
# and let them run during make check
TESTCODES_REQUIRED_TO_RUN = \
     test2009_17.c \
     test2009_18.c

# test2009_20.c  (array -1) becomes (array) bug 368
TESTCODE_CURRENTLY_FAILING = \
     test2004_126.c \
     test2006_14.c  \
     test2006_100.c \
     test2006_134.c \
     test2006_136.c \
     test2009_06.c \
     test2009_20.c \
     test2010_02.c \
     test2010_03.c \
     test2010_05.c \
     Yarden_client.c

noinst_headers = test2006_134.h


# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# DQ (2/1/2010): Temporarily removed for testing.
# $(TESTCODES_REQUIRED_TO_RUN) 

# File option to accumulate performance information about the compilation
# PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/C_ROSE_PERFORMANCE_DATA.csv

ROSE_FLAGS = -rose:C --edg:no_warnings --edg:restrict -w 

PASSING_TEST_Objects = ${TESTCODES:.c=.o}
TEST_Objects = ${ALL_TESTCODES:.c=.o}
TEST_Exe_Objects = ${TESTCODES_REQUIRED_TO_RUN:.c=.o}

# PASSING_TEST_Executables =  ${TEST_Exe_Objects:.o=.out}
# check_PROGRAMS = $(PASSING_TEST_Executables)
# TESTS= $(check_PROGRAMS) caller.out 

testTranslator=../../testTranslator

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.c)
$(TEST_Objects): ../../testTranslator
	$(testTranslator) $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.c)

# This is a redundant test with it also being in the list TESTCODES_REQUIRED_TO_PASS (above).
test2005_168.o: $(srcdir)/test2005_168.c
	$(testTranslator) $(ROSE_FLAGS) -I$(top_srcdir)/src/util/commandlineProcessing -c $(srcdir)/test2005_168.c

# Liao 11/19/2009
# Custom tests to test rose translators as a linker (generating the final executables) for multiple files
# simplest case: generate .o with -c
caller.o: $(srcdir)/callee.c $(srcdir)/caller.c
	$(testTranslator) $(ROSE_FLAGS) -c $^
# default case: generate executable a.out from any number of input files	
caller.out: $(srcdir)/callee.c $(srcdir)/caller.c
	$(testTranslator) $(ROSE_FLAGS) $^ -o caller.out

# Customized test that modifies testTranslator to remove empty elses to test dangling else unparsing
test2008_02.o: $(srcdir)/test2008_02.c
	env ROSE_TEST_ELSE_DISAMBIGUATION=x $(testTranslator) $(ROSE_FLAGS) -c $(srcdir)/test2008_02.c

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

#QMTEST_Objects = ${ALL_TESTCODES:.c=.qmt}
#
# Make rule to build the QMTest database files
#CURRENT_DIRECTORY = `pwd`
#$(QMTEST_Objects): ../../testTranslator $(srcdir)/$(@:.qmt=.c)
#	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.c)
#
#test2005_168.qmt: $(srcdir)/test2005_168.c
#	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(top_srcdir)/src/util/commandlineProcessing -c $(srcdir)/$(@:.qmt=.c)
#
# Include makefile rules specific to QMTest
#include $(top_srcdir)/config/QMTest_makefile.inc

EXTRA_DIST = $(ALL_TESTCODES) builtin-types.def callee.c caller.c

copyFiles:
	cp $(srcdir)/*.h $(top_srcdir)/tests/CompileTests/C_tests
	cp $(srcdir)/*.c $(top_srcdir)/tests/CompileTests/C_tests

check-local:
	@echo "Tests for C."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
#	@$(MAKE) caller.o caller.out
	@echo "*********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/C_tests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] rose_performance_report_lockfile.lock *.out
	rm -rf QMTest

################################################################################
#
# Addition to show how to hook QMTest ROSE_Regression with automake
# Uncomment lines 115,$
#

#export ROSE_TEST_REGRESSION_ROOT=/home/yuan5/tmp/18/ROSE_Regression
#QMTEST=/home/yuan5/opt/qmtest-2.3/bin/qmtest
#ALL_Objects = ${ALL_TESTCODES:.c=.o}
#
#force:
#
#testTranslator_wrapper: force
#	rm -f ../../testTranslator
#	echo -e "#!/bin/bash\n${ROSE_TEST_REGRESSION_ROOT}/roseQMGen/bin/qm.sh ../../../testTranslator \"\$$@\"" > ../../testTranslator && chmod +x ../../testTranslator
#
#rrqm_generate: testTranslator_wrapper $(ALL_Objects)
#	test -d ./c_tests_testdatabase || mkdir c_tests_testdatabase 
#	rm -rf ./c_tests_testdatabase/*
#	find . -type f -mindepth 1 -maxdepth 1 -name "*.qmt" -exec mv {} c_tests_testdatabase/ \;
#	$(QMTEST) -D`pwd`/c_tests_testdatabase create-tdb
#	rm -f ../../testTranslator
#	cd ../..; $(MAKE) testTranslator
#
#c_tests_testdatabase/QMTest:
#	$(MAKE) rrqm_generate
#
#rrqm_run: c_tests_testdatabase/QMTest
#	-cd c_tests_testdatabase && $(QMTEST) run
#
#c_tests_testdatabase/results.qmr:
#	$(MAKE) rrqm_run
#
#rrqm_report: c_tests_testdatabase/results.qmr
#	sh -c "source ${ROSE_TEST_REGRESSION_ROOT}/scripts/rosetestrc; report.sh c_tests_testdatabase/results.qmr c_tests_testdatabase report_c_tests"
#	@echo -e "\nThe results are in `pwd`/report_c_tests/results.xml"
