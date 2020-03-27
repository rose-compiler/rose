add_library(astMerge OBJECT
  merge_support.C fixupTraversal.C merge.C)
add_dependencies(astMerge rosetta_generated)


########### install files ###############

install(FILES
  merge.h fixupTraversal.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
