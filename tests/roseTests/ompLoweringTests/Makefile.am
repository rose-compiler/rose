include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Default header file search path
INCLUDES = $(ROSE_INCLUDES)
roseomp_LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

GOMP_PATH = @gomp_omp_runtime_library_path@

# all input files are put into tests/CompileTests/OpenMP_tests
# They are shared by both OpenMP parsing tests and OpenMP lowering tests
C_TESTCODES_REQUIRED_TO_PASS = \
        3loops.c \
        atomic.c \
        barrier.c \
        critical.c \
        critical_orphaned.c \
        dynamicChunk.c \
        empty.c \
        falsesharing.c \
        firstlastprivate.c \
        firstprivate.c \
        get_max_threads.c \
        hello-1.c \
	hello-2.c \
        hello-ordered.c \
        hello.c \
        jacobi.c \
        lastprivate.c \
        lastprivate0.c \
        lockarray.c \
        nestedpar.c \
        nestedpar1.c \
        masterSingle.c \
        nowait.c \
        ordered.c \
        ordered2.c \
        omp1.c \
        ompfor.c \
        ompfor2.c \
        ompfor3.c \
        ompfor4.c \
        orphaned-directives.c \
        parallel.c \
	parallel-reduction.c \
        parallelfor.c \
        parallelfor2.c \
        private.c \
        privatej.c \
        reduction.c \
        reduction2.c \
        set_num_threads.c \
	single.c \
        spmd1.c \
        staticChunk.c \
        subteam.c \
        subteam2.c
# a subset of C tests can be executed	
# Put their object files with main entries here
C_MAIN_Objects = \
        3loops.o \
        atomic.o \
        critical.o \
        critical_orphaned.o \
        empty.o \
        falsesharing.o \
        firstlastprivate.o \
        firstprivate.o \
        get_max_threads.o \
        hello-1.o \
	hello-2.o \
        hello-ordered.o \
        hello.o \
        jacobi.o \
        lastprivate.o \
        lastprivate0.o \
        lockarray.o \
        nestedpar.o \
        nestedpar1.o \
        masterSingle.o \
        ordered.o \
        ordered2.o \
        omp1.o \
        ompfor.o \
        ompfor2.o \
        ompfor3.o \
        ompfor4.o \
        orphaned-directives.o \
        parallel.o \
	parallel-reduction.o \
        parallelfor.o \
        parallelfor2.o \
	private.o \
        privatej.o \
        reduction.o \
        reduction2.o \
        set_num_threads.o \
	single.o \
	subteam.o \
	subteam2.o \
        spmd1.o

CXX_TESTCODES_REQUIRED_TO_PASS = 

# a subset of Cxx tests can be executed	
CXX_MAIN_Objects = 

# TODO: npb, validation suite, developer tests, epcc
TESTCODE_CURRENTLY_FAILING = \
        collapse.c \
        copyin.c \
        copyprivate.c \
        copyprivate2.c \
        copyprivate3.c \
        critical_dead.c \
        expressions.c \
        flush.c \
        macroCalls.c \
        parallelsections.c \
        recursive.c \
        section.c \
        single_copyprivate.c \
        task_largenumber.c \
        task_link.cpp \
        task_orphaned.c \
        task_underIf.c \
        task_untied.c \
        task_wait.c \
        threadProcessor.c \
        threadprivate.c \
        threadprivate2.c \
        threadprivate3.c \
        variables.c \
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


if WITH_GOMP_OPENMP_LIB
#-------------compile and run--------------------------------	
# Only for C or CXX tests will main()
# a subset of PASSING_C_TEST_Objects and PASSING_CXX_TEST_Objects
TEST_MAIN_Objects = $(C_MAIN_Objects) $(CXX_MAIN_Objects)	
PASSING_TEST_Executables = ${TEST_MAIN_Objects:.o=.out}
# TODO this might have problem with CXX objects!!
MY_FINAL_LINK = -L$(GOMP_PATH) -lgomp -lpthread -lm
%.out: %.o
	$(CC) $< -o $@ $(MY_FINAL_LINK)
check_PROGRAM = $(PASSING_TEST_Executables)
# Executables depend on objects
# check-TESTS happens before check-local
TESTS =  $(check_PROGRAM)
check-local: roseomp
	@echo "Test for ROSE OpenMP lowering."
	@echo "***************** Testing C input *******************"
	$(MAKE) $(PASSING_C_TEST_Objects)
	@echo "***************** Testing C++ input ******************"
	$(MAKE) $(PASSING_CXX_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/ompLoweringTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

else
check-local: roseomp
	@echo "Test for ROSE OpenMP lowering."
	@echo "***************** Testing C input *******************"
	$(MAKE) $(PASSING_C_TEST_Objects)
	@echo "***************** Testing C++ input ******************"
	$(MAKE) $(PASSING_CXX_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/ompLoweringTests:                              *******************************"
	@echo "****       compilation tests completed but executables are not generated and run***************************"
	@echo "****** configure option --with-gomp_omp_runtime_library to enable testing execution of OpenMP   ******"
	@echo "****** for example: --with-gomp_omp_runtime_library=/home/liao6/opt/gcc-4.4.1/lib/              ********"
	@echo "***********************************************************************************************************"
endif


clean-local:
	rm -f *.o test*.C rose_*.c rose_*.cpp rose_*.C *.pdf OUT_* *.dot *.out 
	
EXTRA_DIST = 

CLEANFILES = 



