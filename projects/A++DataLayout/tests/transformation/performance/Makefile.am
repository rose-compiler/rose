include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Set the A++ Install Directory correctly
APP_HOME = $(top_srcdir)/../../A++/installDir/A++/install

CXX_TESTCODES_REQUIRED_TO_PASS = \
perf_test1.C \
perf_test2.C \
perf_test3.C \
perf_test4.C \
perf_test5.C \
perf_test6.C \
perf_test7.C \
perf_test8.C \
perf_test9.C \
perf_test10.C \
perf_test11.C

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
	 $(LAYOUTEXEC) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) $(srcdir)/perf_spec_nolayout.C $(srcdir)/$(@:.o=.C) > log.out
	 mv rose_$(@:.o=.C) rose_wolayout_$(@:.o=.C)


LAYOUTFILES = for i in {1..11} ; do $(LAYOUTEXEC) $(ROSE_FLAGS) $(TESTCODE_INCLUDES) $(srcdir)/perf_spec$$i.C $(srcdir)/perf_test$$i.C > log.out ; mv rose_perf_test$$i.C rose_layout_perf_test$$i.C ; done 

check-local:
	@echo "Test for A++DataLayout preprocessor."
	@rm -rf rose_*.C
	@$(MAKE) $(CXX_TEST_Objects)
	@$(LAYOUTFILES)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/A++DataLayout/transformation/performance/tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES) simpleA++.h transformation*.h

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out rose_*.cc *.patch *.gif

	


