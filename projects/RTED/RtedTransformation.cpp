#include <rose.h>
#include <string>
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;



/* -----------------------------------------------------------
 * Helper Function
 * -----------------------------------------------------------*/
SgStatement*
RtedTransformation::getStatement(SgExpression* exp) {
	SgStatement* stmt = NULL;
	SgNode* expr = exp;
	while (!isSgStatement(expr) && !isSgProject(expr))
		expr = expr->get_parent();
	if (isSgStatement(expr))
		stmt = isSgStatement(expr);
	return stmt;
}

/* -----------------------------------------------------------
 * Run frontend and return project
 * -----------------------------------------------------------*/
SgProject*
RtedTransformation::parse(int argc, char** argv) {
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT(project);
  return project;
}

/* -----------------------------------------------------------
 * Perform Transformation: insertArrayCreateCall
 * -----------------------------------------------------------*/
void
RtedTransformation::insertArrayCreateCall(SgInitializedName* n, SgExpression* value) {
  ROSE_ASSERT(n);
  SgStatement* stmt = getSurroundingStatement(n);
  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();

    ROSE_ASSERT(scope);
    if (isSgBasicBlock(scope)) {
      string name = n->get_mangled_name().str();
      SgStringVal* callNameExp = buildStringVal(name);

      SgExprListExp* arg_list = buildExprListExp();
      appendExpression(arg_list,callNameExp);
      appendExpression(arg_list,value);

      SgVarRefExp* varRef_l = buildVarRefExp("runtimeSystem",globalScope);
      string symbolName = varRef_l->get_symbol()->get_name().str();
      //cerr << " >>>>>>>> Symbol VarRef: " << symbolName << endl;

      ROSE_ASSERT(roseCreateArray);
      string symbolName2 = roseCreateArray->get_name().str();
      //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
      SgMemberFunctionRefExp* memRef_r = buildMemberFunctionRefExp(roseCreateArray,false,true);
      SgArrowExp* sgArrowExp = buildArrowExp(varRef_l,memRef_r);

      SgFunctionCallExp* funcCallExp = buildFunctionCallExp(sgArrowExp,arg_list);
      SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
      insertStatementBefore(isSgStatement(stmt),exprStmt) ;
    }

  } else {
    cerr << "RuntimeInstrumentation :: Surrounding Statement could not be found! " <<endl;
    exit(0);
  }
}

/* -----------------------------------------------------------
 * Perform Transformation: insertArrayCreateAccessCall
 * -----------------------------------------------------------*/
void
RtedTransformation::insertArrayAccessCall(SgVarRefExp* arrayNameRef, SgExpression* value) {
  ROSE_ASSERT(arrayNameRef);
  SgVariableSymbol* varSymbol = arrayNameRef->get_symbol();
  ROSE_ASSERT(varSymbol);
  SgInitializedName* initName = varSymbol->get_declaration();
  ROSE_ASSERT(initName);
 // cerr << " .. Creating Array Access for  : " << arrayNameRef->unparseToString() << value->unparseToString() << endl;

  SgStatement* stmt = getSurroundingStatement(arrayNameRef);
  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    ROSE_ASSERT(scope);
    cerr << "   .. Creating Array Access for  : " << arrayNameRef->unparseToString() << "-"<<value->unparseToString() <<
		"       scope : " << scope << " " << scope->class_name() << "   : " << stmt->unparseToString() << endl;
   	if (isSgIfStmt(stmt->get_parent()) && isSgIfStmt(stmt->get_parent())->get_conditional() == stmt) {
   		cerr << "       >> Current stmt == if-conditional " << endl;
   		return;
    }
   	else if (isSgWhileStmt(stmt->get_parent()) && isSgWhileStmt(stmt->get_parent())->get_condition() == stmt) {
   		cerr << "       >> Current stmt == while-conditional " << endl;
   		return;
    }
    scope = isSgBasicBlock(ensureBasicBlockAsParent(stmt));
    ROSE_ASSERT (isSgBasicBlock(scope));
      string name = initName->get_mangled_name().str();
      SgStringVal* callNameExp = buildStringVal(name);

      SgExprListExp* arg_list = buildExprListExp();
      appendExpression(arg_list,callNameExp);
      appendExpression(arg_list,value);

      SgVarRefExp* varRef_l = buildVarRefExp("runtimeSystem",globalScope);
      string symbolName = varRef_l->get_symbol()->get_name().str();
      //cerr << " >>>>>>>> Symbol VarRef: " << symbolName << endl;

      ROSE_ASSERT(roseArrayAccess);
      string symbolName2 = roseArrayAccess->get_name().str();
      //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
      SgMemberFunctionRefExp* memRef_r = buildMemberFunctionRefExp(roseArrayAccess,false,true);
      SgArrowExp* sgArrowExp = buildArrowExp(varRef_l,memRef_r);

      SgFunctionCallExp* funcCallExp = buildFunctionCallExp(sgArrowExp,arg_list);
      SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
      insertStatementBefore(isSgStatement(stmt),exprStmt) ;
//    }

  } else {
    cerr << "RuntimeInstrumentation :: Surrounding Statement could not be found! " <<endl;
    exit(0);
  }
}

