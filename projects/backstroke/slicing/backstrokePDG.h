#ifndef BACKSTROKEPDG_H
#define	BACKSTROKEPDG_H

#include "backstrokeCDG.h"
#include "backstrokeDDG.h"
#include <vector>
#include <boost/shared_ptr.hpp>

namespace Backstroke
{

#define foreach BOOST_FOREACH

//! Define the edge type of PDG.

//! Note that in this structure, if its type is "ControlDependence", only the member
//! "key" is used, else only the member "varNames" is used.
struct PDGEdge
{
	//typedef std::vector<SgInitializedName*> VarName;
	
	enum PDGEdgeType
	{
		ControlDependence,
		DataDependence
	};

	//! Indicate the type of this edge, whether control or data dependence.
	PDGEdgeType type;

	//! A control dependence edge.
	CDGEdge cdEdge;

	//! A data dependence edge.
	DDGEdge ddEdge;

	////! All variable names in data dependence of this edge.
	//std::set<VarName> varNames;
};

//! A class holding a Program Dependence Graph.

//! In the PDG, if node a is control dependent on node b, there is an control dependence edge a->b,
//! if node c is data dependent on node d, there is an data dependence edge c->d.

template <class CFGType>
class PDG : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
		typename CFGType::CFGNodePtr, PDGEdge>
{
public:
	typedef typename CFGType::CFGNodeType CFGNodeType;
	typedef typename CFGType::CFGNodePtr CFGNodePtr;

	typedef typename boost::graph_traits<PDG<CFGType> >::vertex_descriptor Vertex;
	typedef typename boost::graph_traits<PDG<CFGType> >::edge_descriptor Edge;

	//! The default constructor.
	PDG() {}

	//! The constructor building the PDG from a CFG.
	PDG(const CFGType& cfg)
	{
		buildPDG(cfg);
	}

	//! The constructor building the PDG from a function definition.
	PDG(SgFunctionDefinition* funcDef)
	{
		CFGType cfg(funcDef);
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
		writeCFGNode(out, *(*this)[node]);
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

	ROSE_ASSERT(boost::num_vertices(cfg) == boost::num_vertices(ddg));

	// Build a map from CFGNode to vertices from CDG, DDG and PDG separately.
	typedef boost::tuple<
		typename CDG<CFGType>::Vertex,
		typename DDG<CFGType>::Vertex,
		Vertex> VerticesT;
	std::map<CFGNodePtr, VerticesT> cfgNodesToVertices;

	// Add all CDG nodes in the map.
    foreach (typename CDG<CFGType>::Vertex v, vertices(cdg))
		cfgNodesToVertices[cdg[v]].get<0>() = v;

	// Add all DDG nodes in the map, and build new PDG nodes.
    foreach (typename DDG<CFGType>::Vertex v, vertices(ddg))
	{
		typename std::map<CFGNodePtr, VerticesT>::iterator iter = cfgNodesToVertices.find(ddg[v]);
		ROSE_ASSERT(iter != cfgNodesToVertices.end());

		// Add a vertex to PDG.
		Vertex newNode = boost::add_vertex(*this);
		(*this)[newNode] = ddg[v];
		ROSE_ASSERT(ddg[v]->getNode());
		
		iter->second.get<1>() = v;
		iter->second.get<2>() = newNode;
	}

	// Then start to add edges to PDG.

#if 1
	// Add control dependence edges.
    foreach (const typename CDG<CFGType>::Edge& e, boost::edges(cdg))
	{
		Vertex src = cfgNodesToVertices[cdg[boost::source(e, cdg)]].get<2>();
		Vertex tar = cfgNodesToVertices[cdg[boost::target(e, cdg)]].get<2>();
		Edge newEdge = boost::add_edge(src, tar, *this).first;

		PDGEdge& pdgEdge = (*this)[newEdge];
		pdgEdge.type = PDGEdge::ControlDependence;
		pdgEdge.cdEdge = cdg[e];
	}

	// Add data dependence edges.
    foreach (const typename DDG<CFGType>::Edge& e, boost::edges(ddg))
	{
		Vertex src = cfgNodesToVertices[ddg[boost::source(e, ddg)]].get<2>();
		Vertex tar = cfgNodesToVertices[ddg[boost::target(e, ddg)]].get<2>();
		Edge edge = boost::add_edge(src, tar, *this).first;

		PDGEdge& pdgEdge = (*this)[edge];
		pdgEdge.type = PDGEdge::DataDependence;
		pdgEdge.ddEdge = ddg[e];
	}
#endif

	// PDG build complete.
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
		//str = pdgEdge.key ? "T" : "F";
		CDG<CFGType>::writeGraphEdge(out, pdgEdge.cdEdge);
	}
	else
	{
		foreach (const DDGEdge::VarName& varName, (*this)[edge].ddEdge.varNames)
			str += VariableRenaming::keyToString(varName) + " ";
		style = "dotted";
		out << "[label=\"" << str << "\", style=\"" << style << "\"]";
	}
}


#undef foreach

} // End of namespace Backstroke

#endif	/* BACKSTROKEPDG_H */

