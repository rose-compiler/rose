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
     math.c \
     complex_01.c \
     constants.c

TESTCODE_CURRENTLY_FAILING = \
     test2004_126.c \
     test2006_14.c  \
     test2006_100.c \
     test2006_134.c \
     test2006_136.c \
     Yarden_client.c

noinst_headers = test2006_134.h


# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# File option to accumulate performance information about the compilation
# PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/C_ROSE_PERFORMANCE_DATA.csv

ROSE_FLAGS = -rose:C --edg:no_warnings -w 

PASSING_TEST_Objects = ${TESTCODES:.c=.o}
TEST_Objects = ${ALL_TESTCODES:.c=.o}

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.c)
$(TEST_Objects): ../../testTranslator
	../../testTranslator $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.c)

test2005_168.o: $(srcdir)/test2005_168.c
	../../testTranslator $(ROSE_FLAGS) -I$(top_srcdir)/src/util/commandlineProcessing -c $(srcdir)/test2005_168.c

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

QMTEST_Objects = ${ALL_TESTCODES:.c=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): ../../testTranslator $(srcdir)/$(@:.qmt=.c)
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.c)

test2005_168.qmt: $(srcdir)/test2005_168.c
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/../../testTranslator NULL $(ROSE_FLAGS) -I$(top_srcdir)/src/util/commandlineProcessing -c $(srcdir)/$(@:.qmt=.c)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

EXTRA_DIST = $(ALL_TESTCODES)

copyFiles:
	cp $(srcdir)/*.h $(top_srcdir)/tests/CompileTests/C_tests
	cp $(srcdir)/*.c $(top_srcdir)/tests/CompileTests/C_tests

check-local:
	@echo "Tests for C."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/C_tests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC]
	rm -rf QMTest

