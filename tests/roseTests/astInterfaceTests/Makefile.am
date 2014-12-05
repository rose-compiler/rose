# Liao, Jan. 9, 2008
# Makefile.am for a set of test AST builders and test input code
#-------------------------------------------------------------
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

INCLUDES =  \
        $(ROSE_INCLUDES) \
        -I$(top_srcdir)/src/frontend/SageIII/sageInterface
# We install a few test programs since they might be useful for external users
bin_PROGRAMS = loopInterchange loopUnrolling loopTiling loopCollapsing moveDeclarationToInnermostScope

#not for installation
noinst_PROGRAMS = \
    buildFunctionDeclaration buildNondefiningFunction \
    findMain buildVariableDeclaration buildAssignmentStmt \
    buildFunctionCalls buildPragmaDeclaration attachComment \
    insertHeader buildExpression buildStructDeclaration \
    lookupNamedType buildFile movePreprocessingInfo buildIfStmt \
    buildCpreprocessorDefineDeclaration instrumentEndOfFunction \
    isUpcSharedType isUpcPhaseLessSharedType buildLabelStatement \
    buildSizeOfOp abiStuffTest abiStuffTestUPC buildNullStatement \
    buildForStmt canonicalLoop loopNormalization collectReadWriteVariables \
    typeTraits livenessAnalysis buildProcedureHeaderStatement replaceMacroCalls \
    buildAbstractHandle getDependentDecls buildStructDeclaration2 buildTypedefDeclaration \
    deepDelete insertStatementBeforeFunction removeStatementCommentRelocation \
    generateUniqueName annotateExpressionsWithUniqueNames buildExternalStatement \
    buildCommonBlock doLoopNormalization buildLabelStatement2 replaceWithPattern \
    insertBeforeUsingCommaOp insertAfterUsingCommaOp deepCopy fixVariableReferences \
    buildJavaPackage createAbstractHandles moveDeclarationToInnermostScope

# list of test SAGE AST builders 
fixVariableReferences_SOURCES = fixVariableReferences.C 
annotateExpressionsWithUniqueNames_SOURCES = annotateExpressionsWithUniqueNames.C
deepDelete_SOURCES                       = deepDelete.C 
buildFunctionDeclaration_SOURCES         = buildFunctionDeclaration.C
buildNondefiningFunction_SOURCES         = buildNondefiningFunction.C
findMain_SOURCES                         = findMain.C
buildVariableDeclaration_SOURCES         = buildVariableDeclaration.C
buildAssignmentStmt_SOURCES              = buildAssignmentStmt.C
buildFunctionCalls_SOURCES               = buildFunctionCalls.C 
buildPragmaDeclaration_SOURCES           = buildPragmaDeclaration.C
attachComment_SOURCES                    = attachComment.C
insertHeader_SOURCES                     = insertHeader.C
buildExpression_SOURCES                  = buildExpression.C
buildStructDeclaration_SOURCES           = buildStructDeclaration.C
buildStructDeclaration2_SOURCES          = buildStructDeclaration2.C
lookupNamedType_SOURCES                  = lookupNamedType.C           
buildFile_SOURCES                        = buildFile.C           
movePreprocessingInfo_SOURCES            = movePreprocessingInfo.C
buildIfStmt_SOURCES                      = buildIfStmt.C
buildCpreprocessorDefineDeclaration_SOURCES = buildCpreprocessorDefineDeclaration.C
instrumentEndOfFunction_SOURCES          = instrumentEndOfFunction.C
isUpcSharedType_SOURCES                  = isUpcSharedType.C
isUpcPhaseLessSharedType_SOURCES         = isUpcPhaseLessSharedType.C
buildLabelStatement_SOURCES              = buildLabelStatement.C
buildSizeOfOp_SOURCES                    = buildSizeOfOp.C
abiStuffTest_SOURCES                     = abiStuffTest.C
abiStuffTestUPC_SOURCES                  = abiStuffTestUPC.C
buildNullStatement_SOURCES               = buildNullStatement.C
buildForStmt_SOURCES                     = buildForStmt.C
canonicalLoop_SOURCES                    = canonicalLoop.C
loopNormalization_SOURCES                = loopNormalization.C 
collectReadWriteVariables_SOURCES        = collectReadWriteVariables.C
typeTraits_SOURCES                       = typeTraits.C
livenessAnalysis_SOURCES                 = livenessAnalysis.C
buildProcedureHeaderStatement_SOURCES    = buildProcedureHeaderStatement.C
replaceMacroCalls_SOURCES                = replaceMacroCalls.C
buildAbstractHandle_SOURCES              = buildAbstractHandle.C
buildTypedefDeclaration_SOURCES          = buildTypedefDeclaration.C
loopUnrolling_SOURCES                    = loopUnrolling.C
getDependentDecls_SOURCES                = getDependentDecls.C
loopInterchange_SOURCES                  = loopInterchange.C
loopTiling_SOURCES                       = loopTiling.C
insertStatementBeforeFunction_SOURCES    = insertStatementBeforeFunction.C
removeStatementCommentRelocation_SOURCES = removeStatementCommentRelocation.C
generateUniqueName_SOURCES               = generateUniqueName.C
buildExternalStatement_SOURCES           = buildExternalStatement.C
buildCommonBlock_SOURCES                 = buildCommonBlock.C
doLoopNormalization_SOURCES              = doLoopNormalization.C
buildLabelStatement2_SOURCES	         = buildLabelStatement2.C
replaceWithPattern_SOURCES               = replaceWithPattern.C
insertBeforeUsingCommaOp_SOURCES         = insertBeforeUsingCommaOp.C
insertAfterUsingCommaOp_SOURCES          = insertAfterUsingCommaOp.C
deepCopy_SOURCES                         = deepCopy.C
buildJavaPackage_SOURCES                 = buildJavaPackage.C           
loopCollapsing_SOURCES                    = loopCollapsing.C
createAbstractHandles_SOURCES             = createAbstractHandles.C
moveDeclarationToInnermostScope_SOURCES   = moveDeclarationToInnermostScope.C
# libsageInterface.la is included in rose.la already?
LDADD =  $(ROSE_LIBS)

