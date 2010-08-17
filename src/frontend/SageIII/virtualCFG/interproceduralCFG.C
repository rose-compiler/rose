#include "sage3basic.h"
#include "CallGraph.h"
#include "interproceduralCFG.h"
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH


namespace StaticCFG
{

void InterproceduralCFG::buildFullCFG()
{
  all_nodes_.clear();
  clearNodesAndEdges();

  std::set<VirtualCFG::CFGNode> explored;
  graph_ = new SgIncidenceDirectedGraph;

  buildCFG<VirtualCFG::CFGNode, VirtualCFG::CFGEdge>
    (start_->cfgForBeginning(), all_nodes_, explored);
}

void InterproceduralCFG::buildFilteredCFG()
{
  assert(!"InterproceduralCFG:buildFilteredCFG() is unimplemented");
  all_nodes_.clear();
  clearNodesAndEdges();

  std::set<VirtualCFG::InterestingNode> explored;
  std::map<VirtualCFG::InterestingNode, SgGraphNode*> all_nodes;

  graph_ = new SgIncidenceDirectedGraph;

  buildCFG<VirtualCFG::InterestingNode, VirtualCFG::InterestingEdge>
    (VirtualCFG::makeInterestingCfg(start_), all_nodes, explored);

  typedef std::pair<VirtualCFG::InterestingNode, SgGraphNode*> pair_t;
  foreach (const pair_t& p, all_nodes)
    all_nodes_[VirtualCFG::CFGNode(p.first.getNode(), 0)] = p.second;
}

template <class NodeT, class EdgeT>
void InterproceduralCFG::buildCFG(NodeT n, std::map<NodeT, SgGraphNode*>& all_nodes, std::set<NodeT>& explored)
{
    SgNode* sgnode = n.getNode();
    ROSE_ASSERT(sgnode);

    if (explored.count(n) > 0)
        return;
    explored.insert(n);

    SgGraphNode* from = NULL;
    if (all_nodes.count(n) > 0)
    {
        from = all_nodes[n];
    }
    else
    {
        from = new SgGraphNode;
        from->set_SgNode(sgnode);
        from->addNewAttribute("info", new CFGNodeAttribute(n.getIndex(), graph_));
        all_nodes[n] = from;
        graph_->addNode(from);
    }

    std::vector<EdgeT> outEdges;
    unsigned int idx = n.getIndex();

    if (isSgFunctionCallExp(sgnode) &&
        idx == SGFUNCTIONCALLEXP_INTERPROCEDURAL_INDEX) {
      SgFunctionCallExp* fxnCall = isSgFunctionCallExp(sgnode);
      Rose_STL_Container<SgFunctionDefinition*> defs;
      CallTargetSet::getFunctionDefinitionsForCallLikeExp(fxnCall, defs);
      foreach (SgFunctionDefinition* def, defs) 
        makeEdge(CFGNode(fxnCall, idx), def->cfgForBeginning(), outEdges);
    }
    else if (isSgConstructorInitializer(sgnode) &&
        idx == SGCONSTRUCTORINITIALIZER_INTERPROCEDURAL_INDEX) {
      SgConstructorInitializer* ctorInit = isSgConstructorInitializer(sgnode);
      Rose_STL_Container<SgFunctionDefinition*> defs;
      CallTargetSet::getFunctionDefinitionsForCallLikeExp(ctorInit, defs);
      foreach (SgFunctionDefinition* def, defs) 
        makeEdge(CFGNode(ctorInit, idx), def->cfgForBeginning(), outEdges);
    }
    else if (isSgFunctionDefinition(sgnode) &&
        idx == SGFUNCTIONDEFINITION_INTERPROCEDURAL_INDEX) {
      SgFunctionDefinition* funDef = isSgFunctionDefinition(sgnode);
      SgGraphNode* funDefGraphNode = all_nodes[funDef->cfgForBeginning()];
      ROSE_ASSERT(funDefGraphNode != NULL);
      std::set<SgDirectedGraphEdge*> sgEdges = graph_->computeEdgeSetIn(funDefGraphNode);
      foreach (SgDirectedGraphEdge* edge, sgEdges) {
        SgGraphNode* sourceGN = edge->get_from();
        SgNode* source = sourceGN->get_SgNode();

        // Determine the index to which the interprocedural edge returns. TODO make member function of SgNode ?
        unsigned int index;
        if (source->variantT() == V_SgConstructorInitializer)
          index = SGCONSTRUCTORINITIALIZER_INTERPROCEDURAL_INDEX + 1;
        else if (source->variantT() == V_SgFunctionCallExp)
          index = SGFUNCTIONCALLEXP_INTERPROCEDURAL_INDEX + 1;
        else
          ROSE_ASSERT(!"Error: unable to determine interprocedural return index");

        makeEdge(CFGNode(funDef, idx), CFGNode(source, index), outEdges);
      }
    }
    else {
      outEdges = n.outEdges();
    }

    if (outEdges.size() < 1) {
      std::cerr << "warning: couldn't find edges from " << n.getNode()->class_name() << 
        " on line: " << n.getNode()->get_file_info()->get_line() << std::endl;
      outEdges = n.outEdges();
    }

    foreach (const EdgeT& edge, outEdges)
    {
        NodeT tar = edge.target();

        SgGraphNode* to = NULL;
        if (all_nodes.count(tar) > 0)
            to = all_nodes[tar];
        else
        {
            to = new SgGraphNode;
            to->set_SgNode(tar.getNode());
            to->addNewAttribute("info", new CFGNodeAttribute(tar.getIndex(), graph_));
            all_nodes[tar] = to;
            graph_->addNode(to);
        }

        SgDirectedGraphEdge* new_edge = new SgDirectedGraphEdge(from, to);
        new_edge->addNewAttribute("info", new CFGEdgeAttribute<EdgeT>(edge));
        graph_->addDirectedEdge(new_edge);
    }

    foreach (const EdgeT& edge, outEdges)
    {
        ROSE_ASSERT(edge.source() == n);
        buildCFG<NodeT, EdgeT>(edge.target(), all_nodes, explored);
    }
}

} // end of namespace StaticCFG
