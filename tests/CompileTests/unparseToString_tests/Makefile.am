include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = unparseToString_Type unparseToString_All

unparseToString_Type_SOURCES = unparseToString_Type.C
unparseToString_All_SOURCES = unparseToString_All.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TESTCODES_REQUIRED_TO_PASS = unparseToString_Type-Input.C

TESTCODE_CURRENTLY_FAILING = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
ROSE_FLAGS = --edg:no_warnings -w --edg:restrict

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): unparseToString_Type $(srcdir)/$(@:.o=.C)
$(TEST_Objects): unparseToString_All
	rm -rf rose_unparsedIRnodes.C
	./unparseToString_All $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.C) > rose_unparsedIRnodes.C

QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): unparseToString_Type $(srcdir)/$(@:.qmt=.C)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/unparseToString_All NULL $(ROSE_FLAGS) -c $(srcdir)/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

EXTRA_DIST = $(ALL_TESTCODES)

check-local:
	@echo "Tests for AST unparseToString."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/unparseToString_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC]
	rm -rf QMTest
