//Author: George Vulov <georgevulov@hotmail.com>
//Based on work by Justin Frye <jafrye@tamu.edu>
#include "staticSingleAssignment.h"
#include "rose.h"
#include <boost/foreach.hpp>
#include <queue>

#define foreach BOOST_FOREACH

using namespace std;
using namespace ssa_private;

SgExpression* StaticSingleAssignment::buildVariableReference(const VarName& var, SgScopeStatement* scope)
{
	ROSE_ASSERT(var.size() > 0);

	SgExpression* varsSoFar = SageBuilder::buildVarRefExp(var.front(), scope);

	for (size_t i = 0; i < var.size(); i++)
	{
		SgInitializedName* initName = var[i];
		if (initName == var.back())
		{
			break;
		}

		SgVarRefExp* nextVar = SageBuilder::buildVarRefExp(var[i + 1], scope);

		if (isSgPointerType(initName->get_type()))
		{
			varsSoFar = SageBuilder::buildArrowExp(varsSoFar, nextVar);
		}
		else
		{
			varsSoFar = SageBuilder::buildDotExp(varsSoFar, nextVar);
		}
	}

	return varsSoFar;
}

//Printing functions
string StaticSingleAssignment::varnameToString(const VarName& vec)
{
	string name = "";

	foreach(const VarName::value_type& iter, vec)
	{
		if (iter != vec.front())
		{
			name += ":";
		}
		name += iter->get_name().getString();
	}

	return name;
}

void StaticSingleAssignment::printOriginalDefs(SgNode* node)
{
	cout << "Original Def Table for [" << node->class_name() << ":" << node << "]:" << endl;

	foreach(const VarName& definedVar, originalDefTable[node])
	{
		cout << "  Defs for [" << varnameToString(definedVar) << "]:";
		cout << "    -[" << node->class_name() << ":" << node << "]" << endl;
	}
}

void StaticSingleAssignment::printOriginalDefTable()
{
	cout << "Original Def Table:" << endl;

	pair<SgNode*, std::set<VarName> > node;
	foreach(node, originalDefTable)
	{
		cout << "  Original Def Table for [" << node.first->class_name() << ":" << node.first << "]:" << endl;
		printOriginalDefs(node.first);
	}
}

void StaticSingleAssignment::printLocalDefUseTable(const StaticSingleAssignment::LocalDefUseTable& table)
{
	foreach(const LocalDefUseTable::value_type& nodeVarsPair, table)
	{
		const SgNode* node = nodeVarsPair.first;
		printf("%s@%d: ", node->class_name().c_str(), node->get_file_info()->get_line());
		foreach (const VarName& var, nodeVarsPair.second)
		{
			printf("%s, ", varnameToString(var).c_str());
		}
		printf("\n");
	}
}

void StaticSingleAssignment::toDOT(const string fileName)
{
	ROSE_ASSERT(fileName != "");

	typedef vector<SgSourceFile*> fileVec;
	fileVec files = SageInterface::querySubTree<SgSourceFile > (project, V_SgSourceFile);

	//print all the functions in each file

	foreach(fileVec::value_type& file, files)
	{
		ofstream outFile((StringUtility::stripPathFromFileName(file->getFileName())
				+ "_" + fileName).c_str());

		if (!outFile.good())
		{
			if (getDebug())
				cout << "Error printing DOT." << endl;

			return;
		}

		//Print the CFG of the function
		printToDOT(file, outFile);
	}
}

void StaticSingleAssignment::toFilteredDOT(const string fileName)
{
	ROSE_ASSERT(fileName != "");

	typedef vector<SgSourceFile*> fileVec;
	fileVec files = SageInterface::querySubTree<SgSourceFile > (project, V_SgSourceFile);

	//print all the functions in each file

	foreach(fileVec::value_type& file, files)
	{
		ofstream outFile((StringUtility::stripPathFromFileName(file->getFileName())
				+ "_filtered_" + fileName).c_str());

		if (!outFile.good())
		{
			if (getDebug())
				cout << "Error printing DOT." << endl;

			return;
		}

		//Print the CFG of the function
		printToFilteredDOT(file, outFile);
	}
}

