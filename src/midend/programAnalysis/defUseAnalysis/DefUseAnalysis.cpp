/******************************************
 * Category: DFA
 * DefUse Analysis Definition
 * created by tps in Feb 2007
 *****************************************/
// tps : Switching from rose.h to sage3 changed size from 18,5 MB to 8,2MB

#include "sage3basic.h"
#include "DefUseAnalysis.h"
#include "DefUseAnalysis_perFunction.h"
#include "GlobalVarAnalysis.h"
#include <boost/config.hpp>
#include <boost/bind.hpp>


using namespace std;
// static counter for the node numbering (for visualization)
int DefUseAnalysis::sgNodeCounter = 1;


/**********************************************************
 * Retrieve the unique int representation for a SgNode
 * according to its dataflow (sequence)
 *********************************************************/
int DefUseAnalysis::getIntForSgNode(SgNode* sgNode) {
  //  if (visualizationEnabled) {
    bool contained = searchVizzMap (sgNode);
    if (contained) {
      int nr = vizzhelp[sgNode];
      return nr;
    }
    //  }
  return -1;
}

/**********************************************************
 *  Add helping ID to each node for vizz purpose
 *********************************************************/
bool DefUseAnalysis::addID(SgNode* sgNode) { 
  //  if (visualizationEnabled) {
  if (searchVizzMap(sgNode)==false) {
#if ROSE_GCC_OMP
#pragma omp critical (DefUseAnalysisaddID) 
#endif
      {
	ROSE_ASSERT(sgNode);
      sgNodeCounter++;
      vizzhelp[sgNode] = sgNodeCounter;
      }
      return true;
  }
  //  }
  return false;
}


/**********************************************************
 *  Add an element to the indirect definition table
 *********************************************************
void DefUseAnalysis::addIDefElement(SgNode* sgNode, 
				SgInitializedName* initName) { 
  idefTable.insert(make_pair(sgNode, initName));
}
*/

/**********************************************************
 *  Add an element to the table
 *********************************************************/
void DefUseAnalysis::addDefElement(SgNode* sgNode, 
				SgInitializedName* initName,
				SgNode* defNode) { 
  addAnyElement(&table, sgNode, initName, defNode);
}

/**********************************************************
 *  Add an element to the table
 *********************************************************/
void DefUseAnalysis::addUseElement(SgNode* sgNode, 
				SgInitializedName* initName,
				SgNode* defNode) { 
  addAnyElement(&usetable, sgNode, initName, defNode);
}

/**********************************************************
 *  Add an element to the table
 *********************************************************/
void DefUseAnalysis::addAnyElement(tabletype* tabl, SgNode* sgNode, 
				SgInitializedName* initName,
				SgNode* defNode) { 
#if ROSE_GCC_OMP
#pragma omp critical (DefUseAnalysisaddUseE) 
#endif
  //  (*tabl)[sgNode].insert(make_pair(initName, defNode));
  (*tabl)[sgNode].push_back(make_pair(initName, defNode));
   addID(sgNode);
}

bool DefUseAnalysismycond(std::pair<SgInitializedName*,SgNode* > n1, SgInitializedName* init) {
  if (n1.first==init)
    return true;
  return false;
}

/**********************************************************
 *  Replace an element in the table
 *********************************************************/
void DefUseAnalysis::replaceElement(SgNode* sgNode, 
				    SgInitializedName* initName) {
  ROSE_ASSERT(initName);
  // if the node is contained but not identical, then we overwrite it
  // otherwise, we do nothing
  //table[sgNode].erase(table[sgNode].lower_bound(initName), table[sgNode].upper_bound(initName));
#if ROSE_GCC_OMP
#pragma omp critical (DefUseAnalysisreplaceE1) 
#endif
  {
    //table[sgNode].erase(initName);
    //    table[sgNode].insert(make_pair(initName,sgNode));

    multitype& map = table[sgNode];
    map.erase(std::remove_if(map.begin(), map.end(), boost::bind(boost::type<bool>(), DefUseAnalysismycond, _1, initName)), map.end());
    //         table[sgNode].erase(it);

    table[sgNode].push_back(make_pair(initName,sgNode));
  }
}

/**********************************************************
 *  Replace an element in the table
 *********************************************************/
void DefUseAnalysis::clearUseOfElement(SgNode* sgNode, 
				    SgInitializedName* initName) {
#if ROSE_GCC_OMP
#pragma omp critical (DefUseAnalysisclearUse) 
#endif
  {
  //  usetable[sgNode].erase(initName);

    multitype& map = usetable[sgNode];
    map.erase(std::remove_if(map.begin(), map.end(), boost::bind(boost::type<bool>(), DefUseAnalysismycond, _1, initName)), map.end());

  }
}

