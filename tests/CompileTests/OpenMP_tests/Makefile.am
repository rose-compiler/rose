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

AM_CPPFLAGS = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

C_TESTCODES_REQUIRED_TO_PASS = \
	3loops.c \
	jacobi.c \
	jacobi-collapse.c \
	alignment.c \
	array_init.c \
	array_init_2.c \
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
	firstPrivateArray_2.c \
	firstlastprivate.c \
	flush.c \
	flush_exampleA_21_1c.c \
	full_verify.c \
	get_max_threads.c \
	hello.c \
	hello-1.c \
	hello-2.c \
	hello-ordered.c \
	include_header_only.c \
	init.c \
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
	matrixmultiply.c \
	md_open_mp.c \
	multiple_return.c \
	nestedpar1.c \
	nestedpar.c \
	omp1.c \
	ompfor.c \
	ompfor_c99.c \
	ompfor2.c \
	ompfor3.c \
	ompfor4.c \
	ompfor5.c \
	ompfor6.c \
	ompfor-default.c \
	ompfor-decremental.c \
	ompfor-static.c \
	ompGetNumThreads.c \
	omp_sections.c \
	ordered2.c \
	ordered.c \
	orphaned-directives.c \
	parallel.c \
	parallel-if.c \
	parallel-if-numthreads.c \
	parallel-numthreads.c \
	parallel-reduction.c \
	parallel-reduction2.c \
	parallelfor.c \
	parallelfor2.c \
	parallelsections.c \
	preprocessingInfo.c \
	prettyfunc.c \
	private.c \
	privatej.c \
	private-duplicate.c \
	recursive.c \
	reduction2.c \
	reduction.c \
	reduction-classic.c \
	rice1.c \
	section.c \
	section1.c \
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
	task_array.c \
	task_array2.c \
	task_largenumber.c \
	task_orphaned.c \
	task_outlining.c \
	task_untied.c \
	task_untied2.c \
	task_untied3.c \
	task_untied4.c \
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

C_OMP_ACC_TESTCODES_REQUIRED_TO_PASS = \
        axpy_ompacc.c \
        axpy_ompacc2.c \
	axpy_ompacc3.c \
	axpy_ompacc_parseonly.c \
	matrixmultiply-ompacc.c \
	matrixmultiply-ompacc2.c \
	jacobi-ompacc.c \
	jacobi-ompacc-v2.c \
	jacobi-ompacc-opt1.c \
	jacobi-ompacc-opt2.c \
	jacobi-ompacc-multiGPU.c

CXX_TESTCODES_REQUIRED_TO_PASS = \
        jacobi-ompacc2.cpp \
	classMember.cpp \
	hello1.cpp \
	helloNested.cpp \
	memberFunction.cpp \
	memberFunction2.cpp \
	objectPrivate.cpp \
	objectFirstPrivate.cpp \
	objectLastprivate.cpp \
	orphanedAtomic.cpp \
	preprocessingInfo2.cpp \
	task_link.cpp \
	task_link2.cpp \
	task_tree.cpp \
	referenceType.cpp

# put macroCalls.c here since it requires -rose:wave, which causes seg fault on 64-bit machine
TESTCODE_CURRENTLY_FAILING = \
	ompfor7.c \
	macroCalls.c 

ROSE_FLAGS = -rose:openmp:ast_only --edg:no_warnings -w -rose:verbose 0 --edg:restrict #-rose:wave
# used to find omp.h, added it as one of  rose headers
TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII

PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.o)

PASSING_CXX_TEST_Objects = $(CXX_TESTCODES_REQUIRED_TO_PASS:.cpp=.o)

#C_OMP_ACC_TEST_Objects = $(C_OMP_ACC_TESTCODES_REQUIRED_TO_PASS:.c=.o)
# we only care about unparsing for ACC input, not the object file generation
C_OMP_ACC_TEST_Unparsed = $(addprefix rose_,${C_OMP_ACC_TESTCODES_REQUIRED_TO_PASS})

$(PASSING_C_TEST_Objects): %.o: $(srcdir)/%.c parseOmp
	./parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $<

$(PASSING_CXX_TEST_Objects): %.o: $(srcdir)/%.cpp parseOmp
	./parseOmp ${ROSE_FLAGS} ${TESTCODE_INCLUDES} -c $<

check-c:$(PASSING_C_TEST_Objects) 
check-cxx:$(PASSING_CXX_TEST_Objects)

ALL_TESTCODES = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(C_OMP_ACC_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# Liao 11/18/2009
# Test for a bug which happens when more than two source files show in the command line
# the translator should now be able to avoid duplicated processing of pragma statements
bonds-2.o: $(srcdir)/atoms-2.c $(srcdir)/bonds-2.c parseOmp
	./parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/atoms-2.c $(srcdir)/bonds-2.c

macroIds.o: $(srcdir)/macroIds.c parseOmp
	./parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -rose:collectAllCommentsAndDirectives $(srcdir)/macroIds.c

# test parsing experimental OpenMP accelerator directives, Liao 1/16/2013
#$(C_OMP_ACC_TEST_Objects): %.o:$(srcdir)/%.c parseOmp
#	./parseOmp -rose:openmp:ast_only -rose:skipfinalCompileStep --edg:no_warnings -w -rose:verbose 0 --edg:restrict -c $<
$(C_OMP_ACC_TEST_Unparsed): $(srcdir)/$(@:rose_%=%) parseOmp
	./parseOmp -rose:openmp:ast_only -rose:skipfinalCompileStep --edg:no_warnings -w -rose:verbose 0 --edg:restrict -c $(srcdir)/$(@:rose_%=%)

if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
check-local:
	@echo "Test for ROSE OpenMP Parser."
	@$(MAKE) $(PASSING_C_TEST_Objects) 
	@$(MAKE) $(PASSING_CXX_TEST_Objects)
	@$(MAKE) bonds-2.o macroIds.o $(C_OMP_ACC_TEST_Unparsed)
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

EXTRA_DIST = $(ALL_TESTCODES) README bonds-2.c \
			 macroIds.c macroIds.h macroIdsDef.h \
			 collapse_2.c 

clean-local:
	rm -f *.o rose_*.* *.out
	rm -rf QMTest


