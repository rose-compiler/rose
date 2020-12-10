include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS= .
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
SUBDIRS += fortran
endif

# Default header file search path
AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
roseomp_LDADD = $(ROSE_LIBS)
roseompacc_LDADD = $(ROSE_LIBS)

GOMP_PATH = @gomp_omp_runtime_library_path@
OMNI_PATH = @omni_omp_runtime_support_path@

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

# all input files are put into tests/nonsmoke/functional/CompileTests/OpenMP_tests
# They are shared by both OpenMP parsing tests and OpenMP lowering tests

# Put test codes without main() entry here
# They will be compiled only, not executed.
#	ompfor8.c  , a case not yet supported: ordered schedule(static) , no chunk size is specified. 
C_TESTCODES_REQUIRED_TO_COMPILE = \
	alignment.c \
	collapse.c \
	dijkstra_open_mp.c \
	dynamicChunk.c \
	endif.c \
	endif3.c \
	full_verify.c \
        include_header_only.c \
	init.c \
	lastprivateOrphaned.c \
	linebreak.c \
	master.c \
	md_open_mp.c \
	nowait.c \
	ompfor5.c \
	ompfor6.c \
	parallel_pragma.c \
	private-duplicate.c \
	single2.c \
	sizeof.c \
	staticChunk.c \
	task_orphaned.c \
	task_underIf.c \
	task_wait.c \
	task_wait2.c \
	task_array2.c \
	task_untied4.c \
	twoRegions.c

#        axpy_ompacc2.c 
#	axpy_ompacc4.c # multiple gpu version
OMP_ACC_TESTCODES_REQUIRED_TO_PASS = \
        collapse_2.c\
        axpy_ompacc.c \
        axpy_ompacc3.c \
        matrixmultiply-ompacc.c \
        matrixmultiply-ompacc2.c \
        jacobi-ompacc.c \
        jacobi-ompacc-v2.c \
        jacobi-ompacc-opt1.c \
        jacobi-ompacc-opt2.c \
	jacobi-ompacc-multiGPU.c 

OMP_ACC_TESTCODES_CXX_REQUIRED_TO_PASS = 
#        jacobi-ompacc2.cpp # qualified name issue

# Put test codes which can run (with main() ) here
# They will be compiled and executed when the gomp library path is set
C_TESTCODES_REQUIRED_TO_RUN = \
	3loops.c \
	array_init.c \
	array_init_2.c \
	atomic.c \
	barrier.c \
	critical.c \
	critical_orphaned.c \
	empty.c \
	falsesharing.c \
	firstlastprivate.c \
	firstprivate.c \
	firstPrivateArray.c \
	firstPrivateArray_2.c \
	flush.c \
	get_max_threads.c \
	hello-1.c \
	hello-2.c \
	hello-ordered.c \
	hello.c \
	jacobi.c \
	jacobi-collapse.c \
	lastprivate.c \
	lastprivate0.c \
	lockarray.c \
	loop1.c \
	nestedpar.c \
	nestedpar1.c \
	masterSingle.c \
	matrixmultiply.c \
	multiple_return.c \
	ordered.c \
	ordered2.c \
	omp1.c \
	ompfor.c \
	ompfor_c99.c \
	ompfor2.c \
	ompfor3.c \
	ompfor4.c \
	ompfor-default.c \
	ompfor-decremental.c \
	ompfor-static.c \
	ompGetNumThreads.c \
	orphaned-directives.c \
	parallel.c \
	parallel-if.c \
	parallel-if-numthreads.c \
	parallel-numthreads.c \
	parallel-reduction.c \
	parallel-reduction2.c \
	parallelsections.c \
	parallelfor.c \
	parallelfor2.c \
	pi.c \
	prettyfunc.c \
	private.c \
	privatej.c \
	reduction.c \
	reduction2.c \
	reduction-classic.c \
	rice1.c \
	section.c \
	section1.c \
	set_num_threads.c \
	single.c \
	subteam.c \
	subteam2.c \
	spmd1.c \
	task_largenumber.c \
	task_outlining.c \
	task_untied.c \
	task_untied2.c \
	task_untied3.c \
	omp_version.c

# DQ (9/27/2009): Conditionally compile these files (fail in OSX due to lack of OSX
# support for thread local storage). Liao knows more about the details of these tests.
# threadprivate.c
# threadprivate2.c
# threadprivate3.c

