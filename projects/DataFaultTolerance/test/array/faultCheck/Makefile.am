include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

Other_SOURCES = rose_ErrtestSample.C rose_testCode.C rose_testSample.C rose_test_10Elem.C rose_test_1Elem.C

CXX_TESTCODES_REQUIRED_TO_PASS = \
rose_testSample \
rose_ErrtestSample

TESTCODE_CURRENTLY_FAILING = 

ALL_TESTCODES = \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# used to find .h, added it as one of  rose headers
TESTCODE_INCLUDES = -I$(top_srcdir)/projects/DataFaultTolerance/includes 
LIB_DIR = -L$(top_builddir)/projects/DataFaultTolerance/src
TEST_PATH = $(top_srcdir)/projects/DataFaultTolerance/test/array/faultCheck

CXX_TEST_Objects = ${CXX_TESTCODES_REQUIRED_TO_PASS}


$(CXX_TEST_Objects):
	$(CXX) $(TESTCODE_INCLUDES) $(LIB_DIR) $(TEST_PATH)/$(@:=.C) -o $@ -lfaulttolerancearray ; \
	(./$@ ) >> actual.log 2>&1 & \
	sleep 1 

	 
DIFFOUT = if [[ $$(diff -r actual.log $(srcdir)/expected.log) != "" ]]; then :; echo " Files differ " ; exit 1; fi 
	

check-local:
	@echo "Test for DataFaultTolerance faultChecks"
	@rm -rf *.log rose_*
	@$(MAKE) $(CXX_TEST_Objects)
	@${DIFFOUT}
	@echo "********************************************************************************************************************"
	@echo "****** ROSE/projects/DataFaultTolerance/test/array/faultCheck: make check rule complete (terminated normally) ******"
	@echo "********************************************************************************************************************"

EXTRA_DIST = 

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cc *.patch *.gif *.log

	