/* -----------------------------------------------------------
 * Insert the header files (Step 1)
 * -----------------------------------------------------------*/
void
RtedTransformation::insertProlog(SgProject* proj) {
  cout << "Inserting headers ... " << endl;
  // grep all source (.c) files and insert headers
  Rose_STL_Container<SgNode*> vec = 
    NodeQuery::querySubTree(proj,V_SgSourceFile);
  cerr << "Found source files : " << vec.size() << endl;
  Rose_STL_Container<SgNode*>::iterator it = vec.begin();
  for (;it!=vec.end();++it) {
    SgSourceFile* source = isSgSourceFile(*it);
    ROSE_ASSERT(source);
    //  globalScope = getFirstGlobalScope(isSgProject(proj));
    globalScope = source->get_globalScope();
    pushScopeStack (isSgScopeStatement (globalScope));
  // this needs to be fixed
  //buildCpreprocessorDefineDeclaration(globalScope, "#define EXITCODE_OK 0");

#if 0
  // currently doesnt work -- crashes somewhere in wave
  insertHeader("rose.h",PreprocessingInfo::before,false,globalScope);
#else
  insertHeader("RuntimeSystem.h",PreprocessingInfo::before,false,globalScope);
  insertHeader("iostream",PreprocessingInfo::before,true,globalScope);
  insertHeader("map",PreprocessingInfo::before,true,globalScope);
  insertHeader("string",PreprocessingInfo::before,true,globalScope);
#endif

  popScopeStack ();
  }
}

/* -----------------------------------------------------------
* Insert
* RuntimeSystem* runtimeSystem = new RuntimeSystem();
* -----------------------------------------------------------*/
void
RtedTransformation::insertRuntimeSystemClass() {
#if 0

  Sg_File_Info* fileInfo = globalScope->get_file_info();
  ROSE_ASSERT(runtimeClassSymbol);
  ROSE_ASSERT(runtimeClassSymbol->get_type());
  SgType* type  = runtimeClassSymbol->get_type();
  //SgType* type  = new SgClassType();
  cerr << "Found type : " << type->class_name() << endl;

  SgExprListExp* exprList = buildExprListExp();
  ROSE_ASSERT(roseCreateArray->get_declaration());

  SgConstructorInitializer* constr = buildConstructorInitializer(roseCreateArray->get_declaration(),
								 exprList,type,false,false,false,false);
  SgExprListExp* exprList2 = buildExprListExp();
  //  SgNewExp* newexp = new SgNewExp(fileInfo,type,exprList2,constr,NULL,0,NULL);
  SgNewExp* newexp = buildNewExp(type,NULL,constr,NULL,0,NULL);
  SgAssignInitializer* init = buildAssignInitializer(newexp);

  SgVariableDeclaration* variable =
    buildVariableDeclaration("runtimeSystem",buildPointerType(type),init);
  SgStatement* st = isSgStatement(rememberTopNode->get_parent());
  insertStatement(st,variable,true);
#endif
}

/* -----------------------------------------------------------
 * Perform all transformations needed (Step 2)
 * -----------------------------------------------------------*/
void
RtedTransformation::transform(SgProject* project) {
  cout << "Running Transformation..." << endl;
  globalScope = getFirstGlobalScope(isSgProject(project));

#if 0
  rememberTopNode=NULL;
  runtimeClassSymbol=NULL;
#endif
  // traverse the AST and find locations that need to be transformed
  traverse(project, preorder);
//  ROSE_ASSERT(rememberTopNode);

  // insert: RuntimeSystem* runtimeSystem = new RuntimeSystem();
  //insertRuntimeSystemClass();

  // ---------------------------------------
  // Perform all transformations...
  // ---------------------------------------
  cerr << "\n Number of Elements in create_array_call  : " << create_array_call.size() << endl;
  std::map<SgInitializedName*, SgExpression*>::const_iterator it=create_array_call.begin();
  for (;it!=create_array_call.end();it++) {
    SgInitializedName* array_node = it->first;
    SgExpression* array_size = it->second;
    insertArrayCreateCall(array_node,array_size);
  }

  cerr << "\n Number of Elements in create_array_access_call  : " << create_array_access_call.size() << endl;
  std::map<SgVarRefExp*, SgExpression*>::const_iterator ita=create_array_access_call.begin();
  for (;ita!=create_array_access_call.end();ita++) {
	SgVarRefExp* array_node = ita->first;
    SgExpression* array_size = ita->second;
    insertArrayAccessCall(array_node,array_size);
  }
}


