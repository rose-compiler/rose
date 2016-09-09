# This makefile serves as the canonical example for writing tests.
#
# Most of the documentation is in $ROSE/tests/nonsmoke/ExamplesForTestWriters/Makefile.am

########################################################################################################################
# Prologue

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/smoke/specimens/binary/Specimens.inc

noinst_PROGRAMS =
TEST_TARGETS =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


########################################################################################################################
# A checker taking no specimens but which we want to run with a variety of arguments
# See $ROSE/tests/nonsmoke/specimens/binarySpecimens.inc

noinst_PROGRAMS += runAlgorithm
runAlgorithm_SOURCES = runAlgorithm.C

runAlgorithm_algorithms = a b c d

runAlgorithm_targets = $(addprefix runAlgorithm_, $(addsuffix .passed, $(runAlgorithm_algorithms)))
TEST_TARGETS += $(runAlgorithm_targets)

$(runAlgorithm_targets): runAlgorithm_%.passed: runAlgorithm
	@$(RTH_RUN)						\
		TITLE="algorithm $* [$@]"			\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/runAlgorithm --algorithm=$*"	\
		$(top_srcdir)/scripts/test_exit_status $@

.PHONY: check-runAlgorithm
check-runAlgorithm: $(runAlgorithm_targets)



########################################################################################################################
# Boilerplate.

check-smoke-local: $(TEST_TARGETS)

check-local: check-smoke-local
check-all-local: check-smoke-local check-nonsmoke-local

clean-local:
	rm -f $(MOSTLYCLEANFILES)
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -f $(TEST_TARGETS:.passed=.out)
	rm -f $(TEST_TARGETS:.passed=.err)
