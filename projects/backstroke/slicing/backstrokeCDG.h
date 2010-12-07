#ifndef BACKSTROKECDG_H
#define	BACKSTROKECDG_H

#include "backstrokeCFG.h"
#include <boost/graph/strong_components.hpp>
#include <boost/graph/graph_utility.hpp>

namespace Backstroke
{

#define foreach BOOST_FOREACH




//! Define the edge type of CDG.
struct CDGEdge
{
	//! The condition attached to edges in the CDG.
	VirtualCFG::EdgeConditionKind condition;

	//! If the condition is a case edge, this expression is the case value.
	SgExpression* caseLabel;

//	//! Defines the edge kind in a CDG. There are four kinds of control dependences in a CDG:
//	//! true, false, case and default. The latter two are for switch statement.
//	enum CDGEdgeType
//	{
//		cdTrue,
//		cdFalse,
//		cdCase,
//		cdDefault
//	} type;

	////! If the control dependence edge is a case edge, this is the corresponding case value.
	//int caseValue;
};

//! A class holding a Control Dependence Graph.

//! In the CDG, if node a is control dependent on node b, there is an edge a->b.

template <class CFGType>
class CDG : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
		typename CFGType::CFGNodePtr, CDGEdge>
{
public:
	typedef typename CFGType::CFGNodePtr CFGNodePtr;

	typedef typename boost::graph_traits<CDG<CFGType> >::vertex_descriptor Vertex;
	typedef typename boost::graph_traits<CDG<CFGType> >::edge_descriptor Edge;

	//! The default constructor.
	CDG() {}

	//! The constructor building the CDG from a CFG.
	CDG(const CFGType& cfg)
	{
		buildCDG(cfg);
	}

	//! Build the CDG from the given CFG.
	void buildCDG(const CFGType& cfg);

	//! Write the CDG to a dot file.
	void toDot(const std::string& filename) const;

	//! This function helps to write the DOT file for edges.
	//! It's a static function which can be used by other classes (PDG for example).
	static void writeGraphEdge(std::ostream& out, const CDGEdge& edge);
	
protected:

	//! The data structure holding the dominance frontiers for all nodes in a CFG. Note that the control
	//! dependence information is all included. Each node is mapped to its all control dependent nodes.
	//! It is possible that a node is control dependent on another node in different conditions (switch case),
	//! so there is a vector of CFG edges for each control dependence.
	typedef typename std::map<
		typename CFGType::Vertex,
		std::map<typename CFGType::Vertex, 
				std::vector<typename CFGType::Edge> > >
	DominanceFrontiersT;
	
	//! Check if the CFG contains any cycle without exit. If there is such a cycle, return false.
	bool checkCycle(const CFGType& cfg);

	//! This function helps to write the DOT file for vertices.
	void writeGraphNode(std::ostream& out, const Vertex& node) const
	{
		writeCFGNode(out, *(*this)[node]);
	}

	//! This function helps to write the DOT file for edges.
	void writeGraphEdge(std::ostream& out, const Edge& edge) const
	{
		writeGraphEdge(out, (*this)[edge]);
	}

	//! A function help to find all successors for the given node in the CFG.
	void appendSuccessors(
		Vertex v,
		std::vector<Vertex>& vertices,
		const std::map<Vertex, std::set<Vertex> >& iSucc);

	//! Build dominance frontiers for all nodes in the given CFG.
	DominanceFrontiersT	buildDominanceFrontiers(
		const std::map<typename CFGType::Vertex, typename CFGType::Vertex>& iDom, const CFGType& cfg);
};

template <class CFGType>
void CDG<CFGType>::buildCDG(const CFGType& cfg)
{
	typedef typename CFGType::Vertex CFGVertexT;
	typedef typename CFGType::Edge CFGEdgeT;

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

	// First, build a reverse CFG.
	CFGType rvsCfg = cfg.makeReverseCopy();

	// Build the dominator tree of the reverse CFG.
	std::map<CFGVertexT, CFGVertexT> iDom = rvsCfg.buildDominatorTree();

	// Build the dominance frontiers of the reverse CFG, which represents the CDG
	// of the original CFG.
	DominanceFrontiersT domFrontiers = buildDominanceFrontiers(iDom, rvsCfg);

	// Start to build the CDG.
	std::map<CFGVertexT, Vertex> verticesAdded;

	// Add the exit node into CDG since the dominance frontiers may not contain this node.

	Vertex exit = boost::add_vertex(*this);
	(*this)[exit] = cfg[cfg.getExit()];
	verticesAdded.insert(std::make_pair(cfg.getExit(), exit));

	foreach (const typename DominanceFrontiersT::value_type& vertices, domFrontiers)
	{
		CFGVertexT from = vertices.first;

		Vertex src, tar;
		typename std::map<CFGVertexT, Vertex>::iterator it;
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


		typedef typename std::map<CFGVertexT, std::vector<CFGEdgeT> > VertexEdgesMap;
		foreach (const typename VertexEdgesMap::value_type& vertexEdges, vertices.second)
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
				Edge edge = boost::add_edge(src, tar, *this).first;
				(*this)[edge].condition = rvsCfg[cdEdge]->condition();
				(*this)[edge].caseLabel = rvsCfg[cdEdge]->caseLabel();
			}
		}
	}
}