# Be sure to include proper suffixes, like .C
# TODO: handle side effects when deleting AST nodes
#failing_generatedCodeExamples = rose_inputdeepDelete.C 

generatedCodeExamples = \
  rose_inputfixVariableReferences.C \
  rose_inputBlank1.C \
  rose_inputBlank2.C \
  rose_inputfindMain.C \
  rose_inputbuildVariableDeclaration.C \
  rose_inputbuildAssignmentStmt.C \
  rose_inputbuildFunctionCalls.C \
  rose_inputbuildPragmaDeclaration.c \
  rose_inputAttachComment.C \
  rose_inputInsertHeader.C \
  rose_inputbuildExpression.C \
  rose_inputbuildStructDeclaration.C \
  rose_inputbuildStructDeclaration2.C \
  rose_inputLookupNamedType.C \
  rose_inputbuildFile.C \
  rose_inputMovePreprocessingInfo.C \
  rose_inputbuildIfStmt.C \
  rose_inputbuildCpreprocessorDefineDeclaration.C \
  rose_inputinstrumentEndOfFunction.C \
  rose_inputisUpcSharedType.upc \
  rose_inputisUpcPhaseLessSharedType.upc \
  rose_inputbuildLabelStatement.C \
  rose_inputbuildSizeOfOp.C \
  abiStuffTest.passed \
  abiStuffTestUPC.passed \
  rose_inputbuildNullStatement.C \
  rose_inputbuildForStmt.C \
  rose_inputcanonicalLoop.C \
  rose_inputcollectReadWriteVariables.C \
  rose_inputtypeTraits.C \
  rose_inputlivenessAnalysis.C \
  rose_inputreplaceMacroCalls.C \
  rose_inputbuildAbstractHandle.C \
  rose_inputbuildTypedefDeclaration.C \
  rose_inputloopUnrolling1.C \
  rose_inputloopUnrolling2.C \
  rose_inputgetDependentDecls.C \
  rose_inputloopInterchange.C \
  rose_inputloopTiling.C \
  rose_inputloopNormalization.C \
  deepDelete.passed \
  rose_inputinsertStatementBeforeFunction.C \
  rose_inputRemoveStatementCommentRelocation.C \
  rose_inputgenerateUniqueName.C \
  rose_inputannotateExpressionsWithUniqueNames.C \
  rose_inputreplaceWithPattern.C \
  rose_inputinsertBeforeUsingCommaOp.C \
  rose_inputinsertAfterUsingCommaOp.C \
  rose_inputdeepCopy.C\
  rose_inputloopCollapsing_1.C\
  rose_inputloopCollapsing_2.C\
  rose_inputloopCollapsing_3.C\
  rose_inputloopCollapsing_4.C\
  rose_inputloopCollapsing_5.C\
  buildJavaPackage.passed 

