include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# SUBDIRS = extractFunctionArgumentsTest
SUBDIRS = extractFunctionArgumentsTest  singleStatementToBlockNormalization

noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =

# ROSE Test Harness scripts
CHECK_ANSWER = $(srcdir)/check_rose_output
EXTRA_DIST += check_rose_output
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

# All these executables have the same includes and libraries
AM_CPPFLAGS = $(ROSE_INCLUDES) 
LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#-------------------------------------------------------------------------------------------------------------------------------
# preTest

noinst_PROGRAMS += preTest
preTest_SOURCES = preTest.C
preTest_LDADD = $(LDADD)

pre_TEST_NAMES = pre_1 pre_2 pre_3
pre_TEST_TARGETS = $(addsuffix .passed, $(pre_TEST_NAMES))
TEST_TARGETS += $(pre_TEST_TARGETS)
EXTRA_DIST += pass1.C pass2.C pass3.C rose_pass1.C.save rose_pass2.C.save rose_pass3.C.save

$(pre_TEST_TARGETS): pre_%.passed: $(srcdir)/pass%.C $(srcdir)/rose_pass%.C.save $(CHECK_ANSWER) preTest
	@$(RTH_RUN) CMD=./preTest INPUT=$< ANSWER=$(srcdir)/rose_pass$*.C.save $(CHECK_ANSWER) $@

.PHONY: check-preTest
check-preTest: $(pre_TEST_TARGETS)



#-------------------------------------------------------------------------------------------------------------------------------
# patternRewriteTest

noinst_PROGRAMS += patternRewriteTest
patternRewriteTest_SOURCES = patternRewriteTest.C
patternRewriteTest_LDADD = $(LDADD)

pr_TEST_TARGETS = pr_1.passed
TEST_TARGETS += $(pr_TEST_TARGETS)
EXTRA_DIST += rewrite_test1.C rose_rewrite_test1.C.save

pr_1.passed: $(srcdir)/rewrite_test1.C $(srcdir)/rose_rewrite_test1.C.save $(CHECK_ANSWER) patternRewriteTest
	@$(RTH_RUN) CMD=./patternRewriteTest INPUT=$< ANSWER=$(srcdir)/rose_rewrite_test1.C.save $(CHECK_ANSWER) $@

.PHONY: check-patternRewriteTest
check-patternRewriteTest: $(pr_TEST_TARGETS)


#-------------------------------------------------------------------------------------------------------------------------------
# finiteDifferencingTest

noinst_PROGRAMS += finiteDifferencingTest
finiteDifferencingTest_SOURCES = finiteDifferencingTest.C
finiteDifferencingTest_LDADD = $(LDADD)

fd_TEST_NAMES = fd_1 fd_2 fd_3
fd_TEST_TARGETS = $(addsuffix .passed, $(fd_TEST_NAMES))
TEST_TARGETS += $(fd_TEST_TARGETS)
EXTRA_DIST += finitediff_test1.C finitediff_test2.C finitediff_test3.C
EXTRA_DIST += rose_finitediff_test1.C.save rose_finitediff_test2.C.save rose_finitediff_test3.C.save

$(fd_TEST_TARGETS): fd_%.passed: \
	$(srcdir)/finitediff_test%.C $(srcdir)/rose_finitediff_test%.C.save $(CHECK_ANSWER) finiteDifferencingTest
	@$(RTH_RUN) CMD=./finiteDifferencingTest INPUT=$< ANSWER=$(srcdir)/rose_finitediff_test$*.C.save $(CHECK_ANSWER) $@

.PHONY: check-finiteDifferencingTest
check-finiteDifferencingTest: $(fd_TEST_TARGETS)



#-------------------------------------------------------------------------------------------------------------------------------
# finiteDifferencingDemo

noinst_PROGRAMS += finiteDifferencingDemo
finiteDifferencingDemo_SOURCES = finiteDifferencingDemo.C
finiteDifferencingDemo_LDADD = $(LDADD)

TEST_TARGETS += finiteDifferencingDemo.passed
finiteDifferencingDemo.passed:
	@$(RTH_RUN) \
		DISABLED="not tested in the original makefile" \
		CMD=false \
		$(TEST_EXIT_STATUS) $@


