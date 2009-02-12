/******************************************
 * Category: DFA
 * Variable Liveness Analysis Definition
 * created by tps in Feb 2007
 *****************************************/

#include "rose.h"
#include "LivenessAnalysis.h"
#include "DefUseAnalysis_perFunction.h"
#include "GlobalVarAnalysis.h"
#include <boost/config.hpp>
#include <boost/bind.hpp>
using namespace std;

template <class T>
T LivenessAnalysis::merge_no_dups( T& v1,  T& v2) {
  T ret(v1);
  for (typename T::const_iterator i = v2.begin(); i != v2.end(); ++i) {
    bool notfound = true;
    for (typename T::const_iterator z = ret.begin(); z != ret.end(); ++z) {
      if (*z == *i) {
	notfound = false;
	break;
      }
    }
    if (notfound) {
      ret.push_back(*i);
    }
  }
  return ret;
}
// Get the enclosing function definition of a node
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

//!Print out live-in and live-out variables for a node
void 
LivenessAnalysis::printInAndOut(SgNode* sgNode) {
  if (DEBUG_MODE)
    cout << ">>> in and out for : " << sgNode << "  " << sgNode->class_name() << endl;
  std::vector<SgInitializedName*> currIn = in[sgNode];
  std::vector<SgInitializedName*>::const_iterator it2 = currIn.begin();
  if (DEBUG_MODE)
    cout << "   in : " ;
  for (;it2!=currIn.end();++it2) {
    SgInitializedName* init = isSgInitializedName(*it2);
    ROSE_ASSERT(init);
    std::string name =".";
    name= init->get_name().str();
    if (DEBUG_MODE)
      cout << name << ", " ;
  }
  if (DEBUG_MODE)
    cout << endl;

  std::vector<SgInitializedName*> currOut = out[sgNode];
  std::vector<SgInitializedName*>::const_iterator it3 = currOut.begin();
  if (DEBUG_MODE)
    cout << "   out : " ;
  for (;it3!=currOut.end();++it3) {
    SgInitializedName* init = isSgInitializedName(*it3);
    ROSE_ASSERT(init);
    std::string name =".";
    name= init->get_name().str();
    if (DEBUG_MODE)
      cout << name << ", " ;
  }
  if (DEBUG_MODE)
    cout << endl;
}

static bool sort_using_greater_than(SgNode* u, SgNode* v){
  return u > v;
}

/**********************************************************
 * Traverse the CFG backwards from a given node to 
 * determine whether a change has occurred at that path
 * until the first split (2 in-edges) or root.
 *********************************************************/
template <typename T>
bool LivenessAnalysis::hasANodeAboveCurrentChanged(T source) {
  bool changed = false;
  // go cfg back until split or root
  // check nodeChangeMap if a all nodes have changed.
  // if any has changed return true.
  vector<FilteredCFGEdge < IsDFAFilter > > in_edges = source.outEdges();
  if (in_edges.size()==1) {
    FilteredCFGEdge<IsDFAFilter> filterEdge = in_edges[0];
    FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.target();
    SgNode* sgNode = filterNode.getNode();
    ROSE_ASSERT(sgNode);
    bool changedInTable = nodeChangedMap[sgNode];
    if (DEBUG_MODE)
      cout << " >>> backward CFG : visiting node : " << sgNode << " " << sgNode->class_name() <<
	"  changed : " << changedInTable << endl;
    if (changedInTable) {
      return true;
    } else {
      changed = hasANodeAboveCurrentChanged(filterNode);
    }
  }
  return changed;

}


