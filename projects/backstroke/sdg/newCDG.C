#include "newCDG.h"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/tuple/tuple.hpp>

#define foreach BOOST_FOREACH



namespace SystemDependenceGraph
{


void ControlDependenceGraph::build(const ControlFlowGraph& cfg)
{
	typedef ControlFlowGraph::Vertex CFGVertexT;
	typedef ControlFlowGraph::Edge   CFGEdgeT;

	// Before the build of CDG, check if the CFG contains any cycle without exit, in which case
	// a CDG cannot be built since not all nodes are in the donimator tree of the reverse CFG.
	
	if (!checkCycle(cfg))
	{
		std::cerr << "This function may contain an infinite loop "
				"inside so that its CDG cannot be built" << std::endl;
		throw;
	}
	
	// Remove all nodes and edges.
	this->clear();
    
    // Build a table from CFG edges to edge discriptors in the given CFG.
    // Then we can look up edges from reverse CFG to normal CFG.
    std::map<CFGEdgePtr, CFGEdgeT> edgeTable;
    foreach (const CFGEdgeT& edge, boost::edges(cfg))
        edgeTable[cfg[edge]] = edge;

	// First, build a reverse CFG.
	ControlFlowGraph rvsCfg = cfg.makeReverseCopy();

	// Build the dominator tree of the reverse CFG.
	std::map<CFGVertexT, CFGVertexT> iDom = rvsCfg.getDominatorTree();

	// Build the dominance frontiers of the reverse CFG, which represents the CDG
	// of the original CFG.
	DominanceFrontiersT domFrontiers = buildDominanceFrontiers(iDom, rvsCfg);

	// Start to build the CDG.
	std::map<CFGVertexT, Vertex> verticesAdded;

	// Add the exit node into CDG since the dominance frontiers may not contain this node.

	Vertex exit = boost::add_vertex(*this);
	(*this)[exit] = cfg[cfg.getExit()];
	verticesAdded.insert(std::make_pair(cfg.getExit(), exit));

	foreach (const DominanceFrontiersT::value_type& vertices, domFrontiers)
	{
		CFGVertexT from = vertices.first;

		Vertex src, tar;
		std::map<CFGVertexT, Vertex>::iterator it;
		bool inserted;

		// Add the first node.
		boost::tie(it, inserted) = verticesAdded.insert(std::make_pair(from, Vertex()));
		if (inserted)
		{
			src = boost::add_vertex(*this);
			(*this)[src] = cfg[from];
			it->second = src;
		}
		else
			src = it->second;


		typedef std::map<CFGVertexT, std::vector<CFGEdgeT> > VertexEdgesMap;
		foreach (const VertexEdgesMap::value_type& vertexEdges, vertices.second)
		{
			CFGVertexT to = vertexEdges.first;
			const std::vector<CFGEdgeT>& cdEdges = vertexEdges.second;

			// Add the second node.
			boost::tie(it, inserted) = verticesAdded.insert(std::make_pair(to, Vertex()));
			if (inserted)
			{
				tar = boost::add_vertex(*this);
				(*this)[tar] = cfg[to];
				it->second = tar;
			}
			else
				tar = it->second;

			foreach (CFGEdgeT cdEdge, cdEdges)
			{
				// Add the edge.
				Edge edge = boost::add_edge(tar, src, *this).first;
                (*this)[edge].cfgEdge   = edgeTable[rvsCfg[cdEdge]];
				(*this)[edge].condition = rvsCfg[cdEdge]->condition();
				(*this)[edge].caseLabel = rvsCfg[cdEdge]->caseLabel();
			}
		}
	}
}


ControlDependenceGraph::Vertex 
ControlDependenceGraph::getCDGVertex(SgNode* astNode)
{
    foreach (Vertex node, boost::vertices(*this))
    {
        if ((*this)[node]->getNode() == astNode) 
            return node;
    }    
    return GraphTraits::null_vertex();
}


ControlDependenceGraph::ControlDependences 
ControlDependenceGraph::getControlDependences(CFGNodePtr cfgNode)
{
    ControlDependences controlDeps;
    
    foreach (Vertex node, boost::vertices(*this))
    {
        if ((*this)[node] != cfgNode) continue;
        
        foreach (const Edge& edge, boost::out_edges(node, *this))
        {
            Vertex tgt = boost::target(edge, *this);
            controlDeps.push_back(ControlDependence((*this)[edge], (*this)[tgt]->getNode()));
        }
        return controlDeps;
    }
    
    return controlDeps;
}


ControlDependenceGraph::ControlDependences 
ControlDependenceGraph::getControlDependences(SgNode* astNode)
{
    ControlDependences controlDeps;
    
    Vertex node = getCDGVertex(astNode);
    
    ROSE_ASSERT(node != GraphTraits::null_vertex());
    
    foreach (const Edge& edge, boost::out_edges(node, *this))
    {
        Vertex tgt = boost::target(edge, *this);
        controlDeps.push_back(ControlDependence((*this)[edge], (*this)[tgt]->getNode()));
    }
    return controlDeps;
}


void ControlDependenceGraph::toDot(const std::string& filename) const
{
	std::ofstream ofile(filename.c_str(), std::ios::out);
	boost::write_graphviz(ofile, *this,
		boost::bind(&ControlDependenceGraph::writeGraphNode, this, ::_1, ::_2),
		boost::bind(&ControlDependenceGraph::writeGraphEdge, this, ::_1, ::_2));
}


bool ControlDependenceGraph::checkCycle(const ControlFlowGraph& cfg)
{
	// Add an edge from the exit to entry in the CFG, then count the number of
	// strongly connected components. If the number is greater than 1, there is
	// a cycle in the CFG which has no exit.
	ControlFlowGraph cfgCopy = cfg;
	boost::add_edge(cfgCopy.getExit(), cfgCopy.getEntry(), cfgCopy);

	std::vector<int> component(num_vertices(cfgCopy));
	int num = boost::strong_components(cfgCopy, &component[0]);

	return num == 1;
}



void ControlDependenceGraph::appendSuccessors(
		Vertex v,
		std::vector<Vertex>& vertices,
		const std::map<Vertex, std::set<Vertex> >& iSucc)
{
	vertices.push_back(v);

	std::map<Vertex, std::set<Vertex> >::const_iterator
		iter = iSucc.find(v);
	if (iter != iSucc.end())
	{
		ROSE_ASSERT(!iter->second.empty());

		foreach (Vertex succ, iter->second)
			appendSuccessors(succ, vertices, iSucc);
	}
}


//! Build dominance frontiers for all nodes in the given CFG.

ControlDependenceGraph::DominanceFrontiersT
ControlDependenceGraph::buildDominanceFrontiers(
        const std::map<ControlFlowGraph::Vertex, ControlFlowGraph::Vertex>& iDom, 
        const ControlFlowGraph& cfg)
{
	typedef ControlFlowGraph::Vertex CFGVertexT;
	typedef ControlFlowGraph::Edge   CFGEdgeT;

	typedef std::map<CFGVertexT, CFGVertexT>::value_type VertexVertexMap;
	// Find immediate children in the dominator tree for each node.
	std::map<CFGVertexT, std::set<CFGVertexT> > children;
	foreach (const VertexVertexMap& vv, iDom)
		children[vv.second].insert(vv.first);

	CFGVertexT entry = cfg.getEntry();
	ROSE_ASSERT(children.find(entry) != children.end());

	// Use a stack to make a bottom-up traversal of the dominator tree.
	std::vector<CFGVertexT> vertices;
	appendSuccessors(entry, vertices, children);

	//buildTree(children, cfg, "immediateChildren.dot");

	// Map each node to its control dependence node together with the edge from which we can get the
	// control dependence type (true, false or case).
	DominanceFrontiersT domFrontiers;

	// Start to build the dominance frontiers.
	while (!vertices.empty())
	{
		CFGVertexT v = vertices.back();
		vertices.pop_back();

        foreach (const CFGEdgeT& e, boost::out_edges(v, cfg))
		{
			CFGVertexT succ = boost::target(e, cfg);

			//if (iDom.count(*i) == 0) std::cout << cfg[*i].getNode()->class_name() << std::endl;
			ROSE_ASSERT(iDom.count(succ) > 0);

			if (iDom.find(succ)->second != v)
				domFrontiers[v][succ].push_back(e);
		}

		foreach (CFGVertexT child, children[v])
		{
			typedef std::map<CFGVertexT, std::vector<CFGEdgeT> > VertexEdgesMap;
			foreach (const VertexEdgesMap::value_type& vertexEdges, domFrontiers[child])
			{
				ROSE_ASSERT(iDom.count(vertexEdges.first) > 0);

				if (iDom.find(vertexEdges.first)->second != v)
				{
					foreach (CFGEdgeT edge, vertexEdges.second)
						domFrontiers[v][vertexEdges.first].push_back(edge);
				}
			}
		}
	}
	// End build.

	//cout << "Build complete!" << endl;

	//buildTree(domFrontiers, cfg, "dominanceFrontiers.dot");
	return domFrontiers;
}


void ControlDependenceGraph::writeGraphEdge(std::ostream& out, const CDGEdge& edge)
{
	std::string label;
	switch (edge.condition)
	{
		case VirtualCFG::eckTrue:
			label = "T";
			break;
		case VirtualCFG::eckFalse:
			label = "F";
			break;
		case VirtualCFG::eckCaseLabel:
			label = "case " + edge.caseLabel->unparseToString();
			break;
		case VirtualCFG::eckDefault:
			label = "default";
			break;
		default:
			break;
	}
	out << "[label=\"" << label << "\", style=\"" << "solid" << "\"]";
}



}