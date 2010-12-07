include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Default header file search path
INCLUDES = $(ROSE_INCLUDES)

GOMP_PATH = @gomp_omp_runtime_library_path@
OMNI_PATH = @omni_omp_runtime_support_path@

F77_TESTCODES_REQUIRED_TO_COMPILE = 

F90_TESTCODES_REQUIRED_TO_COMPILE = 

F77_TESTCODES_REQUIRED_TO_RUN = \
  helloworld.f
F90_TESTCODES_REQUIRED_TO_RUN = 

TESTCODE_CURRENTLY_FAILING = 

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
TEST_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII -I$(top_srcdir)/src/midend/programTransformation/ompLowering

../roseomp:
	make -C ../. roseomp
# this flag is used for the long list of CXX test files
TEST_FLAGS = $(TEST_INCLUDES) -rose:openmp:lowering --edg:no_warnings #-fno-underscoring #-rose:verbose 2 #--edg:restrict #-rose:wave 


# rules to build .o from .f for OpenMP test codes
$(PASSING_F77_TEST_Objects): %.o: $(TEST_DIR)/%.f ../roseomp
	../roseomp$(EXEEXT) ${TEST_FLAGS} -c $<

$(F77_TEST_OBJECT_REQUIRED_TO_RUN): %.o: $(TEST_DIR)/%.f ../roseomp
	../roseomp$(EXEEXT) ${TEST_FLAGS} -c $<

$(PASSING_F90_TEST_Objects): %.o: $(TEST_DIR)/%.f90 ../roseomp
	../roseomp$(EXEEXT) ${TEST_FLAGS} -c $<

$(F90_TEST_OBJECT_REQUIRED_TO_RUN): %.o: $(TEST_DIR)/%.f90 ../roseomp
	../roseomp$(EXEEXT) ${TEST_FLAGS} -c $<


if WITH_GOMP_OPENMP_LIB
#-------------compile and run using GCC's runtime library--------------------------------       
PASSING_F77_TEST_Executables = ${F77_TEST_OBJECT_REQUIRED_TO_RUN:.o=.out}
PASSING_F90_TEST_Executables = ${F90_TEST_OBJECT_REQUIRED_TO_RUN:.o=.out}
MY_FINAL_LINK = $(top_builddir)/src/midend/programTransformation/ompLowering/libxomp.a $(GOMP_PATH)/libgomp.a -lpthread -lm

F_LINK_FLAGS= -fno-underscoring
# $(F77) will be expanded to g77, which has different libfortran expectation
# $(F90) is not defined?
$(PASSING_F77_TEST_Executables): %.out: %.o
	$(GFORTRAN_PATH) $(F_LINK_FLAGS) $< -o $@ $(MY_FINAL_LINK)
$(PASSING_F90_TEST_Executables): %.out: %.o
	$(GFORTRAN_PATH) $(F_LINK_FLAGS) $< -o $@ $(MY_FINAL_LINK)
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

clean-local:
	rm -f *.o test*.C rose_*.c rose_*.cpp rose_*.C *.pdf OUT_* *.dot *.out  rose_*.f rose_*.f90 rose_*.f95

EXTRA_DIST =

CLEANFILES =

