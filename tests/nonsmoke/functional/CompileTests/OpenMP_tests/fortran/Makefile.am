include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

F77_TESTCODES_TO_PASS = \
    array-one.f \
    array_initialize.f \
    default.f \
    exampleA251f.f \
    flush.f \
    helloworld.f \
    helloworld0.f \
    helloworld2.f \
    jacobi.f \
    master.f \
    ompdo-decremental.f \
    ompdo-default.f \
    ompdo-multiple.f \
    ompdo-orphaned.f \
    ompdo-static.f \
    parallel-1.f \
    parallel-2.f \
    parallel.f \
    private.f \
    reduction.f \
    schedule.f \
    section1.f \
    shared-array.f \
    shared.f \
    single.f \
    task_largenumber.f \
    task_largenumber2.f \
    task_scalar.f \
    task_simplest.f

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
# task_largenumber2.f : bug 495 https://outreach.scidac.gov/tracker/?func=detail&atid=185&aid=495&group_id=24
TESTCODES_CURRENTLY_FAILING = \
    parallel-if.f \
    parallel-if-numthreads.f \
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

# DQ (11/4/2016): Uncomment to use Address Sanitizer.
ADDRESS_SANITIZER = $(NON_RTH_ADDRESS_SANITIZER_OPTIONS)

#ROSE_FLAGS = -rose:openmp
ROSE_FLAGS = -rose:openmp:ast_only

TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII


PASSING_F77_TEST_RESULTS = $(addprefix rose_,${F77_TESTCODES_TO_PASS})
PASSING_F90_TEST_RESULTS = $(addprefix rose_,${F90_TESTCODES_TO_PASS})

$(PASSING_F77_TEST_RESULTS): $(srcdir)/$(@:rose_%=%) ../parseOmp
	$(ADDRESS_SANITIZER) ../parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:rose_%=%)

$(PASSING_F90_TEST_RESULTS): $(srcdir)/$(@:rose_%=%) ../parseOmp
	$(ADDRESS_SANITIZER) ../parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:rose_%=%)


PASSING_F77_F90_TEST_RESULTS=$(PASSING_F77_TEST_RESULTS) $(PASSING_F90_TEST_RESULTS)

PASSING_F77_F90_TEST_RESULTS_OUTPUT=$(addsuffix .output, $(PASSING_F77_F90_TEST_RESULTS))

$(PASSING_F77_F90_TEST_RESULTS_OUTPUT): %.output:%
	grep "\!\$$omp" $(@:%.output=%) > $@ || exit 0;

PASSING_F77_F90_TEST_RESULTS_OUTPUT_DIFF=$(addsuffix .diff, $(PASSING_F77_F90_TEST_RESULTS_OUTPUT))

REFERENCE_PATH=$(top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenMP_tests/fortran/referenceResults

$(PASSING_F77_F90_TEST_RESULTS_OUTPUT_DIFF): %.diff:%
	if diff $(@:.output.diff=.output) $(REFERENCE_PATH)/$(@:.output.diff=.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; rm -rf $@; exit 1; fi


gen-output: $(PASSING_F77_F90_TEST_RESULTS_OUTPUT)

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
conditional-check-local:
	@echo "Fortran tests for ROSE OpenMP parser"
	@$(MAKE) $(PASSING_F77_F90_TEST_RESULTS)
	@$(MAKE) $(PASSING_F77_F90_TEST_RESULTS_OUTPUT_DIFF)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/CompilTests/OpenMP_tests/fortran: make check rule completes (terminated normally) *******" 
	@echo "***********************************************************************************************************"
endif	
else
conditional-check-local:
	@echo "Fortran tests for ROSE OpenMP parser"
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/CompilTests/OpenMP_tests/fortran: make check rule skipped (FORTRAN NOT CONFIGURED) ******" 
	@echo "***********************************************************************************************************"

endif	

EXTRA_DIST = $(ALL_TESTCODES)

check-local: conditional-check-local

clean-local:
	rm -f *.o rose_*.* *.out *.dot
	rm -rf QMTest
