#ifndef AST_FIX_UP_H
#define AST_FIX_UP_H

// #define AST_FIXES_VERBOSE_LEVEL DIAGNOSTICS_VERBOSE_LEVEL
#define AST_FIXES_VERBOSE_LEVEL DIAGNOSTICS_VERBOSE_LEVEL+1

#include "fixupforGnuBackendCompiler.h"
#include "fixupFunctionDefinitions.h"
#include "fixupInClassDataInitialization.h"
#include "fixupStorageAccessOfForwardTemplateDeclarations.h"
#include "fixupTemplateDeclarations.h"
#include "removeInitializedNamePtr.h"
#include "fixupSourcePositionInformation.h"
#include "fixupEnumValues.h"
#include "fixupFriendTemplateDeclarations.h"
#include "fixupPrettyFunction.h"

// Defined in AstFixup.C
void removeEmptyElses (SgNode* top);

// endif for ifndef AST_FIX_UP_H
#endif















