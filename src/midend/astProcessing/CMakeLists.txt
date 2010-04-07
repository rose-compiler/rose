
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstRestructure.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstProcessing.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstCombinedSimpleProcessing.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstNodePtrs.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstNodeVisitMapping.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstDOTGeneration.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstAttributeMechanism.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstSimpleProcessing.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstClearVisitFlags.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstSuccessorsSelectors.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstPDFGeneration.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstTextAttributesHandling.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/Ast.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstTraversal.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstSharedMemoryParallelSimpleProcessing.C
#${CMAKE_SOURCE_DIR}/src/midend/astProcessing/AstReverseSimpleProcessing.C

########### next target ###############

#add_library( midend_processing  ${libastprocessing_SRC} )
#add_dependencies( midend_processing ROSETTA )


########### install files ###############

if (WIN32)
#tps commented out AstSharedMemoryParallelProcessing.h for Windows 
install(FILES  AstPDFGeneration.h AstNodeVisitMapping.h AstAttributeMechanism.h     AstTextAttributesHandling.h AstDOTGeneration.h AstProcessing.h     AstSimpleProcessing.h AstTraverseToRoot.h AstNodePtrs.h     AstSuccessorsSelectors.h AstReverseProcessing.h     AstReverseSimpleProcessing.h Ast.h AstRestructure.h AstClearVisitFlags.h     AstTraversal.h AstCombinedProcessing.h AstCombinedProcessingImpl.h     AstCombinedSimpleProcessing.h StackFrameVector.h DESTINATION ${INCLUDE_INSTALL_DIR})
else (WIN32)
install(FILES  AstPDFGeneration.h AstNodeVisitMapping.h AstAttributeMechanism.h     AstTextAttributesHandling.h AstDOTGeneration.h AstProcessing.h     AstSimpleProcessing.h AstTraverseToRoot.h AstNodePtrs.h     AstSuccessorsSelectors.h AstReverseProcessing.h     AstReverseSimpleProcessing.h Ast.h AstRestructure.h AstClearVisitFlags.h     AstTraversal.h AstCombinedProcessing.h AstCombinedProcessingImpl.h     AstCombinedSimpleProcessing.h StackFrameVector.h AstSharedMemoryParallelProcessing.h     AstSharedMemoryParallelProcessingImpl.h AstSharedMemoryParallelSimpleProcessing.h DESTINATION ${INCLUDE_INSTALL_DIR})
endif (WIN32)


#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS =
#
#INCLUDES = $(ROSE_INCLUDES)
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libastprocessingSources = \
#   AstPDFGeneration.C AstNodeVisitMapping.C AstTextAttributesHandling.C \
#   AstDOTGeneration.C AstProcessing.C AstSimpleProcessing.C Ast.C \
#   AstNodePtrs.C AstSuccessorsSelectors.C AstAttributeMechanism.C \
#   AstReverseSimpleProcessing.C AstRestructure.C AstClearVisitFlags.C \
#   AstTraversal.C AstCombinedSimpleProcessing.C \
#   AstSharedMemoryParallelSimpleProcessing.C
#
## Removed AstTests.C AstStatistics.C 
#
## lib_LTLIBRARIES = libastprocessing.a
## libastprocessing_a_SOURCES  = $(libastprocessingSources)
## libastprocessing_a_LIBADD  = $(CXX_TEMPLATE_OBJECTS)
## libastprocessing_a_DEPENDENCIES  = $(libastprocessing_a_SOURCES)
#
#noinst_LTLIBRARIES = libastprocessing.la
#libastprocessing_la_SOURCES = $(libastprocessingSources)
#libastprocessing_la_LIBADD = $(CXX_TEMPLATE_OBJECTS)
#libastprocessing_la_DEPENDENCIES = $(libastprocessing_a_SOURCES)
#
## $(GENERATED_SOURCE):
##	cd $(top_builddir)/src/ROSETTA/src; $(MAKE)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS = \
#   AstPDFGeneration.h AstNodeVisitMapping.h AstAttributeMechanism.h \
#   AstTextAttributesHandling.h AstDOTGeneration.h AstProcessing.h \
#   AstSimpleProcessing.h AstTraverseToRoot.h AstNodePtrs.h \
#   AstSuccessorsSelectors.h AstReverseProcessing.h \
#   AstReverseSimpleProcessing.h Ast.h AstRestructure.h AstClearVisitFlags.h \
#   AstTraversal.h AstCombinedProcessing.h AstCombinedProcessingImpl.h \
#   AstCombinedSimpleProcessing.h StackFrameVector.h AstSharedMemoryParallelProcessing.h \
#   AstSharedMemoryParallelProcessingImpl.h AstSharedMemoryParallelSimpleProcessing.h
#
## Removed AstTests.h AstStatistics.h 
#
#EXTRA_DIST = AstProcessing.docs
#
#
#
#
#
#
#
