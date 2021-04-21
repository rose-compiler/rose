#include "staticCFG.h"
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH


namespace StaticCFG
{

  // DQ (1/8/2018): Moved here from header file.
  // CFG(SgNode* node, bool is_filtered = false)
CFG::CFG(SgNode* node, bool is_filtered)
   : graph_(NULL), start_(node), entry_(NULL), exit_(NULL), is_filtered_(is_filtered)
   { 
#if 0
     printf ("Inside of CFG(SgNode.bool) constructor \n");
#endif

     buildCFG();

#if 0
     printf ("Leaving CFG(SgNode.bool) constructor \n");
#endif
   }

void CFG::clearNodesAndEdges()
{
    if (graph_ != NULL)
    {
        foreach (SgGraphNode* node, graph_->computeNodeSet())
        {
            foreach (SgDirectedGraphEdge* edge, graph_->computeEdgeSetOut(node))
            {
                // This would be so much simpler if the attribute used container-owns-attribute paradigm. In any case, we
                // cannot delete the attribute without first removing it from the container because the container needs to be
                // able to figure out how the attribute wants to have its memory managed, and it does so by asking it.
                AstAttribute *edgeAttr = edge->getAttribute("info");
                delete edge;
                delete edgeAttr;
            }
            AstAttribute *nodeAttr = node->getAttribute("info");
            delete node;
            delete nodeAttr;
        }
        delete graph_;
    }
    graph_ = NULL;
    entry_ = NULL;
    exit_ = NULL;
}

int CFG::getIndex(SgGraphNode* node)
{
    CFGNodeAttribute* info = dynamic_cast<CFGNodeAttribute*>(node->getAttribute("info"));
    ROSE_ASSERT(info);
    return info->getIndex();
}

void CFG::cfgToDot(SgNode* node, const std::string& file_name)
{
    std::ofstream ofile(file_name.c_str(), std::ios::out);
    ofile << "digraph defaultName {\n";
    std::set<SgGraphNode*> explored;
    processNodes(ofile, cfgForBeginning(node), explored);
    ofile << "}\n";
}

void CFG::buildFullCFG()
{
    // Before building a new CFG, make sure to clear all nodes built before.
    all_nodes_.clear();
    clearNodesAndEdges();

    std::set<VirtualCFG::CFGNode> explored;

    graph_ = new SgIncidenceDirectedGraph;

    if (SgProject* project = isSgProject(start_))
    {
        Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
        for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
        {
            SgFunctionDefinition* proc = isSgFunctionDefinition(*i);
            if (proc)
            {
                buildCFG<VirtualCFG::CFGNode, VirtualCFG::CFGEdge>
                    (proc->cfgForBeginning(), all_nodes_, explored);
            }
        }
    }
    else
        buildCFG<VirtualCFG::CFGNode, VirtualCFG::CFGEdge>
            (start_->cfgForBeginning(), all_nodes_, explored);
}

void CFG::buildFilteredCFG()
{
    all_nodes_.clear();
    clearNodesAndEdges();

    std::set<VirtualCFG::InterestingNode> explored;
    std::map<VirtualCFG::InterestingNode, SgGraphNode*> all_nodes;

    graph_ = new SgIncidenceDirectedGraph;

    if (SgProject* project = isSgProject(start_))
    {
        Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
        for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
        {
            SgFunctionDefinition* proc = isSgFunctionDefinition(*i);
            if (proc)
            {
                buildCFG<VirtualCFG::InterestingNode, VirtualCFG::InterestingEdge>
                    (VirtualCFG::makeInterestingCfg(proc), all_nodes, explored);
            }
        }
    }
    else
        buildCFG<VirtualCFG::InterestingNode, VirtualCFG::InterestingEdge>
            (VirtualCFG::makeInterestingCfg(start_), all_nodes, explored);

    typedef std::pair<VirtualCFG::InterestingNode, SgGraphNode*> pair_t;
    foreach (const pair_t& p, all_nodes)
        all_nodes_[VirtualCFG::CFGNode(p.first.getNode(), 0)] = p.second;
}

#if 0
void CFG::buildCFG(CFGNode n)
{
    ROSE_ASSERT(n.getNode());

    if (explored_.count(n) > 0)
        return;
    explored_.insert(n);

    SgGraphNode* from = NULL;
    if (all_nodes_.count(n) > 0)
    {
        from = all_nodes_[n];
    }
    else
    {
        from = new SgGraphNode;
        from->set_SgNode(n.getNode());
        from->addNewAttribute("info", new CFGNodeAttribute(n.getIndex(), graph_));
        all_nodes_[n] = from;
        graph_->addNode(from);
    }

    std::vector<VirtualCFG::CFGEdge> outEdges = n.outEdges();
    foreach (const VirtualCFG::CFGEdge& edge, outEdges)
    {
        CFGNode tar = edge.target();

        SgGraphNode* to = NULL;
        if (all_nodes_.count(tar) > 0)
            to = all_nodes_[tar];
        else
        {
            to = new SgGraphNode;
            to->set_SgNode(tar.getNode());
            to->addNewAttribute("info", new CFGNodeAttribute(tar.getIndex(), graph_));
            all_nodes_[tar] = to;
            graph_->addNode(to);
        }

        graph_->addDirectedEdge(new SgDirectedGraphEdge(from, to));
    }

    foreach (const VirtualCFG::CFGEdge& edge, outEdges)
    {
        ROSE_ASSERT(edge.source() == n);
        buildCFG(edge.target());
    }

#if 1
    std::vector<CFGEdge> inEdges = n.inEdges();
    foreach (const VirtualCFG::CFGEdge& edge, inEdges)
    {
        ROSE_ASSERT(edge.target() == n);
        buildCFG(edge.source());
    }
#endif
}
#endif

template <class NodeT, class EdgeT>
void CFG::buildCFG(NodeT n, std::map<NodeT, SgGraphNode*>& all_nodes, std::set<NodeT>& explored)
   {
#if 0
    printf ("Inside of CFG::buildCFG() \n");
#endif

    ROSE_ASSERT(n.getNode());

    if (explored.count(n) > 0)
        return;
    explored.insert(n);

#if 0
    printf ("In CFG::buildCFG(): test 1 \n");
#endif

    SgGraphNode* from = NULL;
    if (all_nodes.count(n) > 0)
    {
        from = all_nodes[n];
    }
    else
    {
#if 0
         printf ("In CFG::buildCFG(): test 2 \n");
#endif
        from = new SgGraphNode;
        from->set_SgNode(n.getNode());
        unsigned int index = n.getIndex();
        from->addNewAttribute("info", new CFGNodeAttribute(index, graph_));
        all_nodes[n] = from;
        graph_->addNode(from);
#if 0
         printf ("In CFG::buildCFG(): test 3 \n");
#endif
        // Here we check if the new node is the entry or exit.
        if (isSgFunctionDefinition(n.getNode()))
        {
            if (index == 0)
                entry_ = from;
            else if (index == 3)
                exit_ = from;
        }
    }

#if 0
    printf ("In CFG::buildCFG(): loop 1: n = %p = %s \n",n.getNode(),n.getNode()->class_name().c_str());
#endif

    std::vector<EdgeT> outEdges = n.outEdges();

#if 0
    printf ("In CFG::buildCFG(): loop 1.1 \n");
#endif

    foreach (const EdgeT& edge, outEdges)
    {
        NodeT tar = edge.target();
#if 0
        printf ("In CFG::buildCFG(): loop 1: A \n");
#endif

        SgGraphNode* to = NULL;
        if (all_nodes.count(tar) > 0)
            to = all_nodes[tar];
        else
        {
#if 0
            printf ("In CFG::buildCFG(): loop 1: B \n");
#endif
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
#if 0
            printf ("In CFG::buildCFG(): loop 1: C \n");
#endif
        }

#if 0
        printf ("In CFG::buildCFG(): loop 1: D \n");
#endif

        SgDirectedGraphEdge* new_edge = new SgDirectedGraphEdge(from, to);
        new_edge->addNewAttribute("info", new CFGEdgeAttribute<EdgeT>(edge));
        graph_->addDirectedEdge(new_edge);

#if 0
        printf ("In CFG::buildCFG(): loop 1: E \n");
#endif
    }

#if 0
    printf ("In CFG::buildCFG(): loop 2 \n");
#endif

    foreach (const EdgeT& edge, outEdges)
    {
        ROSE_ASSERT(edge.source() == n);
        buildCFG<NodeT, EdgeT>(edge.target(), all_nodes, explored);
    }

#if 0
    printf ("In CFG::buildCFG(): loop 3 \n");
#endif

    std::vector<EdgeT> inEdges = n.inEdges();
    foreach (const EdgeT& edge, inEdges)
    {
        ROSE_ASSERT(edge.target() == n);
        buildCFG<NodeT, EdgeT>(edge.source(), all_nodes, explored);
    }

#if 0
    printf ("Leaving CFG::buildCFG() \n");
#endif
}

VirtualCFG::CFGNode CFG::toCFGNode(SgGraphNode* node)
{
    return VirtualCFG::CFGNode(node->get_SgNode(), getIndex(node));
}

SgGraphNode* CFG::cfgForBeginning(SgNode* node)
{
    ROSE_ASSERT(all_nodes_.count(node->cfgForBeginning()));
    return all_nodes_[node->cfgForBeginning()];
}
SgGraphNode* CFG::cfgForEnd(SgNode* node)
{
    ROSE_ASSERT(all_nodes_.count(node->cfgForEnd()));
    return all_nodes_[node->cfgForEnd()];
}

#if 0
std::vector<SgDirectedGraphEdge*> CFG::getOutEdges(SgNode* node, int index)
{
    VirtualCFG::CFGNode n(node, index);
    std::set<SgDirectedGraphEdge*> edges = graph_->computeEdgeSetOut(all_nodes_[n]);
    return std::vector<SgDirectedGraphEdge*>(edges.begin(), edges.end());
}

std::vector<SgDirectedGraphEdge*> CFG::getInEdges(SgNode* node, int index)
{
    VirtualCFG::CFGNode n(node, index);
    std::set<SgDirectedGraphEdge*> edges = graph_->computeEdgeSetIn(all_nodes_[n]);
    return std::vector<SgDirectedGraphEdge*>(edges.begin(), edges.end());
}
#endif

std::vector<SgDirectedGraphEdge*> CFG::getOutEdges(SgGraphNode* node)
{
    std::set<SgDirectedGraphEdge*> edges = graph_->computeEdgeSetOut(node);
    return std::vector<SgDirectedGraphEdge*>(edges.begin(), edges.end());
}

std::vector<SgDirectedGraphEdge*> CFG::getInEdges(SgGraphNode* node)
{
    std::set<SgDirectedGraphEdge*> edges = graph_->computeEdgeSetIn(node);
    return std::vector<SgDirectedGraphEdge*>(edges.begin(), edges.end());
}

void CFG::processNodes(std::ostream & o, SgGraphNode* n, std::set<SgGraphNode*>& explored)
{
    if (explored.count(n) > 0)
        return;
    explored.insert(n);

    printNodePlusEdges(o, n);

    std::set<SgDirectedGraphEdge*> out_edges = graph_->computeEdgeSetOut(n);
    foreach (SgDirectedGraphEdge* e, out_edges)
        processNodes(o, e->get_to(), explored);

    std::set<SgDirectedGraphEdge*> in_edges = graph_->computeEdgeSetIn(n);
    foreach (SgDirectedGraphEdge* e, in_edges)
        processNodes(o, e->get_from(), explored);
}

void CFG::printNodePlusEdges(std::ostream & o, SgGraphNode* node)
{
    printNode(o, node);

    std::set<SgDirectedGraphEdge*> out_edges = graph_->computeEdgeSetOut(node);
    foreach (SgDirectedGraphEdge* e, out_edges)
        printEdge(o, e, false);

#ifdef DEBUG
    std::set<SgDirectedGraphEdge*> in_edges = graph_->computeEdgeSetIn(node);
    foreach (SgDirectedGraphEdge* e, in_edges)
        printEdge(o, e, true);
#endif
}

void CFG::printNode(std::ostream & o, SgGraphNode* node)
{
    CFGNode n = toCFGNode(node);

    std::string id = n.id();
    std::string nodeColor = "black";

    if (isSgStatement(n.getNode()))
        nodeColor = "blue";
    else if (isSgExpression(n.getNode()))
        nodeColor = "green";
    else if (isSgInitializedName(n.getNode()))
        nodeColor = "red";

    o << id << " [label=\""  << escapeString(n.toString()) << "\", color=\"" << nodeColor <<
        "\", style=\"" << (n.isInteresting()? "solid" : "dotted") << "\"];\n";
}

void CFG::printEdge(std::ostream & o, SgDirectedGraphEdge* edge, bool isInEdge)
{
    // Note that CFGEdge will do some checks which forbids us to use it to represent an InterestingEdge. 
    AstAttribute* attr = edge->getAttribute("info");
    if (CFGEdgeAttribute<CFGEdge>* edge_attr = dynamic_cast<CFGEdgeAttribute<CFGEdge>*>(attr))
    {
        CFGEdge e = edge_attr->getEdge();
        o << e.source().id() << " -> " << e.target().id() << " [label=\"" << escapeString(e.toString()) <<
            "\", style=\"" << (isInEdge ? "dotted" : "solid") << "\"];\n";
    }
    else if (CFGEdgeAttribute<InterestingEdge>* edge_attr = dynamic_cast<CFGEdgeAttribute<InterestingEdge>*>(attr))
    {
        InterestingEdge e = edge_attr->getEdge();
        o << e.source().id() << " -> " << e.target().id() << " [label=\"" << escapeString(e.toString()) <<
            "\", style=\"" << (isInEdge ? "dotted" : "solid") << "\"];\n";
    }
    else
        ROSE_ABORT();
}


std::vector<SgDirectedGraphEdge*> outEdges(SgGraphNode* node)
{
#if 0
    printf ("In StaticCFG::outEdges() \n");
#endif

    CFGNodeAttribute* info = dynamic_cast<CFGNodeAttribute*>(node->getAttribute("info"));
    ROSE_ASSERT(info);
    std::set<SgDirectedGraphEdge*> edges = info->getGraph()->computeEdgeSetOut(node);

#if 0
    printf ("Leaving StaticCFG::outEdges() \n");
#endif

    return std::vector<SgDirectedGraphEdge*>(edges.begin(), edges.end());
}

std::vector<SgDirectedGraphEdge*> inEdges(SgGraphNode* node)
{
    CFGNodeAttribute* info = dynamic_cast<CFGNodeAttribute*>(node->getAttribute("info"));
    ROSE_ASSERT(info);
    std::set<SgDirectedGraphEdge*> edges = info->getGraph()->computeEdgeSetIn(node);
    return std::vector<SgDirectedGraphEdge*>(edges.begin(), edges.end());
}

int getIndex(SgGraphNode* node)
{
    CFGNodeAttribute* info = dynamic_cast<CFGNodeAttribute*>(node->getAttribute("info"));
    ROSE_ASSERT(info);
    return info->getIndex();
}


} // end of namespace StaticCFG
