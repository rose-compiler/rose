
#SUBDIRS = fortranTests

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Default header file search path
INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#include $(srcdir)/Makefile-pass.inc
C_TESTCODES_REQUIRED_TO_PASS = \
        hello.c \
        parallel.c 

CXX_TESTCODES_REQUIRED_TO_PASS = 

# put macroCalls.c here since it requires -rose:wave, which causes seg fault on 64-bit machine

TESTCODE_CURRENTLY_FAILING = \
        3loops.c \
        jacobi.c \
        atomic.c \
        collapse.c \
        copyin.c \
        copyprivate2.c \
        copyprivate3.c \
        copyprivate.c \
        critical.c \
        critical_dead.c \
        critical_orphaned.c \
        dynamicChunk.c \
        empty.c \
        nowait.c \
        expressions.c \
        falsesharing.c \
        firstlastprivate.c \
        flush.c \
        get_max_threads.c \
	hello-2.c \
        hello-ordered.c \
        lastprivate0.c \
        lastprivate.c \
        limits_threads.c \
        lockarray.c \
        lu_factorization.c \
        masterSingle.c \
        matrix_vector.c \
        nestedpar1.c \
        nestedpar.c \
        omp1.c \
        ompfor.c \
        ordered2.c \
        ordered.c \
        orphaned-directives.c \
        macroCalls.c \
        parallelfor.c \
        parallelfor2.c \
        parallelsections.c \
        private.c \
        privatej.c \
        recursive.c \
        reduction2.c \
        reduction.c \
        section.c \
        set_num_threads.c \
        single_copyprivate.c \
        spmd1.c \
        staticChunk.c \
        subteam2.c \
        subteam.c \
        task_largenumber.c \
        task_orphaned.c \
        task_untied.c \
        task_underIf.c \
        task_wait.c \
        threadprivate2.c \
        threadprivate3.c \
        threadprivate.c \
        threadProcessor.c \
        variables.c \
        task_link.cpp \
        task_tree.cpp

# we reuse the OpenMP sources files from the OpenMP compile test directory
TEST_DIR = $(top_srcdir)/tests/CompileTests/OpenMP_tests
# the place to find omp.h, libgomp_g.h etc
TEST_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII -I$(top_srcdir)/src/midend/ompLowering

noinst_PROGRAMS = roseomp
roseomp_SOURCES = roseomp.C

# this flag is used for the long list of CXX test files
TEST_FLAGS = $(TEST_INCLUDES) -rose:openmp:lowering --edg:no_warnings #-rose:verbose 2 #--edg:restrict #-rose:wave 

PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.o)
PASSING_CXX_TEST_Objects = $(CXX_TESTCODES_REQUIRED_TO_PASS:.cpp=.o)

$(PASSING_C_TEST_Objects): %.o: $(TEST_DIR)/%.c roseomp
	./roseomp$(EXEEXT) ${TEST_FLAGS} -c $<

$(PASSING_CXX_TEST_Objects): %.o: $(TEST_DIR)/%.cpp roseomp
	./roseomp$(EXEEXT) ${TEST_FLAGS} -c $<

EXTRA_DIST = #Makefile-pass.inc
CLEANFILES = 

check-local: roseomp
	@echo "Test for ROSE OpenMP lowering."
	@echo "***************** Testing C input *******************"
	$(MAKE) $(PASSING_C_TEST_Objects)
	@echo "***************** Testing C++ input ******************"
	$(MAKE) $(PASSING_CXX_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/ompLoweringTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

clean-local:
	rm -f *.o test*.C rose_*.c rose_*.cpp rose_*.C *.pdf OUT_* *.dot 

