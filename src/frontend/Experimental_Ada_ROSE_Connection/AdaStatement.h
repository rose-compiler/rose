#ifndef _ADA_STATEMENT
#define _ADA_STATEMENT 1

#include "Ada_to_ROSE.h"
#include "a_nodes.h"

namespace Ada_ROSE_Translation
{
  /// returns the declaration identified by \ref expr
  ///   if no record was found, nullptr is returned
  SgDeclarationStatement*
  getDecl_opt(Expression_Struct& expr, AstContext ctx);


}

#endif /* _ADA_STATEMENT */
