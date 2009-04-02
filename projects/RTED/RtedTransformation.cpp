#include <rose.h>
#include <string>
#include "RtedSymbols.h"
#include "DataStructures.h"
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
  SgProject* project = frontend(argc, argv);
  ROSE_ASSERT(project);
  return project;
}

/* -----------------------------------------------------------
 * Perform Transformation: insertArrayCreateCall
 * -----------------------------------------------------------*/
void RtedTransformation::insertArrayCreateCall(SgVarRefExp* n, RTedArray* value) {
  ROSE_ASSERT(value);
  SgInitializedName* initName = n->get_symbol()->get_declaration();
  ROSE_ASSERT(initName);
  SgStatement* stmt = getSurroundingStatement(n);
  insertArrayCreateCall(stmt, initName, value);
}

void RtedTransformation::insertArrayCreateCall(SgInitializedName* initName,
					       RTedArray* value) {
  ROSE_ASSERT(value);
  ROSE_ASSERT(initName);
  SgStatement* stmt = getSurroundingStatement(initName);
  insertArrayCreateCall(stmt, initName, value);
}


void 
RtedTransformation::insertMainCloseCall(SgStatement* stmt) {
  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    ROSE_ASSERT(scope);
    if (isSgBasicBlock(scope)) {
      SgVarRefExp* varRef_l =
	buildVarRefExp("runtimeSystem", globalScope);
      string symbolName = varRef_l->get_symbol()->get_name().str();
      ROSE_ASSERT(roseRtedClose);
      string symbolName2 = roseRtedClose->get_name().str();
      //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
      SgMemberFunctionRefExp* memRef_r = buildMemberFunctionRefExp(
								   roseRtedClose, false, true);
      SgArrowExp* sgArrowExp = buildArrowExp(varRef_l, memRef_r);
      SgFunctionCallExp* funcCallExp = buildFunctionCallExp(sgArrowExp,
							    NULL);
      SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
      //cerr << " Last statement in main : " << stmt->class_name() << "  insertBefore : " << insertMainBeforeLast << endl;
      if (insertMainBeforeLast)
	insertStatementBefore(isSgStatement(stmt), exprStmt);
      else
	insertStatementAfter(isSgStatement(stmt), exprStmt);
    }

  } else {
    cerr
      << "RuntimeInstrumentation :: Surrounding Statement could not be found! "
      << endl;
    ROSE_ASSERT(false);
  }
}

void RtedTransformation::insertArrayCreateCall(SgStatement* stmt,
					       SgInitializedName* initName, RTedArray* array) {
  std::vector<SgExpression*> value;
  array->getIndices(value);
  int dimension = array->dimension;
  bool stack = array->stack;
  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    string name = initName->get_mangled_name().str();

    ROSE_ASSERT(scope);
    // what if there is an array creation within a ClassDefinition
    if ( isSgClassDefinition(scope)) {
      // new stmt = the classdef scope
      SgClassDeclaration* decl = isSgClassDeclaration(scope->get_parent());
      ROSE_ASSERT(decl);
      stmt = isSgVariableDeclaration(decl->get_parent());
      if (!stmt) {
	cerr << " Error . stmt is unknown : " << decl->get_parent()->class_name() << endl;
	exit(1);
      } 
      scope = scope->get_scope();
      // We want to insert the stmt before this classdefinition, if its still in a valid block
      cerr <<" ....... Found ClassDefinition Scope. New Scope is : " << scope->class_name() << "  stmt:" << stmt->class_name() <<endl;
    }
    // what is there is an array creation in a global scope
    else if (isSgGlobal(scope)) {
      cerr <<"RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << endl;
      // We need to add this new statement to the beginning of main
      // get the first statement in main as stmt
      stmt = mainFirst;
      scope=stmt->get_scope();
    }
    if (isSgBasicBlock(scope)) {
      // build the function call : runtimeSystem-->createArray(params); ---------------------------
      SgStringVal* callNameExp = buildStringVal(name);
      SgIntVal* dimExpr = buildIntVal(dimension);
      SgBoolValExp* stackExpr = buildBoolValExp(stack);

      SgExprListExp* arg_list = buildExprListExp();
      appendExpression(arg_list, callNameExp);
      appendExpression(arg_list, dimExpr);
      appendExpression(arg_list, stackExpr);
      std::vector<SgExpression*>::const_iterator it = value.begin();
      for (; it != value.end(); ++it) {
	SgExpression* expr = isSgExpression(*it);
	if (expr == NULL)
	  expr = buildIntVal(-1);
	ROSE_ASSERT(expr);
	appendExpression(arg_list, expr);
      }
      SgExpression* filename = buildStringVal(stmt->get_file_info()->get_filename());
      SgExpression* linenr = buildIntVal(stmt->get_file_info()->get_line());
      appendExpression(arg_list, filename);
      appendExpression(arg_list, linenr);
      SgVarRefExp* varRef_l =
	buildVarRefExp("runtimeSystem", globalScope);
      string symbolName = varRef_l->get_symbol()->get_name().str();
      //cerr << " >>>>>>>> Symbol VarRef: " << symbolName << endl;

      ROSE_ASSERT(roseCreateArray);
      string symbolName2 = roseCreateArray->get_name().str();
      //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
      SgMemberFunctionRefExp* memRef_r = buildMemberFunctionRefExp(
								   roseCreateArray, false, true);
      SgArrowExp* sgArrowExp = buildArrowExp(varRef_l, memRef_r);

      SgFunctionCallExp* funcCallExp = buildFunctionCallExp(sgArrowExp,
							    arg_list);
      SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
      // insert new stmt (exprStmt) before (old) stmt
      insertStatementBefore(isSgStatement(stmt), exprStmt);
    } 
    else if (isSgNamespaceDefinitionStatement(scope)) {
      cerr <<"RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << endl;
    } else {
      cerr
	<< "RuntimeInstrumentation :: Surrounding Block is not Block! : "
	<< name << " : " << scope->class_name() << endl;
      ROSE_ASSERT(false);
    }
  } else {
    cerr
      << "RuntimeInstrumentation :: Surrounding Statement could not be found! "
      << endl;
    ROSE_ASSERT(false);
  }
}

