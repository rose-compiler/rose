#include "RuntimeInstrumentation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

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

SgStatement* RuntimeInstrumentation::getSurroundingStatement(SgNode* n) {
    SgNode* stat = n;
    while (!isSgStatement(stat) && !isSgProject(stat)) {
      ROSE_ASSERT(stat->get_parent());
      stat=stat->get_parent();
    }
    return isSgStatement(stat);
}

void RuntimeInstrumentation::insertCheck(SgVarRefExp* n, std::string desc) {
  cerr <<  " Need to Assert that >" << n << "< >"<<desc << "< is not NULL before it is accessed. " << endl;
  ROSE_ASSERT(n);
  ROSE_ASSERT(isSgVarRefExp(n));
  SgStatement* stmt = getSurroundingStatement(n);
  if (isSgStatement(stmt)) {
    SgName roseAssert("check_var");
    SgScopeStatement* scope = stmt->get_scope();
    cerr << " stmt : " << stmt->class_name() << "  desc : " << desc << endl;
    ROSE_ASSERT(scope);
    if (isSgBasicBlock(scope)) {
      SgVarRefExp* arg = buildVarRefExp(n->get_symbol()->get_name(),scope);
      SgStringVal* arg2 = buildStringVal(desc); //buildVarRefExp(desc,scope);
      ROSE_ASSERT(arg2);
      SgExprListExp* arg_list = buildExprListExp();
      appendExpression(arg_list,arg);
      // currently not working
      appendExpression(arg_list,arg2);
      
      SgStatement* callStmt_1 = buildFunctionCallStmt(roseAssert,buildVoidType(),arg_list,scope);
      insertStatementBefore(isSgStatement(stmt),callStmt_1) ;
    }

  } else { 
    cerr << "RuntimeInstrumentation :: Surrounding Statement could not be found! " <<endl;
    exit(0);
  }
}

void RuntimeInstrumentation::run(SgNode* project) {
  ROSE_ASSERT(isSgProject(project));
  globalScope = getFirstGlobalScope(isSgProject(project));


    pushScopeStack (isSgScopeStatement (globalScope));

    // insert rose header
    //insertHeader("rose.h",true,globalScope);

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
    //SgStatement* stmt_if_true = buildLabelStatement("a");
    SgBasicBlock* true_body = buildBasicBlock();

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

  fixVariableReferences(func_decl);

    // mov up the preprocessor information
    if (oldFirstStmt)
      moveUpPreprocessingInfo(func_decl, oldFirstStmt);

    // pop the final scope after all AST insertion
    popScopeStack ();


  traverse(project, preorder);

  cerr << " Number of Elements in VarRefList  : " << varRefList.size() << endl;
  varRefList_Type::iterator it=varRefList.begin();
  for (;it!=varRefList.end();it++) {
    std::pair <SgNode*,std::string > p = *it;
    SgVarRefExp* var= isSgVarRefExp(p.first);
    string str = p.second;
    insertCheck(var,str);
  }



}

/****************************************************
 * visit each node
 ****************************************************/
void RuntimeInstrumentation::visit(SgNode* n) {
  // check for different types of variable access
  // if access is found, then we assert the variable before the current statement

  if (isSgVarRefExp(n)) {
    SgVarRefExp* varRef = isSgVarRefExp(n);
    bool rightHandSide = isRightHandSide(n);
    string static_name = "varRef is NULL!";
    // check if it is NULL
    if (varRef!=NULL) {
      Sg_File_Info* file = varRef->get_file_info();
      int line = file->get_line();
      static_name = varRef->get_symbol()->get_name().str();
      static_name.append(" on line: ");
      static_name.append(to_string(line));
      static_name.append("  in File : ");
      static_name.append(file->get_filenameString()); 
    }
    ROSE_ASSERT(varRef);
    cerr << varRef << " the varRef is " << static_name << endl;
    if (rightHandSide) 
            varRefList[n]=static_name;

  }


}
