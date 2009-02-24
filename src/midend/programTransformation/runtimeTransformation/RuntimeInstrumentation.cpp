/****************************************
 * Runtime Instrumentation 
 * 12 May 2008 - tps
 ****************************************/
#include <rose.h>
#include "RuntimeInstrumentation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

/****************************************
 * This function returns true if an Operand
 * is on the right hand side of an Assignment
 * or AssignInitializer
 ****************************************/
bool RuntimeInstrumentation::isRightHandSide(SgNode* n) {
  bool isRightHand =false;
  SgNode* assignOp = n;
  SgNode* last = n;
  while (!isSgAssignOp(assignOp) && !isSgAssignInitializer(assignOp) && !isSgProject(assignOp)) {
    last=assignOp;
    ROSE_ASSERT(assignOp->get_parent());
    assignOp=assignOp->get_parent();
  }
  SgNode* rightHandSide = NULL;
  if (isSgAssignOp(assignOp)) {
    rightHandSide = isSgAssignOp(assignOp)->get_rhs_operand();
  }
  if (isSgAssignInitializer(assignOp)) {
    rightHandSide = isSgAssignInitializer(assignOp)->get_operand();
  }
  if (isSgAssignOp(assignOp) || isSgAssignInitializer(assignOp)) {
    if (rightHandSide==last) {
      // the varRef is on the right hand side of the assignment
      isRightHand=true;
    }
  }
  return isRightHand;
}

/****************************************
 * This function returns the statement that 
 * surrounds a given Node or Expression
 ****************************************/
SgStatement* RuntimeInstrumentation::getSurroundingStatement(SgNode* n) {
  SgNode* stat = n;
  while (!isSgStatement(stat) && !isSgProject(stat)) {
    ROSE_ASSERT(stat->get_parent());
    stat=stat->get_parent();
  }
  return isSgStatement(stat);
}

/****************************************
 * Iterates up the parents of an Expression
 * and returns the DotExp if that is found.
 * It also checks whether the iteration
 * occured from the left_hand_operand of
 * that DotExp, e.g object.val = 5
 * object is the left_hand_operand of the DotExp.
 ****************************************/
SgNode* RuntimeInstrumentation::isParentDotExp(SgExpression* n, bool& left ) {
  SgNode* stat = n;
  SgNode* last =n;
  left=false;
  while (!isSgDotExp(stat) && !isSgProject(stat)) {
    last=stat;
    ROSE_ASSERT(stat);
    ROSE_ASSERT(stat->get_parent());
    stat=stat->get_parent();
  }
  if (isSgDotExp(stat)) {
    SgNode* leftop = isSgDotExp(stat)->get_lhs_operand();
    if (leftop==last) left=true; 
  }
  return stat;
}

/****************************************
 * Determines if a node has a parent that is a 
 * PointerDerefExp
 ****************************************/
SgNode* RuntimeInstrumentation::isPointerDerefExp(SgVarRefExp* n) {
  SgNode* stat = n;
  while (!isSgPointerDerefExp(stat) && !isSgProject(stat)) {
    ROSE_ASSERT(stat->get_parent());
    stat=stat->get_parent();
  }
  return stat;
}

/****************************************
 * Iterates up the parents of an Expression
 * and returns the ArrowExp if that is found.
 * It also checks whether the iteration
 * occured from the left_hand_operand of
 * that ArrowExp, e.g object->val = 5
 * object is the left_hand_operand of the ArrowExp.
 ****************************************/
SgNode* RuntimeInstrumentation::isArrowExp(SgExpression* n, bool& left ) {
  SgNode* stat = n;
  SgNode* last =n;
  left=false;
  while (!isSgArrowExp(stat) && !isSgProject(stat)) {
    last=stat;
    ROSE_ASSERT(stat);
    ROSE_ASSERT(stat->get_parent());
    stat=stat->get_parent();
  }
  if (isSgArrowExp(stat)) {
    SgNode* leftop = isSgArrowExp(stat)->get_lhs_operand();
    if (leftop==last) left=true; 
  }
  return stat;
}


