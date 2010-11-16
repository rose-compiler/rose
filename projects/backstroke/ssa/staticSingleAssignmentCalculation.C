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
#define reverse_foreach BOOST_REVERSE_FOREACH

using namespace std;
using namespace ssa_private;

//Initializations of the static attribute tags
string StaticSingleAssignment::varKeyTag = "ssa_varname_KeyTag";
SgInitializedName* StaticSingleAssignment::thisDecl = NULL;
StaticSingleAssignment::VarName StaticSingleAssignment::emptyName;
StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::emptyRenameTable;
StaticSingleAssignment::NumNodeRenameEntry StaticSingleAssignment::emptyRenameEntry;

bool StaticSingleAssignment::isFromLibrary(SgNode* node)
{
	Sg_File_Info* fi = node->get_file_info();
	if (fi->isCompilerGenerated())
		return true;
	string filename = fi->get_filenameString();

	if ((filename.find("include") != string::npos))
	{
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

//Function to perform the StaticSingleAssignment and annotate the AST
void StaticSingleAssignment::run()
{
	originalDefTable.clear();
	expandedDefTable.clear();
	reachingDefsTable.clear();
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

			uniqueTrav.traverse(func->get_declaration());

			if (getDebug())
				cout << "Finished UniqueNameTraversal..." << endl;

			if (getDebug())
				cout << "Running DefsAndUsesTraversal on function: " << SageInterface::get_name(func) << func << endl;

			defUseTrav.traverse(func->get_declaration());

			if (getDebug())
				cout << "Finished DefsAndUsesTraversal..." << endl;

			//Expand any member variable definition to also define its parents at the same node
			expandParentMemberDefinitions();

			//Expand any member variable uses to also use the parent variables (e.g. a.x also uses a)
			expandParentMemberUses();

			//Iterate the global table insert a def for each name at the function definition
			foreach(const VarName& globalVar, globalVarList)
			{
				//Add this function definition as a definition point of this variable
				originalDefTable[func].insert(globalVar);
			}

			if (getDebug())
				cout << "Running DefUse Data Flow on function: " << SageInterface::get_name(func) << func << endl;
			runDefUseDataFlow(func);
		}
	}
}

void StaticSingleAssignment::findGlobalVars()
{
	InitNameVec vars = SageInterface::querySubTree<SgInitializedName> (project, V_SgInitializedName);

	foreach(SgInitializedName* iter, vars)
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
	vector<SgFunctionCallExp*> calls = SageInterface::querySubTree<SgFunctionCallExp>(project, V_SgFunctionCallExp);

	foreach(SgFunctionCallExp* call, calls)
	{
		//Iterate the global table insert a def for each name at the function call
		foreach(VarName& entry, globalVarList)
		{
			//Add this function call as a definition point of this variable
			originalDefTable[call].insert(entry);
		}
	}
}

void StaticSingleAssignment::expandParentMemberDefinitions()
{
	foreach(const LocalDefTable::value_type& nodeVarsPair, originalDefTable)
	{
		SgNode* node = nodeVarsPair.first;

		//We want to iterate the vars defined on this node, and expand them
		foreach(const VarName& definedVar, nodeVarsPair.second)
		{
			if (getDebugExtra())
			{
				cout << "Checking [" << keyToString(definedVar) << "]" << endl;
			}

			//Check if the variableName has multiple parts
			if (definedVar.size() == 1)
			{
				continue;
			}

			//We are dealing with a multi-part variable, loop the entry and expand it
			//Start at one so we don't get the same defs in the original and expanded defs
			for (unsigned int i = 1; i < definedVar.size(); i++)
			{
				//Create a new varName vector that goes from beginning to end - i
				VarName newName;
				newName.assign(definedVar.begin(), definedVar.end() - i);

				if (getDebugExtra())
				{
					cout << "Testing for presence of [" << keyToString(newName) << "]" << endl;
				}

				//Only insert the new definition if it does not already exist in the original def table
				if (originalDefTable[node].count(newName) == 0)
				{
					//Insert the new name as being defined here.
					expandedDefTable[node].insert(newName);

					if (getDebugExtra())
					{
						cout << "Inserted new name [" << keyToString(newName) << "] into defs." << endl;
					}
				}
			}
		}
	}
}

