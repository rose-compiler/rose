include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
EXTRA_DIST =
TEST_TARGETS =

########################################################################################################################
# Input specimens for the tests

# All input specimens for the tests, regardless of whether they are expected to pass or fail.
SPECIMENS_all =					\
	test_unordered_map_include.C		\
	test_boost_lambda.C			\
	test_boost_phoenix_v1.C			\
	test_boost_phoenix_v2.C

EXTRA_DIST += $(SPECIMENS_all)

########################################################################################################################
# STUFF FOR THE "testTranslator" TESTS RUN ON EACH INPUT SPECIMEN

# The test executable which runs some test (encoded in the executable) on each specimen.
TRANSLATOR = ../../testTranslator

../../testTranslator:
	make -C ../.. testTranslator

# Flags to use when running the tests.  Note that we need both the preinclude and normal include directories for
# boost because the pre-include path might be empty in some configurations (e.g., when configuring with
# --with-boost=NON_SYSTEM_DIRECTORY as of 2014-04-08).
TRANSLATOR_FLAGS =				\
	--edg:no_warnings			\
	-w					\
	-rose:verbose 0				\
	--edg:restrict				\
	$(ROSE_BOOST_PREINCLUDE_PATH)		\
	$(ROSE_BOOST_NORMAL_INCLUDE_PATH)

# Split specimens into separate lists.  The "special" are those specimens that must be run differently than all the rest (i.e.,
# they need to be run with different command-line switches).
TRANSLATOR_SPECIMENS_special =			\
	test_boost_phoenix_v2.C
TRANSLATOR_SPECIMENS_normal = 			\
	$(filter-out $(TRANSLATOR_SPECIMENS_special), $(SPECIMENS_all))
TRANSLATOR_SPECIMENS_failing =			\
	test_boost_lambda.C			\
	test_boost_phoenix_v1.C			\
	test_boost_phoenix_v2.C
TRANSLATOR_SPECIMENS_passing =			\
	$(filter-out $(TRANSLATOR_SPECIMENS_failing), $(SPECIMENS_all))


# Makefile targets for running tests.  The "special" things need to be separated out because they need different
# rules.  If they appear as the target for multiple rules then we'll get warnings from make.
TRANSLATOR_TARGETS_special = $(addprefix translator-, $(addsuffix .passed, $(TRANSLATOR_SPECIMENS_special)))
TRANSLATOR_TARGETS_normal  = $(addprefix translator-, $(addsuffix .passed, $(TRANSLATOR_SPECIMENS_normal)))
TRANSLATOR_TARGETS_failing = $(addprefix translator-, $(addsuffix .passed, $(TRANSLATOR_SPECIMENS_failing)))
TRANSLATOR_TARGETS_passing = $(addprefix translator-, $(addsuffix .passed, $(TRANSLATOR_SPECIMENS_passing)))

# Rules for testing normal specimens regardless of whether we expect them to pass or fail.
$(TRANSLATOR_TARGETS_normal): translator-%.passed: % $(TRANSLATOR)
	@$(RTH_RUN)									\
		TITLE="testTranslator $* [$@]"						\
		USE_SUBDIR=yes								\
		CMD="$(abspath $(TRANSLATOR)) $(TRANSLATOR_FLAGS) -c $(srcdir)/$*"	\
		$(top_srcdir)/scripts/test_exit_status $@

# Rules for the special specimens regardless of whether we expect them to pass or fail.
test_boost_phoenix_v2_flags =					\
	-rose:verbose 0						\
	-I$(srcdir)/usr/local/mstools/include			\
	-I$(srcdir)/usr/local/mstools/include/phoenix_v2	\
	$(ROSE_BOOST_PREINCLUDE_PATH)				\
	$(ROSE_BOOST_NORMAL_INCLUDE_PATH)
translator-test_boost_phoenix_v2.C.passed: test_boost_phoenix_v2.C $(TRANSLATOR)
	@$(RTH_RUN)										\
		TITLE="testTranslator $< [$@]"							\
		USE_SUBDIR=yes									\
		CMD="$(abspath $(TRANSLATOR)) $(test_boost_phoenix_v2_flags) -c $(abspath $<)"	\
		$(top_srcdir)/scripts/test_exit_status $@

# These are the targets that are run by "make check", those that we expect to pass.
TEST_TARGETS += $(TRANSLATOR_TARGETS_passing)

# These are targets that we might want to run manually.
.PHONY: check-translator-passing check-translator-failing check-translator
check-translator-passing: $(TRANSLATOR_TARGETS_passing)
check-translator-failing: $(TRANSLATOR_TARGETS_failing)
check-translator: check-translator-passing check-translator-failing

# DQ (8/17/2015): This could be made less boost specific, I think.
test_boost_include_ordering:
	../../testTranslator -I/nfs/casc/overture/ROSE/opt/rhel6/x86_64/boost/1_53_0/gcc/4.4.7/include -c $(srcdir)/include_ordering.C


########################################################################################################################
# The original makefile also had $(QMTEST_Objects) which were test targets, but the rules did not work and they were
# not run by "make check".  See the Git commit if you're interested. [Robb P Matzke 2014-04-08]


########################################################################################################################
# Automake boilerplate

# check-local: $(TEST_TARGETS)
check-local: 
	@echo "Tests for boot specific regresstion tests operation."
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
	@$(MAKE) $(TEST_TARGETS)
endif
	@echo "*********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/boost_tests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************************"

clean-local:
	rm -f $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)
	rm -f $(TRANSLATOR_TARGETS_failing) $(TRANSLATOR_TARGETS_failing:.passed=.failed)