if !OS_MACOSX
   C_TESTCODES_REQUIRED_TO_RUN += threadprivate.c threadprivate2.c threadprivate3.c
endif

# C++ test codes which can only be compiled
CXX_TESTCODES_REQUIRED_TO_COMPILE = \
	memberFunction.cpp \
	memberFunction2.cpp \
	referenceType.cpp

# C++ test codes which can be compiled and linked to final executables
CXX_TESTCODES_REQUIRED_TO_RUN =  \
	hello1.cpp \
	helloNested.cpp \
	objectPrivate.cpp \
	objectFirstPrivate.cpp \
        objectLastprivate.cpp \
	orphanedAtomic.cpp \
	preprocessingInfo2.cpp \
	task_link.cpp \
	task_link2.cpp \
	task_tree.cpp

ALL_TESTCODES = \
	$(C_TESTCODES_REQUIRED_TO_COMPILE) \
	$(C_TESTCODES_REQUIRED_TO_RUN) \
	$(CXX_TESTCODES_REQUIRED_TO_COMPILE) \
	$(CXX_TESTCODES_REQUIRED_TO_RUN) \
	$(OMP_ACC_TESTCODES_REQUIRED_TO_PASS) \
	$(OMP_ACC_TESTCODES_CXX_REQUIRED_TO_PASS) \
	$(TESTCODE_CURRENTLY_FAILING)

# The runnable tests' object files
C_TEST_OBJECT_REQUIRED_TO_RUN = ${C_TESTCODES_REQUIRED_TO_RUN:.c=.o}
CXX_TEST_OBJECT_REQUIRED_TO_RUN = ${CXX_TESTCODES_REQUIRED_TO_RUN:.cpp=.o}
#PASSING_Objects_With_main = \
#    $(C_TEST_OBJECT_REQUIRED_TO_RUN) \
#    $(CXX_TEST_OBJECT_REQUIRED_TO_RUN)


# TODO task-wait hangs for gcc 4.4.1 (not for Intel icc), skip the execution for now	
#	task_wait.o
# TODO: npb, validation suite, developer tests, epcc
# full_verify.c complex #if #endif
TESTCODE_CURRENTLY_FAILING = \
	copyin.c \
	copyprivate.c \
	copyprivate2.c \
	copyprivate3.c \
	critical_dead.c \
	endif2.c \
	expressions.c \
	macroCalls.c \
	recursive.c \
	shared.c \
	single_copyprivate.c \
	threadProcessor.c \
	variables.c

# we reuse the OpenMP sources files from the OpenMP compile test directory
TEST_DIR = $(top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenMP_tests
# the place to find omp.h, libgomp_g.h etc
TEST_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII -I$(top_srcdir)/src/midend/programTransformation/ompLowering -I$(top_builddir)

noinst_PROGRAMS = roseomp roseompacc
roseomp_SOURCES = roseomp.C
roseompacc_SOURCES = roseompacc.C # a relaxed version without consistency checking for fast prototyping

# this flag is used for the long list of CXX test files
TEST_FLAGS = $(TEST_INCLUDES) -rose:openmp:lowering -g --edg:no_warnings #-rose:verbose 2 #--edg:restrict #-rose:wave 
ACC_TEST_FLAGS = $(TEST_INCLUDES) -rose:openmp:lowering -g --edg:no_warnings #-rose:openmp:nodde #-rose:verbose 2 #--edg:restrict #-rose:wave 

# We only include test codes required to compile here.
# Runnable codes are handled separatedly, 
#    using check_PROGRAM  if gomp path is available
#   or make their object files only
PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_COMPILE:.c=.o)
PASSING_CXX_TEST_Objects = $(CXX_TESTCODES_REQUIRED_TO_COMPILE:.cpp=.o)

PASSING_OMP_ACC_TEST_CUDA_TEMP_Files = $(OMP_ACC_TESTCODES_REQUIRED_TO_PASS:.c=.cu)
PASSING_OMP_ACC_TEST_CXX_CUDA_TEMP_Files = $(OMP_ACC_TESTCODES_CXX_REQUIRED_TO_PASS:.cpp=.cu)

PASSING_OMP_ACC_TEST_CUDA_Files = $(addprefix rose_, ${PASSING_OMP_ACC_TEST_CUDA_TEMP_Files})
PASSING_OMP_ACC_TEST_CXX_CUDA_Files = $(addprefix rose_, ${PASSING_OMP_ACC_TEST_CXX_CUDA_TEMP_Files})

