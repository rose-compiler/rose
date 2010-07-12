include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libastPostProcessing.la

libastPostProcessing_la_SOURCES      = \
     astPostProcessing.C \
     fixupSymbolTables.C \
     markForOutputInCodeGeneration.C \
     processTemplateHandlingOptions.C \
     checkIsModifiedFlag.C \
     fixupTemplateInstantiations.C \
     markTemplateInstantiationsForOutput.C \
     resetParentPointers.C \
     fixupDefiningAndNondefiningDeclarations.C \
     markCompilerGenerated.C \
     markTemplateSpecializationsForOutput.C \
     resetTemplateNames.C \
     initializeExplicitScopeData.C \
     fixupDeclarations.C \
     markOverloadedTemplateInstantiations.C \
     markTransformationsForOutput.C \
     markBackendCompilerSpecificFunctions.C \
     fixupNullPointers.C \
     fixupNames.C \
     fixupTypes.C \
     fixupConstructorPreinitializationLists.C \
     propagateHiddenListData.C \
     resolveFortranReferences.C \
     insertFortranContainsStatement.C \
     markLhsValues.C \
     fixupUseAndUsingDeclarations.C \
     buildStatementNumbering.C \
     fixupCxxSymbolTablesToSupportAliasingSymbols.C \
     normalizeTypedefSequenceLists.C

libastPostProcessing_la_LIBADD       = 
libastPostProcessing_la_DEPENDENCIES = 

include_HEADERS = \
     astPostProcessing.h \
     fixupDefiningAndNondefiningDeclarations.h \
     markCompilerGenerated.h \
     markTemplateSpecializationsForOutput.h \
     resetTemplateNames.h \
     checkIsModifiedFlag.h \
     fixupSymbolTables.h \
     markForOutputInCodeGeneration.h \
     processTemplateHandlingOptions.h \
     fixupDeclarations.h \
     fixupTemplateInstantiations.h \
     markTemplateInstantiationsForOutput.h \
     resetParentPointers.h \
     initializeExplicitScopeData.h \
     markOverloadedTemplateInstantiations.h \
     markTransformationsForOutput.h \
     markBackendCompilerSpecificFunctions.h \
     fixupNullPointers.h \
     fixupNames.h \
     fixupTypes.h \
     fixupConstructorPreinitializationLists.h \
     propagateHiddenListData.h \
     resolveFortranReferences.h \
     insertFortranContainsStatement.h \
     markLhsValues.h \
     fixupUseAndUsingDeclarations.h \
     fixupCxxSymbolTablesToSupportAliasingSymbols.h \
     normalizeTypedefSequenceLists.h

EXTRA_DIST = CMakeLists.txt
