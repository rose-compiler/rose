#include "customFilteredCFG.h"
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

namespace StaticCFG
{
    template<typename _filter>
    void CustomFilteredCFG<_filter>::buildFilteredCFG()  {
        all_nodes_.clear();
        clearNodesAndEdges();
        std::set< VirtualCFG::FilteredCFGNode<_filter> > explored;
        std::map<VirtualCFG::FilteredCFGNode<_filter>, SgGraphNode*> all_nodes;

        graph_ = new SgIncidenceDirectedGraph;

        
        if (SgProject* project = isSgProject(start_))
        {
            Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
            for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
            {
                SgFunctionDefinition* proc = isSgFunctionDefinition(*i);
                if (proc)
                {
                    buildTemplatedCFG<VirtualCFG::FilteredCFGNode<_filter> , VirtualCFG::FilteredCFGEdge<_filter> >
                        (proc->cfgForBeginning(), all_nodes, explored);
                }
            }
        }
        else
            buildTemplatedCFG<VirtualCFG::FilteredCFGNode<_filter> , VirtualCFG::FilteredCFGEdge<_filter> >
                (start_->cfgForBeginning(), all_nodes, explored);

        typedef std::pair<VirtualCFG::FilteredCFGNode<_filter>, SgGraphNode*> pair_t;
        foreach (const pair_t& p, all_nodes)
            all_nodes_[VirtualCFG::CFGNode(p.first.getNode(), 0)] = p.second; 
    }

template<typename _filter>    
template <class NodeT, class EdgeT>
void CustomFilteredCFG<_filter>::buildTemplatedCFG(NodeT n, std::map<NodeT, SgGraphNode*>& all_nodes, std::set<NodeT>& explored)
{
    ROSE_ASSERT(n.getNode());

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
        from->set_SgNode(n.getNode());
        unsigned int index = n.getIndex();
        from->addNewAttribute("info", new CFGNodeAttribute(index, graph_));
        all_nodes[n] = from;
        graph_->addNode(from);

        // Here we check if the new node is the entry or exit.
        if (isSgFunctionDefinition(n.getNode()))
        {
            if (index == 0)
                entry_ = from;
            else if (index == 3)
                exit_ = from;
        }
    }

    std::vector<EdgeT> outEdges = n.outEdges();
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
            unsigned int index = tar.getIndex();
            to->addNewAttribute("info", new CFGNodeAttribute(index, graph_));
            all_nodes[tar] = to;
            graph_->addNode(to);

            // Here we check if the new node is the entry or exit.
            if (isSgFunctionDefinition(tar.getNode()))
            {
                if (index == 0)
                    entry_ = to;
                else if (index == 3)
                    exit_ = to;
            }
        }

        SgDirectedGraphEdge* new_edge = new SgDirectedGraphEdge(from, to);
        new_edge->addNewAttribute("info", new CFGEdgeAttribute<EdgeT>(edge));
        graph_->addDirectedEdge(new_edge);
    }

    foreach (const EdgeT& edge, outEdges)
    {
        ROSE_ASSERT(edge.source() == n);
        buildTemplatedCFG<NodeT, EdgeT>(edge.target(), all_nodes, explored);
    }

    std::vector<EdgeT> inEdges = n.inEdges();
    foreach (const EdgeT& edge, inEdges)
    {
        ROSE_ASSERT(edge.target() == n);
        buildTemplatedCFG<NodeT, EdgeT>(edge.source(), all_nodes, explored);
    }
}

    
    
}