#-------------------------------------------------------------------------------------------------------------------------------
# testFunctionNormalization

noinst_PROGRAMS += testFunctionNormalization
testFunctionNormalization_SOURCES = testFunctionNormalization.C
testFunctionNormalization_LDADD = $(LDADD)

TEST_TARGETS += testFunctionNormalization.passed
testFunctionNormalization.passed:
	@$(RTH_RUN) \
		DISABLED="not tested in the original makefile" \
		CMD=false \
		$(TEST_EXIT_STATUS) $@


#-------------------------------------------------------------------------------------------------------------------------------
# constantFoldingTest

noinst_PROGRAMS += constantFoldingTest constantFoldingTest2
constantFoldingTest_SOURCES = constantFoldingTest.C
constantFoldingTest_LDADD = $(LDADD)
constantFoldingTest2_SOURCES = constantFoldingTest2.C
constantFoldingTest2_LDADD = $(LDADD)

cf_TEST_NAMES = cf_1 cf_2 cf_3
cf_TEST_TARGETS = $(addsuffix .passed, $(cf_TEST_NAMES))
TEST_TARGETS += $(cf_TEST_TARGETS)
EXTRA_DIST += inputCode_constantFoldingTransformation.C inputCode_constantFoldingTransformation2.C

cf_1.passed: $(srcdir)/inputCode_constantFoldingTransformation.C $(TEST_EXIT_STATUS) constantFoldingTest 
	@$(RTH_RUN) CMD="./constantFoldingTest -c $<" $(TEST_EXIT_STATUS) $@

# Liao, 6/27/2009. This test cannot really test what I want. The EDG frontend will do the constant folding (mysteriously to me). 
# Only translator-introduced constant expressions will be optimized by ROSE.
cf_2.passed: $(srcdir)/inputCode_constantFoldingTransformation.C $(TEST_EXIT_STATUS) constantFoldingTest2
	@$(RTH_RUN) CMD="./constantFoldingTest2 -c $< -rose:output rose_inputCode_constantFoldingTransformation_1.C" \
		$(TEST_EXIT_STATUS) $@

cf_3.passed: $(srcdir)/inputCode_constantFoldingTransformation2.C $(TEST_EXIT_STATUS) constantFoldingTest2
	@$(RTH_RUN) CMD="./constantFoldingTest2 -c $<" $(TEST_EXIT_STATUS) $@

.PHONY: check-constantFoldingTests
check-constantFoldingTests: $(cf_TEST_TARGETS)



#-------------------------------------------------------------------------------------------------------------------------------
# implicitCodeGenerationTest

noinst_PROGRAMS += implicitCodeGenerationTest
implicitCodeGenerationTest_SOURCES = implicitCodeGenerationTest.C
implicitCodeGenerationTest_LDADD = $(LDADD)

icg_TEST_NAMES = icg_1 icg_2
icg_TEST_TARGETS = $(addsuffix .passed, $(icg_TEST_NAMES))
TEST_TARGETS += $(icg_TEST_TARGETS)
EXTRA_DIST += inputCode_implicitCodeGenerationTest.C

icg_SWITCHES = -icg:defaultFunctionGenerator -icg:shortCircuitingTransformation -icg:destructorCallAnnotator
icg_1.passed: $(top_srcdir)/tests/CompileTests/Cxx_tests/test2004_116.C $(TEST_EXIT_STATUS) implicitCodeGenerationTest
	@$(RTH_RUN) CMD="./implicitCodeGenerationTest $(icg_SWITCHES) -c $<" $(TEST_EXIT_STATUS) $@
icg_2.passed: $(srcdir)/inputCode_implicitCodeGenerationTest.C $(TEST_EXIT_STATUS) implicitCodeGenerationTest
	@$(RTH_RUN) CMD="./implicitCodeGenerationTest $(icg_SWITCHES) -c $<" $(TEST_EXIT_STATUS) $@



#-------------------------------------------------------------------------------------------------------------------------------
# Automake boilerplate

# DQ (3/21/2009): None of these tests appear to run under Cygwin.
if ROSE_BUILD_OS_IS_CYGWIN
check-local:
else
check-local: $(TEST_TARGETS)
	@echo "*************************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/programTransformationTests: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************************************"
endif

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
