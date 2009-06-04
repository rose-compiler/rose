include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##########################################################################
# These are OpenMP C testcodes targeting aspects of OpenMP specific to C #
##########################################################################

# Shorten the list of code to compile to simplify checks using "make distcheck"
SHORT_TESTCODES_REQUIRED_TO_PASS = \
for_firstprivate.c \
for_lastprivate.c

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
main.c \
omp_pragma_example1.c \
omp_pragma_example2.c 

TESTCODE_CURRENTLY_FAILING = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

ROSE_FLAGS = -rose:C_only --edg:no_warnings -w -rose:verbose 0 --edg:restrict -rose:openmp -rose:openmp:ast_only

PASSING_TEST_Objects = ${TESTCODES:.c=.o}
TEST_Objects = ${ALL_TESTCODES:.c=.o}

$(TEST_Objects): ../parseOmp $(srcdir)/$(@:.o=.C)
	../parseOmp $(ROSE_FLAGS) -I$(top_srcdir)/src/frontend/SageIII -c $(srcdir)/$(@:.o=.c)
../parseOmp:
	$(MAKE) -C ../. parseOmp

QMTEST_Objects = ${ALL_TESTCODES:.c=.qmt}

# Make rule to build the QMTest database files
CURRENT_DIRECTORY = `pwd`
$(QMTEST_Objects): ../../testTranslator $(srcdir)/$(@:.qmt=.c)
	@echo "Calling QMTEST_Objects rule: "
	qm.sh L rose.RoseTest $(CURRENT_DIRECTORY)/../parseOmp NULL $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.qmt=.c)

# Include makefile rules specific to QMTest
include $(top_srcdir)/config/QMTest_makefile.inc

EXTRA_DIST = $(ALL_TESTCODES) omp_my_sleep.h  omp_testsuite.h

check-local:
	@echo "Tests for OpenMP C validation suite."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "****************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/OpenMP_tests/cvalidation: make check rule complete (terminated normally) ******"
	@echo "****************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC]
	rm -rf QMTest


