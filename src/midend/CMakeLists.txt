
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(abstractHandle)
add_subdirectory(astDiagnostics)
add_subdirectory(astProcessing)
add_subdirectory(astQuery)
add_subdirectory(astRewriteMechanism)
add_subdirectory(astUtil)
add_subdirectory(binaryAnalyses)
add_subdirectory(programAnalysis)
add_subdirectory(programTransformation)
########### next target   ###############
set( midend_src
${CMAKE_SOURCE_DIR}/src/midend/midend_roseh_pch.cpp
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
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstSharedMemoryParallelSimpleProcessing.C
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
#${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/inputProgram1.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/nodeCollection.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteDebuggingSupport.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteHighLevelInterface.C
#${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/inputProgram3.C
#${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/inputProgram2.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteMidLevelInterface.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteSynthesizedAttribute.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteAccumulatedAttribute.C
#${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteTemplateInstantiations.C
#${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/inputProgram4.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteTypeDeclaration.C
${CMAKE_SOURCE_DIR}/src/midend/astRewriteMechanism/rewriteASTFragementString.C
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/GraphAlgorithms.C
${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/binary_analysis.C
#${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/libraryIdentification/libraryIdentification_writer.C
#${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/libraryIdentification/libraryIdentification_reader.C
#${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/libraryIdentification/functionIdentification.C
${CMAKE_SOURCE_DIR}/src/midend/astDiagnostics/AstConsistencyTests.C
${CMAKE_SOURCE_DIR}/src/midend/astDiagnostics/AstWarnings.C
${CMAKE_SOURCE_DIR}/src/midend/astDiagnostics/AstPerformance.C
${CMAKE_SOURCE_DIR}/src/midend/astDiagnostics/AstStatistics.C
)

add_library( midend STATIC ${midend_src}  )
add_dependencies( midend ROSETTA  )

target_link_libraries(midend midend_pa midend_pt midend_loopproc midend_binary
 )
 
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
