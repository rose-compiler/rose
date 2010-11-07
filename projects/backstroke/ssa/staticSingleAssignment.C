//Author: George Vulov <georgevulov@hotmail.com>
//Based on work by Justin Frye <jafrye@tamu.edu>

#include "sage3basic.h"
#include "staticSingleAssignment.h"
#include "sageInterface.h"
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <queue>
#include <fstream>
#include <stack>
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>
#include "uniqueNameTraversal.h"
#include "defsAndUsesTraversal.h"

#define foreach BOOST_FOREACH

using namespace std;
using namespace ssa_private;

//Initializations of the static attribute tags
string StaticSingleAssignment::varKeyTag = "rename_KeyTag";
SgInitializedName* StaticSingleAssignment::thisDecl = NULL;
StaticSingleAssignment::VarName StaticSingleAssignment::emptyName;
StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::emptyRenameTable;
StaticSingleAssignment::NumNodeRenameEntry StaticSingleAssignment::emptyRenameEntry;

//Printing functions

string StaticSingleAssignment::keyToString(const VarName& vec)
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

void StaticSingleAssignment::printDefs(SgNode* node)
{
	cout << "Def Table for [" << node->class_name() << ":" << node << "]:" << endl;

	foreach(TableEntry::value_type& entry, defTable[node])
	{
		cout << "  Defs for [" << keyToString(entry.first) << "]:" << endl;

		foreach(NodeVec::value_type& iter, entry.second)
		{
			cout << "    -[" << iter->class_name() << ":" << iter << "]" << endl;
		}
	}
}

void StaticSingleAssignment::printDefs(map< vector<SgInitializedName*>, vector<SgNode*> >& table)
{
	cout << "Def Table:" << endl;

	foreach(TableEntry::value_type& entry, table)
	{
		cout << "  Defs for [" << keyToString(entry.first) << "]:" << endl;

		foreach(NodeVec::value_type& iter, entry.second)
		{
			cout << "    -[" << iter->class_name() << ":" << iter << "]" << endl;
		}
	}
}

void StaticSingleAssignment::printOriginalDefs(SgNode* node)
{
	cout << "Original Def Table for [" << node->class_name() << ":" << node << "]:" << endl;

	foreach(TableEntry::value_type& entry, originalDefTable[node])
	{
		cout << "  Defs for [" << keyToString(entry.first) << "]:" << endl;

		foreach(NodeVec::value_type& iter, entry.second)
		{
			cout << "    -[" << iter->class_name() << ":" << iter << "]" << endl;
		}
	}
}

void StaticSingleAssignment::printOriginalDefTable()
{
	cout << "Original Def Table:" << endl;

	foreach(DefUseTable::value_type& node, originalDefTable)
	{
		cout << "  Original Def Table for [" << node.first->class_name() << ":" << node.first << "]:" << endl;

		foreach(TableEntry::value_type& entry, originalDefTable[node.first])
		{
			cout << "    Defs for [" << keyToString(entry.first) << "]:" << endl;

			foreach(NodeVec::value_type& iter, entry.second)
			{
				cout << "      -[" << iter->class_name() << ":" << iter << "]" << endl;
			}
		}
	}
}

void StaticSingleAssignment::printUses(SgNode* node)
{
	cout << "Use Table for [" << node->class_name() << ":" << node << "]:" << endl;

	foreach(TableEntry::value_type& entry, useTable[node])
	{
		cout << "  Uses for [" << keyToString(entry.first) << "]:" << endl;

		foreach(NodeVec::value_type& iter, entry.second)
		{
			cout << "    -[" << iter->class_name() << ":" << iter << "]" << endl;
		}
	}
}

void StaticSingleAssignment::printUses(map< vector<SgInitializedName*>, vector<SgNode*> >& table)
{
	cout << "Use Table:" << endl;

	foreach(TableEntry::value_type& entry, table)
	{
		cout << "  Uses for [" << keyToString(entry.first) << "]:" << endl;

		foreach(NodeVec::value_type& iter, entry.second)
		{
			cout << "    -[" << iter->class_name() << ":" << iter << "]" << endl;
		}
	}
}

void StaticSingleAssignment::printRenameTable()
{
	printRenameTable(numRenameTable);
}

void StaticSingleAssignment::printRenameTable(const VarName& var)
{
	cout << "Names for [" << keyToString(var) << "]:" << endl;

	printRenameEntry(numRenameTable[var]);
}

void StaticSingleAssignment::printRenameTable(const NodeNumRenameTable& table)
{
	cout << "Rename Table:" << endl;

	//Iterate the table

	foreach(const NodeNumRenameTable::value_type& entry, table)
	{
		cout << "  Names for [" << keyToString(entry.first) << "]:" << endl;

		//Print out the renamings in order
		printRenameEntry(entry.second);
	}
}

void StaticSingleAssignment::printRenameTable(const NumNodeRenameTable& table)
{
	cout << "Rename Table:" << endl;

	//Iterate the table

	foreach(const NumNodeRenameTable::value_type& entry, table)
	{
		cout << "  Names for [" << keyToString(entry.first) << "]:" << endl;

		//Print out the renamings in order.
		printRenameEntry(entry.second);
	}
}

void StaticSingleAssignment::printRenameEntry(const NodeNumRenameEntry& entry)
{
	int start = 0;
	int end = 0;

	//Iterate the entry

	foreach(const NodeNumRenameEntry::value_type& iter, entry)
	{
		if (start == 0 && end == 0)
		{
			start = iter.second;
			end = iter.second;
			continue;
		}

		if (start > iter.second)
		{
			start = iter.second;
		}

		if (end < iter.second)
		{
			end = iter.second;
		}
	}

	while (start <= end)
	{
		SgNode* current = NULL;
		//Find the entry for start if it exists

		foreach(const NodeNumRenameEntry::value_type& iter, entry)
		{
			if (iter.second == start)
			{
				current = iter.first;
				break;
			}
		}

		if (current != NULL)
		{
			cout << "      " << start << ": " << current << endl;
		}

		start++;
	}
}

void StaticSingleAssignment::printRenameEntry(const NumNodeRenameEntry& entry)
{
	//Iterate the entry

	foreach(const NumNodeRenameEntry::value_type& iter, entry)
	{
		cout << "      " << iter.first << ": " << iter.second << endl;
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
					if (getDebug())
						cout << "Getting Unique Name attribute." << endl;
					VarUniqueName *attr = getUniqueName(current.getNode());
					ROSE_ASSERT(attr);

					name = attr->getNameString();
				}

				//Print the defs to a string
				stringstream defUse;

				foreach(TableEntry::value_type& entry, defTable[current.getNode()])
				{
					defUse << "Def [" << keyToString(entry.first) << "]: ";

					foreach(NodeVec::value_type& val, entry.second)
					{
						defUse << getRenameNumberForNode(entry.first, val) << ": " << val << ", ";
					}
					defUse << "\\n";
				}
				//Print the uses to a string

				foreach(TableEntry::value_type& entry, useTable[current.getNode()])
				{
					defUse << "Use [" << keyToString(entry.first) << "]: ";

					foreach(NodeVec::value_type& val, entry.second)
					{
						defUse << getRenameNumberForNode(entry.first, val) << ": " << val << ", ";
					}
					defUse << "\\n";
				}

				//Copy out the string and trim off the last '\n'
				string defUseStr = defUse.str().substr(0, defUse.str().size() - 2);


				//Print this node
				outFile << id << " [label=\"<" << escapeString(current.getNode()->class_name()) << ">:" << current.getNode()
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

	typedef FilteredCFGNode<IsDefUseFilter> cfgNode;
	typedef FilteredCFGEdge<IsDefUseFilter> cfgEdge;

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
					if (getDebug())
						cout << "Getting Unique Name attribute." << endl;
					VarUniqueName *attr = getUniqueName(current.getNode());
					ROSE_ASSERT(attr);

					name = attr->getNameString();
				}

				//Print the defs to a string
				stringstream defUse;

				foreach(TableEntry::value_type& entry, defTable[current.getNode()])
				{
					defUse << "Def [" << keyToString(entry.first) << "]: ";

					foreach(NodeVec::value_type& val, entry.second)
					{
						defUse << getRenameNumberForNode(entry.first, val) << ": " << val << ", ";
					}
					defUse << "\\n";
				}
				//Print the uses to a string

				foreach(TableEntry::value_type& entry, useTable[current.getNode()])
				{
					defUse << "Use [" << keyToString(entry.first) << "]: ";

					foreach(NodeVec::value_type& val, entry.second)
					{
						defUse << getRenameNumberForNode(entry.first, val) << ": " << val << ", ";
					}
					defUse << "\\n";
				}

				//Copy out the string and trim off the last '\n'
				string defUseStr = defUse.str().substr(0, defUse.str().size() - 2);


				//Print this node
				outFile << id << " [label=\"<" << escapeString(current.getNode()->class_name()) << ">:" << current.getNode()
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

