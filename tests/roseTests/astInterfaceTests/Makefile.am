# Liao, Jan. 9, 2008
# Makefile.am for a set of test AST builders and test input code
#-------------------------------------------------------------
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# JP (10/3/14): Added the unit tests subdirectory to be built.
SUBDIR = typeEquivalenceTests unitTests 

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

AM_CPPFLAGS =  \
        $(ROSE_INCLUDES) \
        -I$(top_srcdir)/src/frontend/SageIII/sageInterface
# We install a few test programs since they might be useful for external users
bin_PROGRAMS = loopInterchange loopUnrolling loopTiling loopCollapsingDirective loopCollapsing moveDeclarationToInnermostScope

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
    buildJavaPackage createAbstractHandles moveDeclarationToInnermostScope buildStatementFromString

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

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
loopCollapsingDirective_SOURCES           = loopCollapsingDirective.C
createAbstractHandles_SOURCES             = createAbstractHandles.C
moveDeclarationToInnermostScope_SOURCES   = moveDeclarationToInnermostScope.C
buildStatementFromString_SOURCES          = buildStatementFromString.C
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
  rose_inputbuildStatementFromString.C \
  buildJavaPackage.passed 

# section for declaration moving tool
#  inputmoveDeclarationToInnermostScope_23.C 
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
  inputmoveDeclarationToInnermostScope_11.C \
  inputmoveDeclarationToInnermostScope_12.C \
  inputmoveDeclarationToInnermostScope_13.C \
  inputmoveDeclarationToInnermostScope_14.C \
  inputmoveDeclarationToInnermostScope_15.C  \
  inputmoveDeclarationToInnermostScope_16.C \
  inputmoveDeclarationToInnermostScope_18.C \
  inputmoveDeclarationToInnermostScope_19.C \
  inputmoveDeclarationToInnermostScope_20.C \
  inputmoveDeclarationToInnermostScope_21.C \
  inputmoveDeclarationToInnermostScope_22.C \
  inputmoveDeclarationToInnermostScope_23.C \
  inputmoveDeclarationToInnermostScope_24.C \
  inputmoveDeclarationToInnermostScope_25.C \
  inputmoveDeclarationToInnermostScope_26.C \
  inputmoveDeclarationToInnermostScope_27.C \
  inputmoveDeclarationToInnermostScope_test2015_126.C \
  inputmoveDeclarationToInnermostScope_test2015_127.C 

# DQ (12/23/2014): This fails because the normal unparsing (without token unparsing) is failing.
# inputmoveDeclarationToInnermostScope_test2014_26.C

MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED = 

# identity translator cannot handle them properly  
# 17: shared SgArrayType issue
MOVE_DECLS_TESTCODES_FAILING = \
  inputmoveDeclarationToInnermostScope_17.C \
  inputmoveDeclarationToInnermostScope_test2014_18.C \
  inputmoveDeclarationToInnermostScope_test2014_22.C

MOVE_DECLS_TESTCODES_REQUIRED_TO_PASS = \
  $(MOVE_DECLS_TESTCODES_FULLY_SUPPORTED) \
  $(MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED)

MOVE_DECLS_TESTCODES_CURRENTLY_FAILING = 

MOVE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS = $(addprefix rose_, ${MOVE_DECLS_TESTCODES_FULLY_SUPPORTED})
MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED_RESULTS = $(addprefix rose_, ${MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED})

MOVE_MERGE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS = $(addprefix rose_v2_, ${MOVE_DECLS_TESTCODES_FULLY_SUPPORTED})
MOVE_MERGE_DECLS_TESTCODES_PARTIALLY_SUPPORTED_RESULTS = $(addprefix rose_v2_, ${MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED})


PASSING_MOVE_DECLS_TESTCODES_RESULTS = \
   $(MOVE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS) \
   $(MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED_RESULTS)

