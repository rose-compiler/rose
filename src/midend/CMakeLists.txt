
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

add_library( midend_static STATIC ${midend_src}  )
add_dependencies( midend_static ROSETTA midend_processing midend_util midend_pa midend_pt midend_loopproc midend_binary )

# tps (12/17/2009) : Experiments to create (small) dynamic libraries.
# The following libraries require dllexport and dllimport in WINDOWS!
#add_library( midend_shared SHARED ${midend_src}  )
#add_dependencies( midend_shared ROSETTA midend_processing midend_util midend_pa midend_pt midend_loopproc midend_binary )

target_link_libraries(midend_static 
     midend_processing midend_util midend_pa midend_pt midend_loopproc midend_binary
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
