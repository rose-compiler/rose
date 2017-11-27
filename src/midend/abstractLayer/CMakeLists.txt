
add_library(abstractLayer OBJECT
  SprayException.h
  AstAnnotator.C
  CPAstAttributeInterface.C
  DFAstAttribute.C
  Labeler.C
  RDAstAttributeInterface.C
  SgNodeHelper.C
  VariableIdMapping.C
  TypeSizeMapping.C
  SgTypeSizeMapping.C
  )

add_dependencies(abstractLayer rosetta_generated)

install(FILES
  AstAnnotator.h
  CPAstAttributeInterface.h
  DFAstAttribute.h
  Labeler.h
  RDAstAttributeInterface.h
  SgNodeHelper.h
  VariableIdMapping.h
  TypeSizeMapping.h
  SgTypeSizeMapping.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
