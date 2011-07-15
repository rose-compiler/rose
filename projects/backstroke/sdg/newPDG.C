#include "newPDG.h"
#include <VariableRenaming.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/graph/graphviz.hpp>


#define foreach BOOST_FOREACH


namespace Backstroke
{


void ProgramDependenceGraph::build(const ControlFlowGraph& cfg)
{
	// Remove all nodes and edges first.
	this->clear();
	
	// Build a CDG and DDG.
	ControlDependenceGraph cdg(cfg);
	DataDependenceGraph ddg(cfg);

	ROSE_ASSERT(boost::num_vertices(cfg) == boost::num_vertices(ddg));

	// Build a map from CFGNode to vertices from CDG, DDG and PDG separately.
	typedef boost::tuple<
		ControlDependenceGraph::Vertex,
		DataDependenceGraph::Vertex,
		Vertex> VerticesT;
	std::map<CFGNodePtr, VerticesT> cfgNodesToVertices;

	// Add all CDG nodes in the map.
    foreach (ControlDependenceGraph::Vertex v, vertices(cdg))
		cfgNodesToVertices[cdg[v]].get<0>() = v;

	// Add all DDG nodes in the map, and build new PDG nodes.
    foreach (DataDependenceGraph::Vertex v, vertices(ddg))
	{
		std::map<CFGNodePtr, VerticesT>::iterator iter = cfgNodesToVertices.find(ddg[v]);
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
    foreach (const ControlDependenceGraph::Edge& e, boost::edges(cdg))
	{
		Vertex src = cfgNodesToVertices[cdg[boost::source(e, cdg)]].get<2>();
		Vertex tar = cfgNodesToVertices[cdg[boost::target(e, cdg)]].get<2>();
		Edge newEdge = boost::add_edge(src, tar, *this).first;

		PDGEdge& pdgEdge = (*this)[newEdge];
		pdgEdge.type = PDGEdge::ControlDependence;
		pdgEdge.cdEdge = cdg[e];
	}

	// Add data dependence edges.
    foreach (const DataDependenceGraph::Edge& e, boost::edges(ddg))
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

void ProgramDependenceGraph::toDot(const std::string& filename) const
{
	std::ofstream ofile(filename.c_str(), std::ios::out);
	boost::write_graphviz(ofile, *this,
		boost::bind(&ProgramDependenceGraph::writeGraphNode, this, ::_1, ::_2),
		boost::bind(&ProgramDependenceGraph::writeGraphEdge, this, ::_1, ::_2));
}

void ProgramDependenceGraph::writeGraphEdge(std::ostream& out, const Edge& edge) const
{
	std::string str, style;
	const PDGEdge& pdgEdge = (*this)[edge];
	if (pdgEdge.type == PDGEdge::ControlDependence)
	{
		//str = pdgEdge.key ? "T" : "F";
		ControlDependenceGraph::writeGraphEdge(out, pdgEdge.cdEdge);
	}
	else
	{
		foreach (const DDGEdge::VarName& varName, (*this)[edge].ddEdge.varNames)
			str += VariableRenaming::keyToString(varName) + " ";
		style = "dotted";
		out << "[label=\"" << str << "\", style=\"" << style << "\"]";
	}
}


}