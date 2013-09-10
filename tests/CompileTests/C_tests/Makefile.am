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

OLD_TESTCODES_REQUIRED_TO_PASS = \
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
     test2005_186.c \
     test2006_48.c  \
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
     test2009_07.c \
     test2009_08.c \
     test2009_16.c \
     test2009_21.c \
     test2009_22.c \
     test2010_01.c \
     test2010_04.c \
     test2010_06.c \
     test2010_07.c \
     test2010_08.c \
     test2010_09.c \
     test2010_10.c \
     test2010_12.c \
     test2010_13.c \
     test2010_15.c \
     test2010_17.c \
     test2011_05.c \
     test2007_177.c \
     test2006_65.c \
     test2010_11.c \
     test2010_14.c \
     math.c \
     complex_01.c \
     constants.c

# This is listed seperately below with the required path to find the "sla.h" header file.
# test2005_168.c 

TESTCODES_REQUIRED_TO_PASS = \
     test_CplusplusMacro_C.c \
     gconv_info.c \
     testCvsCpp.c \
     testAnsiC.c \
     YardenPragmaPackExample.c \
     commentTest.c \
     test_multiline_string_01.c \
     test2003_12.c \
     test2004_20.c \
     test2005_172.c \
     test2005_186.c \
     test2006_110.c \
     test2006_114.c \
     test2006_133.c \
     test2006_135.c \
     test2007_177.c \
     test2008_01.c \
     test2008_02.c \
     test2009_01.c \
     test2009_02.c \
     test2009_03.c \
     test2009_04.c \
     test2009_07.c \
     test2009_08.c \
     test2009_16.c \
     test2009_21.c \
     test2009_22.c \
     test2010_01.c \
     test2010_06.c \
     test2010_07.c \
     test2010_08.c \
     test2010_09.c \
     test2010_13.c \
     test2010_15.c \
     test2010_17.c \
     test2011_05.c \
     test2012_01.c \
     test2012_02.c \
     test2012_03.c \
     test2012_04.c \
     test2012_05.c \
     test2012_06.c \
     test2012_07.c \
     test2012_08.c \
     test2012_09.c \
     test2012_10.c \
     test2012_11.c \
     test2012_12.c \
     test2012_13.c \
     test2012_14.c \
     test2012_15.c \
     test2012_16.c \
     test2012_17.c \
     test2012_18.c \
     test2012_19.c \
     test2012_20.c \
     test2012_21.c \
     test2012_22.c \
     test2012_23.c \
     test2012_24.c \
     test2012_25.c \
     test2012_26.c \
     test2012_27.c \
     test2012_28.c \
     test2012_29.c \
     test2012_30.c \
     test2012_32.c \
     test2012_33.c \
     test2012_34.c \
     test2012_35.c \
     test2012_36.c \
     test2012_37.c \
     test2012_38.c \
     test2012_39.c \
     test2012_40.c \
     test2012_42.c \
     test2012_41.c \
     test2012_43.c \
     test2012_46.c \
     test2012_47.c \
     test2012_48.c \
     test2012_49.c \
     test2012_50.c \
     test2012_51.c \
     test2012_52.c \
     test2012_53.c \
     test2012_54.c \
     test2012_55.c \
     test2012_56.c \
     test2012_57.c \
     test2012_58.c \
     test2012_59.c \
     test2012_60.c \
     test2012_61.c \
     test2012_62.c \
     test2012_63.c \
     test2012_64.c \
     test2012_65.c \
     test2012_66.c \
     test2012_67.c \
     test2012_68.c \
     test2012_69.c \
     test2012_70.c \
     test2012_71.c \
     test2012_72.c \
     test2012_73.c \
     test2012_74.c \
     test2012_75.c \
     test2012_76.c \
     test2012_77.c \
     test2012_79.c \
     test2012_80.c \
     test2012_81.c \
     test2012_82.c \
     test2012_83.c \
     test2012_84.c \
     test2012_85.c \
     test2012_86.c \
     test2012_87.c \
     test2012_88.c \
     test2012_89.c \
     test2012_90.c \
     test2012_91.c \
     test2012_92.c \
     test2012_93.c \
     test2012_94.c \
     test2012_95.c \
     test2012_96.c \
     test2012_97.c \
     test2012_98.c \
     test2012_99.c \
     test2012_100.c \
     test2012_101.c \
     test2012_102.c \
     test2012_103.c \
     test2012_104.c \
     test2012_105.c \
     test2012_106.c \
     test2012_107.c \
     test2012_108.c \
     test2012_109.c \
     test2012_110.c \
     test2012_111.c \
     test2012_112.c \
     test2012_113.c \
     test2012_114.c \
     test2012_115.c \
     test2012_116.c \
     test2012_117.c \
     test2012_118.c \
     test2012_119.c \
     test2012_120.c \
     test2012_121.c \
     test2012_122.c \
     test2012_123.c \
     test2012_124.c \
     test2012_125.c \
     test2012_126.c \
     test2012_127.c \
     test2012_129.c \
     test2012_130.c \
     test2012_131.c \
     test2012_133.c \
     test2012_134.c \
     test2012_135.c \
     test2012_136.c \
     test2012_137.c \
     test2012_139.c \
     test2012_140.c \
     test2012_141.c \
     test2012_142.c \
     test2012_143.c \
     test2012_144.c \
     test2012_146.c \
     test2012_147.c \
     test2012_148.c \
     test2012_149.c \
     test2012_150.c \
     test2012_151.c \
     test2012_152.c \
     test2012_153.c \
     test2012_154.c \
     test2012_156.c \
     test2012_157.c \
     test2012_158.c \
     test2012_159.c \
     test2012_162.c \
     test2012_163.c \
     test2012_164.c \
     test2012_165.c \
     test2012_166.c \
     test2012_167.c \
     test2012_168.c \
     test2012_169.c \
     test2012_170.c \
     test2012_171.c \
     test2012_172.c \
     test2012_173.c \
     test2012_174.c \
     test2012_175.c \
     test2013_01.c \
     test2013_02.c \
     test2013_03.c \
     test2013_05.c \
     test2013_06.c \
     test2013_07.c \
     test2013_08.c \
     test2013_09.c \
     test2013_10.c \
     test2013_11.c \
     test2013_12.c \
     test2013_13.c \
     test2013_14.c \
     test2013_15.c \
     test2013_16.c \
     test2013_17.c \
     test2013_18.c \
     test2013_19.c \
     test2013_20.c \
     test2013_21.c \
     test2013_22.c \
     test2013_23.c \
     test2013_24.c \
     test2013_25.c \
     test2013_26.c \
     test2013_27.c \
     test2013_28.c \
     test2013_29.c \
     test2013_30.c \
     test2013_31.c \
     test2013_32.c \
     test2013_33.c \
     test2013_34.c \
     test2013_35.c \
     test2013_36.c \
     test2013_37.c \
     test2013_38.c \
     test2013_39.c \
     test2013_40.c \
     test2013_41.c \
     test2013_42.c \
     test2013_43.c \
     test2013_44.c \
     test2013_45.c \
     test2013_46.c \
     test2013_47.c \
     test2013_48.c \
     test2013_49.c \
     test2013_50.c \
     test2013_51.c \
     test2013_52.c \
     test2013_53.c \
     test2013_54.c \
     test2013_55.c \
     test2013_56.c \
     test2013_57.c \
     test2013_58.c \
     test2013_59.c \
     test2013_60.c \
     test2013_61.c \
     test2013_62.c \
     test2013_63.c \
     test2013_64.c \
     test2013_65.c \
     test2013_66.c \
     math.c \
     complex_01.c \
     constants.c

