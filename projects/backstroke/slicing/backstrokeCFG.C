#include "backstrokeCFG.h"
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/graph/graphviz.hpp>

namespace Backstroke
{

using namespace std;
using namespace boost;
#define foreach BOOST_FOREACH


void CFG::toDot(const std::string& filename)
{
    std::ofstream ofile(filename.c_str(), std::ios::out);
    write_graphviz(ofile, graph_, 
			bind(&CFG::writeGraphNode, this, _1, _2),
			bind(&CFG::writeGraphEdge, this, _1, _2));
}

void CFG::build(SgFunctionDefinition* func_def)
{
	ROSE_ASSERT(func_def);
	map<CFGNode, GraphNode> nodes_added;
	set<CFGNode> nodes_processed;
	buildCFG(func_def->cfgForBeginning(), nodes_added, nodes_processed);
}

void CFG::buildCFG(const CFGNode& node, map<CFGNode, GraphNode>& nodes_added, set<CFGNode>& nodes_processed)
{
    ROSE_ASSERT(node.getNode());

    if (nodes_processed.count(node) > 0)
        return;
	nodes_processed.insert(node);

    map<CFGNode, GraphNode>::iterator iter;
    bool inserted;
    GraphNode from, to;

    // Add the source node.
    const CFGNode& src = node;
	ROSE_ASSERT(src.getNode());

    tie(iter, inserted) = nodes_added.insert(make_pair(src, GraphNode()));

    if (inserted)
    {
        from = add_vertex(graph_);
        graph_[from] = src;
        iter->second = from;
    }
    else
        from = iter->second;

    vector<CFGEdge> outEdges = node.outEdges();
    foreach (const CFGEdge& edge, outEdges)
    {
        // For each out edge, add the target node.
        CFGNode tar = edge.target();
		ROSE_ASSERT(tar.getNode());

        tie(iter, inserted) = nodes_added.insert(make_pair(tar, GraphNode()));

        if (inserted)
        {
            to = add_vertex(graph_);
            graph_[to] = tar;
            iter->second = to;
        }
        else
            to = iter->second;

        // Add the edge.
        add_edge(from, to, graph_);

        // Build the CFG recursively.
        buildCFG(tar, nodes_added, nodes_processed);
    }
}

void CFG::writeGraphNode(std::ostream& out, const GraphNode& node)
{
	CFGNode n = graph_[node];
	ROSE_ASSERT(n.getNode());

	std::string nodeColor = "black";
	if (isSgStatement(n.getNode()))
		nodeColor = "blue";
	else if (isSgExpression(n.getNode()))
		nodeColor = "green";
	else if (isSgInitializedName(n.getNode()))
		nodeColor = "red";

	out << "[label=\""  << escapeString(n.toString()) << "\", color=\"" << nodeColor <<
		"\", style=\"" << (n.isInteresting()? "solid" : "dotted") << "\"]";
}

void CFG::writeGraphEdge(std::ostream& out, const GraphEdge& edge)
{
	CFGEdge e(graph_[source(edge, graph_)], graph_[target(edge, graph_)]);
	out << "[label=\"" << escapeString(e.toString()) <<
		"\", style=\"" << "solid" << "\"]";
}

} // End of namespace Backstroke