template <class CFGType>
void CDG<CFGType>::toDot(const std::string& filename) const
{
	std::ofstream ofile(filename.c_str(), std::ios::out);
	boost::write_graphviz(ofile, *this,
		boost::bind(&CDG<CFGType>::writeGraphNode, this, ::_1, ::_2),
		boost::bind(&CDG<CFGType>::writeGraphEdge, this, ::_1, ::_2));
}

template <class CFGType>
bool CDG<CFGType>::checkCycle(const CFGType& cfg)
{
	// Add an edge from the exit to entry in the CFG, then count the number of
	// strongly connected components. If the number is greater than 1, there is
	// a cycle in the CFG which has no exit.
	CFGType cfgCopy = cfg;
	boost::add_edge(cfgCopy.getExit(), cfgCopy.getEntry(), cfgCopy);

	std::vector<int> component(num_vertices(cfgCopy));
	int num = boost::strong_components(cfgCopy, &component[0]);

	return num == 1;
}


template <class CFGType>
void CDG<CFGType>::appendSuccessors(
		Vertex v,
		std::vector<Vertex>& vertices,
		const std::map<Vertex, std::set<Vertex> >& iSucc)
{
	vertices.push_back(v);

	typename std::map<Vertex, std::set<Vertex> >::const_iterator
		iter = iSucc.find(v);
	if (iter != iSucc.end())
	{
		ROSE_ASSERT(!iter->second.empty());

		foreach (Vertex succ, iter->second)
			appendSuccessors(succ, vertices, iSucc);
	}
}


//! Build dominance frontiers for all nodes in the given CFG.
template <class CFGType>
typename CDG<CFGType>::DominanceFrontiersT
CDG<CFGType>::buildDominanceFrontiers(
	const std::map<typename CFGType::Vertex, typename CFGType::Vertex>& iDom, const CFGType& cfg)
{
	typedef typename CFGType::Vertex CFGVertexT;
	typedef typename CFGType::Edge CFGEdgeT;

	typedef typename std::map<CFGVertexT, CFGVertexT>::value_type VertexVertexMap;
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

		typename boost::graph_traits<CFGType>::out_edge_iterator i, j;
		for (tie(i, j) = boost::out_edges(v, cfg); i != j; ++i)
		{
			CFGVertexT succ = boost::target(*i, cfg);

			//if (iDom.count(*i) == 0) std::cout << cfg[*i].getNode()->class_name() << std::endl;
			ROSE_ASSERT(iDom.count(succ) > 0);

			if (iDom.find(succ)->second != v)
				domFrontiers[v][succ].push_back(*i);
		}

		foreach (CFGVertexT child, children[v])
		{
			typedef typename std::map<CFGVertexT, std::vector<CFGEdgeT> > VertexEdgesMap;
			foreach (const typename VertexEdgesMap::value_type& vertexEdges, domFrontiers[child])
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

template <class CFGType>
void CDG<CFGType>::writeGraphEdge(std::ostream& out, const CDGEdge& edge)
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

#undef foreach

} // End of namespace Backstroke 

#endif	/* BACKSTROKECDG_H */