/****************************************
 * Transformation: inserts the function insertCheck before
 * a VarRefExp to check if it can be NULL
 * this function calls the previously defined function 
 * insertCheck (void*, string description);
 ****************************************/
void RuntimeInstrumentation::insertCheck(SgNode* n, std::string desc) {
  cerr <<  " Need to Assert that >" << n->class_name() << "<      >"<<
    desc << "< is not NULL before it is accessed. " << endl;
  ROSE_ASSERT(n);
  SgStatement* stmt = getSurroundingStatement(n);
  if (isSgStatement(stmt)) {
    SgName roseAssert("check_var");
    SgScopeStatement* scope = stmt->get_scope();
    //cerr << " stmt : " << stmt->class_name() << "  desc : " << desc << endl;
    ROSE_ASSERT(scope);
    if (isSgBasicBlock(scope)) {
      SgCastExp* cas=NULL;

      // if it is a pointerDeref, we need to find the VarReExp
      // by traversing the tree down to the leaf (get->operand)
      if (isSgPointerDerefExp(n)) {
	SgExpression* deref = isSgPointerDerefExp(n);
	SgExpression* deref_loop = deref;
	while (!isSgVarRefExp(deref_loop)) {
	  cerr << " found : " << deref_loop->class_name() << endl;
	  //	  ROSE_ASSERT(isSgPointerDerefExp(deref_loop)->get_operand());
	  if (!isSgUnaryOp(deref_loop)) {
	    // this could be because the current expression is of type ArrowExp or DotExp
	    // skip those for now.
	    cerr << "skipped this test... " << endl;
	    return;
	  }
	  deref_loop=isSgUnaryOp(deref_loop)->get_operand();
	}
	SgExpression* varRef =isSgVarRefExp(deref_loop);
	ROSE_ASSERT(varRef);
	SgExpression* arg = buildVarRefExp(isSgVarRefExp(varRef)->get_symbol()->get_name(),scope);

	// We have found the VarRefExp
	// The parent of the VarRefExp can be any of the following
	while (deref!=varRef) { //!isSgPointerDerefExp(varRef->get_parent())) {
	  // perfect
	  if (isSgPointerDerefExp(varRef->get_parent())) {
	    arg = buildPointerDerefExp(arg);
	    ROSE_ASSERT(isSgExpression(varRef->get_parent()));
	    varRef=isSgExpression(varRef->get_parent());
	  }
	  else if (isSgPlusPlusOp(varRef->get_parent())) {
	    arg = buildPlusPlusOp(arg);
	    ROSE_ASSERT(isSgExpression(varRef->get_parent()));
	    varRef=isSgExpression(varRef->get_parent());
	  } else if (isSgMinusMinusOp(varRef->get_parent())) {
	    arg = buildMinusMinusOp(arg,SgUnaryOp::prefix);
	    ROSE_ASSERT(isSgExpression(varRef->get_parent()));
	  varRef=isSgExpression(varRef->get_parent());
	  } else if (isSgAddressOfOp(varRef->get_parent())) {
	    arg = buildAddressOfOp(arg);
	    ROSE_ASSERT(isSgExpression(varRef->get_parent()));
	    varRef=isSgExpression(varRef->get_parent());
	  } else if (isSgCastExp(varRef->get_parent())) {
	    arg = buildCastExp(arg,isSgCastExp(varRef->get_parent())->get_type(),isSgCastExp(varRef->get_parent())->cast_type());
	    ROSE_ASSERT(isSgExpression(varRef->get_parent()));
	    varRef=isSgExpression(varRef->get_parent());
	  } else {
	    cerr <<" cant handle this case yet : " << varRef->get_parent()->class_name() << endl;
	    exit(0);
	  }
	}

	//is parent is pointer or not?
	bool onLeftSide = false;
	SgNode* parentIsDotExp = isParentDotExp(varRef,onLeftSide);
	if (isSgDotExp(parentIsDotExp))
	  cas =  buildCastExp(buildAddressOfOp(arg), buildPointerType(buildVoidType()));
	else
	  cas =  buildCastExp(arg, buildPointerType(buildVoidType()));
      } 
      else if (isSgVarRefExp(n)) {
	SgVarRefExp* varRef = isSgVarRefExp(n);
	SgVarRefExp* arg = buildVarRefExp(varRef->get_symbol()->get_name(),scope);
	//is parent is pointer or not?
	bool onLeftSide = false;
	SgNode* parentIsDotExp = isParentDotExp(varRef,onLeftSide);
	if (isSgDotExp(parentIsDotExp))
	  cas =  buildCastExp(buildAddressOfOp(arg), buildPointerType(buildVoidType()));
	else
	  cas =  buildCastExp(arg, buildPointerType(buildVoidType()));
      } else if (isSgPntrArrRefExp(n)) {
	SgPntrArrRefExp* varRef = isSgPntrArrRefExp(n);
	SgPntrArrRefExp* arg = buildPntrArrRefExp(varRef->get_lhs_operand(),varRef->get_rhs_operand());
	cas =  buildCastExp(arg, buildPointerType(buildVoidType()));
      } else {
	cerr <<" RuntimeInstruction: Unhandled case : " << n->class_name() << endl;
	exit(0);
      }
      ROSE_ASSERT(cas);
      SgStringVal* arg2 = buildStringVal(desc); //buildVarRefExp(desc,scope);
      ROSE_ASSERT(arg2);
      SgExprListExp* arg_list = buildExprListExp();
      appendExpression(arg_list,cas);
      appendExpression(arg_list,arg2);
      
      SgStatement* callStmt_1 = buildFunctionCallStmt(roseAssert,buildVoidType(),arg_list,scope);
      insertStatementBefore(isSgStatement(stmt),callStmt_1) ;
    }

  } else { 
    cerr << "RuntimeInstrumentation :: Surrounding Statement could not be found! " <<endl;
    exit(0);
  }
}