# DQ (12/23/2014): Added tests specific to use of move tool with token-based unparsing.
MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_FULLY_SUPPORTED  = \
  inputmoveDeclarationToInnermostScope_1.C \
  inputmoveDeclarationToInnermostScope_2.C \
  inputmoveDeclarationToInnermostScope_3.C \
  inputmoveDeclarationToInnermostScope_4.C \
  inputmoveDeclarationToInnermostScope_6.C \
  inputmoveDeclarationToInnermostScope_7.C \
  inputmoveDeclarationToInnermostScope_8.C \
  inputmoveDeclarationToInnermostScope_9.C \
  inputmoveDeclarationToInnermostScope_10.C \
  inputmoveDeclarationToInnermostScope_11.C \
  inputmoveDeclarationToInnermostScope_12.C \
  inputmoveDeclarationToInnermostScope_13.C \
  inputmoveDeclarationToInnermostScope_14.C \
  inputmoveDeclarationToInnermostScope_15.C \
  inputmoveDeclarationToInnermostScope_16.C \
  inputmoveDeclarationToInnermostScope_18.C \
  inputmoveDeclarationToInnermostScope_test2014_01.C \
  inputmoveDeclarationToInnermostScope_test2014_02.C \
  inputmoveDeclarationToInnermostScope_test2014_03.C \
  inputmoveDeclarationToInnermostScope_test2014_04.C \
  inputmoveDeclarationToInnermostScope_test2014_05.C \
  inputmoveDeclarationToInnermostScope_test2014_06.C \
  inputmoveDeclarationToInnermostScope_test2014_07.C \
  inputmoveDeclarationToInnermostScope_test2014_08.C \
  inputmoveDeclarationToInnermostScope_test2014_10.C \
  inputmoveDeclarationToInnermostScope_test2014_11.C \
  inputmoveDeclarationToInnermostScope_test2014_13.C \
  inputmoveDeclarationToInnermostScope_test2014_14.C \
  inputmoveDeclarationToInnermostScope_test2014_15.C \
  inputmoveDeclarationToInnermostScope_test2014_16.C \
  inputmoveDeclarationToInnermostScope_test2014_17.C \
  inputmoveDeclarationToInnermostScope_test2014_19.C \
  inputmoveDeclarationToInnermostScope_test2014_20.C \
  inputmoveDeclarationToInnermostScope_test2014_21.C \
  inputmoveDeclarationToInnermostScope_test2014_23.C \
  inputmoveDeclarationToInnermostScope_test2014_24.C \
  inputmoveDeclarationToInnermostScope_test2014_25.C \
  inputmoveDeclarationToInnermostScope_test2014_26.C \
  inputmoveDeclarationToInnermostScope_test2014_27.C \
  inputmoveDeclarationToInnermostScope_test2014_28.C \
  inputmoveDeclarationToInnermostScope_test2014_29.C \
  inputmoveDeclarationToInnermostScope_test2014_30.C \
  inputmoveDeclarationToInnermostScope_test2014_31.C \
  inputmoveDeclarationToInnermostScope_test2015_01.C \
  inputmoveDeclarationToInnermostScope_test2015_02.C \
  inputmoveDeclarationToInnermostScope_test2015_03.C \
  inputmoveDeclarationToInnermostScope_test2015_04.C \
  inputmoveDeclarationToInnermostScope_test2015_05.C \
  inputmoveDeclarationToInnermostScope_test2015_06.C \
  inputmoveDeclarationToInnermostScope_test2015_07.C \
  inputmoveDeclarationToInnermostScope_test2015_08.C \
  inputmoveDeclarationToInnermostScope_test2015_09.C \
  inputmoveDeclarationToInnermostScope_test2015_10.C \
  inputmoveDeclarationToInnermostScope_test2015_11.C \
  inputmoveDeclarationToInnermostScope_test2015_12.C \
  inputmoveDeclarationToInnermostScope_test2015_13.C \
  inputmoveDeclarationToInnermostScope_test2015_14.C \
  inputmoveDeclarationToInnermostScope_test2015_15.C \
  inputmoveDeclarationToInnermostScope_test2015_16.C \
  inputmoveDeclarationToInnermostScope_test2015_17.C \
  inputmoveDeclarationToInnermostScope_test2015_18.C \
  inputmoveDeclarationToInnermostScope_test2015_19.C \
  inputmoveDeclarationToInnermostScope_test2015_20.C \
  inputmoveDeclarationToInnermostScope_test2015_21.C \
  inputmoveDeclarationToInnermostScope_test2015_22.C \
  inputmoveDeclarationToInnermostScope_test2015_23.C \
  inputmoveDeclarationToInnermostScope_test2015_24.C \
  inputmoveDeclarationToInnermostScope_test2015_25.C \
  inputmoveDeclarationToInnermostScope_test2015_26.C \
  inputmoveDeclarationToInnermostScope_test2015_27.C \
  inputmoveDeclarationToInnermostScope_test2015_28.C \
  inputmoveDeclarationToInnermostScope_test2015_29.C \
  inputmoveDeclarationToInnermostScope_test2015_30.C \
  inputmoveDeclarationToInnermostScope_test2015_31.C \
  inputmoveDeclarationToInnermostScope_test2015_32.C \
  inputmoveDeclarationToInnermostScope_test2015_33.C \
  inputmoveDeclarationToInnermostScope_test2015_34.C \
  inputmoveDeclarationToInnermostScope_test2015_35.C \
  inputmoveDeclarationToInnermostScope_test2015_36.C \
  inputmoveDeclarationToInnermostScope_test2015_37.C \
  inputmoveDeclarationToInnermostScope_test2015_38.C \
  inputmoveDeclarationToInnermostScope_test2015_39.C \
  inputmoveDeclarationToInnermostScope_test2015_40.C \
  inputmoveDeclarationToInnermostScope_test2015_41.C \
  inputmoveDeclarationToInnermostScope_test2015_42.C \
  inputmoveDeclarationToInnermostScope_test2015_43.C \
  inputmoveDeclarationToInnermostScope_test2015_44.C \
  inputmoveDeclarationToInnermostScope_test2015_45.C \
  inputmoveDeclarationToInnermostScope_test2015_46.C \
  inputmoveDeclarationToInnermostScope_test2015_47.C \
  inputmoveDeclarationToInnermostScope_test2015_48.C \
  inputmoveDeclarationToInnermostScope_test2015_49.C \
  inputmoveDeclarationToInnermostScope_test2015_50.C \
  inputmoveDeclarationToInnermostScope_test2015_51.C \
  inputmoveDeclarationToInnermostScope_test2015_52.C \
  inputmoveDeclarationToInnermostScope_test2015_53.C \
  inputmoveDeclarationToInnermostScope_test2015_54.C \
  inputmoveDeclarationToInnermostScope_test2015_55.C \
  inputmoveDeclarationToInnermostScope_test2015_56.C \
  inputmoveDeclarationToInnermostScope_test2015_57.C \
  inputmoveDeclarationToInnermostScope_test2015_58.C \
  inputmoveDeclarationToInnermostScope_test2015_59.C \
  inputmoveDeclarationToInnermostScope_test2015_60.C \
  inputmoveDeclarationToInnermostScope_test2015_61.C \
  inputmoveDeclarationToInnermostScope_test2015_62.C \
  inputmoveDeclarationToInnermostScope_test2015_63.C \
  inputmoveDeclarationToInnermostScope_test2015_64.C \
  inputmoveDeclarationToInnermostScope_test2015_65.C \
  inputmoveDeclarationToInnermostScope_test2015_66.C \
  inputmoveDeclarationToInnermostScope_test2015_67.C \
  inputmoveDeclarationToInnermostScope_test2015_68.C \
  inputmoveDeclarationToInnermostScope_test2015_69.C \
  inputmoveDeclarationToInnermostScope_test2015_70.C \
  inputmoveDeclarationToInnermostScope_test2015_71.C \
  inputmoveDeclarationToInnermostScope_test2015_72.C \
  inputmoveDeclarationToInnermostScope_test2015_73.C \
  inputmoveDeclarationToInnermostScope_test2015_74.C \
  inputmoveDeclarationToInnermostScope_test2015_75.C \
  inputmoveDeclarationToInnermostScope_test2015_76.C \
  inputmoveDeclarationToInnermostScope_test2015_77.C \
  inputmoveDeclarationToInnermostScope_test2015_78.C \
  inputmoveDeclarationToInnermostScope_test2015_79.C \
  inputmoveDeclarationToInnermostScope_test2015_80.C \
  inputmoveDeclarationToInnermostScope_test2015_81.C \
  inputmoveDeclarationToInnermostScope_test2015_82.C \
  inputmoveDeclarationToInnermostScope_test2015_83.C \
  inputmoveDeclarationToInnermostScope_test2015_84.C \
  inputmoveDeclarationToInnermostScope_test2015_85.C \
  inputmoveDeclarationToInnermostScope_test2015_86.C \
  inputmoveDeclarationToInnermostScope_test2015_87.C \
  inputmoveDeclarationToInnermostScope_test2015_88.C \
  inputmoveDeclarationToInnermostScope_test2015_89.C \
  inputmoveDeclarationToInnermostScope_test2015_90.C \
  inputmoveDeclarationToInnermostScope_test2015_91.C \
  inputmoveDeclarationToInnermostScope_test2015_92.C \
  inputmoveDeclarationToInnermostScope_test2015_93.C \
  inputmoveDeclarationToInnermostScope_test2015_94.C \
  inputmoveDeclarationToInnermostScope_test2015_95.C \
  inputmoveDeclarationToInnermostScope_test2015_96.C \
  inputmoveDeclarationToInnermostScope_test2015_97.C \
  inputmoveDeclarationToInnermostScope_test2015_98.C \
  inputmoveDeclarationToInnermostScope_test2015_99.C \
  inputmoveDeclarationToInnermostScope_test2015_102.C \
  inputmoveDeclarationToInnermostScope_test2015_103.C \
  inputmoveDeclarationToInnermostScope_test2015_104.C \
  inputmoveDeclarationToInnermostScope_test2015_105.C \
  inputmoveDeclarationToInnermostScope_test2015_106.C \
  inputmoveDeclarationToInnermostScope_test2015_107.C \
  inputmoveDeclarationToInnermostScope_test2015_108.C \
  inputmoveDeclarationToInnermostScope_test2015_109.C \
  inputmoveDeclarationToInnermostScope_test2015_110.C \
  inputmoveDeclarationToInnermostScope_test2015_111.C \
  inputmoveDeclarationToInnermostScope_test2015_112.C \
  inputmoveDeclarationToInnermostScope_test2015_113.C \
  inputmoveDeclarationToInnermostScope_test2015_115.C \
  inputmoveDeclarationToInnermostScope_test2015_116.C \
  inputmoveDeclarationToInnermostScope_test2015_117.C \
  inputmoveDeclarationToInnermostScope_test2015_118.C \
  inputmoveDeclarationToInnermostScope_test2015_119.C \
  inputmoveDeclarationToInnermostScope_test2015_120.C \
  inputmoveDeclarationToInnermostScope_test2015_121.C \
  inputmoveDeclarationToInnermostScope_test2015_122.C \
  inputmoveDeclarationToInnermostScope_test2015_123.C \
  inputmoveDeclarationToInnermostScope_test2015_124.C \
  inputmoveDeclarationToInnermostScope_test2015_125.C \
  inputmoveDeclarationToInnermostScope_test2015_126.C \
  inputmoveDeclarationToInnermostScope_test2015_127.C \
  inputmoveDeclarationToInnermostScope_test2015_128.C \
  inputmoveDeclarationToInnermostScope_test2015_129.C \
  inputmoveDeclarationToInnermostScope_test2015_130.C \
  inputmoveDeclarationToInnermostScope_test2015_131.C \
  inputmoveDeclarationToInnermostScope_test2015_132.C \
  inputmoveDeclarationToInnermostScope_test2015_134.C \
  inputmoveDeclarationToInnermostScope_test2015_135.C \
  inputmoveDeclarationToInnermostScope_test2015_137.C \
  inputmoveDeclarationToInnermostScope_test2015_139.C \
  inputmoveDeclarationToInnermostScope_test2015_143.C \
  inputmoveDeclarationToInnermostScope_test2015_144.C \
  inputmoveDeclarationToInnermostScope_test2015_145.C \
  inputmoveDeclarationToInnermostScope_test2015_146.C \
  inputmoveDeclarationToInnermostScope_test2015_147.C \
  inputmoveDeclarationToInnermostScope_test2015_148.C \
  inputmoveDeclarationToInnermostScope_test2015_149.C \
  inputmoveDeclarationToInnermostScope_test2015_150.C \
  inputmoveDeclarationToInnermostScope_test2015_151.C \
  inputmoveDeclarationToInnermostScope_test2015_152.C \
  inputmoveDeclarationToInnermostScope_test2015_153.C \
  inputmoveDeclarationToInnermostScope_test2015_154.C \
  inputmoveDeclarationToInnermostScope_test2015_155.C \
  inputmoveDeclarationToInnermostScope_test2015_156.C \
  inputmoveDeclarationToInnermostScope_test2015_157.C 