/* -----------------------------------------------------------
 * Perform Transformation: insertArrayCreateAccessCall
 * -----------------------------------------------------------*/
void RtedTransformation::insertArrayAccessCall(SgVarRefExp* varRef,
					       RTedArray* value) {
  ROSE_ASSERT(varRef);
  SgStatement* stmt = getSurroundingStatement(varRef);
  SgVariableSymbol* varSymbol = varRef->get_symbol();
  ROSE_ASSERT(varSymbol);
  SgInitializedName* initName = varSymbol->get_declaration();
  ROSE_ASSERT(initName);
  if (stmt)
    cerr <<"   Processing call : " << stmt->unparseToString() << " " << initName->unparseToString() << endl;
  insertArrayAccessCall(stmt, initName, value);
}

void RtedTransformation::insertArrayAccessCall(SgStatement* stmt,
					       SgInitializedName* initName, RTedArray* array) {
  std::vector<SgExpression*> value;
  array->getIndices(value);

  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    ROSE_ASSERT(scope);
    string name = initName->get_mangled_name().str();
    SgStringVal* callNameExp = buildStringVal(name);

    SgExprListExp* arg_list = buildExprListExp();
    appendExpression(arg_list, callNameExp);
    appendExpression(arg_list, array->indx1);
    SgExpression* expr = NULL;
    
    if (array->indx2)
      appendExpression(arg_list, array->indx2);
    else {
      if (array->dimension==1)
	expr = buildIntVal(-1); // call [i,-1]
      else
	expr = buildIntVal(0); // call [i,0]
      appendExpression(arg_list, expr);
    }
    SgExpression* filename = buildStringVal(stmt->get_file_info()->get_filename());
    SgExpression* linenr = buildIntVal(stmt->get_file_info()->get_line());
    appendExpression(arg_list, filename);
    appendExpression(arg_list, linenr);

    //cerr << "Adding runtime ---------------- " << endl;

    SgVarRefExp* varRef_l = buildVarRefExp("runtimeSystem", globalScope);
    string symbolName = varRef_l->get_symbol()->get_name().str();
    //cerr << " >>>>>>>> Symbol VarRef: " << symbolName << endl;

    ROSE_ASSERT(roseArrayAccess);
    string symbolName2 = roseArrayAccess->get_name().str();
    //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
    SgMemberFunctionRefExp* memRef_r = buildMemberFunctionRefExp(
								 roseArrayAccess, false, true);
    SgArrowExp* sgArrowExp = buildArrowExp(varRef_l, memRef_r);

    SgFunctionCallExp* funcCallExp = buildFunctionCallExp(sgArrowExp,
							  arg_list);
    SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
    insertStatementBefore(isSgStatement(stmt), exprStmt);
    //    }

  } else {
    cerr
      << "RuntimeInstrumentation :: Surrounding Statement could not be found! "
      << endl;
    exit(0);
  }
}

