include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
noinst_HEADERS =
TEST_TARGETS =
EXTRA_DIST =
MOSTLYCLEANFILES =

AM_CPPFLAGS = $(ROSE_INCLUDES)
LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#------------------------------------------------------------------------------------------------------------------------
if !ROSE_BUILD_OS_IS_CYGWIN
noinst_PROGRAMS += testCommentInsertion
testCommentInsertion_SOURCES = testCommentInsertion.C

TEST_TARGETS += CommentInsertion.passed
CommentInsertion.passed: inputProgram5.C testCommentInsertion
	@$(RTH_RUN) \
		DISABLED="commented out in the original makefile" \
		CMD="./testCommentInsertion $(AM_CPPFLAGS) -c $<" \
		$(TEST_EXIT_STATUS) $@

EXTRA_DIST += inputProgram5.C
MOSTLYCLEANFILES += rose_inputProgram5.C
endif

#------------------------------------------------------------------------------------------------------------------------
if !ROSE_BUILD_OS_IS_CYGWIN
noinst_PROGRAMS += testIncludeDirectiveInsertion
testIncludeDirectiveInsertion_SOURCES = testIncludeDirectiveInsertion.C

TEST_TARGETS += IncludeDirectiveInsertion.passed
IncludeDirectiveInsertion.passed: inputProgram6.C testIncludeDirectiveInsertion
	@$(RTH_RUN) \
		CMD="./testIncludeDirectiveInsertion -rose:verbose 0 -I$(srcdir) $(TAU_INCLUDES) -c $<" \
		$(TEST_EXIT_STATUS) $@

EXTRA_DIST += inputProgram6.C
MOSTLYCLEANFILES += rose_inputProgram6.C rose_transformation_1_inputProgram6.C
endif

#------------------------------------------------------------------------------------------------------------------------
if !ROSE_BUILD_OS_IS_CYGWIN
noinst_PROGRAMS += testRewritePermutations
noinst_HEADERS += testNewRewrite.h
testRewritePermutations_SOURCES = testNewRewrite.C testRewritePermutations.C

# broken: inputDeclarationTest.C is not in the ROSE source tree, so we can't have it as a dependency [RPM 2012-09-25]
TEST_TARGETS += testAccumulatedAttribute.passed
testAccumulatedAttribute.passed: testRewritePermutations
	@$(RTH_RUN) \
		DISABLED="AST rewrite mechanism still has a bug [DQ 8/6/2005]" \
		CMD="./testRewritePermutations -rose:verbose 0 -E $(ROSE_FLAGS) $(AM_CPPFLAGS) -c $(srcdir)/inputDeclarationTest.C" \
		$(TEST_EXIT_STATUS) $@
#EXTRA_DIST += inputDeclarationTest.C
MOSTLYCLEANFILES += rose_inputDeclarationTest.C

TEST_TARGETS += RewritePermutations.passed
RewritePermutations.passed: inputProgram1.C testRewritePermutations
	@$(RTH_RUN) \
		DISABLED="commented out in the original makefile" \
		CMD="./testRewritePermutations $(ROSE_FLAGS) $(AM_CPPFLAGS) -c $<" \
		$(TEST_EXIT_STATUS) $@

EXTRA_DIST += inputProgram1.C
MOSTLYCLEANFILES += rose_inputProgram1.C
endif

#------------------------------------------------------------------------------------------------------------------------
if !ROSE_BUILD_OS_IS_CYGWIN
noinst_PROGRAMS += testMidLevelInterface
testMidLevelInterface_SOURCES = testMidLevelInterface.C

TEST_TARGETS += localMidLevelInterfaceTest.passed
midLevelInterface_includes = -I$(srcdir) -I/home/bmiller/SOFTWARE/tau-2.12.9/include $(AM_CPPFLAGS)
localMidLevelInterfaceTest.passed: inputProgram2.C testMidLevelInterface
	@$(RTH_RUN) \
		DISABLED="not executed in the original makefile" \
		CMD="./testMidLevelInterface -rose:verbose 0 $(ROSE_FLAGS) $(midLevelInterface_includes) -c $<" \
		$(TEST_EXIT_STATUS) $@

