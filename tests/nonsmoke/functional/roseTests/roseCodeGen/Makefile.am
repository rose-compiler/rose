include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#------------------------------------------------------------------------------------------------------------------------

noinst_PROGRAMS += testCodeGen
testCodeGen_SOURCES = testCodeGen.C
testCodeGen_CPPFLAGS = -DMY_HEADER_PATH=\"$(abs_srcdir)\"
testCodeGen_LDADD = $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testCodeGen.passed

testCodeGen.passed: testCodeGen
	@$(RTH_RUN) CMD="./testCodeGen" $(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)https://www.youtube.com/watch?list=RDupQMjdj-jz8&v=InIw_a9FEtg
