#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>

#include "sageGeneric.hpp"

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace SageInterface;
using namespace SageBuilder;


static
SgReturnStmt* return_stmt(ReturnInfo ri)
{
  return ri.first;
}

static
size_t open_blocks(ReturnInfo ri)
{
  return ri.second;
}

void
RtedTransformation::changeReturnStmt(ReturnInfo rinfo)
{
  SgReturnStmt* rstmt = return_stmt(rinfo);
  ROSE_ASSERT(rstmt);
  std::cerr << "\n\n@@@@@@@@@@@@@@@@@@@@@@@ enter ReturnStmtChange" << std::endl;

  SgExpression*         rightOfRet = rstmt->get_expression();
  // we need to build a new variable of type rightOfRet
  SgType*               typeRet = rightOfRet->get_type();
  // and assign that expr to that new variable:
  //   type x = rightOfRet;
  //   return x;

  requiresParentIsBasicBlock(*rstmt);

  SgScopeStatement*     scope = rstmt->get_scope();
  std::string           name = "rstmt";

  name.append(scope->get_qualified_name().str());

  SgName                rName( name );
  SgAssignInitializer*  init = buildAssignInitializer(rightOfRet);
  SgStatement*          newStmt = buildVariableDeclaration( rName,
                                                            deepCopy(typeRet),
                                                            init,
                                                            scope
                                                          );
  SgVarRefExp*          vexp = buildVarRefExp(rName, scope);
  SgStatement*          newRtnStmt = buildReturnStmt( vexp );

  replaceStatement( rstmt, newRtnStmt );
  insertStatementBefore( newRtnStmt, newStmt );

  /* do not do it right now */
  // SgStatement*          exitBlock = buildExitBlockStmt(open_blocks(rinfo), scope, scope->get_endOfConstruct());
  // insertStatementBefore( newRtnStmt, exitBlock );
}

#endif
