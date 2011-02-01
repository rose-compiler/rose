/******************************************
 * Category: DFA
 * GlobalVar Analysis Definition
 * created by tps in Feb 2007
 *****************************************/
// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 17,3 MB to 7,0MB

#include "sage3basic.h"
#include "GlobalVarAnalysis.h"

using namespace std;

/******************************************
 * filter out global variables from external libraries
 *****************************************/
bool GlobalVarAnalysis::isFromLibrary(SgInitializedName* initName) {
  // Liao, 8/26/2009, Not used and caused problem for builtin function's parameters
//  string name = initName->get_qualified_name().str();
  Sg_File_Info* fi = initName->get_file_info();
  if (fi->isCompilerGenerated())
    return true;
  string filename = fi->get_filenameString();
  if ((filename.find("/include/") != std::string::npos))
      return true;

  return false; 
}

/******************************************
 * is this variable a global variable ?
 *****************************************/
bool GlobalVarAnalysis::isGlobalVar(SgInitializedName* initName) {
  SgScopeStatement* scope = initName->get_scope();
    bool globalVar = false;
    if (isSgGlobal(scope))
      globalVar=true;
    return globalVar;
}


/******************************************
 * Detect all global variables and add them to table
 *****************************************/
vector<SgInitializedName*> GlobalVarAnalysis::run() {
  vector<SgInitializedName*> globalVars;
  globalVars.clear();
  
  Rose_STL_Container<SgNode*> initNames = NodeQuery::querySubTree(project, V_SgInitializedName);
  for (Rose_STL_Container<SgNode*>::const_iterator i = initNames.begin(); i != initNames.end(); ++i) {
    SgInitializedName* iName = isSgInitializedName(*i);
    if (isFromLibrary(iName))
      continue;
    bool global = isGlobalVar(iName);
    if (global) {
      globalVars.push_back(iName);
      dfa->addDefElement(iName, iName, iName);
    }
  }

  if (DEBUG_MODE) {
    int nr=0;
    for (std::vector<SgInitializedName*>::const_iterator c = globalVars.begin(); c!=globalVars.end(); ++c) {
      SgInitializedName* iName = isSgInitializedName(*c); 
      cout << (++nr) << " : " << iName->get_qualified_name().str() << 
        " ( " << iName << " ) " << endl;  
    }
    cout << " Amount of global variables : " << globalVars.size() << endl;
    cout << " -----------------------------------------------------------------*\n\n" << endl;
  }

  return globalVars;
}
