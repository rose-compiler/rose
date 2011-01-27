#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>

#include "rosez.hpp"

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace SageInterface;
using namespace SageBuilder;


void
RtedTransformation::changeReturnStmt(SgReturnStmt * rstmt)
{
	ROSE_ASSERT(rstmt);
  std::cerr << "\n\n@@@@@@@@@@@@@@@@@@@@@@@ enter ReturnStmtChange" << std::endl;

  SgExpression*         rightOfRet = rstmt->get_expression();
  // we need to build a new variable of type rightOfRet
  SgType*               typeRet = rightOfRet->get_type();
  // and assign that expr to that new variable:
  //   type x = rightOfRet;
  //   return x;

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
  SgVarRefExp*          vexp = buildVarRefExp(rName, rstmt->get_scope());
  SgStatement*          newRtnStmt = buildReturnStmt( vexp );

	replaceStatement( rstmt, newRtnStmt );
  insertStatementBefore( newRtnStmt, newStmt );

  //replaceExpression(rightOfRet,vexp);
  std::map<SgStatement*, SgNode*>::iterator it = scopes.find(rstmt);
  if (it!=scopes.end()) {
    SgNode* fcall = it->second;

		scopes.erase(it->first);
    scopes[newStmt]=fcall;
  }

/*
  SgFunctionDefinition* main = &ez::ancestor<SgFunctionDefinition>(*rstmt);

  if (is_main_func(main))
	{
    // overwrite the last statment in main;
    mainLast = newRtnStmt;
    std::cerr << "\n\n@@@@@@@@@@@@@@@@@@@@@@@ Changing mainLast in ReturnStmtChange" << std::endl;
  }
*/
}

#endif
