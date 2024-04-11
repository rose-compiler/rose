#ifndef _LIBADALANG_TYPE
#define _LIBADALANG_TYPE 1

/// Contains functions and classes for converting Ada expressions
///   from Libadalang to ROSE.


#include "Libadalang_to_ROSE.h"

namespace Libadalang_ROSE_Translation {

/// initializes a standard package with built-in ada types
/// \todo this should disappear as soon as the Standard package is included in Asis
void initializePkgStandard(SgGlobal& global);


} //end Libadlang_ROSE_Translation namespace


#endif //_LIBABLANG_TYPE_H
