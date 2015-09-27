include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
MOSTLYCLEANFILES =

# Default header file search path
AM_CPPFLAGS = $(ROSE_INCLUDES)

GOMP_PATH = @gomp_omp_runtime_library_path@
OMNI_PATH = @omni_omp_runtime_support_path@

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

F77_TESTCODES_REQUIRED_TO_COMPILE = \
  array-one.f \
  shared-array.f

F90_TESTCODES_REQUIRED_TO_COMPILE = 

F77_TESTCODES_REQUIRED_TO_RUN = \
  array_initialize.f \
  helloworld.f \
  jacobi.f \
  master.f \
  ompdo-decremental.f \
  ompdo-default.f \
  ompdo-orphaned.f \
  ompdo-static.f \
  ompdo-multiple.f \
  task_largenumber.f \
  task_scalar.f \
  task_simplest.f \
  private.f \
  reduction.f \
  shared.f \
  single.f 

F90_TESTCODES_REQUIRED_TO_RUN = 

TESTCODE_CURRENTLY_FAILING = \
   parallel-if.f \
   parallel-if-numthreads.f \
   section1.f

ALL_TESTCODES = \
  $(F77_TESTCODES_REQUIRED_TO_COMPILE) \
  $(F77_TESTCODES_REQUIRED_TO_RUN) \
  $(F90_TESTCODES_REQUIRED_TO_COMPILE) \
  $(F90_TESTCODES_REQUIRED_TO_RUN) \
  $(TESTCODE_CURRENTLY_FAILING)

# test codes required to compile here.
#    using check_PROGRAM  if gomp path is available
#   or make their object files only
PASSING_F77_TEST_Objects = $(F77_TESTCODES_REQUIRED_TO_COMPILE:.f=.o)
PASSING_F90_TEST_Objects = $(F90_TESTCODES_REQUIRED_TO_COMPILE:.f90=.o)

#runnable tests' object files
F77_TEST_OBJECT_REQUIRED_TO_RUN = ${F77_TESTCODES_REQUIRED_TO_RUN:.f=.o}
F90_TEST_OBJECT_REQUIRED_TO_RUN = ${F90_TESTCODES_REQUIRED_TO_RUN:.f90=.o}


# we reuse the OpenMP sources files from the OpenMP compile test directory
TEST_DIR = $(top_srcdir)/tests/CompileTests/OpenMP_tests/fortran

# the place to find omp.h, libgomp_g.h etc
TEST_AM_CPPFLAGS = -I$(top_srcdir)/src/frontend/SageIII -I$(top_srcdir)/src/midend/programTransformation/ompLowering

../roseomp:
	make -C ../. roseomp
# this flag is used for the long list of CXX test files
TEST_FLAGS = $(TEST_AM_CPPFLAGS) -rose:openmp:lowering -g --edg:no_warnings #-fno-underscoring #-rose:verbose 2 #--edg:restrict #-rose:wave 


# rules to build .o from .f for OpenMP test codes
$(PASSING_F77_TEST_Objects): %.o: $(TEST_DIR)/%.f ../roseomp
	@$(RTH_RUN) \
		TITLE="roseomp $(notdir $<) [$@.passed]" \
		CMD="../roseomp$(EXEEXT) ${TEST_FLAGS} -c $<" \
		$(TEST_EXIT_STATUS) $@.passed

$(F77_TEST_OBJECT_REQUIRED_TO_RUN): %.o: $(TEST_DIR)/%.f ../roseomp
	@$(RTH_RUN) \
		TITLE="roseomp $(notdir $<) [$@.passed]" \
		CMD="../roseomp$(EXEEXT) ${TEST_FLAGS} -c $<" \
		$(TEST_EXIT_STATUS) $@.passed

$(PASSING_F90_TEST_Objects): %.o: $(TEST_DIR)/%.f90 ../roseomp
	@$(RTH_RUN) \
		TITLE="roseomp $(notdir $<) [$@.passed]" \
		CMD="../roseomp$(EXEEXT) ${TEST_FLAGS} -c $<" \
		$(TEST_EXIT_STATUS) $@.passed

$(F90_TEST_OBJECT_REQUIRED_TO_RUN): %.o: $(TEST_DIR)/%.f90 ../roseomp
	@$(RTH_RUN) \
		TITLE="roseomp $(notdir $<) [$@.passed]" \
		CMD="../roseomp$(EXEEXT) ${TEST_FLAGS} -c $<" \
		$(TEST_EXIT_STATUS) $@.passed


