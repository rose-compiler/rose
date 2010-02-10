#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;


void
RtedTransformation::changeReturnStmt(SgReturnStmt * rstmt) {
    cerr << "\n\n@@@@@@@@@@@@@@@@@@@@@@@ enter ReturnStmtChange" << endl;

#if 1
  SgExpression* rightOfRet = rstmt->get_expression();
  // we need to build a new variable of type rightOfRet
  SgType* typeRet = rightOfRet->get_type();
  // and assign that expr to that new variable:
  // type x = rightOfRet;
  // return x;

  SgScopeStatement* scope = rstmt->get_scope();
  string name = "rstmt";
  name+=scope->get_qualified_name().str();
  SgName rName = SgName( name );

  SgStatement* newStmt = buildVariableDeclaration(rName,
						  //typeRet->copy(),
						  deepCopy(typeRet),
						  buildAssignInitializer(rightOfRet),
						  scope);
  SgVarRefExp* vexp = buildVarRefExp(rName,rstmt->get_scope());
  ROSE_ASSERT(vexp);

  
  SgStatement* newRtnStmt = buildReturnStmt( vexp );
  replaceStatement( rstmt, newRtnStmt );
  insertStatementBefore( newRtnStmt, newStmt );

  //replaceExpression(rightOfRet,vexp);
  std::map<SgStatement*, SgNode*>::iterator it = scopes.find(rstmt);
  if (it!=scopes.end()) {
    SgNode* fcall = it->second;
    scopes.erase(it->first);
    scopes[newStmt]=fcall;
  }

  SgFunctionDefinition* main = isSgFunctionDefinition(rstmt->get_parent()->get_parent());
  cerr <<"@@@ Found func: " << rstmt->get_parent()->get_parent()->class_name() << 
    "   main = " << main << "  " << main->class_name() << endl;
  ROSE_ASSERT(main);
  if (main) {
    string funcname = main->get_declaration()->get_name().str();
    if (funcname == "main" ||
	funcname == "RuntimeSystem_original_main")
    {

    // overwrite the last statment in main;
      mainLast=newRtnStmt;
     mainHasBeenChanged=true;
      cerr << "\n\n@@@@@@@@@@@@@@@@@@@@@@@ Changing mainLast in ReturnStmtChange" << endl;
    } else {
      cerr << "\n\n@@@@@@@@@@@@@@@@@@@@@@@ skipping in ReturnStmtChange : " << funcname << endl;
    }
  }

#endif
}

#endif
