#include "sage3basic.h"
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
    switch (sgnode->variantT()) {
      case V_SgFunctionCallExp: {
        SgFunctionCallExp* fxnCall = isSgFunctionCallExp(sgnode);
        unsigned int idx = n.getIndex();
        if (fxnCall && idx == SGFUNCTIONCALLEXP_INTERPROCEDURAL_INDEX) {
          SgFunctionDeclaration* fxnDecl = fxnCall->getAssociatedFunctionDeclaration(); 
          std::cerr << "found fxn call: " << fxnDecl->get_qualified_name().str() << std::endl;
          break;
        }
      } 
      case V_SgConstructorInitializer: {
        std::cerr << "found ctor init" << std::endl;
      }
      default: {
        outEdges = n.outEdges();
      }
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
