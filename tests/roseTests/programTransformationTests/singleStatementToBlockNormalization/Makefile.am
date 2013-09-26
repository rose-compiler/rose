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

singleStatementToBlockNormalization_SPECIMENS = $(ROSE__CXX_TESTS)

singleStatementToBlockNormalization_TEST_TARGETS = $(addsuffix .passed, $(singleStatementToBlockNormalization_SPECIMENS))

singleStatementToBlockNormalization_SWITCHES = \
	--edg:no_warnings -w -rose:verbose 0 -I$(ROSE__CXX_TESTS_DIR) -I$(top_srcdir)/tests/CompileTests/A++Code

$(singleStatementToBlockNormalization_TEST_TARGETS): \
	%.passed: $(ROSE__CXX_TESTS_DIR)/% singleStatementToBlockNormalization $(TEST_EXIT_STATUS)
	@$(RTH_RUN) CMD="./singleStatementToBlockNormalization $(singleStatementToBlockNormalization_SWITCHES) -c $<" $(TEST_EXIT_STATUS) "$@"

check-local: $(singleStatementToBlockNormalization_TEST_TARGETS)
	@echo "*************************************************************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/programTransformationTests/singleStatementToBlockNormalization: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************************************************************************"

clean-local:
	rm -f -- *.out
	rm -f -- *.err
	rm -f -- *.passed
	rm -f -- *.failed
	rm -f -- *.C
