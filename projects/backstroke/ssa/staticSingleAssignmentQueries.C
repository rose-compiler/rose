//Author: George Vulov <georgevulov@hotmail.com>
//Based on work by Justin Frye <jafrye@tamu.edu>
#include "staticSingleAssignment.h"
#include "sage3basic.h"
#include <boost/foreach.hpp>
#include <queue>

#define foreach BOOST_FOREACH

using namespace std;
using namespace ssa_private;

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
	foreach(TableEntry::value_type& entry, reachingDefsTable[node])
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
	foreach(TableEntry::value_type& entry, reachingDefsTable[node])
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
	foreach(const VarName& definedVar, originalDefTable[node])
	{
		//Get the rename number for the current variable at the current def site
		int renameNum = getRenameNumberForNode(definedVar, node);

		//If the name is defined at this node
		if (renameNum > 0)
		{
			//If the renumbering is not already in the result
			if (res[definedVar].count(renameNum) == 0)
			{
				//Add the renumbering to the result
				res[definedVar][renameNum] = node;
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

	return res;
}

StaticSingleAssignment::NumNodeRenameEntry StaticSingleAssignment::getOriginalDefsAtNodeForName(SgNode* node, const VarName& var)
{
	//We want to get all the original defs at this node and insert them into the result table

	NumNodeRenameEntry res;

	//Iterate every variable use at this node
	foreach(const VarName& definedVar, originalDefTable[node])
	{
		//Check that the current var is the one we want.
		if (definedVar != var)
		{
			continue;
		}

		//Get the rename number for the current variable at the current def site
		int renameNum = getRenameNumberForNode(definedVar, node);

		//If the name is defined at this node
		if (renameNum > 0)
		{
			//If the renumbering is not already in the result
			if (res.count(renameNum) == 0)
			{
				//Add the renumbering to the result
				res[renameNum] = node;
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

	return res;
}

StaticSingleAssignment::NumNodeRenameTable StaticSingleAssignment::getExpandedDefsAtNode(SgNode* node)
{
	//We want to get all the expanded defs at this node and insert them into the result table
	NumNodeRenameTable res;

	//Iterate every variable definition expanded on this node
	foreach(const VarName& definedVar, expandedDefTable[node])
	{
		//Get the rename number for the current variable at the current def site
		int renameNum = getRenameNumberForNode(definedVar, node);

		//If the name is defined at this node
		if (renameNum > 0)
		{
			//If the renumbering is not already in the result
		 	if (res[definedVar].count(renameNum) == 0)
			{
				//Add the renumbering to the result
				res[definedVar][renameNum] = node;
			}
			else
			{
				cout << "Error: Same def expanded twice on same node." << endl;
				ROSE_ASSERT(false);
			}
		}
		else
		{
			cout << "Error: Found expanded def for " << varnameToString(definedVar) << " with no entry in rename table." << endl;
			ROSE_ASSERT(false);
		}
	}

	return res;
}

StaticSingleAssignment::NumNodeRenameEntry StaticSingleAssignment::getExpandedDefsAtNodeForName(SgNode* node, const VarName& var)
{
	//We want to get all the uses at this node and insert them into the result table

	NumNodeRenameEntry res;

	//Iterate every variable use at this node
	foreach(const VarName& definedVar, expandedDefTable[node])
	{
		//Check that the current var is the one we want.
		if (definedVar != var)
		{
			continue;
		}

		//Get the rename number for the current variable at the current def site
		int renameNum = getRenameNumberForNode(definedVar, node);

		//If the name is defined at this node
		if (renameNum > 0)
		{
			//If the renumbering is not already in the result
			if (res.count(renameNum) == 0)
			{
				//Add the renumbering to the result
				res[renameNum] = node;
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

	queue<FilteredCfgNode> worklist;

	FilteredCfgNode current = FilteredCfgNode(bb->cfgForBeginning());
	worklist.push(current);

	while (!worklist.empty())
	{
		//Get the node to work on
		current = worklist.front();
		worklist.pop();

		SgNode* currentNode = current.getNode();
		visited.insert(currentNode);

		//Find if any of the children exit the basic block.
		vector<FilteredCfgEdge> outEdges = current.outEdges();

		foreach(FilteredCfgEdge edge, outEdges)
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

	FilteredCfgNode lastNode = FilteredCfgNode(node->cfgForEnd());
	vector<FilteredCfgEdge> lastEdges = lastNode.inEdges();
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
		foreach(FilteredCfgEdge& edge, lastEdges)
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

	FilteredCfgNode lastNode = FilteredCfgNode(node->cfgForEnd());
	vector<FilteredCfgEdge> lastEdges = lastNode.inEdges();
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

		foreach(FilteredCfgEdge& edge, lastEdges)
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

void StaticSingleAssignment::printDefs(SgNode* node)
{
	cout << "Def Table for [" << node->class_name() << ":" << node << "]:" << endl;

	foreach(TableEntry::value_type& entry, reachingDefsTable[node])
	{
		cout << "  Defs for [" << varnameToString(entry.first) << "]:" << endl;

		foreach(NodeVec::value_type& iter, entry.second)
		{
			cout << "    -[" << iter->class_name() << ":" << iter << "]" << endl;
		}
	}
}

void StaticSingleAssignment::printDefs(const map< vector<SgInitializedName*>, vector<SgNode*> >& table)
{
	cout << "Def Table:" << endl;

	foreach(const TableEntry::value_type& entry, table)
	{
		cout << "  Defs for [" << varnameToString(entry.first) << "]:" << endl;

		foreach(const NodeVec::value_type& iter, entry.second)
		{
			cout << "    -[" << iter->class_name() << ":" << iter << "]" << endl;
		}
	}
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

void StaticSingleAssignment::printUses(SgNode* node)
{
	cout << "Use Table for [" << node->class_name() << ":" << node << "]:" << endl;
	foreach(TableEntry::value_type& entry, useTable[node])
	{
		cout << "  Uses for [" << varnameToString(entry.first) << "]:" << endl;
		foreach(NodeVec::value_type& iter, entry.second)
		{
			cout << "    -[" << iter->class_name() << ":" << iter << "]" << endl;
		}
	}
}

void StaticSingleAssignment::printUses(const TableEntry& table) 
{
	cout << "Use Table:" << endl;
	foreach(const TableEntry::value_type& entry, table)
	{
		cout << "  Uses for [" << varnameToString(entry.first) << "]:" << endl;
		foreach(const NodeVec::value_type& iter, entry.second)
		{
			cout << "    -[" << iter->class_name() << "," << iter << " \"" << iter->unparseToString() << "\"]" << endl;
		}
	}
}

void StaticSingleAssignment::printRenameTable()
{
	printRenameTable(numRenameTable);
}

void StaticSingleAssignment::printRenameTable(const VarName& var)
{
	cout << "Names for [" << varnameToString(var) << "]:" << endl;

	printRenameEntry(numRenameTable[var]);
}

void StaticSingleAssignment::printRenameTable(const NodeNumRenameTable& table)
{
	cout << "Rename Table:" << endl;

	//Iterate the table

	foreach(const NodeNumRenameTable::value_type& entry, table)
	{
		cout << "  Names for [" << varnameToString(entry.first) << "]:" << endl;

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
		cout << "  Names for [" << varnameToString(entry.first) << "]:" << endl;

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
		cout << "      " << iter.first << ": line " << iter.second->get_file_info()->get_line() << ", " << iter.second << endl;
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

				foreach(TableEntry::value_type& entry, reachingDefsTable[current.getNode()])
				{
					defUse << "Def [" << varnameToString(entry.first) << "]: ";

					foreach(NodeVec::value_type& val, entry.second)
					{
						defUse << getRenameNumberForNode(entry.first, val) << ": " << val << ", ";
					}
					defUse << "\\n";
				}
				//Print the uses to a string

				foreach(TableEntry::value_type& entry, useTable[current.getNode()])
				{
					defUse << "Use [" << varnameToString(entry.first) << "]: ";

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

				foreach(TableEntry::value_type& entry, reachingDefsTable[current.getNode()])
				{
					defUse << "Def [" << varnameToString(entry.first) << "]: ";

					foreach(NodeVec::value_type& val, entry.second)
					{
						defUse << getRenameNumberForNode(entry.first, val) << ": " << val << ", ";
					}
					defUse << "\\n";
				}
				//Print the uses to a string

				foreach(TableEntry::value_type& entry, useTable[current.getNode()])
				{
					defUse << "Use [" << varnameToString(entry.first) << "]: ";

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