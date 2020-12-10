include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libastPostProcessing.la

libastPostProcessing_la_SOURCES      = \
     astPostProcessing.C \
     fixupSymbolTables.C \
     markForOutputInCodeGeneration.C \
     processTemplateHandlingOptions.C \
     checkIsModifiedFlag.C \
     checkIsFrontendSpecificFlag.C \
     checkIsCompilerGeneratedFlag.C \
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
     normalizeTypedefSequenceLists.C \
     fixupConstantFoldedValues.C \
     detectTransformations.C \
     fixupTypeReferences.C \
     fixupSelfReferentialMacros.C \
     fixupFunctionDefaultArguments.C \
     checkPhysicalSourcePosition.C \
     fixupDeclarationScope.C \
     fixupFileInfoFlags.C \
     fixupTemplateArguments.C \
     addPrototypesForTemplateInstantiations.C \
     markSharedDeclarationsForOutputInCodeGeneration.C \
     addIncludeDirectives.C \
     fixupInitializers.C

if !ROSE_BUILD_CXX_LANGUAGE_SUPPORT
libastPostProcessing_la_SOURCES += dummy.C
endif

libastPostProcessing_la_LIBADD       = 
libastPostProcessing_la_DEPENDENCIES = 

pkginclude_HEADERS = \
     astPostProcessing.h \
     fixupDefiningAndNondefiningDeclarations.h \
     markCompilerGenerated.h \
     markTemplateSpecializationsForOutput.h \
     resetTemplateNames.h \
     checkIsModifiedFlag.h \
     checkIsFrontendSpecificFlag.h \
     checkIsCompilerGeneratedFlag.h \
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
     normalizeTypedefSequenceLists.h \
     fixupConstantFoldedValues.h \
     detectTransformations.h \
     fixupTypeReferences.h \
     fixupSelfReferentialMacros.h \
     fixupFunctionDefaultArguments.h \
     checkPhysicalSourcePosition.h \
     fixupDeclarationScope.h \
     fixupFileInfoFlags.h \
     fixupTemplateArguments.h \
     addPrototypesForTemplateInstantiations.h \
     markSharedDeclarationsForOutputInCodeGeneration.h \
     addIncludeDirectives.h \
     fixupInitializers.h

EXTRA_DIST = CMakeLists.txt

# DQ (10/17/2011): Alternative header file dependency support.
# If this is required we can use this as a way of supporting the header file
# dependency tailored to exclude system headers.  But it is a more complex form of what is alreay
# supported more directly via automake.  So we have decided to use the automake version for now.
# Specification of where we put the *.P files (the comiler will build the *.d files there by default).
# DQ (10/16/2011): Added support for makefile dependences of header files (requested by Rice).
# Need to get this into the compileer command line to generate the *.d file in the .libs directory.
# AM_CPPFLAGS += -MMD
# DEPEND_FILE_DIR = .libs
# This is a specific variable name used by either Make (or automake generated makefiles).
# DEPDIR = $(DEPEND_FILE_DIR)
# dependeFileName = $(DEPEND_FILE_DIR)/$(*F)
# $(DEPEND_FILE_DIR)/%.P : %.lo
#	sed -e 's/.libs\///'  -e 's/o:/lo:/' < $(dependeFileName).d >> $(dependeFileName).P;
# -include $(libastPostProcessing_la_SOURCES:%.C=$(DEPEND_FILE_DIR)/%.P)
# clean-dep:
#	rm -rf $(DEPEND_FILE_DIR)/*.d $(DEPEND_FILE_DIR)/*.P 