StaticSingleAssignment::VarName StaticSingleAssignment::getVarName(SgNode* node)
{
	if (node == NULL || getUniqueName(node) == NULL)
	{
		return StaticSingleAssignment::VarName();
	}
	return getUniqueName(node)->getKey();
}

bool StaticSingleAssignment::isFromLibrary(SgNode* node)
{
	Sg_File_Info* fi = node->get_file_info();
	if (fi->isCompilerGenerated())
		return true;
	string filename = fi->get_filenameString();
	//cout << "Filename string '" << filename << "' for " << node->class_name() << node << endl;
	if ((filename.find("include") != string::npos))
	{
		//cout << "Found 'include' in string." << endl;
		return true;
	}
	return false;
}

bool StaticSingleAssignment::isBuiltinVar(const VarName& var)
{
	//Test if the variable is compiler generated.
	if (var[0]->get_name().getString().find("__") == 0)
	{
		//We are compiler generated, return true.
		return true;
	}
	return false;
}

bool StaticSingleAssignment::isPrefixOfName(VarName name, VarName prefix)
{
	VarName::iterator iter;
	// Search for the first occurance of prefix in name
	iter = search(name.begin(), name.end(), prefix.begin(), prefix.end());

	//If the prefix is at the beginning of the name
	if (iter == name.begin())
	{
		return true;
	}
		//If the prefix is not in the name
	else if (iter == name.end())
	{
		return false;
	}
		//Found the prefix, but inside the name instead of at beginning
	else
	{
		//We don't want to assert, just return false.
		return false;
	}
}

//Function to perform the StaticSingleAssignment and annotate the AST
void StaticSingleAssignment::run()
{
	originalDefTable.clear();
	expandedDefTable.clear();
	defTable.clear();
	useTable.clear();
	firstDefList.clear();
	nodeRenameTable.clear();
	numRenameTable.clear();
	globalVarList.clear();

	if (getDebug())
		cout << "Locating global variables." << endl;

	findGlobalVars();
	//Insert the global variables as being defined at every function call
	insertGlobalVarDefinitions();

	UniqueNameTraversal uniqueTrav;
	DefsAndUsesTraversal defUseTrav(this);

	vector<SgFunctionDefinition*> funcs = SageInterface::querySubTree<SgFunctionDefinition > (project, V_SgFunctionDefinition);
	foreach (SgFunctionDefinition* func, funcs)
	{
		ROSE_ASSERT(func);
		if (!isFromLibrary(func))
		{
			if (getDebug())
				cout << "Running UniqueNameTraversal on function:" << SageInterface::get_name(func) << func << endl;

			uniqueTrav.traverse(func);

			if (getDebug())
				cout << "Finished UniqueNameTraversal..." << endl;

			if (getDebug())
				cout << "Running DefsAndUsesTraversal on function: " << SageInterface::get_name(func) << func << endl;

			defUseTrav.traverse(func);

			if (getDebug())
				cout << "Finished DefsAndUsesTraversal..." << endl;

			//Iterate the global table insert a def for each name at the function definition
			foreach(VarName globalVar, globalVarList)
			{
				//Add this function definition as a definition point of this variable
				originalDefTable[func][globalVar].push_back(func);
			}

			if (getDebug())
				cout << "Running DefUse Data Flow on function: " << SageInterface::get_name(func) << func << endl;
			runDefUse(func);
		}
	}
}

void StaticSingleAssignment::findGlobalVars()
{
	InitNameVec vars = SageInterface::querySubTree<SgInitializedName > (project, V_SgInitializedName);

	foreach(InitNameVec::value_type& iter, vars)
	{
		//Ignore library/compiler generated variables.
		if (isFromLibrary(iter))
			continue;

		//Check if we are in global scope.
		SgNode* scope = iter->get_scope();
		if (isSgGlobal(scope))
		{
			//Since forward declaration parameters are inserted in global scope,
			//Check if we are in a forward declaration
			if (SageInterface::getEnclosingFunctionDeclaration(iter))
			{
				//We are in a declaration, so not a global var.
				continue;
			}
			//Add the variable to the global scope and name it.
			VarUniqueName *uName = new VarUniqueName(iter);
			iter->setAttribute(StaticSingleAssignment::varKeyTag, uName);
			//Add to the global var list
			globalVarList.push_back(uName->getKey());
			if (getDebug())
				cout << "Added global variable [" << iter->get_name().getString() << "] - " << iter << endl;
		}
	}
}

void StaticSingleAssignment::insertGlobalVarDefinitions()
{
	if (getDebug())
		cout << "Global Var List size: " << globalVarList.size() << endl;

	//Iterate the function calls and insert definitions for all global variables
	vector<SgFunctionCallExp*> calls = SageInterface::querySubTree<SgFunctionCallExp > (project, V_SgFunctionCallExp);

	foreach(vector<SgFunctionCallExp*>::value_type& iter, calls)
	{
		SgFunctionCallExp* call = iter;
		ROSE_ASSERT(call);

		//Iterate the global table insert a def for each name at the function call
		foreach(GlobalTable::value_type& entry, globalVarList)
		{
			//Add this function call as a definition point of this variable
			originalDefTable[call][entry].push_back(call);
		}
	}
}

