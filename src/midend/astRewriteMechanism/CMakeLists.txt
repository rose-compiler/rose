add_library(astRewriteMechanism OBJECT
  rewriteASTFragementString.C rewriteLowLevelInterface.C
  rewriteTemplateInstantiations.C nodeCollection.C rewriteDebuggingSupport.C
  rewriteMidLevelInterface.C rewriteTypeDeclaration.C
  rewriteAccumulatedAttribute.C rewriteHighLevelInterface.C
  rewriteSynthesizedAttribute.C)
add_dependencies(astRewriteMechanism rosetta_generated)

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
