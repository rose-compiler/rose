include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

F77_TESTCODES_TO_PASS = 
F77_TESTCODES_TO_PASS += 

F90_TESTCODES_TO_PASS = \
	trans.F90
TESTCODES_CURRENTLY_FAILING = 

# Uncomment to use Address Sanitizer.
ADDRESS_SANITIZER = $(NON_RTH_ADDRESS_SANITIZER_OPTIONS)

ROSE_FLAGS = -rose:openacc
#ROSE_FLAGS = -rose:openacc:ast_only

TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII


PASSING_F77_TEST_RESULTS = $(addprefix rose_,${F77_TESTCODES_TO_PASS})
PASSING_F90_TEST_RESULTS = $(addprefix rose_,${F90_TESTCODES_TO_PASS})

$(PASSING_F77_TEST_RESULTS): $(srcdir)/$(@:rose_%=%) ../../../testTranslator
	$(ADDRESS_SANITIZER) ../../../testTranslator $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:rose_%=%)

$(PASSING_F90_TEST_RESULTS): $(srcdir)/$(@:rose_%=%) ../../../testTranslator
	$(ADDRESS_SANITIZER) ../../../testTranslator $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:rose_%=%)


PASSING_F77_F90_TEST_RESULTS=$(PASSING_F77_TEST_RESULTS) $(PASSING_F90_TEST_RESULTS)

PASSING_F77_F90_TEST_RESULTS_OUTPUT=$(addsuffix .output, $(PASSING_F77_F90_TEST_RESULTS))

$(PASSING_F77_F90_TEST_RESULTS_OUTPUT): %.output:%
	grep "\!\$$acc" $(@:%.output=%) > $@ || exit 0;

PASSING_F77_F90_TEST_RESULTS_OUTPUT_DIFF=$(addsuffix .diff, $(PASSING_F77_F90_TEST_RESULTS_OUTPUT))

REFERENCE_PATH=$(top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenACC_tests/fortran/referenceResults

$(PASSING_F77_F90_TEST_RESULTS_OUTPUT_DIFF): %.diff:%
	if diff $(@:.output.diff=.output) $(REFERENCE_PATH)/$(@:.output.diff=.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; rm -rf $@; exit 1; fi


gen-output: $(PASSING_F77_F90_TEST_RESULTS_OUTPUT)

../../../testTranslator:
	$(MAKE) -C ../../../. testTranslator

ALL_TESTCODES =\
  $(F77_TESTCODES_TO_PASS) \
  $(F90_TESTCODES_TO_PASS) \
  $(TESTCODES_CURRENTLY_FAILING) 

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

if ROSE_USE_SSL_SUPPORT
	@echo "Fortran tests for ROSE OpenMP parser"
	@echo "*******************************************************************************************************************"
	@echo "****** ROSE/tests/CompilTests/OpenACC_tests/fortran: make check rule skipped (SSL support disables this rule) ******" 
	@echo "*******************************************************************************************************************"
else	
conditional-check-local:
	@echo "Fortran tests for ROSE OpenMP parser"
	@$(MAKE) $(PASSING_F77_F90_TEST_RESULTS)
	@$(MAKE) $(PASSING_F77_F90_TEST_RESULTS_OUTPUT_DIFF)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/CompilTests/OpenACC_tests/fortran: make check rule completes (terminated normally) *******" 
	@echo "***********************************************************************************************************"
endif	
else
conditional-check-local:
	@echo "Fortran tests for ROSE OpenACC parser"
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/CompilTests/OpenACC_tests/fortran: make check rule skipped (FORTRAN NOT CONFIGURED) ******" 
	@echo "***********************************************************************************************************"

endif	

EXTRA_DIST = $(ALL_TESTCODES)

check-local: conditional-check-local

clean-local:
	rm -f *.o rose_*.* *.out *.dot *.mod *.rmod *_postprocessed.*
	rm -rf QMTest