void StaticSingleAssignment::runDefUse(SgFunctionDefinition* func)
{
	//Keep track of visited nodes
	boost::unordered_set<SgNode*> visited;

	//Reset the first def list to prevent errors with global vars.
	firstDefList.clear();

	cfgNodeVec worklist;

	cfgNode current = cfgNode(func->cfgForBeginning());
	worklist.push_back(current);

	while (!worklist.empty())
	{
		if (getDebug())
			cout << "-------------------------------------------------------------------------" << endl;
		//Get the node to work on
		current = worklist.front();
		worklist.erase(worklist.begin());

		//We don't want to do def_use on the ending CFGNode of the function definition
		//so if we see it, continue.
		//If we do this, then incorrect information will be propogated to the beginning of the function
		if (current == cfgNode(func->cfgForEnd()))
		{
			if (getDebug())
				cout << "Skipped defUse on End of function definition." << endl;
			continue;
		}

		bool memberRefInserted = false;
		NodeVec changedNodes;
		bool changed = defUse(current, &memberRefInserted, changedNodes);

		//If memberRefs were inserted, then there are nodes previous to this one that are different.
		//Thus, we need to add those nodes to the working list
		if (memberRefInserted)
		{
			//Clear the worklist and visited list
			worklist.clear();
			visited.clear();

			//Insert each changed node into the list

			foreach(SgNode* chNode, changedNodes)
			{
				//Get the cfg node for this node
				cfgNode nextNode = cfgNode(chNode->cfgForBeginning());
				//Only insert the node in the worklist if it isn't there already.
				if (find(worklist.begin(), worklist.end(), nextNode) == worklist.end())
				{
					worklist.push_back(nextNode);
					if (getDebug())
						cout << "Member Ref Inserted: Added " << nextNode.getNode()->class_name() << nextNode.getNode() << " to the worklist." << endl;
				}
			}

			//Restart work from where the new def was inserted.
			continue;
		}

		//Get the outgoing edges
		cfgEdgeVec outEdges = current.outEdges();

		//For every edge, add it to the worklist if it is not seen or something has changed

		foreach(cfgEdgeVec::value_type& edge, outEdges)
		{
			cfgNode nextNode = edge.target();

			//Only insert the node in the worklist if it isn't there already.
			if (find(worklist.begin(), worklist.end(), nextNode) == worklist.end())
			{
				if (changed)
				{
					//Add the node to the worklist
					worklist.push_back(nextNode);
					if (getDebug())
						cout << "Defs Changed: Added " << nextNode.getNode()->class_name() << nextNode.getNode() << " to the worklist." << endl;
				}
					//If the next node has not yet been visited
				else if (visited.count(nextNode.getNode()) == 0)
				{
					//Add it to the worklist
					worklist.push_back(nextNode);
					if (getDebug())
						cout << "Next unvisited: Added " << nextNode.getNode()->class_name() << nextNode.getNode() << " to the worklist." << endl;
				}
			}

		}

		//Mark the current node as seen
		visited.insert(current.getNode());
	}
}

bool StaticSingleAssignment::defUse(FilteredCFGNode<IsDefUseFilter> node, bool *memberRefInserted, NodeVec &changedNodes)
{
	SgNode* current = node.getNode();

	//Handle each type of node
	if (getDebug())
		cout << "Performing DefUse on " << current->class_name() << ":" << current << endl;

	bool defChanged = false;
	bool defRefInserted = false;
	bool useChanged = false;
	bool useRefInserted = false;
	defChanged = mergeDefs(node, &defRefInserted);
	useChanged = resolveUses(node, &useRefInserted, changedNodes);

	*memberRefInserted = useRefInserted;


	if (getDebug())
		cout << "Defs were " << ((defChanged) ? "changed." : "same.") << endl;

	return defChanged;
}

bool StaticSingleAssignment::mergeDefs(cfgNode curNode, bool *memberRefInserted)
{
	SgNode* node = curNode.getNode();

	bool changed = false;

	if (getDebug())
	{
		cout << "merging defs..." << endl;
		printDefs(node);
	}

	//We have the definitions stored as follows:
	//defTable contans the definitions as propogated by the CFG analysis
	//originalDefTable contains the original definitions that are part of the
	//given statement.

	//When we want to propogate the defs from the previous node(s) to this one,
	//We perform a few steps. This is dependent on the number of incoming edges.

	/*1 Edge: When we have linear control flow, we do the following:
	 *        1. Copy the definitions from the previous node wholesale to a staging table.
	 *        2. Copy in the original definitions from the current node, overwriting those
	 *           from the previous node.
	 *        3. Compare the staging and current tables, and only overwrite if needed.
	 */

	/*2+ Edges: When we have branched control flow, we do the following:
	 *       1. Copy the definitions from the previous node(s) wholesale to a staging table.
	 *          Be careful to not insert duplicates.
	 *       2. Copy in the original definitions from the current node, overwriting those
	 *          from the previous node(s).
	 *       3. Compare the staging and current tables, and only overwrite if needed.
	 */

	//Expand any member variable references at the current node.
	*memberRefInserted = expandMemberDefinitions(curNode);

	TableEntry propDefs;
	//Retrieve the defs coming from previous cfgNodes
	aggregatePreviousDefs(curNode, propDefs);

	//Replace every entry in staging table that has definition in original defs
	//Also assign renaming numbers to any new definitions

	foreach(TableEntry::value_type& entry, originalDefTable[node])
	{
		//Replace the entry for this variable with the definitions at this node.
		propDefs[entry.first] = entry.second;

		//Now, iterate the definition vector for this node

		foreach(NodeVec::value_type& defNode, entry.second)
		{
			//Assign a number to each new definition. The function will prevent duplicates
			addRenameNumberForNode(entry.first, defNode);
		}
	}

	//For every originalDef, insert expanded defs for any propagated defs
	//that have an originalDef as a prefix
	VarName expVar;

	foreach(TableEntry::value_type& entry, originalDefTable[node])
	{

		foreach(TableEntry::value_type& propEntry, propDefs)
		{
			//Don't insert a def if it is already originally defined.
			if (originalDefTable[node].count(propEntry.first) != 0)
			{
				continue;
			}
			//If the original def is a prefix of the propogated def, add a def at this node
			//Compare sizes to guard against inserting original def in expanded table
			if (isPrefixOfName(propEntry.first, entry.first) && (propEntry.first.size() > entry.first.size()))
			{
				//Set this node as a definition point of the variable.
				expandedDefTable[node][propEntry.first].assign(1, node);
				*memberRefInserted = true;
				if (getDebugExtra())
				{
					cout << "Inserted expandedDef for [" << keyToString(propEntry.first) << "] with originalDef prefix [" << keyToString(entry.first) << "]" << endl;
				}
			}
		}
	}

	//Replace every entry in staging table that has definition in expandedDefs
	//Also assign renaming numbers to any new definitions

	foreach(TableEntry::value_type& entry, expandedDefTable[node])
	{
		propDefs[entry.first] = entry.second;

		//Now, iterate the definition vector for this node

		foreach(NodeVec::value_type& defNode, entry.second)
		{
			//Assign a number to each new definition. The function will prevent duplicates
			addRenameNumberForNode(entry.first, defNode);
		}
	}

	//If there is an initial definition of a name at this node, we should insert it in the table

	foreach(TableEntry::value_type& entry, originalDefTable[node])
	{
		//If the given variable name is not present in the first def table
		if (firstDefList.count(entry.first) == 0)
		{
			//Set this node as the first definition point of this variable.
			firstDefList[entry.first] = node;
		}
	}

	if (getDebugExtra())
	{
		cout << "Local Defs replaced in propDefs ";
		printDefs(propDefs);
	}

	//Now do a comparison to see if we should copy
	if (propDefs != defTable[node])
	{
		defTable[node] = propDefs;
		changed = true;
	}
	else
	{
		changed = false;
	}

	if (getDebug())
	{
		cout << "Defs after Merge..." << endl;
		printDefs(node);
	}

	if (getDebug())
	{
		printRenameTable();
	}

	return changed;
}