PASSING_OMP_ACC_TEST_EXE_Files = $(OMP_ACC_TESTCODES_REQUIRED_TO_PASS:.c=.out)
PASSING_OMP_ACC_TEST_CXX_EXE_Files = $(OMP_ACC_TESTCODES_CXX_REQUIRED_TO_PASS:.cpp=.out)

# rules to build .o from .c for OpenMP test codes
$(PASSING_C_TEST_Objects): %.o: $(TEST_DIR)/%.c roseomp
	@$(RTH_RUN) \
		TITLE="roseomp $(notdir $<) [$@.passed]" \
		CMD="./roseomp$(EXEEXT) ${TEST_FLAGS} -c $<" \
		$(TEST_EXIT_STATUS) $@.passed

#$(C_TESTCODES_REQUIRED_TO_RUN): %.o: $(TEST_DIR)/%.c roseomp
$(C_TEST_OBJECT_REQUIRED_TO_RUN): %.o: $(TEST_DIR)/%.c roseomp
	@$(RTH_RUN) \
		TITLE="roseomp $(notdir $<) [$@.passed]" \
		CMD="./roseomp$(EXEEXT) ${TEST_FLAGS} -c $<" \
		$(TEST_EXIT_STATUS) $@.passed

$(PASSING_CXX_TEST_Objects): %.o: $(TEST_DIR)/%.cpp roseomp
	@$(RTH_RUN) \
		TITLE="roseomp $(notdir $<) [$@.passed]" \
		CMD="./roseomp$(EXEEXT) ${TEST_FLAGS} -c $<" \
		$(TEST_EXIT_STATUS) $@.passed

#$(CXX_TESTCODES_REQUIRED_TO_RUN): %.o: $(TEST_DIR)/%.cpp roseomp
$(CXX_TEST_OBJECT_REQUIRED_TO_RUN): %.o: $(TEST_DIR)/%.cpp roseomp
	@$(RTH_RUN) \
		TITLE="roseomp $(notdir $<) [$@.passed]" \
		CMD="./roseomp$(EXEEXT) ${TEST_FLAGS} -c $<" \
		$(TEST_EXIT_STATUS) $@.passed

# Experimental translation for OpenMP accelerator model directives
# no final compilation for now, which requires CUDA compiler for the generated code
# each rose_*.cu file depends o its corresponding *.c file
$(PASSING_OMP_ACC_TEST_CUDA_Files): rose_%.cu: $(TEST_DIR)/%.c roseompacc
	@$(RTH_RUN) \
		TITLE="roseompacc $(notdir $<) [$@.passed]" \
		CMD="./roseompacc$(EXEEXT) ${ACC_TEST_FLAGS} -rose:skipfinalCompileStep -c $<" \
		$(TEST_EXIT_STATUS) $@.passed

$(PASSING_OMP_ACC_TEST_CXX_CUDA_Files): rose_%.cu: $(TEST_DIR)/%.cpp roseompacc
	@$(RTH_RUN) \
		TITLE="roseompacc $(notdir $<) [$@.passed]" \
		CMD="./roseompacc$(EXEEXT) ${ACC_TEST_FLAGS} -rose:skipfinalCompileStep -c $<" \
		$(TEST_EXIT_STATUS) $@.passed


#rose_axpy_ompacc.cu:roseompacc
#	./roseompacc$(EXEEXT) ${TEST_FLAGS} -rose:skipfinalCompileStep -c $(TEST_DIR)/axpy_ompacc.c 
#rose_matrixmultiply-ompacc.cu:roseompacc
#	./roseompacc$(EXEEXT) ${TEST_FLAGS} -rose:skipfinalCompileStep -c $(TEST_DIR)/matrixmultiply-ompacc.c 

#axpy_ompacc.out:rose_axpy_ompacc.cu
#	nvcc $<  $(TEST_INCLUDES) $(top_srcdir)/src/midend/programTransformation/ompLowering/xomp_cuda_lib.cu -o $@
#matrixmultiply-ompacc.out:rose_matrixmultiply-ompacc.cu
#	nvcc $< $(TEST_INCLUDES) $(top_srcdir)/src/midend/programTransformation/ompLowering/xomp_cuda_lib.cu -o $@