EXTRA_DIST += inputProgram2.C
MOSTLYCLEANFILES += rose_inputProgram2.C
endif

#------------------------------------------------------------------------------------------------------------------------
if !ROSE_BUILD_OS_IS_CYGWIN
noinst_PROGRAMS += testRewriteReplacementPermutations
testRewriteReplacementPermutations_SOURCES = testNewRewrite.C testRewriteReplacementPermutations.C

TEST_TARGETS += RewriteReplacementPermutations.passed
RewriteReplacementPermutations.passed: inputProgram4.C testRewriteReplacementPermutations
	@$(RTH_RUN) \
		DISABLED="fails because the AST rewrite mechanism still has a bug [DQ 8/6/2005]" \
		CMD="./testRewriteReplacementPermutations $(ROSE_FLAGS) $(AM_CPPFLAGS) -c $<" \
		$(TEST_EXIT_STATUS) $@

EXTRA_DIST += inputProgram4.C
MOSTLYCLEANFILES += rose_inputProgram4.C
endif

#------------------------------------------------------------------------------------------------------------------------
if !ROSE_BUILD_OS_IS_CYGWIN
noinst_PROGRAMS += testReplacementStatements
testReplacementStatements_SOURCES = testReplacementStatements.C

# DQ (7/19/2005): Uncommented so that we can test it with AST Merge mechanism and fix any other problems.
# DQ (12/6/2004): Note current rewrite mechanism is not working  (even though other tests pass) because the scopes
# of newly generated IR nodes fron the string based AST generation do not have scopes set correctly!
# Also the EDG memory can not be freed for the rewrite system to work!
TEST_TARGETS += ReplaceStatements.passed
ReplaceStatements.passed: inputProgram2.C testReplacementStatements
	@$(RTH_RUN) \
		DISABLED="fails until we update scopes of statements added by rewrite system [DQ 12/6/2004]" \
		CMD="./testReplacementStatements $(ROSE_FLAGS) $(AM_CPPFLAGS) -c $<" \
		$(TEST_EXIT_STATUS) $@

EXTRA_DIST += inputProgram2.C
MOSTLYCLEANFILES += rose_inputProgram2.C
endif

#------------------------------------------------------------------------------------------------------------------------
if !ROSE_BUILD_OS_IS_CYGWIN
noinst_PROGRAMS += testRemoveStatements
testRemoveStatements_SOURCES = testRemoveStatements.C

TEST_TARGETS += RemoveStatements.passed
RemoveStatements.passed: inputProgram3.C testRemoveStatements
	@$(RTH_RUN) \
		DISABLED="commented out in original makefile" \
		CMD="./testRemoveStatements $(ROSE_FLAGS) $(AM_CPPFLAGS) -c $<" \
		$(TEST_EXIT_STATUS) $@

EXTRA_DIST += inputProgram3.C
MOSTLYCLEANFILES += rose_inputProgram3.C
endif

#------------------------------------------------------------------------------------------------------------------------
if !ROSE_BUILD_OS_IS_CYGWIN
noinst_PROGRAMS += testExample1
testExample1_SOURCES = testExample1.C

TEST_TARGETS += testExamples.passed
testExamples.passed: inputProgram1.C testExample1
	@$(RTH_RUN) \
		DISABLED="not executed by the original makefile" \
		CMD="./testExample1 $(ROSE_FLAGS) $(AM_CPPFLAGS) -c $<" \
		$(TEST_EXIT_STATUS) $@

EXTRA_DIST += inputProgram1.C
MOSTLYCLEANFILES += rose_inputProgram1.C
endif