/****************************************
 * Entry point:
 * a) Insert insertCheck (void*, string description); function
 * b) Traverse AST and check VarRefExp's
 * c) insert insertCheck function calls 
 ****************************************/
void RuntimeInstrumentation::run(SgNode* project) {
  ROSE_ASSERT(isSgProject(project));
  globalScope = getFirstGlobalScope(isSgProject(project));


  pushScopeStack (isSgScopeStatement (globalScope));

  // insert rose header
  insertHeader("iostream",true,globalScope);

  // defining  void chec_var(SgNode*, string desc)
  // build parameter list first
  SgInitializedName* arg1 = buildInitializedName(SgName("n"),buildPointerType(buildVoidType()));
  SgInitializedName* arg2 = buildInitializedName(SgName("desc"),buildStringType());
  SgFunctionParameterList * paraList = buildFunctionParameterList();
  appendArg(paraList, arg1);
  appendArg(paraList, arg2);

  SgInitializedName* arg12 = buildInitializedName(SgName("n"),buildPointerType(buildVoidType()));
  SgInitializedName* arg22 = buildInitializedName(SgName("desc"),buildStringType());
  SgFunctionParameterList * paraList2 = buildFunctionParameterList();
  appendArg(paraList2, arg12);
  appendArg(paraList2, arg22);
    
  // build defining function declaration
  SgFunctionDeclaration * func_def = buildDefiningFunctionDeclaration	\
    (SgName("check_var"),SgTypeVoid::createType(),paraList);

  SgFunctionDeclaration * func_decl = buildNondefiningFunctionDeclaration \
    (SgName("check_var"),SgTypeVoid::createType(),paraList2);
    
  // build a statement inside the function body
  SgBasicBlock *func_body = func_def->get_definition ()->get_body ();
  ROSE_ASSERT (func_body);
  pushScopeStack (isSgScopeStatement (func_body));


  // create : if (n==NULL) { cerr << "Error at runtime at : " << desc << endl;
  SgName myprintf("printf");
  SgStringVal* pf_arg = buildStringVal("\\n\\nERROR detected: %s \\n");
  SgVarRefExp* pf_arg2 = buildVarRefExp("desc");
  ROSE_ASSERT(pf_arg2);
  SgExprListExp* pf_arg_list = buildExprListExp();
  appendExpression(pf_arg_list,pf_arg);
  appendExpression(pf_arg_list,pf_arg2);
  SgStatement* stmt_if_true = buildFunctionCallStmt(myprintf,buildVoidType(),pf_arg_list);

  SgStringVal* abort_arg = buildStringVal("Aborting this program. Goodbye. \\n");
  SgExprListExp* abort_arg_list = buildExprListExp();
  appendExpression(abort_arg_list,abort_arg);
  SgStatement* abort_stmt = buildFunctionCallStmt(myprintf,buildVoidType(),abort_arg_list);

  SgName myexit("exit");
  SgIntVal* exit_arg = buildIntVal(0);
  SgExprListExp* exit_arg_list = buildExprListExp();
  appendExpression(exit_arg_list,exit_arg);
  SgStatement* exit_stmt = buildFunctionCallStmt(myexit,buildVoidType(),exit_arg_list);

  SgBasicBlock* true_body = buildBasicBlock(stmt_if_true);
  appendStatement(abort_stmt,true_body);
  appendStatement(exit_stmt,true_body);

  //SgStatement* stmt_if_false = buildLabelStatement("b");
  SgBasicBlock* false_body = buildBasicBlock();
  SgVarRefExp* op1 = buildVarRefExp("n",isSgScopeStatement (func_body));
  SgExprStatement* conditional = buildExprStatement(buildEqualityOp(op1,buildIntVal(0)));

  SgIfStmt *ifstmt = buildIfStmt (conditional, true_body, false_body);
  // Insert the statement
  appendStatement (ifstmt);


  popScopeStack ();
  // insert the defining and declaring function
  appendStatement (func_def);
  SgStatement * oldFirstStmt = getFirstStatement(globalScope);
  prependStatement (func_decl);

  //    SgImport * iostr = buildImport (SgName("<iostream>"),SgTypeVoid::createType(),paraList2);
  //prependStatement (iostr);

  fixVariableReferences(func_decl);

  // mov up the preprocessor information
  if (oldFirstStmt)
    moveUpPreprocessingInfo(func_decl, oldFirstStmt);

  // pop the final scope after all AST insertion
  popScopeStack ();


  traverse(project, preorder);
  cerr << "\n Number of Elements in VarRefList  : " << varRefList.size() << endl;
  varRefList_Type::iterator it=varRefList.begin();
  for (;it!=varRefList.end();it++) {
    std::pair <SgNode*,std::string > p = *it;
    SgVarRefExp* var= isSgVarRefExp(p.first);
    SgPntrArrRefExp* var2= isSgPntrArrRefExp(p.first);
    SgPointerDerefExp* deref = isSgPointerDerefExp(p.first);

    string str = p.second;
    if (var) insertCheck(var,str);
    else if (var2) insertCheck(var2,str);
    else if (deref) insertCheck(deref,str);
    else {
      cerr << " UNKNOWN CHECK : " << p.first->class_name();
    }
  }
}


