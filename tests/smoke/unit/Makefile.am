include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (3/8/2017): Restrict testing to versions of GNU compiler for now for specific versions of EDG.
# Matzke (4/21/2017): None of these tests do anything with EDG, so this complicated logic shouldn't be necessary here. Reverting.

# These unit test directories should be listed so lower software layers are tested before higher layers.
SUBDIRS = Boost Sawyer

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
SUBDIRS += BinaryAnalysis
endif

noinst_PROGRAMS=
TEST_TARGETS=
AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)


noinst_PROGRAMS += testTree
testTree_SOURCES = testTree.C
TEST_TARGETS += testTree.passed
testTree.passed: $(top_srcdir)/scripts/test_exit_status testTree
	@$(RTH_RUN) \
		TITLE="tree [$@]" \
		CMD="./testTree" \
		$< $@

check-local: $(TEST_TARGETS)
