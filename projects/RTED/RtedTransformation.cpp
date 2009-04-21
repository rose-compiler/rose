#include <rose.h>
#include <string>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;


/* -----------------------------------------------------------
 * Run frontend and return project
 * -----------------------------------------------------------*/
SgProject*
RtedTransformation::parse(int argc, char** argv) {
  SgProject* project = frontend(argc, argv);
  ROSE_ASSERT(project);
  return project;
}


void 
RtedTransformation::insertFuncCall(RtedArguments* args  ) {
  insertFuncCall(args,true);
  insertFuncCall(args,false);
}


void 
RtedTransformation::insertFuncCall(RtedArguments* args, bool before  ) {
  SgStatement* stmt = getSurroundingStatement(args->varRefExp);
  ROSE_ASSERT(stmt);
  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    ROSE_ASSERT(scope);
    int size = 4+args->arguments.size();
    SgIntVal* sizeExp = buildIntVal(size);
    SgStringVal* callNameExp = buildStringVal(args->name);
    SgStringVal* callNameExp2 = buildStringVal(args->mangled_name);
    SgStringVal* callNameExp3 = buildStringVal(RoseBin_support::ToString(scope));
    SgStringVal* boolVal = buildStringVal("true");
    if (before==false) {
      delete boolVal;
      boolVal = buildStringVal("false");
    }

    SgVarRefExp* varRef_l = buildVarRefExp("runtimeSystem", globalScope);
    string symbolName = varRef_l->get_symbol()->get_name().str();
    //cerr << " >>>>>>>> Symbol VarRef: " << symbolName << endl;
    ROSE_ASSERT(roseFunctionCall);

    SgExprListExp* arg_list = buildExprListExp();
    appendExpression(arg_list, sizeExp);
    appendExpression(arg_list, callNameExp);
    appendExpression(arg_list, callNameExp2);
    appendExpression(arg_list, callNameExp3);
    appendExpression(arg_list, boolVal);

    std::vector<SgExpression*>::const_iterator it = args->arguments.begin();
    for (;it!=args->arguments.end();++it) {
      SgExpression* exp = *it;
      if (isSgUnaryOp(exp))
	exp = isSgUnaryOp(exp)->get_operand();
      //cerr << " exp = " << exp->class_name() << endl;
      if (isSgVarRefExp(exp)) {
	SgVarRefExp* var = isSgVarRefExp(exp);
	SgType* type = var->get_type();
	SgType* base_type = NULL;
	cerr << " type : " << type->class_name() << endl;
	if (isSgArrayType(type) )
	  base_type= isSgArrayType(type)->get_base_type();
	if ( isSgPointerType(type))
	  base_type= isSgPointerType(type)->get_base_type();
	if (isSgTypeChar(type) || isSgTypeChar(base_type))
	  appendExpression(arg_list, var);
	else {
	  ROSE_ASSERT(roseConvertIntToString);
	  SgMemberFunctionRefExp* memRef_r2 = NULL;
	  if (isSgTypeInt(type))
	    memRef_r2 = buildMemberFunctionRefExp( roseConvertIntToString, false, true);
	  else {
	    cerr << "RtedTransformation - unknown type : " << type->class_name() << endl;
	    exit(1);
	  }
	  ROSE_ASSERT(memRef_r2);
	  string symbolName3 = roseConvertIntToString->get_name().str();
	  cerr << " >>>>>>>> Symbol Member::: " << symbolName3 << endl;
	  SgArrowExp* sgArrowExp2 = buildArrowExp(varRef_l, memRef_r2);

	  SgExprListExp* arg_list2 = buildExprListExp();
	  appendExpression(arg_list2, var);
	  SgFunctionCallExp* funcCallExp2 = buildFunctionCallExp(sgArrowExp2,
								arg_list2);
	  ROSE_ASSERT(funcCallExp2);
	  //	  SgCastExp* point= buildCastExp(funcCallExp2,buildPointerType(buildCharType()));
	  appendExpression(arg_list, funcCallExp2);
	  cerr << " Created Function call  convertToString" << endl;
	}
      } else {
	SgStringVal* stringExp = buildStringVal(exp->unparseToString());
	appendExpression(arg_list, stringExp);
      }
    }

    string symbolName2 = roseFunctionCall->get_name().str();
    //cerr << " >>>>>>>> Symbol Member: " << symbolName2 << endl;
    SgMemberFunctionRefExp* memRef_r = buildMemberFunctionRefExp(
								 roseFunctionCall, false, true);
    SgArrowExp* sgArrowExp = buildArrowExp(varRef_l, memRef_r);

    SgFunctionCallExp* funcCallExp = buildFunctionCallExp(sgArrowExp,
							  arg_list);
    SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
    if (before)
      insertStatementBefore(isSgStatement(stmt), exprStmt);
    else
      insertStatementAfter(isSgStatement(stmt), exprStmt);
    //    }

  } else {
    cerr
      << "RuntimeInstrumentation :: Surrounding Statement could not be found! "
      << endl;
    exit(0);
  }
  
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
  roseFunctionCall = symbols->roseFunctionCall;
  roseRtedClose = symbols->roseRtedClose;
  roseConvertIntToString=symbols->roseConvertIntToString;
  ROSE_ASSERT(roseCreateArray);
  ROSE_ASSERT(roseArrayAccess);
  ROSE_ASSERT(roseConvertIntToString);
  ROSE_ASSERT(roseRtedClose);

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

  cerr << "\n Number of Elements in memcopy_call  : "
       << memcopy_call.size() << endl;
  std::vector<RtedArguments*>::const_iterator it4 =
    memcopy_call.begin();
  for (; it4 != memcopy_call.end(); it4++) {
    RtedArguments* funcs = *it4;
    insertFuncCall(funcs);
  }

  // insert main call to ->close();
  insertMainCloseCall(mainLast);
}



/* -----------------------------------------------------------
 * Collects information needed for transformations
 * -----------------------------------------------------------*/

void RtedTransformation::visit(SgNode* n) {

  // find MAIN ******************************************
  if (isSgFunctionDefinition(n)) {
    visit_checkIsMain(n);
  }
  // find MAIN ******************************************


  // ******************** DETECT functions in input program  *********************************************************************

  // *********************** DETECT ALL array creations ***************
  else if (isSgInitializedName(n)) {
    visit_isArraySgInitializedName(n);
  }

  // look for MALLOC
  else if (isSgAssignOp(n)) {
    visit_isArraySgAssignOp(n);
  }
  // *********************** DETECT ALL array creations ***************


  // *********************** DETECT ALL array accesses ***************
  else if (isSgPntrArrRefExp(n)) {
    // checks for array access
    visit_isArrayPntrArrRefExp(n);
  } // pntrarrrefexp
  else if (isSgVarRefExp(n) && isSgExprListExp(isSgVarRefExp(n)->get_parent())) {
    // handles calls to functions that contain array varRefExp
    // and puts the varRefExp on stack to be used by RuntimeSystem
    visit_isArrayExprListExp(n);
  }
  // *********************** DETECT ALL array accesses ***************

  else if (isSgFunctionCallExp(n)) {
    // call to a specific function that needs to be checked
    visit_isFunctionCall(n);
  }

  // ******************** DETECT functions in input program  *********************************************************************

}