void StaticSingleAssignment::aggregatePreviousDefs(cfgNode curNode, TableEntry& results)
{
	//SgNode* node = curNode.getNode();

	//Get the previous edges in the CFG for this node
	cfgEdgeVec inEdges = curNode.inEdges();

	if (inEdges.size() == 1)
	{
		SgNode* prev = inEdges[0].source().getNode();

		/*
		if(getDebug())
		{
			cout << "Merging defs from " << prev->class_name() << prev << " to " << node->class_name() << node << endl;
			printDefs(prev);
		}*/

		//Copy the previous node defs to the staging table
		results = defTable[prev];
	}
	else if (inEdges.size() > 1)
	{
		//Iterate all of the incoming edges
		for (unsigned int i = 0; i < inEdges.size(); i++)
		{
			SgNode* prev = inEdges[i].source().getNode();

			/*
			if(getDebug())
			{
				cout << "Merging defs from " << prev->class_name() << prev << endl;
				printDefs(prev);
			}*/

			//Perform the union of all the infoming definitions.

			foreach(TableEntry::value_type& entry, defTable[prev])
			{
				//Insert the definitions for this node at the end of the list
				results[entry.first].insert(results[entry.first].end(), entry.second.begin(), entry.second.end());
			}
		}
	}

	//Sort every vector in propDefs and remove duplicates

	foreach(TableEntry::value_type& entry, results)
	{
		sort(entry.second.begin(), entry.second.end());
		//Create new sequence of unique elements and remove duplicate ones
		entry.second.resize(unique(entry.second.begin(), entry.second.end()) - entry.second.begin());
	}

	/*
	if(getDebugExtra())
	{
		cout << "Merged propDefs ";
		printDefs(results);
	}*/

	return;
}

bool StaticSingleAssignment::expandMemberDefinitions(cfgNode curNode)
{
	SgNode* node = curNode.getNode();

	bool changed = false;

	if (getDebugExtra())
	{
		cout << "Expanding member defs at " << node->class_name() << node << endl;
		cout << "Original Node ";
		printDefs(originalDefTable[node]);
	}

	//We want to iterate the vars defined on this node, and expand them

	foreach(TableEntry::value_type& entry, originalDefTable[node])
	{
		if (getDebugExtra())
		{
			cout << "Checking [" << keyToString(entry.first) << "]" << endl;
		}

		//Check if the variableName has multiple parts
		if (entry.first.size() == 1)
		{
			continue;
		}

		//We are dealing with a multi-part variable, loop the entry and expand it
		//Start at one so we don't get the same defs in the original and expanded defs
		for (unsigned int i = 1; i < entry.first.size(); i++)
		{
			//Create a new varName vector that goes from beginning to end - i
			VarName newName;
			newName.assign(entry.first.begin(), entry.first.end() - i);

			if (getDebugExtra())
			{
				cout << "Testing for presence of [" << keyToString(newName) << "]" << endl;
			}

			//Only insert the new definition if it does not already exist
			if (originalDefTable[node].count(newName) == 0 && expandedDefTable[node].count(newName) == 0)
			{
				//Insert the new name as being defined here.
				expandedDefTable[node][newName] = NodeVec(1, node);
				changed = true;

				if (getDebugExtra())
				{
					cout << "Inserted new name [" << keyToString(newName) << "] into defs." << endl;
				}
			}
		}
	}

	if (getDebugExtra())
	{
		cout << "Expanded Node";
		printDefs(expandedDefTable[node]);
	}

	return changed;
}

bool StaticSingleAssignment::resolveUses(FilteredCFGNode<IsDefUseFilter> curNode, bool *memberRefInserted, NodeVec &changedNodes)
{
	SgNode* node = curNode.getNode();

	bool changed = false;

	//We want to resolve the uses at the current node
	//We need to look to the defs at the current node, and match them
	//with the uses


	if (getDebug())
		cout << "Resolving uses at " << node->class_name() << node << endl;

	changed = expandMemberUses(curNode);

	//Iterate every use at the current node

	foreach(TableEntry::value_type& entry, useTable[node])
	{
		//Check the defs that are active at the current node to find the reaching definition
		//We want to check if there is a definition entry for this use at the current node
		if (defTable[node].find(entry.first) != defTable[node].end())
		{
			//There is a definition entry. Now we want to see if the use is already up to date
			if (useTable[node][entry.first] != defTable[node][entry.first])
			{
				//The use was not up to date, so we update it
				changed = true;
				//Overwrite the use with this definition location(s).
				useTable[node][entry.first] = defTable[node][entry.first];
			}
		}
		else
		{
			//If there are no defs for this use at this node, then we have a multi-part name
			//that has not been expanded. Thus, we want to expand it.
			*memberRefInserted = insertExpandedDefsForUse(curNode, entry.first, changedNodes);
		}
	}

	TableEntry results;
	//Get the previous defs
	aggregatePreviousDefs(curNode, results);

	//However, if there is a def at the current node, we want to use the previous
	//def as the use for this node.

	//Iterate every use at the current node

	foreach(TableEntry::value_type& entry, useTable[node])
	{
		//If any of these uses are for a variable defined at this node, we will
		//set the flag and correct it later.
		if (originalDefTable[node].count(entry.first) != 0)
		{
			useTable[node][entry.first] = results[entry.first];

			if (getDebug())
				cout << "Fixed use of local def." << endl;
		}
	}

	return changed;
}

bool StaticSingleAssignment::expandMemberUses(cfgNode curNode)
{
	SgNode* node = curNode.getNode();

	bool changed = false;

	if (getDebugExtra())
	{
		cout << "Expanding member uses at " << node->class_name() << node << endl;
		cout << "Original Node ";
		printUses(useTable[node]);
	}

	//We want to iterate the vars used on this node, and expand them

	foreach(TableEntry::value_type& entry, useTable[node])
	{
		if (getDebugExtra())
		{
			cout << "Checking [" << keyToString(entry.first) << "]" << endl;
		}

		//Check if the variableName has multiple parts
		if (entry.first.size() == 1)
		{
			continue;
		}

		//We are dealing with a multi-part variable, loop the entry and expand it
		//Start at one so we don't reinsert same use
		for (unsigned int i = 1; i < entry.first.size(); i++)
		{
			//Create a new varName vector that goes from beginning to end - i
			VarName newName;
			newName.assign(entry.first.begin(), entry.first.end() - i);

			if (getDebugExtra())
			{
				cout << "Testing for presence of [" << keyToString(newName) << "]" << endl;
			}

			//Only insert the new definition if it does not already exist
			if (useTable[node].count(newName) == 0)
			{
				//Insert the new name as being used here.
				useTable[node][newName] = NodeVec(1, node);
				changed = true;

				if (getDebugExtra())
				{
					cout << "Inserted new name [" << keyToString(newName) << "] into uses." << endl;
				}
			}
		}
	}

	if (getDebugExtra())
	{
		cout << "Expanded Node ";
		printUses(useTable[node]);
	}

	return changed;
}