# DQ (9/9/2013): This is a problem for the new block support.
#     test2012_23.c 
#     test2012_25.c 
#     test2012_36.c 
#     test2012_42.c
#     test2012_101.c
#     test2012_102.c
#     test2012_103.c 
#     test2012_104.c 
#     test2012_105.c 
#     test2012_116.c 
#     test2012_118.c 
#     test2012_119.c 


# DQ (5/23/2013): This is a failing case (reported by Sara). Case of "void foo(int N,int a[N]){}"
# test2013_04.c 

# DQ (9/2/2013): This is now fixed.
# DQ (6/19/2013): Failing case of include used to initialize array (still working on this case).
# test2013_05.c 


# DQ (9/28/2012): Failing tests.
# test2012_04.c (infinite recursion!) (DQ: 11/21/2012: NOW FIXED)
# test2012_30.c (break statement not unparsed in loop)
# test2012_42.c (more complex (original) case of test2012_43.c)
# test2012_43.c (macro for compile-time assertions)
# test2012_44.c (asm issue stat64 undefined after compilation)
# test2012_45.c (same problems at test2012_44.c)
# test2012_59.c (C-style comment in quoted string (inside of #if-def comment, so should be ignored)
# test2012_78.c (variable with initializer using #include file to provide value (classic source-to-source issue))
# test2012_79.c (test code demonstrating an outstanding error (generated before focusing on switch support))
# test2012_80.c (same as test2012_79.c)
# test2012_101.c (example of failing test from zsh : jobs.c file: NOW FIXED)
# test2012_102.c (example of failing test from zsh : module.c file)
# test2012_103.c (example of failing test from zsh : params.c file)
# test2012_107.c (fails to unparse "..." in function type: NOW FIXED)
# test2012_128.c (label attached to case option statement is a current bug, but not yet found in application code)
# test2012_132.c (this has a overly complex duff's device like switch statement that I have rewritten to put the
#                 default case at the same scope as the other case option statements. Adding this level of support
#                 to the switch statment support in ROSE is deferred to later. test2012_137.c is a required test
#                 code to pass; so that we can test the rewritten version of the switch.
# test2012_145.c NOT A FAILING test, but tested below with -D options.
# test2012_155.c (array with initializer using #include file to provide value (classic source-to-source issue))
# test2012_160.c (application code with label on default case option statement: full size)
# test2012_161.c (small test code demonstrating bug in test2012_160.c)

