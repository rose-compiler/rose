
#include "sage3basic.h"
#include "sageGeneric.h"

#include "AdaStatement.h"

namespace Ada_ROSE_Translation
{
  SgDeclarationStatement*
  getDecl_opt(Expression_Struct& expr, AstContext ctx)
  {
    return findFirst(asisDecls(), expr.Corresponding_Name_Definition, expr.Corresponding_Name_Declaration);
  }
}
