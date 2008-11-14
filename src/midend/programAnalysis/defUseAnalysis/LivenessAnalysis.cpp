/******************************************
 * Category: DFA
 * DefUse Analysis Definition
 * created by tps in Feb 2007
 *****************************************/

#include "LivenessAnalysis.h"
#include "DefUseAnalysis_perFunction.h"
#include "GlobalVarAnalysis.h"
#include <boost/config.hpp>
#include <boost/bind.hpp>


using namespace std;

SgFunctionDefinition* 
LivenessAnalysis::getFunction(SgNode* node) {
  ROSE_ASSERT(node);
  // make sure that this node is within a function
  SgNode* parent = node->get_parent();
  ROSE_ASSERT(parent);
  while (!isSgFunctionDeclaration(parent)) {
    parent=parent->get_parent();
    ROSE_ASSERT(parent);
    if (isSgProject(parent)) {
      cerr << " This node is not within a function. Cant run Variable Liveness Analysis." << endl;
      return NULL;
    }
  }
  SgFunctionDeclaration* func = isSgFunctionDeclaration(parent);
  ROSE_ASSERT(func);
  SgFunctionDefinition* funcDef = func->get_definition();
  ROSE_ASSERT(funcDef);
  return funcDef;
}


static bool sort_using_greater_than(SgNode* u, SgNode* v){
  return u > v;
}

template <typename T>
bool LivenessAnalysis::defuse(T cfgNode) {
  SgNode* sgNode = cfgNode.getNode();
  SgNode* sgNodeBefore = getCFGPredNode(cfgNode);  
  ROSE_ASSERT(sgNode);

  bool has_changed=false;
  if (DEBUG_MODE)
    cout << "\n\n------------------------------------------------------------------\ncurrent Node: " << 
      sgNode << "  previous Node : " << sgNodeBefore << endl;

  // get def and use for this node 
  SgInitializedName* initName = isSgInitializedName(sgNode);
  bool defNode = false;
  bool useNode = false;
  if (initName) {
    std::vector <SgNode*> defs = dfa->getDefFor(sgNode, initName);
    //get the def and use for the current node
    std::vector<SgNode*>::const_iterator it = defs.begin();
    for (;it!=defs.end();++it) {
      SgNode* itNode = *it;
      if (itNode==sgNode)
	defNode=true;
    }
    std::vector <SgNode*> uses = dfa->getUseFor(sgNode, initName);
    it = uses.begin();
    for (;it!=uses.end();++it) {
      SgNode* itNode = *it;
      if (itNode==sgNode)
	useNode=true;
    }
  }

  if (DEBUG_MODE)
    cout << "     At this point def : " << defNode << "  use : " << useNode << endl;

  // do the algo for variable liveness
  out[sgNode].clear();
  vector<FilteredCFGEdge < IsDFAFilter > > out_edges = cfgNode.outEdges();
  for (vector<FilteredCFGEdge <IsDFAFilter> >::const_iterator i = out_edges.begin(); i != out_edges.end(); ++i) {
    FilteredCFGEdge<IsDFAFilter> filterEdge = *i;
    FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.target();
    SgNode* sgNodeNext = filterNode.getNode();
    ROSE_ASSERT(sgNodeNext);
    std::vector<SgInitializedName*> tmpIn = in[sgNodeNext];
    out[sgNode].swap(tmpIn);
  }


  in[sgNode] = out[sgNode];
  if (defNode) {
    std::vector<SgInitializedName*> vec  = in[sgNode];
    std::vector<SgInitializedName*>::iterator inIt = vec.begin();
    for (;inIt!=vec.end();++inIt) {
      if (*inIt==sgNode)
	vec.erase(inIt); // = initName
    }
    in[sgNode]=vec;
  }
  if (useNode) {
    in[sgNode].push_back(initName); // = initName
    std::sort(in[sgNode].begin(), in[sgNode].end(),sort_using_greater_than);
  }

  if (defNode || useNode) {
    // has_changed only applies here
    if (!std::equal(in.begin(),in.end(),out.begin()))
      has_changed=true;
  } else {
    // if it is a arbitraty node, we assume it has changed, so we can traverse further
    has_changed=true;
  }
  cerr << " value has changed ... : " << has_changed << endl;

  cerr << ">>> in and out for : " << sgNode << endl;
  std::vector<SgInitializedName*> currIn = in[sgNode];
  std::vector<SgInitializedName*>::const_iterator it2 = currIn.begin();
  for (;it2!=currIn.end();++it2) {
    SgInitializedName* init = isSgInitializedName(*it2);
    std::string name = init->get_name().str();
    cerr << "   in : " << name;
  }
  cerr << endl;

  std::vector<SgInitializedName*> currOut = out[sgNode];
  std::vector<SgInitializedName*>::const_iterator it3 = currOut.begin();
  for (;it3!=currOut.end();++it3) {
    SgInitializedName* init = isSgInitializedName(*it3);
    std::string name = init->get_name().str();
    cerr << "   out : " << name;
  }
  cerr << endl;
  return has_changed;
}


