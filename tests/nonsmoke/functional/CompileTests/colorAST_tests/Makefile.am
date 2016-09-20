include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

bin_PROGRAMS = \
   testAstVisualization

testAstVisualization_SOURCES = testAstVisualization.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TESTCODES_REQUIRED_TO_PASS = \
     inputExample.C

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
TEST_Objects = ${ALL_TESTCODES:.C=.o}

$(TEST_Objects): $(bin_PROGRAMS)
	./testAstVisualization $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.C)

QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): $(bin_PROGRAMS) $(srcdir)/$(@:.qmt=.C)
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/graphWholeAST NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

EXTRA_DIST = $(ALL_TESTCODES)

check-local:
	@echo "Tests for Color Graphing examples."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "****************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/colorAST_tests: make check rule complete (terminated normally) ******"
	@echo "****************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot 
	rm -rf QMTest
