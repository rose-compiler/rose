include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODES_REQUIRED_TO_PASS = \
for_firstprivate.c \
for_lastprivate.c \
for_ordered.c \
for_private.c \
for_reduction.c \
for_schedule_dynamic.c \
for_schedule_guided.c \
for_schedule_static.c \
has_openmp.c \
omp_atomic.c \
omp_barrier.c \
omp_copyin.c \
omp_critical.c \
omp_flush.c \
omp_get_num_threads.c \
omp_in_parallel.c \
omp_lock.c \
omp_master_thread.c \
omp_nested.c \
omp_nest_lock.c \
omp_nest_testlock.c \
omp_num_threads.c \
omp_testlock.c \
omp_threadprivate.c \
omp_ticks_time.c \
omp_time.c \
parallel_for_firstprivate.c \
parallel_for_lastprivate.c \
parallel_for_ordered.c \
parallel_for_private.c \
parallel_for_reduction.c \
parallel_section_firstprivate.c \
parallel_section_lastprivate.c \
parallel_section_private.c \
parallel_section_reduction.c \
section_firstprivate.c \
section_lastprivate.c \
section_private.c \
section_reduction.c \
single.c \
single_copyprivate.c \
single_nowait.c \
single_private.c \
main.c 
# Must let ROSE OpenMP translator process main.c to add necessary variables and code supporting OpenMP

TESTCODE_CURRENTLY_FAILING = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# Fails when -rose:C_only is used
# ROSE_FLAGS = -rose:verbose 2 --edg:restrict
# ROSE_FLAGS = --edg:no_warnings -w -rose:C_only -rose:verbose 0 --edg:restrict
ROSE_FLAGS = --edg:no_warnings -w -rose:C_only -rose:verbose 0 --edg:restrict -rose:detect_dangling_pointers 1

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

PASSING_TEST_Objects = ${TESTCODES:.c=.o}
TEST_Objects = ${ALL_TESTCODES:.c=.o}

$(TEST_Objects): ../../ompTranslator $(@:.o=.c)
#	@echo "Calling Copy AST mechanism using TEST_Objects rule: "
	$(VALGRIND) ../../ompTranslator -D_OPENMP $(ROSE_FLAGS) -I$(srcdir)/../.. -I$(srcdir)/../../includes -c $(srcdir)/$(@:.o=.c)

QMTEST_Objects = ${ALL_TESTCODES:.c=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): ../../ompTranslator $(@:.qmt=.c)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh o rose.RoseTest $(CURRENT_DIRECTORY)/../../ompTranslator NULL $(ROSE_FLAGS) -I$(srcdir)/../.. -c $(srcdir)/$(@:.qmt=.c)

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

OMNI_OPENMP_RUNTIME_LIBRARY = @omni_omp_runtime_support_path@

if ROSE_USE_OMNI_OPENMP
# enable make check to build and run the final executable, Liao 1/31/2008
INCLUDES= -I$(srcdir)/../../includes 
LDADD = -L$(OMNI_OPENMP_RUNTIME_LIBRARY) -lompc -lpthread -lm
main_SOURCES=$(TESTCODES)
check_PROGRAMS = main
TESTS = $(check_PROGRAMS)

else
#check-local:$(PASSING_TEST_Objects)
check-local:
	@echo "Test for ROSE OpenMP Translator."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/OpenMP_Translator/tests/cvalidationsuite: make check rule complete (terminated normally) ******"
	@echo "****** make check not run here: configure --with-omni_omp_runtime_support=<path> ******"
	@echo "***********************************************************************************************************"
endif

# else
# check-local:
#	@echo "\nOpenMP tests NOT run.  To run these reconfigure with:"
#	@echo "****************************************************************************"
#	@echo "****** make check not run here: configure --with-omp_runtime_support=<path> ******"
#	@echo "****************************************************************************"
# endif

EXTRA_DIST = omp_my_sleep.h omp_testsuite.h $(ALL_TESTCODES)

clean-local:
	rm -f *.o rose_*.[cC] main test.log
	rm -rf QMTest

