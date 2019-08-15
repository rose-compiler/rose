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
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

# put macroCalls.c here since it requires -rose:wave, which causes seg fault on 64-bit machine
TESTCODE_CURRENTLY_FAILING = \
	ompfor7.c \
	macroCalls.c \
	data-member-access.cpp \
	define-directive2.cpp \
	define-directive.c

C_TESTCODES_REQUIRED_TO_PASS = \
	3loops.c \
	affinity.c \
	jacobi.c \
	jacobi-collapse.c \
	alignment.c \
	array_init.c \
	array_init_2.c \
	atomic.c \
	atoms-2.c \
	atomic-3.c \
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
	ompfor8.c \
	ompfor-default.c \
	ompfor-decremental.c \
	ompfor-static.c \
	ompGetNumThreads.c \
	omp_sections.c \
	ordered3.c \
	ordered2.c \
	ordered.c \
	orphaned-directives.c \
	parallel.c \
	parallel-if.c \
	parallel-if-numthreads.c \
	parallel-numthreads.c \
	parallel-reduction.c \
	parallel-reduction2.c \
	parallel_pragma.c \
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
	reduction_min_max.c \
	rice1.c \
	section.c \
	section1.c \
	set_num_threads.c \
	shared.c \
	simd.c \
	simd2.c \
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
	task_dep1.c \
	task_dep2.c \
	task_dep3.c \
	task_dep.load.compute.c \
	task_final.c \
	task_largenumber.c \
	task_mergeable.c \
	task_orphaned.c \
	task_outlining.c \
	task_priority.c \
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

# removed experimental test cases	
removed_Experimental_TEST = \
	axpy_ompacc_mpi.c \
	axpy_ompacc4.c \
	axpy_ompacc_parseonly.c 



# DQ (2/19/2017): This logic has not yet been updated to reflect the new logic for GNU, Clang, Intel compilers.
# It might be that it need not be updated since it is sufficently portable as it is (if not then it will be updated).
if USING_CLANG_COMPILER
C_OMP_ACC_TESTCODES_REQUIRED_TO_PASS = \
        axpy_ompacc.c \
        axpy_ompacc2.c

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
	task_tree.cpp 
else
C_OMP_ACC_TESTCODES_REQUIRED_TO_PASS = \
        axpy_ompacc.c \
        axpy_ompacc2.c \
	axpy_ompacc3.c \
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
endif

if !USING_INTEL_COMPILER
C_TESTCODES_REQUIRED_TO_PASS += \
	init.c 
endif

#C_OMP_ACC_TEST_RESULTS = $(C_OMP_ACC_TESTCODES_REQUIRED_TO_PASS:.c=.o)
# we only care about unparsing for ACC input, not the object file generation
C_OMP_ACC_TEST_Unparsed = $(addprefix rose_,${C_OMP_ACC_TESTCODES_REQUIRED_TO_PASS})

ROSE_FLAGS = -rose:openmp:ast_only --edg:no_warnings -w -rose:verbose 0 --edg:restrict #-rose:wave
# used to find omp.h, added it as one of  rose headers
TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII

PASSING_C_TEST_RESULTS = $(addprefix rose_,${C_TESTCODES_REQUIRED_TO_PASS})
PASSING_CXX_TEST_RESULTS = $(addprefix rose_,${CXX_TESTCODES_REQUIRED_TO_PASS})

# rose_input.c/cpp 
PASSING_C_CXX_TEST_RESULTS = $(PASSING_C_TEST_RESULTS) $(PASSING_CXX_TEST_RESULTS) 

# DQ (11/30/2018): We have a combination of folded and non-folded values for macros in OpenMP pragmas 
# (Liao is looking into this) and we decided to comment this out in my testing while he does this.
# PASSING_C_CXX_ACC_TEST_RESULTS=$(PASSING_C_CXX_TEST_RESULTS) $(C_OMP_ACC_TEST_Unparsed) rose_bonds-2.c rose_macroIds.c
PASSING_C_CXX_ACC_TEST_RESULTS=$(PASSING_C_CXX_TEST_RESULTS) $(C_OMP_ACC_TEST_Unparsed) rose_bonds-2.c

PASSING_C_CXX_ACC_TEST_OUTPUT=$(addsuffix .output, $(PASSING_C_CXX_ACC_TEST_RESULTS))

$(PASSING_C_CXX_TEST_RESULTS): $(srcdir)/$(@:rose_%=%) parseOmp
	./parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:rose_%=%) 

$(PASSING_C_CXX_ACC_TEST_OUTPUT): %.output:% 
	grep "pragma omp" $(@:%.output=%) > $@ || exit 0;

PASSING_C_CXX_ACC_TEST_OUTPUT_DIFF=$(addsuffix .diff, $(PASSING_C_CXX_ACC_TEST_OUTPUT))

