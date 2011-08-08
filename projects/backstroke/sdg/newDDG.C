#include "newDDG.h"
#include <VariableRenaming.h>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/unordered_set.hpp>

#define foreach BOOST_FOREACH


namespace SDG
{


bool isContainerNode(SgNode* node)
{
    return isSgScopeStatement(node) || 
            isSgCaseOptionStmt(node) || 
            isSgDefaultOptionStmt(node);
}


void DataDependenceGraph::build(const ControlFlowGraph& cfg)
{
	// Remove all nodes and edges first.
	this->clear();
	
	SgProject* project = SageInterface::getProject();
	ROSE_ASSERT(project);
	VariableRenaming varRenaming(project);
	varRenaming.run();

	std::vector<CFGNodePtr> allNodes = cfg.getAllNodes();
	std::sort(allNodes.begin(), allNodes.end(), 
		boost::bind(&CFGNode::getNode, _1) < boost::bind(&CFGNode::getNode, _2));

	const VariableRenaming::DefUseTable& useTable = varRenaming.getUseTable();
	
	// Record which vertex is already added in the DDG to avoid adding the same vertex twice or more.
	std::map<CFGNodePtr, Vertex> verticesAdded;
    
    typedef std::map<std::pair<Vertex, Vertex>, std::set<DDGEdge::VarName> > DDGEdgeTable;
    DDGEdgeTable ddgEdges;

	foreach (const CFGNodePtr& node, allNodes)
	{
		Vertex src, tar;

		std::map<CFGNodePtr, Vertex>::iterator iter;
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

		// In a DDG, we shall not connect scope statement node to defs of its subnodes.
		if (isContainerNode(node->getNode()))
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
							if (!isContainerNode(n->getNode()) &&
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
                                
                                ddgEdges[std::make_pair(tar, src)].insert(nameVars.first);
                                
								//Edge edge = add_edge(tar, src, *this).first;
								//(*this)[edge].addVarName(nameVars.first);
							}
						}
					}
				}
			}
		}
	}
    
    foreach (const DDGEdgeTable::value_type& edgeAndName, ddgEdges)
    {
        Vertex src = edgeAndName.first.first;
        Vertex tar = edgeAndName.first.second;
        
        // If source and target are the same AST node, don't add DDG edges between them.
        if ((*this)[src]->getNode() == (*this)[tar]->getNode())
            continue;
        
        Edge edge = add_edge(src, tar, *this).first;
        foreach (const DDGEdge::VarName& name, edgeAndName.second)
            (*this)[edge].addVarName(name);
    }
}


void DataDependenceGraph::toDot(const std::string& filename) const
{
	std::ofstream ofile(filename.c_str(), std::ios::out);
	boost::write_graphviz(ofile, *this,
		boost::bind(&DataDependenceGraph::writeGraphNode, this, ::_1, ::_2),
		boost::bind(&DataDependenceGraph::writeGraphEdge, this, ::_1, ::_2));
}


void DataDependenceGraph::writeGraphNode(std::ostream& out, const Vertex& node) const
{
    writeCFGNode(out, *(*this)[node]);
}

void DataDependenceGraph::writeGraphEdge(std::ostream& out, const Edge& edge) const
{
    std::string str;
    foreach (const DDGEdge::VarName& varName, (*this)[edge].varNames)
        str += VariableRenaming::keyToString(varName) + " ";
    out << "[label=\"" << str << "\"]";
}



}