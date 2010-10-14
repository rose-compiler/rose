#ifndef VAR_SETS_H
#define VAR_SETS_H

#include "common.h"
#include "VirtualCFGIterator.h"
#include "CallGraphTraverse.h"
#include "variables.h"

namespace varSets
{

  /*=======================================
    =============   Globals   =============
    =======================================*/

  // returns the set of all global variables
  // getCompilerGen - if =true, the returned set includes compiler-generated variables and doesn't if =false
  varIDSet& getGlobalVars(SgProject* project, bool getCompilerGen=false);

  // returns the set of all global arrays
  // getCompilerGen - if =true, the returned set includes compiler-generated variables and doesn't if =false
  varIDSet& getGlobalArrays(SgProject* project, bool getCompilerGen=false);

  // returns the set of all global scalars
  // getCompilerGen - if =true, the returned set includes compiler-generated variables and doesn't if =false
  varIDSet& getGlobalScalars(SgProject* project, bool getCompilerGen=false);



  /*=======================================
    ============   Locals   ============
    =======================================*/  

  // returns the set of variables declared in the given function
  varIDSet& getLocalVars(const Function& func, bool getCompilerGen=false);

  // returns the set of arrays declared in the given function
  varIDSet& getLocalArrays(const Function& func, bool getCompilerGen=false);

  // returns the set of scalars declared in the given function
  varIDSet& getLocalScalars(const Function& func, bool getCompilerGen=false);


  /*=======================================
    ========  Referenced Variables  =======
    =======================================*/

  // returns the set of variables referenced in the given function
  varIDSet& getFuncRefVars(const Function& func);

  // returns the set of arrays referenced in the given function
  varIDSet& getFuncRefArrays(const Function& func);

  // returns the set of scalars referenced in the given function
  varIDSet& getFuncRefScalars(const Function& func);

  /*=======================================
    ============   Utilities   ============
    =======================================*/

  // adds to declaredVars the set of all variables declared within the given AST sub-tree
  // getCompilerGen - if =true, the returned set includes compiler-generated variables and doesn't if =false
  void getDeclaredVars(SgNode* root, bool getCompilerGen, varIDSet& declaredVars);

  // given a set of variables, creates a new set that only contains the 
  // non-array variables in the original set and returns this set
  varIDSet arraysFilter(varIDSet& vars);

  // given a set of variables, creates a new set that only contains the 
  // non-array variables in the original set and returns this set
  varIDSet scalarsFilter(varIDSet& vars);

}//namespace varSets

#endif
