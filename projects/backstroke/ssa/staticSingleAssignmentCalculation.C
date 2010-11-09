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
				originalDefTable[func].push_back(globalVar);
			}

			if (getDebug())
				cout << "Running DefUse Data Flow on function: " << SageInterface::get_name(func) << func << endl;
			runDefUseDataFlow(func);
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
			originalDefTable[call].push_back(entry);
		}
	}
}

void StaticSingleAssignment::runDefUseDataFlow(SgFunctionDefinition* func)
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

bool StaticSingleAssignment::defUse(cfgNode node, bool *memberRefInserted, NodeVec &changedNodes)
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

	TableEntry stagingPropagatedDefs;
	//Retrieve the defs coming from previous cfgNodes
	aggregatePreviousDefs(curNode, stagingPropagatedDefs);

	//Replace every entry in staging table that has definition in original defs
	//Also assign renaming numbers to any new definitions
	foreach(VarName& definedVar, originalDefTable[node])
	{
		//Replace the entry for this variable with the definitions at this node.
		stagingPropagatedDefs[definedVar].clear();
		stagingPropagatedDefs[definedVar].push_back(node);

		addRenameNumberForNode(definedVar, node);

		//Insert expanded defs for the original def, if any such expanded defs are in the staging set.
		//Note that we only expand defs when actually needed.
		foreach(TableEntry::value_type& propEntry, stagingPropagatedDefs)
		{
			//Don't insert a def if it is already originally defined.
			vector<VarName>& originalDefsAtNode = originalDefTable[node];
			if (find(originalDefsAtNode.begin(), originalDefsAtNode.end(), propEntry.first) != originalDefsAtNode.end())
			{
				continue;
			}
			//If the original def is a prefix of the propogated def, add a def at this node
			//Compare sizes to guard against inserting original def in expanded table
			if (isPrefixOfName(propEntry.first, definedVar) && (propEntry.first.size() > definedVar.size()))
			{
				//Set this node as a definition point of the variable.
				expandedDefTable[node][propEntry.first].assign(1, node);
				*memberRefInserted = true;
				if (getDebugExtra())
				{
					cout << "Inserted expandedDef for [" << keyToString(propEntry.first) << "] with originalDef prefix [" << keyToString(definedVar) << "]" << endl;
				}
			}
		}
	}

	//Replace every entry in staging table that has definition in expandedDefs
	//Also assign renaming numbers to any new definitions
	foreach(TableEntry::value_type& entry, expandedDefTable[node])
	{
		stagingPropagatedDefs[entry.first] = entry.second;

		//Now, iterate the definition vector for this node
		foreach(NodeVec::value_type& defNode, entry.second)
		{
			//Assign a number to each new definition. The function will prevent duplicates
			addRenameNumberForNode(entry.first, defNode);
		}
	}

	//If there is an initial definition of a name at this node, we should insert it in the table
	foreach(VarName& definedVar, originalDefTable[node])
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
	if (stagingPropagatedDefs != defTable[node])
	{
		defTable[node] = stagingPropagatedDefs;
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
	//Get the previous edges in the CFG for this node
	cfgEdgeVec inEdges = curNode.inEdges();

	if (inEdges.size() == 1)
	{
		SgNode* prev = inEdges[0].source().getNode();

		//Copy the previous node defs to the staging table
		results = defTable[prev];
	}
	else if (inEdges.size() > 1)
	{
		//Iterate all of the incoming edges
		for (unsigned int i = 0; i < inEdges.size(); i++)
		{
			SgNode* prev = inEdges[i].source().getNode();

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
}

bool StaticSingleAssignment::expandMemberDefinitions(cfgNode curNode)
{
	SgNode* node = curNode.getNode();

	bool changed = false;

	if (getDebugExtra())
	{
		cout << "Expanding member defs at " << node->class_name() << node << endl;
		cout << "Original Node ";
		printOriginalDefs(node);
	}

	//We want to iterate the vars defined on this node, and expand them
	foreach(VarName& definedVar, originalDefTable[node])
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

			//Only insert the new definition if it does not already exist
			vector<VarName>& originalDefsAtNode = originalDefTable[node];
			if (find(originalDefsAtNode.begin(), originalDefsAtNode.end(), newName) == originalDefsAtNode.end()
					&& expandedDefTable[node].count(newName) == 0)
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
		vector<VarName>& originalDefsAtNode = originalDefTable[node];
		if (find(originalDefsAtNode.begin(), originalDefsAtNode.end(), entry.first) != originalDefsAtNode.end())
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

		vector<VarName>& originalDefsAtNode = originalDefTable[firstDefList[rootName]];
		if (find(originalDefsAtNode.begin(), originalDefsAtNode.end(), newName) == originalDefsAtNode.end())
		{
			originalDefTable[firstDefList[rootName]].push_back(newName);
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
