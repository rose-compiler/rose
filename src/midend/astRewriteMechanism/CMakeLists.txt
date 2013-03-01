
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############
set(TemplateFiles
  rewriteMidLevelInterfaceTemplatesImpl.h
  prefixGenerationImpl.h
  ASTFragmentCollectorTraversalImpl.h
  rewriteSynthesizedAttributeTemplatesImpl.h
  rewriteTreeTraversalImpl.h
  rewriteASTFragementStringTemplatesImpl.h
  nodeCollectionTemplatesImpl.h
  rewriteDebuggingSupportTemplatesImpl.h
  rewriteTemplateImpl.h)

install(
  FILES rewrite.h lowLevelRewriteInterface.h midLevelRewriteInterface.h
        highLevelRewriteInterface.h ${TemplateFiles}
  DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
### The Rewrite Library should use its own template repository
## CXX_TEMPLATE_REPOSITORY_PATH = .
#
#INCLUDES = $(ROSE_INCLUDES)
#
## Separate out the template files to make clear which sources are templates
## the hope was that this would be useful to generate the TemplateSourceFiles
#TemplateFiles = \
#     rewriteMidLevelInterfaceTemplatesImpl.h \
#     prefixGenerationImpl.h \
#     ASTFragmentCollectorTraversalImpl.h \
#     rewriteSynthesizedAttributeTemplatesImpl.h \
#     rewriteTreeTraversalImpl.h \
#     rewriteASTFragementStringTemplatesImpl.h \
#     nodeCollectionTemplatesImpl.h \
#     rewriteDebuggingSupportTemplatesImpl.h \
#     rewriteTemplateImpl.h
#
#librewriteSources = \
#     nodeCollection.C \
#     rewriteMidLevelInterface.C \
#     rewriteHighLevelInterface.C \
#     rewriteSynthesizedAttribute.C \
#     rewriteASTFragementString.C \
#     rewriteAccumulatedAttribute.C \
#     rewriteTypeDeclaration.C \
#     rewriteDebuggingSupport.C \
#     rewriteLowLevelInterface.C \
#     $(TemplateFiles)
#
## lib_LTLIBRARIES = librewrite.a
## librewrite_a_SOURCES  = $(librewriteSources)
## librewrite_a_DEPENDENCIES  = $(librewrite_a_SOURCES)
#
#noinst_LTLIBRARIES = librewrite.la
#librewrite_la_SOURCES = $(librewriteSources) rewriteTemplateInstantiations.C $(TemplateFiles)
#librewrite_la_DEPENDENCIES = $(librewrite_la_SOURCES)
#
## Header files we want to have installed
## (note that rewrite.h includes the rewriteTreeTraversal.C 
## and rewriteSynthesizedAttributeTemplates.C because of how
## templates are used).
#include_HEADERS = \
#     rewrite.h \
#     lowLevelRewriteInterface.h \
#     midLevelRewriteInterface.h \
#     highLevelRewriteInterface.h \
#     $(TemplateFiles)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files core *.dot *.pdf
#	rm -rf rose_inputProgram*.C \
#          rose_transformation_*.C *.C.pdf
#
#EXTRA_DIST = ASTRewrite.docs
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
