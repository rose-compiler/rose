include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

noinst_PROGRAMS = parseOmp
parseOmp_SOURCES = parseOmp.C

SUBDIRS=
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
  SUBDIRS += . cvalidation
endif
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  SUBDIRS += fortran 
endif

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

C_TESTCODES_REQUIRED_TO_PASS = \
	3loops.c \
	jacobi.c \
	alignment.c \
	atomic.c \
	atoms-2.c \
	barrier.c \
	collapse.c \
	copyin.c \
	copyprivate2.c \
	copyprivate3.c \
	copyprivate.c \
	critical.c \
	critical_dead.c \
	critical_orphaned.c \
	dijkstra_open_mp.c \
	dynamicChunk.c \
	empty.c \
	endif.c \
	endif2.c \
	endif3.c \
	nowait.c \
	expressions.c \
	falsesharing.c \
	firstprivate.c \
	firstPrivateArray.c \
	firstlastprivate.c \
	flush.c \
	flush_exampleA_21_1c.c \
	full_verify.c \
	get_max_threads.c \
	hello.c \
	hello-1.c \
	hello-2.c \
	hello-ordered.c \
	lastprivate0.c \
	lastprivate.c \
	lastprivateOrphaned.c \
	limits_threads.c \
	linebreak.c \
	lockarray.c \
	loop1.c \
	lu_factorization.c \
	master.c \
	masterSingle.c \
	matrix_vector.c \
	nestedpar1.c \
	nestedpar.c \
	omp1.c \
	ompfor.c \
	ompfor2.c \
	ompfor3.c \
	ompfor4.c \
	ompfor5.c \
	ompfor6.c \
	ompfor-default.c \
	ompfor-decremental.c \
	ompfor-static.c \
	ompGetNumThreads.c \
	ordered2.c \
	ordered.c \
	orphaned-directives.c \
	parallel.c \
	parallel-reduction.c \
	parallel-reduction2.c \
	parallelfor.c \
	parallelfor2.c \
	parallelsections.c \
	preprocessingInfo.c \
	private.c \
	privatej.c \
	recursive.c \
	reduction2.c \
	reduction.c \
	rice1.c \
	section.c \
	set_num_threads.c \
	shared.c \
	single.c \
	single2.c \
	single_copyprivate.c \
	sizeof.c \
	spmd1.c \
	staticChunk.c \
	subteam2.c \
	subteam.c \
	task_largenumber.c \
	task_orphaned.c \
	task_untied.c \
	task_untied2.c \
	task_untied3.c \
	task_underIf.c \
	task_wait.c \
	task_wait2.c \
	twoRegions.c \
	threadprivate2.c \
	threadprivate3.c \
	threadprivate.c \
	threadProcessor.c \
	upperCase.c \
	variables.c

CXX_TESTCODES_REQUIRED_TO_PASS = \
	classMember.cpp \
	hello1.cpp \
	helloNested.cpp \
	memberFunction.cpp \
	objectPrivate.cpp \
	objectFirstPrivate.cpp \
	objectLastprivate.cpp \
	orphanedAtomic.cpp \
	preprocessingInfo2.cpp \
	task_link.cpp \
	task_link2.cpp \
	task_tree.cpp 

# put macroCalls.c here since it requires -rose:wave, which causes seg fault on 64-bit machine
TESTCODE_CURRENTLY_FAILING = \
	ompfor7.c \
	macroCalls.c 

ROSE_FLAGS = -rose:openmp:ast_only --edg:no_warnings -w -rose:verbose 0 --edg:restrict #-rose:wave
# used to find omp.h, added it as one of  rose headers
TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII

PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.o)

PASSING_CXX_TEST_Objects = $(CXX_TESTCODES_REQUIRED_TO_PASS:.cpp=.o)

$(PASSING_C_TEST_Objects): %.o: $(srcdir)/%.c parseOmp
	./parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $<

$(PASSING_CXX_TEST_Objects): %.o: $(srcdir)/%.cpp parseOmp
	./parseOmp ${ROSE_FLAGS} ${TESTCODE_INCLUDES} -c $<

ALL_TESTCODES = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# Liao 11/18/2009
# Test for a bug which happens when more than two source files show in the command line
# the translator should now be able to avoid duplicated processing of pragma statements
bonds-2.o: $(srcdir)/atoms-2.c $(srcdir)/bonds-2.c parseOmp
	./parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/atoms-2.c $(srcdir)/bonds-2.c

if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
check-local:
	@echo "Test for ROSE OpenMP Parser."
	@$(MAKE) $(PASSING_C_TEST_Objects) 
	@$(MAKE) $(PASSING_CXX_TEST_Objects)
	@$(MAKE) bonds-2.o
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/OpenMP_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"
else
check-local:
	@echo "Test for ROSE OpenMP Parser."
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/OpenMP_tests: make check rule for C/C++ skipped (terminated normally) ******"
	@echo "***********************************************************************************************************"
endif

EXTRA_DIST = $(ALL_TESTCODES) README bonds-2.c

clean-local:
	rm -f *.o rose_*.* *.out
	rm -rf QMTest


