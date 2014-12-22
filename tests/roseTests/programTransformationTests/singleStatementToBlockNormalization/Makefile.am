include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/input_codes/cxx/Makefile_variables

EXTRA_DIST = 
MOSTLYCLEANFILES = 
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

# Build the single statement to block normalization translator
noinst_PROGRAMS = singleStatementToBlockNormalization
singleStatementToBlockNormalization_SOURCES =  SingleStatementToBlockNormalization.C
singleStatementToBlockNormalization_CPPFLAGS = $(ROSE_INCLUDES)
singleStatementToBlockNormalization_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# The following tests are disable because they're broken in ways unrelated to what this makefile is testing
#   test2013_242.C: ROSE is unable to parse boost-1.47 header file
BROKEN_TESTS = test2013_242.C

singleStatementToBlockNormalization_SPECIMENS = $(filter-out $(BROKEN_TESTS), $(ROSE__CXX_TESTS))

singleStatementToBlockNormalization_TEST_TARGETS = $(addsuffix .passed, $(singleStatementToBlockNormalization_SPECIMENS))

singleStatementToBlockNormalization_SWITCHES = \
	--edg:no_warnings -w -rose:verbose 0 $(ROSE_BOOST_NORMAL_INCLUDE_PATH) -I$(ROSE__CXX_TESTS_DIR) \
	-I$(top_srcdir)/tests/CompileTests/A++Code

$(singleStatementToBlockNormalization_TEST_TARGETS): \
	%.passed: $(ROSE__CXX_TESTS_DIR)/% singleStatementToBlockNormalization $(TEST_EXIT_STATUS)
	@$(RTH_RUN) \
		CMD="./singleStatementToBlockNormalization $(singleStatementToBlockNormalization_SWITCHES) -c $<" \
		$(TEST_EXIT_STATUS) "$@"

check-local: $(singleStatementToBlockNormalization_TEST_TARGETS)

clean-local:
	rm -f -- *.passed
	rm -f -- *.failed
	rm -f -- *.C
