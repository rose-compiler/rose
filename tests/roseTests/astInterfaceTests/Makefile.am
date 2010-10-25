# Liao, Jan. 9, 2008
# Makefile.am for a set of test AST builders and test input code
#-------------------------------------------------------------
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES =  \
        $(ROSE_INCLUDES) \
        -I$(top_srcdir)/src/frontend/SageIII/sageInterface
# We install a few test programs since they might be useful for external users
bin_PROGRAMS = loopInterchange loopUnrolling loopTiling

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
    generateUniqueName

# list of test SAGE AST builders 
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

# libsageInterface.la is included in rose.la already?
LDADD =  $(ROSE_LIBS)

#    $(top_builddir)/src/frontend/SageIII/sageInterface/libsageInterface.la 
if ROSE_USE_LONG_MAKE_CHECK_RULE
else
endif
# Be sure to include proper suffixes, like .C
# TODO: handle side effects when deleting AST nodes
#failing_generatedCodeExamples = rose_inputdeepDelete.C 

generatedCodeExamples = \
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
  abiStuffTestDone \
  rose_inputAbiStuffTestUPC.upc \
  rose_inputbuildNullStatement.C \
  rose_inputbuildForStmt.C \
  rose_inputcanonicalLoop.C \
  rose_inputloopNormalization.C \
  rose_inputcollectReadWriteVariables.C \
  rose_inputtypeTraits.C \
  rose_inputlivenessAnalysis.C \
  rose_inputreplaceMacroCalls.C \
  rose_inputbuildAbstractHandle.C \
  rose_inputbuildTypedefDeclaration.C \
  rose_inputloopUnrolling.C \
  rose_inputgetDependentDecls.C \
  rose_inputloopInterchange.C \
  rose_inputloopTiling.C \
  rose_inputdeepDelete.C \
  rose_inputinsertStatementBeforeFunction.C \
  rose_inputRemoveStatementCommentRelocation.C \
  rose_inputgenerateUniqueName.C

# Liao 8/17/2010
#if USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  generatedCodeExamples += \
    rose_inputbuildProcedureHeaderStatement.f 
endif

# turn off edg warnings such as "variable x declared but never referenced" for test input files
TEST_CXXFLAGS = -rose:verbose 0 --edg:no_warnings -g3

rose_inputdeepDelete.C:deepDelete
	./deepDelete$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputdeepDelete.C
rose_inputBlank1.C:buildFunctionDeclaration
	./buildFunctionDeclaration$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputBlank1.C
rose_inputBlank2.C:buildNondefiningFunction
	./buildNondefiningFunction$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputBlank2.C
rose_inputfindMain.C:findMain
	./findMain$(EXEEXT)  $(TEST_CXXFLAGS) -c  $(srcdir)/inputfindMain.C
rose_inputbuildVariableDeclaration.C:buildVariableDeclaration
	./buildVariableDeclaration$(EXEEXT)  $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildVariableDeclaration.C
rose_inputbuildAssignmentStmt.C:buildAssignmentStmt
	./buildAssignmentStmt$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildAssignmentStmt.C	
rose_inputbuildIfStmt.C:buildIfStmt
	./buildIfStmt$(EXEEXT) $(TEST_CXXFLAGS) -I$(srcdir) -c $(srcdir)/inputbuildIfStmt.C 
rose_inputbuildFile.C:buildFile
	rm -rf testfile525.cpp
	rm -rf testfile626.cpp
	./buildFile$(EXEEXT) $(TEST_CXXFLAGS) -I$(srcdir) -c $(srcdir)/inputbuildFile.C 
rose_inputbuildFunctionCalls.C:buildFunctionCalls
	./buildFunctionCalls$(EXEEXT) $(TEST_CXXFLAGS) -I$(srcdir) -c $(srcdir)/inputbuildFunctionCalls.C	
rose_inputbuildPragmaDeclaration.c:buildPragmaDeclaration
	./buildPragmaDeclaration$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildPragmaDeclaration.c
rose_inputAttachComment.C:attachComment
	./attachComment$(EXEEXT)  $(TEST_CXXFLAGS) -c $(srcdir)/inputAttachComment.C	