# Failing tests of interest relative to C application codes:
# test2012_128
# test2012_31
# test2012_132
# test2012_138
# test2012_159



# This is a C99 test code (moved to the C99_tests directory). (These are not really C99 codes, I thought they were).
# test2012_20.c 
# test2012_23.c
# test2012_25.c
# test2012_58.c 
# test2012_100.c 
# test2012_104.c
# test2012_105.c 
# test2012_106.c 


if ROSE_USE_CLANG_FRONTEND
   TESTCODES_REQUIRED_TO_PASS += \
     stdio.c \
     test2006_48.c \
     test2006_132.c \
     test2010_04.c \
     test2006_14.c  \
     test2006_100.c \
     test2006_136.c \
     test2010_02.c \
     test2010_03.c \
     test2011_03.c \
     test2011_04.c \
     Yarden_client.c
# Failling tests for CLANG only:
#  -  test2004_21.c
#  -  test2005_168.c
#  -  test2006_65.c
#  -  test2010_11.c
#  -  test2010_14.c
# Failling tests for CLANG but also in TESTCODE_CURRENTLY_FAILING
#  -  test2004_126.c
#  -  test2006_134.c
#  -  test2009_06.c
#  -  test2010_05.c     -> Unknown statement kind: CompoundLiteralExpr
#  -  test2010_16.c     -> Unknown statement kind: CompoundLiteralExpr
endif

if !ROSE_USE_CLANG_FRONTEND
   TESTCODES_REQUIRED_TO_PASS += \
     test2004_21.c \
     test2005_168.c \
     test2006_65.c
if !ROSE_USE_EDG_VERSION_4_4
if !ROSE_USE_EDG_VERSION_4_7
   TESTCODES_REQUIRED_TO_PASS += \
     stdio.c \
     test2006_48.c \
     test2006_132.c \
     test2010_04.c \
     test2010_11.c \
     test2010_12.c \
     test2010_14.c
endif
endif
endif

