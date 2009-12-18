
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(astInterface)
add_subdirectory(astSupport)
add_subdirectory(symbolicVal)
add_subdirectory(annotation)


########### install files ###############


set( midend_util_src
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
)

add_library( midend_util STATIC ${midend_util_src}  )
add_dependencies( midend_util ROSETTA )



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = astInterface astSupport  symbolicVal  annotation
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#clean-local:
#
#distclean-local:
#
#
#
#
#
#
#
