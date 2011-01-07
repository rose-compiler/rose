include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

F77_TESTCODES_TO_PASS = \
    helloworld.f \
    helloworld0.f \
    helloworld2.f \
    jacobi.f \
    default.f \
    flush.f \
    exampleA251f.f \
    ompdo-default.f \
    ompdo-decremental.f \
    ompdo-static.f \
    parallel.f \
    parallel-1.f \
    parallel-2.f \
    private.f \
    schedule.f \
    shared.f

# DQ (3/4/2010): Added simple test code to debug fortran OFP usage.
F77_TESTCODES_TO_PASS += testNewOFP.f

F90_TESTCODES_TO_PASS = \
  paralleldo.f90 \
  paralleldo-1.f90 \
  collapse.f90 \
  critical.f90 \
  single-copyprivate.f90 \
  continuation.f90

#if !OS_VENDOR_DEBIAN
## This fails for Debian 5.0 systems (at least 32-bit, all compilers (4.3 tested), I think)
#if !ROSE_USING_GFORTRAN_VERSION_4_0
## These are tests that fail using the gfrotran version 4.0 compiler (but pass with all other versions of gfortran).
#     F90_TESTCODES_TO_PASS += exampleA134f.f90
#endif
#endif

# case insensitive in symobl lookup for Fortran
# exampleA281f.f: case insensitive for included header file's name, also bug 354
# flush2.f : wrong position for flush (scope becomes function, not inner block)
# conditional.f:  conditional compilation
# exampleA41f.f90:  test code has error?
# barrier.f, exmpaleA221f.f  wrong comment position for the 2nd barrier
# exampleA11f.f  combined parallel do is attached to wrong place, and is not removed!
# ompdo-decremental-2.f  wrong location for !$omp parallel do 
TESTCODES_CURRENTLY_FAILING = \
    barrier.f \
    exampleA11f.f \
    exampleA211f.f \
    exampleA281f.f \
    exampleA102f.f90 \
    exampleA161f.f90 \
    flush2.f \
    exmpaleA221f.f \
    conditional.f \
    exampleA41f.f90 \
    exampleA134f.f90 \
    ompdo-decremental-2.f 

#ROSE_FLAGS = -rose:openmp
ROSE_FLAGS = -rose:openmp:ast_only

TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII

PASSING_F77_TEST_OBJECTS = $(F77_TESTCODES_TO_PASS:.f=.o)
PASSING_F90_TEST_OBJECTS = $(F90_TESTCODES_TO_PASS:.f90=.o)

$(PASSING_F77_TEST_OBJECTS): %.o:$(srcdir)/%.f ../parseOmp
	../parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $<

$(PASSING_F90_TEST_OBJECTS): %.o:$(srcdir)/%.f90 ../parseOmp
	../parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $<

../parseOmp:
	$(MAKE) -C ../. parseOmp
ALL_TESTCODES =\
  $(F77_TESTCODES_TO_PASS) \
  $(F90_TESTCODES_TO_PASS) \
  $(TESTCODES_CURRENTLY_FAILING) 

# follow the astOutlining test to guide fortran tests  
# Liao, 5/27/2009
# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

if ROSE_USE_SSL_SUPPORT
	@echo "Fortran tests for ROSE OpenMP parser"
	@echo "*******************************************************************************************************************"
	@echo "****** ROSE/tests/CompilTests/OpenMP_tests/fortran: make check rule skipped (SSL support disables this rule) ******" 
	@echo "*******************************************************************************************************************"
else	
check-local:
	@echo "Fortran tests for ROSE OpenMP parser"
	@$(MAKE) $(PASSING_F77_TEST_OBJECTS)
	@$(MAKE) $(PASSING_F90_TEST_OBJECTS)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/CompilTests/OpenMP_tests/fortran: make check rule completes (terminated normally) *******" 
	@echo "***********************************************************************************************************"
endif	
else
check-local:
	@echo "Fortran tests for ROSE OpenMP parser"
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/CompilTests/OpenMP_tests/fortran: make check rule skipped (FORTRAN NOT CONFIGURED) ******" 
	@echo "***********************************************************************************************************"

endif	

EXTRA_DIST = $(ALL_TESTCODES)
clean-local:
	rm -f *.o rose_*.* *.out *.dot
	rm -rf QMTest