REFERENCE_PATH=$(top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenMP_tests/referenceResults

$(PASSING_C_CXX_ACC_TEST_OUTPUT_DIFF): %.diff:%
	if diff $(@:.output.diff=.output) $(REFERENCE_PATH)/$(@:.output.diff=.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; rm -rf $@; exit 1; fi

gen-output: $(PASSING_C_CXX_ACC_TEST_OUTPUT)

check-c:$(PASSING_C_TEST_RESULTS) 
check-cxx:$(PASSING_CXX_TEST_RESULTS)

ALL_TESTCODES = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(C_OMP_ACC_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# Liao 11/18/2009
# Test for a bug which happens when more than two source files show in the command line
# the translator should now be able to avoid duplicated processing of pragma statements
rose_bonds-2.c: $(srcdir)/atoms-2.c $(srcdir)/bonds-2.c parseOmp
	./parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/atoms-2.c $(srcdir)/bonds-2.c

rose_macroIds.c: $(srcdir)/macroIds.c parseOmp
	./parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -rose:collectAllCommentsAndDirectives $(srcdir)/macroIds.c

# we now experiment with multi gpu and MPI support, test parse_only without OpenMP-specific AST creation for now. Liao10/28/2015
# test parsing experimental OpenMP accelerator directives, Liao 1/16/2013
$(C_OMP_ACC_TEST_Unparsed): $(srcdir)/$(@:rose_%=%) parseOmp
	./parseOmp -rose:openmp:ast_only -rose:skipfinalCompileStep --edg:no_warnings -w -rose:verbose 0 --edg:restrict -c $(srcdir)/$(@:rose_%=%)
#	./parseOmp -rose:openmp:parse_only -rose:skipfinalCompileStep --edg:no_warnings -w -rose:verbose 0 --edg:restrict -c $(srcdir)/$(@:rose_%=%)

if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
conditional-check-local:
	@echo "Test for ROSE OpenMP Parser."
#	@$(MAKE) $(PASSING_C_TEST_RESULTS) 
#	@$(MAKE) $(PASSING_CXX_TEST_RESULTS)
#	@$(MAKE) bonds-2.o macroIds.o $(C_OMP_ACC_TEST_Unparsed)
##	@$(MAKE) $(PASSING_TEST_RESULTS)
# DQ (2/19/2017): NEW VERSION OF LOGIC
# DQ (2/18/2017): More specific logic required for portability.
# When using EDG 4.9 these files don't compile on later versions of GNU, CLANG, and Intel compilers 
# but we still want them tests as we transition to EDG 4.12 (where they all work fine independent of 
# the compiler vendor and GNU compiler version).
if ROSE_USE_EDG_VERSION_4_9
	@echo "ROSE_USE_EDG_VERSION_4_9 == true"
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
if ROSE_USING_GCC_VERSION_LATER_4_9
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == true"
	@echo "OpenMP tests using EDG 4.9 and GNU version 4.9 backend compiler are disabled."
# Exclude these tests on GNU 4.9 and later compilers).
else  # NOT ROSE_USING_GCC_VERSION_LATER_4_8
# If this is not the CLANG or Intel compilers and an earlier version of GNU than GNU 4.9 compiler, so include these files.
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == false"
	@$(MAKE) $(PASSING_C_CXX_ACC_TEST_OUTPUT_DIFF)
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else  # NOT USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
	@echo "OpenMP tests using EDG 4.9: Clang backend compiler are NOT allowed with the EDG 4.9 frontend."
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
	@echo "OpenMP tests using EDG 4.9 Intel backend compiler are allowed."
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
else  # NOT ROSE_USE_EDG_VERSION_4_9
# Using EDG 4.12 or later (run all tests independent of the compiler).
	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
if USING_GNU_COMPILER
if ROSE_USING_GCC_VERSION_LATER_4_8
	@echo "OpenMP tests require configuration with either EDG version 4.9 and GNU version 4.8 or backend compiler or EDG 4.12 and 4.8 or any later GNU compiler."
	@echo "OpenMP tests using EDG 4.12 and any supported backend compiler are allowed."
	@$(MAKE) $(PASSING_C_CXX_ACC_TEST_OUTPUT_DIFF)
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else
	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
	@echo "OpenMP tests using EDG 4.9: Clang backend compiler using EDG 4.12 frontend."
	@$(MAKE) $(PASSING_C_CXX_ACC_TEST_OUTPUT_DIFF)
# DQ (2/28/2017): Skip these files with Clang for now and come back to them later (when we understand the limits of portability better).
#	@$(MAKE) $(PASSING_TEST_RESULTS)
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
	@echo "OpenMP tests using EDG 4.12 Intel backend compiler are allowed."
	@$(MAKE) $(PASSING_C_CXX_ACC_TEST_OUTPUT_DIFF)
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
endif # ROSE_USE_EDG_VERSION_4_9
	@echo "**********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/OpenMP_tests: make check rule complete (terminated normally) ******"
	@echo "**********************************************************************************************************************"
else
conditional-check-local:
	@echo "Test for ROSE OpenMP Parser."
	@echo "*******************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/OpenMP_tests: make check rule for C/C++ skipped (terminated normally) ******"
	@echo "*******************************************************************************************************************************"
endif

EXTRA_DIST = $(ALL_TESTCODES) README bonds-2.c \
			 macroIds.c macroIds.h macroIdsDef.h \
			 collapse_2.c 

check-local: conditional-check-local

clean-local:
	rm -f *.o rose_*.* *.out
	rm -rf QMTest