void StaticSingleAssignment::printToDOT(SgSourceFile* source, ofstream &outFile)
{
	if (!outFile.good())
	{
		if (getDebug())
			cout << "Error: Bad ofstream in printToDOT()" << endl;
		return;
	}

	typedef CFGNode cfgNode;
	typedef CFGEdge cfgEdge;

	typedef vector<SgFunctionDefinition*> funcDefVec;
	funcDefVec funcs = SageInterface::querySubTree<SgFunctionDefinition > (source, V_SgFunctionDefinition);

	//Start the graph
	outFile << "digraph SSAGraph {\n";

	//Iterate all the functions and print them in the same file.

	foreach(funcDefVec::value_type& func, funcs)
	{
		vector<cfgNode> visited;
		stack<cfgNode> traverse;
		cfgNode current = cfgNode(func->cfgForBeginning());

		traverse.push(current);

		while (!traverse.empty())
		{
			current = traverse.top();

			//If not visited
			if (count(visited.begin(), visited.end(), current) == 0)
			{
				//Visit the node and print it
				string id = current.id();
				string nodeColor = "black";

				bool uniqueName = current.getNode()->attributeExists(StaticSingleAssignment::varKeyTag);

				if (isSgStatement(current.getNode()))
					nodeColor = "blue";
				else if (isSgExpression(current.getNode()))
					nodeColor = "green";
				else if (isSgInitializedName(current.getNode()))
					nodeColor = "red";

				string name = "";
				if (uniqueName)
				{
					VarUniqueName *attr = getUniqueName(current.getNode());
					ROSE_ASSERT(attr);

					name = attr->getNameString();
				}

				//Print the defs to a string
				stringstream defUse;

				//Print defs to a string
				foreach(NodeReachingDefTable::value_type& varDefPair, reachingDefsTable[current.getNode()].second)
				{
					defUse << "Def [" << varnameToString(varDefPair.first) << "]: ";
					defUse << varDefPair.second->getRenamingNumber() << " - "
							<< (varDefPair.second->isPhiFunction() ? "Phi" : "Concrete") << "\\n";
				}

				//TODO
				//Print the uses to a string
				/*foreach(TableEntry::value_type& entry, useTable[current.getNode()])
				{
					defUse << "Use [" << varnameToString(entry.first) << "]: ";

					foreach(NodeVec::value_type& val, entry.second)
					{
						defUse << getRenameNumberForNode(entry.first, val) << ": " << val << ", ";
					}
					defUse << "\\n";
				}*/

				//Copy out the string and trim off the last '\n'
				string defUseStr = defUse.str().substr(0, defUse.str().size() - 2);


				string label = escapeString(current.getNode()->class_name());
				if (isSgFunctionDefinition(current.getNode()))
					label += ":" + escapeString(isSgFunctionDefinition(current.getNode())->get_declaration()->get_name());

				//Print this node
				outFile << id << " [label=\"<" << label << ">:" << current.getNode()
						//Now we add the unique name information
						<< ((name != "") ? "\\n" : "") << name
						<< ((defUseStr != "") ? "\\n" : "") << defUseStr
						<< "\", color=\"" << nodeColor << "\", style=\""
						<< (current.isInteresting() ? "solid" : "dotted") << "\"];\n";

				//Now print the out edges
				vector<cfgEdge> outEdges = current.outEdges();

				foreach(vector<cfgEdge>::value_type& edge, outEdges)
				{
					outFile << edge.source().id() << " -> " << edge.target().id()
							<< " [label=\"" << escapeString(edge.toString())
							<< "\"];\n";
				}
			}

			visited.push_back(current);

			vector<cfgEdge> outEdges = current.outEdges();

			foreach(vector<cfgEdge>::value_type& edge, outEdges)
			{
				//If we haven't seen the target of this node yet, process the node
				if (count(visited.begin(), visited.end(), edge.target()) == 0)
				{
					traverse.push(edge.target());
					break;
				}
			}

			//If there are no new out edges to explore
			if (traverse.top() == current)
			{
				vector<cfgEdge> inEdges = current.inEdges();

				foreach(vector<cfgEdge>::value_type& edge, inEdges)
				{
					//If we haven't seen the target of this node yet, process the node
					if (count(visited.begin(), visited.end(), edge.target()) == 0)
					{
						traverse.push(edge.source());
						break;
					}
				}
			}

			//No out or in edges left to print, pop this node
			if (traverse.top() == current)
			{
				traverse.pop();
			}
		}
	}

	//End the graph
	outFile << "}\n";
}

