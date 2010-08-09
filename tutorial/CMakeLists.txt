
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(roseHPCT)
add_subdirectory(outliner)
add_subdirectory(intelPin)


########### next target ###############

set($(TUTORIAL_EXAMPLES)_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

kde4_add_executable($(TUTORIAL_EXAMPLES) ${$(TUTORIAL_EXAMPLES)_SRCS})

target_link_libraries($(TUTORIAL_EXAMPLES) ${KDE4_KDECORE_LIBS})


########### next target ###############

set($(TUTORIAL_EXAMPLES)_SRCS ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

kde4_add_executable($(TUTORIAL_EXAMPLES) ${$(TUTORIAL_EXAMPLES)_SRCS})

target_link_libraries($(TUTORIAL_EXAMPLES) ${KDE4_KDECORE_LIBS})


########### next target ###############

set(sharedMemoryTraversals_SRCS sharedMemoryTraversals.C)

kde4_add_executable(sharedMemoryTraversals ${sharedMemoryTraversals_SRCS})

target_link_libraries(sharedMemoryTraversals ${KDE4_KDECORE_LIBS})


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## Add Projects that are associated with capabilities that belong to ROSE here
#SUBDIRS = database roseHPCT outliner intelPin
#
#INCLUDES = $(ROSE_INCLUDES)
#
## all: exampleMakefile
#
#ROSE_FLAGS = --edg:no_warnings -w
#
#noinst_HEADERS = \
#   coverage_funcsC.h \
#   inputCode_collectComments.h
#
##varSubstCopy , disabled this one Liao, and sorted the list
#TUTORIAL_EXAMPLES = \
#   abstractHandle1 \
#   abstractHandle2 \
#   accumulatorAttributeTraversal \
#   addAssignmentStmt \
#   addComments \
#   addArbitraryTextForUnparser \
#   addExpression \
#   addFunctionCalls \
#   addFunctionDeclaration \
#   addFunctionDeclaration2 \
#   addFunctionDeclaration3 \
#   addStruct \
#   addVariableDeclaration \
#   addVariableDeclaration2 \
#   AST_PDF_Generator \
#   astFileIO_GenerateBinaryFile \
#   astFileIO_ReadMultipleASTs \
#   ASTGraphGenerator \
#   bin_example \
#   binaryAnalysis \
#   binaryAnalysis_symbolTable \
#   binaryFileRead \
#   sb_callgraphAnalysis \
#   buildCFG \
#   buildCG \
#   callGraphAnalysis \
#   changeFunction \
#   charmSupport \
#   classHierarchyGraph \
#   classicObjectOrientedVisitorPatternMemoryPoolTraversal \
#   classicObjectOrientedVisitorPatternTraversal \
#   codeCoverage \
#   codeGenerationFormatControl \
#   collectComments \
#   collectDefineDirectives \
#   combinedTraversals \
#   commandlineProcessing \
#   constantFoldingTransformation \
#   constantUnFoldingTransformation \
#   customGraphGeneration \
#   customGraphGenerationWithColors \
#   customGraphGenerationWithFiltering \
#   dataBaseUsage \
#   dataFlowGraph \
#   debuggingIRnodeToString \
#   debuggingSourceCodePositionInformation \
#   defuseAnalysis \
#   dwarfAnalysis \
#   dwarfInstructionAddressToSourceLineAnalysis \
#   generatingUniqueNamesFromDeclaration \
#   graphGenerationOfSubtree \
#   graphTraversal \
#   identityTranslator \
#   inheritedAndSynthesizedAttributeTraversal \
#   inheritedAttributeTraversal \
#   inlineTransformations \
#   instrumentationExample \
#   instrumentEndOfFunction \
#   interveneAtVariables \
#   lineReplacement \
#   loopNestingInfoProcessing \
#   loopOptimization \
#   loopProcessor \
#   loopRecognition \
#   nestedQueryExample \
#   nestedTraversal \
#   partialRedundancyElimination \
#   persistantAttributes \
#   prePostTraversal \
#   queryLibraryExample \
#   reductionRecognition \
#   resolveOverloadedFunction \
#   seedBugsExample_arrayIndexing \
#   scopeInformation \
#   selectedFileTranslation \
#   sprintf_to_snprintf  \
#   switchWithoutDefault \
#   symbolTableHandling \
#   synthesizedAttributeTraversal \
#   tauInstrumenter \
#   templateParameter \
#   traversalShortCircuit \
#   traverseIRnodeTypes \
#   typeInfoFromFunctionParameters \
#   typeTransformation \
#   virtualCFG \
#   virtualFunctionIdentification \
#   visitorMemoryPoolTraversal \
#   visitorTraversal \
#   volatileTypeModifier \
#   wholeASTGraphGenerator \
#   wholeGraphAST \
#   wholeGraphFilterOnPersistantAttributes
#
## DQ (3/21/2009): Separate those that compile under Cygwin from those that don't.
#if ROSE_BUILD_OS_IS_CYGWIN
#  noinst_PROGRAMS = $(TUTORIAL_EXAMPLES)
#else
#  noinst_PROGRAMS = $(TUTORIAL_EXAMPLES) sharedMemoryTraversals
#  sharedMemoryTraversals_SOURCES = sharedMemoryTraversals.C
#endif
#
#binaryFileRead_SOURCES = binaryFileRead.C
#
#ASTGraphGenerator_SOURCES = ASTGraphGenerator.C
#
#AST_PDF_Generator_SOURCES = AST_PDF_Generator.C
#
#wholeASTGraphGenerator_SOURCES = wholeASTGraphGenerator.C
#
#identityTranslator_SOURCES = identityTranslator.C
#
#lineReplacement_SOURCES = lineReplacement.C
#
#queryLibraryExample_SOURCES = queryLibraryExample.C
#
#nestedQueryExample_SOURCES = nestedQueryExample.C
#
## need to call this something different since it is likely
## to be easily confused with the visitorPatternTraversal.C
#visitorTraversal_SOURCES = visitorTraversal.C
#
#prePostTraversal_SOURCES = prePostTraversal.C
#
#combinedTraversals_SOURCES = combinedTraversals.C
#
#traverseIRnodeTypes_SOURCES = traverseIRnodeTypes.C
#
#classicObjectOrientedVisitorPatternTraversal_SOURCES = classicObjectOrientedVisitorPatternTraversal.C
#
#inheritedAttributeTraversal_SOURCES = inheritedAttributeTraversal.C
#
#synthesizedAttributeTraversal_SOURCES = synthesizedAttributeTraversal.C
#
#inheritedAndSynthesizedAttributeTraversal_SOURCES = inheritedAndSynthesizedAttributeTraversal.C
#
#accumulatorAttributeTraversal_SOURCES = accumulatorAttributeTraversal.C
#
#persistantAttributes_SOURCES = persistantAttributes.C
#
#nestedTraversal_SOURCES = nestedTraversal.C
#
#visitorMemoryPoolTraversal_SOURCES = visitorMemoryPoolTraversal.C
#
#classicObjectOrientedVisitorPatternMemoryPoolTraversal_SOURCES = classicObjectOrientedVisitorPatternMemoryPoolTraversal.C
#
#loopRecognition_SOURCES = loopRecognition.C
#
#typeInfoFromFunctionParameters_SOURCES = typeInfoFromFunctionParameters.C
#
#resolveOverloadedFunction_SOURCES = resolveOverloadedFunction.C
#
#templateParameter_SOURCES = templateParameter.C
#
#instrumentEndOfFunction_SOURCES = instrumentEndOfFunction.C
#
#reductionRecognition_SOURCES= reductionRecognition.C
#
#instrumentationExample_SOURCES = instrumentationExample.C
#
#abstractHandle1_SOURCES = abstractHandle1.cpp
#
#abstractHandle2_SOURCES = abstractHandle2.cpp
#
#addVariableDeclaration_SOURCES = addVariableDeclaration.C
#
#addVariableDeclaration2_SOURCES = addVariableDeclaration2.C
#
#addAssignmentStmt_SOURCES = addAssignmentStmt.C
#
#addExpression_SOURCES = addExpression.C
#
#addFunctionCalls_SOURCES = addFunctionCalls.C
#
#addFunctionDeclaration_SOURCES = addFunctionDeclaration.C
#
#addFunctionDeclaration2_SOURCES = addFunctionDeclaration2.C
#
#addFunctionDeclaration3_SOURCES = addFunctionDeclaration3.C
#
#
#loopOptimization_SOURCES = loopOptimization.C
#
#loopProcessor_SOURCES = LoopProcessor.C
#
#buildCFG_SOURCES = buildCFG.C
#
#defuseAnalysis_SOURCES = defuseAnalysis.C
#
#binaryAnalysis_SOURCES = binaryAnalysis.C
#
#sb_callgraphAnalysis_SOURCES = callGraphSourceBinaryExample.C 
#
#binaryAnalysis_symbolTable_SOURCES = binaryAnalysis_symbolTable.C
#
#bin_example_SOURCES = bin_example.C
#
#buildCG_SOURCES = buildCG.C
#
#charmSupport_SOURCES = CharmSupport.C
#
#debuggingIRnodeToString_SOURCES = debuggingIRnodeToString.C
#
#debuggingSourceCodePositionInformation_SOURCES = debuggingSourceCodePositionInformation.C
#
#commandlineProcessing_SOURCES = commandlineProcessing.C
#
#codeCoverage_SOURCES = codeCoverage.C coverageCodeFromShmuel.c
#
#interveneAtVariables_SOURCES = interveneAtVariables.C
#
#changeFunction_SOURCES = changeFunction.C
#
#callGraphAnalysis_SOURCES = callGraphAnalysis.C
#
#classHierarchyGraph_SOURCES = classHierarchyGraph.C
#
#dataBaseUsage_SOURCES = dataBaseUsage.C
#
#dwarfAnalysis_SOURCES = dwarfAnalysis.C
#
#dwarfInstructionAddressToSourceLineAnalysis_SOURCES = dwarfInstructionAddressToSourceLineAnalysis.C
#
#loopNestingInfoProcessing_SOURCES = loopNestingInfoProcessing.C
#
#tauInstrumenter_SOURCES = tauInstrumenter.C
#
#typeTransformation_SOURCES = typeTransformation.C
#
#astFileIO_GenerateBinaryFile_SOURCES = astFileIO_GenerateBinaryFile.C
#
#generatingUniqueNamesFromDeclaration_SOURCES = generatingUniqueNamesFromDeclaration.C
#
#symbolTableHandling_SOURCES = symbolTableHandling.C
#
#scopeInformation_SOURCES = scopeInformation.C
#
#customGraphGeneration_SOURCES = customGraphGeneration.C
#
#inlineTransformations_SOURCES = inlineTransformations.C
#
#addComments_SOURCES = addComments.C
#
#addArbitraryTextForUnparser_SOURCES = addArbitraryTextForUnparser.C
#
#collectComments_SOURCES = collectComments.C
#
#collectDefineDirectives_SOURCES = collectDefineDirectives.C
#
#partialRedundancyElimination_SOURCES = partialRedundancyElimination.C
#
#codeGenerationFormatControl_SOURCES = codeGenerationFormatControl.C
#
#dataFlowGraph_SOURCES = dataFlowGraph.C
#
#customGraphGenerationWithFiltering_SOURCES = customGraphGenerationWithFiltering.C
#
#graphTraversal_SOURCES = graphTraversal.C
#
#wholeGraphAST_SOURCES = wholeGraphAST.C
#
#traversalShortCircuit_SOURCES = traversalShortCircuit.C
#
#customGraphGenerationWithColors_SOURCES = customGraphGenerationWithColors.C
#
#astFileIO_ReadMultipleASTs_SOURCES = astFileIO_ReadMultipleASTs.C
#
#constantFoldingTransformation_SOURCES = constantFoldingTransformation.C
#
#constantUnFoldingTransformation_SOURCES = constantUnFoldingTransformation.C
#
#selectedFileTranslation_SOURCES = selectedFileTranslation.C
#
#wholeGraphFilterOnPersistantAttributes_SOURCES = wholeGraphFilterOnPersistantAttributes.C
#
#graphGenerationOfSubtree_SOURCES = graphGenerationOfSubtree.C
#
#virtualCFG_SOURCES = virtualCFG.C
#
#virtualFunctionIdentification_SOURCES = virtualFunctionIdentification.C
#
##varSubstCopy_SOURCES = varSubstCopy.C
#
#switchWithoutDefault_SOURCES = switchWithoutDefault.C
#
#addStruct_SOURCES = addStruct.C
#
#sprintf_to_snprintf_SOURCES = sprintf_to_snprintf.C
#
#seedBugsExample_arrayIndexing_SOURCES = seedBugsExample_arrayIndexing.C
#
#volatileTypeModifier_SOURCES = volatileTypeModifier.C
#
## Use this to control use of -lrose -ledg vs. the very long list 
## of separate libraries which is faster to use in development.
## LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#LDADD = $(LIBS_WITH_RPATH) $(WAVE_LDFLAGS) $(ROSE_SEPARATE_LIBS) 
#
## #####################################
## Tests for debugging tutorial examples
## #####################################
#
#VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=80 --leak-check=no --error-limit=no --show-reachable=yes
## VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
#VALGRIND =
#
#testASTGraphGenerator: ASTGraphGenerator
#	@echo "Generating dot file ..."
#	./ASTGraphGenerator $(srcdir)/inputCode_2.C
#	@echo "Generate pdf file (default name: test.pdf) from dot file ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_2.C.dot inputCode_2.pdf
#
#testAST_PDF_Generator: AST_PDF_Generator
#	@echo "Generating PDF file ..."
#	./AST_PDF_Generator $(srcdir)/inputCode_2.C
#
#testWholeASTGraphGenerator_small: wholeASTGraphGenerator
#	@echo "Generating dot file of whole AST ..."
#	./wholeASTGraphGenerator $(srcdir)/inputCode_wholeAST_1.C
#	@echo "Generate pdf file (default name: test.pdf) from dot file ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_wholeAST_1_WholeAST.dot inputCode_WholeAST_small.pdf
#
#testWholeASTGraphGenerator_large: wholeASTGraphGenerator
#	@echo "Generating dot file of whole AST ..."
#	./wholeASTGraphGenerator $(srcdir)/inputCode_wholeAST_2.C
#	@echo "Generate pdf file (default name: test.pdf) from dot file ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_wholeAST_2_WholeAST.dot inputCode_WholeAST_large.pdf
#
## DQ (9/6/2008): Added test to generate a graph of the binary for the tiny program example (with debugging data).
#testBinaryASTGraphGenerator: wholeASTGraphGenerator
#	gcc -g $(srcdir)/inputCode_binaryAST_1.c -o inputCode_binaryAST_1
#	./wholeASTGraphGenerator inputCode_binaryAST_1
#	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_binaryAST_1_WholeAST.dot inputCode_binaryAST_1_WholeAST.pdf
#
#testLoopRecognition: loopRecognition
#	./loopRecognition $(srcdir)/inputCode_1.C
#
#testResolveOverloadedFunction: resolveOverloadedFunction
#	./resolveOverloadedFunction $(srcdir)/inputCode_1.C
#
#testInstantiatedTemplate: templateParameter
#	./templateParameter $(srcdir)/inputCode_1.C
#
#testInstrumentationExample: instrumentationExample
#	./instrumentationExample -rose:verbose 0 -c $(srcdir)/inputCode_InstrumentationTranslator.C
#
#testInstrumentEndOfFunction: instrumentEndOfFunction
#	./instrumentEndOfFunction -rose:verbose 0 -c $(srcdir)/inputCode_instrumentEndOfFunction.C
#
#testReductionRecognition: reductionRecognition
#	./reductionRecognition -rose:verbose 0 -c $(srcdir)/inputCode_reductionRecognition.C
#
## turn off the warning for input code with unused variable declarations
#testAddVariableDeclaration: addVariableDeclaration
#	./addVariableDeclaration --edg:no_warnings -c $(srcdir)/inputCode_AddVariableDeclaration.C
#
#testAddVariableDeclaration2: addVariableDeclaration2
#	./addVariableDeclaration2 --edg:no_warnings -c $(srcdir)/inputCode_AddVariableDeclaration2.C
#
#testAddFunctionDeclaration: addFunctionDeclaration
#	./addFunctionDeclaration --edg:no_warnings -c $(srcdir)/inputCode_AddFunctionDeclaration.C
#
#testAddFunctionDeclaration2: addFunctionDeclaration2
#	./addFunctionDeclaration2 --edg:no_warnings -c $(srcdir)/inputCode_AddFunctionDeclaration2.C
#testAddFunctionDeclaration3: addFunctionDeclaration3
#	./addFunctionDeclaration3 --edg:no_warnings -c $(srcdir)/inputCode_AddFunctionDeclaration3.C
#
#testAddAssignmentStmt: addAssignmentStmt
#	./addAssignmentStmt --edg:no_warnings -c $(srcdir)/inputCode_AddAssignmentStmt.C
#
#testAbstractHandle1:abstractHandle1
#	./abstractHandle1  --edg:no_warnings -c $(srcdir)/inputCode_AbstractHandle1.cpp 
#
#testAbstractHandle2:abstractHandle2
#	./abstractHandle2  --edg:no_warnings -c $(srcdir)/inputCode_AbstractHandle2.cpp 
#
#testAddExpression: addExpression
#	./addExpression --edg:no_warnings -c $(srcdir)/inputCode_AddExpression.C
#
#testAddFunctionCalls: addFunctionCalls
#	./addFunctionCalls --edg:no_warnings -c $(srcdir)/inputCode_AddFunctionCalls.C
#
#testLoopOptimization: loopOptimization
#	$(VALGRIND) ./loopOptimization -c $(srcdir)/inputCode_LoopOptimization.C
#
#testIdentityTranslator: identityTranslator
#	./identityTranslator -c $(srcdir)/inputCode_IdentityTranslator.C
#
#testCharmSupport: charmSupport
#	./charmSupport -c $(srcdir)/inputCode_ExampleCharmSupport.C
#
#testBuildCFG: buildCFG
##	./buildCFG $(srcdir)/inputCode_1.C
##	./buildCFG $(srcdir)/inputCode_CallGraphAnalysis.C
#	./buildCFG $(srcdir)/inputCode_ControlFlowGraphAnalysis.C
#	@echo "Building postscript file from foo.dot ..."
##	$(top_srcdir)/scripts/dotgenLetterPDF controlFlowGraph.dot controlFlowGraph.ps
##	$(top_srcdir)/scripts/dotgenLetterPDF foo.dot controlFlowGraph.ps
#	$(top_srcdir)/scripts/dotgenLetterPDF main.dot controlFlowGraph.pdf
#
#testBuildCFG_testfile1: buildCFG
#	./buildCFG $(srcdir)/inputCode_ControlFlowGraphAnalysis_testfile1.c
#	@echo "Building postscript file from foo.dot ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF testCFG.dot controlFlowGraph_testfile1.pdf
#
#testVirtualCFG: virtualCFG
#	./virtualCFG $(srcdir)/inputCode_ControlFlowGraphAnalysis.C > vcfg.dot || (rm -f vcfg.dot; exit 1)
#	$(top_srcdir)/scripts/dotgenLetterPDF vcfg.dot vcfg.pdf
#
## Both this and the testCallGraphAnalysis rule generate a file called callGraph.ps
#testBuildCG: buildCG
#	./buildCG $(srcdir)/inputCode_BuildCG.C
#	@echo "Building postscript file from DATABASE.dot ..."
##	$(top_srcdir)/scripts/dotgenLetterPDF controlFlowGraph.dot controlFlowGraph.ps
#	$(top_srcdir)/scripts/dotgenLetterPDF callGraph.dot callGraph.pdf
#
#testClassHierarchyGraph: classHierarchyGraph
#	./classHierarchyGraph $(srcdir)/inputCode_ClassHierarchyGraph.C
#	@echo "Building postscript file from DATABASE.dot ..."
##	$(top_srcdir)/scripts/dotgenLetterPDF controlFlowGraph.dot controlFlowGraph.ps
#	$(top_srcdir)/scripts/dotgenLetterPDF classHier.dot classHierarchyGraph.pdf
#
#testCodeCoverage: codeCoverage
#	./codeCoverage -c $(srcdir)/inputCode_ExampleCodeCoverage.C
#
#testCodeCoverageIBMExample: codeCoverage
#	./codeCoverage -c $(srcdir)/inputCode_ExampleCodeCoverageIBMExample.C
#
#testChangeFunction: changeFunction
#	./changeFunction -rose:verbose 0 -c $(srcdir)/inputCode_ChangeFunctionExample.C
#
#testInterveneAtVariables: interveneAtVariables
#	./interveneAtVariables -rose:verbose 0 -c $(srcdir)/inputCode_interveneAtVariables.C
#
#testCallGraphAnalysis: callGraphAnalysis
#	./callGraphAnalysis -c $(srcdir)/inputCode_CallGraphAnalysis.C
#	@echo "Building postscript file from inputCode_CallGraphAnalysis.C.callgraph.dot ..."
##	$(top_srcdir)/scripts/dotgenLetterPDF callgraph.dot callGraph.ps
##	$(top_srcdir)/scripts/dotgen callgraph.dot callGraph.ps
#	$(top_srcdir)/scripts/dotgenLetterPDF callGraph.dot callGraph.pdf
#
#testTemplateExample1: identityTranslator
#	./identityTranslator -c $(srcdir)/inputCode_templateExample1.C
#
#testTemplateExample2: identityTranslator
#	./identityTranslator -c $(srcdir)/inputCode_templateExample2.C
#
#testDataBaseExample1: dataBaseUsage
#	./dataBaseUsage -c $(srcdir)/inputCode_dataBaseExample1.C
#
#testLoopNestingInfoProcessing: loopNestingInfoProcessing
#	./loopInfoProcessing -c $(srcdir)/inputCode_loopNestingInfoProcessing.C
#
#loopOptimizationOutputFiles = \
#   rose_inputCode_LoopOptimization_blocking.C \
#   rose_inputCode_LoopOptimization_fusion.C \
#   rose_inputCode_LoopOptimization_mm.C \
#   rose_inputCode_LoopOptimization_dgemm.C \
#   rose_inputCode_LoopOptimization_lufac.C \
#   rose_inputCode_LoopOptimization_tridvpk.C
#
#testLoopOptimization: loopProcessor $(loopOptimizationOutputFiles)
#
#testInheritedAttributeTraversal: inheritedAttributeTraversal
#	$(VALGRIND) ./inheritedAttributeTraversal -c $(srcdir)/inputCode_ExampleTraversals_1.C
#
#testPersistantAttributes: persistantAttributes
#	./persistantAttributes -c $(srcdir)/inputCode_ExampleTraversals_2.C
#
## TAU_INCLUDES = -I/home/dquinlan/TAU/tau-2.14.8/include
#TAU_INCLUDES = -I$(srcdir)/TAU_INCLUDE_DIR
#testTauInstrumenter: tauInstrumenter
#	./tauInstrumenter $(TAU_INCLUDES) -c $(srcdir)/inputCode_tauInstrumenter.C
#
#testTypeTransformation: typeTransformation
#	./typeTransformation -c $(srcdir)/inputCode_typeTransformation.C
#
#testAstFileIO_GenerateBinaryFile: astFileIO_GenerateBinaryFile
#	./astFileIO_GenerateBinaryFile -c $(srcdir)/inputCode_astFileIO_GenerateBinaryFile.C
#
#testAstFileIO_ReadMultipleASTs: astFileIO_ReadMultipleASTs
#	./astFileIO_GenerateBinaryFile -c $(srcdir)/inputCode_astFileIO_GenerateBinaryFile_1.C
#	./astFileIO_GenerateBinaryFile -c $(srcdir)/inputCode_astFileIO_GenerateBinaryFile_2.C
#	./astFileIO_GenerateBinaryFile -c $(srcdir)/inputCode_astFileIO_GenerateBinaryFile_3.C
#	./astFileIO_ReadMultipleASTs inputCode_astFileIO_GenerateBinaryFile_1 inputCode_astFileIO_GenerateBinaryFile_2 inputCode_astFileIO_GenerateBinaryFile_3
#
#testTraverseIRnodeTypes: traverseIRnodeTypes
#	$(VALGRIND) ./traverseIRnodeTypes -c $(srcdir)/inputCode_ExampleTraversals_3.C
#
#testGeneratingUniqueNamesFromDeclaration: generatingUniqueNamesFromDeclaration
#	./generatingUniqueNamesFromDeclaration -c $(srcdir)/inputCode_generatingUniqueNamesFromDeclaration.C
#
#testGeneratingUniqueNamesFromDeclaration2: generatingUniqueNamesFromDeclaration
#	./generatingUniqueNamesFromDeclaration -c $(srcdir)/inputCode_generatingUniqueNamesFromDeclaration2.C
#
#testSymbolTableHandling: symbolTableHandling
#	./symbolTableHandling -c $(srcdir)/inputCode_testsymbolTableHandling.C
#
#testScopeInformation: scopeInformation
#	./scopeInformation -c $(srcdir)/inputCode_scopeInformation.C
#
#testCustomGraphGeneration: customGraphGeneration customGraphGenerationWithFiltering
#	./customGraphGeneration
#	@echo "Building postscript file from foo.dot ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF customGraph.dot customGraph.pdf
#	./customGraphGenerationWithFiltering
#	$(top_srcdir)/scripts/dotgenLetterPDF customGraphWithFiltering.dot customGraphWithFiltering_test.pdf
#
#testInlineTransformations: inlineTransformations
#	./inlineTransformations -c $(srcdir)/inputCode_inlineTransformations.C
#
#testAddComments: addComments
#	./addComments -c $(srcdir)/inputCode_addComments.C
#
#testAddArbitraryTextForUnparser:
#	./addArbitraryTextForUnparser -c $(srcdir)/inputCode_addArbitraryTextForUnparser.C
#
#testCollectComments: collectComments
#	./collectComments -rose:collectAllCommentsAndDirectives -I$(srcdir) -c $(srcdir)/inputCode_collectComments.C
#
#testCollectCommentsSkipHeaders: collectComments
#	./collectComments -I$(srcdir) -c $(srcdir)/inputCode_collectComments.C
#
#testCollectDefineDirectives: collectDefineDirectives
#	./collectDefineDirectives -rose:collectAllCommentsAndDirectives -I$(srcdir) -c $(srcdir)/inputCode_collectComments.C
#
#testPartialRedundancyElimination: partialRedundancyElimination
#	./partialRedundancyElimination -c $(srcdir)/inputCode_partialRedundancyElimination.C
#
#testCodeGenerationFormatControl: codeGenerationFormatControl
#	./codeGenerationFormatControl -c $(srcdir)/inputCode_codeGenerationFormatControl.C
#
#testDataFlowGraph: dataFlowGraph
#	./dataFlowGraph -c $(srcdir)/inputCode_dataFlowGraphAnalysis.C
#	@echo "Building postscript file from inputCode_dataFlowGraphAnalysis.C.dot ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_dataFlowGraphAnalysis.C.dot inputCode_dataFlowGraphAnalysis.pdf
#
#testDataFlowGraph_testfile1: dataFlowGraph
#	./dataFlowGraph -c $(srcdir)/inputCode_dataFlowGraphAnalysis_testfile1.C
#	@echo "Building postscript file from inputCode_dataFlowGraphAnalysis.C.dot ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_dataFlowGraphAnalysis_testfile1.C.dot inputCode_dataFlowGraphAnalysis_testfile1.pdf
#
#testGraphTraversal: graphTraversal
#	./graphTraversal
#
#testWholeGraphAST: wholeGraphAST
#	./wholeGraphAST $(srcdir)/inputCode_wholeGraphAST.C
#	@echo "Building postscript file from wholeGraphAST.dot and wholeGraphASTPool.dot ..."
##	$(top_srcdir)/scripts/dotgenLetterPDF wholeGraphAST.dot wholeGraphAST.ps
#	$(top_srcdir)/scripts/dotgenLetterPDF wholeGraphAST1.dot wholeGraphAST.pdf
##	$(top_srcdir)/scripts/dotgenLetterPDF wholeGraphASTPool.dot wholeGraphASTPool.ps
#	$(top_srcdir)/scripts/dotgenLetterPDF wholeGraphASTPool1.dot wholeGraphASTPool.pdf
#
#testCustomGraphGenerationWithColors: customGraphGenerationWithColors
#	./customGraphGenerationWithColors $(srcdir)/inputCode_customGraphGenerationWithColors.C
#	@echo "Building postscript file from customGraphGenerationWithColors.dot and customGraphGenerationWithColors.dot ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF customGraphGenerationWithColors.dot customGraphGenerationWithColors.pdf
#
#testConstantFoldingTransformation: constantFoldingTransformation
##	./customGraphGenerationWithColors $(srcdir)/inputCode_constantFoldingTransformation.C
##	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_constantFoldingTransformation.C.dot constantFoldingTransformation.ps
#	./constantFoldingTransformation -c $(srcdir)/inputCode_constantFoldingTransformation.C
#
## This tests the generation f an intrmediate example in the tutorial
#testConstantUnFoldingTransformation: constantUnFoldingTransformation
#	./constantUnFoldingTransformation -c $(srcdir)/inputCode_constantUnFoldingTransformation.C
#
#testSelectedFileTranslationFilterPath: selectedFileTranslation
#	./selectedFileTranslation -rose:verbose 0 -skipFile:path /home/dquinlan/ROSE -c $(srcdir)/inputCode_addComments.C
#
#testSelectedFileTranslation: selectedFileTranslation
#	./selectedFileTranslation -rose:verbose 0 -skipFile:path /nonDirectory -c $(srcdir)/inputCode_addComments.C
#
#testMultipleFiles: selectedFileTranslation
#	$(VALGRIND) ./selectedFileTranslation -rose:verbose 0 -c $(srcdir)/inputCode_addComments.C $(srcdir)/inputCode_wholeGraphAST.C $(srcdir)/inputCode_constantFoldingTransformation.C $(srcdir)/inputCode_dataFlowGraphAnalysis.C $(srcdir)/inputCode_partialRedundancyElimination.C $(srcdir)/inputCode_scopeInformation.C $(srcdir)/inputCode_ExampleTraversals_4.C
#
#testVirtualFunctionIdentification: virtualFunctionIdentification
#	./virtualFunctionIdentification -c $(srcdir)/inputCode_virtualFunctionIdentification.C
#
#
#testGraphGenerationOfSubtree: graphGenerationOfSubtree
#	@echo "Generating dot file ..."
#	./graphGenerationOfSubtree $(srcdir)/inputCode_graphGenerationOfSubtree.C
#	@echo "Generate postscript file (default name: test.pdf) from dot file ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_graphGenerationOfSubtree.subtree.dot inputCode_2.pdf
#
#testSwitchWithoutDefault: switchWithoutDefault
#	./switchWithoutDefault -rose:verbose 0 -I$(srcdir) -c $(srcdir)/inputCode_switchWithoutDefault.C
#
#testRoseSwitchWithoutDefault: switchWithoutDefault
#	./switchWithoutDefault -rose:verbose 0 -I.. $(INCLUDES) -c ../src/frontend/SageIII/Cxx_Grammar.C
#
#testAddStruct: addStruct
#	./addStruct -rose:verbose 0 -I.. $(INCLUDES) -c $(srcdir)/inputCode_addStruct.C
#
#testsnprintf: sprintf_to_snprintf
##	./sprintf_to_snprintf -rose:verbose 0 -I.. $(INCLUDES) -c $(srcdir)/inputCode_sprintf_to_snprintf.C
#	./sprintf_to_snprintf -rose:verbose 0 -c $(srcdir)/inputCode_sprintf_to_snprintf.C
#
#testSeedBugsExample_arrayIndexing: seedBugsExample_arrayIndexing
#	./seedBugsExample_arrayIndexing -rose:verbose 0 -c $(srcdir)/inputCode_seedBugsExample_arrayIndexing.C
#
#testVolatileTypeModifier: volatileTypeModifier
#	./volatileTypeModifier -rose:verbose 0 -c $(srcdir)/inputCode_volatileTypeModifier.C
#
#if ROSE_USE_DWARF
## This should only be tested if Dwarf is in use, and it should not 
## be used on the Mac OSX system because we build the executable and
## ROSE does not yet handle the Mach-O executable i386 binary file format.
#inputCode_dwarfAnalysis:
#	g++ -g3 -o inputCode_dwarfAnalysis $(srcdir)/inputCode_dwarfAnalysis.C
#else
#inputCode_dwarfAnalysis:
##	DQ (2/12/2009): This fails when make check is run and the file ($(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/i386-nologin)
##	is not available (e.g. external distributions).
##	cp $(top_srcdir)/developersScratchSpace/Dan/Disassembler_tests/i386-nologin inputCode_dwarfAnalysis
#	cp $(srcdir)/buffer2.bin inputCode_dwarfAnalysis
#endif
#
#
#testDwarfAnalysis: dwarfAnalysis inputCode_dwarfAnalysis 
## Note: different stages of AST representation and visualization.
##	./dwarfAnalysis -c $(srcdir)/inputCode_dwarfAnalysis.C
##	./dwarfAnalysis -rose:visualize_dwarf_only inputCode_dwarfAnalysis
##	./dwarfAnalysis -rose:visualize_executable_file_format_skip_symbols inputCode_dwarfAnalysis
##	./dwarfAnalysis -rose:read_executable_file_format_only inputCode_dwarfAnalysis
##	./dwarfAnalysis -rose:read_executable_file_format_only -rose:visualize_executable_file_format_skip_symbols inputCode_dwarfAnalysis
#	./dwarfAnalysis -rose:visualize_dwarf_only -rose:read_executable_file_format_only inputCode_dwarfAnalysis
#
#testDwarfInstructionAddressToSourceLineAnalysis: dwarfInstructionAddressToSourceLineAnalysis inputCode_dwarfAnalysis
##	./dwarfAnalysis -rose:verbose 0 -c $(srcdir)/inputCode_dwarfAnalysis.C
##	./dwarfInstructionAddressToSourceLineAnalysis -rose:visualize_dwarf_only inputCode_dwarfAnalysis
#	./dwarfInstructionAddressToSourceLineAnalysis inputCode_dwarfAnalysis
#
#
## #####################################
## Output files use in tutorial
## #####################################
#
#traversalOutput = \
#   visitorTraversal.out \
#   prePostTraversal.out \
#   combinedTraversals.out \
#   sharedMemoryTraversals.out \
#   classicObjectOrientedVisitorPatternTraversal.out \
#   inheritedAttributeTraversal.out \
#   synthesizedAttributeTraversal.out \
#   inheritedAndSynthesizedAttributeTraversal.out \
#   accumulatorAttributeTraversal.out \
#   persistantAttributes.out \
#   visitorMemoryPoolTraversal.out \
#   classicObjectOrientedVisitorPatternMemoryPoolTraversal.out \
#   traverseIRnodeTypes.out \
#   nestedTraversal.out
#
#
#   #rose_inputCode_varSubstCopy.C , temporary disabled this test, don't go into patch!,Liao
## .out suffix may cause conflicts with a temporary file used by makefile/libtool
## using a different name is highly recommended!! Liao, 10/15/2008
#generatedCodeExamples = \
#   rose_inputCode_inlineTransformations.C \
#   rose_inputCode_partialRedundancyElimination.C \
#   rose_inputCode_tauInstrumenter.C \
#   inputCode_ASTGraphGenerator.pdf \
#   inputCode_AST_PDF_Generator.pdf \
#   inputCode_ASTWholeGraphGenerator_small.pdf \
#   inputCode_ASTWholeGraphGenerator_large.pdf \
#   rose_inputCode_IdentityTranslator.C \
#   queryLibrary.out \
#   nestedQuery.out \
#   $(traversalOutput) \
#   rose_inputCode_AddFunctionDeclaration.C \
#   rose_inputCode_AddFunctionDeclaration2.C \
#   rose_inputCode_AddFunctionDeclaration3.C \
#   rose_inputCode_AddVariableDeclaration.C \
#   rose_inputCode_AddAssignmentStmt.C \
#   abstractHandle1.outx \
#   abstractHandle2.outx \
#   rose_inputCode_AddExpression.C \
#   rose_inputCode_AddFunctionCalls.C \
#   rose_inputCode_InstrumentationTranslator.C \
#   rose_inputCode_instrumentEndOfFunction.C \
#   loopRecognition.out \
#   reductionRecognition.out \
#   typeInfoFromFunctionParameters.out \
#   resolveOverloadedFunction.out \
#   templateParameter.out \
#   rose_inputCode_LoopOptimization_blocking.C \
#   rose_inputCode_LoopOptimization_fusion.C \
#   rose_inputCode_LoopOptimization_mm.C \
#   rose_inputCode_LoopOptimization_dgemm.C \
#   rose_inputCode_LoopOptimization_lufac.C \
#   rose_inputCode_LoopOptimization_tridvpk.C \
#   controlFlowGraph.pdf \
#   vcfg.pdf \
#   debuggingIRnodeToString.out \
#   debuggingSourceCodePositionInformation.out \
#   commandlineProcessing.out \
#   charmSupport.out \
#   codeCoverage.out \
#   callGraph.pdf \
#   classHierarchyGraph.pdf \
#   rose_inputCode_templateExample1.C \
#   rose_inputCode_templateExample2.C \
#   dataBaseExample1.out \
#   loopNestingInfoProcessing.out \
#   loopProcessor.C.aa loopProcessor.C.ab \
#   generatingUniqueNamesFromDeclaration.out \
#   generatingUniqueNamesFromDeclaration2.out \
#   symbolTableHandling.out \
#   scopeInformation.out \
#   rose_inputCode_typeTransformation.C \
#   astFileIO_GenerateBinaryFile.out \
#   customGraph.pdf \
#   customGraphWithFiltering.pdf \
#   graphTraversal.pdf \
#   rose_inputCode_addComments.C \
#   rose_inputCode_addArbitraryTextForUnparser.C \
#   rose_inputCode_codeGenerationFormatControl.C \
#   rose_inputCode_addStruct.C \
#   traversalShortCircuit.out \
#   wholeGraphAST.pdf \
#   collectCommentsSkipHeaderFiles.out \
#   collectCommentsAcrossHeaderFiles.out \
#   collectDefineDirectives.out \
#   defuseAnalysis_pic1.pdf \
#   binaryAnalysis_pic1.pdf \
#   binaryAnalysis_pic3.pdf \
#   rose_inputCode_seedBugsExample_arrayIndexing.C \
#   rose_inputCode_VolatileTypeModifier.C \
#   inputCode_dwarfAnalysis.pdf \
#   inputCode_dwarfAnalysis.out
#
## #####################################
## Output file generation rules
## #####################################
#
#inputCode_ASTGraphGenerator.pdf: ASTGraphGenerator
#	@echo "Generating required documentation for tutorial (inputCode_ASTGraphGenerator.pdf) ..."
## Build smaller versions of these files (so that they fit onto two separate pages (within the tutorial))
#	split -80 $(srcdir)/addFunctionDeclaration.C addFunctionDeclaration.
#	split -80 $(srcdir)/inheritedAndSynthesizedAttributeTraversal.C inheritedAndSynthesizedAttributeTraversal.
#	split -64 $(srcdir)/loopRecognition.C loopRecognition.
#	./ASTGraphGenerator $(ROSE_FLAGS) $(srcdir)/inputCode_ASTGraphGenerator.C
#	@echo "Generate postscript file (default name: test.pdf) from dot file ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_ASTGraphGenerator.C.dot inputCode_ASTGraphGenerator.pdf
#
#inputCode_AST_PDF_Generator.pdf: AST_PDF_Generator
#	@echo "Generating required documentation for tutorial (inputCode_AST_PDF_Generator.pdf) ..."
#	@echo "Reusing the same inputCode_ASTGraphGenerator.C as for the DOT graph (but copy to new name) ..."
#	cp $(srcdir)/inputCode_ASTGraphGenerator.C inputCode_AST_PDF_Generator.C
#	./AST_PDF_Generator $(ROSE_FLAGS) inputCode_AST_PDF_Generator.C
#	cp -f inputCode_AST_PDF_Generator.C.pdf inputCode_AST_PDF_Generator.pdf
#
#inputCode_ASTWholeGraphGenerator_small.pdf: wholeASTGraphGenerator
#	@echo "Generating dot file of whole AST ..."
#	./wholeASTGraphGenerator $(srcdir)/inputCode_wholeAST_1.C
#	@echo "Generate pdf file (default name: test.pdf) from dot file ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_wholeAST_1_WholeAST.dot inputCode_ASTWholeGraphGenerator_small.pdf
#
#inputCode_ASTWholeGraphGenerator_large.pdf: wholeASTGraphGenerator
#	@echo "Generating dot file of whole AST ..."
#	./wholeASTGraphGenerator $(srcdir)/inputCode_wholeAST_2.C
#	@echo "Generate pdf file (default name: test.pdf) from dot file ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_wholeAST_2_WholeAST.dot inputCode_ASTWholeGraphGenerator_large.pdf
#
#rose_inputCode_IdentityTranslator.C: identityTranslator
#	@echo "Generate executable from compilation of input code (identityTranslator) ..."
#	./identityTranslator $(ROSE_FLAGS) -c $(srcdir)/inputCode_IdentityTranslator.C
#
#queryLibrary.out: queryLibraryExample
#	@echo "Generate executable from compilation of input code (queryLibraryExample) ..."
#	./queryLibraryExample $(ROSE_FLAGS) -c $(srcdir)/inputCode_QueryLibrary.C > queryLibrary.out
#
#nestedQuery.out: nestedQueryExample
#	@echo "Generate executable from compilation of input code (nestedQueryExample) ..."
#	./nestedQueryExample $(ROSE_FLAGS) -c $(srcdir)/inputCode_NestedQuery.C > nestedQuery.out
#
## DQ (9/7/2006): These need separate rules so that when using "make -j" each is run by at most only one process.
## traversalExecutables = \
##   visitorTraversal \
##   classicObjectOrientedVisitorPatternTraversal \
##   inheritedAttributeTraversal \
##   synthesizedAttributeTraversal \
##   inheritedAndSynthesizedAttributeTraversal \
##   accumulatorAttributeTraversal \
##   persistantAttributes \
##   visitorMemoryPoolTraversal \
##   classicObjectOrientedVisitorPatternMemoryPoolTraversal \
##   nestedTraversal
## $(traversalOutput): $(traversalExecutables)
##	@echo "Generate executable from compilation of input code (visitorTraversal) ..."
##	./visitorTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_5.C > visitorTraversal.out
##	@echo "Generate executable from compilation of input code (classicObjectOrientedVisitorPatternTraversal) ..."
##	./classicObjectOrientedVisitorPatternTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_6.C > classicObjectOrientedVisitorPatternTraversal.out
##	@echo "Generate executable from compilation of input code (inheritedAttributeTraversal) ..."
##	./inheritedAttributeTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_7.C > inheritedAttributeTraversal.out
##	@echo "Generate executable from compilation of input code (synthesizedAttributeTraversal) ..."
##	./synthesizedAttributeTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_8.C > synthesizedAttributeTraversal.out
##	@echo "Generate executable from compilation of input code (inheritedAndSynthesizedAttributeTraversal) ..."
##	./inheritedAndSynthesizedAttributeTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_9.C > inheritedAndSynthesizedAttributeTraversal.out
##	@echo "Generate executable from compilation of input code (accumulatorAttributeTraversal) ..."
##	./accumulatorAttributeTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_10.C > accumulatorAttributeTraversal.out
##	@echo "Generate executable from compilation of input code (nestedTraversal) ..."
##	./nestedTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_11.C > nestedTraversal.out
##	@echo "Generate executable from compilation of input code (persistantAttributes) ..."
##	./persistantAttributes $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_12.C > persistantAttributes.out
##	@echo "Generate executable from compilation of input code (visitorMemoryPoolTraversal) ..."
##	./visitorMemoryPoolTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_13.C > visitorMemoryPoolTraversal.out
##	@echo "Generate executable from compilation of input code (classicObjectOrientedVisitorPatternMemoryPoolTraversal) ..."
##	./classicObjectOrientedVisitorPatternMemoryPoolTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_14.C > classicObjectOrientedVisitorPatternMemoryPoolTraversal.out
##	@echo "Generate executable from compilation of input code (traverseIRnodeTypes) ..."
##	./traverseIRnodeTypes $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_15.C > traverseIRnodeTypes.out
#
## DQ (9/7/2006): These need separate rules so that when using "make -j" each is run by at most only one process.
#visitorTraversal.out: visitorTraversal
#	@echo "Generate executable from compilation of input code (visitorTraversal) ..."
#	./visitorTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_5.C > visitorTraversal.out
#
#prePostTraversal.out: prePostTraversal
#	@echo "Generate executable from compilation of input code (prePostTraversal) ..."
#	./prePostTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_16.C > prePostTraversal.out
#
#combinedTraversals.out: combinedTraversals
#	@echo "Generate executable from compilation of input code (combinedTraversals) ..."
#	./combinedTraversals $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_17.C > combinedTraversals.out
#
## DQ (3/21/2009): sharedMemoryTraversals fails to compile under Cygwin.
#if ROSE_BUILD_OS_IS_CYGWIN
#sharedMemoryTraversals.out:
#	@echo "Commented out shared memory tests for Cygwin (sharedMemoryTraversals fails to compile) ..."
#	touch sharedMemoryTraversals.out
#else
#sharedMemoryTraversals.out: sharedMemoryTraversals
#	@echo "Generate executable from compilation of input code (sharedMemoryTraversals) ..."
#	./sharedMemoryTraversals $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_18.C > sharedMemoryTraversals.out
#endif
#
#classicObjectOrientedVisitorPatternTraversal.out: classicObjectOrientedVisitorPatternTraversal
#	@echo "Generate executable from compilation of input code (classicObjectOrientedVisitorPatternTraversal) ..."
#	./classicObjectOrientedVisitorPatternTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_6.C > classicObjectOrientedVisitorPatternTraversal.out
#
#inheritedAttributeTraversal.out: inheritedAttributeTraversal
#	@echo "Generate executable from compilation of input code (inheritedAttributeTraversal) ..."
#	./inheritedAttributeTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_7.C > inheritedAttributeTraversal.out
#
#synthesizedAttributeTraversal.out: synthesizedAttributeTraversal
#	@echo "Generate executable from compilation of input code (synthesizedAttributeTraversal) ..."
#	./synthesizedAttributeTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_8.C > synthesizedAttributeTraversal.out
#
#inheritedAndSynthesizedAttributeTraversal.out: inheritedAndSynthesizedAttributeTraversal
#	@echo "Generate executable from compilation of input code (inheritedAndSynthesizedAttributeTraversal) ..."
#	./inheritedAndSynthesizedAttributeTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_9.C > inheritedAndSynthesizedAttributeTraversal.out
#
#accumulatorAttributeTraversal.out: accumulatorAttributeTraversal
#	@echo "Generate executable from compilation of input code (accumulatorAttributeTraversal) ..."
#	./accumulatorAttributeTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_10.C > accumulatorAttributeTraversal.out
#
#nestedTraversal.out: nestedTraversal
#	@echo "Generate executable from compilation of input code (nestedTraversal) ..."
#	./nestedTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_11.C > nestedTraversal.out
#
#persistantAttributes.out: persistantAttributes
#	@echo "Generate executable from compilation of input code (persistantAttributes) ..."
#	./persistantAttributes $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_12.C > persistantAttributes.out
#
#visitorMemoryPoolTraversal.out: visitorMemoryPoolTraversal
#	@echo "Generate executable from compilation of input code (visitorMemoryPoolTraversal) ..."
#	./visitorMemoryPoolTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_13.C > visitorMemoryPoolTraversal.out
#
#classicObjectOrientedVisitorPatternMemoryPoolTraversal.out: classicObjectOrientedVisitorPatternMemoryPoolTraversal
#	@echo "Generate executable from compilation of input code (classicObjectOrientedVisitorPatternMemoryPoolTraversal) ..."
#	./classicObjectOrientedVisitorPatternMemoryPoolTraversal $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_14.C > classicObjectOrientedVisitorPatternMemoryPoolTraversal.out
#
#traverseIRnodeTypes.out: traverseIRnodeTypes
#	@echo "Generate executable from compilation of input code (traverseIRnodeTypes) ..."
#	./traverseIRnodeTypes $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleTraversals_15.C > traverseIRnodeTypes.out
#
#
#rose_inputCode_AddFunctionDeclaration.C: addFunctionDeclaration
#	@echo "Generate executable from compilation of input code (addFunctionDeclaration) ..."
#	./addFunctionDeclaration $(ROSE_FLAGS) -c $(srcdir)/inputCode_AddFunctionDeclaration.C
#
#rose_inputCode_AddFunctionDeclaration2.C: addFunctionDeclaration2
#	@echo "Generate executable from compilation of input code (addFunctionDeclaration2) ..."
#	./addFunctionDeclaration2 $(ROSE_FLAGS) -c $(srcdir)/inputCode_AddFunctionDeclaration2.C
#
#rose_inputCode_AddFunctionDeclaration3.C: addFunctionDeclaration3
#	@echo "Generate executable from compilation of input code (addFunctionDeclaration3) ..."
#	./addFunctionDeclaration3 $(ROSE_FLAGS) -c $(srcdir)/inputCode_AddFunctionDeclaration3.C
#
#rose_inputCode_AddVariableDeclaration.C: addVariableDeclaration
#	@echo "Generate executable from compilation of input code (addVariableDeclaration) ..."
#	./addVariableDeclaration $(ROSE_FLAGS) -c $(srcdir)/inputCode_AddVariableDeclaration.C
#
#rose_inputCode_AddVariableDeclaration2.C: addVariableDeclaration2
#	@echo "Generate executable from compilation of input code (addVariableDeclaration2) ..."
#	./addVariableDeclaration2 $(ROSE_FLAGS) -c $(srcdir)/inputCode_AddVariableDeclaration2.C
#
#abstractHandle1.outx: abstractHandle1
#	@echo "Generate executable from compilation of input code (abstractHandle1) ..."
#	./abstractHandle1 $(ROSE_FLAGS) -c $(srcdir)/inputCode_AbstractHandle1.cpp | fold > abstractHandle1.outx
#
#abstractHandle2.outx: abstractHandle2
#	@echo "Generate executable from compilation of input code (abstractHandle2) ..."
#	./abstractHandle2 $(ROSE_FLAGS) -c $(srcdir)/inputCode_AbstractHandle2.cpp | fold > abstractHandle2.outx
#
#rose_inputCode_AddAssignmentStmt.C: addAssignmentStmt
#	@echo "Generate executable from compilation of input code (addAssignmentStmt) ..."
#	./addAssignmentStmt $(ROSE_FLAGS) -c $(srcdir)/inputCode_AddAssignmentStmt.C
#
#rose_inputCode_AddExpression.C: addExpression
#	@echo "Generate executable from compilation of input code (addExpression) ..."
#	./addExpression $(ROSE_FLAGS) -c $(srcdir)/inputCode_AddExpression.C
#
#rose_inputCode_AddFunctionCalls.C: addFunctionCalls
#	@echo "Generate executable from compilation of input code (addFunctionCalls) ..."
#	./addFunctionCalls $(ROSE_FLAGS) -c $(srcdir)/inputCode_AddFunctionCalls.C
#
#rose_inputCode_InstrumentationTranslator.C: instrumentationExample
#	@echo "Generate executable from compilation of input code ..."
#	./instrumentationExample $(ROSE_FLAGS) -c $(srcdir)/inputCode_InstrumentationTranslator.C
#
#rose_inputCode_instrumentEndOfFunction.C: instrumentEndOfFunction
#	@echo "Generate executable from compilation of input code ..."
#	./instrumentEndOfFunction $(ROSE_FLAGS) -c $(srcdir)/inputCode_instrumentEndOfFunction.C
#reductionRecognition.out: reductionRecognition
#	@echo "Generate executable from compilation of input code ..."
#	./reductionRecognition $(ROSE_FLAGS) -c $(srcdir)/inputCode_reductionRecognition.C >reductionRecognition.out
#
#loopRecognition.out: loopRecognition
#	@echo "Generate executable from compilation of input code (loopRecognition) ..."
#	./loopRecognition $(ROSE_FLAGS) -c $(srcdir)/inputCode_LoopRecognition.C > loopRecognition.out
#
#typeInfoFromFunctionParameters.out: typeInfoFromFunctionParameters
#	@echo "Generate executable from compilation of input code (typeInfoFromFunctionParameters) ..."
#	./typeInfoFromFunctionParameters $(ROSE_FLAGS) -c $(srcdir)/inputCode_TypeInfoFromFunctionParameters.C > typeInfoFromFunctionParameters.out
#
#resolveOverloadedFunction.out: resolveOverloadedFunction
#	@echo "Generate executable from compilation of input code (resolveOverloadedFunction) ..."
#	./resolveOverloadedFunction $(ROSE_FLAGS) -c $(srcdir)/inputCode_ResolvingOverloadedFunctions.C > resolveOverloadedFunction.out
#
#templateParameter.out: templateParameter $(srcdir)/inputCode_TemplateParameterExtration.C
#	@echo "Generate executable from compilation of input code (templateParameter) ..."
#	./templateParameter $(ROSE_FLAGS) -c $(srcdir)/inputCode_TemplateParameterExtration.C > templateParameter.out
#
#rose_inputCode_LoopOptimization_blocking.C: loopOptimization
#	@echo "Generate executable from compilation of input code (loopOptimization) ..."
#	$(VALGRIND) ./loopOptimization $(ROSE_FLAGS) -bk1 -fs0 -c $(srcdir)/inputCode_LoopOptimization_blocking.C
#
#loopProcessor.C.aa loopProcessor.C.ab: $(srcdir)/LoopProcessor.C
#	split -42 $(srcdir)/LoopProcessor.C loopProcessor.C.
#
#rose_inputCode_LoopOptimization_fusion.C: loopOptimization
#	@echo "Generate executable from compilation of input code (loopOptimization) ..."
#	./loopOptimization $(ROSE_FLAGS) -fs2 -c $(srcdir)/inputCode_LoopOptimization_fusion.C
#
#rose_inputCode_LoopOptimization_mm.C: loopProcessor
#	@echo "Generate executable from compilation of input code (loopOptimization) ..."
##	./loopOptimization -bk1 -fs0 -c $(srcdir)/inputCode_LoopOptimization_mm.C
## DQ (1/2/2008): This used to work, but it is part of what Qing is fixing.
#	$(VALGRIND) ./loopProcessor $(ROSE_FLAGS) -bk1 -fs0 -c $(srcdir)/inputCode_LoopOptimization_mm.C
#
#rose_inputCode_LoopOptimization_dgemm.C: loopProcessor
#	@echo "Generate executable from compilation of input code (loopOptimization) ..."
#	./loopProcessor $(ROSE_FLAGS) -bk1 -unroll nvar 16 -c $(srcdir)/inputCode_LoopOptimization_dgemm.C
#
#rose_inputCode_LoopOptimization_lufac.C: loopProcessor
#	@echo "Generate executable from compilation of input code (loopOptimization) ..."
#	./loopProcessor $(ROSE_FLAGS) -bk1 -fs0  -splitloop -annotation -c $(srcdir)/inputCode_LoopOptimization_lufac.C
#
#rose_inputCode_LoopOptimization_tridvpk.C: loopProcessor
#	@echo "Generate executable from compilation of input code (loopOptimization) ..."
#	./loopProcessor $(ROSE_FLAGS) -fs2 -ic1 -opt 1 -c $(srcdir)/inputCode_LoopOptimization_tridvpk.C
#
#controlFlowGraph.pdf: buildCFG
#	@echo "Building an example Control Flow Graph (buildCFG) ..."
#	./buildCFG $(ROSE_FLAGS) $(srcdir)/inputCode_ControlFlowGraphAnalysis.C
#	@echo "Building postscript file from main.dot ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF main.dot controlFlowGraph.pdf
#
#vcfg.pdf: virtualCFG
#	@echo "Building an example virtual Control Flow Graph (virtualCFG) ..."
#	./virtualCFG $(ROSE_FLAGS) $(srcdir)/inputCode_ControlFlowGraphAnalysis.C > vcfg.dot || (rm -f vcfg.dot; exit 1)
#	$(top_srcdir)/scripts/dotgenLetterPDF vcfg.dot vcfg.pdf
#
#defuseAnalysis_pic1.pdf: defuseAnalysis
#	@echo "Building an example DefUse ..."
#	./defuseAnalysis $(ROSE_FLAGS) $(srcdir)/input_defuseAnalysis.C
#	@echo "DefUseAnalysis: Building postscript file from foo.dot ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF dfa.dot defuseAnalysis_pic1.pdf
#
#
#
#binaryAnalysis_pic1.pdf: binaryAnalysis
#	@echo "Building an example BinaryAnalysis ..."
## This does currently not work because we don't support 64bit binaries
##	./binaryAnalysis bin_example
## trick is to load a 32bit binary
#	cp $(srcdir)/buffer2.bin ./bin_example
#	./binaryAnalysis bin_example
#	@echo "BinaryAnalysis: Building assembly listing for binary (rose_bin_example.s.a) ..."
#	split -42 rose_bin_example.s rose_bin_example.s.
#	@echo "BinaryAnalysis: Building postscript file from cfg.dot & dfg.dot ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF cfg.dot binaryAnalysis_pic1.pdf
#	$(top_srcdir)/scripts/dotgenLetterPDF dfg.dot binaryAnalysis_pic2.pdf
#
## DQ (8/17/2008): Added example to show how to use traversal on AST for binary executable format.
#binaryAnalysis_pic3.pdf: binaryAnalysis_symbolTable
#	@echo "Building an example BinaryAnalysis ..."
#	cp -f $(srcdir)/buffer2.bin ./bin_example_2
#	./binaryAnalysis_symbolTable -rose:read_executable_file_format_only bin_example_2
##	DQ (9/6/2008): We are not ready for these yet, so don't generate them.
##	@echo "BinaryAnalysis: Building postscript file from AST and whole graph dot files..."
#	$(top_srcdir)/scripts/dotgenLetterPDF bin_example_2_WholeAST.dot binaryAnalysis_pic3.pdf
#	$(top_srcdir)/scripts/dotgenLetterPDF bin_example_2.dot binaryAnalysis_pic4.pdf
#
## rose_bin_example.s.aa: binaryAnalysis_pic1.pdf
#
#debuggingIRnodeToString.out: debuggingIRnodeToString
#	@echo "Generate executable from compilation of input code (debuggingIRnodeToString) ..."
#	./debuggingIRnodeToString $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleDebugging.C > debuggingIRnodeToString.out
#
#debuggingSourceCodePositionInformation.out: debuggingSourceCodePositionInformation
#	@echo "Generate executable from compilation of input code (debuggingSourceCodePositionInformation) ..."
#	./debuggingSourceCodePositionInformation $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleDebugging.C > debuggingSourceCodePositionInformation.out
#
#commandlineProcessing.out: commandlineProcessing
#	@echo "Generate executable from compilation of input code (commandlineProcessing) ..."
#	./commandlineProcessing $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleDebugging.C -edg:restrict -myTranslator:verbose 42 > commandlineProcessing.out
#
#charmSupport.out: charmSupport
#	@echo "Generate executable from compilation of input code (charmSupport) ..."
#	./charmSupport $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleCharmSupport.C > charmSupport.out
#	split -80 $(srcdir)/CharmSupport.C CharmSupport.
#
#codeCoverage.out: codeCoverage $(srcdir)/inputCode_ExampleCodeCoverage.C
#	@echo "Generate executable from compilation of input code (codeCoverage) ..."
##	While in development just run without building codeCoverage.out (and touch codeCoverage.out to allow makefile to work)
#	./codeCoverage $(ROSE_FLAGS) -c $(srcdir)/inputCode_ExampleCodeCoverage.C
##	link the input file with the coverageCodeFromShmuel.o file (written by Shmuel, tool specific)
#	$(CXX) -o ExampleCodeCoverage_GeneratedBinary inputCode_ExampleCodeCoverage.o coverageCodeFromShmuel.o
##	Run the input code (collect any output in codeCoverage.out)
#	./ExampleCodeCoverage_GeneratedBinary > codeCoverage.out
#	split -80 $(srcdir)/codeCoverage.C codeCoverage.
#
## DQ (4/10/2006): Not clear on which CG program we want!
#callGraph.pdf: callGraphAnalysis buildCG
#	@echo "Generate call graph from processing of input code (callGraph) ..."
##	./callGraphAnalysis -c $(srcdir)/inputCode_CallGraphAnalysis.C
#	./buildCG $(ROSE_FLAGS) -c $(srcdir)/inputCode_CallGraphAnalysis.C
##	$(top_srcdir)/scripts/dotgenLetterPDF callgraph.dot callGraph.ps
##	$(top_srcdir)/scripts/dotgen callgraph.dot callGraph.ps
##	$(top_srcdir)/scripts/dotgen DATABASE.dot callGraph.ps
#	$(top_srcdir)/scripts/dotgenLetterPDF callGraph.dot callGraph.pdf
#
#classHierarchyGraph.pdf: classHierarchyGraph
#	./classHierarchyGraph $(srcdir)/inputCode_ClassHierarchyGraph.C
#	@echo "Building postscript file from classHier.dot ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF classHier.dot classHierarchyGraph.pdf
#
#rose_inputCode_templateExample1.C: identityTranslator $(srcdir)/inputCode_templateExample1.C
#	@echo "Generate executable from compilation of input code (template example #1) ..."
#	./identityTranslator $(ROSE_FLAGS) -c $(srcdir)/inputCode_templateExample1.C
#
#rose_inputCode_templateExample2.C: identityTranslator $(srcdir)/inputCode_templateExample2.C
#	@echo "Generate executable from compilation of input code (template example #2) ..."
#	./identityTranslator $(ROSE_FLAGS) -c $(srcdir)/inputCode_templateExample2.C
#
#dataBaseExample1.out: dataBaseUsage
#	@echo "Generate data base from compilation of input code (data base example #1) ..."
## Build smaller versions of these files (so that they fit onto two separate pages (within the tutorial))
#	split -64 $(srcdir)/dataBaseUsage.C dataBaseUsage.
#	./dataBaseUsage $(ROSE_FLAGS) -c $(srcdir)/inputCode_dataBaseExample1.C > dataBaseExample1.out
#
#loopNestingInfoProcessing.out: loopNestingInfoProcessing $(srcdir)/inputCode_loopNestingInfoProcessing.C
#	@echo "Generate executable from compilation of input code (loopNestingInfoProcessing) ..."
#	split -l80 $(srcdir)/loopNestingInfoProcessing.C loopNestingInfoProcessing.
#	./loopNestingInfoProcessing $(ROSE_FLAGS) -c $(srcdir)/inputCode_loopNestingInfoProcessing.C > loopNestingInfoProcessing.out
#
#rose_inputCode_typeTransformation.C: typeTransformation
#	./typeTransformation $(ROSE_FLAGS) -c $(srcdir)/inputCode_typeTransformation.C
#
#astFileIO_GenerateBinaryFile.out: astFileIO_GenerateBinaryFile
#	./astFileIO_GenerateBinaryFile $(ROSE_FLAGS) -c $(srcdir)/inputCode_astFileIO_GenerateBinaryFile.C > astFileIO_temp.out
#	head -n 20 astFileIO_temp.out > astFileIO_GenerateBinaryFile.out
##	echo "File I/O temporarily disabled!" > astFileIO_GenerateBinaryFile.out
#
#generatingUniqueNamesFromDeclaration.out: generatingUniqueNamesFromDeclaration
#	./generatingUniqueNamesFromDeclaration $(ROSE_FLAGS) -c $(srcdir)/inputCode_generatingUniqueNamesFromDeclaration.C | grep -v rose_edg_macros_and_functions_required_for_gnu > generatingUniqueNamesFromDeclaration.out
#
#generatingUniqueNamesFromDeclaration2.out: generatingUniqueNamesFromDeclaration
#	./generatingUniqueNamesFromDeclaration $(ROSE_FLAGS) -c $(srcdir)/inputCode_generatingUniqueNamesFromDeclaration2.C | grep -v rose_edg_macros_and_functions_required_for_gnu > generatingUniqueNamesFromDeclaration2.out
##	echo "Test Failed!" > generatingUniqueNamesFromDeclaration2.out
#
#symbolTableHandling.out: symbolTableHandling
#	./symbolTableHandling $(ROSE_FLAGS) -c $(srcdir)/inputCode_testsymbolTableHandling.C > symbolTableHandling.out
#
#scopeInformation.out: scopeInformation
#	./scopeInformation $(ROSE_FLAGS) -c $(srcdir)/inputCode_scopeInformation.C > scopeInformation.out
#
#rose_inputCode_tauInstrumenter.C: tauInstrumenter
##	./tauInstrumenter $(ROSE_FLAGS) $(TAU_INCLUDES) -c $(srcdir)/inputCode_tauInstrumenter.C
#	echo "Test Failed!" > rose_inputCode_tauInstrumenter.C
#
#rose_inputCode_inlineTransformations.C: inlineTransformations
#	./inlineTransformations $(ROSE_FLAGS) -c $(srcdir)/inputCode_inlineTransformations.C
#
#customGraph.pdf: customGraphGeneration
#	./customGraphGeneration 
#	@echo "Building postscript file from foo.dot ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF customGraph.dot customGraph.pdf
#
#customGraphWithFiltering.pdf: customGraphGenerationWithFiltering
#	./customGraphGenerationWithFiltering
#	$(top_srcdir)/scripts/dotgenLetterPDF customGraphWithFiltering.dot customGraphWithFiltering.pdf
#
#graphTraversal.pdf: graphTraversal
#	./graphTraversal
#	$(top_srcdir)/scripts/dotgenLetterPDF graphTraversal.dot graphTraversal.pdf
#
## Liao, 2/13/2008 , added this
#rose_inputCode_addStruct.C:addStruct
#	./addStruct $(ROSE_FLAGS) -c $(srcdir)/inputCode_addStruct.C
#
#rose_inputCode_addComments.C: addComments
#	./addComments $(ROSE_FLAGS) -c $(srcdir)/inputCode_addComments.C
#
#rose_inputCode_addArbitraryTextForUnparser.C: addArbitraryTextForUnparser
#if ROSE_BUILD_OS_IS_CYGWIN
##	DQ (3/21/2009): These tests don't appear to run under Cygwin.
#	@echo "Cygwin test of addArbitraryTextForUnparser fails ..."
#	touch rose_inputCode_addArbitraryTextForUnparser.C
#else
#	./addArbitraryTextForUnparser $(ROSE_FLAGS) -c $(srcdir)/inputCode_addArbitraryTextForUnparser.C
#endif
#
#collectCommentsSkipHeaderFiles.out: collectComments
#	./collectComments $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/inputCode_collectComments.C > collectCommentsSkipHeaderFiles.out
#
#collectCommentsAcrossHeaderFiles.out: collectComments
#	./collectComments $(ROSE_FLAGS) -rose:collectAllCommentsAndDirectives -I$(srcdir) -c $(srcdir)/inputCode_collectComments.C > collectCommentsAcrossHeaderFiles.out
#
#collectDefineDirectives.out: collectDefineDirectives
#	./collectDefineDirectives $(ROSE_FLAGS) -rose:collectAllCommentsAndDirectives -I$(srcdir) -c $(srcdir)/inputCode_collectComments.C > collectDefineDirectives.out
#
#rose_inputCode_partialRedundancyElimination.C: partialRedundancyElimination
#	./partialRedundancyElimination $(ROSE_FLAGS) -c $(srcdir)/inputCode_partialRedundancyElimination.C
#
#rose_inputCode_codeGenerationFormatControl.C: codeGenerationFormatControl
#	./codeGenerationFormatControl $(ROSE_FLAGS) -c $(srcdir)/inputCode_codeGenerationFormatControl.C
#
##rose_inputCode_varSubstCopy.C: varSubstCopy
##	./varSubstCopy $(ROSE_FLAGS) -c $(srcdir)/inputCode_varSubstCopy.C
#
#dataFlowGraph.pdf: dataFlowGraph
#	@echo "Generate the data flow graph from processing of input code (dataFlowGraph) ..."
#	./callGraphAnalysis $(ROSE_FLAGS) -c $(srcdir)/inputCode_dataFlowGraphAnalysis.C
##	$(top_srcdir)/scripts/dotgenLetterPDF callgraph.dot dataFlowGraph.ps
##	$(top_srcdir)/scripts/dotgen inputCode_dataFlowGraphAnalysis.C.dot dataFlowGraph.ps
#	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_dataFlowGraphAnalysis.C.dot dataFlowGraph.pdf
#
#traversalShortCircuit.out: traversalShortCircuit
#	./traversalShortCircuit $(ROSE_FLAGS) -c $(srcdir)/inputCode_traversalShortCircuit.C > traversalShortCircuit.out
#
#wholeGraphAST.pdf: wholeGraphAST
#	./wholeGraphAST $(ROSE_FLAGS) $(srcdir)/inputCode_wholeGraphAST.C
#	@echo "Building postscript file from wholeGraphAST.dot and wholeGraphASTPool.dot ..."
#	$(top_srcdir)/scripts/dotgenLetterPDF wholeGraphAST1.dot wholeGraphAST.pdf
#	$(top_srcdir)/scripts/dotgenLetterPDF wholeGraphASTPool1.dot wholeGraphASTPool.pdf
#
#rose_inputCode_seedBugsExample_arrayIndexing.C: seedBugsExample_arrayIndexing
#	./seedBugsExample_arrayIndexing $(ROSE_FLAGS) -c $(srcdir)/inputCode_seedBugsExample_arrayIndexing.C
#
#rose_inputCode_VolatileTypeModifier.C: volatileTypeModifier
#	./volatileTypeModifier $(ROSE_FLAGS) -c $(srcdir)/inputCode_VolatileTypeModifier.C
#
#inputCode_dwarfAnalysis.pdf: dwarfAnalysis inputCode_dwarfAnalysis
#	./dwarfAnalysis -rose:visualize_dwarf_only -rose:read_executable_file_format_only inputCode_dwarfAnalysis
#	$(top_srcdir)/scripts/dotgenLetterPDF inputCode_dwarfAnalysis.dot inputCode_dwarfAnalysis.pdf
#
#inputCode_dwarfAnalysis.out: dwarfInstructionAddressToSourceLineAnalysis inputCode_dwarfAnalysis
#	./dwarfInstructionAddressToSourceLineAnalysis inputCode_dwarfAnalysis > inputCode_dwarfAnalysis.out
#
#
#
## Note that we have made multiple copies of "inputCode_ExampleTraversals.C"
## so that the processing could be done in parallel with out collisions.
#EXTRA_DIST = \
#   inputCode_1.C \
#   inputCode_BuildCFG.C \
#   inputCode_InstrumentationTranslator.C \
#   inputCode_instrumentEndOfFunction.C \
#   inputCode_wholeAST_1.C \
#   inputCode_wholeAST_2.C \
#   inputCode_QueryLibrary.C \
#   inputCode_AddFunctionDeclaration.C \
#   inputCode_AddFunctionDeclaration2.C \
#   inputCode_AddFunctionDeclaration3.C \
#   inputCode_AddExpression.C \
#   inputCode_AddAssignmentStmt.C \
#   inputCode_AbstractHandle1.cpp \
#   inputCode_AbstractHandle2.cpp \
#   inputCode_AddFunctionCalls.C \
#   inputCode_ExampleDebugging.C \
#   inputCode_LoopOptimization.C \
#   inputCode_ResolvingOverloadedFunctions.C \
#   inputCode_AddVariableDeclaration.C \
#   inputCode_ExampleTraversals.C \
#   inputCode_ExampleTraversals_1.C \
#   inputCode_ExampleTraversals_2.C \
#   inputCode_ExampleTraversals_3.C \
#   inputCode_ExampleTraversals_4.C \
#   inputCode_ExampleTraversals_5.C \
#   inputCode_ExampleTraversals_6.C \
#   inputCode_ExampleTraversals_7.C \
#   inputCode_ExampleTraversals_8.C \
#   inputCode_ExampleTraversals_9.C \
#   inputCode_ExampleTraversals_10.C \
#   inputCode_ExampleTraversals_11.C \
#   inputCode_ExampleTraversals_12.C \
#   inputCode_ExampleTraversals_13.C \
#   inputCode_ExampleTraversals_14.C \
#   inputCode_ExampleTraversals_15.C \
#   inputCode_ExampleTraversals_16.C \
#   inputCode_ExampleTraversals_17.C \
#   inputCode_ExampleTraversals_18.C \
#   inputCode_LoopOptimization_blocking.C \
#   inputCode_LoopOptimization_fusion.C \
#   inputCode_LoopOptimization_mm.C \
#   inputCode_LoopOptimization_dgemm.C \
#   inputCode_LoopOptimization_lufac.C \
#   inputCode_LoopOptimization_tridvpk.C \
#   inputCode_TemplateParameterExtration.C \
#   inputCode_ASTGraphGenerator.C \
#   inputCode_IdentityTranslator.C \
#   inputCode_LoopRecognition.C \
#   inputCode_TypeInfoFromFunctionParameters.C \
#   inputCode_ExampleCodeCoverage.C \
#   exampleMakefile.in \
#   slicingExample.C \
#   inputCode_ExampleCharmSupport.C \
#   inputCode_CallGraphAnalysis.C \
#   inputCode_templateExample1.C \
#   inputCode_templateExample2.C \
#   inputCode_dataBaseExample1.C \
#   inputCode_loopNestingInfoProcessing.C \
#   inputCode_ChangeFunctionExample.C \
#   inputCode_interveneAtVariables.C \
#   inputCode_tauInstrumenter.C \
#   inputCode_typeTransformation.C \
#   inputCode_astFileIO_GenerateBinaryFile.C \
#   inputCode_generatingUniqueNamesFromDeclaration.C \
#   inputCode_generatingUniqueNamesFromDeclaration2.C \
#   inputCode_scopeInformation.C \
#   inputCode_BuildCG.C \
#   inputCode_inlineTransformations.C \
#   inputCode_NestedQuery.C \
#   inputCode_ControlFlowGraphAnalysis.C \
#   inputCode_addComments.C \
#   inputCode_addArbitraryTextForUnparser.C \
#   inputCode_partialRedundancyElimination.C \
#   inputCode_codeGenerationFormatControl.C \
#   inputCode_dataFlowGraphAnalysis.C \
#   inputCode_wholeGraphAST.C \
#   inputCode_testsymbolTableHandling.C \
#   inputCode_traversalShortCircuit.C \
#   inputCode_collectComments.C \
#   inputCode_ClassHierarchyGraph.C \
#   inputCode_VolatileTypeModifier.C \
#   input_defuseAnalysis.C \
#   input_runtimeErrorCheck.C \
#   input_runtimeErrorCheck2.C \
#   attributeCopyExamples.C \
#   TAU_INCLUDE_DIR \
#   inputCode_addStruct.C \
#   rose_bin_example_backup.s \
#   buffer2.bin \
#   inputCode_seedBugsExample_arrayIndexing.C \
#   inputCode_dwarfAnalysis.C \
#   inputCode_reductionRecognition.C
#
## disabled this one due to problem with copy mechanism, Liao
##   inputCode_varSubstCopy.C 
#
#CLEANFILES = rose_bin_example.s callgraph.gml
#
#docs: $(noinst_PROGRAMS) $(generatedCodeExamples)
#	@echo ""
##	DQ (5/20/2007): Added to support Rich's work with outlining and the HPC Toolkit
#	cd outliner; $(MAKE) docs
#	cd roseHPCT; $(MAKE) docs
#	cd intelPin; $(MAKE) docs
#	@echo "******************************************************************"
#	@echo "Generated required documentation for tutorial (make docs rule) ..."
#	@echo "******************************************************************"
#
## Note that the *.trace files come from the execution of the codeCoverage.C test,
## and the *.binary files are generated from the astFileIO_GenerateBinaryFile.C test.
## The *.binary are the binary form of the ASTs.
#cleanDocs:
##	This can not be called after make distclean (by make distcheck)
##	cd outliner; $(MAKE) cleanDocs
#	rm -f rose_*.C rose_*.cpp *.out *.a[a-l] *.dot *.ps *.pdf *.trace *.binaryAST 
#	rm -f ExampleCodeCoverage_GeneratedBinary 
#	rm -f inputCode_AST_PDF_Generator.C 
#	rm -f rose_inputCode_dwarfAnalysis.s inputCode_dwarfAnalysis rose_inputCode_dwarfAnalysis
#
#clean-local: cleanDocs
#	rm -rf Templates.DB ii_files ti_files *.ti *.svg  *.new rose_bin_example.s rose_bin_example_2.s bin_example_2 *.dump
#	find . -depth -name \*.dSYM -exec rm -rf {} \; # OS X object file debug info
#	rm -rf $(generatedCodeExamples)
##	rm -f exampleTranslator exampleTranslatorMultiLib
##	rm -f inputCode_ExampleCodeCoverage
#
## Compile everything with -g (compiler's debug flag)
## AM_CPPFLAGS = -g
#
## DQ (1/4/2006): Added check rule (remove all generated code and rerun "make docs").
#check-local:
##	DQ (3/27/2006): Note that these should not have to already exist, so use "rm -rf"
##	rm -rf $(generatedCodeExamples)
#	$(MAKE) docs
#	@echo "***************************************************************************"
#	@echo "****** ROSE/tutorial: make check rule complete (terminated normally) ******"
#	@echo "***************************************************************************"
#
#installcheck: exampleMakefile
#	@echo "Remove existing binaries so that they can be rebuilt using the installed libraries."
#	rm -f $(noinst_PROGRAMS) *.o
#	@echo "Call exampleMakefile to show how to build ROSE tutorial examples using installed libraries."
#if USE_ROSE_IN_BUILD_TREE_VAR
#	unset ROSE_IN_BUILD_TREE; $(MAKE) -f exampleMakefile
#else
#	$(MAKE) -f exampleMakefile
#endif
#
