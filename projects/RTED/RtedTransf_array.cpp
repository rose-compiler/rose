#include <rose.h>
#include <string>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;


// TODO 2: remove or set to 0 when rts has user type information
#define ENABLE_STRUCT_DECOMPOSITION 1


/* -----------------------------------------------------------
 * Is the Initialized Name already known as an array element ?
 * -----------------------------------------------------------*/
bool
RtedTransformation::isVarRefInCreateArray(SgInitializedName* search) {
  bool found=false;
  std::map<SgVarRefExp*, RTedArray*>::const_iterator it= 
    create_array_define_varRef_multiArray.begin();
  for (;it!=create_array_define_varRef_multiArray.end();++it) {
    //SgVarRefExp* varRef = it->first;
    RTedArray* array = it->second;
    SgInitializedName* initName = array->initName;
    if (initName==search) {
      found=true;
    }
  }
  std::map<SgInitializedName*, RTedArray*>::const_iterator it2 =  
    create_array_define_varRef_multiArray_stack.begin();
  for (;it2!=create_array_define_varRef_multiArray_stack.end();++it2) {
    SgInitializedName* initName = it2->first;
    if (initName==search) {
      found=true;
    }
  }
  return found;
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


void RtedTransformation::insertArrayCreateCall(SgStatement* stmt,
					       SgInitializedName* initName, 
					       RTedArray* array) {
  // make sure there is no extern in front of stmt
#if 1
  SgDeclarationStatement* declstmt = isSgDeclarationStatement(stmt);
  SgFunctionParameterList* funcparam = isSgFunctionParameterList(stmt);
  bool externQual =false;
  if (funcparam) {
    SgFunctionDeclaration* funcdeclstmt = isSgFunctionDeclaration(funcparam->get_parent());
    ROSE_ASSERT(funcdeclstmt);
    externQual = funcdeclstmt->get_declarationModifier().get_storageModifier().isExtern();
    cerr << ">>>>>>>>>>>>>>>> stmt-param : " << funcdeclstmt->unparseToString() << "  " << funcdeclstmt->class_name() << 
      "  " << externQual << endl;
  } else if (declstmt) {
    externQual = declstmt->get_declarationModifier().get_storageModifier().isExtern();
  }  
  cerr << ">>>>>>>>>>>>>>>> stmt : " << stmt->unparseToString() << "  " << stmt->class_name() <<
	  " parent : " << stmt->get_parent()->class_name() << " " << endl;
  if (externQual) {
    cerr << "Skipping this insertArrayCreate because it probably occurs multiple times (with and without extern)." << endl;
    return;
  }
#endif


  std::vector<SgExpression*> value;
  array->getIndices(value);
  int dimension = array->dimension;
  //bool stack = array->stack;
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
      SgExpression* plainname = buildString(initName->get_name());
      SgExpression* callNameExp = buildString(name);
      SgIntVal* dimExpr = buildIntVal(dimension);
      //SgBoolValExp* stackExpr = buildBoolValExp(stack);

      SgExprListExp* arg_list = buildExprListExp();
      appendExpression(arg_list, plainname);
      appendExpression(arg_list, callNameExp);
      appendExpression(arg_list, dimExpr);
    SgVarRefExp* var_ref = buildVarRefExp( initName, scope);

    appendAddressAndSize(initName, var_ref, stmt, arg_list,1); 


      std::vector<SgExpression*>::const_iterator it = value.begin();
      for (; it != value.end(); ++it) {
	SgExpression* expr = isSgExpression(*it);
	if (expr == NULL)
	  expr = buildIntVal(-1);
	ROSE_ASSERT(expr);
	appendExpression(arg_list, expr);
      }
      SgIntVal* ismalloc = buildIntVal(0);
      if (array->ismalloc)
	ismalloc = buildIntVal(1);
      appendExpression(arg_list, ismalloc);

      SgExpression* filename = buildString(stmt->get_file_info()->get_filename());
      SgExpression* linenr = buildString(RoseBin_support::ToString(stmt->get_file_info()->get_line()));
      appendExpression(arg_list, filename);
      appendExpression(arg_list, linenr);

      SgExpression* linenrTransformed = buildString("x%%x");
      appendExpression(arg_list, linenrTransformed);

      //      appendExpression(arg_list, buildString(stmt->unparseToString()));
      ROSE_ASSERT(roseCreateArray);
      string symbolName2 = roseCreateArray->get_name().str();
      //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
      SgFunctionRefExp* memRef_r = buildFunctionRefExp(  roseCreateArray);
      //SgArrowExp* sgArrowExp = buildArrowExp(varRef_l, memRef_r);

      SgFunctionCallExp* funcCallExp = buildFunctionCallExp(memRef_r,
							    arg_list);
      SgExprStatement* exprStmt = buildExprStatement(funcCallExp);

#if 1
      cerr << "++++++++++++ stmt :"<<stmt << " mainFirst:"<<mainFirst<<
      "   initName->get_scope():"<<initName->get_scope() <<
      "   mainFirst->get_scope():"<<mainFirst->get_scope()<<endl;
      if( stmt == mainFirst && initName->get_scope()!=mainFirst->get_scope()) {
        insertStatementBefore(isSgStatement(stmt), exprStmt);
        cerr << "+++++++ insert Before... "<<endl;
      } else {
        // insert new stmt (exprStmt) after (old) stmt
        insertStatementAfter(isSgStatement(stmt), exprStmt);
        cerr << "+++++++ insert After... "<<endl;
      }
#endif
      string empty_comment = "";
      attachComment(exprStmt,empty_comment,PreprocessingInfo::before);
      string comment = "RS : Create Array Variable, paramaters : (name, manglname, dimension, address, sizeof(type), size dim 1, size dim 2, ismalloc, filename, linenr, linenrTransformed)";
      attachComment(exprStmt,comment,PreprocessingInfo::before);
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
      << stmt->class_name() << endl;
    ROSE_ASSERT(false);
  }

  bool ismalloc = array->ismalloc;
  // unfortunately the arrays are filled with '\0' which is a problem
  // for detecting other bugs such as not null terminated strings
  // therefore we call a function that appends code to the 
  // original program to add padding different from '\0'
  if (ismalloc)
    addPaddingToAllocatedMemory(stmt, array);

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
    // if the initName is part of the function parameter
    // then we use the short name instead of mangled name
    // this mechanism is used in order to determine the right
    // variable from the function that is being called
    SgNode* parent = initName->get_parent();
    ROSE_ASSERT(parent);
    string name = initName->get_mangled_name().str();
    if (isSgFunctionParameterList(parent))
      name = initName->get_name();
    SgExpression* callNameExp = buildString(name);

    SgExprListExp* arg_list = buildExprListExp();
    appendExpression(arg_list, callNameExp);
    appendExpression(arg_list, array->indx1);
    SgExpression* expr = NULL;

    SgVarRefExp* var_ref = buildVarRefExp( initName, scope);    
    if (array->indx2)
      appendExpression(arg_list, array->indx2);
    else {
      if (array->dimension==1)
	expr = buildIntVal(-1); // call [i,-1]
      else
	expr = buildIntVal(0); // call [i,0]
      appendExpression(arg_list, expr);
    }
    SgExpression* filename = buildString(stmt->get_file_info()->get_filename());
    SgExpression* linenr = buildString(RoseBin_support::ToString(stmt->get_file_info()->get_line()));
    appendExpression(arg_list, filename);
    appendAddressAndSize(initName, var_ref, stmt, arg_list,0); 


    appendExpression(arg_list, linenr);

    SgExpression* linenrTransformed = buildString("x%%x");
    appendExpression(arg_list, linenrTransformed);

    appendExpression(arg_list, buildString(removeSpecialChar(stmt->unparseToString())));

    ROSE_ASSERT(roseArrayAccess);
    string symbolName2 = roseArrayAccess->get_name().str();
    //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
    SgFunctionRefExp* memRef_r = buildFunctionRefExp(
						     roseArrayAccess);
    //SgArrowExp* sgArrowExp = buildArrowExp(varRef_l, memRef_r);

    SgFunctionCallExp* funcCallExp = buildFunctionCallExp(memRef_r,
							  arg_list);
    SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
    insertStatementBefore(isSgStatement(stmt), exprStmt);
    string empty_comment = "";
    attachComment(exprStmt,empty_comment,PreprocessingInfo::before);
    string comment = "RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, filename, linenr, linenrTransformed, part of error message)";
    attachComment(exprStmt,comment,PreprocessingInfo::before);

    //    }

  } else {
    cerr
      << "RuntimeInstrumentation :: Surrounding Statement could not be found! "
      << endl;
    exit(0);
  }
}


