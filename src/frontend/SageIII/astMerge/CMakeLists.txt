add_library(astMerge OBJECT
  fixupTraversal.C merge.C link.C delete.C share.C load.C consistency.C)
add_dependencies(astMerge rosetta_generated)


########### install files ###############

install(FILES
  merge.h fixupTraversal.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