void StaticSingleAssignment::expandParentMemberUses()
{
	foreach(DefUseTable::value_type& nodeTablePair, useTable)
	{
		SgNode* node = nodeTablePair.first;
		//We want to iterate the vars used on this node, and expand them
		foreach(TableEntry::value_type& entry, nodeTablePair.second)
		{
			const VarName& usedVar = entry.first;
			if (getDebugExtra())
			{
				cout << "Checking [" << keyToString(usedVar) << "]" << endl;
			}

			//Check if the variableName has multiple parts
			if (usedVar.size() == 1)
			{
				continue;
			}

			//We are dealing with a multi-part variable, loop the entry and expand it
			//Start at one so we don't reinsert same use
			for (unsigned int i = 1; i < usedVar.size(); i++)
			{
				//Create a new varName vector that goes from beginning to end - i
				VarName newName;
				newName.assign(usedVar.begin(), usedVar.end() - i);

				if (getDebugExtra())
				{
					cout << "Testing for presence of [" << keyToString(newName) << "]" << endl;
				}

				//Only insert the new definition if it does not already exist
				if (useTable[node].count(newName) == 0)
				{
					//Insert the new name as being used here.
					useTable[node][newName] = NodeVec(1, node);

					if (getDebugExtra())
					{
						cout << "Inserted new name [" << keyToString(newName) << "] into uses." << endl;
					}
				}
			}
		}
	}
}

void StaticSingleAssignment::runDefUseDataFlow(SgFunctionDefinition* func)
{
	if (getDebug())
		printOriginalDefTable();
	//Keep track of visited nodes
	boost::unordered_set<SgNode*> visited;

	//Reset the first def list to prevent errors with global vars.
	firstDefList.clear();

	vector<FilteredCfgNode> worklist;

	FilteredCfgNode current = FilteredCfgNode(func->cfgForBeginning());
	worklist.push_back(current);

	while (!worklist.empty())
	{
		if (getDebug())
			cout << "-------------------------------------------------------------------------" << endl;
		//Get the node to work on
		current = worklist.back();
		worklist.pop_back();

		//We don't want to do def_use on the ending CFGNode of the function definition
		//so if we see it, continue.
		//If we do this, then incorrect information will be propogated to the beginning of the function
		if (current == FilteredCfgNode(func->cfgForEnd()))
		{
			if (getDebug())
				cout << "Skipped defUse on End of function definition." << endl;
			continue;
		}

		NodeVec memberRefInsertedNodes;
		bool changed = defUse(current, memberRefInsertedNodes);

		//If memberRefs were inserted, then there are nodes previous to this one that are different.
		//Thus, we need to add those nodes to the working list
		if (!memberRefInsertedNodes.empty())
		{
			//Clear the worklist and visited list
			worklist.clear();
			visited.clear();

			//Insert each changed node into the list
			foreach(SgNode* chNode, memberRefInsertedNodes)
			{
				//Get the cfg node for this node
				FilteredCfgNode nextNode = FilteredCfgNode(chNode->cfgForBeginning());
				worklist.push_back(nextNode);
				if (getDebug())
					cout << "Member Ref Inserted: Added " << nextNode.getNode()->class_name() << nextNode.getNode() << " to the worklist." << endl;
			}

			//Restart work from where the new def was inserted.
			continue;
		}

		//Get the outgoing edges
		cfgEdgeVec outEdges = current.outEdges();

		//For every edge, add it to the worklist if it is not seen or something has changed
		reverse_foreach(FilteredCfgEdge& edge, outEdges)
		{
			FilteredCfgNode nextNode = edge.target();

			//Insert the child in the worklist if the parent is changed or it hasn't been visited yet
			if (changed || visited.count(nextNode.getNode()) == 0)
			{
				//Add the node to the worklist
				if (find(worklist.begin(), worklist.end(), nextNode) == worklist.end())
				{
					worklist.push_back(nextNode);
					if (getDebug())
					{
						if (changed)
							cout << "Defs Changed: Added " << nextNode.getNode()->class_name() << nextNode.getNode() << " to the worklist." << endl;
						else
							cout << "Next unvisited: Added " << nextNode.getNode()->class_name() << nextNode.getNode() << " to the worklist." << endl;
					}
				}
			}
		}

		//Mark the current node as seen
		visited.insert(current.getNode());
	}
}