# DQ (11/13/2015): Note that inputmoveDeclarationToInnermostScope_test2015_141.C 
# and inputmoveDeclarationToInnermostScope_test2015_133.C only fail when using 
# the -rose:merge_decl_assign option.

# DQ (12/23/2014): Added tests specific to use of move tool with token-based unparsing.
FAILING_MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_FULLY_SUPPORTED  = \
  inputmoveDeclarationToInnermostScope_test2014_09.C \
  inputmoveDeclarationToInnermostScope_test2014_12.C \
  inputmoveDeclarationToInnermostScope_test2015_100.C \
  inputmoveDeclarationToInnermostScope_test2015_101.C \
  inputmoveDeclarationToInnermostScope_test2015_114.C \
  inputmoveDeclarationToInnermostScope_test2015_133.C \
  inputmoveDeclarationToInnermostScope_test2015_136.C \
  inputmoveDeclarationToInnermostScope_test2015_138.C \
  inputmoveDeclarationToInnermostScope_test2015_140.C \
  inputmoveDeclarationToInnermostScope_test2015_141.C \
  inputmoveDeclarationToInnermostScope_test2015_142.C

# DQ (11/28/2015: Test codes demonstrating imperfect formatting:
#  inputmoveDeclarationToInnermostScope_test2015_04.C 


