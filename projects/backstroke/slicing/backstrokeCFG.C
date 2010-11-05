#include "backstrokeCFG.h"
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/dominator_tree.hpp>

namespace Backstroke
{

using namespace std;
using namespace boost;
#define foreach BOOST_FOREACH


void CFG::toDot(const std::string& filename)
{
    std::ofstream ofile(filename.c_str(), std::ios::out);
    write_graphviz(ofile, *this,
			bind(&CFG::writeGraphNode, this, _1, _2),
			bind(&CFG::writeGraphEdge, this, _1, _2));
}

void CFG::build(SgFunctionDefinition* funcDef)
{
	ROSE_ASSERT(funcDef);
	
	clear();
	map<CFGNode, Vertex> nodesAdded;
	set<CFGNode> nodesProcessed;
	buildCFG(funcDef->cfgForBeginning(), nodesAdded, nodesProcessed);
	setEntryAndExit();

	ROSE_ASSERT(isSgFunctionDefinition((*this)[entry_].getNode()));
	ROSE_ASSERT(isSgFunctionDefinition((*this)[exit_].getNode()));
}

void CFG::setEntryAndExit()
{
	vertex_iterator i, j;
	for (tie(i, j) = vertices(*this); i != j; ++i)
	{
		CFGNode node = (*this)[*i];
		if (isSgFunctionDefinition(node.getNode()))
		{
			if (node.getIndex() == 0)
				entry_ = *i;
			else if (node.getIndex() == 3)
				exit_ = *i;
		}
	}
}

void CFG::buildCFG(const CFGNode& node, map<CFGNode, Vertex>& nodesAdded, set<CFGNode>& nodesProcessed)
{
    ROSE_ASSERT(node.getNode());

    if (nodesProcessed.count(node) > 0)
        return;
	nodesProcessed.insert(node);

    map<CFGNode, Vertex>::iterator iter;
    bool inserted;
    Vertex from, to;

    // Add the source node.
    const CFGNode& src = node;
	ROSE_ASSERT(src.getNode());

    tie(iter, inserted) = nodesAdded.insert(make_pair(src, Vertex()));

    if (inserted)
    {
        from = add_vertex(*this);
        (*this)[from] = src;
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

        tie(iter, inserted) = nodesAdded.insert(make_pair(tar, Vertex()));

        if (inserted)
        {
            to = add_vertex(*this);
            (*this)[to] = tar;
            iter->second = to;
        }
        else
            to = iter->second;

        // Add the edge.
        add_edge(from, to, *this);

        // Build the CFG recursively.
        buildCFG(tar, nodesAdded, nodesProcessed);
    }
}

CFG::DomTreePredMap CFG::buildDominatorTree() const
{
	DomTreePredMap immediateDominators;
	associative_property_map<DomTreePredMap> domTreePredMap(immediateDominators);

	// Here we use the algorithm in boost::graph to build an map from each node to its immediate dominator.
	lengauer_tarjan_dominator_tree(*this, entry_, domTreePredMap);
	return immediateDominators;
}

void CFG::writeGraphNode(std::ostream& out, const Vertex& node)
{
	writeCFGNode(out, (*this)[node]);
}

void CFG::writeGraphEdge(std::ostream& out, const Edge& edge)
{
	CFGEdge e((*this)[source(edge, *this)], (*this)[target(edge, *this)]);
	writeCFGEdge(out, e);
}

void writeCFGNode(std::ostream& out, const CFGNode& n)
{
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

void writeCFGEdge(std::ostream& out, const CFGEdge& e)
{
	out << "[label=\"" << escapeString(e.toString()) <<
		"\", style=\"" << "solid" << "\"]";
}

} // End of namespace Backstroke
