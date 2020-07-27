set(astPostProcessingSources
  astPostProcessing.C
  fixupSymbolTables.C
  markForOutputInCodeGeneration.C
  processTemplateHandlingOptions.C
  checkIsModifiedFlag.C
  checkIsFrontendSpecificFlag.C
  checkIsCompilerGeneratedFlag.C
  fixupTemplateInstantiations.C
  markTemplateInstantiationsForOutput.C
  resetParentPointers.C
  fixupDefiningAndNondefiningDeclarations.C
  markCompilerGenerated.C
  markTemplateSpecializationsForOutput.C
  resetTemplateNames.C
  initializeExplicitScopeData.C
  fixupDeclarations.C
  markOverloadedTemplateInstantiations.C
  markTransformationsForOutput.C
  markBackendCompilerSpecificFunctions.C
  fixupNullPointers.C
  fixupNames.C
  fixupTypes.C
  fixupTypeReferences.C
  fixupConstructorPreinitializationLists.C
  propagateHiddenListData.C
  resolveFortranReferences.C
  insertFortranContainsStatement.C
  markLhsValues.C
  fixupUseAndUsingDeclarations.C
  buildStatementNumbering.C
  fixupCxxSymbolTablesToSupportAliasingSymbols.C
  normalizeTypedefSequenceLists.C
  fixupConstantFoldedValues.C
  detectTransformations.C
  fixupFunctionDefaultArguments.C
  fixupSelfReferentialMacros.C
  fixupDeclarationScope.C
  checkPhysicalSourcePosition.C
  fixupFileInfoFlags.C
  fixupTemplateArguments.C
  addPrototypesForTemplateInstantiations.C
  markSharedDeclarationsForOutputInCodeGeneration.C)

if(NOT enable-c)
  set(astPostProcessingSources ${astPostProcessingSources} dummy.C)
endif()

add_library(astPostProcessing OBJECT ${astPostProcessingSources})

add_dependencies(astPostProcessing rosetta_generated)


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

    checkIsCompilerGeneratedFlag.h checkIsFrontendSpecificFlag.h
    checkPhysicalSourcePosition.h detectTransformations.h
    fixupDeclarationScope.h fixupFunctionDefaultArguments.h
    fixupSelfReferentialMacros.h fixupTypeReferences.h
    fixupFileInfoFlags.h
    fixupTemplateArguments.h
    addPrototypesForTemplateInstantiations.h
    markSharedDeclarationsForOutputInCodeGeneration.h addIncludeDirectives.h

    DESTINATION ${INCLUDE_INSTALL_DIR})

