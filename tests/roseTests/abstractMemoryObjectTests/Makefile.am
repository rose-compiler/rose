include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
EXTRA_DIST =
TEST_TARGETS =

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(srcdir)
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#------------------------------------------------------------------------------------------------------------------------
# memObjectGenerator

noinst_PROGRAMS += memObjectGenerator
memObjectGenerator_SOURCES = memObjectGenerator.C
memObjectGenerator_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

memObjectGenerator_specimens =			\
	struct_simplest.c 			\
	expression.c				\
	function.c				\
	array.c					\
	array2.c				\
	arrayDereferencing.c			\
	test_2011_08_15.cpp			\
	test_2011_08_26.cpp			\
	pointer.cpp				\
	labeledAggregate.cpp

EXTRA_DIST += $(memObjectGenerator_specimens)

memObjectGenerator_test_targets = $(addprefix memObjectGenerator_, $(addsuffix .passed, $(memObjectGenerator_specimens)))
TEST_TARGETS += $(memObjectGenerator_test_targets)
$(memObjectGenerator_test_targets): memObjectGenerator_%.passed: % memObjectGenerator
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/memObjectGenerator -g --edg:no_warnings -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# indexSetTest

noinst_PROGRAMS += indexSetTest
indexSetTest_SOURCES = indexSetTest.C
indexSetTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

TEST_TARGETS += indexSetTest.passed
indexSetTest.passed: indexSetTest
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/indexSetTest" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# ObjSetTest;  contributed by Sriram

noinst_PROGRAMS += ObjSetTest
ObjSetTest_SOURCES = ObjSetTest.C ObjSetPtr.h
ObjSetTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST += array2.c
TEST_TARGETS += ObjSetTest.passed
ObjSetTest.passed: array2.c ObjSetTest
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/ObjSetTest -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