/****************************************
 * This function returns the statement that
 * surrounds a given Node or Expression
 ****************************************/
SgStatement*
RtedTransformation::getSurroundingStatement(SgNode* n) {
  SgNode* stat = n;
  while (!isSgStatement(stat) && !isSgProject(stat)) {
    ROSE_ASSERT(stat->get_parent());
    stat=stat->get_parent();
  }
  return isSgStatement(stat);
}

/* -----------------------------------------------------------
 * Collects information needed for transformations
 * -----------------------------------------------------------*/
void
RtedTransformation::visit(SgNode* n) {
  if (n==NULL) return;
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
	cerr << ">>>> FuncDef : " << funcName << " " <<  endl;
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

  // *********************** DETECT THE rose CreateArray func call in Runtime System -- needed for Transformations ***************
  SgMemberFunctionDeclaration* roseCreateArray_tmp = isSgMemberFunctionDeclaration(n);
  if (roseCreateArray_tmp) {
    if (roseCreateArray_tmp->get_symbol_from_symbol_table()) {
    string memberName = roseCreateArray_tmp->get_symbol_from_symbol_table()->get_name().str();
    //cerr <<"Found MemberName : " << memberName << endl;
    if (memberName=="roseCreateArray") {
      cerr <<">>>>>>>>>>>>> Found MemberName : " << memberName << endl;
      roseCreateArray = isSgMemberFunctionSymbol(roseCreateArray_tmp->get_symbol_from_symbol_table());
    }
    }
  }

  // *********************** DETECT THE rose ArrayAccess func call in Runtime System -- needed for Transformations ***************
  SgMemberFunctionDeclaration* roseArrayAccess_tmp = isSgMemberFunctionDeclaration(n);
  if (roseArrayAccess_tmp) {
    if (roseArrayAccess_tmp->get_symbol_from_symbol_table()) {
    string memberName = roseArrayAccess_tmp->get_symbol_from_symbol_table()->get_name().str();
    //cerr <<"Found MemberName : " << memberName << endl;
    if (memberName=="roseArrayAccess") {
      cerr <<">>>>>>>>>>>>> Found MemberName : " << memberName << endl;
      roseArrayAccess = isSgMemberFunctionSymbol(roseArrayAccess_tmp->get_symbol_from_symbol_table());
    }
    }
  }

  // ******************** DETECT Member functions in RuntimeSystem.h *************************************************************



  // ******************** DETECT functions in input program  *********************************************************************

  // *********************** DETECT ALL array creations ***************
  SgInitializedName* initName = isSgInitializedName(n);
  if (initName) {
    // lets see if we assign an array here
    SgType* type = initName->get_type();
    SgArrayType* array = isSgArrayType(type);
    if (array) {
      SgExpression * expr = array->get_index();
      //cerr <<"Found SgInitalizedName with type : " << type->class_name() << " array expr: " << expr << endl;
      if (expr!=NULL)
    	  create_array_call[initName]=expr;
    }
  }

  // *********************** DETECT ALL array accesses ***************
  SgPntrArrRefExp* arrRefExp = isSgPntrArrRefExp(n);
  if (arrRefExp) {
	 SgExpression* left = arrRefExp->get_lhs_operand();
	 // right hand side can be any expression!
	 SgExpression* right = arrRefExp->get_rhs_operand();
	 SgVarRefExp* arrayNameRef = isSgVarRefExp(left);
	 if (arrayNameRef==NULL) {
		 SgArrowExp* arrow = isSgArrowExp(left);
		 if (arrow) {
			 //cerr << " left : " << arrow->get_lhs_operand()->class_name() << endl;
			 //cerr << " right : " << arrow->get_rhs_operand()->class_name() << endl;
			arrayNameRef = isSgVarRefExp(arrow->get_rhs_operand());
			ROSE_ASSERT(arrayNameRef);
		 } else {
			 cerr << ">> RtedTransformation::SgPntrArrRefExp:: unknown left side : " << left->class_name() <<
			 " --" << arrRefExp->unparseToString() << "-- " <<endl;
			 return;
		 }
	 }
	 create_array_access_call[arrayNameRef]=right;
  }

  // ******************** DETECT functions in input program  *********************************************************************

}

