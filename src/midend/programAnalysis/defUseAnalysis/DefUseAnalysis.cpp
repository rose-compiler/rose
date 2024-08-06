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
#include <boost/bind/bind.hpp>

using namespace std;

// static counter for the node numbering (for visualization)
int DefUseAnalysis::sgNodeCounter = 1;

/**********************************************************
 * Retrieve the unique int representation for a SgNode
 * according to its dataflow (sequence)
 *********************************************************/
int DefUseAnalysis::getIntForSgNode(SgNode* sgNode) {
    bool contained = searchVizzMap (sgNode);
    if (contained) {
      int nr = vizzhelp[sgNode];
      return nr;
    }
  return -1;
}

/**********************************************************
 *  Add helping ID to each node for vizz purpose
 *********************************************************/
bool DefUseAnalysis::addID(SgNode* sgNode) { 
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
  return false;
}

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
  std::pair<SgInitializedName*, SgNode*> el = make_pair(initName, defNode); 
  multitype currentList = (*tabl)[sgNode];
  if (find ( currentList.begin(), currentList.end(), el) == currentList.end())
  {
    (*tabl)[sgNode].push_back(el);
    addID(sgNode);
  }
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
#if ROSE_GCC_OMP
#pragma omp critical (DefUseAnalysisreplaceE1) 
#endif
  {
    multitype& map = table[sgNode];
    map.erase(std::remove_if(map.begin(), map.end(), boost::bind(boost::type<bool>(), DefUseAnalysismycond, boost::placeholders::_1, initName)), map.end());

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
    multitype& map = usetable[sgNode];

    // Removal of boost requires C++20
    map.erase(std::remove_if(map.begin(), map.end(), boost::bind(boost::type<bool>(), DefUseAnalysismycond, boost::placeholders::_1, initName)), map.end());

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
      (*tabl)[sgNode].clear(); // both before and other nodes have empty sets
    else   // only other node has a set
      (*tabl)[sgNode]=(*tabl)[other];
  } else {
    if (!otherFound)   // only before node has a set
      (*tabl)[sgNode]=(*tabl)[before];
    else {  // both has a set, perform the actual union operation : insert two sets into a single set
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

void DefUseAnalysis::printMultiMap(const multitype& m) {
  const multitype* multi = &m; 
  printMultiMap(multi);
}
void DefUseAnalysis::printMultiMap(const multitype* multi) {
  cout<<"\tmultitype element count:" << multi->size() <<endl;
  for (multitype::const_iterator j = multi->begin(); j != multi->end(); ++j) {  
    SgInitializedName* sgInitMM = (*j).first;
    SgNode* sgNodeMM = (*j).second;
    ROSE_ASSERT(sgInitMM);
    ROSE_ASSERT(sgNodeMM);
    cout <<"\t"
         <<sgInitMM->class_name()<<" "<<sgInitMM << " " << sgInitMM->get_qualified_name().str() << 
            " id ( " << ToString(getIntForSgNode(sgInitMM)) <<" ) - ";
    if (sgInitMM!=sgNodeMM)
     cout <<sgNodeMM->class_name()<<" "<<sgNodeMM << 
            " id ( " << ToString(getIntForSgNode(sgNodeMM)) <<" ) "<< endl;
    else
     cout << "same self node" <<endl;
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
    cout<<"........................."<<endl;
    cout << pos << ": " << ToString(theNode) << " var: " << name <<" " <<sgNode <<endl;
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
  ASSERT_not_null(node);

  convtype::iterator i= vizzhelp.find(node);
  if (i!=vizzhelp.end()) {
    isCurrentValueContained=true;
  }
  return isCurrentValueContained;
}

/**********************************************************
 *  Search for the value for a certain key in the map
 *********************************************************/
bool DefUseAnalysis::searchMap(const tabletype* ltable, SgNode* node) {
  bool isCurrentValueContained=false;
   if (!ltable->empty()) {
      if (ltable->find(node) != ltable->end()) {
         isCurrentValueContained=true;
      }
    }
  return isCurrentValueContained;
}


/******************************************
 * return vector to user
 * for any given node and initName, return all reaching definitions
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
    if (DEBUG_MODE) 
      cout << "\t function Def@"<< proc->get_file_info()->get_filename() <<":" << proc->get_file_info()->get_line() << endl;
    FilteredCFGNode <IsDFAFilter> rem_source = defuse_perfunc->run(proc,abortme);
    nrOfNodesVisited += defuse_perfunc->getNumberOfNodesVisited();

    if (rem_source.getNode() != nullptr) {
      dfaFunctions.push_back(rem_source);
    }
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

  defuse_perfunc->run(proc,abortme);
  nrOfNodesVisited = defuse_perfunc->getNumberOfNodesVisited();

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
  if (DEBUG_MODE) {
    cout << "START: DefUse Analysis " <<  (DEBUG_MODE ? "True" : "False") << endl;
  }
  ASSERT_not_null(project);

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

  if (aborted) {
    return 1;
  }
   return 0;
}