# DQ (8/16/2015): Failing test with bugfix for type transformations.
#  inputmoveDeclarationToInnermostScope_test2014_12.C 
#  inputmoveDeclarationToInnermostScope_test2015_04.C 
#  inputmoveDeclarationToInnermostScope_test2015_45.C 
#  inputmoveDeclarationToInnermostScope_test2015_62.C 
#  inputmoveDeclarationToInnermostScope_test2015_114.C 

# DQ (1/11/2014): Failing test with new design.
#  inputmoveDeclarationToInnermostScope_test2015_45.C 
#  inputmoveDeclarationToInnermostScope_test2015_46.C 
#  inputmoveDeclarationToInnermostScope_test2015_47.C 
#  inputmoveDeclarationToInnermostScope_test2015_48.C 

# DQ (1/11/2014): Failing test with new design.
# inputmoveDeclarationToInnermostScope_test2014_09.C 
# inputmoveDeclarationToInnermostScope_test2014_26.C 
# inputmoveDeclarationToInnermostScope_test2015_06.C 

# DQ (1/4/2014): Failing test with new design.
#  inputmoveDeclarationToInnermostScope_test2014_20.C 
#  inputmoveDeclarationToInnermostScope_test2014_26.C 
#  inputmoveDeclarationToInnermostScope_test2015_06.C 
#  inputmoveDeclarationToInnermostScope_test2015_07.C 
#  inputmoveDeclarationToInnermostScope_test2015_11.C 

