#ifndef AST_POST_PROCESSING_H
#define AST_POST_PROCESSING_H

#define AST_POST_PROCESSING_VERBOSE_LEVEL DIAGNOSTICS_VERBOSE_LEVEL+1

#include "AstFixup.h"

#include "resetParentPointers.h"
#include "processTemplateHandlingOptions.h"
#include "fixupSymbolTables.h"

// DQ (12/29/2011): This header file requires rose_config.h and we don't want all of ROSE to require this.
// #include "markCompilerGenerated.h"

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
#include "checkIsFrontendSpecificFlag.h"
#include "checkIsCompilerGeneratedFlag.h"
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

// DQ (9/14/2011): Added support to make the AST consistant with respect to constant folded values.
// We can either leave the constant folded values or replace the constant folded values with the 
// original expression trees. The default will be to replace the constant folded values with the
// original expression trees.
#include "fixupConstantFoldedValues.h"

// DQ (5/1/2012): Added testing for marked transformations in the AST (should be none after EDG/ROSE translation).
#include "detectTransformations.h"

// DQ (8/12/2012): Fixup the SgModifiers used to hold type reference in EDG to types in instantiated templates that had not yet been seen).
// This post processing effects test2012_190.C and test2007_141.C.
#include "fixupTypeReferences.h"

// DQ (10/5/2012): Fixup known macros that might expand into a recursive mess in the unparsed code.
#include "fixupSelfReferentialMacros.h"

// DQ (4/24/2013): Detect the correct function declaration to declare the use of default arguments.
// This can only be a single function and it can't be any function (this is a moderately complex issue).
#include "fixupFunctionDefaultArguments.h"

// DQ (12/20/2012): Added support for testing the physical source position information.
#include "checkPhysicalSourcePosition.h"

// DQ (6/11/2013): This corrects where EDG can set the scope of a friend declaration to be different from the defining declaration.
#include "fixupDeclarationScope.h"

// DQ (11/14/2015): This corrects inconstancies in the setting of flags in the Sg_File_Info objects.
#include "fixupFileInfoFlags.h"

// DQ (11/27/2016): Provide alternative typedef type that when unparsed will not contain private types).
#include "fixupTemplateArguments.h"

// DQ (5/18/2017): Added support to insert template instantiation prototypes.
#include "addPrototypesForTemplateInstantiations.h"

// DQ (2/25/2019): Adding support for marking shared defining declarations across multiple files.
#include "markSharedDeclarationsForOutputInCodeGeneration.h"


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
ROSE_DLL_API void AstPostProcessing(SgNode* node);


#if 0
// DQ (4/26/2013): Test constructed to detect problems with where default arguments are marked.
class PostProcessingTestFunctionCallArguments : public AstSimpleProcessing
   {
     public:
          void visit (SgNode* node);
   };

void postProcessingTestFunctionCallArguments(SgNode* node);
#endif

#endif
