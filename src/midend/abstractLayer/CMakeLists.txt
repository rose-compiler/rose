
add_library(abstractLayer OBJECT
  AstAnnotator.C
  CodeThornException.C
  CPAstAttributeInterface.C
  DFAstAttribute.C
  Labeler.C
  RDAstAttributeInterface.C
  SgNodeHelper.C
  VariableIdMapping.C
  )

add_dependencies(abstractLayer rosetta_generated)

install(FILES
  AstAnnotator.h
  CodeThornException.h
  CPAstAttributeInterface.h
  DFAstAttribute.h
  Labeler.h
  RDAstAttributeInterface.h
  SgNodeHelper.h
  VariableIdMapping.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
