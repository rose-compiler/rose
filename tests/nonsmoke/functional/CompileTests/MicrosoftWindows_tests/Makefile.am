include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Should this be here!
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc

##################################################################################
# These are C testcodes targeting aspects of C that might not be a subset of C++ #
##################################################################################

TEST_TRANSLATOR = $(top_builddir)/tests/nonsmoke/functional/testTranslator 

if ROSE_USE_WINDOWS_ANALYSIS_SUPPORT

TESTCODES_REQUIRED_TO_PASS = \
     test2017_01.C \
     test2017_02.C \
     test2017_03.C \
     test2017_04.C \
     test2017_05.C \
     simple_windows_prog1.C \
     tibbar_packer.C 

# DQ (4/23/2017): Adding some extra tests.
# TESTCODES_REQUIRED_TO_PASS +=

# Need to handle C source codes separately from C++ source codes
#    simple_windows_prog.c

TESTCODE_CURRENTLY_FAILING = 

noinst_headers = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# File option to accumulate performance information about the compilation
PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/MSWindows_ROSE_PERFORMANCE_DATA.csv

# Note that when we compile 32 bit code on a 64 bit machine the generated code should 
# not be compiled as 64 bit code so we used the option: -rose:skipfinalCompileStep
# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict $(PERFORMANCE_REPORT_OPTION)
ROSE_FLAGS = --edg:restrict --edg:no_warnings -w -rose:skipfinalCompileStep $(PERFORMANCE_REPORT_OPTION)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

# $(TEST_Objects): $(TEST_TRANSLATOR) $(srcdir)/$(@:.o=.C)
$(TEST_Objects): $(TEST_TRANSLATOR)
	$(VALGRIND) $(TEST_TRANSLATOR) $(ROSE_WINE_INCLUDES)/msvcrt $(ROSE_WINE_INCLUDES) $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.C)

QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): $(TEST_TRANSLATOR) $(srcdir)/$(@:.qmt=.C)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh f rose.RoseTest $(TEST_TRANSLATOR) NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

copyFiles:
	cp $(srcdir)/*.h $(top_srcdir)/tests/nonsmoke/functional/CompileTests/MicrosoftWindows_tests
	cp $(srcdir)/*.C $(top_srcdir)/tests/nonsmoke/functional/CompileTests/MicrosoftWindows_tests

# Flymake Support in ROSE
check-syntax:
	@$(compass_build_tooldir)/compass/compassMain $(CHK_SOURCES)

conditional-check-local:
	@echo "Dan Quinlan's development tests for C."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/MicrosoftWindows_tests: make check rule complete (terminated normally) ******"
	@echo "********************************************************************************************************************"
else
conditional-check-local:
	@echo "\nMicrosoft Windows development tests NOT run.  To run these reconfigure with:"
	@echo "**********************************************************************************"
	@echo "****** make check not run here: configure --with-wine=<WINE directory path> ******"
	@echo "**********************************************************************************"
endif

check-local: conditional-check-local

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.pdf *~ *.ps *.out rose_performance_report_lockfile.lock
	rm -rf QMTest


# EXTRA_DIST = CMakeLists.txt README $(ALL_TESTCODES)
EXTRA_DIST = README $(ALL_TESTCODES)
