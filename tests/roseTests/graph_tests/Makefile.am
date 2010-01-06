include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##################################################################################
# These are testcodes for testing the new Graph IR nodes in ROSE
##################################################################################

INCLUDES = $(ROSE_INCLUDES)

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

$(TEST_Objects):
	echo "Copy the file to build a second one to main ..."
	$(VALGRIND) ./graph_test_1 $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)

bin_PROGRAMS = hash_multimap graph_test_1 graph_test_2 graph_test_3 graph_test_4 graph_test_5 graph_test_6 

hash_multimap_SOURCES = hash_multimap.C
graph_test_1_SOURCES = graph_test_1.C
graph_test_2_SOURCES = graph_test_2.C
graph_test_3_SOURCES = graph_test_3.C
graph_test_4_SOURCES = graph_test_4.C
graph_test_5_SOURCES = graph_test_5.C
graph_test_6_SOURCES = graph_test_6.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

test_hash_multimap: hash_multimap
	$(VALGRIND) ./hash_multimap

test_1: graph_test_1
	$(VALGRIND) ./graph_test_1

test_2: graph_test_2
	$(VALGRIND) ./graph_test_2

test_3: graph_test_3
	$(VALGRIND) ./graph_test_3

test_4: graph_test_4
	$(VALGRIND) ./graph_test_4 $(srcdir)/inputCode_main.C

test_5: graph_test_5
	$(VALGRIND) ./graph_test_5 $(srcdir)/inputCode_main.C

test_6: graph_test_6
	$(VALGRIND) ./graph_test_6 $(srcdir)/inputCode_main.C

small_binary_test_6: graph_test_6
	$(VALGRIND) ./graph_test_6 $(srcdir)/../Disassembler_tests/x86-64-ctrlaltdel

large_binary_test_6: graph_test_6
	$(VALGRIND) ./graph_test_6 $(srcdir)/../Disassembler_tests/pocketputty.exe


QMTEST_Objects = ${ALL_TESTCODES:.C=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): graph_test_1 $(srcdir)/$(@:.qmt=.C)
	@echo "Calling QMTEST_Objects rule: "
#	qm.sh f rose.RoseTest $(CURRENT_DIRECTORY)/main NULL $(ROSE_FLAGS) -c $(srcdir)/tests/$(@:.qmt=.C)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

check-local:
	@echo "Development tests for new graph IR nodes."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "**********************************************************************************************"
	@echo "****** ROSE/tests/roseTests/graph_tests: make check rule complete (terminated normally) ******"
	@echo "**********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot test200?_*.[cC]
	rm -rf QMTest

EXTRA_DIST = CMakeLists.txt
