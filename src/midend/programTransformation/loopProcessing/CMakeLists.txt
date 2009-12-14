
include_directories(${CMAKE_SOURCE_DIR}/src/util/support ${CMAKE_SOURCE_DIR}/src/util/graphs ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport ${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(depInfo)
add_subdirectory(depGraph)
add_subdirectory(computation)
add_subdirectory(outsideInterface)
add_subdirectory(slicing)
add_subdirectory(driver)
add_subdirectory(prepostTransformation)


########### install files ###############

set( midend_loopproc_src
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
)

add_library( midend_loopproc STATIC ${midend_loopproc_src}  )
add_dependencies( midend_loopproc ROSETTA )




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = depInfo depGraph computation outsideInterface slicing driver prepostTransformation
#
#INCLUDES = \
#   $(ROSE_INCLUDES) -I${srcdir}/driver -I${srcdir}/prepostTransformation -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/symbolicVal -I$(top_srcdir)/src/midend/astUtil/astInterface
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#	rm -rf rose_mm.C
#
#distclean-local:
#	rm -rf Templates.DB
#
