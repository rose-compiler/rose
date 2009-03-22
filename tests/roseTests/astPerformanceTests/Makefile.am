include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = testPerformance rosePerformanceTest

# Allow development using -lrose -ledg (simpler) or using 
# long list of separate libraries (for faster development)
ROSE_DEVELOPMENT_LIBS = $(ROSE_SEPARATE_LIBS)

testPerformance_SOURCES      = testPerformance.C
testPerformance_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)

rosePerformanceTest_SOURCES      = rosePerformanceTest.C
rosePerformanceTest_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)

# source files don't contain anything that would be merged
# TESTCODES = $(srcdir)/performanceTest.C

checkPerformance: testPerformance
	./testPerformance

checkRosePerformance: rosePerformanceTest
	./rosePerformanceTest -rose:compilationPerformanceFile ROSE_PERFORMANCE_DATA.csv -c $(srcdir)/input.C

EXTRA_DIST =

# DQ (3/21/2009): These tests don't appear to run under Cygwin.
if ROSE_BUILD_OS_IS_CYGWIN
check-local:
#	@$(MAKE) checkPerformance
	@echo "****************************************************************************"
	@echo "*** ROSE/tests/roseTests/astPerformanceTests: (commented out for Cygwin) ***"
	@echo "****************************************************************************"
else
check-local:
	@$(MAKE) checkPerformance
	@echo "************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/astPerformanceTests: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************************"
endif

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files rose_*.C rose_performance_report_lockfile.lock

distclean-local:
	rm -rf Templates.DB 


