#ifndef _______DDG_H__________
#define	_______DDG_H__________

#include "newCFG.h"

namespace SDG
{


//! Define the edge type of DDG.
struct DDGEdge
{
	typedef std::vector<SgInitializedName*> VarName;

	//! All variable names in data dependence of this edge.
	std::set<VarName> varNames;

	void addVarName(const VarName& varName)
	{ varNames.insert(varName);	}
};


class DataDependenceGraph : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, 
		CFGNodePtr, DDGEdge>
{
public:
	typedef boost::graph_traits<DataDependenceGraph>::vertex_descriptor Vertex;
	typedef boost::graph_traits<DataDependenceGraph>::edge_descriptor   Edge;

	//! The default constructor.
	DataDependenceGraph() {}

	//! The constructor building the DDG from a CFG.
	DataDependenceGraph(const ControlFlowGraph& cfg)
	{
		build(cfg);
	}

	//! Build the DDG from the given CFG.
	void build(const ControlFlowGraph& cfg);

	//! Write the DDG to a dot file.
	void toDot(const std::string& filename) const;

protected:

	//! This function helps to write the DOT file for vertices.
	void writeGraphNode(std::ostream& out, const Vertex& node) const;

	//! This function helps to write the DOT file for edges.
	void writeGraphEdge(std::ostream& out, const Edge& edge) const;
};


} // end of namespace SystemDependenceGraph


#endif	/* _______DDG_H__________ */