if WITH_GOMP_OPENMP_LIB
#-------------compile and run using GCC's runtime library--------------------------------       
PASSING_F77_TEST_Executables = ${F77_TEST_OBJECT_REQUIRED_TO_RUN:.o=.out}
PASSING_F90_TEST_Executables = ${F90_TEST_OBJECT_REQUIRED_TO_RUN:.o=.out}
MY_FINAL_LINK = -L$(top_builddir)/src/midend -lxomp $(GOMP_PATH)/libgomp.a -lpthread -lm -lrt

F_LINK_FLAGS= #-fno-underscoring
# $(F77) will be expanded to g77, which has different libfortran expectation
# $(F90) is not defined?
$(PASSING_F77_TEST_Executables): %.out: %.o
	$(LIBTOOL) --mode=link --tag=FC $(GFORTRAN_PATH) $(F_LINK_FLAGS) $< -o $@ $(MY_FINAL_LINK)
$(PASSING_F90_TEST_Executables): %.out: %.o
	$(LIBTOOL) --mode=link --tag=FC $(GFORTRAN_PATH) $(F_LINK_FLAGS) $< -o $@ $(MY_FINAL_LINK)
check_PROGRAM = $(PASSING_F77_TEST_Executables) $(PASSING_F90_TEST_Executables)
# Executables depend on objects
# check-TESTS happens before check-local
TESTS =  $(check_PROGRAM)
check-local: ../roseomp
	@echo "Test for ROSE OpenMP lowering."
	@echo "***************** Testing Fortran 77 input *******************"
	$(MAKE) $(PASSING_F77_TEST_Objects)
#if OS_MACOSX
#        @echo "/* threadprivate.c not run on MAC OSX (no thread local storage on OSX). */" > rose_threadprivate.c
#endif
	@echo "***************** Testing Fortran 90 input ******************"
	$(MAKE) $(PASSING_F90_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/ompLoweringTests/fortran: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

else
if ROSE_USE_OMNI_OPENMP
check-local: ../roseomp
	@echo "Test for ROSE OpenMP lowering."
	@echo "Link with Omni OpenMP runtime is not tested/supported yet."
else
#-------------compile only --------------------------------     
check-local: ../roseomp
	@echo "Test for ROSE OpenMP lowering."
	@echo "***************** Testing Fortran 77 input *******************"
	$(MAKE) $(PASSING_F77_TEST_Objects)
#if OS_MACOSX
#        @echo "/* threadprivate.c not run on MAC OSX (no thread local storage on OSX). */" > rose_threadprivate.c
#endif
	$(MAKE) $(F77_TEST_OBJECT_REQUIRED_TO_RUN)
	@echo "***************** Testing Fortran 90 input ******************"
	$(MAKE) $(PASSING_F90_TEST_Objects)
	$(MAKE) $(F90_TEST_OBJECT_REQUIRED_TO_RUN)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/ompLoweringTests/fortran:                       *******************************"
	@echo "****       compilation tests completed but executables are not generated and run***************************"
	@echo "****** configure option --with-gomp_omp_runtime_library to enable testing execution of OpenMP   ******"
	@echo "****** for example: --with-gomp_omp_runtime_library=/home/liao6/opt/gcc-4.4.1/lib/              ********"
	@echo "***********************************************************************************************************"
endif # ROSE_USE_OMNI_OPENMP
endif  

OBJECTS_WITH_TESTS =				\
	$(PASSING_F77_TEST_Objects)		\
	$(F77_TEST_OBJECT_REQUIRED_TO_RUN)	\
	$(PASSING_F90_TEST_Objects)		\
	$(F90_TEST_OBJECT_REQUIRED_TO_RUN)

clean-local:
	rm -f $(OBJECTS_WITH_TESTS)
	rm -f $(addsuffix .passed, $(OBJECTS_WITH_TESTS))
	rm -f $(addsuffix .failed, $(OBJECTS_WITH_TESTS))
	rm -f *.o test*.C rose_*.c rose_*.cpp rose_*.C *.pdf OUT_* *.dot *.out  rose_*.f rose_*.f90 rose_*.f95

EXTRA_DIST = 

CLEANFILES =

