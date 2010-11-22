#ifndef BACKSTROKEPDG_H
#define	BACKSTROKEPDG_H

#include "backstrokeCDG.h"
#include "backstrokeDDG.h"
#include <vector>

namespace Backstroke
{

#define foreach BOOST_FOREACH

struct PDGEdge
{
	typedef std::vector<SgInitializedName*> VarName;
	
	enum PDGEdgeType
	{
		ControlDependence,
		DataDependence
	};

	//! Indicate the type of this edge, whether control or data dependence.
	PDGEdgeType type;

	//! Indicate the trueth value of the control dependence.
	bool key;

	//! All variable names in data dependence of this edge.
	std::vector<VarName> varNames;
};

//! A class holding a Program Dependence Graph.

template <class CFGType>
class PDG : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
		typename CFGType::CFGNodeType, PDGEdge>
{
public:
	typedef typename CFGType::CFGNodeType CFGNodeType;

	typedef typename boost::graph_traits<PDG<CFGType> >::vertex_descriptor Vertex;
	typedef typename boost::graph_traits<PDG<CFGType> >::edge_descriptor Edge;

	//! The default constructor.
	PDG() {}

	//! The constructor building the PDG from a CFG.
	PDG(const CFGType& cfg)
	{
		buildPDG(cfg);
	}

	//! Build the PDG from the given CFG.
	void buildPDG(const CFGType& cfg);

	//! Write the PDG to a dot file.
	void toDot(const std::string& filename) const;

protected:

	//! This function helps to write the DOT file for vertices.
	void writeGraphNode(std::ostream& out, const Vertex& node) const
	{
		writeCFGNode(out, (*this)[node]);
	}

	//! This function helps to write the DOT file for edges.
	void writeGraphEdge(std::ostream& out, const Edge& edge) const;
};

template <class CFGType>
void PDG<CFGType>::buildPDG(const CFGType& cfg)
{
	// Remove all nodes and edges first.
	this->clear();
	
	// Build a CDG and DDG.
	CDG<CFGType> cdg(cfg);
	DDG<CFGType> ddg(cfg);

	std::cout << num_vertices(cdg) << ' ' << num_vertices(ddg) << std::endl;

	
	// Build a map from CFGNode to vertices from CDG, DDG and PDG separately.
	typedef boost::tuple<
		typename CDG<CFGType>::Vertex,
		typename DDG<CFGType>::Vertex,
		Vertex> VerticesT;
	std::map<CFGNodeType, VerticesT> cfgNodesToVertices;

	// Add all CDG nodes in the map.
	typename boost::graph_traits<CDG<CFGType> >::vertex_iterator cvIter, cvEnd;
	for (boost::tie(cvIter, cvEnd) = vertices(cdg); cvIter != cvEnd; ++cvIter)
		cfgNodesToVertices[cdg[*cvIter]].get<0>() = *cvIter;

	// Add all DDG nodes in the map, and build new PDG nodes.
	typename boost::graph_traits<DDG<CFGType> >::vertex_iterator dvIter, dvEnd;
	for (boost::tie(dvIter, dvEnd) = vertices(ddg); dvIter != dvEnd; ++dvIter)
	{
		typename std::map<CFGNodeType, VerticesT>::iterator iter = cfgNodesToVertices.find(ddg[*dvIter]);
		ROSE_ASSERT(iter != cfgNodesToVertices.end());

		// Add a vertex to PDG.
		Vertex v = boost::add_vertex(*this);
		(*this)[v] = ddg[*dvIter];
		ROSE_ASSERT(ddg[*dvIter].getNode());
		
		iter->second.get<1>() = *dvIter;
		iter->second.get<2>() = v;
	}

	// Then start to add edges to PDG.

#if 1
	// Add control dependence edges.
	typename boost::graph_traits<CDG<CFGType> >::edge_iterator ceIter, ceEnd;
	for (boost::tie(ceIter, ceEnd) = boost::edges(cdg); ceIter != ceEnd; ++ceIter)
	{
		Vertex src = cfgNodesToVertices[cdg[boost::source(*ceIter, cdg)]].get<2>();
		Vertex tar = cfgNodesToVertices[cdg[boost::target(*ceIter, cdg)]].get<2>();
		Edge edge = boost::add_edge(src, tar, *this).first;

		PDGEdge& pdgEdge = (*this)[edge];
		pdgEdge.type = PDGEdge::ControlDependence;
		pdgEdge.key = cdg[*ceIter];
	}

	// Add data dependence edges.
	typename boost::graph_traits<DDG<CFGType> >::edge_iterator deIter, deEnd;
	for (boost::tie(deIter, deEnd) = boost::edges(ddg); deIter != deEnd; ++deIter)
	{
		Vertex src = cfgNodesToVertices[ddg[boost::source(*deIter, ddg)]].get<2>();
		Vertex tar = cfgNodesToVertices[ddg[boost::target(*deIter, ddg)]].get<2>();
		Edge edge = boost::add_edge(src, tar, *this).first;

		PDGEdge& pdgEdge = (*this)[edge];
		pdgEdge.type = PDGEdge::DataDependence;
		pdgEdge.varNames = ddg[*deIter].varNames;
	}
#endif

	// PDG build complete.
	

	//ROSE_ASSERT(num_vertices(cfg) == num_vertices(ddg));
}

template <class CFGType>
void PDG<CFGType>::toDot(const std::string& filename) const
{
	std::ofstream ofile(filename.c_str(), std::ios::out);
	boost::write_graphviz(ofile, *this,
		boost::bind(&PDG<CFGType>::writeGraphNode, this, ::_1, ::_2),
		boost::bind(&PDG<CFGType>::writeGraphEdge, this, ::_1, ::_2));
}

template <class CFGType>
void PDG<CFGType>::writeGraphEdge(std::ostream& out, const Edge& edge) const
{
	std::string str, style;
	const PDGEdge& pdgEdge = (*this)[edge];
	if (pdgEdge.type == PDGEdge::ControlDependence)
	{
		str = pdgEdge.key ? "T" : "F";
		style = "solid";
	}
	else
	{
		foreach (const PDGEdge::VarName& varName, (*this)[edge].varNames)
			str += VariableRenaming::keyToString(varName) + " ";
		style = "dotted";
	}
	out << "[label=\"" << str << "\", style=\"" << style << "\"]";
}


#undef foreach

} // End of namespace Backstroke

#endif	/* BACKSTROKEPDG_H */

