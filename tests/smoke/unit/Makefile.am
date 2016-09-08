include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
SUBDIRS =

noinst_PROGRAMS =
TEST_TARGETS =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

###############################################################################################################################
# Check boost consistency
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
# Boilerplate
###############################################################################################################################

check-smoke-local: $(TEST_TARGETS)
check-nonsmoke-local:

check-local: check-smoke-local
check-all-local: check-smoke-local check-nonsmoke-local

clean-local:
	rm -f $(MOSTLYCLEANFILES)
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -f $(TEST_TARGETS:.passed=.out)
	rm -f $(TEST_TARGETS:.passed=.err)
