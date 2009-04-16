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
RtedTransformation::insertFunctionCall(RTedFunctionCall* funcCall) {
  insertFunctionCall(funcCall, true);
  insertFunctionCall(funcCall, false);
}

void 
RtedTransformation::insertFunctionCall(RTedFunctionCall* funcCall, 
				       bool before) {
  SgStatement* stmt = getSurroundingStatement(funcCall->varRefExp);
  ROSE_ASSERT(stmt);
  if (isSgStatement(stmt)) {
    SgScopeStatement* scope = stmt->get_scope();
    ROSE_ASSERT(scope);
    string name = funcCall->name;
    SgStringVal* callNameExp = buildStringVal(name);
    SgStringVal* callNameExp2 = buildStringVal(funcCall->mangled_name);
    SgBoolValExp* boolVal = buildBoolValExp(before);

    SgExprListExp* arg_list = buildExprListExp();
    appendExpression(arg_list, callNameExp);
    appendExpression(arg_list, callNameExp2);
    appendExpression(arg_list, boolVal);

    SgVarRefExp* varRef_l = buildVarRefExp("runtimeSystem", globalScope);
    string symbolName = varRef_l->get_symbol()->get_name().str();
    //cerr << " >>>>>>>> Symbol VarRef: " << symbolName << endl;

    ROSE_ASSERT(roseFunctionCall);
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

  cerr << "\n Number of Elements in create_function_call  : "
       << create_function_call.size() << endl;
  std::vector<RTedFunctionCall*>::const_iterator itf =
    create_function_call.begin();
  for (; itf != create_function_call.end(); itf++) {
    RTedFunctionCall* funcs = *itf;
    insertFunctionCall(funcs);
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

  // ******************** DETECT functions in input program  *********************************************************************

}