rose_inputInsertHeader.C:insertHeader
	./insertHeader$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputInsertHeader.C
rose_inputbuildExpression.C: buildExpression
	./buildExpression$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildExpression.C
rose_inputbuildStructDeclaration.C:buildStructDeclaration
	./buildStructDeclaration$(EXEEXT)  $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildStructDeclaration.C
rose_inputbuildStructDeclaration2.C:buildStructDeclaration2
	./buildStructDeclaration2$(EXEEXT)  $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildStructDeclaration2.C
rose_inputLookupNamedType.C:lookupNamedType
	./lookupNamedType$(EXEEXT)  $(TEST_CXXFLAGS) -c $(srcdir)/inputLookupNamedType.C
rose_inputMovePreprocessingInfo.C:movePreprocessingInfo
	./movePreprocessingInfo$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputMovePreprocessingInfo.C
rose_inputbuildCpreprocessorDefineDeclaration.C:buildCpreprocessorDefineDeclaration
	./buildCpreprocessorDefineDeclaration$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildCpreprocessorDefineDeclaration.C
rose_inputinstrumentEndOfFunction.C:instrumentEndOfFunction
	./instrumentEndOfFunction$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputinstrumentEndOfFunction.C
rose_inputisUpcSharedType.upc:isUpcSharedType
	./isUpcSharedType$(EXEEXT) $(TEST_CXXFLAGS) -rose:skipfinalCompileStep -c $(srcdir)/inputisUpcSharedType.upc

rose_inputisUpcPhaseLessSharedType.upc:isUpcPhaseLessSharedType
	./isUpcPhaseLessSharedType$(EXEEXT) $(TEST_CXXFLAGS) -rose:skipfinalCompileStep -c $(srcdir)/inputisUpcPhaseLessSharedType.upc

rose_inputbuildLabelStatement.C:buildLabelStatement
	./buildLabelStatement$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildLabelStatement.C

rose_inputbuildSizeOfOp.C:buildSizeOfOp
	./buildSizeOfOp$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildSizeOfOp.C

abiStuffTestDone: abiStuffTest inputAbiStuffTest.c
	./abiStuffTest$(EXEEXT) $(srcdir)/inputAbiStuffTest.c && touch abiStuffTestDone

rose_inputAbiStuffTestUPC.upc:abiStuffTestUPC
	./abiStuffTestUPC$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputAbiStuffTestUPC.upc

rose_inputbuildNullStatement.C:buildNullStatement
	./buildNullStatement$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildNullStatement.C

rose_inputbuildForStmt.C:buildForStmt
	./buildForStmt$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildForStmt.C

rose_inputcanonicalLoop.C:canonicalLoop
	./canonicalLoop$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputcanonicalLoop.C

rose_inputloopNormalization.C:loopNormalization
	./loopNormalization$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputloopNormalization.C

rose_inputcollectReadWriteVariables.C:collectReadWriteVariables
	./collectReadWriteVariables$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputcollectReadWriteVariables.C

rose_inputtypeTraits.C:typeTraits
	./typeTraits$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputtypeTraits.C

rose_inputlivenessAnalysis.C:livenessAnalysis
	./livenessAnalysis$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputlivenessAnalysis.C
rose_inputgenerateUniqueName.C:generateUniqueName
	./generateUniqueName$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputgenerateUniqueName.C 1>rose_inputgenerateUniqueName.out


rose_inputbuildProcedureHeaderStatement.f : buildProcedureHeaderStatement
# DQ (9/13/2010): I think that we need both levels of if tests else --without-java will not work.
# DQ (3/18/2009): Make this dependent upon if ROSE was configured to use Fortran.
# if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT 
# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
	./buildProcedureHeaderStatement$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildProcedureHeaderStatement.f
else
	@echo "Fortran test (rose_inputbuildProcedureHeaderStatement.f) not run because ROSE was not configured to support Fortran tests (ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT)."
endif