# NOTE: ROSE Test Harness has mechanisms for checking answers via diffs that could possibly be used here. It would also
# be able to handle via its filters the cases mentioned below where function names have random integer numbers. [RPM
# 2013-09-28]
OMP_ACC_DIFF_FILES=$(PASSING_OMP_ACC_TEST_CUDA_Files:.cu=.cu.diff)
DIFF=diff
EXPECTED_PATH = $(top_srcdir)/tests/nonsmoke/functional/roseTests/ompLoweringTests/referenceResults
$(OMP_ACC_DIFF_FILES):$(PASSING_OMP_ACC_TEST_CUDA_Files)
	echo "Verifying OMP ACC translation by diff ..."; \
	if $(DIFF) $(@:.cu.diff=.cu) $(EXPECTED_PATH)/$(@:.cu.diff=.cu) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; rm -rf $@; exit 1; fi 

# We cannot run the diff since the function names of an outlined function has a random integer numbers
ompacc_diff_check: $(OMP_ACC_DIFF_FILES)

if USE_CUDA

if WITH_GOMP_OPENMP_LIB
# we need both cuda and libgomp to run the CUDA code generated from OpenMP 4.0 code	
cuda_PROGRAM = $(PASSING_OMP_ACC_TEST_EXE_Files) $(PASSING_OMP_ACC_TEST_CXX_EXE_Files) 
#without libgomp, we only generate the CUDA file, without compile it
else
cuda_PROGRAM =
endif 
endif

if WITH_GOMP_OPENMP_LIB
#-------------compile and run using GCC's runtime library--------------------------------	
# Only for C or CXX tests will main()
#PASSING_TEST_Executables = ${PASSING_Objects_With_main:.o=.out}
PASSING_C_TEST_Executables = ${C_TEST_OBJECT_REQUIRED_TO_RUN:.o=.out}
PASSING_CXX_TEST_Executables = ${CXX_TEST_OBJECT_REQUIRED_TO_RUN:.o=.out}
# TODO this might have problem with CXX objects!!
MY_FINAL_LINK = -L$(top_builddir)/src/midend -lxomp $(GOMP_PATH)/libgomp.a -lpthread -lm

# build executables using nvcc
$(PASSING_OMP_ACC_TEST_EXE_Files): %.out: rose_%.cu
	@$(RTH_RUN) \
		TITLE="$(NVCCBIN)/nvcc $(notdir $<) [$@.passed]" \
		CMD="$(NVCCBIN)/nvcc $< $(TEST_INCLUDES) $(top_srcdir)/src/midend/programTransformation/ompLowering/xomp.c $(top_srcdir)/src/midend/programTransformation/ompLowering/xomp_cuda_lib.cu -o $@ $(GOMP_PATH)/libgomp.a -lpthread -lm" \
		$(TEST_EXIT_STATUS) $@.passed

$(PASSING_OMP_ACC_TEST_CXX_EXE_Files): %.out: rose_%.cu
	@$(RTH_RUN) \
		TITLE="$(NVCCBIN)/nvcc $(notdir $<) [$@.passed]" \
		CMD="$(NVCCBIN)/nvcc $< $(TEST_INCLUDES) $(top_srcdir)/src/midend/programTransformation/ompLowering/xomp.c $(top_srcdir)/src/midend/programTransformation/ompLowering/xomp_cuda_lib.cu -o $@ $(GOMP_PATH)/libgomp.a -lpthread -lm" \
		$(TEST_EXIT_STATUS) $@.passed


# a short cut to run omp-acc support test
#	./axpy_ompacc3.out TODO: real data distribution example
#           ./jacobi-ompacc2.out  // qualified name issue, 2018/8/17
ompacc_test:$(PASSING_OMP_ACC_TEST_EXE_Files) $(PASSING_OMP_ACC_TEST_CXX_EXE_Files)
ompacc_run: ompacc_test
	./axpy_ompacc.out && \
	   ./collapse_2.out  && \
           ./jacobi-ompacc-opt1.out  && \
           ./jacobi-ompacc-opt2.out  && \
           ./jacobi-ompacc.out  && \
           ./jacobi-ompacc-v2.out  && \
           ./matrixmultiply-ompacc.out

$(PASSING_C_TEST_Executables): %.out: %.o
	$(LIBTOOL) --mode=link $(CC) $< -o $@ $(MY_FINAL_LINK)
$(PASSING_CXX_TEST_Executables): %.out: %.o 
	$(LIBTOOL) --mode=link $(CXX) $< -o $@ $(MY_FINAL_LINK)