void RtedTransformation::visit_isArraySgInitializedName(SgNode* n) {
  SgInitializedName* initName = isSgInitializedName(n);
  ROSE_ASSERT(initName);
  int dimension = 0;
  dimension = getDimension(initName);
  // STACK ARRAY : lets see if we assign an array here
  SgType* type = initName->get_typeptr();
  ROSE_ASSERT(type);
  SgArrayType* array = isSgArrayType(type);
  if (array) {
    SgExpression * expr = array->get_index();
    SgExpression * expr2 = NULL;
    SgType* basetype = array->get_base_type();
    SgArrayType* array2 = isSgArrayType(basetype);
    cerr << " unparse : " << array->unparseToString() << "   " << n->unparseToString() << endl;
    if (array2) {
      dimension++;
      expr2=array2->get_index();
      cerr << " --------------------- Two dimensional Array array2 : "<<array2->class_name()<<" expr2 : " << expr2->unparseToString() << endl;
      ROSE_ASSERT(expr2);
    } else {
      cerr << " file : " << initName->get_file_info()->get_filename() << "  line  : " << initName->get_file_info()->get_line()<<endl;
      cerr << " --------------------- One Dimensional Array  Type : " << type->class_name() <<endl;
    }

    if (expr) {
      cerr << " >>> Found expression -- expr : " << expr->class_name() << 
	"   initName : " << initName << "  array : " << array << endl;
    } else {
      cerr << " >>> Found expression -- expr : NULL "  << 
	"   initName : " << initName << "  array : " << array << endl;
    }
    if (expr != NULL) {
      cerr << "... Found stack array: " << initName->unparseToString()
	   << " " << type->class_name() << " array expr: "
	   << expr->unparseToString() << "  dim: " << dimension
	   << endl;
      if (expr2)
	cerr << "   Expr2 : " << expr2->unparseToString() << endl;
      ROSE_ASSERT(dimension>0);
      RTedArray* arrayRted = new RTedArray(true, dimension, initName,
					   expr, expr2,false);
      cerr << " The problem: " << n->get_parent()->class_name() << " :: " << n->get_parent()->unparseToString() << endl;
      if (!isSgFunctionParameterList(n->get_parent()))
    	  create_array_define_varRef_multiArray_stack[initName] = arrayRted;
    } else if (isSgAssignInitializer(initName->get_initptr())) {
      cerr << "... Found Array AssignInitializer ... " << endl;
      SgExpression* operand = isSgAssignInitializer(initName->get_initptr())->get_operand();
      ROSE_ASSERT(operand);
      cerr << "... Found operand : " << operand->class_name() << endl;
      string arrayValue ="";
      int length=0;
      if (isSgStringVal(operand)) {
	arrayValue=isSgStringVal(operand)->get_value();
	length=arrayValue.size()+1;
      } else {
	cerr << "...Unknown type of operand :   arr[] = ...unknown... " << endl;
	ROSE_ASSERT(false);
      }
      // now that we know what the array is initialized with and the length,
      // create the array variable
      ROSE_ASSERT(dimension>0);
      SgIntVal* sizeExp = buildIntVal(length);
      RTedArray* arrayRted = new RTedArray(true, dimension, initName,
					   sizeExp, expr2,false);
      create_array_define_varRef_multiArray_stack[initName] = arrayRted;

      array->set_index(buildIntVal(length));

    } else {
      cerr << "... There is a problem detecting this array ... " << initName->get_initptr()->class_name() << endl;
      ROSE_ASSERT(false);
    }
  }

}


