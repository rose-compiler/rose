
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(abstractHandle)
add_subdirectory(astDiagnostics)
add_subdirectory(astInlining)
add_subdirectory(astOutlining)
add_subdirectory(astProcessing)
add_subdirectory(astQuery)
add_subdirectory(astRewriteMechanism)
add_subdirectory(astUtil)
add_subdirectory(binaryAnalyses)
add_subdirectory(loopProcessing)
add_subdirectory(ompLowering)
add_subdirectory(programAnalysis)
add_subdirectory(programTransformation)
########### next target   ###############
set( midend_src
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/NameGenerator.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/Preprocess.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/Outliner.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/Insert.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/NonLocalDecls.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/ExtractIfs.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/Case.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/Block.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/PreprocIfs.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/This.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/Check.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/NonLocalControlFlow.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/ASTtools.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/PrePostTraversal.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/GenerateFunc.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/IfDirectiveContextFinder.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/Jumps.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/IfDirectiveExtractor.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/VarSym.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/PreprocessingInfo.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/PragmaInterface.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/StmtRewrite.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/CollectVars.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/ThisExprs.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/Copy.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/If.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/Transform.cc
${CMAKE_SOURCE_DIR}/src/midend/astOutlining/GenerateCall.cc
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/language-lexer.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/language-parser.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/annotations.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/annvariable.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/structure.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/enum_property.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/procedure.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/set_property.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/pointerrule.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/analyze.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/rule.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/broadway_expr.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/report.cc 
${CMAKE_SOURCE_DIR}/src/midend/ompLowering/omp_lowering.cpp
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/RoseBin_CallGraphAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/RoseBin_FlowAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/RoseBin_DataFlowAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/RoseBin_CompareAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/RoseBin_printInstructions.cpp
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/RoseBin_ControlFlowAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/dataflowanalyses/RoseBin_Emulate.cpp
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/dataflowanalyses/RoseBin_DataFlowAbstract.cpp
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/dataflowanalyses/RoseBin_VariableAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/dataflowanalyses/InterruptAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/dataflowanalyses/RoseBin_DefUseAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/DefUseAnalysisAbstract.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/DefUseAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/GlobalVarAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/LivenessAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/dfaToDot.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/DefUseAnalysis_perFunction.cpp
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/runTest.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/graphAnalysis/RoseBin_GmlGraph.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/graphAnalysis/RoseBin_Graph.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/graphAnalysis/RoseBin_DotGraph.cpp
${CMAKE_SOURCE_DIR}/src/midend/abstractHandle/abstract_handle.cpp
${CMAKE_SOURCE_DIR}/src/midend/abstractHandle/loopAdapter.cpp
${CMAKE_SOURCE_DIR}/src/midend/abstractHandle/roseAdapter.cpp
${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface/CPPAstInterface.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface/AstInterface.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal/SymbolicVal.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal/SymbolicOperator.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal/SymbolicUnwrapCond.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal/SymbolicBound.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal/SymbolicFind.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal/SymbolicExpr.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/annotation/OperatorAnnotation.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/annotation/AnnotIO.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/annotation/AnnotGen.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/annotation/AnnotCollect.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/annotation/AnnotDescriptors.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/annotation/OperatorDescriptors.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/annotation/AnnotExpr.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/annotation/TypeAnnotation.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport/ProcessAstTree.C
${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport/StmtInfoCollect.C
${CMAKE_SOURCE_DIR}/src/midend/astInlining/isPotentiallyModified.C
${CMAKE_SOURCE_DIR}/src/midend/astInlining/replaceExpressionWithStatement.C
${CMAKE_SOURCE_DIR}/src/midend/astInlining/inliner.C
${CMAKE_SOURCE_DIR}/src/midend/astInlining/inlinerSupport.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/prepostTransformation/PrivatizeScalar.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/prepostTransformation/BreakupStmt.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/prepostTransformation/LoopUnroll.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/computation/LoopTreeTransform.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/computation/LoopTree.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/computation/LoopTreeDummyNode.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/computation/LoopTreeShadow.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/computation/DepCompTransform.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/computation/LoopTreeDepComp.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/computation/LoopTreeLocality.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/slicing/CompSliceDepGraph.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/slicing/DynamicCompSlice.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/slicing/CompSliceLocality.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/slicing/CompSlice.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/slicing/DependenceHoisting.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/depGraph/TransDepGraph.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/depGraph/DepGraphBuild.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/depGraph/LoopAnalysis.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/outsideInterface/ArrayRewrite.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/outsideInterface/ArrayAnnot.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/outsideInterface/ArrayInterface.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/driver/CopyArrayAnal.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/driver/LoopTransformInterface.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/driver/FusionAnal.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/driver/NormalizeCPP.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/driver/InterchangeAnal.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/driver/LoopTransformOptions.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/driver/TransformComputation.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/driver/BlockingAnal.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/driver/TypedFusionImpl.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/depInfo/StmtDepAnal.C
#${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/depInfo/DepTestStatistics.C
#${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/depInfo/PlatoOmegaInterface.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/depInfo/DepInfo.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/depInfo/DepRel.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/depInfo/DepInfoSet.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/depInfo/DomainInfo.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/depInfo/DepEDD.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/depInfo/ReuseAnalysis.C
${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/depInfo/DepInfoAnal.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstRestructure.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstProcessing.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstCombinedSimpleProcessing.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstNodePtrs.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstNodeVisitMapping.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstDOTGeneration.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstAttributeMechanism.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstSimpleProcessing.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstClearVisitFlags.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstSuccessorsSelectors.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstPDFGeneration.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstTextAttributesHandling.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/Ast.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstTraversal.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstSharedMemoryParallelSimpleProcessing.C
${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstReverseSimpleProcessing.C
${CMAKE_SOURCE_DIR}/src/midend/astQuery/nodeQueryInheritedAttribute.C
${CMAKE_SOURCE_DIR}/src/midend/astQuery/nameQuery.C
${CMAKE_SOURCE_DIR}/src/midend/astQuery/astQueryInheritedAttribute.C
${CMAKE_SOURCE_DIR}/src/midend/astQuery/numberQueryInheritedAttribute.C
${CMAKE_SOURCE_DIR}/src/midend/astQuery/booleanQuery.C
${CMAKE_SOURCE_DIR}/src/midend/astQuery/booleanQueryInheritedAttribute.C
${CMAKE_SOURCE_DIR}/src/midend/astQuery/numberQuery.C
${CMAKE_SOURCE_DIR}/src/midend/astQuery/astQuery.C
${CMAKE_SOURCE_DIR}/src/midend/astQuery/nameQueryInheritedAttribute.C
#${CMAKE_SOURCE_DIR}/src/midend/astQuery/queryVariant.C
${CMAKE_SOURCE_DIR}/src/midend/astQuery/nodeQuery.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteLowLevelInterface.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/inputProgram1.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/nodeCollection.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteDebuggingSupport.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteHighLevelInterface.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/inputProgram3.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/inputProgram2.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteMidLevelInterface.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteSynthesizedAttribute.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteAccumulatedAttribute.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteTemplateInstantiations.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/inputProgram4.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteTypeDeclaration.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteASTFragementString.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/implicitCodeGeneration/defaultFunctionGenerator.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/implicitCodeGeneration/destructorCallAnnotator.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/implicitCodeGeneration/analysisUtils.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/implicitCodeGeneration/shortCircuitingTransformation.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/partialRedundancyElimination/expressionTreeEqual.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/partialRedundancyElimination/controlFlowGraph.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/partialRedundancyElimination/pre.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/constantFolding/constantFolding.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/functionCallNormalization/FunctionNormalization.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/finiteDifferencing/finiteDifferencing.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/finiteDifferencing/patternRewrite.C
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/GraphAlgorithms.C
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/binary_analysis.C
#${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/libraryIdentification/libraryIdentification_writer.C
#${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/libraryIdentification/libraryIdentification_reader.C
#${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/libraryIdentification/functionIdentification.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/valuePropagation/ValueAnnot.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/valuePropagation/ValuePropagate.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OAWrap/OAWrap.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OAWrap/SAGE2OA.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/proceduralSlicing/SlicingInfo.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/sideEffectAnalysis/sideEffect.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysis/CallGraphDB.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysis/CallGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysis/callgraph_example.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysis/ClassHierarchyGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/CFG/OARIFG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/CFG/TarjanIntervals.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/CFG/CFG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/CallGraph/CallGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/BaseGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/Tree.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/DGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/UnionFindUniverse.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/Graph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/trtest.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/grtest.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/SSA/DomTree.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/SSA/SSA.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/SSA/Phi.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/createFDG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/testCDG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/generateSFCFG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/printDUC.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/createDDG.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/MergedDependenceGraph.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/ControlFlowGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/multiPassTest.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/newControlDependenceGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/testDDG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/testSlicing.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/FunctionDependenceGraph.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/ControlDependenceGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/generateSFBDT.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/createSDG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/generateSFDT.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/DependenceGraph.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/EDefUse.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/DataDependenceGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/generateSFDF.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/SlicingInfo.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/slice.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/CreateSlice.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/CreateSliceSet.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/newDU.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/testSDG.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/SystemDependenceGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/createCDG.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/DefUseExtension.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/distributedMemoryAnalysis/functionNames.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/distributedMemoryAnalysis/DistributedMemoryAnalysis.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CFG/CFG_OA.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CFG/CFG_ROSE.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/pointerAnal/PtrAnalCFG.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/pointerAnal/PtrAnal.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysisMySQL/Callgraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysisMySQL/callgraph_example.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysisMySQL/Classhierarchy.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/dataflowAnalysis/DataFlowAnalysis.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/dataflowAnalysis/ReachingDefinition.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/dataflowAnalysis/DefUseChain.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/graphAnalysis/SB_Graph.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/graphAnalysis/CallGraphAnalysis.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/graphAnalysis/RoseSrc_CallGraphAnalysis.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/dominatorTreesAndDominanceFrontiers/ControlFlowGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/dominatorTreesAndDominanceFrontiers/DominanceFrontier.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/dominatorTreesAndDominanceFrontiers/DominatorTree.C
${CMAKE_SOURCE_DIR}/src/midend/astDiagnostics/AstConsistencyTests.C
${CMAKE_SOURCE_DIR}/src/midend/astDiagnostics/AstWarnings.C
${CMAKE_SOURCE_DIR}/src/midend/astDiagnostics/AstPerformance.C
${CMAKE_SOURCE_DIR}/src/midend/astDiagnostics/AstStatistics.C
)

add_library( midend ${midend_src}  )
add_dependencies( midend ROSETTA )

########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = \
#        abstractHandle \
#        astDiagnostics \
#        astInlining \
#        astOutlining \
#        astProcessing \
#        astQuery \
#        astRewriteMechanism \
#        astUtil \
#        binaryAnalyses \
#        loopProcessing \
#        ompLowering \
#        programAnalysis \
#        programTransformation
#
## DQ (7/25/2008):
## This file is deliberately excluded from a distribution so that the
## "make docs" rule (at the top level directory) can query the file to
## determin if this is a distribution.
## EXTRA_DIST = midend.docs
