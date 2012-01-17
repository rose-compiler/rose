include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

CXX_TESTCODES_REQUIRED_TO_PASS = \
				multiArray.C \
				multiLoop_nopragma.C \
				singleLoop_defaultChunksize.C \
				singleLoop_pragma.C \
				multiDim.C \
				multiLoop_pragma.C \
				singleLoop_nopragma.C \
				singleLoop_pragmaFilter.C

TESTCODE_CURRENTLY_FAILING =

ALL_TESTCODES = \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

EXEC=$(top_builddir)/projects/DataFaultTolerance/src/DataFaultToleranceTransformation
TEST_PATH = $(top_srcdir)/projects/DataFaultTolerance/test/array/transformation/tests
EXPECTED_PATH = $(top_srcdir)/projects/DataFaultTolerance/test/array/transformation/expected
ROSE_COMPILE_FLAGS= -rose:skipfinalCompileStep
ROSE_OUTPUT_FLAGS= -rose:output
DIFF=diff

CXX_TEST_Objects = ${CXX_TESTCODES_REQUIRED_TO_PASS:.C=.exe}

$(CXX_TEST_Objects): $(CXX_TESTCODES_REQUIRED_TO_PASS)
		     $(EXEC) $(ROSE_COMPILE_FLAGS) -c $(TEST_PATH)/$(@:.exe=.C) $(ROSE_OUTPUT_FLAGS) $(@:.exe=.out)

# diff the output of 2 executables      
DIFF_CHECK = $(CXX_TESTCODES_REQUIRED_TO_PASS:.C=.C.diff)

$(DIFF_CHECK): $(CXX_TESTCODES_REQUIRED_TO_PASS)
		echo "In Diff Check $@ ..." ; \
		if $(DIFF) $(@:.C.diff=.out) $(EXPECTED_PATH)/$(@:.C.diff=.C) ; then echo "Test Passed" ; else echo "Files differ; test falied"; exit 1; fi 

check-local:
	@echo "Test for DataFaultTolerance transformation"
	@rm -rf *.out
	@$(MAKE) $(CXX_TEST_Objects)
	@echo " Diff Check "
	@$(MAKE) $(DIFF_CHECK)
	@echo "*********************************************************************************************************************************"
	@echo "****** ROSE/projects/DataFaultTolerance/array/transformation/tests: make check rule complete (terminated normally) **************"
	@echo "*********************************************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES)

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cc *.patch *.gif *.log

	