/**********************************************************
 *  Union of two maps
 *********************************************************/
void DefUseAnalysis::mapDefUnion(SgNode* before, SgNode* other, SgNode* sgNode) {
  mapAnyUnion(&table, before, other, sgNode);
}

/**********************************************************
 *  Union of two maps
 *********************************************************/
void DefUseAnalysis::mapUseUnion(SgNode* before, SgNode* other, SgNode* sgNode) {
  mapAnyUnion(&usetable, before, other, sgNode);
}

/**********************************************************
 *  Union of two maps
 *********************************************************/
void DefUseAnalysis::mapAnyUnion(tabletype* tabl, SgNode* before, SgNode* other, SgNode* sgNode) {
  
  bool beforeFound = true;
  if ((*tabl).find(before)==(*tabl).end())
    beforeFound = false;
  bool otherFound = true;
  if ((*tabl).find(other)==(*tabl).end())
    otherFound = false;

  addID(sgNode);

#if ROSE_GCC_OMP
#pragma omp critical (DefUseAnalysismapUse)
#endif
  if (!beforeFound) {
    if (!otherFound)
      (*tabl)[sgNode].clear();
    else 
      (*tabl)[sgNode]=(*tabl)[other];
  } else {
    if (!otherFound) 
      (*tabl)[sgNode]=(*tabl)[before];
    else {

      const multitype& multiA  = (*tabl)[before];
      const multitype& multiB  = (*tabl)[other];
      std::set<std::pair<SgInitializedName*, SgNode*> > s_before(multiA.begin(), multiA.end());
       ROSE_ASSERT (s_before.size() == (*tabl)[before].size());

      s_before.insert(multiB.begin(), multiB.end());
      multitype multiC(s_before.begin(), s_before.end());
      (*tabl)[sgNode].swap(multiC);
      
    }
  }
}

/**********************************************************
 *  return whether a node is a global variable
 *  meaning is it in the globalVar table
 *********************************************************/
bool DefUseAnalysis::isNodeGlobalVariable(SgInitializedName* sgNode){
  //  SgNode* node = sgNode;
  return isContainedinVector(sgNode, globalVarList);
}


/**********************************************************
 *  get the InitName for a sgNode
 *********************************************************/
std::string DefUseAnalysis::getInitName(SgNode* sgNode){
  SgInitializedName* initName = NULL;
  string name = "none";
  if (isSgVarRefExp(sgNode)) {
    SgVarRefExp* varRefExp = isSgVarRefExp(sgNode);
    initName = varRefExp->get_symbol()->get_declaration();
    name = initName->get_qualified_name().str();
  }
  else if (isSgInitializedName(sgNode)) {
    initName =isSgInitializedName(sgNode);
    name = initName->get_qualified_name().str();
  }  
  else {
    name = sgNode->class_name();
  }
  return name;
}


/**********************************************************
 *  print out the multimap
 *********************************************************/
void DefUseAnalysis::printMultiMap(const multitype* multi) {
  for (multitype::const_iterator j = multi->begin(); j != multi->end(); ++j) {  
    SgInitializedName* sgInitMM = (*j).first;
    SgNode* sgNodeMM = (*j).second;
    ROSE_ASSERT(sgInitMM);
    ROSE_ASSERT(sgNodeMM);
    //cout << "  ..  initName:" << sgInitMM->get_qualified_name().str() << " ( " <<
    //  ToString(getIntForSgNode(sgInitMM)) << " ) - SgNode " << 
    //  ToString(getIntForSgNode(sgNodeMM)) << endl;
  }      
}

/**********************************************************
 *  print out the table containing all nodes
 *********************************************************/
void DefUseAnalysis::printDefMap() {
  printAnyMap(&table);
}

/**********************************************************
 *  print out the table containing all nodes
 *********************************************************/
void DefUseAnalysis::printUseMap() {
  printAnyMap(&usetable);
}

/**********************************************************
 *  print out the table containing all nodes
 *********************************************************/
void DefUseAnalysis::printAnyMap(tabletype* tabl) {
  int pos = 0;
  cout << "\n **************** MAP ************************** " << endl;
  for (tabletype::const_iterator i = tabl->begin(); i != tabl->end(); ++i) {  
    pos++;
    SgNode* sgNode = (*i).first;
    ROSE_ASSERT(sgNode);
    multitype multi = (*i).second;
    string name = getInitName(sgNode);
    int theNode = getIntForSgNode(sgNode);
    cout << pos << ": " << ToString(theNode) << " var: " << name << endl;
    printMultiMap(&multi);
  }
}