# DQ (9/26/2009): Conditionally compile this file (fails in OSX due to lack of OSX
# support for thread local storage). Liao know more about the details of this test.
# test2009_19.c
if !OS_MACOSX
   TESTCODES_REQUIRED_TO_PASS += test2009_19.c
endif

#Liao, 5/16/2009 add a set of self-verifiable test code with main() 
# and let them run during make check
# TV, 9/7/12 do not run them if using Clang
TESTCODES_REQUIRED_TO_RUN =

if !ROSE_USE_CLANG_FRONTEND
   TESTCODES_REQUIRED_TO_RUN += \
     test2009_05.c \
     test2009_17.c
endif

TESTCODE_CURRENTLY_FAILING = \
     test2004_126.c \
     test2006_14.c  \
     test2006_100.c \
     test2006_134.c \
     test2006_136.c \
     test2009_06.c \
     test2010_02.c \
     test2010_03.c \
     test2010_05.c \
     test2010_16.c \
     test2011_03.c \
     test2011_04.c \
     Yarden_client.c



# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING) \
$(OLD_TESTCODES_REQUIRED_TO_PASS)

# File option to accumulate performance information about the compilation
# PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/C_ROSE_PERFORMANCE_DATA.csv

# DQ (3/28/2013): Now that we have changed the default C behavior to C99 (from C89), we need 
# to explicitly specify the C89 behavior for make of the C test codes in this directory.
# ROSE_FLAGS = -rose:C
ROSE_FLAGS = -rose:C89
if !ROSE_USE_CLANG_FRONTEND
   ROSE_FLAGS += --edg:no_warnings --edg:restrict
endif
ROSE_FLAGS += -w


PASSING_TEST_Objects = ${TESTCODES:.c=.o}
TEST_Objects = ${ALL_TESTCODES:.c=.o}
TEST_Exe_Objects = ${TESTCODES_REQUIRED_TO_RUN:.c=.o}

PASSING_TEST_Executables =  ${TEST_Exe_Objects:.o=.out}

if !ROSE_USE_EDG_VERSION_4_4
if !ROSE_USE_EDG_VERSION_4_7
# any programs needed by your tests that does not need to be built by make all.
check_PROGRAMS = $(PASSING_TEST_Executables)
# a list of programs or scripts to run in order to do the testing 
TESTS= $(check_PROGRAMS) caller.out 
endif
endif

testTranslator = ../../testTranslator

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.c)
$(TEST_Objects): ../../testTranslator
	$(testTranslator) $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.c)

# This is a redundant test with it also being in the list TESTCODES_REQUIRED_TO_PASS (above).
test2005_168.o: $(srcdir)/test2005_168.c
	$(testTranslator) $(ROSE_FLAGS) -I$(top_srcdir)/src/util/commandlineProcessing -c $(srcdir)/test2005_168.c

# DQ (11/1/2012): Added test code to test string concatination using a value defined 
# via a macro specified on the command line. This tests the string quoting mechanism.
test2012_145.o: $(srcdir)/test2012_145.c
	$(testTranslator) $(ROSE_FLAGS) -DSTRING_MACRO=\"some_text\" -c $(srcdir)/test2012_145.c

$(TEST_Exe_Objects): %.o: $(srcdir)/%.c $(testTranslator)
	$(testTranslator) $(ROSE_FLAGS) -c $<

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

EXTRA_DIST = $(ALL_TESTCODES) builtin-types.def callee.c caller.c c-common.def \
             test2006_134.h test2010_08.h predict.def test2010_15.h \
             test2009_18.c test2009_20.c grep_verify.h test2012_75.h \
             test2012_78.h test2012_155.h test2012_156.h

copyFiles:
	cp $(srcdir)/*.h $(top_srcdir)/tests/CompileTests/C_tests
	cp $(srcdir)/*.c $(top_srcdir)/tests/CompileTests/C_tests

check-local:
	@echo "Tests for C."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
#	@$(MAKE) caller.o caller.out
	@$(MAKE) test2012_145.o
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