void RtedTransformation::visit_isSgVarRefExp(SgVarRefExp* n) {
  // make sure that this variable is not on 
  // the left hand side of an assignment.
  // And it should not be an argument to a function
  // if true, we add it to the list of variables being accessed

#if 0
	   SgNode* parent = isSgVarRefExp(n)->get_parent();
	   SgNode* last = parent;
	  while (!isSgAssignOp(parent) &&
	        !isSgAssignInitializer(parent) &&
	        !isSgProject(parent) &&
	        !isSgFunctionCallExp(parent) &&
	   !isSgDotExp(parent)) {
	     last=parent;
	     parent=parent->get_parent();
	  }
	   if( isSgProject(parent) ||
	       isSgFunctionCallExp(parent) ||
	       isSgDotExp(parent))
	     { // do nothing
	     }
	   else if (isSgAssignOp(parent)) {
	     // make sure that we came from the right hand side of the assignment
	     SgExpression* right = isSgAssignOp(parent)->get_rhs_operand();
	     if (right==last)
	      variable_access_varref.push_back(n);
	   }
	   else if (isSgAssignInitializer(parent)) {
	     // make sure that we came from the right hand side of the assignment
	     SgExpression* right = isSgAssignInitializer(parent)->get_operand();
	     if (right==last)
	       variable_access_varref.push_back(n);
	   }
	  else {
	     // its a plain variable access
		  variable_access_varref.push_back(n);
	   }


#else
  SgNode* parent = isSgVarRefExp(n)->get_parent();
  SgNode* last = parent;
  bool hitRoof=false;
  while (!isSgProject(parent)) {
    last=parent;
    parent=parent->get_parent();

  if( isSgProject(parent) ||
      isSgFunctionCallExp(parent) ||
      isSgDotExp(parent))
    { // do nothing 
      hitRoof=true;
	  break;
    }
  else if (isSgAssignOp(parent)) {
    // make sure that we came from the right hand side of the assignment
    SgExpression* right = isSgAssignOp(parent)->get_rhs_operand();
    if (right==last)
      variable_access_varref.push_back(n);
    hitRoof=true;
    break;
  }
  else if (isSgAssignInitializer(parent)) {
    // make sure that we came from the right hand side of the assignment
    SgExpression* right = isSgAssignInitializer(parent)->get_operand();
    if (right==last)
    	variable_access_varref.push_back(n);
    hitRoof=true;
    break;
  } else if (isSgPointerDerefExp(parent)) {
	  cerr << "------------ Found Pointer deref : " << parent->unparseToString() << endl;
	  variable_access_pointerderef[isSgPointerDerefExp(parent)]=n;
  }
  } //while

  if (!hitRoof) {
    // its a plain variable access
	  variable_access_varref.push_back(n);
  }
#endif
}



