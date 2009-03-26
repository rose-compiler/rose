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
RtedTransformation::insertArrayCreateCall(SgNode* n, SgExpression* value) {
  ROSE_ASSERT(n);
  SgInitializedName* initName =NULL;
  if (isSgVarRefExp(n)) {
    SgVariableSymbol* varSymbol = isSgVarRefExp(n)->get_symbol();
    ROSE_ASSERT(varSymbol);
    initName= varSymbol->get_declaration();
    ROSE_ASSERT(initName);
  }

  SgStatement* stmt = getSurroundingStatement(n);
  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    string name = "";
    if (isSgVarRefExp(n))
      name= initName->get_mangled_name().str();
    else if (isSgInitializedName(n))
      name=isSgInitializedName(n)->get_mangled_name().str();

    ROSE_ASSERT(scope);
    if (isSgBasicBlock(scope) ) {
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
    } else if (isSgClassDefinition(scope)) {
      //cerr <<"RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << endl;
    } else if (isSgGlobal(scope)) {
      //cerr <<"RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << endl;
    }  else if (isSgNamespaceDefinitionStatement(scope)) {
      //cerr <<"RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << endl;
    } 
    else {
      cerr <<"RuntimeInstrumentation :: Surrounding Block is not Block! : " << name << " : " << scope->class_name() << endl;
      ROSE_ASSERT(false);
    }

  } else {
    cerr << "RuntimeInstrumentation :: Surrounding Statement could not be found! " <<endl;
    ROSE_ASSERT(false);
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
    cerr << "Creating pdf..." << endl;
    AstPDFGeneration pdf;
    pdf.generateWithinFile(source);
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
  cerr << "\n Number of Elements in create_array_call_initName  : " << create_array_call_initName.size() << endl;
  std::map<SgInitializedName*, SgExpression*>::const_iterator iti=create_array_call_initName.begin();
  for (;iti!=create_array_call_initName.end();iti++) {
    SgInitializedName* array_node = iti->first;
    SgExpression* array_size = iti->second;
    cerr << ">>> INserting array create (INITNAME): " << array_node->unparseToString() << 
      "  size : " << array_size->unparseToString() << endl;
    insertArrayCreateCall(array_node,array_size);
  }

  cerr << "\n Number of Elements in create_array_call_varRef  : " << create_array_call_varRef.size() << endl;
  std::map<SgVarRefExp*, SgExpression*>::const_iterator itv=create_array_call_varRef.begin();
  for (;itv!=create_array_call_varRef.end();itv++) {
    SgVarRefExp* array_node = itv->first;
    SgExpression* array_size = itv->second;
    cerr << ">>> INserting array create (VARREF): " << array_node->unparseToString() << 
      "  size : " << array_size->unparseToString() << endl;
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
 * This function returns InitializedName
 * for a DotExpr
 ****************************************/
SgInitializedName*
RtedTransformation::getRightOfDot(SgDotExp* dot ,std::string str) {
  SgInitializedName* initName = NULL;
  SgExpression* rightDot = dot->get_rhs_operand();
  ROSE_ASSERT(rightDot);
  SgVarRefExp* varRef = isSgVarRefExp(rightDot);
  if (varRef) {
    initName = varRef->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : " << rightDot->class_name() << endl;
    ROSE_ASSERT(false);
  }
  return initName;
}

/****************************************
 * This function returns InitializedName
 * for a ArrowExp
 ****************************************/
SgInitializedName*
RtedTransformation::getRightOfArrow(SgArrowExp* arrow ,std::string str) {
  SgInitializedName* initName = NULL;
  SgExpression* rightArrow = arrow->get_rhs_operand();
  ROSE_ASSERT(rightArrow);
  SgVarRefExp* varRef = isSgVarRefExp(rightArrow);
  if (varRef) {
    initName = varRef->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : " << rightArrow->class_name() << endl;
    ROSE_ASSERT(false);
  }
  return initName;
}

/****************************************
 * This function returns InitializedName
 * for a PlusPlusOp
 ****************************************/
SgInitializedName*
RtedTransformation::getPlusPlusOp(SgPlusPlusOp* plus ,std::string str) {
  SgInitializedName* initName = NULL;
  SgExpression* expPl = plus->get_operand();
  ROSE_ASSERT(expPl);
  SgVarRefExp* varRef = isSgVarRefExp(expPl);
  if (varRef) {
    initName = varRef->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : " << expPl->class_name() <<endl;
    ROSE_ASSERT(false);
  }
  return initName;
}

/****************************************
 * This function returns InitializedName
 * for a MinusMinusOp
 ****************************************/
SgInitializedName*
RtedTransformation::getMinusMinusOp(SgMinusMinusOp* minus ,std::string str) {
  SgInitializedName* initName = NULL;
  SgExpression* expPl = minus->get_operand();
  ROSE_ASSERT(expPl);
  SgVarRefExp* varRef = isSgVarRefExp(expPl);
  if (varRef) {
    initName = varRef->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : " << expPl->class_name() <<endl;
    ROSE_ASSERT(false);
  }
  return initName;
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
  if (isSgInitializedName(n)) {
    SgInitializedName* initName = isSgInitializedName(n);
    // STACK ARRAY : lets see if we assign an array here
    SgType* type = initName->get_type();
    SgArrayType* array = isSgArrayType(type);
    if (array) {
      SgExpression * expr = array->get_index();
      //cerr <<"Found SgInitalizedName with type : " << type->class_name() << " array expr: " << expr << endl;
      if (expr!=NULL)
	create_array_call_initName[initName]=expr;
    }
  }

  // look for MALLOC
  if (isSgAssignOp(n)) {
    SgAssignOp* assign = isSgAssignOp(n);
    SgInitializedName* initName = NULL;
    // left hand side of assign
    SgExpression* expr_l = assign->get_lhs_operand();
    // right hand side of assign
    SgExpression* expr_r = assign->get_rhs_operand();
    
    // left side contains SgInitializedName somewhere ... search
    SgVarRefExp* varRef = isSgVarRefExp(expr_l);
    SgPntrArrRefExp* pntrArr = isSgPntrArrRefExp(expr_l);
    SgPointerDerefExp* pointerDeref = isSgPointerDerefExp(expr_l);
    if  (varRef) {
      // is variable on left side
      initName = varRef->get_symbol()->get_declaration();
    } // ------------------------------------------------------------
    else if (pntrArr) {
      // is array on left side
      SgExpression* expr_ll = pntrArr->get_lhs_operand();
      ROSE_ASSERT(expr_ll);
      varRef = isSgVarRefExp(expr_ll);
      if (varRef) {
	initName = varRef->get_symbol()->get_declaration();
      } else if (isSgPntrArrRefExp(expr_ll)) {
	SgPntrArrRefExp* pntrArr2 = isSgPntrArrRefExp(expr_ll);
	ROSE_ASSERT(pntrArr2);
	SgExpression* expr_lll = pntrArr2->get_lhs_operand();
	varRef = isSgVarRefExp(expr_lll);
	if (varRef) {
	  initName = varRef->get_symbol()->get_declaration();
	}  else if (isSgDotExp(expr_lll)) {
	  initName = getRightOfDot(isSgDotExp(expr_lll),"Left of pntrArr2 - Right of Dot  - line: " + expr_lll->unparseToString()+ " ");
	}  else if (isSgArrowExp(expr_lll)) {
	  initName = getRightOfArrow(isSgArrowExp(expr_lll),"Left of pntrArr2 - Right of Arrow  - line: " + expr_lll->unparseToString()+ " ");
	}else {
	  cerr << "RtedTransformation : Left of pntrArr2 - Unknown : " << expr_lll->class_name() << endl;
	  ROSE_ASSERT(false);
	}
      } else if (isSgDotExp(expr_ll)) {
	initName = getRightOfDot(isSgDotExp(expr_ll),"Left of pntrArr - Right of Dot  - line: " + expr_ll->unparseToString()+ " ");
      }  else if (isSgArrowExp(expr_ll)) {
	initName = getRightOfArrow(isSgArrowExp(expr_ll),"Left of pntrArr - Right of Arrow  - line: " + expr_ll->unparseToString()+ " ");
      } else {
	cerr << "RtedTransformation : Left of pntrArr - Unknown : " << expr_ll->class_name() <<
	  "  line:" << expr_ll->unparseToString() <<endl;
	ROSE_ASSERT(false);
      }
    } // ------------------------------------------------------------
    else if (isSgDotExp(expr_l)) {
      initName = getRightOfDot(isSgDotExp(expr_l),"Right of Dot  - line: " + expr_l->unparseToString()+ " ");
    }// ------------------------------------------------------------
    else if (isSgArrowExp(expr_l)) {
      initName = getRightOfArrow(isSgArrowExp(expr_l),"Right of Arrow  - line: " + expr_l->unparseToString()+ " ");
    } // ------------------------------------------------------------
    else if (pointerDeref) {
      SgExpression* exp = pointerDeref->get_operand();
      ROSE_ASSERT(exp);
      if (isSgPlusPlusOp(exp)) {
	initName = getPlusPlusOp(isSgPlusPlusOp(exp),"PointerDerefExp PlusPlus  - line: " + pointerDeref->unparseToString()+ " ");
      } else if (isSgMinusMinusOp(exp)) {
	initName = getMinusMinusOp(isSgMinusMinusOp(exp),"PointerDerefExp MinusMinus  - line: " + pointerDeref->unparseToString()+ " ");
      } else {
	cerr << "RtedTransformation : PointerDerefExp - Unknown : " << exp->class_name() << 
	  "  line:" << pointerDeref->unparseToString() <<endl;
	ROSE_ASSERT(false);
      }
    } // ------------------------------------------------------------
    else if (isSgFunctionCallExp(expr_l)) {
      cerr << "RtedTransformation: UNHANDLED BUT ACCEPTED FOR NOW - Left of assign - Unknown : " << expr_l->class_name() << 
	  "  line:" << expr_l->unparseToString() <<endl;
    } else{
      cerr << "RtedTransformation : Left of assign - Unknown : " << expr_l->class_name() << 
	  "  line:" << expr_l->unparseToString() <<endl;
      ROSE_ASSERT(false);
    }

    vector <SgNode*> calls = NodeQuery::querySubTree(expr_r,V_SgFunctionCallExp);
    vector <SgNode*>::const_iterator it = calls.begin();
    for (;it!=calls.end();++it) {
      SgFunctionCallExp* funcc = isSgFunctionCallExp(*it);
      if (funcc) {
	// MALLOC : function call
	SgExprListExp* args = funcc->get_args();
	SgExpression* func = funcc->get_function();
	if (func && args) {
	  bool ismalloc=false;
	  SgFunctionRefExp* funcr = isSgFunctionRefExp(func);
	  if (funcr) {
	    SgFunctionDeclaration* funcd = funcr->getAssociatedFunctionDeclaration();
	    ROSE_ASSERT(funcd);
	    string funcname = funcd->get_name().str();
	    if (funcname=="malloc") {
	      ismalloc=true;
	      cerr << "... Detecting func call on right hand side : " << funcname << endl;
	    }
	  }  else {
	    // right hand side of assign should only contain call to malloc somewhere
	    cerr << "RtedTransformation: UNHANDLED AND ACCEPTED FOR NOW. Right of Assign : Unknown (Array creation) : "
		 << func->class_name() << "  line:" << funcc->unparseToString() <<endl;
	    //	    ROSE_ASSERT(false);
	  }
	  if (ismalloc) {
	    ROSE_ASSERT(initName);
	    ROSE_ASSERT(varRef);
	    cerr << "... Creating arrow malloc with initName : " << initName->unparseToString() << " " << args->unparseToString() <<endl;
	    create_array_call_varRef[varRef]=args;
	  }
	}
      }
    }
  }

  // *********************** DETECT ALL array creations ***************





  // *********************** DETECT ALL array accesses ***************
  SgPntrArrRefExp* arrRefExp = isSgPntrArrRefExp(n);
  if (arrRefExp) {
    SgExpression* left = arrRefExp->get_lhs_operand();
    // right hand side can be any expression!
    SgExpression* right = arrRefExp->get_rhs_operand();
    ROSE_ASSERT(right);
    SgVarRefExp* varRef = isSgVarRefExp(left);
    if (varRef==NULL) {
      SgArrowExp* arrow = isSgArrowExp(left);
      SgPntrArrRefExp* arrRefExp2 = isSgPntrArrRefExp(left);
      if (arrow) {
	varRef = isSgVarRefExp(arrow->get_rhs_operand());
	ROSE_ASSERT(varRef);
      } else if (arrRefExp2) { 
	SgExpression* expr2 = arrRefExp2->get_lhs_operand();
	varRef = isSgVarRefExp(expr2);
	if (varRef) {
	  // do nothing
	} else {
	  cerr << ">> RtedTransformation::ACCESS::SgPntrArrRefExp:: unknown left of SgPntrArrRefExp2: " << expr2->class_name() <<
	    " --" << arrRefExp->unparseToString() << "-- " <<endl;
	  ROSE_ASSERT(false);
	}
      } else {
	cerr << ">> RtedTransformation::ACCESS::SgPntrArrRefExp:: unknown left of SgArrowExp: " << left->class_name() <<
	  " --" << arrRefExp->unparseToString() << "-- " <<endl;
	ROSE_ASSERT(false);
      }
    }
    create_array_access_call[varRef]=right;
  }

  // ******************** DETECT functions in input program  *********************************************************************

}

