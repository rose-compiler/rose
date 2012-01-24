include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODES_REQUIRED_TO_PASS = bt.c

TESTCODE_CURRENTLY_FAILING = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# ROSE_FLAGS = -rose:C_only -rose:verbose 2 --edg:restrict
# ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict
ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict -rose:detect_dangling_pointers 2

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

PASSING_TEST_Objects = ${TESTCODES:.c=.o}
TEST_Objects = ${ALL_TESTCODES:.c=.o}

$(TEST_Objects): ../../../ompTranslator $(srcdir)/$(@:.o=.c)
	$(VALGRIND) ../../../ompTranslator -D_OPENMP $(ROSE_FLAGS) -I$(srcdir) -I$(srcdir)/../common -I$(srcdir)/../../.. -I$(srcdir)/../../../includes -c $(srcdir)/$(@:.o=.c)

QMTEST_Objects = ${ALL_TESTCODES:.c=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): ../../../ompTranslator $(srcdir)/$(@:.qmt=.c)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh o rose.RoseTest $(CURRENT_DIRECTORY)/../../../ompTranslator NULL $(ROSE_FLAGS) -I$(srcdir)/../../.. -c $(srcdir)/$(@:.qmt=.c)

# This is a rule to add the current files into a QMTest database
build_qmtest_database: $(QMTEST_Objects)
#	@echo "Building QMTest database using file "
	rm -rf QMTest;
	mkdir QMTest;
	rm -rf QMTest/*.qmt
	mv *.qmt QMTest
	cd QMTest; qmtest -D`pwd` create-tdb

run_qm_tests:
	cd QMTest; qmtest run -o TestResults.qmr -j4

run_qm_gui:
	cd QMTest; qmtest gui

check-local:
	@echo "Test for ROSE OpenMP Translator."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/OpenMP_Translator/tests/npb2.3-omp-c/BT: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = header.h  npbparams.h bt.cfg inputbt.data.sample $(ALL_TESTCODES)

clean-local:
	rm -f *.o rose_*.[cC] runme test.log
	rm -rf QMTest

