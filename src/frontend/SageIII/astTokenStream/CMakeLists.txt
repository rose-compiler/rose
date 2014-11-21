add_library( astTokenStream OBJECT
  createMap.C
  doCompleteMapping.C
  linearizeAST.C
  unparseMacro.C
  tokenStreamMapping.C
  frontierDetection.C
  previousAndNextNode.C)
add_dependencies(astTokenStream rosetta_generated)



########### install files ###############

install(FILES
  createMap.h  doCompleteMapping.h  linearizeAST.h unparseMacro.h
  tokenStreamMapping.h frontierDetection.h previousAndNextNode.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
