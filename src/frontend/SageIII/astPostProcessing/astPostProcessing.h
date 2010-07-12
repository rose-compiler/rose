#ifndef AST_POST_PROCESSING_H
#define AST_POST_PROCESSING_H

#define AST_POST_PROCESSING_VERBOSE_LEVEL DIAGNOSTICS_VERBOSE_LEVEL+1

#include "AstFixup.h"

#include "resetParentPointers.h"
#include "processTemplateHandlingOptions.h"
#include "fixupSymbolTables.h"
#include "markCompilerGenerated.h"
#include "markTemplateInstantiationsForOutput.h"
#include "resetTemplateNames.h"
#include "fixupDeclarations.h"
#include "fixupTemplateInstantiations.h"
#include "markForOutputInCodeGeneration.h"
#include "markTemplateSpecializationsForOutput.h"
#include "initializeExplicitScopeData.h"
#include "fixupDefiningAndNondefiningDeclarations.h"
#include "markOverloadedTemplateInstantiations.h"
#include "markTransformationsForOutput.h"
#include "markBackendCompilerSpecificFunctions.h"
#include "fixupNullPointers.h"
#include "checkIsModifiedFlag.h"
#include "fixupNames.h"
#include "fixupTypes.h"
#include "fixupConstructorPreinitializationLists.h"
#include "propagateHiddenListData.h"
#include "markLhsValues.h"

// DQ (11/24/2007): Fortran support to resolution of array references vs. function references.
#include "resolveFortranReferences.h"
#include "insertFortranContainsStatement.h"

// DQ (9/28/2008): This is for Fortran and eventually maybe C++ support.
#include "fixupUseAndUsingDeclarations.h"

// DQ (4/14/2010): This is the C++ specific support for symbol aliasing (to support better name qualification).
#include "fixupCxxSymbolTablesToSupportAliasingSymbols.h"

// DQ (6/24/2010): Fixup the SgTypedefSeq lists to support the AST merge mechanism.
#include "normalizeTypedefSequenceLists.h"

/*! \brief Postprocessing that is not likely to be handled in the EDG/Sage III translation.
 */
void postProcessingSupport (SgNode* node);

/*! \brief This does all post-processing fixup and translation of the Sage III AST.

    This function was added to provide a more representative name, the post processing does not
    just include temporary fixes.  We might make if nore explicit what function are representative
    of temporary fixes later (at the moment I think there are none that are temporary).
    This does all post-processing fixup of the AST including:
       1) initialization of the parent pointers
       2) fixup of instantiated template names
       3) fixup of ...
       4) ...

 */
void AstPostProcessing(SgNode* node);

#endif
