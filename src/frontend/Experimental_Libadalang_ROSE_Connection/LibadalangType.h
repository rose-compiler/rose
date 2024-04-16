#ifndef _LIBADALANG_TYPE
#define _LIBADALANG_TYPE 1

/// Contains functions and classes for converting Ada expressions
///   from Libadalang to ROSE.


#include "Libadalang_to_ROSE.h"

#include "libadalang.h"

namespace Libadalang_ROSE_Translation {

//Function to hash a unique int from a node using the node's kind and location.
//The kind and location can be provided, but if not they will be determined in the function
int hash_node(ada_base_entity *node, int kind = -1, std::string full_sloc = "");

/// initializes a standard package with built-in ada types
/// \todo this should disappear as soon as the Standard package is included in Asis
void initializePkgStandard(SgGlobal& global, ada_base_entity* lal_root);

/// returns the ROSE type for the Libadalang type represented by \ref lal_id
SgType&
getDeclType(ada_base_entity* lal_id, AstContext ctx);

} //end Libadlang_ROSE_Translation namespace


#endif //_LIBABLANG_TYPE_H
