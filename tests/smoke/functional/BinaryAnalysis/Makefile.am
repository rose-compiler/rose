include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/smoke/specimens/binary/Specimens.inc

if USE_BINARY_ANALYSIS_TESTS

noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =
PHONIES =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

###############################################################################################################################
# Disassembly tests using rose::BinaryAnalysis::Partitioner2
###############################################################################################################################

noinst_PROGRAMS += testPartitioner2
testPartitioner2_SOURCES = testPartitioner2.C

testPartitioner2_specdir = $(smoke_specimens_binary_directory)
testPartitioner2_specimens = x86-elf-exe
testPartitioner2_targets = $(addprefix testPartitioner2_, $(addsuffix .passed, $(testPartitioner2_specimens)))
testPartitioner2_answers = $(testPartitioner2_targets:.passed=.ans)

EXTRA_DIST += $(testPartitioner2_answers)
TEST_TARGETS += $(testPartitioner2_targets)

$(testPartitioner2_targets): \
testPartitioner2_%.passed: $(testPartitioner2_specdir)/% testPartitioner2 testPartitioner2_%.ans
	@$(RTH_RUN)						\
		TITLE="testPartitioner2 $(notdir $<) [$@]"	\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/testPartitioner2 $<"		\
		ANS="$(srcdir)/testPartitioner2_$*.ans"	\
		$(top_srcdir)/scripts/test_with_answer $@

PHONIES += check-testPartitioner2
check-testPartitioner2: $(testPartitioner2_targets)


###############################################################################################################################
# Boilerplate
###############################################################################################################################

endif

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

.PHONY: $(PHONIES)
