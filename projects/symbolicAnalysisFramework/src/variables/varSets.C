#include "varSets.h"
#include<vector>
using namespace std;

namespace varSets
{

  // adds to declaredVars the set of all variables declared within the given AST sub-tree
  // onlyGlobal - if onlyGlobal==true, only global variables are added
  // getCompilerGen - if =true, the returned set includes compiler-generated variables and doesn't if =false
  void getDeclaredVars(SgNode* root, bool onlyGlobal, varIDSet& declaredVars, bool getCompilerGen);

  // adds to refVars the set of all variables referenced in the given function
  void getReferencedVars(SgFunctionDefinition* func, varIDSet &refVars);

  /*=======================================
    =============   Globals   =============
    =======================================*/

  static varIDSet globalVars;
  static varIDSet globalArrays;
  static varIDSet globalScalars;
  static bool globalVars_initialized=false;

  // initializes globalVars, globalArrays and globalScalars
  void initGlobalVars(SgProject* project, bool getCompilerGen=false)
  {
    if(!globalVars_initialized)
    {
      getDeclaredVars(project, true, globalVars, getCompilerGen);
      globalArrays = arraysFilter(globalVars);
      globalScalars = scalarsFilter(globalVars);
      globalVars_initialized = true;
    }
  }

  // returns the set of all global variables
  // getCompilerGen - if =true, the returned set includes compiler-generated variables and doesn't if =false
  varIDSet& getGlobalVars(SgProject* project, bool getCompilerGen)
  {
    initGlobalVars(project, getCompilerGen);
    return globalVars;
  }

  // returns the set of all global arrays
  // getCompilerGen - if =true, the returned set includes compiler-generated variables and doesn't if =false
  varIDSet& getGlobalArrays(SgProject* project, bool getCompilerGen)
  {
    initGlobalVars(project, getCompilerGen);
    return globalArrays;
  }

  // returns the set of all global scalars
  // getCompilerGen - if =true, the returned set includes compiler-generated variables and doesn't if =false
  varIDSet& getGlobalScalars(SgProject* project, bool getCompilerGen)
  {
    initGlobalVars(project, getCompilerGen);
    return globalScalars;
  }


  /*=======================================
    ============   Locals   ============
    =======================================*/

  static map <Function, bool> localVars_initialized;
  static map <Function, varIDSet> localVars;
  static map <Function, varIDSet> localArrays;
  static map <Function, varIDSet> localScalars;

  void initLocalVars(const Function& func, bool getCompilerGen=false)
  {
    if(localVars_initialized.find(func) == localVars_initialized.end())
    {
      //printf("initLocalVars(%s) initializing, func.get_declaration()=%p\n", func.get_name().str(), func.get_declaration());
      getDeclaredVars(func.get_declaration(), false, localVars[func], getCompilerGen);
      getDeclaredVars(func.get_definition(), false, localVars[func], getCompilerGen);
      localArrays[func] = arraysFilter(localVars[func]);
      localScalars[func] = scalarsFilter(localVars[func]);
      localVars_initialized[func] = true;
    }	
  }

  // returns the set of variables declared in the given function
  varIDSet& getLocalVars(const Function& func, bool getCompilerGen)
  {
    initLocalVars(func, getCompilerGen);
    return localVars[func];
  }

  // returns the set of arrays declared in the given function
  varIDSet& getLocalArrays(const Function& func, bool getCompilerGen)
  {
    initLocalVars(func, getCompilerGen);
    return localArrays[func];
  }

  // returns the set of scalars declared in the given function
  varIDSet& getLocalScalars(const Function& func, bool getCompilerGen)
  {
    initLocalVars(func, getCompilerGen);
    return localScalars[func];
  }
  /*=======================================
    ========  Referenced Variables  =======
    =======================================*/

  static map <Function, bool> refVars_initialized;
  static map <Function, varIDSet> refVars;
  static map <Function, varIDSet> refArrays;
  static map <Function, varIDSet> refScalars;

  void initFuncRefVars(const Function& func, bool getCompilerGen=false)
  {
    if(refVars_initialized.find(func) == refVars_initialized.end())
    {
      //printf("initFuncRefVars\n");
      getReferencedVars(func.get_definition(), refVars[func]);
      refArrays[func] = arraysFilter(refVars[func]);
      refScalars[func] = scalarsFilter(refVars[func]);
      refVars_initialized[func] = true;
    }	
  }

  // returns the set of variables referenced in the given function
  varIDSet& getFuncRefVars(const Function& func)
  {
    initFuncRefVars(func);
    return refVars[func];
  }

  // returns the set of arrays referenced in the given function
  varIDSet& getFuncRefArrays(const Function& func)
  {
    initFuncRefVars(func);
    return refArrays[func];
  }

  // returns the set of scalars referenced in the given function
  varIDSet& getFuncRefScalars(const Function& func)
  {
    initFuncRefVars(func);
    return refScalars[func];
  }


  /*=======================================
    ============   Utilities   ============
    =======================================*/

  // adds to declaredVars the set of all variables declared within the given AST sub-tree
  // onlyGlobal - if onlyGlobal==true, only global variables are added
  // getCompilerGen - if =true, the returned set includes compiler-generated variables and doesn't if =false
  void getDeclaredVars(SgNode* root, bool onlyGlobal, varIDSet& declaredVars, bool getCompilerGen=false)
  {
    /*!!! NOTE: THIS CODE DOES NOT CAPTURE FIELDS WITHIN STRUCTS, UNIONS AND CLASSES !!! */
    Rose_STL_Container<SgNode*> initNames = NodeQuery::querySubTree(root, V_SgInitializedName);
    for(Rose_STL_Container<SgNode*>::iterator it = initNames.begin(); it != initNames.end(); it++)
    {
      ROSE_ASSERT(isSgInitializedName(*it));

      // skip over compiler-generated names if necessary
      if(!getCompilerGen && (*it)->get_file_info()->isCompilerGenerated())
        continue;

      varID var(isSgInitializedName(*it));
      //printf("   getDeclaredVars() var=%s\n", var.str().c_str());
      // if this is a declaration of a variable with a globally visible scope
      //if(isGlobalVarDecl(isSgInitializedName(*it)))
      if(!onlyGlobal || var.isGlobal())
      {
        //printf("       inserting\n");
        declaredVars.insert(var);
      }
    }
  }

  // adds to refVars the set of all variables referenced in the given function
  void getReferencedVars(SgFunctionDefinition* func, varIDSet &refVars)
  {
    //	printf("getting vars of function %s, %s\n", func->get_declaration()->get_name().str(), func->get_file_info()->isCompilerGenerated()? "Compiler generated": "In Original Code");

    if(!func->get_file_info()->isCompilerGenerated())
    {
      refVars = getVarRefsInSubtree(func);
    }
  }

  // given a set of variables, creates a new set that only contains the 
  // non-array variables in the original set and returns this set
  varIDSet arraysFilter(varIDSet& vars)
  {
    varIDSet arrays;
    for(varIDSet::iterator it=vars.begin(); it!=vars.end(); it++)
    {
      if(((varID)(*it)).isArrayType())
      {
        arrays.insert(*it);
      }
    }
    return arrays;
  }

  // given a set of variables, creates a new set that only contains the 
  // non-array variables in the original set and returns this set
  varIDSet scalarsFilter(varIDSet& vars)
  {
    varIDSet scalars;
    for(varIDSet::iterator it=vars.begin(); it!=vars.end(); it++)
    {
      if(!((varID)(*it)).isArrayType())
      {
        scalars.insert(*it);
      }
    }
    return scalars;
  }

}//namespace varSets
