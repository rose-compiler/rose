#ifndef BACKSTROKEDDG_H
#define	BACKSTROKEDDG_H

#include "backstrokeCFG.h"
#include <VariableRenaming.h>
#include <boost/lambda/lambda.hpp>
#include <boost/unordered_set.hpp>

namespace Backstroke
{

#define foreach BOOST_FOREACH


//! Define the edge type of DDG.
struct DDGEdge
{
	typedef std::vector<SgInitializedName*> VarName;

	//! All variable names in data dependence of this edge.
	std::set<VarName> varNames;

	void addVarName(const VarName& varName)
	{ varNames.insert(varName);	}
};


//! A class holding a Data Dependence Graph.

//! In the DDG, if node a is data dependent on node b, there is an edge a->b.
//! This graph disallows parallel edges (by using boost::setS).

template <class CFGType>
class DDG : public boost::adjacency_list<boost::setS, boost::vecS, boost::bidirectionalS,
		typename CFGType::CFGNodePtr, DDGEdge>
{
public:
	typedef typename CFGType::CFGNodeType CFGNodeType;
	typedef typename CFGType::CFGNodePtr CFGNodePtr;

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
		writeCFGNode(out, *(*this)[node]);
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

	std::vector<CFGNodePtr> allNodes = cfg.getAllNodes();
	std::sort(allNodes.begin(), allNodes.end(), 
		boost::bind(&CFGNodeType::getNode, _1) < boost::bind(&CFGNodeType::getNode, _2));

	const VariableRenaming::DefUseTable& useTable = varRenaming.getUseTable();
	
	// Record which vertex is already added in the DDG to avoid adding the same vertex twice or more.
	std::map<CFGNodePtr, Vertex> verticesAdded;

	foreach (const CFGNodePtr& node, allNodes)
	{
		Vertex src, tar;

		typename std::map<CFGNodePtr, Vertex>::iterator iter;
		bool inserted;

		// Add the source node into the DDG.
		tie(iter, inserted) = verticesAdded.insert(std::make_pair(node, Vertex()));
		if (inserted)
		{
			src = boost::add_vertex(*this);
			(*this)[src] = node;
			iter->second = src;
		}
		else
			src = iter->second;

		// In a DDG, we shall not connet scope statement node to defs of its subnodes.
		if (isSgScopeStatement(node->getNode()))
			continue;

		// Find all defs of this node, and connect them in DDG.
		// A SgNode may contain sevaral subnodes which have defs, and we will find all those defs.
		std::vector<SgNode*> nodes = SageInterface::querySubTree<SgNode>(node->getNode(), V_SgNode);
		foreach (SgNode* subnode, nodes)
		{
			VariableRenaming::DefUseTable::const_iterator entry = useTable.find(subnode);
			if (entry != useTable.end())
			{
				//SgNode* node = res->first;
				//const VariableRenaming::TableEntry& entry = iter->second;

				typedef VariableRenaming::TableEntry::value_type NameAndVars;
				foreach (const NameAndVars& nameVars, entry->second)
				{
					foreach (SgNode* targetNode, nameVars.second)
					{
						foreach (const CFGNodePtr& n, allNodes)
						{
							// If a non-scope node contains the target node, make this node the target.
							// Note that SageInterface::isAncestor() returns true only on strict ancestor.

							// If the target node is a function definition, no edge is added.
							if (//isSgScopeStatement(n->getNode()) == NULL &&
									isSgFunctionDefinition(n->getNode()) == NULL &&
									(n->getNode() == targetNode ||
									SageInterface::isAncestor(n->getNode(), targetNode)))
							{
								tie(iter, inserted) = verticesAdded.insert(std::make_pair(n, Vertex()));
								if (inserted)
								{
									tar = boost::add_vertex(*this);
									(*this)[tar] = n;
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