void StaticSingleAssignment::printToFilteredDOT(SgSourceFile* source, ofstream& outFile)
{
	if (!outFile.good())
	{
		if (getDebug())
			cout << "Error: Bad ofstream in printToDOT()" << endl;
		return;
	}

	typedef FilteredCFGNode<DataflowCfgFilter> cfgNode;
	typedef FilteredCFGEdge<DataflowCfgFilter> cfgEdge;

	typedef vector<SgFunctionDefinition*> funcDefVec;
	funcDefVec funcs = SageInterface::querySubTree<SgFunctionDefinition > (source, V_SgFunctionDefinition);

	//Start the graph
	outFile << "digraph SSAGraph {\n";

	//Iterate all the functions and print them in the same file.

	foreach(funcDefVec::value_type& func, funcs)
	{
		vector<cfgNode> visited;
		stack<cfgNode> traverse;
		cfgNode current = cfgNode(func->cfgForBeginning());

		traverse.push(current);

		while (!traverse.empty())
		{
			current = traverse.top();

			//If not visited
			if (count(visited.begin(), visited.end(), current) == 0)
			{
				//Visit the node and print it
				string id = current.id();
				string nodeColor = "black";

				bool uniqueName = current.getNode()->attributeExists(StaticSingleAssignment::varKeyTag);

				if (isSgStatement(current.getNode()))
					nodeColor = "blue";
				else if (isSgExpression(current.getNode()))
					nodeColor = "green";
				else if (isSgInitializedName(current.getNode()))
					nodeColor = "red";

				string name = "";
				if (uniqueName)
				{
					VarUniqueName *attr = getUniqueName(current.getNode());
					ROSE_ASSERT(attr);

					name = attr->getNameString();
				}

				//Print the defs to a string
				stringstream defUse;

				//Print defs to a string
				foreach(NodeReachingDefTable::value_type& varDefPair, reachingDefsTable[current.getNode()].second)
				{
					defUse << "Def [" << varnameToString(varDefPair.first) << "]: ";
					defUse << varDefPair.second->getRenamingNumber() << " - "
							<< (varDefPair.second->isPhiFunction() ? "Phi" : "Concrete") << "\\n";
				}

				//TODO: Update dot file generation
				/*//Print the uses to a string
				foreach(TableEntry::value_type& entry, useTable[current.getNode()])
				{
					defUse << "Use [" << varnameToString(entry.first) << "]: ";

					foreach(NodeVec::value_type& val, entry.second)
					{
						defUse << getRenameNumberForNode(entry.first, val) << ": " << val << ", ";
					}
					defUse << "\\n";
				}
				*/

				//Copy out the string and trim off the last '\n'
				string defUseStr = defUse.str().substr(0, defUse.str().size() - 2);

				string label = escapeString(current.getNode()->class_name());
				if (isSgFunctionDefinition(current.getNode()))
					label += ":" + escapeString(isSgFunctionDefinition(current.getNode())->get_declaration()->get_name());

				//Print this node
				outFile << id << " [label=\"<" << label << ">:" << current.getNode()
						//Now we add the unique name information
						<< ((name != "") ? "\\n" : "") << name
						<< ((defUseStr != "") ? "\\n" : "") << defUseStr
						<< "\", color=\"" << nodeColor << "\", style=\""
						<< (current.isInteresting() ? "solid" : "dotted") << "\"];\n";

				//Now print the out edges
				vector<cfgEdge> outEdges = current.outEdges();

				foreach(vector<cfgEdge>::value_type& edge, outEdges)
				{
					outFile << edge.source().id() << " -> " << edge.target().id()
							<< " [label=\"" << escapeString(edge.toString())
							<< "\"];\n";
				}
			}

			visited.push_back(current);

			vector<cfgEdge> outEdges = current.outEdges();

			foreach(vector<cfgEdge>::value_type& edge, outEdges)
			{
				//If we haven't seen the target of this node yet, process the node
				if (count(visited.begin(), visited.end(), edge.target()) == 0)
				{
					traverse.push(edge.target());
					break;
				}
			}

			//If there are no new out edges to explore
			if (traverse.top() == current)
			{
				vector<cfgEdge> inEdges = current.inEdges();

				foreach(vector<cfgEdge>::value_type& edge, inEdges)
				{
					//If we haven't seen the target of this node yet, process the node
					if (count(visited.begin(), visited.end(), edge.target()) == 0)
					{
						traverse.push(edge.source());
						break;
					}
				}
			}

			//No out or in edges left to print, pop this node
			if (traverse.top() == current)
			{
				traverse.pop();
			}
		}
	}

	//End the graph
	outFile << "}\n";
}