bool StaticSingleAssignment::defUse(FilteredCfgNode node, NodeVec &memberRefInsertedNodes)
{
	SgNode* current = node.getNode();

	//Handle each type of node
	if (getDebug())
		cout << "Performing DefUse on " << 
				current->class_name() << ", line " << current->get_file_info()->get_line() << ":" << current << endl;

	bool defChanged = false;
	defChanged = mergeDefs(node);
	resolveUses(node, memberRefInsertedNodes);

	if (getDebug())
		cout << "Defs were " << ((defChanged) ? "changed." : "same.") << endl;

	return defChanged;
}

bool StaticSingleAssignment::mergeDefs(FilteredCfgNode curNode)
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

	TableEntry stagingPropagatedDefs;
	//Retrieve the defs coming from previous cfgNodes
	aggregatePreviousDefs(curNode, stagingPropagatedDefs);

	//Replace every entry in staging table that has definition in original defs
	//Also assign renaming numbers to any new definitions
	foreach(const VarName& definedVar, originalDefTable[node])
	{
		//Replace the entry for this variable with the definitions at this node.
		stagingPropagatedDefs[definedVar].clear();
		stagingPropagatedDefs[definedVar].push_back(node);

		addRenameNumberForNode(definedVar, node);

		//Insert expanded defs for the original def, if any such expanded defs are in the staging set.
		//Note that we only expand child defs as needed, even though we expanded all the parent defs up front.
		foreach(TableEntry::value_type& propEntry, stagingPropagatedDefs)
		{
			//Don't insert a def if it is already originally defined.
			if (originalDefTable[node].count(propEntry.first) != 0)
			{
				continue;
			}
			//If the original def is a prefix of the propogated def, add a def at this node
			//Compare sizes to guard against inserting original def in expanded table
			if (isPrefixOfName(propEntry.first, definedVar) && (propEntry.first.size() > definedVar.size()))
			{
				//Set this node as a definition point of the variable.
				expandedDefTable[node].insert(propEntry.first);
				if (getDebugExtra())
				{
					cout << "Inserted expandedDef for [" << keyToString(propEntry.first) << "] with originalDef prefix [" << keyToString(definedVar) << "]" << endl;
				}
			}
		}
	}

	//Replace every entry in staging table that has definition in expandedDefs
	//Also assign renaming numbers to any new definitions
	foreach(const VarName& definedVar, expandedDefTable[node])
	{
		stagingPropagatedDefs[definedVar].clear();
		stagingPropagatedDefs[definedVar].push_back(node);
		
		addRenameNumberForNode(definedVar, node);
	}

	//If there is an initial definition of a name at this node, we should insert it in the table
	foreach(const VarName& definedVar, originalDefTable[node])
	{
		//If the given variable name is not present in the first def table
		if (firstDefList.count(definedVar) == 0)
		{
			//Set this node as the first definition point of this variable.
			firstDefList[definedVar] = node;
		}
	}

	if (getDebugExtra())
	{
		cout << "Local Defs replaced in propDefs ";
		printDefs(stagingPropagatedDefs);
	}

	//Now do a comparison to see if we should copy
	if (stagingPropagatedDefs != reachingDefsTable[node])
	{
		reachingDefsTable[node] = stagingPropagatedDefs;
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

void StaticSingleAssignment::aggregatePreviousDefs(FilteredCfgNode curNode, TableEntry& results)
{
	//Get the previous edges in the CFG for this node
	cfgEdgeVec inEdges = curNode.inEdges();

	if (inEdges.size() == 1)
	{
		SgNode* prev = inEdges[0].source().getNode();

		//Copy the previous node defs to the staging table
		results = reachingDefsTable[prev];
	}
	else if (inEdges.size() > 1)
	{
		//Iterate all of the incoming edges
		for (unsigned int i = 0; i < inEdges.size(); i++)
		{
			SgNode* prev = inEdges[i].source().getNode();

			//Perform the union of all the infoming definitions.
			foreach(TableEntry::value_type& entry, reachingDefsTable[prev])
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
}

void StaticSingleAssignment::resolveUses(FilteredCfgNode curNode, NodeVec &memberRefInsertedNodes)
{
	SgNode* node = curNode.getNode();

	//We want to resolve the uses at the current node
	//We need to look to the defs at the current node, and match them
	//with the uses

	if (getDebug())
		cout << "Resolving uses at " << node->class_name() << node << endl;

	//Iterate every use at the current node
	foreach(const TableEntry::value_type& entry, useTable[node])
	{
		const VarName& usedVar = entry.first;
		//Check the defs that are active at the current node to find the reaching definition
		//We want to check if there is a definition entry for this use at the current node
		if (reachingDefsTable[node].find(usedVar) != reachingDefsTable[node].end())
		{
			//There is a definition entry. Now we want to see if the use is already up to date
			if (useTable[node][usedVar] != reachingDefsTable[node][usedVar])
			{
				//The use was not up to date, so we update it
				//Overwrite the use with this definition location(s).
				useTable[node][usedVar] = reachingDefsTable[node][usedVar];
			}
		}
		else
		{
			//If there are no defs for this use at this node, then we have a multi-part name
			//that has not been expanded. Thus, we want to expand it.
			insertExpandedDefsForUse(curNode, entry.first, memberRefInsertedNodes);
		}
	}

	TableEntry results;
	//Get the previous defs
	aggregatePreviousDefs(curNode, results);

	//However, if there is a def at the current node, we want to use the previous
	//def as the use for this node.

	//Iterate every use at the current node
	foreach(const TableEntry::value_type& entry, useTable[node])
	{
		if (originalDefTable[node].count(entry.first) != 0)
		{
			useTable[node][entry.first] = results[entry.first];

			if (getDebug())
				cout << "Fixed use of local def." << endl;
		}
	}
}


void StaticSingleAssignment::insertExpandedDefsForUse(FilteredCfgNode curNode, VarName name, NodeVec &changedNodes)
{
	SgNode* node = curNode.getNode();

	if (getDebugExtra())
	{
		cout << "Checking for needed extra defs for uses at " << node->class_name() << node << endl;
		cout << "Checking for [" << keyToString(name) << "]" << endl;
	}

	//Check if the given name has a def at this node
	if (reachingDefsTable.count(node) == 0 || reachingDefsTable[node].count(name) != 0)
	{
		if (getDebugExtra())
			cout << "Already have def." << endl;

		//If there is already a def, then nothing changes
		return;
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
			SgFunctionDeclaration* declaration = SageInterface::getEnclosingNode<SgFunctionDeclaration>(node);
			ROSE_ASSERT(declaration != NULL);
			SgFunctionDefinition* func = declaration->get_definition();
			ROSE_ASSERT(func);

			firstDefList[rootName] = func;
		}
		//Otherwise, see if it is in namespace scope
		else if (isSgNamespaceDefinitionStatement(SageInterface::getScope(rootName[0])) != NULL)
		{
			//It is declared in namespace scope.
			//Get our enclosing function definition to insert the first definition into.
			SgFunctionDeclaration* declaration = SageInterface::getEnclosingNode<SgFunctionDeclaration>(node);
			ROSE_ASSERT(declaration != NULL);
			SgFunctionDefinition* func = declaration->get_definition();
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
			originalDefTable[firstDefList[rootName]].insert(newName);
			changedNodes.push_back(firstDefList[rootName]);
			if (getDebugExtra())
			{
				cout << "Inserted def for [" << keyToString(newName) << "] (root) [" << keyToString(rootName) << "] at node " << firstDefList[rootName] << endl;
			}
		}
	}

	if (getDebugExtra())
		cout << "Finished inserting references. Changed: " << ((!changedNodes.empty()) ? "true" : "false") << endl;
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