/* -----------------------------------------------------------
 * Insert the header files (Step 1)
 * -----------------------------------------------------------*/
void RtedTransformation::insertProlog(SgProject* proj) {
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
void RtedTransformation::insertRuntimeSystemClass() {
#if 0

  Sg_File_Info* fileInfo = globalScope->get_file_info();
  ROSE_ASSERT(runtimeClassSymbol);
  ROSE_ASSERT(runtimeClassSymbol->get_type());
  SgType* type = runtimeClassSymbol->get_type();
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
void RtedTransformation::transform(SgProject* project) {
  cout << "Running Transformation..." << endl;
  globalScope = getFirstGlobalScope(isSgProject(project));

#if 0
  rememberTopNode=NULL;
  runtimeClassSymbol=NULL;
#endif
  // traverse the AST and find locations that need to be transformed

  symbols->traverse(project, preorder);
  roseCreateArray = symbols->roseCreateArray;
  roseArrayAccess = symbols->roseArrayAccess;
  roseRtedClose = symbols->roseRtedClose;
  ROSE_ASSERT(roseCreateArray);
  ROSE_ASSERT(roseArrayAccess);

  traverseInputFiles(project,preorder);
  //  ROSE_ASSERT(rememberTopNode);

  // insert: RuntimeSystem* runtimeSystem = new RuntimeSystem();
  //insertRuntimeSystemClass();

  // ---------------------------------------
  // Perform all transformations...
  // ---------------------------------------

  cerr
    << "\n Number of Elements in create_array_define_varRef_multiArray  : "
    << create_array_define_varRef_multiArray.size() << endl;
  std::map<SgVarRefExp*, RTedArray*>::const_iterator itm =
    create_array_define_varRef_multiArray.begin();
  for (; itm != create_array_define_varRef_multiArray.end(); itm++) {
    SgVarRefExp* array_node = itm->first;
    RTedArray* array_size = itm->second;
    //cerr << ">>> INserting array create (VARREF): "
    //		<< array_node->unparseToString() << "  size : "
    //		<< array_size->unparseToString() << endl;
    insertArrayCreateCall(array_node, array_size);
  }

  cerr
    << "\n Number of Elements in create_array_define_varRef_multiArray_stack  : "
    << create_array_define_varRef_multiArray_stack.size() << endl;
  std::map<SgInitializedName*, RTedArray*>::const_iterator itv =
    create_array_define_varRef_multiArray_stack.begin();
  for (; itv != create_array_define_varRef_multiArray_stack.end(); itv++) {
    SgInitializedName* array_node = itv->first;
    RTedArray* array_size = itv->second;
    //cerr << ">>> INserting array create (VARREF): "
    //		<< array_node->unparseToString() << "  size : "
    //		<< array_size->unparseToString() << endl;
    insertArrayCreateCall(array_node, array_size);
  }

  cerr << "\n Number of Elements in create_array_access_call  : "
       << create_array_access_call.size() << endl;
  std::map<SgVarRefExp*, RTedArray*>::const_iterator ita =
    create_array_access_call.begin();
  for (; ita != create_array_access_call.end(); ita++) {
    SgVarRefExp* array_node = ita->first;
    RTedArray* array_size = ita->second;
    insertArrayAccessCall(array_node, array_size);
  }

  // insert main call to ->close();
  insertMainCloseCall(mainLast);
}

/****************************************
 * This function returns InitializedName
 * for a DotExpr
 ****************************************/
SgInitializedName*
RtedTransformation::getRightOfDot(SgDotExp* dot, std::string str) {
  SgInitializedName* initName = NULL;
  SgExpression* rightDot = dot->get_rhs_operand();
  ROSE_ASSERT(rightDot);
  SgVarRefExp* varRef = isSgVarRefExp(rightDot);
  if (varRef) {
    initName = varRef->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : "
	 << rightDot->class_name() << endl;
    ROSE_ASSERT(false);
  }
  return initName;
}

/****************************************
 * This function returns InitializedName
 * for a ArrowExp
 ****************************************/
SgInitializedName*
RtedTransformation::getRightOfArrow(SgArrowExp* arrow, std::string str) {
  SgInitializedName* initName = NULL;
  SgExpression* rightArrow = arrow->get_rhs_operand();
  ROSE_ASSERT(rightArrow);
  SgVarRefExp* varRef = isSgVarRefExp(rightArrow);
  if (varRef) {
    initName = varRef->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : "
	 << rightArrow->class_name() << endl;
    ROSE_ASSERT(false);
  }
  return initName;
}

/****************************************
 * This function returns InitializedName
 * for a PlusPlusOp
 ****************************************/
SgInitializedName*
RtedTransformation::getPlusPlusOp(SgPlusPlusOp* plus, std::string str) {
  SgInitializedName* initName = NULL;
  SgExpression* expPl = plus->get_operand();
  ROSE_ASSERT(expPl);
  SgVarRefExp* varRef = isSgVarRefExp(expPl);
  if (varRef) {
    initName = varRef->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : "
	 << expPl->class_name() << endl;
    ROSE_ASSERT(false);
  }
  return initName;
}

/****************************************
 * This function returns InitializedName
 * for a MinusMinusOp
 ****************************************/
SgInitializedName*
RtedTransformation::getMinusMinusOp(SgMinusMinusOp* minus, std::string str) {
  SgInitializedName* initName = NULL;
  SgExpression* expPl = minus->get_operand();
  ROSE_ASSERT(expPl);
  SgVarRefExp* varRef = isSgVarRefExp(expPl);
  if (varRef) {
    initName = varRef->get_symbol()->get_declaration();
  } else {
    cerr << "RtedTransformation : " << str << " - Unknown : "
	 << expPl->class_name() << endl;
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
    stat = stat->get_parent();
  }
  return isSgStatement(stat);
}

SgVarRefExp*
RtedTransformation::resolveToVarRefRight(SgExpression* expr) {
  SgVarRefExp* result = NULL;
  SgExpression* newexpr = NULL;
  ROSE_ASSERT(expr);
  if (isSgDotExp(expr)) {
    newexpr= isSgDotExp(expr)->get_rhs_operand();
    result = isSgVarRefExp(newexpr);
    if (!result) {
      cerr <<"  >> resolveToVarRefRight : right : " << newexpr->class_name() << endl;
      exit(1);
    }
  } else {
    cerr <<" >> resolveToVarRefRight : unknown expression " << expr->class_name() <<endl;
    exit(1);
  }

  return result;
}

SgVarRefExp*
RtedTransformation::resolveToVarRefLeft(SgExpression* expr) {
  SgVarRefExp* result = NULL;
  SgExpression* newexpr = NULL;
  ROSE_ASSERT(expr);
  if (isSgDotExp(expr)) {
    newexpr= isSgDotExp(expr)->get_lhs_operand();
    result = isSgVarRefExp(newexpr);
    if (!result) {
      cerr <<"  >> resolveToVarRefRight : right : " << newexpr->class_name() << endl;
      exit(1);
    }
  } else {
    cerr <<" >> resolveToVarRefRight : unknown expression " << expr->class_name() <<endl;
    exit(1);
  }
  
  return result;
}

int RtedTransformation::getDimension(SgInitializedName* initName) {
  ROSE_ASSERT(initName);
  int dimension = 0;
  SgType* type = initName->get_type();
  ROSE_ASSERT(type);
  if (isSgArrayType(type)) {
    dimension++;
  } else {
    while (isSgPointerType(type) && !isSgPointerMemberType(type)) {
      SgPointerType* pointer = isSgPointerType(type);
      ROSE_ASSERT(pointer);
      type = pointer->dereference();
      ROSE_ASSERT(type);
      dimension++;
      ROSE_ASSERT(dimension<10);
      //cerr << "Dimension : " << dimension << "  : " << type->class_name() << endl;
    }
  }
  return dimension;
}

int 
RtedTransformation::getDimension(SgInitializedName* initName, SgVarRefExp* varRef) {
  int dim =-1;
  std::map<SgVarRefExp*, RTedArray*>::const_iterator it = create_array_define_varRef_multiArray.begin();
  for (;it!=create_array_define_varRef_multiArray.end();++it) {
    RTedArray* array = it->second;
    SgInitializedName* init = array->initName;
    if (init==initName) {
      dim=array->dimension;
      cerr << "Found init : " << init->unparseToString() << " dim : " << dim << "  compare to : " << initName->unparseToString()<<endl;
    }
  }

  std::map<SgInitializedName*, RTedArray*>::const_iterator it2= create_array_define_varRef_multiArray_stack.find(initName);
  for (;it2!=create_array_define_varRef_multiArray_stack.end();++it2) {
    RTedArray* array = it2->second;
    SgInitializedName* init = array->initName;
    if (init==initName) {
      dim=array->dimension;
    }
  }
  cerr << " -------------------------- resizing dimension to : " << dim << "  for : " << varRef->unparseToString() << endl;
  return dim;
}


/* -----------------------------------------------------------
 * Collects information needed for transformations
 * -----------------------------------------------------------*/

void RtedTransformation::visit(SgNode* n) {

  // find MAIN ******************************************
  if (isSgFunctionDefinition(n)) {
    SgFunctionDefinition* mainFunc = isSgFunctionDefinition(n);
    ROSE_ASSERT(mainFunc);
    string funcname = mainFunc->get_declaration()->get_name().str();
    if (funcname == "main") {
      // find the last statement
      SgBasicBlock* block = mainFunc->get_body();
      ROSE_ASSERT(block);
      Rose_STL_Container<SgStatement*> stmts = block->get_statements();
      SgStatement* first = stmts.front();
      SgStatement* last = stmts.back();
      if (isSgReturnStmt(last)) 
	insertMainBeforeLast = true;
      else
	insertMainBeforeLast=false;
      //cerr << " Last statement in main : " << last->class_name() << "  insertBefore : " << 
      //	RoseBin_support::resBool(insertMainBeforeLast) << endl;
      ROSE_ASSERT(last);
      // insert call to close before last statement (could be return)
      mainLast = last;
      mainFirst = first;
    }

  }

  // find MAIN ******************************************


  // ******************** DETECT functions in input program  *********************************************************************

  // *********************** DETECT ALL array creations ***************
  else if (isSgInitializedName(n)) {
    SgInitializedName* initName = isSgInitializedName(n);
    int dimension = 0;
    dimension = getDimension(initName);
    // STACK ARRAY : lets see if we assign an array here
    SgType* type = initName->get_typeptr();
    SgArrayType* array = isSgArrayType(type);
    if (array) {
      SgExpression * expr = array->get_index();
      SgExpression * expr2 = NULL;
      SgType* basetype = array->get_base_type();
      SgArrayType* array2 = isSgArrayType(basetype);
      //SgExprListExp* dim_info = array->get_dim_info();
      cerr << " unparse : " << array->unparseToString() << endl;
      if (array2) {
	dimension++;
	expr2=array2->get_index();
	cerr << " array2 : "<<array2->class_name()<<" expr2 : " << expr2->unparseToString() << endl;
	ROSE_ASSERT(expr2);
      } else {
	cerr << " file : " << initName->get_file_info()->get_filename() << "  line  : " << initName->get_file_info()->get_line()<<endl;
	cerr << " ---------------------PROBLEM :  Dim_info == NULL  Type : " << type->class_name() <<endl;
      }
      if (expr != NULL) {
	cerr << "Found stack array: " << initName->unparseToString()
	     << " " << type->class_name() << " array expr: "
	     << expr->unparseToString() << "  dim: " << dimension
	     << endl;
	if (expr2)
	  cerr << "   Expr2 : " << expr2->unparseToString() << endl;
	ROSE_ASSERT(dimension>0);
	RTedArray* array = new RTedArray(true, dimension, initName,
					 expr, expr2);
	create_array_define_varRef_multiArray_stack[initName] = array;
      }
    }
  }

  // look for MALLOC
  else if (isSgAssignOp(n)) {
    SgAssignOp* assign = isSgAssignOp(n);
    SgInitializedName* initName = NULL;
    // left hand side of assign
    SgExpression* expr_l = assign->get_lhs_operand();
    // right hand side of assign
    SgExpression* expr_r = assign->get_rhs_operand();

    // varRef ([indx1][indx2]) = malloc (size); // total array alloc
    // varRef [indx1]([]) = malloc (size); // indx2 array alloc
    SgExpression* indx1 = NULL;
    SgExpression* indx2 = NULL;
    int dimension = 0;

    // left side contains SgInitializedName somewhere ... search
    SgVarRefExp* varRef = isSgVarRefExp(expr_l);
    SgPntrArrRefExp* pntrArr = isSgPntrArrRefExp(expr_l);
    SgPointerDerefExp* pointerDeref = isSgPointerDerefExp(expr_l);
    if (varRef) {
      // is variable on left side
      // could be something like int** pntr; pntr = malloc ... (double array)
      // assume in this case explicitly pntr[indx1]=...
      initName = varRef->get_symbol()->get_declaration();
    } // ------------------------------------------------------------
    else if (pntrArr) {
      // is array on left side
      // could be pntr[indx1]  or pntr[indx1][indx2]
      SgExpression* expr_ll = pntrArr->get_lhs_operand();
      indx1 = pntrArr->get_rhs_operand();
      ROSE_ASSERT(expr_ll);
      varRef = isSgVarRefExp(expr_ll);
      if (varRef) {
	// we assume pntr[indx1] = malloc
	initName = varRef->get_symbol()->get_declaration();
      } else if (isSgPntrArrRefExp(expr_ll)) {
	SgPntrArrRefExp* pntrArr2 = isSgPntrArrRefExp(expr_ll);
	ROSE_ASSERT(pntrArr2);
	SgExpression* expr_lll = pntrArr2->get_lhs_operand();
	indx2 = pntrArr2->get_rhs_operand();
	varRef = isSgVarRefExp(expr_lll);
	if (varRef) {
	  // we assume pntr[indx1][indx2] = malloc
	  initName = varRef->get_symbol()->get_declaration();
	} else if (isSgDotExp(expr_lll)) {
	  initName = getRightOfDot(isSgDotExp(expr_lll),
				   "Left of pntrArr2 - Right of Dot  - line: "
				   + expr_lll->unparseToString() + " ");
	} else if (isSgArrowExp(expr_lll)) {
	  initName = getRightOfArrow(isSgArrowExp(expr_lll),
				     "Left of pntrArr2 - Right of Arrow  - line: "
				     + expr_lll->unparseToString() + " ");
	} else {
	  cerr
	    << "RtedTransformation : Left of pntrArr2 - Unknown : "
	    << expr_lll->class_name() << endl;
	  ROSE_ASSERT(false);
	}
      } else if (isSgDotExp(expr_ll)) {
	initName = getRightOfDot(isSgDotExp(expr_ll),
				 "Left of pntrArr - Right of Dot  - line: "
				 + expr_ll->unparseToString() + " ");
      } else if (isSgArrowExp(expr_ll)) {
	initName = getRightOfArrow(isSgArrowExp(expr_ll),
				   "Left of pntrArr - Right of Arrow  - line: "
				   + expr_ll->unparseToString() + " ");
      } else {
	cerr << "RtedTransformation : Left of pntrArr - Unknown : "
	     << expr_ll->class_name() << "  line:"
	     << expr_ll->unparseToString() << endl;
	ROSE_ASSERT(false);
      }
    } // ------------------------------------------------------------
    else if (isSgDotExp(expr_l)) {
      initName = getRightOfDot(isSgDotExp(expr_l),
			       "Right of Dot  - line: " + expr_l->unparseToString() + " ");
    }// ------------------------------------------------------------
    else if (isSgArrowExp(expr_l)) {
      initName = getRightOfArrow(isSgArrowExp(expr_l),
				 "Right of Arrow  - line: " + expr_l->unparseToString()
				 + " ");
    } // ------------------------------------------------------------
    else if (pointerDeref) {
      SgExpression* exp = pointerDeref->get_operand();
      ROSE_ASSERT(exp);
      if (isSgPlusPlusOp(exp)) {
	initName = getPlusPlusOp(isSgPlusPlusOp(exp),
				 "PointerDerefExp PlusPlus  - line: "
				 + pointerDeref->unparseToString() + " ");
      } else if (isSgMinusMinusOp(exp)) {
	initName = getMinusMinusOp(isSgMinusMinusOp(exp),
				   "PointerDerefExp MinusMinus  - line: "
				   + pointerDeref->unparseToString() + " ");
      } else {
	cerr << "RtedTransformation : PointerDerefExp - Unknown : "
	     << exp->class_name() << "  line:"
	     << pointerDeref->unparseToString() << endl;
	ROSE_ASSERT(false);
      }
    } // ------------------------------------------------------------
    else if (isSgFunctionCallExp(expr_l)) {
      cerr
	<< "RtedTransformation: UNHANDLED BUT ACCEPTED FOR NOW - Left of assign - Unknown : "
	<< expr_l->class_name() << "  line:"
	<< expr_l->unparseToString() << endl;
    } else {
      cerr << "RtedTransformation : Left of assign - Unknown : "
	   << expr_l->class_name() << "  line:"
	   << expr_l->unparseToString() << endl;
      ROSE_ASSERT(false);
    }

    // handle MALLOC
    vector<SgNode*> calls = NodeQuery::querySubTree(expr_r,
						    V_SgFunctionCallExp);
    vector<SgNode*>::const_iterator it = calls.begin();
    for (; it != calls.end(); ++it) {
      SgFunctionCallExp* funcc = isSgFunctionCallExp(*it);
      if (funcc) {
	// MALLOC : function call
	SgExprListExp* size = funcc->get_args();
	SgExpression* func = funcc->get_function();
	if (func && size) {
	  bool ismalloc = false;
	  SgFunctionRefExp* funcr = isSgFunctionRefExp(func);
	  if (funcr) {
	    SgFunctionDeclaration* funcd =
	      funcr->getAssociatedFunctionDeclaration();
	    ROSE_ASSERT(funcd);
	    string funcname = funcd->get_name().str();
	    if (funcname == "malloc") {
	      ismalloc = true;
	      cerr
		<< "... Detecting func call on right hand side : "
		<< funcname << "     and size : "
		<< size->unparseToString() << "   idx1 : "
		<< indx1 << "  idx2 : " << indx2 << endl;
	    }
	  } else {
	    // right hand side of assign should only contain call to malloc somewhere
	    cerr
	      << "RtedTransformation: UNHANDLED AND ACCEPTED FOR NOW. Right of Assign : Unknown (Array creation) : "
	      << func->class_name() << "  line:"
	      << funcc->unparseToString() << endl;
	    //	    ROSE_ASSERT(false);
	  }
	  if (ismalloc) {
	    ROSE_ASSERT(initName);
	    ROSE_ASSERT(varRef);
	    // what is the dimension of the array?
	    dimension = getDimension(initName);
	    string idx1_s = "";
	    if (indx1)
	      idx1_s = indx1->unparseToString();
	    string idx2_s = "";
	    if (indx2)
	      idx2_s = indx2->unparseToString();
	    cerr << "... Creating  malloc with initName : "
		 << initName->unparseToString() << "  size:"
		 << size->unparseToString() << "  dimension : "
		 << dimension << "  indx1 : " << idx1_s
		 << "  indx2 : " << idx2_s << endl;
	    ROSE_ASSERT(dimension>0);
#if 1
	    // because we got malloc (20 * sizeof(int)) we need to divide the allocation by int
	    SgType* varRef_type = varRef->get_type();
	    // multiplier : if arr** then we have sizeof (*int) = 8
	    int mult=0;
	    while( (isSgPointerType(varRef_type))) {
	      varRef_type= isSgPointerType(varRef_type)->get_base_type();
	      // if we dereference 2 times, then mult=2
	      mult++;
	    }
	    // divider is necessary because malloc (20 * sizeof(int)), we dont know sizeof(int)
	    // so we determine the type of the varRef on left of assign and divide the allocation
	    // e.g. (20 * sizeof(int) / 4) for arr[i]=  or (20 * sizeof(*int) /8) for arr[i][j]= 
	    int divider =0;
	    if (indx1 != NULL && indx2 == NULL) {
	      // array : pntr[i] = malloc (size)
	      // if we access pntr[i] of a 2 dim field, we make sure to decrease the multiplier
	      mult=mult/2;
	    }
	    if (isSgTypeInt(varRef_type))
	      divider=sizeof(int)*mult;
	    if (isSgTypeFloat(varRef_type))
	      divider=sizeof(float)*mult;
	    if (isSgTypeDouble(varRef_type))
	      divider=sizeof(double)*mult;
	    if (divider>0) {
	      // Change ExpressionList : size  to   (size) /divider
	      Rose_STL_Container<SgExpression*> express = size->get_expressions();
	      SgExpression* topNode = *(express.begin());
	      SgValueExp* topNodeVal = isSgValueExp(topNode);
	      // fixme : Thomas
	      if (topNodeVal) {
		SgValueExp* deepCopyTop = deepCopy(topNodeVal);
		SgExpression* div = buildDivideOp();
		SgExpression* divVal = buildIntVal(divider);
		setLhsOperand(div,deepCopyTop);
		setRhsOperand(div,divVal);
		replaceExpression(topNodeVal,div);
	      }
	    }
	    cerr <<" >>>>>>>>>>>>>>>>> array should be divided by : " << divider << "  type :" <<varRef_type->class_name() << endl;
#endif
	    if (indx1 == NULL && indx2 == NULL) {
	      // array initialized to pointer: pntr = malloc (size)
	      indx1 = size; // / divider;
	    } else if (indx1 != NULL && indx2 == NULL) {
	      // array : pntr[i] = malloc (size)
	      indx1 = indx1;
	      indx2 = size;
	    } else if (indx1 != NULL && indx2 != NULL) {
	      // array : pntr[i][j] = malloc
	      indx1 = indx1;
	      indx2 = indx2;
	      // This should right now not happend for array creation
	      ROSE_ASSERT(false);
	    } else {
	      ROSE_ASSERT(false);
	    }
	    RTedArray* array = new RTedArray(false, dimension,
					     initName, indx1, indx2);
	    // varRef can not be a array access, its only an array Create
	    createVariables.push_back(varRef);
	    create_array_define_varRef_multiArray[varRef] = array;
	  }
	}
      }
    }
  }

  // *********************** DETECT ALL array creations ***************


  // *********************** DETECT ALL array accesses ***************
  else if (isSgPntrArrRefExp(n)) {
    SgPntrArrRefExp* arrRefExp = isSgPntrArrRefExp(n);
    // make sure the parent is not another pntr array (pntr->pntr), we only want the top one
    if (!isSgPntrArrRefExp(arrRefExp->get_parent())) {

      int dimension = 1;
      SgExpression* left = arrRefExp->get_lhs_operand();
      // right hand side can be any expression!
      SgExpression* right1 = arrRefExp->get_rhs_operand();
      SgExpression* right2 = NULL;
      ROSE_ASSERT(right1);
      SgVarRefExp* varRef = isSgVarRefExp(left);
      if (varRef == NULL) {
	SgArrowExp* arrow = isSgArrowExp(left);
	SgPntrArrRefExp* arrRefExp2 = isSgPntrArrRefExp(left);
	if (arrow) {
	  varRef = isSgVarRefExp(arrow->get_rhs_operand());
	  ROSE_ASSERT(varRef);
	} else if (arrRefExp2) {
	  dimension = 2;
	  SgExpression* expr2 = arrRefExp2->get_lhs_operand();
	  right2 = arrRefExp2->get_rhs_operand();
	  varRef = isSgVarRefExp(expr2);
	  if (varRef) {
	    // do nothing
	  } else if ((varRef=resolveToVarRefRight(expr2))) {
	    ROSE_ASSERT(varRef);
	  } else {
	    cerr
	      << ">> RtedTransformation::ACCESS::SgPntrArrRefExp:: unknown left of SgPntrArrRefExp2: "
	      << expr2->class_name() << " --"
	      << arrRefExp->unparseToString() << "-- "
	      << endl;
	    ROSE_ASSERT(false);
	  }
	  ROSE_ASSERT(varRef);
	} else {
	  cerr
	    << ">> RtedTransformation::ACCESS::SgPntrArrRefExp:: unknown left of SgArrowExp: "
	    << left->class_name() << " --"
	    << arrRefExp->unparseToString() << "-- " << endl;
	  ROSE_ASSERT(false);
	}
      }
      ROSE_ASSERT(varRef);
      bool create_access_call=true;
      vector<SgVarRefExp*>::const_iterator cv = createVariables.begin();
      for (;cv!=createVariables.end();++cv) {
	SgVarRefExp* stored = *cv;
	if (stored == varRef)
	  create_access_call=false;
      }
      if (create_access_call) {
	SgInitializedName* initName =
	  varRef->get_symbol()->get_declaration();
	ROSE_ASSERT(initName);
	// check dimension again, because it could be 2dim but a call like this : arr[i] = ... (arr[i][0])
	// to check the dimension, we check if this variable was created.
	int dim = getDimension(initName, varRef);
	if (dim!=-1)
	  dimension=dim;
	RTedArray* array = NULL;
	if (right2 == NULL) {
	  array = new RTedArray(false, dimension, initName, right1,
				NULL);
	} else {
	  array = new RTedArray(false, dimension, initName, right2,
				right1);
	}
	cerr << "!! CALL : " << varRef << " - "
	     << varRef->unparseToString() << "    size : "
	     << create_array_access_call.size() << "  -- "
	     << array->unparseToString() << " : "
	     << arrRefExp->unparseToString() << endl;
	create_array_access_call[varRef] = array;
      }
    }
  }

  // ******************** DETECT functions in input program  *********************************************************************

}

