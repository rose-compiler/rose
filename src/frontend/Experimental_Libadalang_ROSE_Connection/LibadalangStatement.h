#ifndef _LIBADALANG_STATEMENT
#define _LIBADALANG_STATEMENT 1

/// Contains functions and classes for converting Ada statements and
///   declarations from Libadalang to ROSE.

#include "Libadalang_to_ROSE.h"
#include "libadalang.h"

namespace Libadalang_ROSE_Translation
{


  /// converts an Asis declaration and adds the new node to the current scope
  void handleDeclaration(ada_base_entity* lal_element, AstContext ctx, bool isPrivate = false);

}


#endif /* _LIBADALANG_STATEMENT */
