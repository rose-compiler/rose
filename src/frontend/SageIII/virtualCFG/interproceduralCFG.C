#include "sage3basic.h"
#include "CallGraph.h"
#include "interproceduralCFG.h"
#include <boost/foreach.hpp>
#include <vector>

#define foreach BOOST_FOREACH


namespace StaticCFG
{

void addEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result) {
  // Makes a CFG edge, adding appropriate labels
  SgNode* fromNode = from.getNode();
  unsigned int fromIndex = from.getIndex();
  SgNode* toNode = to.getNode();

  // Exit early if the edge should not exist because of a control flow discontinuity
  if (fromIndex == 1 && (isSgGotoStatement(fromNode) || isSgBreakStmt(fromNode) || isSgContinueStmt(fromNode))) {
    return;
  }
  if (isSgReturnStmt(fromNode) && toNode == fromNode->get_parent()) {
    SgReturnStmt* rs = isSgReturnStmt(fromNode);
    if (fromIndex == 1 || (fromIndex == 0 && !rs->get_expression())) return;
  }
  if (isSgStopOrPauseStatement(fromNode) && toNode == fromNode->get_parent()) {
    SgStopOrPauseStatement* sps = isSgStopOrPauseStatement(fromNode);
    if (fromIndex == 0 && sps->get_stop_or_pause() == SgStopOrPauseStatement::e_stop) return;
  }
  if (fromIndex == 1 && isSgSwitchStatement(fromNode) &&
      isSgSwitchStatement(fromNode)->get_body() == toNode) return;

  // Create the edge
  result.push_back(CFGEdge(from, to));
}

void InterproceduralCFG::buildFullCFG()
{
  std::set<VirtualCFG::CFGNode> explored;
  graph_ = new SgIncidenceDirectedGraph;
  ClassHierarchyWrapper classHierarchy(SageInterface::getProject());

  buildCFG(start_->cfgForBeginning(), all_nodes_, explored, &classHierarchy);
}

void InterproceduralCFG::buildFilteredCFG()
{
  ROSE_ASSERT(!"InterproceduralCFG:buildFilteredCFG() is unimplemented");
}

void InterproceduralCFG::buildCFG(CFGNode n, 
                                  std::map<CFGNode, SgGraphNode*>& all_nodes, 
                                  std::set<CFGNode>& explored,
                                  ClassHierarchyWrapper* classHierarchy)
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

    std::vector<CFGEdge> outEdges;
    unsigned int idx = n.getIndex();

    if ((isSgFunctionCallExp(sgnode) &&
         idx == SGFUNCTIONCALLEXP_INTERPROCEDURAL_INDEX) ||
        (isSgConstructorInitializer(sgnode) &&
         idx == SGCONSTRUCTORINITIALIZER_INTERPROCEDURAL_INDEX)) {
          ROSE_ASSERT( isSgExpression(sgnode) );
          Rose_STL_Container<SgFunctionDefinition*> defs;
          CallTargetSet::getDefinitionsForExpression(isSgExpression(sgnode), classHierarchy, defs); 
          foreach (SgFunctionDefinition* def, defs) {
            addEdge(n, def->cfgForBeginning(), outEdges);
            addEdge(def->cfgForEnd(), CFGNode(sgnode, idx+1), outEdges);
          }
    }
    else {
      outEdges = n.outEdges();
    }

    if (outEdges.size() < 1) {
      outEdges = n.outEdges();
    }

    std::set<CFGNode> targets; 
    foreach (const CFGEdge& edge, outEdges)
    {
        CFGNode tar = edge.target();
        targets.insert(tar);

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
        new_edge->addNewAttribute("info", new CFGEdgeAttribute<CFGEdge>(edge));
        graph_->addDirectedEdge(new_edge);
    }

    foreach (const CFGNode& target, targets)
    {
        buildCFG(target, all_nodes, explored, classHierarchy);
    }
}

} // end of namespace StaticCFG
