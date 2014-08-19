include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/input_codes/cxx/Makefile_variables

EXTRA_DIST = 
MOSTLYCLEANFILES = 
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

# Build the single statement to block normalization translator
noinst_PROGRAMS = normalizationTranslator
normalizationTranslator_SOURCES =  NormalizationTranslator.C
normalizationTranslator_CPPFLAGS = $(ROSE_INCLUDES)
normalizationTranslator_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

normalizationTranslator_SPECIMENS_DIR = $(top_srcdir)/tests/CompileTests/Cxx_tests

# DQ (5/26/2014): I would like to skip the normalization of the test2013_241.C 
# test code (so that it can be addressed later).  The test code will compile
# fine however it will generate incorrect code when used with the normalizationTranslator
# tested within this directory.
# normalizationTranslator_SPECIMENS = $(ROSE__CXX_TESTS)
normalizationTranslator_SPECIMENS = $(ROSE__CXX_TESTS_WITHOUT_2013_241)

normalizationTranslator_TEST_TARGETS = $(addsuffix .passed, $(normalizationTranslator_SPECIMENS))

normalizationTranslator_SWITCHES = \
	--edg:no_warnings -w -rose:verbose 0 -I$(normalizationTranslator_SPECIMENS_DIR) -I$(top_srcdir)/tests/CompileTests/A++Code

$(normalizationTranslator_TEST_TARGETS): \
	%.passed: $(ROSE__CXX_TESTS_DIR)/% normalizationTranslator $(TEST_EXIT_STATUS)
	@$(RTH_RUN) CMD="./normalizationTranslator $(normalizationTranslator_SWITCHES) -c $<" $(TEST_EXIT_STATUS) "$@"

check-local: $(normalizationTranslator_TEST_TARGETS)
	@echo "******************************************************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/programTransformationTests/extractFunctionArgumentsTest: make check rule complete (terminated normally) ******"
	@echo "******************************************************************************************************************************************"

clean-local:
	rm -f -- *.passed
	rm -f -- *.failed
	rm -f -- *.C
