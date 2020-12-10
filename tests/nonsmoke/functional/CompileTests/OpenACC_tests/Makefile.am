include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS=
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
  SUBDIRS += . 
endif

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  SUBDIRS += fortran 
endif

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

TESTCODE_CURRENTLY_FAILING = 
C_TESTCODES_REQUIRED_TO_PASS = 

if USING_CLANG_COMPILER
C_OMP_ACC_TESTCODES_REQUIRED_TO_PASS = 
CXX_TESTCODES_REQUIRED_TO_PASS = 
else
C_OMP_ACC_TESTCODES_REQUIRED_TO_PASS = 
CXX_TESTCODES_REQUIRED_TO_PASS = 
endif

if !USING_INTEL_COMPILER
C_TESTCODES_REQUIRED_TO_PASS += 
endif

# we only care about unparsing for ACC input, not the object file generation
C_OMP_ACC_TEST_Unparsed = $(addprefix rose_,${C_OMP_ACC_TESTCODES_REQUIRED_TO_PASS})

ROSE_FLAGS = -rose:openmp:ast_only --edg:no_warnings -w -rose:verbose 0 --edg:restrict #-rose:wave
# used to find omp.h, added it as one of  rose headers
TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII

PASSING_C_TEST_RESULTS = $(addprefix rose_,${C_TESTCODES_REQUIRED_TO_PASS})
PASSING_CXX_TEST_RESULTS = $(addprefix rose_,${CXX_TESTCODES_REQUIRED_TO_PASS})

# rose_input.c/cpp 
PASSING_C_CXX_TEST_RESULTS = $(PASSING_C_TEST_RESULTS) $(PASSING_CXX_TEST_RESULTS) 

PASSING_C_CXX_ACC_TEST_RESULTS=$(PASSING_C_CXX_TEST_RESULTS) $(C_OMP_ACC_TEST_Unparsed) 

PASSING_C_CXX_ACC_TEST_OUTPUT=$(addsuffix .output, $(PASSING_C_CXX_ACC_TEST_RESULTS))

#$(PASSING_C_CXX_TEST_RESULTS): $(srcdir)/$(@:rose_%=%) ../../testTranslator
#	../../testTranslator $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:rose_%=%) 

$(PASSING_C_CXX_ACC_TEST_OUTPUT): %.output:% 
	grep "pragma omp" $(@:%.output=%) > $@ || exit 0;

PASSING_C_CXX_ACC_TEST_OUTPUT_DIFF=$(addsuffix .diff, $(PASSING_C_CXX_ACC_TEST_OUTPUT))

REFERENCE_PATH=$(top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenACC_tests/referenceResults

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

if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
conditional-check-local:
	@echo "**********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/OpenACC_tests: make check rule complete (terminated normally) ******"
	@echo "**********************************************************************************************************************"
else
conditional-check-local:
	@echo "Test for ROSE OpenACC Parser."
	@echo "*******************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/OpenACC_tests: make check rule for C/C++ skipped (terminated normally) ******"
	@echo "*******************************************************************************************************************************"
endif

EXTRA_DIST = $(ALL_TESTCODES) 
check-local: conditional-check-local

clean-local:
	rm -f *.o rose_*.* *.out
	rm -rf QMTest
