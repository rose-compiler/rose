include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/input_codes/Makefile.for.input.codes

# Forward declarations
noinst_PROGRAMS =
TEST_TARGETS =

AM_CPPFLAGS = $(ROSE_INCLUDES)
EXTRA_DIST = tests.conf

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# Test that rose::FileSystem::copyFile works
noinst_PROGRAMS += testCopyFile
testCopyFile_SOURCES = testCopyFile.C
testCopyFile_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += testCopyFile.passed
testCopyFile.passed: testCopyFile
	@$(RTH_RUN)						\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/testCopyFile"			\
		$(top_srcdir)/scripts/test_exit_status $@

# Test that boost::regex works
noinst_PROGRAMS += testBoostRegex
testBoostRegex_SOURCES = testBoostRegex.C
testBoostRegex_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += testBoostRegex.passed
testBoostRegex.passed: testBoostRegex
	@$(RTH_RUN) CMD=./testBoostRegex $(top_srcdir)/scripts/test_exit_status $@

# Tests the RangeMap class
noinst_PROGRAMS += testRangeMap
testRangeMap_SOURCES = testRangeMap.C
testRangeMap_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += testRangeMap.passed
testRangeMap.passed: tests.conf testRangeMap
	@$(RTH_RUN) CMD=./testRangeMap $< $@

noinst_PROGRAMS += testFileNameClassifier
testFileNameClassifier_SOURCES = testFileNameClassifier.C
testFileNameClassifier_LDADD   = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
if ROSE_BUILD_OS_IS_CYGWIN
# This test fails to run under Cygwin. [DQ 2009-03-21]
else
TEST_TARGETS += testFileNameClassifier.passed
endif
testFileNameClassifier.passed: tests.conf testFileNameClassifier
	@$(RTH_RUN) CMD=./testFileNameClassifier $< $@

# Tests the StringUtility functions
noinst_PROGRAMS += testStrings
testStrings_SOURCES = testStrings.C
testStrings_LDADD   = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += testStrings.passed
testStrings.passed: tests.conf testStrings
	@$(RTH_RUN) CMD=./testStrings $< $@

# Tests that diagnostics are working
noinst_PROGRAMS += testDiagnostics
testDiagnostics_SOURCES = testDiagnostics.C
testDiagnostics_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += testDiagnostics.passed
testDiagnostics.passed: $(top_srcdir)/scripts/test_exit_status testDiagnostics
	@$(RTH_RUN)				\
		TITLE="diagnostics [$@]"	\
		CMD=./testDiagnostics $< $@

# Test file name support
noinst_PROGRAMS += filenameSupport
filenameSupport_SOURCES = filenameSupport.C
filenameSupport_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += filenameSupport.passed
filenameSupport.passed: tests.conf filenameSupport
	@$(RTH_RUN) CMD=./filenameSupport $< $@

# Test for directory support (SgDirectory IR node)
noinst_PROGRAMS += directorySupport
directorySupport_SOURCES = directorySupport.C
directorySupport_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += directorySupport.passed
directorySupport.passed: tests.conf directorySupport $(__minimal_input_code)
	@$(RTH_RUN) CMD=./directorySupport ARGS="-c -rose:verbose 0 $(__minimal_input_code)" $< $@

# Tests for parallel sorting
noinst_PROGRAMS += testSort
testSort_SOURCES = testSort.C
testSort_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += testSort.passed
EXTRA_DIST += testSort.conf
testSort.passed: testSort.conf testSort
	@$(RTH_RUN) TITLE="various parallel sorting [$@]" CMD="$$(pwd)/testSort"  $< $@

# Tests performance of various graph implementations
noinst_PROGRAMS += graphPerformance
graphPerformance_SOURCES = graphPerformance.C
graphPerformance_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += graphPerformance.passed
graphPerformance.passed: graphPerformance
	@$(RTH_RUN) TITLE="graph performance [$@]" CMD="$(abspath $<)" $(top_srcdir)/scripts/test_exit_status $@

# Tests and demonstrates one way to serialize and deserialize a graph
noinst_PROGRAMS += graphIO
graphIO_SOURCES = graphIO.C
graphIO_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += graphIO.passed
graphIO.passed: graphIO
	@$(RTH_RUN) TITLE="graph I/O [$@]" CMD="$(abspath $<)" $(top_srcdir)/scripts/test_exit_status $@

# tests the RangeMap class
noinst_PROGRAMS += rangeMapTests
rangeMapTests_SOURCES = rangeMapTests.C
rangeMapTests_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += rangeMapTests.passed
EXTRA_DIST += rangeMapTests.ans
rangeMapTests.passed: rangeMapTests rangeMapTests.ans $(top_srcdir)/scripts/test_with_answer
	@$(RTH_RUN)						\
		TITLE="RangeMap tests [$@]"			\
		CMD="$(abspath $<)"				\
		ANS="$(srcdir)/rangeMapTests.ans"		\
		$(top_srcdir)/scripts/test_with_answer $@

# Test AstAttributeMechanism
noinst_PROGRAMS += attributeTests
attributeTests_SOURCES = attributeTests.C
attributeTests_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += attributeTests.passed
attributeTests.passed: attributeTests
	@$(RTH_RUN)						\
		TITLE="AstAttributeMechanism tests [$@]"	\
		CMD="$(abspath $<)"				\
		$(top_srcdir)/scripts/test_exit_status $@


check-local: $(TEST_TARGETS)

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f rose_*.C *.dot
	rm -rf xxx_new_directory_node_support
	rm -f $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)

distclean-local:
	rm -rf Templates.DB