# section for declaration moving tool
#--------------------------------------------
MOVE_DECLS_TESTCODES_FULLY_SUPPORTED  = \
  inputmoveDeclarationToInnermostScope_1.C \
  inputmoveDeclarationToInnermostScope_2.C \
  inputmoveDeclarationToInnermostScope_3.C \
  inputmoveDeclarationToInnermostScope_4.C \
  inputmoveDeclarationToInnermostScope_5.C \
  inputmoveDeclarationToInnermostScope_6.C \
  inputmoveDeclarationToInnermostScope_7.C \
  inputmoveDeclarationToInnermostScope_8.C \
  inputmoveDeclarationToInnermostScope_9.C \
  inputmoveDeclarationToInnermostScope_10.C \
  inputmoveDeclarationToInnermostScope_11.C

MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED = 
 
MOVE_DECLS_TESTCODES_REQUIRED_TO_PASS = \
  $(MOVE_DECLS_TESTCODES_FULLY_SUPPORTED) \
  $(MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED)

MOVE_DECLS_TESTCODES_CURRENTLY_FAILING = 

MOVE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS = $(addprefix rose_, ${MOVE_DECLS_TESTCODES_FULLY_SUPPORTED})
MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED_RESULTS = $(addprefix rose_, ${MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED})

PASSING_MOVE_DECLS_TESTCODES_RESULTS = \
   $(MOVE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS) \
   $(MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED_RESULTS)

REFERENCE_PATH = $(top_srcdir)/tests/roseTests/astInterfaceTests/referenceResults
MOVE_DECLS_DIFF_FILES=$(PASSING_MOVE_DECLS_TESTCODES_RESULTS:.C=.C.diff)
DIFF=diff
$(MOVE_DECLS_DIFF_FILES):$(PASSING_MOVE_DECLS_TESTCODES_RESULTS)
	echo "Verifying moveDeclarationToInnermostScope translation by diff ..."; \
	if $(DIFF) $(@:.C.diff=.C) $(REFERENCE_PATH)/$(@:.C.diff=.C) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; rm -rf $@; xit 1; fi

test_move_decls: $(PASSING_MOVE_DECLS_TESTCODES_RESULTS)

move_decls_diff_check: $(MOVE_DECLS_DIFF_FILES)
# Liao 8/17/2010
#if USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  generatedCodeExamples += \
    rose_inputbuildProcedureHeaderStatement.f \
    rose_inputbuildExternalStatement.f \
    rose_inputbuildCommonBlock.f \
    rose_inputdoLoopNormalization.f \
    rose_inputbuildLabelStatement2.f
endif

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

# turn off edg warnings such as "variable x declared but never referenced" for test input files
TEST_CXXFLAGS = -rose:verbose 0 --edg:no_warnings -g3

# Many of these tests follow a very regular pattern and can be transformed into a testable command in the same way.  Some
# of these tests originally redirected their stdout and/or stderr to a particular file--we no longer need to do that since
# $(RTH_RUN) automatically does it for all the tests (look in the *.passed or *.failed files) 
group1 =						\
	rose_inputfixVariableReferences.C		\
	rose_inputfindMain.C				\
	rose_inputbuildVariableDeclaration.C		\
	rose_inputbuildAssignmentStmt.C			\
	rose_inputbuildIfStmt.C				\
	rose_inputbuildFile.C				\
	rose_inputbuildExpression.C			\
	rose_inputbuildStructDeclaration.C		\
	rose_inputbuildStructDeclaration2.C		\
	rose_inputbuildCpreprocessorDefineDeclaration.C	\
	rose_inputinstrumentEndOfFunction.C		\
	rose_inputbuildLabelStatement.C			\
	rose_inputbuildSizeOfOp.C			\
	rose_inputbuildNullStatement.C			\
	rose_inputbuildForStmt.C			\
	rose_inputcanonicalLoop.C			\
	rose_inputloopNormalization.C			\
	rose_inputcollectReadWriteVariables.C		\
	rose_inputtypeTraits.C				\
	rose_inputlivenessAnalysis.C			\
	rose_inputgenerateUniqueName.C			\
	rose_inputannotateExpressionsWithUniqueNames.C	\
	rose_inputinsertBeforeUsingCommaOp.C		\
	rose_inputinsertAfterUsingCommaOp.C		\
	rose_inputdeepCopy.C				\
	rose_inputbuildAbstractHandle.C			\
	rose_inputbuildTypedefDeclaration.C		\
	rose_inputgetDependentDecls.C			\
	rose_inputreplaceWithPattern.C                  \
	rose_inputcreateAbstractHandles.C