# DQ (12/23/2014): These fail (for the same reason) and demonstrate a bug in the move tool.
# inputmoveDeclarationToInnermostScope_test2014_18.C 
# inputmoveDeclarationToInnermostScope_test2014_22.C
# inputmoveDeclarationToInnermostScope_5.C (while statements not yet supported)

# DQ (12/23/2014): Notes:
# inputmoveDeclarationToInnermostScope_2.C (works, but demonstrates that "\" is not output as token)
# inputmoveDeclarationToInnermostScope_4.C (works, but diff includes "{" and "}" for for loop bodies that didn't include that syntax).
# inputmoveDeclarationToInnermostScope_6.C (works, but same issue as inputmoveDeclarationToInnermostScope_4.C).
# inputmoveDeclarationToInnermostScope_9.C (works, but same issue as inputmoveDeclarationToInnermostScope_4.C).
# inputmoveDeclarationToInnermostScope_14.C (works, but same issue as inputmoveDeclarationToInnermostScope_4.C).


MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_FULLY_SUPPORTED_RESULTS = $(addprefix rose_, ${MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_FULLY_SUPPORTED})

PASSING_MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_RESULTS = \
   $(MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_FULLY_SUPPORTED_RESULTS) 

PASSING_MOVE_MERGE_DECLS_TESTCODES_RESULTS = \
   $(MOVE_MERGE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS) \
   $(MOVE_MERGE_DECLS_TESTCODES_PARTIALLY_SUPPORTED_RESULTS)

REFERENCE_PATH = $(top_srcdir)/tests/roseTests/astInterfaceTests/referenceResults
MOVE_DECLS_DIFF_FILES=$(PASSING_MOVE_DECLS_TESTCODES_RESULTS:.C=.C.diff)

