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



/* -----------------------------------------------------------
 * Perform all transformations needed (Step 2)
 * -----------------------------------------------------------*/
void RtedTransformation::transform(SgProject* project) {
  cout << "Running Transformation..." << endl;
  globalScope = getFirstGlobalScope(isSgProject(project));

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

  cerr << "\n Number of Elements in funccall_call  : "
       << function_call.size() << endl;
  std::vector<RtedArguments*>::const_iterator it4 =
    function_call.begin();
  for (; it4 != function_call.end(); it4++) {
    RtedArguments* funcs = *it4;
    string name = funcs->name;
    if (isInterestingFunctionCall(name)) {
      insertFuncCall(funcs,true);
    } else
      // add other internal function alls, such as push variable on stack
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