$(group1): rose_input%.C: input%.C %
	@$(RTH_RUN) \
		EXE="$$(pwd)/$*$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS)" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed

# deepDelete apparently doesn't actually create an output file
deepDelete.passed: inputdeepDelete.C deepDelete
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/deepDelete$(EXEEXT) $(TEST_CXXFLAGS) -rose:detect_dangling_pointers 1 -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

# Like group1, except that EXE doesn't follow the pattern
rose_inputBlank1.C: inputBlank1.C buildFunctionDeclaration
	@$(RTH_RUN) \
		EXE="$$(pwd)/buildFunctionDeclaration$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS)" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed

# Like group1, except that EXE doesn't follow the pattern
rose_inputBlank2.C: inputBlank2.C buildNondefiningFunction
	@$(RTH_RUN) \
		EXE="$$(pwd)/buildNondefiningFunction$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS)" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed

# Like group1, except FLAGS includes an extra -I
rose_inputbuildFunctionCalls.C: inputbuildFunctionCalls.C buildFunctionCalls
	@$(RTH_RUN) \
		EXE="$$(pwd)/buildFunctionCalls$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -I$(abs_srcdir)" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed

# Like group1, except this is a C, not C++ file (not sure why we're passing CXXFLAGS instead of CFLAGS though)
rose_inputbuildPragmaDeclaration.c: inputbuildPragmaDeclaration.c buildPragmaDeclaration
	@$(RTH_RUN) \
		EXE="$$(pwd)/buildPragmaDeclaration$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS)" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed

# Like group1, except capitalization of the target is different
rose_inputAttachComment.C: inputAttachComment.C attachComment
	@$(RTH_RUN) \
		EXE="$$(pwd)/attachComment$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS)" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed
rose_inputInsertHeader.C: inputInsertHeader.C insertHeader
	@$(RTH_RUN) \
		EXE="$$(pwd)/insertHeader$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS)" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed
rose_inputLookupNamedType.C: inputLookupNamedType.C lookupNamedType
	@$(RTH_RUN) \
		EXE="$$(pwd)/lookupNamedType$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS)" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed
rose_inputMovePreprocessingInfo.C: inputMovePreprocessingInfo.C movePreprocessingInfo
	@$(RTH_RUN) \
		EXE="$$(pwd)/movePreprocessingInfo$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS)" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed

# Like group1, except for target extension and extra FLAGS
rose_inputisUpcSharedType.upc: inputisUpcSharedType.upc isUpcSharedType
	@$(RTH_RUN) \
		EXE="$$(pwd)/isUpcSharedType$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:skipfinalCompileStep -rose:upc_threads 1" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed
rose_inputisUpcPhaseLessSharedType.upc: inputisUpcPhaseLessSharedType.upc isUpcPhaseLessSharedType
	@$(RTH_RUN) \
		EXE="$$(pwd)/isUpcPhaseLessSharedType$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:skipfinalCompileStep -rose:upc_threads 1" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed

# Doesn't produce a C file
abiStuffTest.passed: inputAbiStuffTest.c abiStuffTest
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/abiStuffTest$(EXEEXT) $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

# Doesn't produce a C file
abiStuffTestUPC.passed: inputAbiStuffTestUPC.upc abiStuffTestUPC
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/abiStuffTestUPC$(EXEEXT) $(TEST_CXXFLAGS) -rose:upc_threads 1 -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@


if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

# DQ (9/13/2010): I think that we need both levels of if tests else --without-java will not work.
# DQ (3/18/2009): Make this dependent upon if ROSE was configured to use Fortran.
# if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT 
# if ROSE_USE_OPEN_FORTRAN_PARSER

group2 =						\
	rose_inputbuildProcedureHeaderStatement.f	\
	rose_inputbuildExternalStatement.f		\
	rose_inputbuildCommonBlock.f			\
	rose_inputdoLoopNormalization.f			\
	rose_inputbuildLabelStatement2.f

$(group2): rose_input%.f: input%.f %
	@$(RTH_RUN) \
		EXE="$$(pwd)/$*$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS)" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed
endif


if ROSE_USE_INDENT
rose_inputloopUnrolling.C: inputloopUnrolling.C loopUnrolling
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopUnrolling$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopunroll:abstract_handle 'Statement<position,5>' -rose:loopunroll:factor 3" \
		INPUT=$(abspath $<) \
		OUTPUT="-rose:output $@" \
		INDENT=yes \
		$(srcdir)/astInterface.conf $@.passed
