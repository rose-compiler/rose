#ifndef BACKSTROKEDDG_H
#define	BACKSTROKEDDG_H

#include "backstrokeCFG.h"
#include <VariableRenaming.h>
#include <boost/lambda/lambda.hpp>

namespace Backstroke
{

#define foreach BOOST_FOREACH


//! Define the edge type of DDG.
struct DDGEdge
{
	typedef std::vector<SgInitializedName*> VarName;

	//! All variable names in data dependence of this edge.
	std::vector<VarName> varNames;

	void addVarName(const VarName& varName)
	{ varNames.push_back(varName); }
};


//! A class holding a Data Dependence Graph.

//! This graph disallows parallel edges (by using boost::setS).

template <class CFGType>
class DDG : public boost::adjacency_list<boost::setS, boost::vecS, boost::bidirectionalS,
		typename CFGType::CFGNodeType, DDGEdge>
{
public:
	typedef typename CFGType::CFGNodeType CFGNodeType;

	typedef typename boost::graph_traits<DDG<CFGType> >::vertex_descriptor Vertex;
	typedef typename boost::graph_traits<DDG<CFGType> >::edge_descriptor Edge;

	//! The default constructor.
	DDG() {}

	//! The constructor building the DDG from a CFG.
	DDG(const CFGType& cfg)
	{
		buildDDG(cfg);
	}

	//! Build the DDG from the given CFG.
	void buildDDG(const CFGType& cfg);

	//! Write the DDG to a dot file.
	void toDot(const std::string& filename) const;

protected:

	//! This function helps to write the DOT file for vertices.
	void writeGraphNode(std::ostream& out, const Vertex& node) const
	{
		writeCFGNode(out, (*this)[node]);
	}

	//! This function helps to write the DOT file for edges.
	void writeGraphEdge(std::ostream& out, const Edge& edge) const
	{
		std::string str;
		foreach (const DDGEdge::VarName& varName, (*this)[edge].varNames)
			str += VariableRenaming::keyToString(varName) + " ";
		out << "[label=\"" << str << "\"]";
	}
};

template <class CFGType>
void DDG<CFGType>::buildDDG(const CFGType& cfg)
{
	// Remove all nodes and edges first.
	this->clear();
	
	SgProject* project = SageInterface::getProject();
	ROSE_ASSERT(project);
	VariableRenaming varRenaming(project);
	varRenaming.run();

	std::vector<CFGNodeType> allNodes = cfg.getAllNodes();
	std::sort(allNodes.begin(), allNodes.end(), 
		boost::bind(&CFGNodeType::getNode, _1) < boost::bind(&CFGNodeType::getNode, _2));

	const VariableRenaming::DefUseTable& useTable = varRenaming.getUseTable();
	
	// Record which vertex is already added in the DDG to avoid adding the same vertex twice or more.
	std::map<CFGNodeType, Vertex> verticesAdded;

	foreach (const CFGNodeType& node, allNodes)
	{
		Vertex src, tar;

		typename std::map<CFGNodeType, Vertex>::iterator iter;
		bool inserted;

		// Add this node into the DDG.
		tie(iter, inserted) = verticesAdded.insert(std::make_pair(node, Vertex()));
		if (inserted)
		{
			src = boost::add_vertex(*this);
			(*this)[src] = node;
			iter->second = src;
		}
		else
			src = iter->second;

		// Find all defs of this node, and connect them in DDG.
		VariableRenaming::DefUseTable::const_iterator entry = useTable.find(node.getNode());
		if (entry != useTable.end())
		{
			//SgNode* node = res->first;
			//const VariableRenaming::TableEntry& entry = iter->second;

			typedef VariableRenaming::TableEntry::value_type NameAndVars;
			foreach (const NameAndVars& nameVars, entry->second)
			{
				foreach (SgNode* n, nameVars.second)
				{
					// Find all CFG node with the same SgNode as n.
					typename std::vector<CFGNodeType>::iterator i, j;
					boost::tie(i, j) = std::equal_range(allNodes.begin(), allNodes.end(), CFGNodeType(n),
						boost::bind(&CFGNodeType::getNode, _1) < boost::bind(&CFGNodeType::getNode, _2));

					// FIXME: Why the following code does not work?
					//boost::tie(i, j) = std::equal_range(allNodes.begin(), allNodes.end(), n,
					//	boost::bind(&CFGNodeType::getNode, _1) < _2);

					// For every target node, add it into the DDG.
					for (;i != j; ++i)
					{
						tie(iter, inserted) = verticesAdded.insert(std::make_pair(*i, Vertex()));
						if (inserted)
						{
							tar = boost::add_vertex(*this);
							(*this)[tar] = *i;
							iter->second = tar;
						}
						else
							tar = iter->second;

						// Add the edge.
						Edge edge = add_edge(src, tar, *this).first;
						(*this)[edge].addVarName(nameVars.first);
					}
				}
			}
		}
	}
}

template <class CFGType>
void DDG<CFGType>::toDot(const std::string& filename) const
{
	std::ofstream ofile(filename.c_str(), std::ios::out);
	boost::write_graphviz(ofile, *this,
		boost::bind(&DDG<CFGType>::writeGraphNode, this, ::_1, ::_2),
		boost::bind(&DDG<CFGType>::writeGraphEdge, this, ::_1, ::_2));
}

#undef foreach

} // End of namespace Backstroke

#endif	/* BACKSTROKEDDG_H */