MOVE_MERGE_DECLS_DIFF_FILES=$(PASSING_MOVE_MERGE_DECLS_TESTCODES_RESULTS:.C=.C.diff)
DIFF=diff
#$(MOVE_DECLS_DIFF_FILES):$(@:.C.diff=.C)
$(MOVE_DECLS_DIFF_FILES):$(PASSING_MOVE_DECLS_TESTCODES_RESULTS)
	echo "Verifying moveDeclarationToInnermostScope translation by diff ..."; \
	if $(DIFF) $(@:.C.diff=.C) $(REFERENCE_PATH)/$(@:.C.diff=.C) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; rm -rf $@; exit 1; fi

$(MOVE_MERGE_DECLS_DIFF_FILES):$(PASSING_MOVE_MERGE_DECLS_TESTCODES_RESULTS)
	echo "Verifying moveDeclarationToInnermostScope translation with merge by diff ..."; \
	if $(DIFF) $(@:.C.diff=.C) $(REFERENCE_PATH)/withmerge/$(@:.C.diff=.C) > $@ ; then echo "Test Passed" ; else echo "Files differ; test falied"; rm -rf $@; exit 1; fi


# DQ (6/28/2015): Added additional test using new option: -rose:merge_decl_assign
$(MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_FULLY_SUPPORTED_RESULTS): $(srcdir)/$(@:rose_%=%) moveDeclarationToInnermostScope 
	echo "Test without new option: -rose:merge_decl_assign"
	$(VALGRIND) ./moveDeclarationToInnermostScope $(TEST_CXXFLAGS) -rose:unparse_tokens -c -rose:debug $(srcdir)/$(@:rose_%=%)
	echo "Test using new option: -rose:merge_decl_assign"
	$(VALGRIND) ./moveDeclarationToInnermostScope $(TEST_CXXFLAGS) -rose:unparse_tokens -rose:merge_decl_assign -c -rose:debug $(srcdir)/$(@:rose_%=%)

# Without new option fails for test2015_04.C?
# With new option fails for test2014_12.C

test_move_decls: $(PASSING_MOVE_DECLS_TESTCODES_RESULTS)

# DQ (8/16/2015): This test demonstrates a recent bug in the token based unparsing caused by a 
# fix to support more general recognition of transforamtions of types).
# test2014_12.C demonstrates the first issue.
test_move_decls_with_token_unparsing: $(PASSING_MOVE_DECLS_TESTCODES_WITH_TOKEN_UNPARSING_RESULTS)
	@echo "********************************************************************************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/astInterface_tests/moveDeclarationToInnermostScope: make test_move_decls_with_token_unparsing rule complete (terminated normally) ******"
	@echo "********************************************************************************************************************************************************************"

# DQ (11/4/2015): Restrict testing of the token-based unparsing to later versions of GNU compilers (at least initially).
# DQ (11/2/2015): Added token-based unparsing tests using the move-tool to the move_diff_check test rule.
# test both diff cases
# move_diff_check: move_decls_diff_check move_merge_decls_diff_check
# move_diff_check: move_decls_diff_check move_merge_decls_diff_check test_move_decls_with_token_unparsing
if ROSE_USING_GCC_VERSION_LATER_4_4
move_diff_check: move_decls_diff_check move_merge_decls_diff_check test_move_decls_with_token_unparsing
else
move_diff_check: move_decls_diff_check move_merge_decls_diff_check
endif

# only move , no merge
move_decls_diff_check: $(MOVE_DECLS_DIFF_FILES)
# both move and merge
move_merge_decls_diff_check: $(MOVE_MERGE_DECLS_DIFF_FILES)

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
	rose_inputbuildStatementFromString.C            \
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
		$(VALGRIND) ./moveDeclarationToInnermostScope $(TEST_CXXFLAGS) -c -rose:debug -rose:trans-tracking $(srcdir)/$(@:rose_%=%)

$(MOVE_DECLS_TESTCODES_PARTIALLY_SUPPORTED_RESULTS): $(srcdir)/$(@:rose_%=%) moveDeclarationToInnermostScope 
		$(VALGRIND) ./moveDeclarationToInnermostScope $(TEST_CXXFLAGS) -c -rose:debug -rose:trans-tracking -rose:keep_going $(srcdir)/$(@:rose_%=%)

$(MOVE_MERGE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS): $(srcdir)/$(@:rose_v2_%=%) moveDeclarationToInnermostScope 
		$(VALGRIND) ./moveDeclarationToInnermostScope $(TEST_CXXFLAGS) -c -rose:debug -rose:merge_decl_assign -rose:trans-tracking $(srcdir)/$(@:rose_v2_%=%) -rose:output $@