VarUniqueName* StaticSingleAssignment::getUniqueName(SgNode* node)
{
	if (!node->attributeExists(StaticSingleAssignment::varKeyTag))
	{
		return NULL;
	}
	VarUniqueName* uName = dynamic_cast<VarUniqueName*> (node->getAttribute(StaticSingleAssignment::varKeyTag));
	return uName;
}

const StaticSingleAssignment::VarName& StaticSingleAssignment::getVarName(SgNode* node)
{
	if (node == NULL || getUniqueName(node) == NULL)
	{
		return emptyName;
	}
	return getUniqueName(node)->getKey();
}

bool StaticSingleAssignment::isPrefixOfName(VarName name, VarName prefix)
{
	if (name.size() < prefix.size())
		return false;

	for (size_t i = 0; i < prefix.size(); i++)
	{
		if (name[i] != prefix[i])
			return false;
	}

	return true;
}

const StaticSingleAssignment::NodeReachingDefTable& StaticSingleAssignment::getReachingDefsAtNode(SgNode* node) const
{
	const static NodeReachingDefTable emptyTable;
	GlobalReachingDefTable::const_iterator reachingDefsIter = reachingDefsTable.find(node);
	if (reachingDefsIter == reachingDefsTable.end())
	{
		return emptyTable;
	}
	else
	{
		return reachingDefsIter->second.second;
	}
}

const StaticSingleAssignment::NodeReachingDefTable& StaticSingleAssignment::getUsesAtNode(SgNode* node) const
{
	const static NodeReachingDefTable emptyTable;
	UseTable::const_iterator usesIter = useTable.find(node);
	if (usesIter == useTable.end())
	{
		return emptyTable;
	}
	else
	{
		return usesIter->second;
	}
}

set<StaticSingleAssignment::VarName> StaticSingleAssignment::getVarsDefinedInSubtree(SgNode* root) const
{
	class CollectDefsTraversal : public AstSimpleProcessing
	{
	public:
		const StaticSingleAssignment* ssa;

		//All the varNames that have uses in the function
		set<VarName> definedNames;

		void visit(SgNode* node)
		{
			//Vars defined on function entry are not 'really' defined. These definitions just represent the external value
			//of the variable flowing inside the function body.
			if (isSgFunctionDefinition(node))
				return;

			if (ssa->ssaLocalDefTable.find(node) == ssa->ssaLocalDefTable.end())
				return;

			const NodeReachingDefTable& nodeDefs = ssa->ssaLocalDefTable.find(node)->second;

			foreach(const NodeReachingDefTable::value_type& varDefPair, nodeDefs)
			{
				definedNames.insert(varDefPair.first);
			}
		}
	};

	CollectDefsTraversal defsTrav;
	defsTrav.ssa = this;
	defsTrav.traverse(root, preorder);

	return defsTrav.definedNames;
}