# else
#	@echo "Fortran test (rose_inputbuildProcedureHeaderStatement.f) not run because ROSE was not configured to support Fortran tests (ROSE_USE_OPEN_FORTRAN_PARSER)."
# endif

rose_inputbuildAbstractHandle.C:buildAbstractHandle
	./buildAbstractHandle$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildAbstractHandle.C

rose_inputbuildTypedefDeclaration.C:buildTypedefDeclaration
	./buildTypedefDeclaration$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildTypedefDeclaration.C
rose_inputgetDependentDecls.C:getDependentDecls
	./getDependentDecls$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputgetDependentDecls.C

if ROSE_USE_INDENT
rose_inputloopUnrolling.C:loopUnrolling
	./loopUnrolling$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputloopUnrolling.C -rose:loopunroll:abstract_handle "Statement<position,5>" -rose:loopunroll:factor 3 && indent rose_inputloopUnrolling.C
	./loopUnrolling$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputloopUnrolling.C -rose:loopunroll:abstract_handle "Statement<position,6>" -rose:loopunroll:factor 5 -rose:output rose_inputloopUnrolling1.C && indent rose_inputloopUnrolling1.C 
	./loopUnrolling$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputloopUnrolling.C -rose:loopunroll:abstract_handle "Statement<position,6>" -rose:loopunroll:factor 3 -rose:output rose_inputloopUnrolling2.C && indent rose_inputloopUnrolling2.C
else
# This case is required for platforms where we test ROSE, and which sometimes don't have "indent" available.
rose_inputloopUnrolling.C:loopUnrolling
	./loopUnrolling$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputloopUnrolling.C -rose:loopunroll:abstract_handle "Statement<position,5>" -rose:loopunroll:factor 3
	./loopUnrolling$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputloopUnrolling.C -rose:loopunroll:abstract_handle "Statement<position,6>" -rose:loopunroll:factor 5 -rose:output rose_inputloopUnrolling1.C
	./loopUnrolling$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputloopUnrolling.C -rose:loopunroll:abstract_handle "Statement<position,6>" -rose:loopunroll:factor 3 -rose:output rose_inputloopUnrolling2.C
endif

# interchange the first loop within the input file, interchange depth is 4, lexicographical order is 1.
rose_inputloopInterchange.C:loopInterchange
	./loopInterchange$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputloopInterchange.C -rose:loopInterchange:abstract_handle "ForStatement<numbering,1>" -rose:loopInterchange:depth 4 -rose:loopInterchange:order 1 

# Tile the first loop within the input file at depth 3 , tile size is 5
rose_inputloopTiling.C:loopTiling
	./loopTiling$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputloopTiling.C -rose:loopTiling:abstract_handle "ForStatement<numbering,1>" -rose:loopTiling:depth 3 -rose:loopTiling:tilesize 5


rose_inputreplaceMacroCalls.C:replaceMacroCalls
# comment out since boost:wave causes seg fault on 64-bit machines
#	./replaceMacroCalls$(EXEEXT) $(TEST_CXXFLAGS) -rose:wave -c $(srcdir)/inputreplaceMacroCalls.C

# DQ (9/16/2010): Added this test code to demonstrate problem with adding statments around statments with comments or CPP directives attached.
rose_inputinsertStatementBeforeFunction.C: insertStatementBeforeFunction $(srcdir)/inputinsertStatementBeforeFunction_1.h $(srcdir)/inputinsertStatementBeforeFunction_2.h
	./insertStatementBeforeFunction$(EXEEXT) $(TEST_CXXFLAGS) -c -I$(srcdir) $(srcdir)/inputinsertStatementBeforeFunction.C