$(MOVE_MERGE_DECLS_TESTCODES_PARTIALLY_SUPPORTED_RESULTS): $(srcdir)/$(@:rose_v2%=%) moveDeclarationToInnermostScope 
		$(VALGRIND) ./moveDeclarationToInnermostScope $(TEST_CXXFLAGS) -c -rose:debug -rose:merge_decl_assign -rose:trans-tracking -rose:keep_going $(srcdir)/$(@:rose_v2%=%) -rose:output $@

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
		CMD="$$(pwd)/buildJavaPackage$(EXEEXT) $(TEST_CXXFLAGS) -I$(abs_srcdir) -c $(abspath $<)"  \
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
       inputbuildJavaPackage.C inputloopCollapsing_1.C inputbuildStatementFromString.C \
       inputmoveDeclarationToInnermostScope_test2014_15.h \
       inputmoveDeclarationToInnermostScope_test2014_19.h \
       inputmoveDeclarationToInnermostScope_test2014_23.h \
       inputmoveDeclarationToInnermostScope_test2015_13.h \
       inputmoveDeclarationToInnermostScope_test2015_16.h \
       inputmoveDeclarationToInnermostScope_test2015_22.h \
       inputmoveDeclarationToInnermostScope_test2015_40.h \
       inputmoveDeclarationToInnermostScope_test2015_142.h \
       inputmoveDeclarationToInnermostScope_20.C \
       inputmoveDeclarationToInnermostScope_21.C \
       inputmoveDeclarationToInnermostScope_22.C \
       inputmoveDeclarationToInnermostScope_23.C \
       inputmoveDeclarationToInnermostScope_24.C \
       inputmoveDeclarationToInnermostScope_25.C \
       inputmoveDeclarationToInnermostScope_26.C \
       inputmoveDeclarationToInnermostScope_27.C \
       referenceResults/rose_inputmoveDeclarationToInnermostScope_15.C \
       referenceResults/rose_inputmoveDeclarationToInnermostScope_16.C \
       referenceResults/rose_inputmoveDeclarationToInnermostScope_17.C \
       referenceResults/rose_inputmoveDeclarationToInnermostScope_18.C \
       referenceResults/rose_inputmoveDeclarationToInnermostScope_19.C \
       referenceResults/rose_inputmoveDeclarationToInnermostScope_20.C \
       referenceResults/rose_inputmoveDeclarationToInnermostScope_21.C \
       referenceResults/rose_inputmoveDeclarationToInnermostScope_22.C \
       referenceResults/rose_inputmoveDeclarationToInnermostScope_23.C \
       referenceResults/rose_inputmoveDeclarationToInnermostScope_24.C \
       referenceResults/rose_inputmoveDeclarationToInnermostScope_25.C \
       referenceResults/rose_inputmoveDeclarationToInnermostScope_26.C \
       referenceResults/rose_inputmoveDeclarationToInnermostScope_27.C \
       referenceResults/withmerge/rose_v2_inputmoveDeclarationToInnermostScope_22.C


# JP (10/4/14): Added the unit tests
unit-tests:
	$(MAKE) check-local -C unitTests

#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

# move_decls_diff_check move_merge_decls_diff_check
check-local: $(generatedCodeExamples) move_diff_check 
	@echo "*****************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/astInterface_tests: make check rule complete (terminated normally) ******"
	@echo "*****************************************************************************************************"

clean-local:
	rm -f $(generatedCodeExamples) *.dot $(PASSING_MOVE_DECLS_TESTCODES_RESULTS) $(MOVE_MERGE_DECLS_TESTCODES_FULLY_SUPPORTED_RESULTS)
	rm -f $(addsuffix .passed, $(generatedCodeExamples)) $(MOVE_DECLS_DIFF_FILES) $(MOVE_MERGE_DECLS_DIFF_FILES)
	rm -f $(addsuffix .failed, $(generatedCodeExamples))
	rm -f $(addsuffix .passed, $(PASSING_MOVE_DECLS_TESTCODES_RESULTS))
	rm -f $(addsuffix .failed, $(PASSING_MOVE_DECLS_TESTCODES_RESULTS))
	rm -f $(addsuffix .diff, $(PASSING_MOVE_DECLS_TESTCODES_RESULTS))
	rm -rf rose_*.C
