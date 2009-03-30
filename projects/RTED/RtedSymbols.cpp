#include "rose.h"
#include "RtedSymbols.h"

using namespace std;

void RtedSymbols::visit(SgNode* n) {

  if (n == NULL)
    return;
  // *********************** NEEDED to insert RuntimeSystem runtimeSystem = new RuntimeSystem() ***************
#if 0
  SgScopeStatement* scope = isSgScopeStatement(n);
  if (scope && rememberTopNode==NULL && scope!=globalScope) {
    //    cerr << "Scope : " << scope->class_name()<< " " << scope->get_file_info() << endl;
    if (scope->get_file_info()->isSameFile(globalScope->get_file_info())) {
      cerr << ">>>> Found top scope : " << scope->class_name() << endl;
      SgFunctionDefinition* funcDef = isSgFunctionDefinition(scope);
      string funcName="";
      if (funcDef)
	funcName = funcDef->get_mangled_name().str();
      cerr << ">>>> FuncDef : " << funcName << " " << endl;
      rememberTopNode=scope;
      cerr << ">>>> rememberTopNode assigned : " << scope << endl;
    }
  }
  // check for different types of variable access
  // if access is found, then we assert the variable before the current statement
  SgClassDeclaration* classDecl_tmp = isSgClassDeclaration(n);
  if (classDecl_tmp) {
    if (classDecl_tmp->get_symbol_from_symbol_table()) {
      string name = classDecl_tmp->get_symbol_from_symbol_table()->get_name().str();
      //      cerr << "Looking at class : " << name << endl;
      if (name=="RuntimeSystem") {
	cerr << ">>>>>> Found class : " << name << endl;
	runtimeClassSymbol = isSgClassSymbol(classDecl_tmp->get_symbol_from_symbol_table());
      }
    }
  }
#endif

  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

  if (isSgScopeStatement(n)) {
	  SgFunctionSymbol* func = isSgScopeStatement(n)->lookup_function_symbol("roseCreateArray");
	  if (isSgMemberFunctionSymbol(func)) {
		  roseCreateArray = isSgMemberFunctionSymbol(func);
		  cerr << ">>>>>>>>>>>>> Found MemberName : roseCreateArray" <<  endl;
	  }
	  SgFunctionSymbol* func2 = isSgScopeStatement(n)->lookup_function_symbol("roseArrayAccess");
	  if (isSgMemberFunctionSymbol(func2)) {
		  roseArrayAccess = isSgMemberFunctionSymbol(func2);
		  cerr << ">>>>>>>>>>>>> Found MemberName : roseArrayAccess" <<  endl;
	  }
  }


#if 0
  // *********************** DETECT THE rose CreateArray func call in Runtime System -- needed for Transformations ***************
  SgMemberFunctionDeclaration* roseCreateArray_tmp =
    isSgMemberFunctionDeclaration(n);
  if (roseCreateArray_tmp) {
    if (roseCreateArray_tmp->get_symbol_from_symbol_table()) {
      string
	memberName =
	roseCreateArray_tmp->get_symbol_from_symbol_table()->get_name().str();
      //cerr <<"Found MemberName : " << memberName << endl;
      if (memberName == "roseCreateArray") {
	cerr << ">>>>>>>>>>>>> Found MemberName : " << memberName
	     << endl;
	roseCreateArray
	  = isSgMemberFunctionSymbol(
				     roseCreateArray_tmp->get_symbol_from_symbol_table());
      }
    }
  }

  // *********************** DETECT THE rose ArrayAccess func call in Runtime System -- needed for Transformations ***************
  SgMemberFunctionDeclaration* roseArrayAccess_tmp =
    isSgMemberFunctionDeclaration(n);
  if (roseArrayAccess_tmp) {
    if (roseArrayAccess_tmp->get_symbol_from_symbol_table()) {
      string
	memberName =
	roseArrayAccess_tmp->get_symbol_from_symbol_table()->get_name().str();
      //cerr <<"Found MemberName : " << memberName << endl;
      if (memberName == "roseArrayAccess") {
	cerr << ">>>>>>>>>>>>> Found MemberName : " << memberName
	     << endl;
	roseArrayAccess
	  = isSgMemberFunctionSymbol(
				     roseArrayAccess_tmp->get_symbol_from_symbol_table());
      }
    }
  }
#endif
  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************

}