rose_inputloopUnrolling1.C: inputloopUnrolling.C loopUnrolling
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopUnrolling$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopunroll:abstract_handle 'Statement<position,6>' -rose:loopunroll:factor 5" \
		INPUT=$(abspath $<) \
		OUTPUT="-rose:output $@" \
		INDENT=yes \
		$(srcdir)/astInterface.conf $@.passed
rose_inputloopUnrolling2.C: inputloopUnrolling.C loopUnrolling
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopUnrolling$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopunroll:abstract_handle 'Statement<position,6>' -rose:loopunroll:factor 3" \
		INPUT=$(abspath $<) \
		OUTPUT="-rose:output $@" \
		INDENT=yes \
		$(srcdir)/astInterface.conf $@.passed
else
rose_inputloopUnrolling.C: inputloopUnrolling.C loopUnrolling
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopUnrolling$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopunroll:abstract_handle 'Statement<position,5>' -rose:loopunroll:factor 3" \
		INPUT=$(abspath $<) \
		OUTPUT="-rose:output $@" \
		INDENT=no \
		$(srcdir)/astInterface.conf $@.passed
rose_inputloopUnrolling1.C: inputloopUnrolling.C loopUnrolling
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopUnrolling$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopunroll:abstract_handle 'Statement<position,6>' -rose:loopunroll:factor 5" \
		INPUT=$(abspath $<) \
		OUTPUT="-rose:output $@" \
		INDENT=no \
		$(srcdir)/astInterface.conf $@.passed
rose_inputloopUnrolling2.C: inputloopUnrolling.C loopUnrolling
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopUnrolling$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopunroll:abstract_handle 'Statement<position,6>' -rose:loopunroll:factor 3" \
		INPUT=$(abspath $<) \
		OUTPUT="-rose:output $@" \
		INDENT=no \
		$(srcdir)/astInterface.conf $@.passed
endif

if ROSE_USE_INDENT

rose_inputloopCollapsing_1.C: inputloopCollapsing_1.C loopCollapsing
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopCollapsing$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopcollapse:abstract_handle 'Statement<position,5>' -rose:loopcollapse:factor 2" \
		INPUT=$(abspath $<) \
		INDENT=yes \
		$(srcdir)/astInterface.conf $@.passed
rose_inputloopCollapsing_2.C: inputloopCollapsing_2.C loopCollapsing
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopCollapsing$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopcollapse:abstract_handle 'Statement<position,9>' -rose:loopcollapse:factor 2" \
		INPUT=$(abspath $<) \
		INDENT=yes \
		$(srcdir)/astInterface.conf $@.passed
rose_inputloopCollapsing_3.C: inputloopCollapsing_3.C loopCollapsing
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopCollapsing$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopcollapse:abstract_handle 'Statement<position,9>' -rose:loopcollapse:factor 3" \
		INPUT=$(abspath $<) \
		INDENT=yes \
		$(srcdir)/astInterface.conf $@.passed
rose_inputloopCollapsing_4.C: inputloopCollapsing_4.C loopCollapsing
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopCollapsing$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopcollapse:abstract_handle 'Statement<position,9>' -rose:loopcollapse:factor 4" \
		INPUT=$(abspath $<) \
		INDENT=yes \
		$(srcdir)/astInterface.conf $@.passed
rose_inputloopCollapsing_5.C: inputloopCollapsing_5.C loopCollapsing
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopCollapsing$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopcollapse:abstract_handle 'Statement<position,9>' -rose:loopcollapse:factor 5" \
		INPUT=$(abspath $<) \
		INDENT=yes \
		$(srcdir)/astInterface.conf $@.passed
else

rose_inputloopCollapsing_1.C: inputloopCollapsing_1.C loopCollapsing
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopCollapsing$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopcollapse:abstract_handle 'Statement<position,5>' -rose:loopcollapse:factor 2" \
		INPUT=$(abspath $<) \
		INDENT=no \
		$(srcdir)/astInterface.conf $@.passed
rose_inputloopCollapsing_2.C: inputloopCollapsing_2.C loopCollapsing
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopCollapsing$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopcollapse:abstract_handle 'Statement<position,9>' -rose:loopcollapse:factor 2" \
		INPUT=$(abspath $<) \
		INDENT=no \
		$(srcdir)/astInterface.conf $@.passed
