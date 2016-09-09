include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/input_codes/Makefile.for.input.codes

# Forward declarations
noinst_PROGRAMS =
TEST_TARGETS =

AM_CPPFLAGS = $(ROSE_INCLUDES)

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

noinst_PROGRAMS += testFileNameClassifier
testFileNameClassifier_SOURCES = testFileNameClassifier.C
testFileNameClassifier_LDADD   = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
if ROSE_BUILD_OS_IS_CYGWIN
# This test fails to run under Cygwin. [DQ 2009-03-21]
else
TEST_TARGETS += testFileNameClassifier.passed
endif
testFileNameClassifier.passed: $(top_srcdir)/scripts/test_exit_status testFileNameClassifier
	@$(RTH_RUN) CMD=./testFileNameClassifier $< $@

# Test file name support
noinst_PROGRAMS += filenameSupport
filenameSupport_SOURCES = filenameSupport.C
filenameSupport_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += filenameSupport.passed
filenameSupport.passed: $(top_srcdir)/scripts/test_exit_status filenameSupport
	@$(RTH_RUN) CMD=./filenameSupport $< $@

# Test for directory support (SgDirectory IR node)
noinst_PROGRAMS += directorySupport
directorySupport_SOURCES = directorySupport.C
directorySupport_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
TEST_TARGETS += directorySupport.passed
directorySupport.passed: $(top_srcdir)/scripts/test_exit_status directorySupport $(__minimal_input_code)
	@$(RTH_RUN) CMD="./directorySupport -c -rose:verbose 0 $(__minimal_input_code)" $< $@

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

check-local: $(TEST_TARGETS)

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f rose_*.C *.dot
	rm -rf xxx_new_directory_node_support
	rm -f $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)

distclean-local:
	rm -rf Templates.DB
