#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

namespace SI = SageInterface;
namespace SB = SageBuilder;

static
SgStatement& buildFunctionExitStmt(RtedTransformation& trans, SgInitializedName& resvar, ReturnInfo ri, SgScopeStatement& scope)
{
  SgExprStatement* exitBlock = NULL;

  if (ri.expected_return == ReturnInfo::rtValue)
  {
    exitBlock = trans.buildExitBlockStmt(ri.open_blocks, scope, scope.get_endOfConstruct());
  }
  else
  {
    ROSE_ASSERT(ri.expected_return == ReturnInfo::rtIndirection);
    exitBlock = trans.buildDelayedLeakCheckExitStmt(ri.filetype, ri.open_blocks, scope, resvar, scope.get_endOfConstruct());
  }

  ROSE_ASSERT(exitBlock);
  return *exitBlock;
}


void
RtedTransformation::changeReturnStmt(ReturnInfo rinfo)
{
  SgReturnStmt* const   rstmt = rinfo.stmt;
  SgExpression* const   returnExpr = rstmt->get_expression();

  requiresParentIsBasicBlock(*rstmt);

  if (!returnExpr)
  {
    // function returns a value but return statement has no expression
    insertErrorReport(*rstmt, "return statement is expected to return a value");
    return;
  }

  // We need to build a new variable of type returnExpr
  // \pp why do we have to do that?
  //     most likely b/c exitScope clears all local pointers ...
  SgScopeStatement*      scope = rstmt->get_scope();
  SgType* const          typeRet = returnExpr->get_type();
  std::string            name = "rstmt";

  ROSE_ASSERT(scope);
  name.append(scope->get_qualified_name().str());

  SgName                 rName( name );
  SgAssignInitializer*   init = SB::buildAssignInitializer(returnExpr);
  SgVariableDeclaration* resDecl = SB::buildVariableDeclaration( rName, typeRet, init, scope );
  SgInitializedName&     resVar = SI::getFirstVariable(*resDecl);

  SgVarRefExp* const     vexp = SB::buildVarRefExp(rName, scope);
  SgStatement* const     newRtnStmt = SB::buildReturnStmt( vexp );

  SI::replaceStatement( rstmt, newRtnStmt );
  SI::insertStatementBefore( newRtnStmt, resDecl );

  Sg_File_Info* const    fileinfo = scope->get_endOfConstruct();

  // handle C++ only if the function returns a pointer
  if (rinfo.filetype != ftCxx)
  {
    SgStatement* const   exitStmt = (rinfo.expected_return == ReturnInfo::rtValue)
                                          ? buildExitBlockStmt(rinfo.open_blocks, *scope, fileinfo)
                                          : buildDelayedLeakCheckExitStmt(rinfo.filetype, rinfo.open_blocks, *scope, resVar, fileinfo)
                                          ;

    SI::insertStatementBefore( newRtnStmt, exitStmt );
  }
  else if (rinfo.expected_return == ReturnInfo::rtIndirection)
  {
    SgStatement* const   exitStmt = buildDelayedLeakCheckExitStmt(rinfo.filetype, rinfo.open_blocks, *scope, resVar, fileinfo);

    SI::insertStatementBefore( newRtnStmt, exitStmt );
  }
}

#endif