rose_inputloopCollapsing_3.C: inputloopCollapsing_3.C loopCollapsing
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopCollapsing$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopcollapse:abstract_handle 'Statement<position,9>' -rose:loopcollapse:factor 3" \
		INPUT=$(abspath $<) \
		INDENT=no \
		$(srcdir)/astInterface.conf $@.passed
rose_inputloopCollapsing_4.C: inputloopCollapsing_4.C loopCollapsing
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopCollapsing$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopcollapse:abstract_handle 'Statement<position,9>' -rose:loopcollapse:factor 4" \
		INPUT=$(abspath $<) \
		INDENT=no \
		$(srcdir)/astInterface.conf $@.passed
rose_inputloopCollapsing_5.C: inputloopCollapsing_5.C loopCollapsing
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopCollapsing$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopcollapse:abstract_handle 'Statement<position,9>' -rose:loopcollapse:factor 5" \
		INPUT=$(abspath $<) \
		INDENT=no \
		$(srcdir)/astInterface.conf $@.passed
endif

# rules for declaration moving tests
$(MOVE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS): $(srcdir)/$(@:rose_%=%) moveDeclarationToInnermostScope 
		./moveDeclarationToInnermostScope $(TEST_CXXFLAGS) -c -rose:debug $(srcdir)/$(@:rose_%=%)

$(MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED_RESULTS): $(srcdir)/$(@:rose_%=%) moveDeclarationToInnermostScope 
		./moveDeclarationToInnermostScope $(TEST_CXXFLAGS) -c -rose:debug -rose:keep_going $(srcdir)/$(@:rose_%=%)

#rose_inputmoveDeclarationToInnermostScope_1.C: inputmoveDeclarationToInnermostScope_1.C moveDeclarationToInnermostScope
#	@$(RTH_RUN) \
#		EXE="$$(pwd)/moveDeclarationToInnermostScope$(EXEEXT)" \
#		FLAGS="$(TEST_CXXFLAGS) -rose:debug"  \
#		INPUT=$(abspath $<) \
#		$(srcdir)/astInterface.conf $@.passed
#
#rose_inputmoveDeclarationToInnermostScope_2.C: inputmoveDeclarationToInnermostScope_2.C moveDeclarationToInnermostScope
#	@$(RTH_RUN) \
#		EXE="$$(pwd)/moveDeclarationToInnermostScope$(EXEEXT)" \
#		FLAGS="$(TEST_CXXFLAGS) -rose:debug" \
#		INPUT=$(abspath $<) \
#		$(srcdir)/astInterface.conf $@.passed
#
#rose_inputmoveDeclarationToInnermostScope_3.C: inputmoveDeclarationToInnermostScope_3.C moveDeclarationToInnermostScope
#	@$(RTH_RUN) \
#		EXE="$$(pwd)/moveDeclarationToInnermostScope$(EXEEXT)" \
#		FLAGS="$(TEST_CXXFLAGS) -rose:debug" \
#		INPUT=$(abspath $<) \
#		$(srcdir)/astInterface.conf $@.passed
#
#rose_inputmoveDeclarationToInnermostScope_4.C: inputmoveDeclarationToInnermostScope_4.C moveDeclarationToInnermostScope
#	@$(RTH_RUN) \
#		EXE="$$(pwd)/moveDeclarationToInnermostScope$(EXEEXT)" \
#		FLAGS="$(TEST_CXXFLAGS) -rose:debug" \
#		INPUT=$(abspath $<) \
#		$(srcdir)/astInterface.conf $@.passed
#
#rose_inputmoveDeclarationToInnermostScope_5.C: inputmoveDeclarationToInnermostScope_5.C moveDeclarationToInnermostScope
#	@$(RTH_RUN) \
#		EXE="$$(pwd)/moveDeclarationToInnermostScope$(EXEEXT)" \
#		FLAGS="$(TEST_CXXFLAGS) -rose:debug" \
#		INPUT=$(abspath $<) \
#		$(srcdir)/astInterface.conf $@.passed
#
#rose_inputmoveDeclarationToInnermostScope_6.C: inputmoveDeclarationToInnermostScope_6.C moveDeclarationToInnermostScope
#	@$(RTH_RUN) \
#		EXE="$$(pwd)/moveDeclarationToInnermostScope$(EXEEXT)" \
#		FLAGS="$(TEST_CXXFLAGS) -rose:debug -rose:aggressive" \
#		INPUT=$(abspath $<) \
#		$(srcdir)/astInterface.conf $@.passed
#
#rose_inputmoveDeclarationToInnermostScope_7.C: inputmoveDeclarationToInnermostScope_7.C moveDeclarationToInnermostScope
#	@$(RTH_RUN) \
#		EXE="$$(pwd)/moveDeclarationToInnermostScope$(EXEEXT)" \
#		FLAGS="$(TEST_CXXFLAGS) -rose:debug" \
#		INPUT=$(abspath $<) \
#		$(srcdir)/astInterface.conf $@.passed
#
#rose_inputmoveDeclarationToInnermostScope_8.C: inputmoveDeclarationToInnermostScope_8.C moveDeclarationToInnermostScope
#	@$(RTH_RUN) \
#		EXE="$$(pwd)/moveDeclarationToInnermostScope$(EXEEXT)" \
#		FLAGS="$(TEST_CXXFLAGS) -rose:debug" \
#		INPUT=$(abspath $<) \
#		$(srcdir)/astInterface.conf $@.passed
#
# test keep going first
#rose_inputmoveDeclarationToInnermostScope_9.C: inputmoveDeclarationToInnermostScope_9.C moveDeclarationToInnermostScope
#	@$(RTH_RUN) \
#		EXE="$$(pwd)/moveDeclarationToInnermostScope$(EXEEXT)" \
#		FLAGS="$(TEST_CXXFLAGS) -rose:debug -rose:keep_going" \
#		INPUT=$(abspath $<) \
#		$(srcdir)/astInterface.conf $@.passed
#
#rose_inputmoveDeclarationToInnermostScope_10.C: inputmoveDeclarationToInnermostScope_10.C moveDeclarationToInnermostScope
#	@$(RTH_RUN) \
#		EXE="$$(pwd)/moveDeclarationToInnermostScope$(EXEEXT)" \
#		FLAGS="$(TEST_CXXFLAGS) -rose:debug -rose:keep_going" \
#		INPUT=$(abspath $<) \
#		$(srcdir)/astInterface.conf $@.passed
#



