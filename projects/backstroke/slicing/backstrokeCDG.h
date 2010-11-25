#ifndef BACKSTROKECDG_H
#define	BACKSTROKECDG_H

#include "backstrokeCFG.h"

namespace Backstroke
{

#define foreach BOOST_FOREACH


namespace Details
{

template <class VertexT>
void appendSuccessors(
		VertexT v,
		std::vector<VertexT>& vertices,
		const std::map<VertexT, std::set<VertexT> >& iSucc)
{
	vertices.push_back(v);

	typename std::map<VertexT, std::set<VertexT> >::const_iterator
		iter = iSucc.find(v);
	if (iter != iSucc.end())
	{
		ROSE_ASSERT(!iter->second.empty());

		foreach (VertexT succ, iter->second)
			appendSuccessors(succ, vertices, iSucc);
	}
}

} // End of namespace Details.


//! Build dominance frontiers for all nodes in the given CFG.
template <class VertexT, class CFGType>
std::map<VertexT, std::set<VertexT> >
buildDominanceFrontiers(const std::map<VertexT, VertexT>& iDom, const CFGType& cfg)
{
	typedef typename std::map<VertexT, VertexT>::value_type VV;

	// Find immediate children in the dominator tree for each node.
	std::map<VertexT, std::set<VertexT> > children;
	foreach (const VV& vv, iDom)
		children[vv.second].insert(vv.first);

	VertexT entry = cfg.getEntry();
	ROSE_ASSERT(children.find(entry) != children.end());

	// Use a stack to make a bottom-up traversal of the dominator tree.
	std::vector<VertexT> vertices;
	Details::appendSuccessors(entry, vertices, children);

	//buildTree(children, cfg, "immediateChildren.dot");

	std::map<VertexT, std::set<VertexT> > domFrontiers;

	// Start to build the dominance frontiers.
	while (!vertices.empty())
	{
		VertexT v = vertices.back();
		vertices.pop_back();

		typename boost::graph_traits<CFGType>::adjacency_iterator i, j;
		for (tie(i, j) = boost::adjacent_vertices(v, cfg); i != j; ++i)
		{
			//if (iDom.count(*i) == 0) std::cout << cfg[*i].getNode()->class_name() << std::endl;
			ROSE_ASSERT(iDom.count(*i) > 0);

			if (iDom.find(*i)->second != v)
				domFrontiers[v].insert(*i);
		}

		foreach (VertexT child, children[v])
		{
			foreach (VertexT u, domFrontiers[child])
			{
				ROSE_ASSERT(iDom.count(u) > 0);

				if (iDom.find(u)->second != v)
					domFrontiers[v].insert(u);
			}
		}
	}
	// End build.

	//cout << "Build complete!" << endl;

	//buildTree(domFrontiers, cfg, "dominanceFrontiers.dot");
	return domFrontiers;
}

//! A class holding a Control Dependence Graph.

//! In the CDG, if node a is control dependent on node b, there is an edge a->b.

template <class CFGType>
class CDG : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
		typename CFGType::CFGNodePtr, bool>
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

protected:
	
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
		out << "[label=\"" << ((*this)[edge] ? "T" : "F") <<
		"\", style=\"" << "solid" << "\"]";
	}

};

template <class CFGType>
void CDG<CFGType>::buildCDG(const CFGType& cfg)
{
	typedef typename CFGType::Vertex CFGVertexT;

	// Before the build of CDG, check if the CFG contains any cycle without exit, in which case
	// a CDG cannot be built since not all nodes are in the donimator tree of the reverse CFG.
	
	if (!checkCycle(cfg))
	{
		// Throws an exception if the check fails.
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
	std::map<CFGVertexT, std::set<CFGVertexT> > domFrontiers =
			buildDominanceFrontiers(iDom, rvsCfg);

	// Start to build the CDG.
	std::map<CFGVertexT, Vertex> verticesAdded;

	// Add the exit node into CDG since the dominance frontiers may not contain this node.

	Vertex exit = boost::add_vertex(*this);
	(*this)[exit] = cfg[cfg.getExit()];
	verticesAdded.insert(std::make_pair(cfg.getExit(), exit));

	typedef typename std::map<CFGVertexT, std::set<CFGVertexT> >::value_type VVS;
	foreach (const VVS& vertices, domFrontiers)
	{
		CFGVertexT from = vertices.first;

		Vertex src, tar;
		typename std::map<CFGVertexT, Vertex>::iterator it;
		bool inserted;

		// Add the first node.
		tie(it, inserted) = verticesAdded.insert(std::make_pair(from, Vertex()));
		if (inserted)
		{
			src = boost::add_vertex(*this);
			(*this)[src] = cfg[from];
			it->second = src;
		}
		else
			src = it->second;


		foreach (CFGVertexT to, vertices.second)
		{
			// Add the second node.
			tie(it, inserted) = verticesAdded.insert(std::make_pair(to, Vertex()));
			if (inserted)
			{
				tar = boost::add_vertex(*this);
				(*this)[tar] = cfg[to];
				it->second = tar;
			}
			else
				tar = it->second;

			// Add the edge.
			Edge edge = add_edge(src, tar, *this).first;
			(*this)[edge] = true;
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

	// TODO: this function is not complete.
	return true;
}

#undef foreach

} // End of namespace Backstroke 

#endif	/* BACKSTROKECDG_H */

