add_library(astFixup OBJECT
  AstFixup.C fixupforGnuBackendCompiler.C fixupInClassDataInitialization.C
  removeInitializedNamePtr.C fixupDeclarations2.C fixupFunctionDefinitions.C
  fixupStorageAccessOfForwardTemplateDeclarations.C
  fixupSourcePositionInformation.C fixupEnumValues.C
  fixupFriendTemplateDeclarations.C fixupPrettyFunction.C
)
add_dependencies(astFixup rosetta_generated)

########### install files ###############

install(FILES  AstFixup.h fixupFunctionDefinitions.h
    fixupStorageAccessOfForwardTemplateDeclarations.h removeInitializedNamePtr.h
    fixupforGnuBackendCompiler.h fixupInClassDataInitialization.h
    fixupTemplateDeclarations.h fixupSourcePositionInformation.h
    fixupEnumValues.h fixupFriendTemplateDeclarations.h
    fixupPrettyFunction.h fixupPrettyFunction.h
    DESTINATION ${INCLUDE_INSTALL_DIR})