/**********************************************************
 *  Return the size of the table
 *********************************************************/
int DefUseAnalysis::getDefSize() {
  return table.size();
}

/**********************************************************
 *  Return the size of the table
 *********************************************************/
int DefUseAnalysis::getUseSize() {
  return usetable.size();
}

/**********************************************************
 *  Search for the value for a certain key in the map
 *********************************************************/
bool DefUseAnalysis::searchMap(SgNode* node) {
  return searchMap(&table, node);
}



/**********************************************************
 *  Search for the value for a certain key in the vizzmap
 *********************************************************/
bool DefUseAnalysis::searchVizzMap(SgNode* node) {
  bool isCurrentValueContained=false;
  //  std::cerr << " size of vizzhelp " << vizzhelp.size() << std::endl;
  ROSE_ASSERT(node);

  convtype::iterator i= vizzhelp.find(node);
  if (i!=vizzhelp.end()) {
    isCurrentValueContained=true;
  }
#if 0
    convtype::const_iterator i = vizzhelp.begin();
    i = vizzhelp.begin();
    //SgNode* sgNodeMM = NULL;
    for (; i != vizzhelp.end(); ++i) {
      SgNode* initNameMM = (*i).first;
      if (initNameMM==node) {
	isCurrentValueContained=true;
	break;
      }
    } 
#endif
  return isCurrentValueContained;
}

/**********************************************************
 *  Search for the value for a certain key in the map
 *********************************************************/
bool DefUseAnalysis::searchMap(const tabletype* ltable, SgNode* node) {
  bool isCurrentValueContained=false;
  //  std::cerr << " size map : " << ltable->size() << std::endl;
#if 0   
  if (ltable->size()>0) {
    tabletype::const_iterator i = ltable->begin();
    for (; i != ltable->end(); ++i) {
      SgNode* initNameMM = (*i).first;
      if (initNameMM==node) {
	isCurrentValueContained=true;
	break;
      }
    } 
  }
#else
   if (!ltable->empty()) {
      if (ltable->find(node) != ltable->end()) {
         isCurrentValueContained=true;
      }
    }
#endif  
  return isCurrentValueContained;
}


/******************************************
 * return vector to user
 * for any given node and initName, return all definitions 
 *****************************************/
std::vector < SgNode* > DefUseAnalysis::getDefFor(SgNode* node, SgInitializedName* initName) {
  multitype multi = getDefMultiMapFor(node);
  return getAnyFor( &multi, initName); 
}

/******************************************
 * return vector to user
 * for any given node and initName, return all definitions 
 *****************************************/
std::vector < SgNode* > DefUseAnalysis::getUseFor(SgNode* node, SgInitializedName* initName) {
  multitype multi = getUseMultiMapFor(node);
  return getAnyFor(&multi, initName); 
}

/******************************************
 * return vector to user
 * for any given node and initName, return all definitions 
 *****************************************/
std::vector < SgNode* > DefUseAnalysis::getAnyFor(const multitype* multi, SgInitializedName* initName) {
  vector < SgNode*> defNodes;
  defNodes.clear();
  //multitype multi = getDefUseFor(node);
  if (multi->size()>0) {
    multitype::const_iterator i = multi->begin();
    for (; i!=multi->end();++i) {
      SgInitializedName* initNameMM = isSgInitializedName(i->first);
      SgNode* thenode = i->second;
      if (initName==initNameMM) {
	// we have found the right node and right initName
	defNodes.push_back(thenode);
      }
    }
  }
  return defNodes;
}

/******************************************
 * return multimap to user
 * for any given node, return all definitions 
 *****************************************/
std::vector <std::pair < SgInitializedName* , SgNode*> > DefUseAnalysis::getDefMultiMapFor(SgNode* node) {
  multitype multi;
  if (searchMap(&table, node)==true) {
    // multimap is contained
    multi = table[node];
  }
  return multi;
}

/******************************************
 * return multimap to user
 * for any given node, return all definitions 
 *****************************************/
std::vector <std::pair < SgInitializedName* , SgNode*> > DefUseAnalysis::getUseMultiMapFor(SgNode* node) {
  multitype multi;
  if (searchMap(&usetable, node)==true) {
    // multimap is contained
    multi = usetable[node];
  }
  return multi;
}

/******************************************
 * return all global variables
 *****************************************/
vector <SgInitializedName* > DefUseAnalysis::getGlobalVariables() {
  return globalVarList;
}

/******************************************
 * Find all global variables
 * and add them to def-use table
 *****************************************/
