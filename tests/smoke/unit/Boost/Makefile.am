include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
SUBDIRS =

noinst_PROGRAMS =
TEST_TARGETS =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)

###############################################################################################################################
# Check boost library linkage consistency
###############################################################################################################################

noinst_PROGRAMS += testBoost
testBoost_SOURCES = testBoost.C

TEST_TARGETS += testBoost.passed

testBoost.passed: $(top_srcdir)/scripts/test_exit_status testBoost
	@$(RTH_RUN)				\
		TITLE="boost linkage [$@]"	\
		CMD="./testBoost"		\
		$< $@


###############################################################################################################################
# Test that boost::regex works. Also ensures that boost hasn't been compiled with -DBOOST_REGEX_MATCH_EXTRA
###############################################################################################################################

noinst_PROGRAMS += testBoostRegex
testBoostRegex_SOURCES = testBoostRegex.C

TEST_TARGETS += testBoostRegex.passed

testBoostRegex.passed: $(top_srcdir)/scripts/test_exit_status testBoostRegex
	@$(RTH_RUN)					\
		TITLE="boost regex library [$@]"	\
		CMD=./testBoostRegex			\
		$< $@


###############################################################################################################################
# Boilerplate
###############################################################################################################################

check-smoke-local: $(TEST_TARGETS)