template <typename T>
bool LivenessAnalysis::defuse(T cfgNode, bool *unhandled) {
  SgNode* sgNode = cfgNode.getNode();
  ROSE_ASSERT(sgNode);
  if (visited.find(sgNode)==visited.end())
    visited[sgNode]=1;
  else
    visited[sgNode]++;
  SgNode* sgNodeBefore = getCFGPredNode(cfgNode);  

  vector<FilteredCFGEdge < IsDFAFilter > > out_edges2 = cfgNode.inEdges();
  for (vector<FilteredCFGEdge <IsDFAFilter> >::const_iterator i = out_edges2.begin(); i != out_edges2.end(); ++i) {
    FilteredCFGEdge<IsDFAFilter> filterEdge = *i;
    FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.source();
    SgNode* sgNode2 = filterNode.getNode();
    if (visited.find(sgNode2)==visited.end())
      *unhandled=true;
  }
  bool has_changed=false;
  if (DEBUG_MODE) {
    cout << "\n\n------------------------------------------------------------------\ncurrent Node: " << 
      sgNode << "  previous Node : " << sgNodeBefore << endl;
    printInAndOut(sgNode);
  }

  // get def and use for this node 
  SgInitializedName* initName = isSgInitializedName(sgNode);
  SgVarRefExp* varRef = isSgVarRefExp(sgNode);
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
  }


  if (varRef) {
    initName = varRef->get_symbol()->get_declaration();
    ROSE_ASSERT(initName);
    std::vector <SgNode*> uses = dfa->getUseFor(sgNode, initName);
    std::vector<SgNode*>::const_iterator it = uses.begin();
    for (;it!=uses.end();++it) {
      SgNode* itNode = *it;
      if (itNode==sgNode)
	useNode=true;
    }
  }

  if (DEBUG_MODE) {
    cout << "     At this point def : " << defNode << "  use : " << useNode << endl;
    if (initName)
      cout << "  initName : " << initName->get_name().str() << endl;
    cout << " Doing out = " << endl;
  }
  // do the algo for variable liveness
  out[sgNode].clear();
  vector<FilteredCFGEdge < IsDFAFilter > > out_edges = cfgNode.outEdges();
  for (vector<FilteredCFGEdge <IsDFAFilter> >::const_iterator i = out_edges.begin(); i != out_edges.end(); ++i) {
    FilteredCFGEdge<IsDFAFilter> filterEdge = *i;
    FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.target();
    SgNode* sgNodeNext = filterNode.getNode();
    ROSE_ASSERT(sgNodeNext);
    std::vector<SgInitializedName*> tmpIn = in[sgNodeNext];
    if (DEBUG_MODE)
      cout << "   out : previous node : " << sgNodeNext << " " << sgNodeNext->class_name() << "   in Size : " << 
	tmpIn.size() << "   out[sgNode].size = " << out[sgNode].size() << endl;
    //    out[sgNode].swap(tmpIn);
    std::vector<SgInitializedName*> tmpOut = out[sgNode];
    out[sgNode]=merge_no_dups(tmpOut,tmpIn);
    std::sort(out[sgNode].begin(), out[sgNode].end(),sort_using_greater_than);
  }

  if (DEBUG_MODE)
    printInAndOut(sgNode);
  if (DEBUG_MODE)
    cout << " Doing in = " << endl;

  // what if it is an assignment
  switch(sgNode->variant()) {
  case V_SgPlusPlusOp:
  case V_SgMinusMinusOp: 
  case V_SgAssignOp: 
  case V_SgModAssignOp:
  case V_SgDivAssignOp:
  case V_SgMultAssignOp:
  case V_SgLshiftAssignOp:
  case V_SgRshiftAssignOp:
  case V_SgXorAssignOp:
  case V_SgAndAssignOp:
  case V_SgMinusAssignOp:
  case V_SgPlusAssignOp: {
    // go through all initialized names for out
    // and cancel the InitializedName for in if it is 
    // defined for this node
    std::vector<SgInitializedName*> vec  = out[sgNode];
    std::vector<SgInitializedName*>::iterator inIt = vec.begin();
    for (;inIt!=vec.end();++inIt) {
      SgInitializedName* initN = isSgInitializedName(*inIt);
      std::vector <SgNode*> defs = dfa->getDefFor(sgNode, initN);      
      std::vector<SgNode*>::const_iterator it = defs.begin();
      for (;it!=defs.end();++it) {
	SgNode* itNode = *it;
	if (itNode==sgNode) {
	  defNode=true;
	  initName=initN;
	  break;
	}
      }
    }    
    if (initName)
      cout << " This is an assignment :  initName = " <<
	initName->get_name().str() << "  def : " << defNode << endl;
    else {
      cout << " !!! This is an assignment but no initName matched. defnode = "
	   << defNode << endl;
    }
    break;
  }
  default: {
    // its none of the above
    // none of the above breakPointForWhiles is hit 
    *unhandled=true;
    // take care of the case where we have none of the above within a loop (breakPointForWhile)
    // i.e. no : VarRefExp, InitializedName, FunctionDefinition ...
    // If this unhandled node has been added to the map before (visited)
    // then we do want to mark it as handled.
      if (isSgWhileStmt(sgNode) || isSgForStatement(sgNode)
	  || isSgDoWhileStmt(sgNode)) {
	if (breakPointForWhileNode==NULL) {
	  breakPointForWhileNode=sgNode;
	  breakPointForWhile++;
	  if (DEBUG_MODE)
	    cout << ">>> Setting Breakpoint : " << sgNode->class_name() << " " <<sgNode << " " << breakPointForWhile <<endl;
	} else if (sgNode==breakPointForWhileNode) {
	  // reaching the breakPoint for a second time
	  // check if any node above this node up to the branch or root has changed
	  breakPointForWhile++;
	  bool hasAnyNodeAboveChanged = hasANodeAboveCurrentChanged(cfgNode);
	  if (hasAnyNodeAboveChanged==false) {
	    // need to break this loop
	    // add current node to doNotVisitMap
	    doNotVisitMap.insert(sgNode);
	  }

	  if (DEBUG_MODE)
	    cout << ">>> Inc Breakpoint : " << sgNode->class_name() << " " <<sgNode << " " << breakPointForWhile <<endl;
	  *unhandled = false;
	  breakPointForWhileNode=NULL;
	  breakPointForWhile=0;
	  if (DEBUG_MODE)
	    cout << ">>> Resetting Breakpoint : " << sgNode->class_name() << " " <<sgNode << " " << breakPointForWhile <<endl;
	} else {
	  if (DEBUG_MODE)
	    cout << ">>> Skipping unhandled node ... " << endl;
	}
      }
  
    break;
  }
  }


  
  in[sgNode] = out[sgNode];
  if (defNode) {
    std::vector<SgInitializedName*> vec  = in[sgNode];
    std::vector<SgInitializedName*>::iterator inIt = vec.begin();
    for (;inIt!=vec.end();++inIt) {
      if (*inIt==initName) {
	vec.erase(inIt); // = initName
	break;
      }
    }
    in[sgNode]=vec;
  }
  if (useNode) {
    std::vector<SgInitializedName*> vec  = in[sgNode];
    std::vector<SgInitializedName*>::iterator inIt = vec.begin();
    bool found=false;
    for (;inIt!=vec.end();++inIt) {
      if (*inIt==initName) {
	found=true;
	break;
      }
    }
    if (!found) {
      std::string name = initName->get_name().str();
      if (DEBUG_MODE)
	cout << " did not find initName : " << name << " in in[sgNode]    size: " << in[sgNode].size() <<endl;
      in[sgNode].push_back(initName); // = varRef
      std::sort(in[sgNode].begin(), in[sgNode].end(),sort_using_greater_than);
      if (DEBUG_MODE)
	cout << " added sgNode :   new size [sgNode] = " <<in[sgNode].size() <<endl;
    }
  }

  if (defNode || useNode) {
    if (DEBUG_MODE)
      cout << " This was a def or use node " << endl;
    // has_changed only applies here
    bool equal = false;
    std::map<SgNode*, std::vector<SgInitializedName*> >::const_iterator it = in.find(sgNode);
    std::vector<SgInitializedName*> vec = out[sgNode];
    if (it!=in.end() && vec.size()>0) {
      equal = std::equal(in[sgNode].begin(),in[sgNode].end(),out[sgNode].begin());
      if (!equal)
	has_changed=true;
      if (DEBUG_MODE) {
	cout << " CHECKME : IN AND OUT ARE equal : " << equal << endl;
	printInAndOut(sgNode);
      }
    }
  } else {
    // if it is a arbitraty node, we assume it has changed, so we can traverse further
    has_changed=true;
  }

  if (DEBUG_MODE) {
    cout << " value has changed ... : " << has_changed << endl;
    printInAndOut(sgNode);
  }


  return has_changed;
}