# There are a lot of different cases that we now test for with the relocation of comments support in the removeStatement() function.
rose_inputRemoveStatementCommentRelocation.C: removeStatementCommentRelocation $(srcdir)/inputRemoveStatementCommentRelocation_1.h $(srcdir)/inputRemoveStatementCommentRelocation_2.h
	./removeStatementCommentRelocation$(EXEEXT) $(TEST_CXXFLAGS) -c -I$(srcdir) $(srcdir)/inputRemoveStatementCommentRelocation.C
	./removeStatementCommentRelocation$(EXEEXT) $(TEST_CXXFLAGS) -c -I$(srcdir) $(srcdir)/inputRemoveStatementCommentRelocation_withTopBoundingStatement.C
	./removeStatementCommentRelocation$(EXEEXT) $(TEST_CXXFLAGS) -c -I$(srcdir) $(srcdir)/inputRemoveStatementCommentRelocation_withoutTopBoundingStatement.C
	./removeStatementCommentRelocation$(EXEEXT) $(TEST_CXXFLAGS) -c -I$(srcdir) $(srcdir)/inputRemoveStatementCommentRelocation_withBottomBoundingStatement.C
	./removeStatementCommentRelocation$(EXEEXT) $(TEST_CXXFLAGS) -c -I$(srcdir) $(srcdir)/inputRemoveStatementCommentRelocation_withoutBottomBoundingStatement.C
	./removeStatementCommentRelocation$(EXEEXT) $(TEST_CXXFLAGS) -c -I$(srcdir) $(srcdir)/inputRemoveStatementCommentRelocation_withTopandBottomBoundingStatement.C
	./removeStatementCommentRelocation$(EXEEXT) $(TEST_CXXFLAGS) -c -I$(srcdir) $(srcdir)/inputRemoveStatementCommentRelocation_withoutTopandBottomBoundingStatement.C


# MUST keep this up-to-date!!
EXTRA_DIST = inputBlank1.C inputBlank2.C inputfindMain.C inputbuildVariableDeclaration.C \
       inputbuildAssignmentStmt.C inputbuildFunctionCalls.C inputbuildFunctionCalls.h \
       inputbuildPragmaDeclaration.c inputAttachComment.C inputInsertHeader.C \
       inputbuildExpression.C inputbuildStructDeclaration.C inputLookupNamedType.C \
       inputbuildFile.C inputMovePreprocessingInfo.C inputbuildIfStmt.C \
       inputbuildCpreprocessorDefineDeclaration.C inputinstrumentEndOfFunction.C \
       inputisUpcSharedType.upc inputisUpcPhaseLessSharedType.upc inputbuildLabelStatement.C \
       inputbuildSizeOfOp.C inputAbiStuffTest.c inputAbiStuffTestUPC.upc \
       inputbuildNullStatement.C inputbuildForStmt.C inputcanonicalLoop.C \
       inputloopNormalization.C inputcollectReadWriteVariables.C inputtypeTraits.C \
       inputlivenessAnalysis.C inputbuildProcedureHeaderStatement.f inputreplaceMacroCalls.C \
       inputbuildAbstractHandle.C inputloopUnrolling.C inputgetDependentDecls.C \
       inputloopInterchange.C inputloopTiling.C inputbuildStructDeclaration2.C \
       inputbuildTypedefDeclaration.C inputdeepDelete.C \
       inputinsertStatementBeforeFunction.C inputinsertStatementBeforeFunction_1.h \
       inputinsertStatementBeforeFunction_2.h inputRemoveStatementCommentRelocation.C \
       inputRemoveStatementCommentRelocation_1.h inputRemoveStatementCommentRelocation_2.h \
       inputRemoveStatementCommentRelocation_withBottomBoundingStatement.C \
       inputRemoveStatementCommentRelocation_withTopBoundingStatement.C \
       inputRemoveStatementCommentRelocation_withTopandBottomBoundingStatement.C \
       inputRemoveStatementCommentRelocation_withoutBottomBoundingStatement.C \
       inputRemoveStatementCommentRelocation_withoutTopBoundingStatement.C \
       inputRemoveStatementCommentRelocation_withoutTopandBottomBoundingStatement.C \
       inputgenerateUniqueName.C


check-local:
	@echo "Test for ROSE SAGE Interfaces."
	$(MAKE) $(generatedCodeExamples)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/astInterfaceTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

clean-local:
	rm -f *.o test*.C rose_*.c rose_*.C rose_*.f rose_*.cpp *.C.pdf *.c.pdf testfile525.cpp testfile626.cpp rose_*.upc abiStuffTestDone *.dot rose_*
	   