bool StaticSingleAssignment::insertExpandedDefsForUse(cfgNode curNode, VarName name, NodeVec &changedNodes)
{
	SgNode* node = curNode.getNode();

	bool changed = false;

	if (getDebugExtra())
	{
		cout << "Checking for needed extra defs for uses at " << node->class_name() << node << endl;
		cout << "Checking for [" << keyToString(name) << "]" << endl;
	}

	//Check if the given name has a def at this node
	if (defTable[node].count(name) != 0)
	{
		if (getDebugExtra())
			cout << "Already have def." << endl;

		//If there is already a def, then nothing changes
		return false;
	}

	//No def for this name at this node, so we need to insert a def at the location
	//where the first part of this name was defined.
	//eg. s.a.b = x; (insert definition of s.a & s.a.b where s is first defined.)

	//Get the root of this name
	VarName rootName;
	rootName.assign(1, name[0]);

	//We want to see if the name is a class member (no def so far)
	if (firstDefList.count(rootName) == 0)
	{
		//Check if the variable is a compiler builtin
		if (isBuiltinVar(rootName))
		{
			//Add a definition at the start of the function
			SgFunctionDefinition *func = SageInterface::getEnclosingFunctionDefinition(node);
			ROSE_ASSERT(func);

			firstDefList[rootName] = func;
		}
			//Check if the variable is declared in a class scope
		else if (isSgClassDefinition(SageInterface::getScope(rootName[0])) != NULL)
		{
			//It is declared in class scope.
			//Get our enclosing function definition to insert the first definition into.

			SgFunctionDefinition *func = SageInterface::getEnclosingFunctionDefinition(node);
			ROSE_ASSERT(func);

			firstDefList[rootName] = func;
		}
			//Otherwise, see if it is in namespace scope
		else if (isSgNamespaceDefinitionStatement(SageInterface::getScope(rootName[0])) != NULL)
		{
			//It is declared in namespace scope.
			//Get our enclosing function definition to insert the first definition into.

			SgFunctionDefinition *func = SageInterface::getEnclosingFunctionDefinition(node);
			ROSE_ASSERT(func);

			firstDefList[rootName] = func;
		}
		else
		{
			cout << "Error: Found variable with no firstDef point that is not a class or namespace member." << endl;
			cout << "Variable Scope: " << SageInterface::getScope(rootName[0])->class_name() << SageInterface::getScope(rootName[0]) << endl;
			cout << rootName[0]->class_name() << rootName[0] << "@" << rootName[0]->get_file_info()->get_line() << ":" << rootName[0]->get_file_info()->get_col() << endl;
			ROSE_ASSERT(false);
		}
	}

	//Start from the end of the name and insert definitions of every part
	//at the first definition point
	for (int i = 0; i < (signed int) name.size(); i++)
	{
		//Create a new varName vector that goes from beginning to end - i
		VarName newName;
		newName.assign(name.begin(), name.end() - i);

		if (getDebugExtra())
		{
			cout << "Testing for def of [" << keyToString(newName) << "] at var initial def." << endl;
		}

		if (originalDefTable[firstDefList[rootName]].count(newName) == 0)
		{
			originalDefTable[firstDefList[rootName]][newName].push_back(firstDefList[rootName]);
			changed = true;
			changedNodes.push_back(firstDefList[rootName]);
			if (getDebugExtra())
			{
				cout << "Inserted def for [" << keyToString(newName) << "] (root) [" << keyToString(rootName) << "] at node " << firstDefList[rootName] << endl;
			}
		}
	}

	if (getDebugExtra())
		cout << "Finished inserting references. Changed: " << ((changed) ? "true" : "false") << endl;

	return changed;
}

int StaticSingleAssignment::getRenameNumberForNode(const VarName& var, SgNode* node) const
{
	ROSE_ASSERT(node);

	NodeNumRenameTable::const_iterator iter;
	iter = nodeRenameTable.find(var);

	if (iter == nodeRenameTable.end())
	{
		return -1;
	}
	else
	{
		//Try and get the number for the node
		NodeNumRenameEntry::const_iterator iter2;
		iter2 = (*iter).second.find(node);
		if (iter2 != (*iter).second.end())
		{
			return (*iter2).second;
		}
			//Node not in table
		else
		{
			return -1;
		}
	}
}

SgNode* StaticSingleAssignment::getNodeForRenameNumber(const VarName& var, int num) const
{
	ROSE_ASSERT(num > 0);

	NumNodeRenameTable::const_iterator iter;
	iter = numRenameTable.find(var);
	if (iter == numRenameTable.end())
	{
		return NULL;
	}
	else
	{
		//Try and get the node for the number
		NumNodeRenameEntry::const_iterator iter2;
		iter2 = (*iter).second.find(num);
		if (iter2 != (*iter).second.end())
		{
			return (*iter2).second;
		}
			//Number not in table
		else
		{
			return NULL;
		}
	}
}

int StaticSingleAssignment::getMaxRenameNumberForName(const VarName& var) const
{
	int res = -1;

	NumNodeRenameTable::const_iterator iter;
	iter = numRenameTable.find(var);
	if (iter == numRenameTable.end())
	{
		return res;
	}

	NumNodeRenameEntry::const_iterator iter2;
	for (iter2 = (*iter).second.begin(); iter2 != (*iter).second.end(); ++iter2)
	{
		if ((*iter2).first > res)
		{
			res = (*iter2).first;
		}
	}

	return res;
}

int StaticSingleAssignment::addRenameNumberForNode(const VarName& var, SgNode* node)
{
	ROSE_ASSERT(node);

	//Check if the var/node combination is already in the table.
	if (nodeRenameTable[var].count(node) == 1)
	{
		return nodeRenameTable[var][node];
	}

	//Not in the table, so we now need to add it.
	int nextNum = nodeRenameTable[var].size() + 1;

	nodeRenameTable[var][node] = nextNum;
	numRenameTable[var][nextNum] = node;

	if (getDebug())
		cout << "Renaming Added:[" << keyToString(var) << "]:" << nextNum << " - " << node << endl;

	return nextNum;
}

StaticSingleAssignment::NodeVec StaticSingleAssignment::getAllUsesForDef(const VarName& var, int num)
{
	NodeVec res;
	SgNode* defNode = getNodeForRenameNumber(var, num);

	if (defNode == NULL)
	{
		res.clear();
		return res;
	}

	//Traverse the use Table looking for locations where the def is used

	foreach(DefUseTable::value_type& entry, useTable)
	{
		//If this entry contains the variable that we want
		if (entry.second.count(var) != 0)
		{
			//See if the use vector contains the defining node we want
			if (find(entry.second[var].begin(), entry.second[var].end(), defNode) != entry.second[var].end())
			{
				//Add the current node to list of those using the variable
				res.push_back(entry.first);
			}
		}
	}

	return res;
}

StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::getReachingDefsAtNode(SgNode* node)
{
	//We want to get all the reaching defs at this node and insert them into the result table

	NumNodeRenameTable res;

	//Iterate every variable definition reaching this node

	foreach(TableEntry::value_type& entry, defTable[node])
	{
		//Iterate every definition site for this variable

		foreach(NodeVec::value_type& defEntry, entry.second)
		{
			//Get the rename number for the current variable at the current def site
			int renameNum = getRenameNumberForNode(entry.first, defEntry);

			//If the name is defined at this node
			if (renameNum > 0)
			{
				//If the renumbering is not already in the result
				if (res[entry.first].count(renameNum) == 0)
				{
					//Add the renumbering to the result
					res[entry.first][renameNum] = defEntry;
				}
				else
				{
					cout << "Error: Same def propogated twice to same node." << endl;
					ROSE_ASSERT(false);
				}
			}
			else
			{
				cout << "Error: Found propogated def with no entry in rename table." << endl;
				ROSE_ASSERT(false);
			}
		}
	}

	return res;
}

StaticSingleAssignment::NumNodeRenameEntry StaticSingleAssignment::getReachingDefsAtNodeForName(SgNode* node, const VarName& var)
{
	//We want to get all the reaching defs at this node and insert them into the result table

	NumNodeRenameEntry res;

	//Iterate every variable definition reaching this node

	foreach(TableEntry::value_type& entry, defTable[node])
	{
		//Check that the current var is the one we want.
		if (entry.first != var)
		{
			continue;
		}

		//Iterate every definition site for this variable

		foreach(NodeVec::value_type& defEntry, entry.second)
		{
			//Get the rename number for the current variable at the current def site
			int renameNum = getRenameNumberForNode(entry.first, defEntry);

			//If the name is defined at this node
			if (renameNum > 0)
			{
				//If the renumbering is not already in the result
				if (res.count(renameNum) == 0)
				{
					//Add the renumbering to the result
					res[renameNum] = defEntry;
				}
				else
				{
					cout << "Error: Same def propogated twice to same node." << endl;
					ROSE_ASSERT(false);
				}
			}
			else
			{
				cout << "Error: Found propogated def with no entry in rename table." << endl;
				ROSE_ASSERT(false);
			}
		}
	}

	return res;
}

StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::getUsesAtNode(SgNode* node)
{
	//We want to get all the uses at this node and insert them into the result table

	NumNodeRenameTable res;

	//Iterate every variable definition used at this node

	foreach(TableEntry::value_type& entry, useTable[node])
	{
		//Iterate every definition site for this variable

		foreach(NodeVec::value_type& defEntry, entry.second)
		{
			//Get the rename number for the current variable at the current def site
			int renameNum = getRenameNumberForNode(entry.first, defEntry);

			//If the name is defined at this node
			if (renameNum > 0)
			{
				//If the renumbering is not already in the result
				if (res[entry.first].count(renameNum) == 0)
				{
					//Add the renumbering to the result
					res[entry.first][renameNum] = defEntry;
				}
				else
				{
					cout << "Error: Same use propogated twice to same node." << endl;
					ROSE_ASSERT(false);
				}
			}
			else
			{
				/* This situation can happen in certain cases, so we don;t want to assert.
				 *
				 * ex. for(int i = 0; i < 10; i++)
				 *     {
				 *        return i;
				 *     }
				 *
				 * The varRef for i++ will not have its uses corrected, and so will
				 * be using itself. This is not technically wrong, since control will
				 * never reach that varRef.
				 */
				if (getDebug())
				{
					cout << "Warning: Found use with no entry in rename table." << endl;
				}
			}
		}
	}

	return res;
}

StaticSingleAssignment::NumNodeRenameEntry StaticSingleAssignment::getUsesAtNodeForName(SgNode* node, const VarName& var)
{
	//We want to get all the uses at this node and insert them into the result table

	NumNodeRenameEntry res;

	//Iterate every variable use at this node

	foreach(TableEntry::value_type& entry, useTable[node])
	{
		//Check that the current var is the one we want.
		if (entry.first != var)
		{
			continue;
		}

		//Iterate every definition site for this variable

		foreach(NodeVec::value_type& defEntry, entry.second)
		{
			//Get the rename number for the current variable at the current def site
			int renameNum = getRenameNumberForNode(entry.first, defEntry);

			//If the name is defined at this node
			if (renameNum > 0)
			{
				//If the renumbering is not already in the result
				if (res.count(renameNum) == 0)
				{
					//Add the renumbering to the result
					res[renameNum] = defEntry;
				}
				else
				{
					cout << "Error: Same use propogated twice to same node." << endl;
					ROSE_ASSERT(false);
				}
			}
			else
			{
				/* This situation can happen in certain cases, so we don't want to assert.
				 *
				 * ex. for(int i = 0; i < 10; i++)
				 *     {
				 *        return i;
				 *     }
				 *
				 * The varRef for i++ will not have its uses corrected, and so will
				 * be using itself. This is not technically wrong, since control will
				 * never reach that varRef.
				 */
				if (getDebug())
				{
					cout << "Warning: Found use with no entry in rename table." << endl;
				}
			}
		}
	}

	return res;
}

StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::getOriginalUsesAtNode(SgNode* node)
{
	//The original variables are always attached to higher levels in the AST. For example,
	//if we have p.x, the dot expression has the varname p.x attached to it, while its left
	//child has the varname p. Hence, if we get the top nodes in the AST that are variables, we'll have all the
	//original (not exanded) variables used in the AST.

	class FindOriginalVariables : public AstTopDownProcessing<bool>
	{
	public:
		set<StaticSingleAssignment::VarName> originalVariablesUsed;

		virtual bool evaluateInheritedAttribute(SgNode* node, bool isParentVariable)
		{
			if (isParentVariable)
			{
				return true;
			}

			if (StaticSingleAssignment::getVarName(node) != StaticSingleAssignment::emptyName)
			{
				originalVariablesUsed.insert(StaticSingleAssignment::getVarName(node));
				return true;
			}
			else
			{
				return false;
			}
		}
	};
	FindOriginalVariables originalVariableUsesTraversal;
	originalVariableUsesTraversal.traverse(node, false);

	StaticSingleAssignment::NumNodeRenameTable result;

	foreach(StaticSingleAssignment::VarName varName, originalVariableUsesTraversal.originalVariablesUsed)
	{
		ROSE_ASSERT(result.count(varName) == 0);
		StaticSingleAssignment::NumNodeRenameEntry varDefs = getUsesAtNodeForName(node, varName);
		result[varName] = varDefs;
	}

	return result;
}

StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::getDefsAtNode(SgNode* node)
{
	NumNodeRenameTable original = getOriginalDefsAtNode(node);
	NumNodeRenameTable expanded = getExpandedDefsAtNode(node);

	//Loop the expanded table and insert it into the original table

	foreach(NumNodeRenameTable::value_type& entry, expanded)
	{
		//Insert the entry wholesale
		if (original.count(entry.first) == 0)
		{
			original[entry.first] = entry.second;
		}
			//Or merge it with an existing one
		else
		{

			foreach(NumNodeRenameEntry::value_type& tableEntry, entry.second)
			{
				//Insert the entry wholesale
				if (original[entry.first].count(tableEntry.first) == 0)
				{
					original[entry.first][tableEntry.first] = tableEntry.second;
				}
				else
				{
					cout << "Error: Same renaming present in original and expanded defs." << endl;
					cout << "At node " << node << endl;
					cout << "Original ";
					printRenameTable(original);
					cout << "Expanded ";
					printRenameTable(expanded);
					ROSE_ASSERT(false);
				}
			}
		}
	}

	return original;
}

StaticSingleAssignment::NumNodeRenameEntry StaticSingleAssignment::getDefsAtNodeForName(SgNode* node, const VarName& var)
{
	NumNodeRenameEntry original = getOriginalDefsAtNodeForName(node, var);
	NumNodeRenameEntry expanded = getExpandedDefsAtNodeForName(node, var);

	//Loop the expanded table and insert it into the original table

	foreach(NumNodeRenameEntry::value_type& tableEntry, expanded)
	{
		//Insert the entry wholesale
		if (original.count(tableEntry.first) == 0)
		{
			original[tableEntry.first] = tableEntry.second;
		}
		else
		{
			cout << "Error: Same renaming present in original and expanded defs." << endl;
			ROSE_ASSERT(false);
		}
	}

	return original;
}

StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::getOriginalDefsAtNode(SgNode* node)
{
	//We want to get all the original defs at this node and insert them into the result table

	NumNodeRenameTable res;

	//Iterate every variable definition reaching this node

	foreach(TableEntry::value_type& entry, originalDefTable[node])
	{
		//Iterate every definition site for this variable

		foreach(NodeVec::value_type& defEntry, entry.second)
		{
			//Get the rename number for the current variable at the current def site
			int renameNum = getRenameNumberForNode(entry.first, defEntry);

			//If the name is defined at this node
			if (renameNum > 0)
			{
				//If the renumbering is not already in the result
				if (res[entry.first].count(renameNum) == 0)
				{
					//Add the renumbering to the result
					res[entry.first][renameNum] = defEntry;
				}
				else
				{
					cout << "Error: Same def originally defined twice on same node." << endl;
					ROSE_ASSERT(false);
				}
			}
			else
			{
				/* This situation can happen in certain cases, so we don't want to assert.
				 *
				 * ex. for(int i = 0; i < 10; i++)
				 *     {
				 *        return i;
				 *     }
				 *
				 * The i++ will have an original def for i. However, it will not be in the rename
				 * table for i. This is not technically wrong, since control will never
				 * reach that i++, so it will not have an ordering wrt. the other definitions.
				 */
				if (getDebug())
				{
					cout << "Warning: Found original def with no entry in rename table." << endl;
				}
			}
		}
	}

	return res;
}

