include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =
MOSTLYCLEANFILES =

AM_CPPFLAGS = $(ROSE_INCLUDES)
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#------------------------------------------------------------------------------------------------------------------------
noinst_PROGRAMS += testSymbolTable
testSymbolTable_SOURCES = testSymbolTable.C
testSymbolTable_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += test1.passed
test1.passed: input.C testSymbolTable
	@$(RTH_RUN) CMD="./testSymbolTable -c $<" $(TEST_EXIT_STATUS) $@

EXTRA_DIST += input.C
MOSTLYCLEANFILES += rose_input.C

#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
