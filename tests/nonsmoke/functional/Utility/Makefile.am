include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/nonsmoke/functional/input_codes/Makefile.for.input.codes

SUBDIRS =

# Forward declarations
noinst_PROGRAMS =
TEST_TARGETS =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


########################################################################################################################
# Test that rose::FileSystem::copyFile works
########################################################################################################################

noinst_PROGRAMS += testCopyFile
testCopyFile_SOURCES = testCopyFile.C

TEST_TARGETS += testCopyFile.passed

testCopyFile.passed: $(top_srcdir)/scripts/test_exit_status testCopyFile
	@$(RTH_RUN)				\
		TITLE="copying files [$@]"	\
		USE_SUBDIR=yes			\
		CMD="$$(pwd)/testCopyFile"	\
		$< $@

########################################################################################################################
# File name classification functions
########################################################################################################################

noinst_PROGRAMS += testFileNameClassifier
testFileNameClassifier_SOURCES = testFileNameClassifier.C

TEST_TARGETS += testFileNameClassifier.passed

testFileNameClassifier.passed: $(top_srcdir)/scripts/test_exit_status testFileNameClassifier
	@$(RTH_RUN)					\
		TITLE="file name classification [$@]"	\
		USE_SUBDIR=yes				\
		DISABLED="assumes certain files exist"	\
		CMD="$$(pwd)/testFileNameClassifier"	\
		$< $@


########################################################################################################################
# Test file name support
########################################################################################################################

noinst_PROGRAMS += filenameSupport
filenameSupport_SOURCES = filenameSupport.C

TEST_TARGETS += filenameSupport.passed

filenameSupport.passed: $(top_srcdir)/scripts/test_exit_status filenameSupport
	@$(RTH_RUN)				\
		TITLE="file name support [$@]"	\
		CMD=./filenameSupport		\
		$< $@


########################################################################################################################
# Test for directory support (SgDirectory IR node)
########################################################################################################################

noinst_PROGRAMS += directorySupport
directorySupport_SOURCES = directorySupport.C

TEST_TARGETS += directorySupport.passed

directorySupport.passed: $(top_srcdir)/scripts/test_exit_status directorySupport $(__minimal_input_code)
	@$(RTH_RUN)									\
		TITLE="directory support [$@]"						\
		CMD="./directorySupport -c -rose:verbose 0 $(__minimal_input_code)"	\
		$< $@


########################################################################################################################
# Tests performance of various graph implementations
########################################################################################################################

noinst_PROGRAMS += graphPerformance
graphPerformance_SOURCES = graphPerformance.C

TEST_TARGETS += graphPerformance.passed

graphPerformance.passed: $(top_srcdir)/scripts/test_exit_status graphPerformance
	@$(RTH_RUN)				\
		TITLE="graph performance [$@]"	\
		CMD="./graphPerformance"	\
		$< $@


########################################################################################################################
# Tests and demonstrates one way to serialize and deserialize a graph
########################################################################################################################

noinst_PROGRAMS += graphIO
graphIO_SOURCES = graphIO.C

TEST_TARGETS += graphIO.passed

graphIO.passed: $(top_srcdir)/scripts/test_exit_status graphIO
	@$(RTH_RUN)				\
		TITLE="graph I/O [$@]"		\
		CMD="./graphIO"			\
		$< $@


########################################################################################################################
# Boilerplate
########################################################################################################################

check-smoke-local:
check-nonsmoke-local: $(TEST_TARGETS)

# FIXME: check-local temporarily runs non-smoke tests as well until Jenkins and the matrix are fixed [Matzke, 2016-09-07]
#check-local: check-smoke-local
check-local: check-smoke-local check-nonsmoke-local
check-all-local: check-smoke-local check-nonsmoke-local

clean-local:
	rm -f $(MOSTLYCLEANFILES)
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -f $(TEST_TARGETS:.passed=.out)
	rm -f $(TEST_TARGETS:.passed=.err)
