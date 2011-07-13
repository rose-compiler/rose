#include "newCFG.h"

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/dominator_tree.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/transpose_graph.hpp>
#include <boost/tuple/tuple.hpp>


namespace Backstroke
{


#define foreach BOOST_FOREACH

void ControlFlowGraph::toDot(const std::string& filename) const
{
	std::ofstream ofile(filename.c_str(), std::ios::out);
	boost::write_graphviz(ofile, *this,
			boost::bind(&ControlFlowGraph::writeGraphNode, this, ::_1, ::_2),
			boost::bind(&ControlFlowGraph::writeGraphEdge, this, ::_1, ::_2));
}

void ControlFlowGraph::build(SgFunctionDefinition* funcDef)
{
	ROSE_ASSERT(funcDef);
	funcDef_ = funcDef;

	// The following two variables are used to record the nodes traversed.
	nodesToVertices_.clear();
	std::set<CFGNode> nodesProcessed;

	// Remove all nodes and edges first.
	clear();
	entry_ = GraphTraits::null_vertex();
	exit_ = GraphTraits::null_vertex();

    CFGNode entry(funcDef->cfgForBeginning());
	buildCFG(entry, nodesToVertices_, nodesProcessed);

	// Find the entry and exit of this CFG.
	setEntryAndExit();

	ROSE_ASSERT(isSgFunctionDefinition((*this)[entry_]->getNode()));
	ROSE_ASSERT(isSgFunctionDefinition((*this)[exit_]->getNode()));
}

void ControlFlowGraph::setEntryAndExit()
{
    foreach (Vertex v, boost::vertices(*this))
	{
		CFGNodePtr node = (*this)[v];
		if (isSgFunctionDefinition(node->getNode()))
		{
			if (node->getIndex() == 0)
				entry_ = v;
			else if (node->getIndex() == 3)
				exit_ = v;
		}
	}

	//In graphs with an infinite loop, we might never get to the end vertex
	//In those cases, we need to add it explicitly
	if (exit_ == GraphTraits::null_vertex())
	{
		std::cerr << "This function may contain an infinite loop "
				"inside so that its CFG cannot be built" << std::endl;
		exit_ = add_vertex(*this);
		(*this)[exit_] = newCFGNode(CFGNode(funcDef_->cfgForEnd()));
	}

	ROSE_ASSERT(entry_ != GraphTraits::null_vertex());
	ROSE_ASSERT(exit_ != GraphTraits::null_vertex());
}

void ControlFlowGraph::buildCFG(
		const CFGNode& node,
		std::map<CFGNode, Vertex>& nodesAdded,
		std::set<CFGNode>& nodesProcessed)
{
	ROSE_ASSERT(node.getNode());

	if (nodesProcessed.count(node) > 0)
		return;
	nodesProcessed.insert(node);

	std::map<CFGNode, Vertex>::iterator iter;
	bool inserted;
	Vertex from, to;

	// Add the source node.
	const CFGNode& src = node;
	ROSE_ASSERT(src.getNode());

	boost::tie(iter, inserted) = nodesAdded.insert(std::make_pair(src, Vertex()));

	if (inserted)
	{
		from = add_vertex(*this);
		(*this)[from] = newCFGNode(src);
		iter->second = from;
	}
	else
	{
		from = iter->second;
	}

    CFGNode nodeCopy = node;
    nodeCopy.setFilter(filter_);
	std::vector<CFGEdge> outEdges = nodeCopy.outEdges();

	foreach (const CFGEdge& cfgEdge, outEdges)
	{
		// For each out edge, add the target node.
		CFGNode tar = cfgEdge.target();
		ROSE_ASSERT(tar.getNode());

		boost::tie(iter, inserted) = nodesAdded.insert(std::make_pair(tar, Vertex()));

		if (inserted)
		{
			to = add_vertex(*this);
			(*this)[to] = newCFGNode(tar);
			iter->second = to;
		}
		else
		{
			to = iter->second;
		}

		// Add the edge.
		Edge edge = add_edge(from, to, *this).first;
		(*this)[edge] = newCFGEdge(cfgEdge);

		// Build the CFG recursively.
		buildCFG(tar, nodesAdded, nodesProcessed);
	}
}

const ControlFlowGraph::VertexVertexMap& ControlFlowGraph::getDominatorTree() const
{
    if (!dominatorTree_.empty())
        return dominatorTree_;

	boost::associative_property_map<VertexVertexMap> domTreePredMap(dominatorTree_);

	// Here we use the algorithm in boost::graph to build a map from each node to its immediate dominator.
	boost::lengauer_tarjan_dominator_tree(*this, entry_, domTreePredMap);
	return dominatorTree_;
}

const ControlFlowGraph::VertexVertexMap& 
ControlFlowGraph::getPostdominatorTree() const
{
    if (!postdominatorTree_.empty())
        return postdominatorTree_;
    
	boost::associative_property_map<VertexVertexMap> postdomTreePredMap(postdominatorTree_);

	// Here we use the algorithm in boost::graph to build an map from each node to its immediate dominator.
	boost::lengauer_tarjan_dominator_tree(boost::make_reverse_graph(*this), exit_, postdomTreePredMap);
	return postdominatorTree_;
}

ControlFlowGraph ControlFlowGraph::makeReverseCopy() const
{
	ControlFlowGraph reverseCFG;
	// The following function makes a reverse CFG copy.
	boost::transpose_graph(*this, reverseCFG, 
		boost::vertex_copy(VertexCopier(*this, reverseCFG)).
		edge_copy(EdgeCopier(*this, reverseCFG)));

	// Swap entry and exit.
	reverseCFG.entry_ = this->exit_;
	reverseCFG.exit_ = this->entry_;
	return reverseCFG;
}

std::vector<CFGNodePtr> ControlFlowGraph::getAllNodes() const
{
	std::vector<CFGNodePtr> allNodes;
    foreach (Vertex v, boost::vertices(*this))
		allNodes.push_back((*this)[v]);
	return allNodes;
}

std::vector<CFGEdgePtr> ControlFlowGraph::getAllEdges() const
{
	std::vector<CFGEdgePtr> allEdges;
    foreach (const Edge& e, boost::edges(*this))
		allEdges.push_back((*this)[e]);
	return allEdges;
}

ControlFlowGraph::Vertex ControlFlowGraph::getVertexForNode(const CFGNode &node) const
{
	std::map<CFGNode, Vertex>::const_iterator vertexIter = nodesToVertices_.find(node);
	if (vertexIter == nodesToVertices_.end())
		return GraphTraits::null_vertex();
	else
	{
		ROSE_ASSERT(*(*this)[vertexIter->second] == node);
		return vertexIter->second;
	}
}

std::set<ControlFlowGraph::Edge> 
ControlFlowGraph::getAllBackEdges() const
{
    std::set<Edge> backEdges;

    // If the dominator tree is not built yet, build it now.
    getDominatorTree();

    foreach (const Edge& e, boost::edges(*this))
    {
        Vertex src = boost::source(e, *this);
        Vertex tar = boost::target(e, *this);

        //Vertex v = *(dominatorTree.find(src));
        VertexVertexMap::const_iterator iter = dominatorTree_.find(src);
        while (iter != dominatorTree_.end())
        {
            if (iter->second == tar)
            {
                backEdges.insert(e);
                break; // break the while loop
            }
            iter = dominatorTree_.find(iter->second);
        }
    }

    return backEdges;
}

ControlFlowGraph::Vertices
ControlFlowGraph::getAllLoopHeaders() const
{
    std::set<Edge> backEdges = getAllBackEdges();
    Vertices headers;
    foreach (Edge e, backEdges)
        headers.insert(boost::target(e, *this));
    return headers;
}

//! A DFS visitor used in depth first search.
template <typename VertexT> 
struct DFSVisitor : public boost::default_dfs_visitor
{
    DFSVisitor(std::set<VertexT>& vertices) : vertices_(vertices) {}
    template <typename Vertex, typename Graph>
    void discover_vertex(Vertex u, const Graph & g)
    { vertices_.insert(u); }
    std::set<VertexT>& vertices_;
};

std::map<ControlFlowGraph::Vertex, ControlFlowGraph::Vertices>
ControlFlowGraph::getAllLoops() const
{
    std::set<Edge> backEdges = getAllBackEdges();
    std::map<Vertex, Vertices> loops;
    
    // Build a reverse CFG.
    ControlFlowGraph rvsCFG = makeReverseCopy();
    size_t verticesNum = boost::num_vertices(rvsCFG);
    
    // For each back edge, make the target (loop header) visited then
    // do a DFS from the source of this edge on the reverse CFG.
    foreach (const Edge& edge, backEdges)
    {
        Vertex root = boost::source(edge, rvsCFG);
        Vertex header = boost::target(edge, rvsCFG);
        Vertices vertices;
        
        // Create a DFS visitor in which we add all nodes to vertices set.
        DFSVisitor<Vertex> dfsVisitor(vertices);
        // Build a vector of colors and set all initial colors to white.
        std::vector<boost::default_color_type> colors(verticesNum, boost::white_color);
        // Set the color of the header to black.
        colors[header] = boost::black_color;

        // Do a DFS.
        boost::depth_first_visit(rvsCFG, root, dfsVisitor, &colors[0]);
        
        Vertices& verticesInLoop = loops[header];
        verticesInLoop.insert(vertices.begin(), vertices.end());
        //verticesInLoop.insert(header);
    }
    return loops;
}


namespace
{

//! This function helps to write the DOT file for vertices.
void writeCFGNode(std::ostream& out, const CFGNode& cfgNode)
{
	SgNode* node = cfgNode.getNode();
	ROSE_ASSERT(node);

	std::string nodeColor = "black";
	if (isSgStatement(node))
		nodeColor = "blue";
	else if (isSgExpression(node))
		nodeColor = "green";
	else if (isSgInitializedName(node))
		nodeColor = "red";

	std::string label;

	if (SgFunctionDefinition* funcDef = isSgFunctionDefinition(node))
	{
		std::string funcName = funcDef->get_declaration()->get_name().str();
		if (cfgNode.getIndex() == 0)
			label = "Entry\\n" + funcName;
		else if (cfgNode.getIndex() == 3)
			label = "Exit\\n" + funcName;
	}
	
	if (!isSgScopeStatement(node) && !isSgCaseOptionStmt(node) && !isSgDefaultOptionStmt(node))
	{
		std::string content = node->unparseToString();
		boost::replace_all(content, "\"", "\\\"");
		boost::replace_all(content, "\\n", "\\\\n");
		label += content;
	}
    else
		label += "<" + node->class_name() + ">";
    
    if (label == "")
		label += "<" + node->class_name() + ">";
	
	out << "[label=\""  << label << "\", color=\"" << nodeColor <<
		"\", style=\"" << (cfgNode.isInteresting()? "solid" : "dotted") << "\"]";
}


//! This function helps to write the DOT file for edges.
void writeCFGEdge(std::ostream& out, const CFGEdge& e)
{
	out << "[label=\"" << escapeString(e.toString()) <<
		"\", style=\"" << "solid" << "\"]";
}


} // end of anonymous namespace


//! This function helps to write the DOT file for vertices.
void ControlFlowGraph::writeGraphNode(std::ostream& out, const Vertex& node) const
{
    writeCFGNode(out, *(*this)[node]);
    //VirtualCFG::printNode(out, (*this)[node]);
}

//! This function helps to write the DOT file for edges.
void ControlFlowGraph::writeGraphEdge(std::ostream& out, const Edge& edge) const
{
    writeCFGEdge(out, *(*this)[edge]);
    //VirtualCFG::printEdge(out, (*this)[edge], true);
}


} // end of namespace