/****************************************************
 * visit each node and check for 
 * a) SgVarRef, including SgDotExp, SgArrowExp
 * b) SgPointerDerefExp
 * c) SgPntrArrRefExp
 * 
 * Only add SgVarRefExp if parent is SgDotExp and VarRef on left of SgDotExp 
 * Only add SgVarRefExp if parent is SgArrowExp and VarRef on left of SgArrowExp 
 * Otherwise add SgVarRefExp if it is on the right hand side of an expression.
 ****************************************************/
void RuntimeInstrumentation::visit(SgNode* n) {
  // check for different types of variable access
  // if access is found, then we assert the variable before the current statement

  if (isSgVarRefExp(n)) {
    SgVarRefExp* varRef = isSgVarRefExp(n);
    bool rightHandSide = isRightHandSide(n);
    string static_name = "NULL!!!";
    // check if it is NULL
    if ((varRef->get_parent()!=NULL && isSgPntrArrRefExp(varRef->get_parent()))) {
      // skip if this varRefExp is part of an array. This is handled further below
      return;
    }
    Sg_File_Info* file = varRef->get_file_info();
    int line = file->get_line();
    static_name = varRef->unparseToString();
    static_name.append("==NULL on line: "+to_string(line)+"\\\n  in File : "+file->get_filenameString());
    ROSE_ASSERT(varRef);
    
    bool isOnLeftSideDOT = false;
    bool isOnLeftSideARROW = false;
    SgNode* parentIsDotExp = isParentDotExp(varRef, isOnLeftSideDOT);
    SgNode* arrow = isArrowExp(varRef, isOnLeftSideARROW);

    // is parent SgDotExp
    if (isSgDotExp(parentIsDotExp)) {
      if (isOnLeftSideDOT) {
	static_name = parentIsDotExp->unparseToString();
	static_name.append("==NULL on line: "+to_string(line)+"\\\n  in File : "+file->get_filenameString());
	varRefList[varRef]=static_name;
	cerr << "DotExp:: Checking (left): " << parentIsDotExp->unparseToString() << endl;
      } else cerr << "DotExp::  NOT Checking (right): " << parentIsDotExp->unparseToString() << endl;
    }
    // is parent SgArrowExp
    else if (isSgArrowExp(arrow) ) {
      if (isOnLeftSideARROW) {
	static_name = arrow->unparseToString();
	static_name.append("==NULL on line: "+to_string(line)+"\\\n  in File : "+file->get_filenameString());
	varRefList[varRef]=static_name;
	cerr << "ArrowExp:: Checking (left): " << arrow->unparseToString() << endl;
      } else cerr << "ArrowExp:: NOT Checking (right): " << arrow->unparseToString() << endl;
    } else {
      // it is a simple varRefExp. Make sure it is on the right hand side of an assignment
      if (rightHandSide) {
	cerr << "VarRef:: Checking (right-assignment) " << static_name << endl;
	varRefList[n]=static_name;
      } 
    }
  }

  else if (isSgPointerDerefExp(n)) {
    SgPointerDerefExp* deref = isSgPointerDerefExp(n);
    Sg_File_Info* file = deref->get_file_info();
    int line = file->get_line();
    string static_name = deref->unparseToString();
    static_name.append("==NULL on line: "+to_string(line)+"\\\n  in File : "+file->get_filenameString());
    varRefList[deref]=static_name;
    cerr << "DerefExp:: Checking: " << deref->unparseToString() << endl;

  } else if (isSgPntrArrRefExp(n)) {
    // if array
    SgPntrArrRefExp* varRef = isSgPntrArrRefExp(n);
    bool rightHandSide = isRightHandSide(n);
    string static_name = "NULL!!!";
    // check if it is NULL
    if (varRef!=NULL) {
      Sg_File_Info* file = varRef->get_file_info();
      int line = file->get_line();
      static_name = varRef->unparseToString();
      static_name.append("==NULL on line: "+to_string(line)+"\\\n  in File : "+file->get_filenameString());
    }
    ROSE_ASSERT(varRef);
    cerr << varRef << " the pntrArrRefExp is " << static_name << endl;
    if (rightHandSide) 
      varRefList[n]=static_name;
  }
  

}