FilteredCFGNode < IsDFAFilter > 
LivenessAnalysis::run(SgFunctionDefinition* funcDecl) {
  // filter functions -- to only functions in analyzed file  
  nrOfNodesVisitedPF= 0;
  breakPointForWhileNode=NULL;
  breakPointForWhile=0;
  // clear those maps for each function run
  doNotVisitMap.clear();
  nodeChangedMap.clear();

  string funcName = getFullName(funcDecl);
  //  DEBUG_MODE = false;
  DEBUG_MODE_EXTRA=false;

  if (funcName=="") {
    FilteredCFGNode < IsDFAFilter > empty(CFGNode(NULL, 0));
    return empty;
  }
  ROSE_ASSERT(funcDecl);

  if (DEBUG_MODE) 
    cout << " Found function " << funcName << endl;

  // DFA on that function
  vector<FilteredCFGNode<IsDFAFilter> > worklist;
  
  //waitAtMergeNode.clear();

  // add this node to worklist and work through the outgoing edges
  FilteredCFGNode < IsDFAFilter > source =
    FilteredCFGNode < IsDFAFilter > (funcDecl->cfgForEnd());
  CFGNode cmpSrc = CFGNode(funcDecl->cfgForEnd());
  FilteredCFGNode < IsDFAFilter > rem_source = source;

  if (DEBUG_MODE) {
    std::ofstream f("cfg.dot");
    cfgToDot(f, string("cfg"), source);
    f.close();
  }

  
  worklist.push_back(source);
  vector<FilteredCFGNode<IsDFAFilter> > debug_path;
  debug_path.push_back(source);


  bool valueHasChanged = false;
  while (!worklist.empty()) {
    source = worklist.front();
    worklist.erase(worklist.begin());
    // do current node
    SgNode* next = source.getNode();
    if (doNotVisitMap.find(next)!=doNotVisitMap.end())
      continue;
    valueHasChanged = defuse(source);  
    nodeChangedMap[source.getNode()] = valueHasChanged;
    // do follow-up nodes
    // get nodes of outgoing edges and pushback (if not already contained)
    if (DEBUG_MODE) {
      cout << " Current Node: " << source.getNode() << " changed: " 
	   << resBool(valueHasChanged) << endl;
      cout << " Current Node: " << source.getNode() << endl;
    }
    if (valueHasChanged ) {
      vector<FilteredCFGEdge < IsDFAFilter > > out_edges = source.inEdges();
      for (vector<FilteredCFGEdge <IsDFAFilter> >::const_iterator i = out_edges.begin(); i != out_edges.end(); ++i) {
	FilteredCFGEdge<IsDFAFilter> filterEdge = *i;
	FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.source();
	if (find(worklist.begin(), worklist.end(), filterNode)==worklist.end()) {
	  worklist.push_back(filterNode);
	  debug_path.push_back(filterNode);
	}
      }
      if (DEBUG_MODE) 
	printCFGVector(worklist);
    }
  }
  nrOfNodesVisitedPF= debug_path.size();

  if (DEBUG_MODE) {
    cout << " Exiting function " << funcName << endl;
    cout << "\nNr of nodes visited " << debug_path.size() << "  of nodes : "  << endl;
    printCFGVector(debug_path);
  }
  return rem_source;

}