# interchange the first loop within the input file, interchange depth is 4, lexicographical order is 1.
rose_inputloopInterchange.C: inputloopInterchange.C loopInterchange
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopInterchange$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:loopInterchange:abstract_handle 'ForStatement<numbering,1>' -rose:loopInterchange:depth 4 -rose:loopInterchange:order 1" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed

# Tile the first loop within the input file at depth 3 , tile size is 5
rose_inputloopTiling.C: inputloopTiling.C loopTiling
	@$(RTH_RUN) \
		EXE="$$(pwd)/loopTiling$(EXEEXT) -rose:loopTiling:abstract_handle 'ForStatement<numbering,1>' -rose:loopTiling:depth 3 -rose:loopTiling:tilesize 5" \
		FLAGS="$(TEST_CXXFLAGS)" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed

rose_inputreplaceMacroCalls.C: inputreplaceMacroCalls.C replaceMacroCalls
	@$(RTH_RUN) \
		DISABLED="boost:wave causes seg fault on 64-bit machines" \
		EXE="$$(pwd)/replaceMacroCalls$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -rose:wave" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed

# DQ (9/16/2010): Added this test code to demonstrate problem with adding statments around statments with comments or
# CPP directives attached.
rose_inputinsertStatementBeforeFunction.C: inputinsertStatementBeforeFunction.C insertStatementBeforeFunction $(srcdir)/inputinsertStatementBeforeFunction_1.h
	@$(RTH_RUN) \
		EXE="$$(pwd)/insertStatementBeforeFunction$(EXEEXT)" \
		FLAGS="$(TEST_CXXFLAGS) -I$(abs_srcdir)" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed

# There are a lot of different cases that we now test for with the relocation of comments support in the
# removeStatement() function.
group3 =											\
	rose_inputRemoveStatementCommentRelocation.C						\
	rose_inputRemoveStatementCommentRelocation_withTopBoundingStatement.C			\
	rose_inputRemoveStatementCommentRelocation_withoutTopBoundingStatement.C		\
	rose_inputRemoveStatementCommentRelocation_withBottomBoundingStatement.C		\
	rose_inputRemoveStatementCommentRelocation_withoutBottomBoundingStatement.C		\
	rose_inputRemoveStatementCommentRelocation_withTopandBottomBoundingStatement.C		\
	rose_inputRemoveStatementCommentRelocation_withoutTopandBottomBoundingStatement.C

group3_headers = 						\
	$(srcdir)/inputRemoveStatementCommentRelocation_1.h	\
	$(srcdir)/inputRemoveStatementCommentRelocation_2.h

