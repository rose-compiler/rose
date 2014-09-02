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

# test2013_242.C "(problem with Boost code in ROSE compiling ROSE)" as of 2014-03-26
broken_tests = test2013_242.C

# DQ (5/26/2014): I would like to skip the normalization of the test2013_241.C 
# test code (so that it can be addressed later).  The test code will compile
# fine however it will generate incorrect code when used with the normalizationTranslator
# tested within this directory.
broken_tests += test2013_241.C


normalizationTranslator_SPECIMENS_DIR = $(top_srcdir)/tests/CompileTests/Cxx_tests
normalizationTranslator_SPECIMENS = $(filter-out $(broken_tests), $(ROSE__CXX_TESTS))
normalizationTranslator_TEST_TARGETS = $(addsuffix .passed, $(normalizationTranslator_SPECIMENS))

normalizationTranslator_SWITCHES = \
	--edg:no_warnings -w -rose:verbose 0 $(ROSE_BOOST_NORMAL_INCLUDE_PATH) \
	-I$(normalizationTranslator_SPECIMENS_DIR) -I$(top_srcdir)/tests/CompileTests/A++Code

$(normalizationTranslator_TEST_TARGETS): %.passed: $(ROSE__CXX_TESTS_DIR)/% normalizationTranslator $(TEST_EXIT_STATUS)
	@$(RTH_RUN) \
		CMD="./normalizationTranslator $(normalizationTranslator_SWITCHES) -c $<" \
		TITLE="normalizationTranslator $(notdir $<) [$@]" \
		$(TEST_EXIT_STATUS) "$@"

# Just like normalizationTranslator_TEST_TARGETS except for the stuff we've listed as failing above
broken_TEST_TARGETS = $(addsuffix .passed, $(broken_tests))
$(broken_TEST_TARGETS): %.passed: $(ROSE__CXX_TESTS_DIR)/% normalizationTranslator $(TEST_EXIT_STATUS)
	@$(RTH_RUN) CMD=false \
		TITLE="normalizationTranslator $(notdir $<) [$@]" \
		DISABLED="does not reliably pass" \
		$(TEST_EXIT_STATUS) "$@"

check-local: $(normalizationTranslator_TEST_TARGETS) $(broken_TEST_TARGETS)

clean-local:
	rm -f $(normalizationTranslator_TEST_TARGETS) $(normalizationTranslator_TEST_TARGETS:.passed=.failed)
	rm -f $(broken_TEST_TARGETS) $(broken_TEST_TARGETS:.passed=.failed)
	rm -f -- *.C
