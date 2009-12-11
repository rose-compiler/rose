
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(partialRedundancyElimination)
add_subdirectory(finiteDifferencing)
add_subdirectory(functionCallNormalization)
add_subdirectory(constantFolding)
add_subdirectory(implicitCodeGeneration)
add_subdirectory(astInlining)
add_subdirectory(astOutlining)
add_subdirectory(loopProcessing)
add_subdirectory(ompLowering)



set( midend_pt_src
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/NameGenerator.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/Preprocess.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/Outliner.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/Insert.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/NonLocalDecls.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/ExtractIfs.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/Case.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/Block.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/PreprocIfs.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/This.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/Check.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/NonLocalControlFlow.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/ASTtools.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/PrePostTraversal.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/GenerateFunc.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/IfDirectiveContextFinder.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/Jumps.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/IfDirectiveExtractor.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/VarSym.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/PreprocessingInfo.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/PragmaInterface.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/StmtRewrite.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/CollectVars.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/ThisExprs.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/Copy.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/If.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/Transform.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astOutlining/GenerateCall.cc
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/ompLowering/omp_lowering.cpp
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astInlining/isPotentiallyModified.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astInlining/replaceExpressionWithStatement.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astInlining/inliner.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/astInlining/inlinerSupport.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/prepostTransformation/PrivatizeScalar.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/prepostTransformation/BreakupStmt.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/prepostTransformation/LoopUnroll.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/computation/LoopTreeTransform.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/computation/LoopTree.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/computation/LoopTreeDummyNode.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/computation/LoopTreeShadow.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/computation/DepCompTransform.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/computation/LoopTreeDepComp.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/computation/LoopTreeLocality.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/slicing/CompSliceDepGraph.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/slicing/DynamicCompSlice.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/slicing/CompSliceLocality.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/slicing/CompSlice.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/slicing/DependenceHoisting.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depGraph/TransDepGraph.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depGraph/DepGraphBuild.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depGraph/LoopAnalysis.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/outsideInterface/ArrayRewrite.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/outsideInterface/ArrayAnnot.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/outsideInterface/ArrayInterface.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/driver/CopyArrayAnal.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/driver/LoopTransformInterface.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/driver/FusionAnal.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/driver/NormalizeCPP.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/driver/InterchangeAnal.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/driver/LoopTransformOptions.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/driver/TransformComputation.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/driver/BlockingAnal.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/driver/TypedFusionImpl.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depInfo/StmtDepAnal.C
#${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depInfo/DepTestStatistics.C
#${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depInfo/PlatoOmegaInterface.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depInfo/DepInfo.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depInfo/DepRel.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depInfo/DepInfoSet.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depInfo/DomainInfo.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depInfo/DepEDD.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depInfo/ReuseAnalysis.C
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depInfo/DepInfoAnal.C
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
)

add_library( midend_pt STATIC ${midend_pt_src}  )
add_dependencies( midend_pt ROSETTA )


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = partialRedundancyElimination finiteDifferencing functionCallNormalization constantFolding implicitCodeGeneration 
#
#
#