# check_PROGRAMS does not work!!	
check_PROGRAM = $(PASSING_C_TEST_Executables) $(PASSING_CXX_TEST_Executables)
# Executables depend on objects
# check-TESTS happens before check-local
TESTS =  $(check_PROGRAM) $(cuda_PROGRAM)
conditional-check-local: roseomp
	@echo "Test for ROSE OpenMP lowering."
	@echo "***************** Testing C input *******************"
	$(MAKE) $(PASSING_C_TEST_Objects)
	$(MAKE)	$(PASSING_OMP_ACC_TEST_CUDA_Files)
	$(MAKE)	$(PASSING_OMP_ACC_TEST_CXX_CUDA_Files)
if OS_MACOSX
#	DQ (9/27/2009): We need to generate this file temporaily because the documentation depends on it.
#	However, documentation should only depend upon generated files in the tutorial directory.
#	Please more the test to the tutorial directory if it is only for documentation purposes
#	or recreate it there for purposes of documentation and leave it here for purposes of testing
#	(I would like to not mix the two purposes together).
	@echo "/* threadprivate.c not run on MAC OSX (no thread local storage on OSX). */" > rose_threadprivate.c
endif
	@echo "***************** Testing C++ input ******************"
	$(MAKE) $(PASSING_CXX_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/roseTests/ompLoweringTests: make check rule complete (terminated normally)         ******"
	@echo "***********************************************************************************************************"

else

#Currently  GOMP takes precedence compared to Omni
# if both of them are specified.
# TODO: a better compiler/configuration option to  handle this
if ROSE_USE_OMNI_OPENMP

#-------------compile and run using Omni-------------------------------
# Only for C or CXX tests will main()
#PASSING_TEST_Executables = ${PASSING_Objects_With_main:.o=.out}
PASSING_C_TEST_Executables = ${C_TEST_OBJECT_REQUIRED_TO_RUN:.o=.out}
PASSING_CXX_TEST_Executables = ${CXX_TEST_OBJECT_REQUIRED_TO_RUN:.o=.out}
# TODO this might have problem with CXX objects!!
MY_FINAL_LINK = -L$(top_builddir)/src/midend -lxomp $(OMNI_PATH)/libompc.a -lpthread -lm
#MY_FINAL_LINK = $(top_builddir)/src/midend/programTransformation/ompLowering/libxomp.a $(OMNI_PATH)/libtlog.a  $(OMNI_PATH)/libompc.a -lpthread -lm

$(PASSING_C_TEST_Executables): %.out: %.o
	$(LIBTOOL) --mode=link $(CC) $< -o $@ $(MY_FINAL_LINK)
$(PASSING_CXX_TEST_Executables): %.out: %.o 
	$(LIBTOOL) --mode=link $(CXX) $< -o $@ $(MY_FINAL_LINK)
check_PROGRAM = $(PASSING_C_TEST_Executables) $(PASSING_CXX_TEST_Executables)
# Executables depend on objects
# check-TESTS happens before check-local
TESTS =  $(check_PROGRAM)  $(cuda_PROGRAM)

conditional-check-local: roseomp
	@echo "Test for ROSE OpenMP lowering."
	@echo "***************** Testing C input *******************"
	$(MAKE) $(PASSING_C_TEST_Objects)
	$(MAKE)	$(PASSING_OMP_ACC_TEST_CUDA_Files)
	$(MAKE)	$(PASSING_OMP_ACC_TEST_CXX_CUDA_Files)
if OS_MACOSX
#	DQ (9/27/2009): We need to generate this file temporaily because the documentation depends on it.
#	However, documentation should only depend upon generated files in the tutorial directory.
#	Please more the test to the tutorial directory if it is only for documentation purposes
#	or recreate it there for purposes of documentation and leave it here for purposes of testing
#	(I would like to not mix the two purposes together).
	@echo "/* threadprivate.c not run on MAC OSX (no thread local storage on OSX). */" > rose_threadprivate.c
endif
	@echo "***************** Testing C++ input ******************"
	$(MAKE) $(PASSING_CXX_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/roseTests/ompLoweringTests: make check rule complete (terminated normally)         ******"
	@echo "***********************************************************************************************************"

# USE gcc GOMP library
else

TESTS =  $(cuda_PROGRAM)
#-------------compile only --------------------------------	
conditional-check-local: roseomp \
		$(PASSING_C_TEST_Objects) $(C_TEST_OBJECT_REQUIRED_TO_RUN) $(PASSING_OMP_ACC_TEST_CUDA_Files) \
		$(PASSING_CXX_TEST_Objects) $(CXX_TEST_OBJECT_REQUIRED_TO_RUN) $(PASSING_OMP_ACC_TEST_CXX_CUDA_Files)
if OS_MACOSX
#	DQ (9/27/2009): We need to generate this file temporaily because the documentation depends on it.
#	However, documentation should only depend upon generated files in the tutorial directory.
#	Please more the test to the tutorial directory if it is only for documentation purposes
#	or recreate it there for purposes of documentation and leave it here for purposes of testing
#	(I would like to not mix the two purposes together).
	@echo "/* threadprivate.c not run on MAC OSX (no thread local storage on OSX). */" > rose_threadprivate.c
endif
#	$(MAKE) $(PASSING_Objects_With_main) # make objects with main entries here 
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/roseTests/ompLoweringTests:                                                        ******"
	@echo "******       compilation tests completed but executables are not generated and run                   ******"
	@echo "****** configure option --with-gomp_omp_runtime_library to enable testing execution of OpenMP        ******"
	@echo "****** for example: --with-gomp_omp_runtime_library=/home/liao6/opt/gcc-4.4.1/lib/                   ******"
	@echo "***********************************************************************************************************"

endif # ROSE_USE_OMNI_OPENMP
 
endif #

check-local: conditional-check-local

# Try not to delete files that a developer might have sitting in this directory--delete only things created by
# running the makefile.  I.e., try not to use wildcards!  Also, we could have combined these variables into a
# single list, but sometimes those lists tend to get too long, so we just do them one at a time.
clean-local:
	rm -f $(addprefix rose_, $(C_TESTCODES_REQUIRED_TO_COMPILE))
	rm -f $(addprefix rose_, $(CXX_TESTCODES_REQUIRED_TO_COMPILE))
	rm -f $(addprefix rose_, $(C_TESTCODES_REQUIRED_TO_RUN))
	rm -f $(addprefix rose_, $(CXX_TESTCODES_REQUIRED_TO_RUN))
	rm -f $(PASSING_C_TEST_Objects)
	rm -f $(addsuffix .passed, $(PASSING_C_TEST_Objects))
	rm -f $(addsuffix .failed, $(PASSING_C_TEST_Objects))
	rm -f $(C_TEST_OBJECT_REQUIRED_TO_RUN)
	rm -f $(addsuffix .passed, $(C_TEST_OBJECT_REQUIRED_TO_RUN))
	rm -f $(addsuffix .faild, $(C_TEST_OBJECT_REQUIRED_TO_RUN))
	rm -f $(PASSING_CXX_TEST_Objects)
	rm -f $(addsuffix .passed, $(PASSING_CXX_TEST_Objects))
	rm -f $(addsuffix .failed, $(PASSING_CXX_TEST_Objects))
	rm -f $(CXX_TEST_OBJECT_REQUIRED_TO_RUN)
	rm -f $(addsuffix .passed, $(CXX_TEST_OBJECT_REQUIRED_TO_RUN))
	rm -f $(addsuffix .failed, $(CXX_TEST_OBJECT_REQUIRED_TO_RUN))
	rm -f $(PASSING_OMP_ACC_TEST_CUDA_Files)
	rm -f $(addsuffix .passed, $(PASSING_OMP_ACC_TEST_CUDA_Files))
	rm -f $(addsuffix .failed, $(PASSING_OMP_ACC_TEST_CUDA_Files))
	rm -f $(PASSING_OMP_ACC_TEST_CXX_CUDA_Files)
	rm -f $(addsuffix .passed, $(PASSING_OMP_ACC_TEST_CXX_CUDA_Files))
	rm -f $(addsuffix .failed, $(PASSING_OMP_ACC_TEST_CXX_CUDA_Files))
	rm -f $(PASSING_OMP_ACC_TEST_EXE_Files)
	rm -f $(addsuffix .passed, $(PASSING_OMP_ACC_TEST_EXE_Files))
	rm -f $(addsuffix .failed, $(PASSING_OMP_ACC_TEST_EXE_Files))
	rm -f $(PASSING_OMP_ACC_TEST_CXX_EXE_Files)
	rm -f $(addsuffix .passed, $(PASSING_OMP_ACC_TEST_CXX_EXE_Files))
	rm -f $(addsuffix .failed, $(PASSING_OMP_ACC_TEST_CXX_EXE_Files))
	rm -f *.out *.dot


EXTRA_DIST = referenceResults

CLEANFILES = 