void RtedTransformation::visit_isArraySgAssignOp(SgNode* n) {
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

  cerr <<"   ::: Checking assignment : " << n->unparseToString()<<endl;

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
	std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfDot(isSgDotExp(expr_lll),
									  "Left of pntrArr2 - Right of Dot  - line: "
									  + expr_lll->unparseToString() + " ", varRef);
	initName = mypair.first;
	varRef = mypair.second;
	if (initName)
	  ROSE_ASSERT(varRef);
      } else if (isSgArrowExp(expr_lll)) {
	std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfArrow(isSgArrowExp(expr_lll),
									    "Left of pntrArr2 - Right of Arrow  - line: "
									    + expr_lll->unparseToString() + " ", varRef);
	initName = mypair.first;
	varRef = mypair.second;
	if (initName)
	  ROSE_ASSERT(varRef);
      } else {
	cerr
	  << "RtedTransformation : Left of pntrArr2 - Unknown : "
	  << expr_lll->class_name() << endl;
	ROSE_ASSERT(false);
      }
    } else if (isSgDotExp(expr_ll)) {
      cerr << "RtedTransformation : isSgDotExp : " << endl;
      
      std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfDot(isSgDotExp(expr_ll),
									"Left of pntrArr - Right of Dot  - line: "
									+ expr_ll->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      if (initName)
	ROSE_ASSERT(varRef);
    } else if (isSgPointerDerefExp(expr_ll)) {
      cerr << "RtedTransformation : isSgPointerDerefExp : " << endl;
      
      std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfPointerDeref(isSgPointerDerefExp(expr_ll),
										 "Left of pntrArr - Right of PointerDeref  - line: "
										 + expr_ll->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      if (initName)
	ROSE_ASSERT(varRef);
    }
    else if (isSgArrowExp(expr_ll)) {
      cerr << "RtedTransformation : isSgArrowExp : " << endl;
      std::pair<SgInitializedName*,SgVarRefExp*> mypair  = getRightOfArrow(isSgArrowExp(expr_ll),
									   "Left of pntrArr - Right of Arrow  - line: "
									   + expr_ll->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      if (initName)
	ROSE_ASSERT(varRef);
    } else {
      cerr << "RtedTransformation : Left of pntrArr - Unknown : "
	   << expr_ll->class_name() << "  line:"
	   << expr_ll->unparseToString() << endl;
      ROSE_ASSERT(false);
    }
  } // ------------------------------------------------------------
  else if (isSgDotExp(expr_l)) {
#if ENABLE_STRUCT_DECOMPOSITION
    std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfDot(isSgDotExp(expr_l),
								      "Right of Dot  - line: " + expr_l->unparseToString() + " ", varRef);
    initName = mypair.first;
    varRef = mypair.second;
    if (initName)
      ROSE_ASSERT(varRef);
#else
    // do nothing -- do not decompose user types
    //  e.g.
    //
    //    struct typ { int a; int b; } v;
    //    v.a = 2;
    //
    //  do not track a as initialized -- we just say all of v is initialized
    return;
#endif
  }// ------------------------------------------------------------
  else if (isSgArrowExp(expr_l)) {
    std::pair<SgInitializedName*,SgVarRefExp*> mypair  = getRightOfArrow(isSgArrowExp(expr_l),
									 "Right of Arrow  - line: " + expr_l->unparseToString()
									 + " ", varRef);
    initName = mypair.first;
    varRef = mypair.second;
    if (initName)
      ROSE_ASSERT(varRef);
  } // ------------------------------------------------------------
  else if (pointerDeref) {
    SgExpression* exp = pointerDeref->get_operand();
    ROSE_ASSERT(exp);
    if (isSgPlusPlusOp(exp)) {
      std::pair<SgInitializedName*,SgVarRefExp*> mypair  = getPlusPlusOp(isSgPlusPlusOp(exp),
									 "PointerDerefExp PlusPlus  - line: "
									 + pointerDeref->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      ROSE_ASSERT(varRef);
    } else if (isSgMinusMinusOp(exp)) {
      std::pair<SgInitializedName*,SgVarRefExp*> mypair  = getMinusMinusOp(isSgMinusMinusOp(exp),
									   "PointerDerefExp MinusMinus  - line: "
									   + pointerDeref->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      ROSE_ASSERT(varRef);
    } 
    else if (isSgVarRefExp(exp)) {
      initName = isSgVarRefExp(exp)->get_symbol()->get_declaration();
      varRef = isSgVarRefExp(exp);
      ROSE_ASSERT(varRef);
    }
    else if (isSgDotExp(exp)) {
      std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfDot(isSgDotExp(exp),
									"Right of Dot  - line: " + exp->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      if (initName)
	ROSE_ASSERT(varRef);
    }// ------------------------------------------------------------
    else if (isSgPointerDerefExp(exp)) {
      std::pair<SgInitializedName*,SgVarRefExp*> mypair = getRightOfPointerDeref(isSgPointerDerefExp(exp),
										 "Right of PointerDeref  - line: " + exp->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      if (initName)
	ROSE_ASSERT(varRef);
    }// ------------------------------------------------------------
    else {
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


  // varRef contains variable
  int derefCounter = 0;
  // derefCounter counts the number of dereferences within the expression
  // based on that we decide if it is a one or two dim array
  // if derefCounter == 1 assume 1dim array otherwise 2dim
  getExprBelowAssignment(varRef, derefCounter);


  // handle MALLOC
  bool ismalloc=false;
  vector<SgNode*> calls = NodeQuery::querySubTree(expr_r,
						  V_SgFunctionCallExp);
  vector<SgNode*>::const_iterator it = calls.begin();
  for (; it != calls.end(); ++it) {
    SgFunctionCallExp* funcc = isSgFunctionCallExp(*it);
    if (funcc) {
      // MALLOC : function call
      SgTreeCopy treeCopy;
      SgExprListExp* size = isSgExprListExp(funcc->get_args()->copy(treeCopy));
      ROSE_ASSERT(size);
      // find if sizeof present in size operator
      vector<SgNode*> results = NodeQuery::querySubTree(size,V_SgSizeOfOp);
      SgSizeOfOp* sizeofOp = NULL;
      if (results.size()==1) {
	sizeofOp = isSgSizeOfOp(*(results.begin()));
	ROSE_ASSERT(sizeofOp);
      } else if (results.size()>1) {
	cerr << "More than 1 sizeof operand. Abort." << endl;
	exit(1);
      }
      SgExpression* func = funcc->get_function();
      if (func && size) {
	ismalloc = false;
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
	  ROSE_ASSERT(varRef);
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
	  if (dimension!=derefCounter && derefCounter>0) {
	    cout << ">>>>>>>>> WARNING Dimension changed : " << dimension << " to " << derefCounter << endl;
	    dimension=derefCounter;
	  }
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
	       << "  indx2 : " << idx2_s 
	       << "  dimension based on derefCounter: " << derefCounter << endl;
	  ROSE_ASSERT(dimension>0);
	  // remove the sizeof allocation
	  SgNode* parentOfSizeof = sizeofOp->get_parent();
	  ROSE_ASSERT(parentOfSizeof);
	  // the parent of sizeof is usally A Value, and thereafter Mul or Div
	  parentOfSizeof = parentOfSizeof->get_parent();
	  ROSE_ASSERT(parentOfSizeof);
	  if (isSgBinaryOp(parentOfSizeof)) {
	    SgBinaryOp* bin = isSgBinaryOp(parentOfSizeof);
	    if (!(isSgDivideOp(bin) || isSgMultiplyOp(bin))) {
	      cerr << "The operand above sizeof is not of type multiply or divide. Not handled" << endl;
	      exit (1);
	    }
	    SgExpression* val = buildIntVal(1);
	    if (bin->get_rhs_operand()==sizeofOp->get_parent()) {
	      bin->set_rhs_operand(val);
	    } else {
	      bin->set_lhs_operand(val);
	    }
	    delete sizeofOp->get_parent();
	  } 
#if 0
	  else {
	    cerr << "Unhandled Operand for sizeof parent. " << parentOfSizeof->class_name() << endl;
	    exit(1);
	  }
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

	  // we are creating a new array variable based on the malloc call
	  RTedArray* array = new RTedArray(false, dimension,
					   initName, indx1, indx2,
					   ismalloc);
	  // varRef can not be a array access, its only an array Create
	  variablesUsedForArray.push_back(varRef);
	  create_array_define_varRef_multiArray[varRef] = array;
	}
      }
    }
  }

  // ---------------------------------------------
  // handle variables ..............................
  // here we should know the initName of the variable on the left hand side
  ROSE_ASSERT(initName);
  ROSE_ASSERT(varRef);
  if (initName && varRef) {
    // we now know that this variable must be initialized
    // if we have not set this variable to be initialized yet,
    // we do so
    cerr  << ">> Setting this var to be initialized : " << initName->unparseToString() << endl;
    variableIsInitialized[varRef]=std::pair<SgInitializedName*,bool>(initName,ismalloc);
  }
  // ---------------------------------------------


}



void RtedTransformation::addPaddingToAllocatedMemory(SgStatement* stmt,  RTedArray* array) {
  printf(">>> Padding allocated memory with blank space\n");
  //SgStatement* stmt = getSurroundingStatement(varRef);
  ROSE_ASSERT(stmt);
  // if you find this:
  //   str1 = ((char *)(malloc(((((4 * n)) * (sizeof(char )))))));
  // add the following lines:
  //   int i;
  //   for (i = 0; (i) < malloc(((((4 * n)) * (sizeof(char )); i++)
  //     str1[i] = ' ';

  // we do this only for char*
  bool cont=false;
  SgInitializedName* initName = array->initName;
  SgType* type = initName->get_type();
  ROSE_ASSERT(type);
  cerr << " Padding type : " << type->class_name() << endl;
  if (isSgPointerType(type)) {
    SgType* basetype = isSgPointerType(type)->get_base_type();
    cerr << " Base type : " << basetype->class_name() << endl;
    if (basetype && isSgTypeChar(basetype))
      cont=true;
  }
  
  // since this is mainly to handle char* correctly, we only deal with one dim array for now
  if (cont && array->dimension==1) {
    // allocated size
    SgScopeStatement* scope = stmt->get_scope();
    SgExpression* size = array->indx1;
    pushScopeStack (scope);
    // int i;
    SgVariableDeclaration* stmt1 = buildVariableDeclaration("i",buildIntType(),NULL); 
    //for(i=0;..)
    SgStatement* init_stmt= buildAssignStatement(buildVarRefExp("i"),buildIntVal(0));

    // for(..,i<size,...) It is an expression, not a statement!
    SgExprStatement* cond_stmt=NULL;
    cond_stmt= buildExprStatement(buildLessThanOp(buildVarRefExp("i"),size)); 
 
    // for (..,;...;i++); not ++i;
    SgExpression* incr_exp = NULL;
    incr_exp=buildPlusPlusOp(buildVarRefExp("i"),SgUnaryOp::postfix);
    // loop body statement
    SgStatement* loop_body= NULL; 
    SgExpression* lhs = buildPntrArrRefExp(buildVarRefExp(array->initName->get_name()),buildVarRefExp("i"));
    SgExpression* rhs = buildCharVal(' ');
    loop_body = buildAssignStatement(lhs,rhs);
    //loop_body = buildExprStatement(stmt2); 


    SgForStatement* forloop = buildForStatement (init_stmt,cond_stmt,incr_exp,loop_body);

    SgBasicBlock* bb = buildBasicBlock(stmt1,
				       forloop);
    insertStatementAfter(isSgStatement(stmt), bb);
    string comment = "RS: Padding this newly generated array with empty space.";
    attachComment(bb,comment,PreprocessingInfo::before);
    popScopeStack();
  } 
}


void RtedTransformation::visit_isArrayPntrArrRefExp(SgNode* n) {
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
      SgDotExp* dot = isSgDotExp(left);
      SgPointerDerefExp* pointerDeref = isSgPointerDerefExp(left);
      SgPntrArrRefExp* arrRefExp2 = isSgPntrArrRefExp(left);
      if (arrow) {
	varRef = isSgVarRefExp(arrow->get_rhs_operand());
	ROSE_ASSERT(varRef);
      } else if (dot) {

#if ENABLE_STRUCT_DECOMPOSITION
varRef = isSgVarRefExp(dot->get_rhs_operand());
ROSE_ASSERT(varRef);
#else
        // do nothing -- do not decompose user types
        //  e.g.
        //
        //    struct typ { int a; int b; } v;
        //    v.a = 2;
        //
        //  do not track a as initialized -- we just say all of v is initialized
        return;
#endif
      } else if (pointerDeref) {
	varRef = isSgVarRefExp(pointerDeref->get_operand());
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
    vector<SgVarRefExp*>::const_iterator cv = variablesUsedForArray.begin();
    for (;cv!=variablesUsedForArray.end();++cv) {
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
			      NULL, false);
      } else {
	array = new RTedArray(false, dimension, initName, right2,
			      right1, false);
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


void RtedTransformation::visit_isArrayExprListExp(SgNode* n) {
  // there could be a cast between SgExprListExp and SgVarRefExp
  SgExprListExp* exprlist = isSgExprListExp(isSgVarRefExp(n)->get_parent());
#if 1
  cerr << " >> checking node : " << n->class_name() << endl;
  if (isSgVarRefExp(n)) {
    cerr << ">>>>>>>>>>> checkign func : " << isSgVarRefExp(n)->unparseToString() <<
      "    parent : " <<  isSgVarRefExp(n)->get_parent()->class_name() <<
      "    parent : " <<  isSgVarRefExp(n)->get_parent()->get_parent()->class_name() << endl;
  }
#endif
  SgNode* parent = isSgVarRefExp(n)->get_parent();
  while (!isSgExprListExp(parent) && !isSgProject(parent)) {
    parent=parent->get_parent();
  }
  if (isSgExprListExp(parent))
    exprlist = isSgExprListExp(parent);
  //  if (isSgCastExp(isSgVarRefExp(n)->get_parent()))
  //  exprlist = isSgExprListExp(isSgVarRefExp(n)->get_parent()->get_parent());
  // check if this is a function call with array as parameter
  if (exprlist) {
    SgFunctionCallExp* fcexp = isSgFunctionCallExp(exprlist->get_parent());
    if (fcexp) {
      cerr <<"      ... Found a function call with varRef as parameter: " << fcexp->unparseToString() <<endl;
      // check if parameter is array - then check function name
      // call func(array_name) to runtime system for runtime inspection 
      SgInitializedName* initName =
	isSgVarRefExp(n)->get_symbol()->get_declaration();
      if (isVarRefInCreateArray(initName) ||
	  isVarInCreatedVariables(initName)) {
	// create this function call only, if it is not one of the 
	// interesting function calls, such as strcpy, strcmp ...
	// because for those we do not need the parameters and do not 
	// need to put them on the stack
	SgFunctionRefExp* refExp = isSgFunctionRefExp(fcexp->get_function());
	ROSE_ASSERT(refExp);
	SgFunctionDeclaration* decl = isSgFunctionDeclaration(refExp->getAssociatedFunctionDeclaration ());
	ROSE_ASSERT(decl);
	string name = decl->get_name();
	string mangled_name = decl->get_mangled_name().str();
	cerr <<">>Found a function call " << name << endl;
	if (isStringModifyingFunctionCall(name)==false &&
	    isFileIOFunctionCall(name)==false ) {
	  vector<SgExpression*> args;
	  SgExpression* varOnLeft = buildString("NoAssignmentVar2");
	  SgStatement* stmt = getSurroundingStatement(isSgVarRefExp(n));
	  ROSE_ASSERT(stmt);
	  RtedArguments* funcCall = new RtedArguments(name, // function name
						      mangled_name,
						      // we need this for the function as well
						      initName->get_name(), // variable
						      initName->get_mangled_name().str(),
						      isSgVarRefExp(n),
						      stmt,
						      args,
						      varOnLeft,
						      varOnLeft
						      );
	  ROSE_ASSERT(funcCall);	
	  cerr << " !!!!!!!!!! Adding function call." << name << endl;
	  function_call.push_back(funcCall);
	}
      } else {
	cerr << " This is a function call but its not passing an array element " << endl;
      }
    }
  }
}