void  DefUseAnalysis::find_all_global_variables() {
  if (DEBUG_MODE) 
    cout << "START: Finding global variables" << endl;

  GlobalVarAnalysis* globalVars = new GlobalVarAnalysis(DEBUG_MODE, project, this);
  globalVarList = globalVars->run();
  delete globalVars;
  //bool test = isNodeGlobalVariable(*(globalVarList.begin()));
  //cerr << "isGlobalVar : " << resBool(test) << endl; 

  if (DEBUG_MODE) {
    cout << "FINISH: Finding global variables" << endl; 
    printDefMap();
  }
}

/******************************************
 * print the DFA Graph to DOT
 *****************************************/
void DefUseAnalysis::dfaToDOT() {
    std::ofstream f2("dfa.dot");
    dfaToDot(f2, string("dfa"), dfaFunctions, this);     
    f2.close();
}

/******************************************
 * Traversal over all functions
 * this needs to be improved... for correctness, the traversal must 
 * be according to controlflow (otherwise global variables are incorrect)
 *****************************************/
bool  DefUseAnalysis::start_traversal_of_functions() {
  if (DEBUG_MODE) 
    cout << "START: Traversal over Functions" << endl;

  nrOfNodesVisited = 0;
  dfaFunctions.clear();

  // Traverse through each FunctionDefinition and check for DefUse
  Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition); 
  DefUseAnalysisPF* defuse_perfunc = new DefUseAnalysisPF(DEBUG_MODE, this);
  bool abortme=false;
  for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i) {
    SgFunctionDefinition* proc = isSgFunctionDefinition(*i);
    FilteredCFGNode <IsDFAFilter> rem_source = defuse_perfunc->run(proc,abortme);
    nrOfNodesVisited += defuse_perfunc->getNumberOfNodesVisited();
    //cout << nrOfNodesVisited << " ......... function " << proc->get_declaration()->get_name().str() << endl; 
    if (rem_source.getNode()!=NULL)
      dfaFunctions.push_back(rem_source);
  }
  delete defuse_perfunc;

  if (DEBUG_MODE) {
    dfaToDOT();
  }

  if (DEBUG_MODE) 
    cout << "FINISH: Traversal over Functions" << endl;
  return abortme;  
}

/******************************************
 * Traversal over one function
 *****************************************/
int  
DefUseAnalysis::start_traversal_of_one_function(SgFunctionDefinition* proc) {

  nrOfNodesVisited = 0;
  bool abortme=false;
  DefUseAnalysisPF*  defuse_perfunc = new DefUseAnalysisPF(false, this);
  FilteredCFGNode <IsDFAFilter> rem_source = defuse_perfunc->run(proc,abortme);
  nrOfNodesVisited = defuse_perfunc->getNumberOfNodesVisited();
  //cout << " nodes visited: " << nrOfNodesVisited << " ......... function " << proc->get_declaration()->get_name().str() << endl; 
  
  return nrOfNodesVisited;
}

/******************************************
 * Delegation to run
 ******************************************/
int DefUseAnalysis::run(bool debug) {
  if (debug) 
    DEBUG_MODE = true;
  else 
    DEBUG_MODE = false;
  return run();
}

/******************************************
 * This algo consists of two parts: 
 * a) locate all global variables and add them to the def-use table  
 * b) Traverse all functions of the program and create def-use relations
 * return 0 if successful, 1 if fails
 *****************************************/
int DefUseAnalysis::run() {
  bool aborted;
  sgNodeCounter = 1;
  nrOfNodesVisited = 0;
  if (DEBUG_MODE) 
    cout << "START: DefUse Analysis " <<  (DEBUG_MODE ? "True" : "False") << endl;
  // assert input is correct
  ROSE_ASSERT(project != NULL);

  table.clear();
  vizzhelp.clear();

  clock_t start = clock();
  find_all_global_variables();
  // traverse through all functions and for each function doWorklist
  aborted=start_traversal_of_functions();
  clock_t ends = clock();
  if (DEBUG_MODE)
  {
  cout << "\n\n>>>>> Time for dfa-test: " << (double) (ends - start) / CLOCKS_PER_SEC << " sec"<< endl;
  cout <<     ">>>>> Total CFG nodes: " << getDefSize() << 
    "  --  #CFG nodes visited: "<< nrOfNodesVisited << endl;
    }

  //if (DEBUG_MODE) 
  //cout << "FINISH: DefUse Analysis " <<  (DEBUG_MODE ? "True" : "False") << endl;
  if (aborted) 
    return 1;
   return 0;
}
