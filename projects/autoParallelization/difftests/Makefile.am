include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODE_CURRENTLY_FAILING = 

C_TESTCODES = 

# name qualification of constructor is wrongly unparsed for 4.2 and 4.4 testing
if ROSE_USING_GCC_VERSION_LATER_4_8
CXX_TESTCODES = \
     missingReduction.C \
     omp_lib_call.C
endif

ALL_TESTCODES = \
$(C_TESTCODES) \
$(CXX_TESTCODES) \
$(TESTCODE_CURRENTLY_FAILING)

# used to find omp.h, added it as one of  rose headers
TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII

# Turned of the debug flag to have a cleaner screen output, Liao, 9/16/2009
#-rose:autopar:enable_distance
#-rose:autopar:enable_debug 
# -rose:autopar:enable_patch
# -rose:autopar:unique_indirect_index
# -rose:autopar:no_aliasing
# --edg:restrict
# --edg:no_warnings
# -rose:verbose 0
#  We focus on testing the check for existing directives
ROSE_COMMON_FLAGS = -rose:autopar:enable_diff 
#-fopenmp, automatically add this option 

ROSE_CFLAGS = $(ROSE_COMMON_FLAGS) -rose:C99 
# for C++ input codes
ROSE_CXXFLAGS = $(ROSE_COMMON_FLAGS)

# build the translator
../autoPar:
	$(MAKE) -C ../. autoPar

# we want to use diff to compare generated files with reference files
# TODO: diff patches is a better choice.  Tolerate other differences introduced by ROSE.

CXX_TEST_OUTPUT = $(addsuffix .output, $(CXX_TESTCODES))

DIFF=diff
REFERENCE_PATH=$(top_srcdir)/projects/autoParallelization/difftests

C_TEST_DIFF_FILES=$(C_TESTCODES:.c=.c.diff)
CXX_TEST_DIFF_FILES=$(CXX_TESTCODES:.C=.C.diff)

# we remove -c to test linking , all test input files must have the main() entry now.
$(C_TEST_OUTPUT):../autoPar $(srcdir)/$(@:%.output=%)
	../autoPar $(ROSE_CFLAGS) $(TESTCODE_INCLUDES) $(srcdir)/$(@:%.output=%) > $@

$(CXX_TEST_OUTPUT):../autoPar  $(srcdir)/$(@:%.output=%)
	../autoPar $(ROSE_CXXFLAGS) $(TESTCODE_INCLUDES) $(srcdir)/$(@:%.output=%) >$@

# diff files
$(CXX_TEST_DIFF_FILES): $(CXX_TEST_OUTPUT) 
	echo "Verifying autoPar translation by using diff ..."; \
	if $(DIFF) $(@:.C.diff=.C.output) $(REFERENCE_PATH)/$(@:.C.diff=.C.output) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi

check-local:
	@echo "Test for ROSE automatic parallelization."
	@$(MAKE) $(C_TEST_DIFF_FILES) $(CXX_TEST_DIFF_FILES)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/autoParallelization/difftests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES) 

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cc *.patch *.diff