$(group3): rose_%: % $(group3_headers)
	@$(RTH_RUN) \
		EXE="$$(pwd)/removeStatementCommentRelocation$(EXEEXT)" \
		FLAGS="$(TEST_CXX_FLAGS) -I$(abs_srcdir)" \
		INPUT=$(abspath $<) \
		$(srcdir)/astInterface.conf $@.passed


# Doesn't produce any output source files
buildJavaPackage.passed: inputbuildJavaPackage.C buildJavaPackage
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/buildJavaPackage$(EXEEXT) $(TEST_CXXFLAGS) -I$(abs_srcdir) -c $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

# MUST keep this up-to-date!!
EXTRA_DIST = astInterface.conf inputBlank1.C inputBlank2.C inputfindMain.C inputbuildVariableDeclaration.C	\
       inputbuildAssignmentStmt.C inputbuildFunctionCalls.C inputbuildFunctionCalls.h				\
       inputbuildPragmaDeclaration.c inputAttachComment.C inputInsertHeader.C					\
       inputbuildExpression.C inputbuildStructDeclaration.C inputLookupNamedType.C				\
       inputbuildFile.C inputMovePreprocessingInfo.C inputbuildIfStmt.C						\
       inputbuildCpreprocessorDefineDeclaration.C inputinstrumentEndOfFunction.C				\
       inputisUpcSharedType.upc inputisUpcPhaseLessSharedType.upc inputbuildLabelStatement.C			\
       inputbuildSizeOfOp.C inputAbiStuffTest.c inputAbiStuffTestUPC.upc					\
       inputbuildNullStatement.C inputbuildForStmt.C inputcanonicalLoop.C					\
       inputloopNormalization.C inputcollectReadWriteVariables.C inputtypeTraits.C				\
       inputlivenessAnalysis.C inputbuildProcedureHeaderStatement.f inputreplaceMacroCalls.C			\
       inputbuildAbstractHandle.C inputloopUnrolling.C inputgetDependentDecls.C					\
       inputloopInterchange.C inputloopTiling.C inputbuildStructDeclaration2.C					\
       inputbuildTypedefDeclaration.C inputdeepDelete.C								\
       inputinsertStatementBeforeFunction.C inputinsertStatementBeforeFunction_1.h				\
       inputinsertStatementBeforeFunction_2.h inputRemoveStatementCommentRelocation.C				\
       inputRemoveStatementCommentRelocation_1.h inputRemoveStatementCommentRelocation_2.h			\
       inputRemoveStatementCommentRelocation_withBottomBoundingStatement.C					\
       inputRemoveStatementCommentRelocation_withTopBoundingStatement.C						\
       inputRemoveStatementCommentRelocation_withTopandBottomBoundingStatement.C				\
       inputRemoveStatementCommentRelocation_withoutBottomBoundingStatement.C					\
       inputRemoveStatementCommentRelocation_withoutTopBoundingStatement.C					\
       inputRemoveStatementCommentRelocation_withoutTopandBottomBoundingStatement.C				\
       inputgenerateUniqueName.C inputannotateExpressionsWithUniqueNames.C					\
       inputbuildExternalStatement.f inputbuildCommonBlock.f inputdoLoopNormalization.f				\
       inputbuildLabelStatement2.f inputreplaceWithPattern.C inputinsertBeforeUsingCommaOp.C			\
       inputinsertAfterUsingCommaOp.C inputdeepCopy.C inputfixVariableReferences.C  inputcreateAbstractHandles.C \
       inputloopCollapsing_2.C  inputloopCollapsing_3.C  inputloopCollapsing_4.C  inputloopCollapsing_5.C \
       inputbuildJavaPackage.C inputloopCollapsing_1.C


#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(generatedCodeExamples) move_decls_diff_check

clean-local:
	rm -f $(generatedCodeExamples) *.dot $(PASSING_MOVE_DECLS_TESTCODES_RESULTS)
	rm -f $(addsuffix .passed, $(generatedCodeExamples))
	rm -f $(addsuffix .failed, $(generatedCodeExamples))
	rm -f $(addsuffix .passed, $(PASSING_MOVE_DECLS_TESTCODES_RESULTS))
	rm -f $(addsuffix .failed, $(PASSING_MOVE_DECLS_TESTCODES_RESULTS))
	rm -f $(addsuffix .diff, $(PASSING_MOVE_DECLS_TESTCODES_RESULTS))