FilteredCFGNode < IsDFAFilter > 
LivenessAnalysis::run(SgFunctionDefinition* funcDecl, bool& abortme) {
  // filter functions -- to only functions in analyzed file  
  abort=false;
  counter=0;
  nrOfNodesVisitedPF= 0;
  breakPointForWhileNode=NULL;
  breakPointForWhile=0;;
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
  bool unhandledNode=false;
  while (!worklist.empty()) {
    source = worklist.front();
    worklist.erase(worklist.begin());
    // do current node
    unhandledNode = false;
    SgNode* next = source.getNode();
    if (doNotVisitMap.find(next)!=doNotVisitMap.end())
      continue;
    valueHasChanged = defuse(source, &unhandledNode);  
    nodeChangedMap[source.getNode()] = valueHasChanged;
    // do follow-up nodes
    // get nodes of outgoing edges and pushback (if not already contained)
    if (DEBUG_MODE) {
      cout << " Current Node: " << source.getNode() << " " << source.getNode()->class_name() << " changed: " 
	   << resBool(valueHasChanged) << endl;
      cout << " Current Node: " << source.getNode() << " unhandled: " 
	   << resBool(unhandledNode) << endl;
    }
    if (valueHasChanged || unhandledNode) {
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

    counter++;
    if (counter==4000)
      abort=true;

    if (abort) {
      cerr << " \n\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>> ABORTING !! INFINITE EXECUTION ... \n\n" << endl;
      worklist.clear();
      abortme=true;
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
