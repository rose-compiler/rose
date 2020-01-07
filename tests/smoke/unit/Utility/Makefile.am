include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
SUBDIRS =

noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)

###############################################################################################################################
# AstAttributeMechanism
###############################################################################################################################

noinst_PROGRAMS += attributeTests
attributeTests_SOURCES = attributeTests.C

TEST_TARGETS += attributeTests.passed

attributeTests.passed: $(top_srcdir)/scripts/test_exit_status attributeTests
	@$(RTH_RUN)					\
		TITLE="AstAttributeMechanism [$@]"	\
		CMD="./attributeTests"			\
		$< $@


###############################################################################################################################
# Tests that diagnostics are working
###############################################################################################################################

noinst_PROGRAMS += testDiagnostics
testDiagnostics_SOURCES = testDiagnostics.C

TEST_TARGETS += testDiagnostics.passed

testDiagnostics.passed: $(top_srcdir)/scripts/test_exit_status testDiagnostics
	@$(RTH_RUN)				\
		TITLE="diagnostic streams [$@]"	\
		CMD="./testDiagnostics"		\
		$< $@


###############################################################################################################################
# Tests the StringUtility functions
###############################################################################################################################

noinst_PROGRAMS += testStringUtility
testStringUtility_SOURCES = testStringUtility.C

TEST_TARGETS += testStringUtility.passed

testStringUtility.passed: $(top_srcdir)/scripts/test_exit_status testStringUtility
	@$(RTH_RUN)				\
		TITLE="string utilities"	\
		CMD="./testStringUtility"	\
		$< $@


###############################################################################################################################
# Tests for parallel sorting
###############################################################################################################################

noinst_PROGRAMS += testSort
testSort_SOURCES = testSort.C

TEST_TARGETS += testSort.passed
EXTRA_DIST += testSort.conf

testSort.passed: testSort.conf testSort
	@$(RTH_RUN)					\
		TITLE="various parallel sorting [$@]"	\
		CMD="$$(pwd)/testSort"			\
		$< $@


###############################################################################################################################
# tests the RangeMap class
###############################################################################################################################

noinst_PROGRAMS += rangeMapTests
rangeMapTests_SOURCES = rangeMapTests.C

TEST_TARGETS += rangeMapTests.passed
EXTRA_DIST += rangeMapTests.ans

rangeMapTests.passed: $(top_srcdir)/scripts/test_with_answer rangeMapTests rangeMapTests.ans
	@$(RTH_RUN)					\
		TITLE="RangeMap container [$@]"		\
		CMD="./rangeMapTests"			\
		ANS="$(srcdir)/rangeMapTests.ans"	\
		$< $@


noinst_PROGRAMS += testRangeMap
testRangeMap_SOURCES = testRangeMap.C

TEST_TARGETS += testRangeMap.passed
testRangeMap.passed: $(top_srcdir)/scripts/test_exit_status testRangeMap
	@$(RTH_RUN)				\
		TITLE="more RangeMap"		\
		CMD="./testRangeMap"		\
		$< $@

###############################################################################################################################
# tests the builtin SHA-256 hash
###############################################################################################################################

noinst_PROGRAMS += testSha256Builtin
testSha256Builtin_SOURCES = testSha256Builtin.C

TEST_TARGETS += testSha256Builtin-9399.passed testSha256Builtin-empty.passed
EXTRA_DIST += testSha256Builtin-9399.ans testSha256Builtin-empty.ans

testSha256Builtin-9399.passed: $(top_srcdir)/scripts/test_with_answer testSha256Builtin testSha256Builtin-9399.passed
	@$(RTH_RUN)						\
		TITLE="SHA-256 builtin [$@]"			\
		CMD="./testSha256Builtin hash 9399"		\
		ANS="$(srcdir)/testSha256Builtin-9399.ans"	\
		$< $@

testSha256Builtin-empty.passed: $(top_srcdir)/scripts/test_with_answer testSha256Builtin testSha256Builtin-empty.passed
	@$(RTH_RUN)						\
		TITLE="SHA-256 builtin [$@]"			\
		CMD="./testSha256Builtin hash 0"		\
		ANS="$(srcdir)/testSha256Builtin-empty.ans"	\
		$< $@

###############################################################################################################################
# Boilerplate
###############################################################################################################################

check-local: $(TEST_TARGETS)
