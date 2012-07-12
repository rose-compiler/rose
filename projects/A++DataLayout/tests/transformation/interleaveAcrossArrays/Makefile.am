include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Set the A++ Install Directory correctly
APP_HOME = $(top_srcdir)/../../A++/installDir/A++/install

PERF_TESTCODES_REQUIRED_TO_PASS = \
perf_test1.C

CXX_TESTCODES_REQUIRED_TO_PASS = \
test1.C \
test2.C \
test3.C \
test4.C \
test5.C \
test6.C \
test7.C

TESTCODE_CURRENTLY_FAILING = 

ALL_TESTCODES = \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# used to find omp.h, added it as one of  rose headers
TESTCODE_INCLUDES = -I$(top_srcdir)/projects/A++DataLayout/include 

ROSE_FLAGS = -rose:skipfinalCompileStep --edg:no_warnings -w -rose:verbose 0 --edg:restrict 

LAYOUTEXEC = $(top_builddir)/projects/A++DataLayout/ArrayLayoutTransformation

CXX_TEST_Objects = ${CXX_TESTCODES_REQUIRED_TO_PASS:.C=.o}


$(CXX_TEST_Objects): $(LAYOUTEXEC) $(@:.o=.C)
	 $(LAYOUTEXEC) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) $(srcdir)/spec1.C $(srcdir)/$(@:.o=.C) >> out.log 2>> actual.log & \
	 sleep 1 

	 
DIFFOUT = if [[ $$(diff -r actual.log $(srcdir)/expected.log) != "" ]]; then :; echo " Files differ " ; exit ; fi 

PERF_TEST_Objects = ${PERF_TESTCODES_REQUIRED_TO_PASS:.C=.o}

$(PERF_TEST_Objects): $(LAYOUTEXEC) $(@:.o=.C)
	 $(LAYOUTEXEC) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) $(srcdir)/perf_spec1.C $(srcdir)/$(@:.o=.C) > log.out

performance:
	@echo "Performance Test for A++DataLayout interleaveAcrossArrays."
	@$(MAKE) $(PERF_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/A++DataLayout/transformation/interleaveAcrossArrays/tests: make performance rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

check-local:
	@echo "Test for A++DataLayout interleaveAcrossArrays."
	@rm -rf *.log
	@$(MAKE) $(CXX_TEST_Objects)
	@${DIFFOUT}
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/A++DataLayout/transformation/interleaveAcrossArrays/tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES) simpleA++.h transformation*.h

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cc *.patch *.gif *.log

	


