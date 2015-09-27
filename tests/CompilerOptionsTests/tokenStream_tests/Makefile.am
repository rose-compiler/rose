include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##################################################################################
# These are C testcodes targeting aspects of C that might not be a subset of C++ #
##################################################################################

# if ROSE_USE_BOOST_WAVE

AM_CPPFLAGS = $(ROSE_INCLUDES)

# This is a shorted list that tests more quickly.
TESTCODES_REQUIRED_TO_PASS = \
inputCode_main.C 

TESTCODES_CURRENTLY_FAILING = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODES_CURRENTLY_FAILING)


VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0

$(TEST_Objects): main
	echo "Copy the file to build a second one to main ..."
#	$(VALGRIND) ./main $(ROSE_FLAGS) $(TESTCODE_AM_CPPFLAGS) -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)
	$(VALGRIND) ./main $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.C)

bin_PROGRAMS = main

main_SOURCES = \
   main.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# testMain: main
#	$(VALGRIND) ./main -c $(srcdir)/tests/inputCode_main.C


QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): main $(srcdir)/$(@:.qmt=.C)
	@echo "Calling QMTEST_Objects rule: "
#	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/main NULL $(ROSE_FLAGS) -c $(srcdir)/tests/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

# EXTRA_DIST = tests 

check-local:
	@echo "Development tests for token stream examples."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***************************************************************************************************************"
	@echo "****** ROSE/tests/CompilerOptionsTests/tokenStream_tests: make check rule complete (terminated normally) ******"
	@echo "***************************************************************************************************************"

# endif

clean-local:
	rm -f *.o rose_*.[cC] *.dot test200?_*.[cC]
	rm -rf QMTest

EXTRA_DIST = CMakeLists.txt $(ALL_TESTCODES)
