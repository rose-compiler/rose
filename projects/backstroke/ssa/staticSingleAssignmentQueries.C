//Author: George Vulov <georgevulov@hotmail.com>
#include "staticSingleAssignment.h"
#include <rose.h>
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

		if (SageInterface::isPointerType(initName->get_type()))
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
	ROSE_ASSERT(!vec.empty());
	string name = "";

	foreach(SgInitializedName* iter, vec)
	{
		name += iter->get_name().getString();

		if (iter != vec.back())
		{
			if (SageInterface::isPointerType(iter->get_type()))
				name += "->";
			else
				name += ".";
		}
	}

	return name;
}

void StaticSingleAssignment::printNodeDefTable(const NodeReachingDefTable& table)
{

	foreach(const NodeReachingDefTable::value_type& varDefPair, table)
	{
		printf("\t%s: \t", varnameToString(varDefPair.first).c_str());

		string defName;
		switch (varDefPair.second->getType())
		{
			case ReachingDef::EXPANDED_DEF:
				defName = "EXPANDED_DEF";
				break;
			case ReachingDef::ORIGINAL_DEF:
				defName = "ORIGINAL_DEF";
				break;
			case ReachingDef::EXTERNAL_DEF:
				defName = "EXTERNAL_DEF";
				break;
			case ReachingDef::PHI_FUNCTION:
				defName = "PHI_FUNCTION";
				break;
			default:
				ROSE_ASSERT(false);
		}

		printf("(%d)-%s\n", varDefPair.second->getRenamingNumber(), defName.c_str());
	}
}

void StaticSingleAssignment::printFullDefTable(const CFGNodeToDefTableMap& defTable)
{

	foreach(const CFGNodeToDefTableMap::value_type& nodeDefTablePair, defTable)
	{
		const CFGNode& definingNode = nodeDefTablePair.first;

		printf("%s\n", definingNode.toStringForDebugging().c_str());
		printNodeDefTable(nodeDefTablePair.second);
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

void StaticSingleAssignment::printToDOT(SgNode* source, ostream &outFile)
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

				bool uniqueName = current.getNode()->attributeExists(UniqueNameTraversal::varKeyTag);

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

					name = StaticSingleAssignment::varnameToString(attr->getKey());
				}

				//Print the defs to a string
				stringstream defUse;

				//Print defs to a string

				foreach(NodeReachingDefTable::value_type& varDefPair, outgoingDefTable[current])
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

				int line = -1;
				if (current.getNode()->get_file_info() != NULL)
					line = current.getNode()->get_file_info()->get_line();

				//Print this node
				outFile << id << " [label=\"<" << label << "> @ " << line
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

				bool uniqueName = current.getNode()->attributeExists(UniqueNameTraversal::varKeyTag);

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

					name = StaticSingleAssignment::varnameToString(attr->getKey());
				}

				//Print the defs to a string
				stringstream defUse;

				//Print defs to a string

				foreach(NodeReachingDefTable::value_type& varDefPair, outgoingDefTable[current.toNode()])
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
	if (!node->attributeExists(UniqueNameTraversal::varKeyTag))
	{
		return NULL;
	}
	VarUniqueName* uName = dynamic_cast<VarUniqueName*> (node->getAttribute(UniqueNameTraversal::varKeyTag));
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

const static StaticSingleAssignment::NodeReachingDefTable emptyTable;

const StaticSingleAssignment::NodeReachingDefTable& StaticSingleAssignment::getReachingDefsBefore(const CFGNode& node) const
{
	CFGNodeToDefTableMap::const_iterator defs = reachingDefTable.find(node);
	if (defs == reachingDefTable.end())
		return emptyTable;
	else
		return defs->second;
}

const StaticSingleAssignment::NodeReachingDefTable& StaticSingleAssignment::getReachingDefsAfter(const CFGNode& node) const
{
	CFGNodeToDefTableMap::const_iterator defs = outgoingDefTable.find(node);
	if (defs == outgoingDefTable.end())
		return emptyTable;
	else
		return defs->second;
}

const StaticSingleAssignment::NodeReachingDefTable& StaticSingleAssignment::getDefsAtNode(const CFGNode& node) const
{
	CFGNodeToDefTableMap::const_iterator defs = localDefTable.find(node);
	if (defs == localDefTable.end())
		return emptyTable;
	else
		return defs->second;
}

const StaticSingleAssignment::NodeReachingDefTable& StaticSingleAssignment::getReachingDefsBefore(SgNode* astNode) const
{
	return getReachingDefsBefore(astNode->cfgForBeginning());
}

const StaticSingleAssignment::NodeReachingDefTable& StaticSingleAssignment::getReachingDefsAfter(SgNode* astNode) const
{
	return getReachingDefsAfter(astNode->cfgForEnd());
}

const static set<SgVarRefExp*> emptyVarRefSet;

const set<SgVarRefExp*>& StaticSingleAssignment::getUsesAtNode(SgNode* astNode) const
{
	ASTNodeToVarRefsMap::const_iterator uses = astNodeToUses.find(astNode);
	if (uses == astNodeToUses.end())
		return emptyVarRefSet;
	else
		return uses->second;
}

const StaticSingleAssignment::ReachingDefPtr StaticSingleAssignment::getDefinitionForUse(SgVarRefExp* astNode) const
{
	const VarName& varName = getVarName(astNode);
	if (varName == emptyName)
		return ReachingDefPtr();
	
	const NodeReachingDefTable& varReachingDefs = getReachingDefsBefore(astNode);
	
	//Look up the var name in the reaching defs to find the corresponding def
	NodeReachingDefTable::const_iterator reachingDef = varReachingDefs.find(varName);
	
	if (reachingDef == varReachingDefs.end())
		return ReachingDefPtr();
	else
		return reachingDef->second;
}
