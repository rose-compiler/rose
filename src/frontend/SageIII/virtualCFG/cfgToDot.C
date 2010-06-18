// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "filteredCFG.h"
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <stdint.h>
using namespace std;

namespace VirtualCFG {
#if 0 // redefinition , perviously defined in filteredCFGImpl.h, Liao, 3/6/2009
template <typename NodeT, typename EdgeT, bool Debug>
class CfgToDotImpl {
  multimap<SgNode*, NodeT> exploredNodes;
  set<SgNode*> nodesPrinted;
  ostream& o;

  public:
  CfgToDotImpl(ostream& o): exploredNodes(), nodesPrinted(), o(o) {}
  void processNodes(NodeT n);
};
#endif
template <typename NodeT, bool Debug>
inline void printNode(ostream& o, const NodeT& n) {
  string id = n.id();
  string nodeColor = "black";
  if (isSgStatement(n.getNode())) nodeColor = "blue";
  else if (isSgExpression(n.getNode())) nodeColor = "green";
  else if (isSgInitializedName(n.getNode())) nodeColor = "red";
  o << id << " [label=\"" << escapeString(Debug ? n.toStringForDebugging() : n.toString()) << "\", color=\"" << nodeColor << "\", style=\"" << (n.isInteresting() ? "solid" : "dotted") << "\"];\n";
}

template <typename EdgeT, bool Debug>
inline void printEdge(ostream& o, const EdgeT& e, bool isInEdge) {
  o << e.source().id() << " -> " << e.target().id() << " [label=\"" << escapeString(Debug ? e.toStringForDebugging() : e.toString()) << "\", style=\"" << (isInEdge ? "dotted" : "solid") << "\"];\n";
}

template <typename NodeT, typename EdgeT, bool Debug>
void printNodePlusEdges(ostream& o, NodeT n);

#if 0 //redefinition, already defined in filteredCFGImpl.h, Liao, 3/6/2009
template <typename NodeT, typename EdgeT, bool Debug>
void CfgToDotImpl<NodeT, EdgeT, Debug>::processNodes(NodeT n) {
  ROSE_ASSERT (n.getNode());
  pair<typename multimap<SgNode*, NodeT>::const_iterator,
       typename multimap<SgNode*, NodeT>::const_iterator> ip = exploredNodes.equal_range(n.getNode());
  for (typename multimap<SgNode*, NodeT>::const_iterator i = ip.first; i != ip.second; ++i) {
    if (i->second == n) return;
  }
  exploredNodes.insert(make_pair(n.getNode(), n));
  printNodePlusEdges<NodeT, EdgeT, Debug>(o, n);
  vector<EdgeT> outEdges = n.outEdges();
  for (unsigned int i = 0; i < outEdges.size(); ++i) {
    ROSE_ASSERT (outEdges[i].source() == n);
    processNodes(outEdges[i].target());
  }
  if (Debug) {
    vector<EdgeT> inEdges = n.inEdges();
    for (unsigned int i = 0; i < inEdges.size(); ++i) {
      ROSE_ASSERT (inEdges[i].target() == n);
      processNodes(inEdges[i].source());
    }
  }
}
#endif
template <typename NodeT, typename EdgeT, bool Debug>
void printNodePlusEdges(ostream& o, NodeT n) {
  printNode<NodeT, Debug>(o, n);
  vector<EdgeT> outEdges = n.outEdges();
  for (unsigned int i = 0; i < outEdges.size(); ++i) {
    printEdge<EdgeT, Debug>(o, outEdges[i], false);
  }
  if (Debug) {
    vector<EdgeT> inEdges = n.inEdges();
    for (unsigned int i = 0; i < inEdges.size(); ++i) {
      printEdge<EdgeT, Debug>(o, inEdges[i], true);
    }
  }
}

#if 0 // Enable for clustering by AST hierarchy, which makes the graph look bad
void CfgToDotImpl::processNodes(SgNode* top) {
  if (nodesPrinted.find(top) != nodesPrinted.end()) return;
  nodesPrinted.insert(top);
  o << "subgraph cluster_" << uintptr_t(top) << " {" << endl;
  o << "style=invis;" << endl;
  for (multimap<SgNode*, CFGNode>::const_iterator it = exploredNodes.lower_bound(top);
       it != exploredNodes.upper_bound(top); ++it) {
    printNodePlusEdges(o, it->second);
  }
  vector<SgNode*> children = top->get_traversalSuccessorContainer();
  for (unsigned int i = 0; i < children.size(); ++i) {
    if (children[i]) {
      processNodes(children[i]);
    }
  }
  o << "}" << endl;
}
#endif

#if 0
template <typename NodeT, typename EdgeT, bool Debug>
void CfgToDotImpl<NodeT, EdgeT, Debug>::processNodes(SgNode*) {
  for (typename multimap<SgNode*, NodeT>::const_iterator it = exploredNodes.begin();
       it != exploredNodes.end(); ++it) {
    printNodePlusEdges<NodeT, EdgeT, Debug>(o, it->second);
  }
}
#endif

ostream& cfgToDot(ostream& o, string graphName, CFGNode start) {
  o << "digraph " << graphName << " {\n";
  CfgToDotImpl<CFGNode, CFGEdge, false> impl(o);
  impl.processNodes(start);
  o << "}\n";
  return o;
}

//! dump the filtered dot graph of a virtual control flow graph starting from SgNode (start)
void cfgToDot (SgNode* start, const std::string& file_name)
{
  ROSE_ASSERT (start != NULL);
  ofstream ofile (file_name.c_str(), ios::out);
  cfgToDot(ofile, "defaultName", start->cfgForBeginning());
}

//! Dump a CFG with only interesting nodes for a SgNode
void interestingCfgToDot (SgNode* start, const std::string& file_name)
{
  ROSE_ASSERT (start != NULL);
  ofstream ofile (file_name.c_str(), ios::out);
  //cfgToDot(ofile, "defaultName", makeInterestingCfg(start));
}

ostream& cfgToDot(ostream& o, string graphName, InterestingNode start) {
  o << "digraph " << graphName << " {\n";
  CfgToDotImpl<InterestingNode, InterestingEdge, false> impl(o);
  impl.processNodes(start);
  o << "}\n";
  return o;
}

ostream& cfgToDotForDebugging(ostream& o, string graphName, CFGNode start) {
  o << "digraph " << graphName << " {\n";
  CfgToDotImpl<CFGNode, CFGEdge, true> impl(o);
  impl.processNodes(start);
  o << "}\n";
  return o;
}

  //dump the full dot graph of a virtual control flow graph starting from SgNode (start)
  void cfgToDotForDebugging(SgNode* start, const std::string& file_name)
  {
    ROSE_ASSERT (start != NULL);
    ofstream ofile (file_name.c_str(), ios::out);
    cfgToDotForDebugging(ofile, "defaultName", start->cfgForBeginning());
  }  

ostream& cfgToDotForDebugging(ostream& o, string graphName,
			      InterestingNode start) {
  o << "digraph " << graphName << " {\n";
  CfgToDotImpl<InterestingNode, InterestingEdge, true> impl(o);
  impl.processNodes(start);
  o << "}\n";
  return o;
}

} //end of namespace
