#ifndef ROSE_FORTRAN_SUPPORT
#define ROSE_FORTRAN_SUPPORT

extern SgSourceFile* OpenFortranParser_globalFilePointer;

#ifdef __cplusplus
extern "C" {
#endif

// #include "ActionEnums.h"
// #include "token.h"
// #include "FortranParserAction.h"

#ifdef __cplusplus
} /* End extern C. */
#endif

// Include the module specific header file here.
// #include "FortranModuleInfo.h"

// Control output from Fortran parser
#define DEBUG_FORTRAN_SUPPORT true
#define DEBUG_RULE_COMMENT_LEVEL 1
#define DEBUG_COMMENT_LEVEL 2



// #include "aterm_traversal/traversal.hpp"
// #include "aterm_traversal/OFPUnparser.hpp"
#include "traversal.hpp"
#include "OFPUnparser.hpp"
#include "UntypedASTBuilder.hpp"










// endif for ROSE_FORTRAN_SUPPORT
#endif


