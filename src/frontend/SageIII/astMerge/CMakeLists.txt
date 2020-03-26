add_library(astMerge OBJECT
  merge_support.C test_support.C buildMangledNameMap.C
  buildSetOfFrontendSpecificNodes.C deleteNodes.C fixupTraversal.C nullifyAST.C
  buildReplacementMap.C collectAssociateNodes.C deleteOrphanNodes.C
  normalizeTypes.C requiredNodes.C merge.C AstFixParentTraversal.C)
add_dependencies(astMerge rosetta_generated)


########### install files ###############

install(FILES
  buildMangledNameMap.h buildReplacementMap.h collectAssociateNodes.h
  deleteOrphanNodes.h fixupTraversal.h merge.h merge_support.h nullifyAST.h
  test_support.h requiredNodes.h AstFixParentTraversal.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
