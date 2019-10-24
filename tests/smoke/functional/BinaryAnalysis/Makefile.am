include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/smoke/specimens/binary/Specimens.inc

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =
PHONIES =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD = $(ROSE_LIBS)

###############################################################################################################################
# Disassembly tests using Rose::BinaryAnalysis::Partitioner2
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

########################################################################################################################
# Test that certain header files can be included by themselves. Test passes if the source can be compiled.
########################################################################################################################

noinst_PROGRAMS += testSemanticSValue
testSemanticSValue_SOURCES = testSemanticSValue.C

noinst_PROGRAMS += testSemanticRegisterState
testSemanticRegisterState_SOURCES = testSemanticRegisterState.C

noinst_PROGRAMS += testSemanticMemoryState
testSemanticMemoryState_SOURCES = testSemanticMemoryState.C

noinst_PROGRAMS += testSemanticState
testSemanticState_SOURCES = testSemanticState.C

noinst_PROGRAMS += testSemanticRiscOperators
testSemanticRiscOperators_SOURCES = testSemanticRiscOperators.C

noinst_PROGRAMS += testSemanticDispatcher
testSemanticDispatcher_SOURCES = testSemanticDispatcher.C


###############################################################################################################################
# Boilerplate
###############################################################################################################################

endif

check-local: $(TEST_TARGETS)

.PHONY: $(PHONIES)