#------------------------------------------------------------------------------------------------------------------------
if !ROSE_BUILD_OS_IS_CYGWIN
noinst_PROGRAMS += testDeclarationPrefixGeneration
testDeclarationPrefixGeneration_SOURCES = testDeclarationPrefixGeneration.C

TEST_TARGETS += PrefixAndSuffixTest.passed PrefixAndSuffixTestNewEDGFrontEnd.passed PrefixAndSuffixTestKCCFrontEnd.passed

PrefixAndSuffixTest.passed: testDeclarationPrefixGeneration_testCode.C testDeclarationPrefixGeneration
	@$(RTH_RUN) \
		DISABLED="not executed by the original makefile" \
		CMD="./testDeclarationPrefixGeneration $(ROSE_FLAGS) $(AM_CPPFLAGS) -c $<" \
		$(TEST_EXIT_STATUS) $@
PrefixAndSuffixTestNewEDGFrontEnd.passed: testDeclarationPrefixGeneration_testCode.C testDeclarationPrefixGeneration
	@$(RTH_RUN) \
		DISABLED="not executed by the original makefile" \
		CMD="./testDeclarationPrefixGeneration -rose:test 2 $(ROSE_FLAGS) $(AM_CPPFLAGS) -c $<" \
		$(TEST_EXIT_STATUS) $@
PrefixAndSuffixTestKCCFrontEnd.passed: testDeclarationPrefixGeneration_testCode.C testDeclarationPrefixGeneration
	@$(RTH_RUN) \
		DISABLED="not executed by the original makefile" \
		CMD="./testDeclarationPrefixGeneration -rose:test 1 $(ROSE_FLAGS) $(AM_CPPFLAGS) -c $<" \
		$(TEST_EXIT_STATUS) $@

EXTRA_DIST += testDeclarationPrefixGeneration_testCode.C
MOSTLYCLEANFILES += rose_testDeclarationPrefixGeneration_testCode.C
endif

#------------------------------------------------------------------------------------------------------------------------
if !ROSE_BUILD_OS_IS_CYGWIN
noinst_PROGRAMS += tauifyPreprocessor
tauifyPreprocessor_SOURCES = tauifyPreprocessor.C TauMidTraversal.h
tauifyPreprocessor.o : $(srcdir)/tauTestProgram.C TauMidTraversal.h

TEST_TARGETS += testMillerBug.passed
testMillerBug.passed: tauTestProgram.C TauMidTraversal.h tauifyPreprocessor
	@$(RTH_RUN) \
		DISABLED="not executed by the original makefile" \
		CMD="./tauifyPreprocessor $(AM_CPPFLAGS) -c $<" \
		$(TEST_EXIT_STATUS) $@
EXTRA_DIST += tauTestProgram.C
MOSTLYCLEANFILES += rose_tauTestProgram.C

TEST_TARGETS += testTAU.passed
testTAU.passed: $(srcdir)/tauExample.C
	@$(RTH_RUN) \
		DISABLED="not executed by the original makefile" \
		CMD="g++ -I$(top_srcdir)/tutorial/TAU_INCLUDE_DIR $(TAU_INCLUDES) -o tauExample.o -c $<" \
		$(TEST_EXIT_STATUS) $@
EXTRA_DIST += tauExample.C
MOSTLYCLEANFILES += rose_tauExample.C
endif

EXTRA_DIST += tauProtos.h 
#------------------------------------------------------------------------------------------------------------------------
if !ROSE_BUILD_OS_IS_CYGWIN
noinst_PROGRAMS += testTranslator2004_01
testTranslator2004_01_SOURCES = testTranslator2004_01.C

TEST_TARGETS += testTranslator2004_01.passed
testTranslator2004_01.passed:
	@$(RTH_RUN) \
		DISABLED="not tested by the original makefile" \
		CMD=false \
		$(TEST_EXIT_STATUS) $@
endif

#------------------------------------------------------------------------------------------------------------------------
# Automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