StaticSingleAssignment::NumNodeRenameEntry StaticSingleAssignment::getOriginalDefsAtNodeForName(SgNode* node, const VarName& var)
{
	//We want to get all the original defs at this node and insert them into the result table

	NumNodeRenameEntry res;

	//Iterate every variable use at this node

	foreach(TableEntry::value_type& entry, originalDefTable[node])
	{
		//Check that the current var is the one we want.
		if (entry.first != var)
		{
			continue;
		}

		//Iterate every definition site for this variable

		foreach(NodeVec::value_type& defEntry, entry.second)
		{
			//Get the rename number for the current variable at the current def site
			int renameNum = getRenameNumberForNode(entry.first, defEntry);

			//If the name is defined at this node
			if (renameNum > 0)
			{
				//If the renumbering is not already in the result
				if (res.count(renameNum) == 0)
				{
					//Add the renumbering to the result
					res[renameNum] = defEntry;
				}
				else
				{
					cout << "Error: Same original Def twice to same node." << endl;
					ROSE_ASSERT(false);
				}
			}
			else
			{
				/* This situation can happen in certain cases, so we don't want to assert.
				 *
				 * ex. for(int i = 0; i < 10; i++)
				 *     {
				 *        return i;
				 *     }
				 *
				 * The i++ will have an original def for i. However, it will not be in the rename
				 * table for i. This is not technically wrong, since control will never
				 * reach that i++, so it will not have an ordering wrt. the other definitions.
				 */
				if (getDebug())
				{
					cout << "Warning: Found original def with no entry in rename table." << endl;
				}
			}
		}
	}

	return res;
}

StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::getExpandedDefsAtNode(SgNode* node)
{
	//We want to get all the expanded defs at this node and insert them into the result table

	NumNodeRenameTable res;

	//Iterate every variable definition expanded on this node

	foreach(TableEntry::value_type& entry, expandedDefTable[node])
	{
		//Iterate every definition site for this variable

		foreach(NodeVec::value_type& defEntry, entry.second)
		{
			//Get the rename number for the current variable at the current def site
			int renameNum = getRenameNumberForNode(entry.first, defEntry);

			//If the name is defined at this node
			if (renameNum > 0)
			{
				//If the renumbering is not already in the result
				if (res[entry.first].count(renameNum) == 0)
				{
					//Add the renumbering to the result
					res[entry.first][renameNum] = defEntry;
				}
				else
				{
					cout << "Error: Same def expanded twice on same node." << endl;
					ROSE_ASSERT(false);
				}
			}
			else
			{
				cout << "Error: Found expanded def with no entry in rename table." << endl;
				ROSE_ASSERT(false);
			}
		}
	}

	return res;
}

StaticSingleAssignment::NumNodeRenameEntry StaticSingleAssignment::getExpandedDefsAtNodeForName(SgNode* node, const VarName& var)
{
	//We want to get all the uses at this node and insert them into the result table

	NumNodeRenameEntry res;

	//Iterate every variable use at this node

	foreach(TableEntry::value_type& entry, expandedDefTable[node])
	{
		//Check that the current var is the one we want.
		if (entry.first != var)
		{
			continue;
		}

		//Iterate every definition site for this variable

		foreach(NodeVec::value_type& defEntry, entry.second)
		{
			//Get the rename number for the current variable at the current def site
			int renameNum = getRenameNumberForNode(entry.first, defEntry);

			//If the name is defined at this node
			if (renameNum > 0)
			{
				//If the renumbering is not already in the result
				if (res.count(renameNum) == 0)
				{
					//Add the renumbering to the result
					res[renameNum] = defEntry;
				}
				else
				{
					cout << "Error: Same expanded def twice to same node." << endl;
					ROSE_ASSERT(false);
				}
			}
			else
			{
				cout << "Error: Found expanded def with no entry in rename table." << endl;
				ROSE_ASSERT(false);
			}
		}
	}

	return res;
}

StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::getDefsForSubtree(SgNode* node)
{

	class DefSearchTraversal : public AstSimpleProcessing
	{
	public:
		StaticSingleAssignment::NumNodeRenameTable result;
		StaticSingleAssignment* varRenamingAnalysis;

		virtual void visit(SgNode* node)
		{
			//Look up defs at this particular node
			StaticSingleAssignment::NumNodeRenameTable defsAtNode = varRenamingAnalysis->getDefsAtNode(node);

			//Traverse the defs

			foreach(StaticSingleAssignment::NumNodeRenameTable::value_type& entry, defsAtNode)
			{
				//If this is the first time the var has been seen, add it wholesale
				if (result.count(entry.first) == 0)
				{
					result[entry.first] = entry.second;
					continue;
				}
				//Traverse each definition of the variable

				foreach(StaticSingleAssignment::NumNodeRenameEntry::value_type& tableEntry, entry.second)
				{
					if (result[entry.first].count(tableEntry.first) == 0)
					{
						result[entry.first][tableEntry.first] = tableEntry.second;
					}
					else
					{
						cout << "Error: Same rename number defined on multiple nodes." << endl;
						ROSE_ASSERT(false);
					}
				}
			}
		}
	};

	DefSearchTraversal traversal;
	traversal.varRenamingAnalysis = this;
	traversal.traverse(node, preorder);

	return traversal.result;
}

StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::getOriginalDefsForSubtree(SgNode* node)
{

	class DefSearchTraversal : public AstSimpleProcessing
	{
	public:
		StaticSingleAssignment::NumNodeRenameTable result;
		StaticSingleAssignment* varRenamingAnalysis;

		virtual void visit(SgNode* node)
		{
			//Look up defs at this particular node
			StaticSingleAssignment::NumNodeRenameTable defsAtNode = varRenamingAnalysis->getOriginalDefsAtNode(node);

			//Traverse the defs

			foreach(StaticSingleAssignment::NumNodeRenameTable::value_type& entry, defsAtNode)
			{
				//If this is the first time the var has been seen, add it wholesale
				if (result.count(entry.first) == 0)
				{
					result[entry.first] = entry.second;
					continue;
				}
				//Traverse each definition of the variable

				foreach(StaticSingleAssignment::NumNodeRenameEntry::value_type& tableEntry, entry.second)
				{
					if (result[entry.first].count(tableEntry.first) == 0)
					{
						result[entry.first][tableEntry.first] = tableEntry.second;
					}
					else
					{
						cout << "Error: Same rename number defined on multiple nodes." << endl;
						ROSE_ASSERT(false);
					}
				}
			}
		}
	};

	DefSearchTraversal traversal;
	traversal.varRenamingAnalysis = this;
	traversal.traverse(node, preorder);

	return traversal.result;
}

StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::getReachingDefsAtScopeEnd(SgScopeStatement* bb)
{
	if (isSgFunctionDefinition(bb))
	{
		bb = isSgFunctionDefinition(bb)->get_body();
	}

	ROSE_ASSERT(bb);
	NumNodeRenameTable result;

	//Keep track of visited nodes
	set<SgNode*> visited;

	queue<cfgNode> worklist;

	cfgNode current = cfgNode(bb->cfgForBeginning());
	worklist.push(current);

	while (!worklist.empty())
	{
		//Get the node to work on
		current = worklist.front();
		worklist.pop();

		SgNode* currentNode = current.getNode();
		visited.insert(currentNode);

		//Find if any of the children exit the basic block.
		cfgEdgeVec outEdges = current.outEdges();

		foreach(cfgEdge edge, outEdges)
		{
			SgNode* targetNode = edge.target().getNode();

			//Prevent infinite looping
			if (visited.count(targetNode) > 0)
			{
				continue;
			}

			if (!SageInterface::isAncestor(bb, targetNode))
			{
				//This edge leads outside the basic block! Gotta save the variable versions here
				NumNodeRenameTable temp = getReachingDefsAtNode(currentNode);

				//Merge the tables

				foreach(NumNodeRenameTable::value_type& entry, temp)
				{
					//Insert the entry wholesale
					if (result.count(entry.first) == 0)
					{
						result[entry.first] = entry.second;
					}
						//Or merge it with an existing one
					else
					{

						foreach(NumNodeRenameEntry::value_type& tableEntry, entry.second)
						{
							//Insert the entry wholesale
							if (result[entry.first].count(tableEntry.first) == 0)
							{
								result[entry.first][tableEntry.first] = tableEntry.second;
							}
							else
							{
								//Check for equivalence
								if (result[entry.first][tableEntry.first] != tableEntry.second)
								{
									cout << "Error: Same Renaming number has two different definition points." << endl;
									ROSE_ASSERT(false);
								}
							}
						}
					}
				}
			}
			else
			{
				//Still in the basic block, add this edge to the worklist
				worklist.push(edge.target());
			}
		}
	}

	return result;
}

StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::getReachingDefsAtFunctionEnd(SgFunctionDefinition* node)
{
	ROSE_ASSERT(node);
	NumNodeRenameTable result;

	cfgNode lastNode = cfgNode(node->cfgForEnd());
	cfgEdgeVec lastEdges = lastNode.inEdges();
	if (lastEdges.size() == 0)
	{
		cout << "Error: No incoming edges to end of function definition." << endl;
		ROSE_ASSERT(false);
	}
	else if (lastEdges.size() == 1)
	{
		return getReachingDefsAtNode(lastEdges[0].source().getNode());
	}
	else
	{
		//Iterate and merge each edge

		foreach(cfgEdgeVec::value_type& edge, lastEdges)
		{
			NumNodeRenameTable temp = getReachingDefsAtNode(edge.source().getNode());

			//Merge the tables

			foreach(NumNodeRenameTable::value_type& entry, temp)
			{
				//Insert the entry wholesale
				if (result.count(entry.first) == 0)
				{
					result[entry.first] = entry.second;
				}
					//Or merge it with an existing one
				else
				{

					foreach(NumNodeRenameEntry::value_type& tableEntry, entry.second)
					{
						//Insert the entry wholesale
						if (result[entry.first].count(tableEntry.first) == 0)
						{
							result[entry.first][tableEntry.first] = tableEntry.second;
						}
						else
						{
							//Check for equivalence
							if (result[entry.first][tableEntry.first] != tableEntry.second)
							{
								cout << "Error: Same Renaming number has two different definition points." << endl;
								ROSE_ASSERT(false);
							}
						}
					}
				}
			}
		}

		return result;
	}
}

StaticSingleAssignment::NumNodeRenameEntry StaticSingleAssignment::getReachingDefsAtFunctionEndForName(SgFunctionDefinition* node, const VarName& var)
{
	ROSE_ASSERT(node);
	NumNodeRenameEntry result;

	cfgNode lastNode = cfgNode(node->cfgForEnd());
	cfgEdgeVec lastEdges = lastNode.inEdges();
	if (lastEdges.size() == 0)
	{
		cout << "Error: No incoming edges to end of function definition." << endl;
		ROSE_ASSERT(false);
	}
	else if (lastEdges.size() == 1)
	{
		return getReachingDefsAtNodeForName(lastEdges[0].source().getNode(), var);
	}
	else
	{
		//Iterate and merge each edge

		foreach(cfgEdgeVec::value_type& edge, lastEdges)
		{
			NumNodeRenameEntry temp = getReachingDefsAtNodeForName(edge.source().getNode(), var);

			foreach(NumNodeRenameEntry::value_type& tableEntry, temp)
			{
				//Insert the entry wholesale
				if (result.count(tableEntry.first) == 0)
				{
					result[tableEntry.first] = tableEntry.second;
				}
				else
				{
					//Check for equivalence
					if (result[tableEntry.first] != tableEntry.second)
					{
						cout << "Error: Same Renaming number has two different definition points." << endl;
						ROSE_ASSERT(false);
					}
				}
			}
		}

		return result;
	}
}

StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::getReachingDefsAtStatementStart(SgStatement* statement)
{
	ROSE_ASSERT(statement);
	NumNodeRenameTable result;

	//Get all the incoming edges for the statement. These are UNFILTERED nodes, so we get the true beginning of the statement
	vector<CFGEdge> inEdges = statement->cfgForBeginning().inEdges();

	//Trace back from an unfiltered node to all the filtered nodes immediately leading to it
	queue <CFGNode> unfilteredNodes;
	set<CFGNode> visited;
	vector<CFGNode> filteredNodes;

	//Add all the unfiltered inEdges to the initial worklist

	foreach(CFGEdge inEdge, inEdges)
	{
		unfilteredNodes.push(inEdge.source());
	}

	while (!unfilteredNodes.empty())
	{
		CFGNode node = unfilteredNodes.front();
		unfilteredNodes.pop();
		visited.insert(node);

		//If we reached a filtered node, we're done
		IsDefUseFilter filter;
		if (filter(node))
		{
			filteredNodes.push_back(node);
		}
			//This node is unfiltered, explore its parents
		else
		{

			foreach(CFGEdge inEdge, node.inEdges())
			{
				CFGNode parentNode = inEdge.source();
				if (visited.count(parentNode) == 0)
				{
					unfilteredNodes.push(parentNode);
				}
			}
		}
	}

	//Merge in the defs from the reaching CFG nodes that are not children of the current statement

	foreach(CFGNode cfgNode, filteredNodes)
	{
		SgNode* sourceNode = cfgNode.getNode();
		if (statement == sourceNode || SageInterface::isAncestor(statement, sourceNode))
		{
			continue;
		}

		NumNodeRenameTable temp = getReachingDefsAtNode(sourceNode);

		//Merge the tables
		//TODO: Factor this out into a function

		foreach(NumNodeRenameTable::value_type& entry, temp)
		{
			//Insert the entry wholesale
			if (result.count(entry.first) == 0)
			{
				result[entry.first] = entry.second;
			}
				//Or merge it with an existing one
			else
			{

				foreach(NumNodeRenameEntry::value_type& tableEntry, entry.second)
				{
					//Insert the entry wholesale
					if (result[entry.first].count(tableEntry.first) == 0)
					{
						result[entry.first][tableEntry.first] = tableEntry.second;
					}
					else
					{
						//Check for equivalence
						if (result[entry.first][tableEntry.first] != tableEntry.second)
						{
							cout << "Error: Same Renaming number has two different definition points." << endl;
							ROSE_ASSERT(false);
						}
					}
				}
			}
		}
	}

	return result;
}

StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::getReachingDefsAtFunctionStart(SgFunctionDefinition* node)
{
	return getReachingDefsAtNode(node);
}

StaticSingleAssignment::NumNodeRenameEntry StaticSingleAssignment::getReachingDefsAtFunctionStartForName(SgFunctionDefinition* node, const VarName& var)
{
	return getReachingDefsAtNodeForName(node, var);
}

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
