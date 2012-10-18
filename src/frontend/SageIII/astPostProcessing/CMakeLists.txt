
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


#add_library( libastPostProcessing STATIC ${libastPostProcessing_SRC}  )
#add_dependencies( libastPostProcessing ROSETTA   )


########### install files ###############

install(FILES  astPostProcessing.h       fixupDefiningAndNondefiningDeclarations.h
    markCompilerGenerated.h       markTemplateSpecializationsForOutput.h
    resetTemplateNames.h       checkIsModifiedFlag.h checkIsFrontendSpecificFlag.C checkIsCompilerGeneratedFlag.C
    fixupSymbolTables.h
    markForOutputInCodeGeneration.h       processTemplateHandlingOptions.h
    fixupDeclarations.h       fixupTemplateInstantiations.h
    markTemplateInstantiationsForOutput.h       resetParentPointers.h
    initializeExplicitScopeData.h       markOverloadedTemplateInstantiations.h
    markTransformationsForOutput.h       markBackendCompilerSpecificFunctions.h
    fixupNullPointers.h       fixupNames.h       fixupTypes.h
    fixupConstructorPreinitializationLists.h       propagateHiddenListData.h
    resolveFortranReferences.h       insertFortranContainsStatement.h
    markLhsValues.h       fixupUseAndUsingDeclarations.h
    fixupCxxSymbolTablesToSupportAliasingSymbols.h normalizeTypedefSequenceLists.h
    fixupConstantFoldedValues.h
    DESTINATION ${INCLUDE_INSTALL_DIR})


