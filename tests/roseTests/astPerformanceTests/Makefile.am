include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

EXTRA_DIST = tests.conf

# Forward declarations
bin_PROGRAMS  =
noinst_PROGRAMS =
ROSE_TESTS =
MOSTLYCLEANFILES =


################################################################################
# testPerformance -- tests how fast the "new" operator works
################################################################################
bin_PROGRAMS += testPerformance
testPerformance_SOURCES = testPerformance.C
testPerformance_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
if !ROSE_BUILD_OS_IS_CYGWIN
    ROSE_TESTS += testPerformance
endif
testPerformance.passed: testPerformance
	@$(RTH_RUN) EXE=./$< $(srcdir)/tests.conf $@

################################################################################
# rosePerformanceTest
################################################################################
bin_PROGRAMS += rosePerformanceTest
rosePerformanceTest_SOURCES = rosePerformanceTest.C
rosePerformanceTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
if !ROSE_BUILD_OS_IS_CYGWIN
    ROSE_TESTS += rosePerformanceTest
endif
rosePerformanceTest.passed: rosePerformanceTest
	@$(RTH_RUN) EXE=./$< ARGS="-rose:compilationPerformanceFile ROSE_PERFORMANCE_DATA.csv -c $(srcdir)/input.C" \
		$(srcdir)/tests.conf $@
MOSTLYCLEANFILES += ROSE_PERFORMANCE_DATA.csv

################################################################################
# astThreadedCreation -- creates/deletes nodes with lots of threads
################################################################################
noinst_PROGRAMS += astThreadedCreation
astThreadedCreation_SOURCES = astThreadedCreation.C
astThreadedCreation_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
ROSE_TESTS += astThreadedCreation
astThreadedCreation.passed: astThreadedCreation
	@$(RTH_RUN) EXE=./$< $(srcdir)/tests.conf $@








################################################################################
# Run all tests
################################################################################
check-local: $(addsuffix .passed, $(ROSE_TESTS)) $(srcdir)/tests.conf
MOSTLYCLEANFILES += \
	$(addsuffix .passed, $(ROSE_TESTS)) \
	$(addsuffix .failed, $(ROSE_TESTS))
