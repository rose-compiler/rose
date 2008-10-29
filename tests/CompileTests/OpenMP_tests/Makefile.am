include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

noinst_PROGRAMS = parseOmp
parseOmp_SOURCES = parseOmp.C

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

C_TESTCODES_REQUIRED_TO_PASS = \
	3loops.c \
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
	expressions.c \
	falsesharing.c \
	firstlastprivate.c \
	flush.c \
	get_max_threads.c \
	hello.c \
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
	parallel.c \
	parallelfor.c \
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
	variables.c

CXX_TESTCODES_REQUIRED_TO_PASS = \
	task_link.cpp \
	task_tree.cpp 

TESTCODE_CURRENTLY_FAILING =  

ROSE_FLAGS = -rose:openmp --edg:no_warnings -w -rose:verbose 0 --edg:restrict
# used to find omp.h, added it as one of  rose headers
#TESTCODE_INCLUDES = -I$(srcdir)

PASSING_C_TEST_Objects = ${C_TESTCODES_REQUIRED_TO_PASS:.c=.o}

PASSING_CXX_TEST_Objects = ${CXX_TESTCODES_REQUIRED_TO_PASS:.cpp=.o}

$(PASSING_C_TEST_Objects): parseOmp $(srcdir)/$(@:.o=.c)
	./parseOmp ${ROSE_FLAGS} ${TESTCODE_INCLUDES} -c $(srcdir)/$(@:.o=.c)

$(PASSING_CXX_TEST_Objects): parseOmp $(srcdir)/$(@:.o=.cpp)
	./parseOmp ${ROSE_FLAGS} ${TESTCODE_INCLUDES} -c $(srcdir)/$(@:.o=.cpp)

ALL_TESTCODES = \
${C_TESTCODES_REQUIRED_TO_PASS} \
${CXX_TESTCODES_REQUIRED_TO_PASS} \
${TESTCODE_CURRENTLY_FAILING}


check-local:
	@echo "Test for ROSE OpenMP Parser."
	@$(MAKE) $(PASSING_C_TEST_Objects) 
	@$(MAKE) $(PASSING_CXX_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/OpenMP_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES) README

clean-local:
	rm -f *.o rose_*.* *.out
	rm -rf QMTest
	
