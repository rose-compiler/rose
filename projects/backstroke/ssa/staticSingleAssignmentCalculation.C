//Author: George Vulov <georgevulov@hotmail.com>
//Based on work by Justin Frye <jafrye@tamu.edu>

#include "rose.h"
#include "CallGraph.h"
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
#include <boost/tuple/tuple.hpp>
#include "uniqueNameTraversal.h"
#include "defsAndUsesTraversal.h"
#include "iteratedDominanceFrontier.h"
#include "controlDependence.h"

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

using namespace std;
using namespace ssa_private;
using namespace boost;

//Initializations of the static attribute tags
string StaticSingleAssignment::varKeyTag = "ssa_varname_KeyTag";
StaticSingleAssignment::VarName StaticSingleAssignment::emptyName;

bool StaticSingleAssignment::isBuiltinVar(const VarName& var)
{
	string name = var[0]->get_name().getString();
	if (name == "__func__" ||
			name == "__FUNCTION__" ||
			name == "__PRETTY_FUNCTION__")
		return true;

	return false;
}

bool StaticSingleAssignment::isVarInScope(const VarName& var, SgNode* astNode)
{
	SgScopeStatement* scope = SageInterface::getScope(astNode);
	ROSE_ASSERT(var.size() > 0 && scope != NULL);
	SgScopeStatement* varScope = SageInterface::getScope(var[0]);

	if (varScope == scope || SageInterface::isAncestor(varScope, scope))
	{
		//FIXME: In a basic block, the definition of the variable might come AFTER the node in question
		//We should return false in this case.

		//Special case: a variable cannot be accessed in its own assign initializer
		//This is important for loops where a variable is redefined on every iteration
		//E.g. while (int a = 3) {}
		if (SageInterface::isAncestor(var[0], astNode))
			return false;

		return true;
	}
	else if (isSgNamespaceDefinitionStatement(varScope) || isSgGlobal(varScope))
	{
		//Variables defined in a namespace or in global scope are always accessible if they're fully qualified
		return true;
	}
	else if (isSgInitializedName(astNode) && isSgCtorInitializerList(astNode->get_parent()))
	{
		//Work around a SageInterface::getScope peculiarity
		//SageInterface::getScope returns class scope for the initialized names in the constructor initializer list,
		//because these are class-scoped variables. However, those initializers can actually access function parameters
		SgFunctionDeclaration* funcDeclaration = isSgFunctionDeclaration(astNode->get_parent()->get_parent());
		ROSE_ASSERT(funcDeclaration != NULL);
		SgFunctionParameterList* parameters = funcDeclaration->get_parameterList();
		const SgInitializedNamePtrList& paramList = parameters->get_args();
		//If the variable in question in the parameter list, it can be reached by the constructor initializers
		if (find(paramList.begin(), paramList.end(), var[0]) != paramList.end())
		{
			return true;
		}
	}
	else if (SgClassDefinition* varClassScope = isSgClassDefinition(varScope))
	{
		//If the variable is static & public, it's accessible
		SgVariableDeclaration* varDeclaration = isSgVariableDeclaration(var[0]->get_parent());
		if (varDeclaration != NULL) //variable declaration is null inside constructor initializer list
		{
			if (varDeclaration->get_declarationModifier().get_storageModifier().isStatic() &&
					varDeclaration->get_declarationModifier().get_accessModifier().isPublic())
			{
				return true;
			}
		}

		//If the variable is a class member, see if the scope is a member function
		SgNode* curr = scope;
		while (curr != NULL && !isSgMemberFunctionDeclaration(curr))
		{
			curr = curr->get_parent();
		}

		if (curr == NULL)
			return false;

		SgMemberFunctionDeclaration* memFunction = isSgMemberFunctionDeclaration(curr);
		ROSE_ASSERT(memFunction != NULL);
		SgClassDefinition* funcClassScope = memFunction->get_class_scope();
		ROSE_ASSERT(funcClassScope != NULL);

		//If they are members of the same class, we're done
		if (funcClassScope == varClassScope)
		{
			return true;
		}

		//The two are not from the same class. Let's see if there is a friend class declaration
		vector<SgClassDeclaration*> nestedDeclarations =
				SageInterface::querySubTree<SgClassDeclaration>(varClassScope, V_SgClassDeclaration);
		foreach (SgClassDeclaration* nestedDeclaration, nestedDeclarations)
		{
			if (nestedDeclaration->get_declarationModifier().isFriend())
			{
				//The variable's class has friend class. Check if the member function in question is in that friend
				if (nestedDeclaration->get_firstNondefiningDeclaration() ==
						funcClassScope->get_declaration()->get_firstNondefiningDeclaration())
				{
					return true;
				}
			}
		}

		//The variable is not in the same class and there is no friend class declaration, but we need to check the inheritance tree
		//We do a search of the inheritance tree; this will terminate because it's a DAG
		set<SgBaseClass*> worklist;
		worklist.insert(funcClassScope->get_inheritances().begin(), funcClassScope->get_inheritances().end());

		while (!worklist.empty())
		{
			SgBaseClass* baseClass = *worklist.begin();
			worklist.erase(worklist.begin());

			//Get the class definition so we can get its base classes
			SgClassDeclaration* definingDeclaration = isSgClassDeclaration(baseClass->get_base_class()->get_definingDeclaration());
			if (definingDeclaration == NULL)
				continue;

			SgClassDefinition* baseClassDefinition = isSgClassDefinition(definingDeclaration->get_definition());
			ROSE_ASSERT(baseClassDefinition != NULL);
			foreach (SgBaseClass* grandparentClass, baseClassDefinition->get_inheritances())
			{
				worklist.insert(grandparentClass);
			}

			//Check if this base class matches the var scope
			if (baseClassDefinition == varClassScope)
			{
				//Check that the variable is public or protected
				const SgAccessModifier& access = varDeclaration->get_declarationModifier().get_accessModifier();
				return (access.isPublic() || access.isProtected());
			}
		}
	}

	return false;
}

//Function to perform the StaticSingleAssignment and annotate the AST
void StaticSingleAssignment::run()
{
	originalDefTable.clear();
	expandedDefTable.clear();
	reachingDefsTable.clear();
	localUsesTable.clear();
	useTable.clear();

	UniqueNameTraversal uniqueTrav(SageInterface::querySubTree<SgInitializedName>(project, V_SgInitializedName));
	DefsAndUsesTraversal defUseTrav(this);

	vector<SgFunctionDefinition*> funcs = SageInterface::querySubTree<SgFunctionDefinition> (project, V_SgFunctionDefinition);
	FunctionFilter functionFilter;
	foreach (SgFunctionDefinition* func, funcs)
	{
		if (functionFilter(func->get_declaration()))
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

			insertDefsForExternalVariables(func->get_declaration());

			//Expand any member variable definition to also define its parents at the same node
			expandParentMemberDefinitions(func->get_declaration());

			//Expand any member variable uses to also use the parent variables (e.g. a.x also uses a)
			expandParentMemberUses(func->get_declaration());

			insertDefsForChildMemberUses(func->get_declaration());

			//Create all ReachingDef objects:
			//Create ReachingDef objects for all original definitions
			populateLocalDefsTable(func->get_declaration());
			//Insert phi functions at join points
			multimap< FilteredCfgNode, pair<FilteredCfgNode, FilteredCfgEdge> > controlDependencies = insertPhiFunctions(func);

			//Renumber all instantiated ReachingDef objects
			renumberAllDefinitions(func);

			if (getDebug())
				cout << "Running DefUse Data Flow on function: " << SageInterface::get_name(func) << func << endl;
			runDefUseDataFlow(func);

			//We have all the propagated defs, now update the use table
			buildUseTable(func);

			//Annotate phi functions with dependencies
			annotatePhiNodeWithConditions(func, controlDependencies);
		}
	}
}

void StaticSingleAssignment::expandParentMemberDefinitions(SgFunctionDeclaration* function)
{
	class ExpandDefsTraversal : public AstSimpleProcessing
	{
	public:
		StaticSingleAssignment* ssa;

		void visit(SgNode* node)
		{
			if (ssa->originalDefTable.count(node) == 0)
				return;

			//We want to iterate the vars defined on this node, and expand them
			foreach(const VarName& definedVar, ssa->originalDefTable[node])
			{
				if (getDebugExtra())
				{
					cout << "Checking [" << varnameToString(definedVar) << "]" << endl;
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
						cout << "Testing for presence of [" << varnameToString(newName) << "]" << endl;
					}

					//Only insert the new definition if it does not already exist in the original def table
					if (ssa->originalDefTable[node].count(newName) == 0)
					{
						//Insert the new name as being defined here.
						ssa->expandedDefTable[node].insert(newName);

						if (getDebugExtra())
						{
							cout << "Inserted new name [" << varnameToString(newName) << "] into defs." << endl;
						}
					}
				}
			}
		}
	};

	ExpandDefsTraversal trav;
	trav.ssa = this;
	trav.traverse(function, preorder);
}

void StaticSingleAssignment::expandParentMemberUses(SgFunctionDeclaration* function)
{
	class ExpandUsesTraversal : public AstSimpleProcessing
	{
	public:
		StaticSingleAssignment* ssa;

		void visit(SgNode* node)
		{
			if (ssa->localUsesTable.count(node) == 0)
				return;

			//We want to iterate the vars used on this node, and expand them
			foreach(const VarName& usedVar, ssa->localUsesTable[node])
			{
				if (getDebugExtra())
				{
					cout << "Checking [" << varnameToString(usedVar) << "]" << endl;
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
						cout << "Testing for presence of [" << varnameToString(newName) << "]" << endl;
					}

					//Only insert the new definition if it does not already exist
					if (ssa->localUsesTable[node].count(newName) == 0)
					{
						//Insert the new name as being used here.
						ssa->localUsesTable[node].insert(newName);

						if (getDebugExtra())
						{
							cout << "Inserted new name [" << varnameToString(newName) << "] into uses." << endl;
						}
					}
				}
			}
		}
	};

	ExpandUsesTraversal trav;
	trav.ssa = this;
	trav.traverse(function, preorder);
}

void StaticSingleAssignment::runDefUseDataFlow(SgFunctionDefinition* func)
{
	if (getDebug())
		printOriginalDefTable();
	//Keep track of visited nodes
	unordered_set<SgNode*> visited;

	set<FilteredCfgNode> worklist;

	FilteredCfgNode current = FilteredCfgNode(func->cfgForBeginning());
	worklist.insert(current);

	while (!worklist.empty())
	{
		if (getDebug())
			cout << "-------------------------------------------------------------------------" << endl;
		//Get the node to work on
		current = *worklist.begin();
		worklist.erase(worklist.begin());

		//We don't want to do def_use on the ending CFGNode of the function definition
		//so if we see it, continue.
		//If we do this, then incorrect information will be propogated to the beginning of the function
		if (current == FilteredCfgNode(func->cfgForEnd()))
		{
			if (getDebug())
				cout << "Skipped defUse on End of function definition." << endl;
			continue;
		}

		//Propagate defs to the current node
		bool changed = propagateDefs(current);

		//For every edge, add it to the worklist if it is not seen or something has changed
		reverse_foreach(const FilteredCfgEdge& edge, current.outEdges())
		{
			FilteredCfgNode nextNode = edge.target();

			//Insert the child in the worklist if the parent is changed or it hasn't been visited yet
			if (changed || visited.count(nextNode.getNode()) == 0)
			{
				//Add the node to the worklist
				bool insertedNew = worklist.insert(nextNode).second;
				if (insertedNew && getDebug())
				{
					if (changed)
						cout << "Defs Changed: Added " << nextNode.getNode()->class_name() << nextNode.getNode() << " to the worklist." << endl;
					else
						cout << "Next unvisited: Added " << nextNode.getNode()->class_name() << nextNode.getNode() << " to the worklist." << endl;
				}
			}
		}

		//Mark the current node as seen
		visited.insert(current.getNode());
	}
}

bool StaticSingleAssignment::propagateDefs(FilteredCfgNode cfgNode)
{
	SgNode* node = cfgNode.getNode();

	//This updates the IN table with the reaching defs from previous nodes
	updateIncomingPropagatedDefs(cfgNode);

	//Create a staging OUT table. At the end, we will check if this table
	//Was the same as the currently available one, to decide if any changes have occurred
	//We initialize the OUT tabel to the IN table
	NodeReachingDefTable outDefsTable = reachingDefsTable[node].first;

	//Special case: the IN table of the function definition can have phi nodes inserted for the
	//definitions reaching the END of the function. So, start with an empty table to prevent definitions
	//from the bottom of the function from propagating to the top.
	if (isSgFunctionDefinition(node))
	{
		outDefsTable.clear();
	}

	//Now overwrite any local definitions:
	if (ssaLocalDefTable.count(node) > 0)
	{
		foreach(const NodeReachingDefTable::value_type& varDefPair, ssaLocalDefTable[node])
		{
			const VarName& definedVar = varDefPair.first;
			ReachingDefPtr localDef = varDefPair.second;

			outDefsTable[definedVar] = localDef;
		}
	}

	//Compare old to new OUT tables
	bool changed = (reachingDefsTable[node].second != outDefsTable);
	if (changed)
	{
		reachingDefsTable[node].second = outDefsTable;
	}

	return changed;
}

void StaticSingleAssignment::updateIncomingPropagatedDefs(FilteredCfgNode cfgNode)
{
	//Get the previous edges in the CFG for this node
	vector<FilteredCfgEdge> inEdges = cfgNode.inEdges();
	SgNode* astNode = cfgNode.getNode();

	NodeReachingDefTable& incomingDefTable = reachingDefsTable[astNode].first;

	//Iterate all of the incoming edges
	for (unsigned int i = 0; i < inEdges.size(); i++)
	{
		SgNode* prev = inEdges[i].source().getNode();

		const NodeReachingDefTable& previousDefs = reachingDefsTable[prev].second;

		//Merge all the previous defs into the IN table of the current node
		foreach(const NodeReachingDefTable::value_type& varDefPair, previousDefs)
		{
			const VarName& var = varDefPair.first;
			const ReachingDefPtr previousDef = varDefPair.second;

			//Here we don't propagate defs for variables that went out of scope
			//(built-in vars are body-scoped but we inserted the def at the SgFunctionDefinition node, so we make an exception)
			if (!isVarInScope(var, astNode) && !isBuiltinVar(var))
				continue;

			//If this is the first time this def has propagated to this node, just copy it over
			if (incomingDefTable.count(var) == 0)
			{
				incomingDefTable[var] = previousDef;
			}
			else
			{
				ReachingDefPtr existingDef = incomingDefTable[var];

				if (existingDef->isPhiFunction() && existingDef->getDefinitionNode() == astNode)
				{
					//There is a phi node here. We update the phi function to point to the previous reaching definition
					existingDef->addJoinedDef(previousDef);
				}
				else
				{
					//If there is no phi node, and we get a new definition, it better be the same as the one previously
					//propagated.
					if (!(*previousDef == *existingDef))
					{
						printf("ERROR: At node %s@%d, two different definitions reach for variable %s\n",
								astNode->class_name().c_str(), astNode->get_file_info()->get_line(), varnameToString(var).c_str());
						ROSE_ASSERT(false);
					}
				}
			}
		}
	}
}

void StaticSingleAssignment::buildUseTable(SgFunctionDefinition* func)
{
	if (getDebug())
	{
		printf("Local uses table:\n");
		printLocalDefUseTable(localUsesTable);
	}

	struct UpdateUsesTrav : public AstSimpleProcessing
	{
		StaticSingleAssignment* ssa;

		void visit(SgNode* node)
		{
			if (ssa->localUsesTable.count(node) == 0)
				return;

			foreach(const VarName& usedVar, ssa->localUsesTable[node])
			{
  				//Check the defs that are active at the current node to find the reaching definition
				//We want to check if there is a definition entry for this use at the current node
				if (ssa->reachingDefsTable[node].first.count(usedVar) > 0)
				{
					ssa->useTable[node][usedVar] = ssa->reachingDefsTable[node].first[usedVar];
				}
				else
				{
					// There are no defs for this use at this node, this shouldn't happen
					printf("Error: Found use for the name '%s', but no reaching defs!\n", varnameToString(usedVar).c_str());
					printf("Node is %s:%d\n", node->class_name().c_str(), node->get_file_info()->get_line());
					ROSE_ASSERT(false);
				}
			}
		}
	};

	UpdateUsesTrav trav;
	trav.ssa = this;
	trav.traverse(func->get_declaration(), preorder);
}

/** Returns a set of all the variables names that have uses in the subtree. */
set<StaticSingleAssignment::VarName> StaticSingleAssignment::getVarsUsedInSubtree(SgNode* root)
{
	class CollectUsesVarsTraversal : public AstSimpleProcessing
	{
	public:
		StaticSingleAssignment* ssa;

		//All the varNames that have uses in the function
		set<VarName> usedNames;

		void visit(SgNode* node)
		{
			LocalDefUseTable::const_iterator useEntry = ssa->localUsesTable.find(node);
			if (useEntry != ssa->localUsesTable.end())
			{
				usedNames.insert(useEntry->second.begin(), useEntry->second.end());
			}

			LocalDefUseTable::const_iterator defEntry = ssa->originalDefTable.find(node);
			if (defEntry != ssa->originalDefTable.end())
			{
				usedNames.insert(defEntry->second.begin(), defEntry->second.end());
			}
		}
	};

	CollectUsesVarsTraversal usesTrav;
	usesTrav.ssa = this;
	usesTrav.traverse(root, preorder);
	return usesTrav.usedNames;
}

void StaticSingleAssignment::insertDefsForChildMemberUses(SgFunctionDeclaration* function)
{
	ROSE_ASSERT(function->get_definition() != NULL);

	set<VarName> usedNames = getVarsUsedInSubtree(function);

	//Map each varName to all used names for which it is a prefix
	map<VarName, set<VarName> > nameToChildNames;
	foreach(const VarName& rootName, usedNames)
	{
		foreach(const VarName& childName, usedNames)
		{
			if (childName.size() <= rootName.size())
				continue;

			if (isPrefixOfName(childName, rootName))
			{
				nameToChildNames[rootName].insert(childName);
			}
		}
	}

	//Now that we have all the used names, we iterate the definitions.
	//If there is a definition and a child of it is used, we have to insert a definition for the child also
	class InsertExpandedDefsTraversal : public AstSimpleProcessing
	{
	public:
		StaticSingleAssignment* ssa;
		map<VarName, set<VarName> >* nameToChildNames;

		void visit(SgNode* node)
		{
			LocalDefUseTable::const_iterator childDefs = ssa->originalDefTable.find(node);

			if (childDefs == ssa->originalDefTable.end())
				return;

			foreach(const VarName& definedVar, childDefs->second)
			{
				map<VarName, set<VarName> >::iterator childVars = nameToChildNames->find(definedVar);
				if (childVars == nameToChildNames->end())
					continue;

				//Go over all the child names and define them here also
				foreach (const VarName& childName, childVars->second)
				{
					ROSE_ASSERT(childName.size() > definedVar.size());
					for (size_t i = 0; i < (childName.size() - definedVar.size()); i++)
					{
						//Create a new varName vector that goes from beginning to end - i
						VarName newName;
						newName.assign(childName.begin(), childName.end() - i);

						if (ssa->expandedDefTable[node].count(newName) == 0 && ssa->originalDefTable[node].count(newName) == 0)
						{
							ssa->expandedDefTable[node].insert(newName);
						}
					}
				}
			}
		}
	};

	InsertExpandedDefsTraversal trav;
	trav.ssa = this;
	trav.nameToChildNames = &nameToChildNames;
	trav.traverse(function, preorder);
}


/** Insert defs for functions that are declared outside the function scope. */
void StaticSingleAssignment::insertDefsForExternalVariables(SgFunctionDeclaration* function)
{
	ROSE_ASSERT(function->get_definition() != NULL);

	set<VarName> usedNames = getVarsUsedInSubtree(function);

	set<VarName>& originalVarsAtFunctionEntry = originalDefTable[function->get_definition()];
	set<VarName>& expandedVarsAtFunctionEntry = expandedDefTable[function->get_definition()];

	//Iterate over each used variable and check it it is declared outside of the function scope
	foreach(const VarName& usedVar, usedNames)
	{
		VarName rootName;
		rootName.assign(1, usedVar[0]);

		SgScopeStatement* varScope = SageInterface::getScope(rootName[0]);
		SgScopeStatement* functionScope = function->get_definition();

		//If it is a local variable, there should be a def somewhere inside the function
		if (varScope == functionScope || SageInterface::isAncestor(functionScope, varScope))
		{
			//We still need to insert defs for compiler-generated variables (e.g. __func__), since they don't have defs in the AST
			if (!isBuiltinVar(rootName))
				continue;
		}
		else if (isSgGlobal(varScope))
		{
			//Handle the case of declaring "extern int x" inside the function
			//Then, x has global scope but it actually has a definition inside the function so we don't need to insert one
			if (SageInterface::isAncestor(function->get_definition(), rootName[0]))
			{
				//When else could a var be declared inside a function and be global?
				SgVariableDeclaration* varDecl = isSgVariableDeclaration(rootName[0]->get_parent());
				ROSE_ASSERT(varDecl != NULL);
				ROSE_ASSERT(varDecl->get_declarationModifier().get_storageModifier().isExtern());
				continue;
			}
		}

		//Are there any other types of external vars?
		ROSE_ASSERT(isBuiltinVar(rootName) || isSgClassDefinition(varScope) || isSgNamespaceDefinitionStatement(varScope)
				|| isSgGlobal(varScope));

		//The variable is not in local scope; we need to insert a def for it at the function definition
		for (size_t i = 0; i < usedVar.size(); i++)
		{
			//Create a new varName vector that goes from beginning to end - i
			VarName newName;
			newName.assign(usedVar.begin(), usedVar.end() - i);
			originalVarsAtFunctionEntry.insert(newName);
			ROSE_ASSERT(expandedVarsAtFunctionEntry.count(newName) == 0);
		}
	}
}


multimap< StaticSingleAssignment::FilteredCfgNode, pair<StaticSingleAssignment::FilteredCfgNode, StaticSingleAssignment::FilteredCfgEdge> > 
StaticSingleAssignment::insertPhiFunctions(SgFunctionDefinition* function)
{
	if (getDebug())
		printf("Inserting phi nodes in function %s...\n", function->get_declaration()->get_name().str());
	ROSE_ASSERT(function != NULL);

	//First, find all the places where each name is defined
	map<VarName, vector<FilteredCfgNode> > nameToDefNodesMap;

	set<FilteredCfgNode> worklist;
	unordered_set<SgNode*> visited;
	worklist.insert(FilteredCfgNode(function->cfgForBeginning()));

	while (!worklist.empty())
	{
		FilteredCfgNode cfgNode = *worklist.begin();
		worklist.erase(worklist.begin());

		SgNode* node = cfgNode.getNode();
		visited.insert(node);

		//For every edge, add it to the worklist if it is not seen
		foreach(const FilteredCfgEdge& edge, cfgNode.outEdges())
		{
			FilteredCfgNode nextNode = edge.target();

			//Insert the child in the worklist if it hasn't been visited yet
			if (visited.count(nextNode.getNode()) == 0)
			{
				worklist.insert(nextNode);
			}
		}

		//Check the definitions at this node and add them to the map
		LocalDefUseTable::const_iterator defEntry = originalDefTable.find(node);
		if (defEntry != originalDefTable.end())
		{
			foreach (const VarName& definedVar, defEntry->second)
			{
				nameToDefNodesMap[definedVar].push_back(cfgNode);
			}
		}

		defEntry = expandedDefTable.find(node);
		if (defEntry != expandedDefTable.end())
		{
			foreach (const VarName& definedVar, defEntry->second)
			{
				nameToDefNodesMap[definedVar].push_back(cfgNode);
			}
		}
	}

	//Build an iterated dominance frontier for this function
	map<FilteredCfgNode, FilteredCfgNode> iPostDominatorMap;
	map<FilteredCfgNode, set<FilteredCfgNode> > domFrontiers =
			calculateDominanceFrontiers<FilteredCfgNode, FilteredCfgEdge>(function, NULL, &iPostDominatorMap);

	//Calculate control dependencies (for annotating the phi functions)
	multimap< FilteredCfgNode, pair<FilteredCfgNode, FilteredCfgEdge> > controlDependencies =
			calculateControlDependence<FilteredCfgNode, FilteredCfgEdge>(function, iPostDominatorMap);

	//Find the phi function locations for each variable
	VarName var;
	vector<FilteredCfgNode> definitionPoints;
	foreach (tie(var, definitionPoints), nameToDefNodesMap)
	{
		ROSE_ASSERT(!definitionPoints.empty() && "We have a variable that is not defined anywhere!");

		//Calculate the iterated dominance frontier
		set<FilteredCfgNode> phiNodes = calculateIteratedDominanceFrontier(domFrontiers, definitionPoints);

		if (getDebug())
			printf("Variable %s has phi nodes inserted at\n", varnameToString(var).c_str());

		foreach (FilteredCfgNode phiNode, phiNodes)
		{
			SgNode* node = phiNode.getNode();
			ROSE_ASSERT(reachingDefsTable[node].first.count(var) == 0);

			//We don't want to insert phi defs for functions that have gone out of scope
			if (!isVarInScope(var, node))
				continue;
			
			reachingDefsTable[node].first[var] = ReachingDefPtr(new ReachingDef(node, ReachingDef::PHI_FUNCTION));

			if (getDebug())
				printf("\t\t%s\n", phiNode.toStringForDebugging().c_str());
		}
	}

	return controlDependencies;
}

void StaticSingleAssignment::populateLocalDefsTable(SgFunctionDeclaration* function)
{
	ROSE_ASSERT(function->get_definition() != NULL);
	struct InsertDefs : public AstSimpleProcessing
	{
		StaticSingleAssignment* ssa;

		void visit(SgNode* node)
		{
			//Short circuit to prevent creating empty entries in the local def table when we don't need them
			if ((ssa->originalDefTable.count(node) == 0 || ssa->originalDefTable[node].empty()) &&
				(ssa->expandedDefTable.count(node) == 0 || ssa->expandedDefTable[node].empty()))
			{
				return;
			}

			//This is the table of local definitions at the current node
			NodeReachingDefTable& localDefs = ssa->ssaLocalDefTable[node];

			if (ssa->originalDefTable.count(node) > 0)
			{
				foreach(const VarName& definedVar, ssa->originalDefTable[node])
				{
					localDefs[definedVar] = ReachingDefPtr(new ReachingDef(node, ReachingDef::ORIGINAL_DEF));
				}
			}

			if (ssa->expandedDefTable.count(node) > 0)
			{
				foreach(const VarName& definedVar, ssa->expandedDefTable[node])
				{
					localDefs[definedVar] = ReachingDefPtr(new ReachingDef(node, ReachingDef::EXPANDED_DEF));
				}
			}
		}
	};

	InsertDefs trav;
	trav.ssa = this;
	trav.traverse(function, preorder);
}

void StaticSingleAssignment::renumberAllDefinitions(SgFunctionDefinition* function)
{
	ROSE_ASSERT(function != NULL);
	//Renumber all definitions. We do a depth-first traversal of the control flow graph
	unordered_set<SgNode*> visited;
	vector<FilteredCfgNode> worklist;
	FilteredCfgNode entry = function->cfgForBeginning();
	worklist.push_back(entry);

	//Map from each name to the next index. Not in map means 0
	map<VarName, int> nameToNextIndexMap;

	while (!worklist.empty())
	{
		FilteredCfgNode cfgNode = worklist.back();
		worklist.pop_back();
		SgNode* astNode = cfgNode.getNode();
		visited.insert(astNode);

		//Add all the children to the worklist. The reverse order here is important so we visit if statement bodies first
		reverse_foreach(const FilteredCfgEdge outEdge, cfgNode.outEdges())
		{
			FilteredCfgNode target = outEdge.target();
			if (visited.count(target.getNode()) == 0 &&
					find(worklist.begin(), worklist.end(), target) == worklist.end())
			{
				worklist.push_back(target);
			}
		}

		//Iterate over all the phi functions inserted at this node
		foreach (NodeReachingDefTable::value_type& varDefPair, reachingDefsTable[astNode].first)
		{
			const VarName& definedVar = varDefPair.first;
			ReachingDefPtr reachingDef = varDefPair.second;

			//Give an index to the variable
			int index = 0;
			if (nameToNextIndexMap.count(definedVar) > 0)
			{
				index = nameToNextIndexMap[definedVar];
			}
			nameToNextIndexMap[definedVar] = index + 1;

			reachingDef->setRenamingNumber(index);
		}

		//Iterate over all the local definitions at the node
		foreach (NodeReachingDefTable::value_type& varDefPair, ssaLocalDefTable[astNode])
		{
			const VarName& definedVar = varDefPair.first;
			ReachingDefPtr reachingDef = varDefPair.second;

			//Give an index to the variable
			int index = 0;
			if (nameToNextIndexMap.count(definedVar) > 0)
			{
				index = nameToNextIndexMap[definedVar];
			}
			nameToNextIndexMap[definedVar] = index + 1;

			reachingDef->setRenamingNumber(index);
		}
	}
}

void StaticSingleAssignment::annotatePhiNodeWithConditions(SgFunctionDefinition* function,
			const std::multimap< FilteredCfgNode, std::pair<FilteredCfgNode, FilteredCfgEdge> > & controlDependencies)
{
	//Find all the phi functions
	struct FindPhiNodes : public AstSimpleProcessing
	{
		StaticSingleAssignment* ssa;
		unordered_set<ReachingDefPtr> phiNodes;

		void visit(SgNode* astNode)
		{
			foreach(NodeReachingDefTable::value_type& varDefPair, ssa->reachingDefsTable[astNode].first)
			{
				if (varDefPair.second->isPhiFunction())
				{
					phiNodes.insert(varDefPair.second);
				}
			}
		}
	};

	FindPhiNodes trav;
	trav.ssa = this;
	trav.traverse(function, preorder);

	//Use a control dependence map based on ast nodes rather than cfg nodes
	typedef multimap< SgNode*, FilteredCfgEdge > DependenceMap;
	DependenceMap astControlDependence;

	typedef multimap< FilteredCfgNode, pair<FilteredCfgNode, FilteredCfgEdge> > CFGControlDependence;
	foreach(const CFGControlDependence::value_type & nodeControlNodePair, controlDependencies)
	{
		SgNode* dependentNode = nodeControlNodePair.first.getNode();
		FilteredCfgEdge controllingEdge = nodeControlNodePair.second.second;
		astControlDependence.insert(make_pair(dependentNode, controllingEdge));
	}

	//For each phi function found, calculate the conditions on its incoming defs
	foreach(ReachingDefPtr phiDef, trav.phiNodes)
	{
		//First, get the control dependencies of the phi node
		vector<FilteredCfgEdge> phiControlDependencies;
		DependenceMap::const_iterator currentIter, lastIter;
		tie(currentIter, lastIter) = astControlDependence.equal_range(phiDef->getDefinitionNode());
		for (; currentIter != lastIter; currentIter++)
		{
			//We exclude a node's control dependence on itself. This occurs at phi
			//functions inserted at loop entry
			//if (currentIter->second.source().getNode() == phiDef->getDefinitionNode())
			//	continue;

			phiControlDependencies.push_back(currentIter->second);
		}

		if (getDebug())
		{
			FilteredCfgNode phiNode = phiDef->getDefinitionNode()->cfgForBeginning();
			printf("Phi node at %s:\n", phiNode.toStringForDebugging().c_str());
		}

		//Iterate all the incoming nodes
		foreach (ReachingDefPtr joinedDef, phiDef->getJoinedDefs())
		{
			set<SgNode*> visitedNodes;
			CfgPredicate conditions =
					getConditionsForNodeExecution(joinedDef->getDefinitionNode(), phiControlDependencies, astControlDependence, visitedNodes);

			//Attach the conditions to the reaching def
			if (getDebug())
			{
				printf("\tReaching def %s has conditions %s\n", joinedDef->getDefinitionNode()->class_name().c_str(),
					conditions.toString().c_str());
			}
		}
	}
}

StaticSingleAssignment::CfgPredicate
StaticSingleAssignment::getConditionsForNodeExecution(SgNode* node, const vector<FilteredCfgEdge>& stopEdges,
		const multimap<SgNode*, FilteredCfgEdge> & controlDependencies, set<SgNode*>& visited)
{
	//Get the control dependencies of the node in question. They guarantee its execution
	typedef multimap<SgNode*, FilteredCfgEdge> DependenceMap;
	DependenceMap::const_iterator currentIter, lastIter;

	CfgPredicate result(CfgPredicate::OR);

	for (tie(currentIter, lastIter) = controlDependencies.equal_range(node); currentIter != lastIter; currentIter++)
	{
		FilteredCfgEdge dependenceEdge = currentIter->second;

		//If this edge is one of our stop edges, don't include it in the predicate
		if (find(stopEdges.begin(), stopEdges.end(), dependenceEdge) != stopEdges.end())
			continue;

		//Prevent infinite when there is a loop in the control dependence graph
		if (visited.count(dependenceEdge.source().getNode()) != 0)
		{
			continue;
		}
		else
		{
			visited.insert(dependenceEdge.source().getNode());
		}

		//This edge is one of the ones sufficient for execution of the node in question
		CfgPredicate currEdgeCondition(dependenceEdge);

		//See what this edge is dependent on on
		CfgPredicate nestedConditions =
				getConditionsForNodeExecution(dependenceEdge.source().getNode(), stopEdges, controlDependencies, visited);

		//If this condition has some additional requirements for execution, add them to the result
		if (nestedConditions.getPredicateType() != CfgPredicate::NONE)
		{
			//Both the nested condition and the original condition must be true
			CfgPredicate newCondition(CfgPredicate::AND);
			newCondition.addChildPredicate(currEdgeCondition);
			newCondition.addChildPredicate(nestedConditions);

			//Overwrite the previous condition for this edge
			currEdgeCondition = newCondition;
		}

		//Add the requirement for this edge to the list of all requirements
		result.addChildPredicate(currEdgeCondition);
	}

	if (result.getChildren().empty())
	{
		return CfgPredicate(CfgPredicate::NONE);
	}
	else
	{
		return result;
	}
}


/** This function is experimentation with interprocedural analysis.*/
void StaticSingleAssignment::interprocedural()
{
	//First, let's build a call graph. Our goal is to find an order in which to process the functions
	//So that callees are processed before callers. This way we would have exact information at each call site
	CallGraphBuilder cgBuilder(project);
	FunctionFilter functionFilter;
	cgBuilder.buildCallGraph(functionFilter);

	SgIncidenceDirectedGraph* callGraph = cgBuilder.getGraph();

	//Build a map from SgGraphNode* to the corresponding function definitions
	unordered_map<SgFunctionDefinition*, SgGraphNode*> graphNodeToFunction;
	set<SgGraphNode*> allNodes = callGraph->computeNodeSet();

	foreach(SgGraphNode* graphNode, allNodes)
	{
		SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(graphNode->get_SgNode());
		if (funcDecl == NULL)
			continue;

		funcDecl = isSgFunctionDeclaration(funcDecl->get_definingDeclaration());
		if (funcDecl == NULL)
			continue;

		SgFunctionDefinition* funcDef = funcDecl->get_definition();
		graphNodeToFunction[funcDef] = graphNode;
	}

	//Find functions of interest
	vector<SgFunctionDefinition*> funcs = SageInterface::querySubTree<SgFunctionDefinition> (project, V_SgFunctionDefinition);

	//For each function of interest, we will either find a root node or we will find a cycle.
	//The call graph might be disconnected, so we have to find all the root nodes. Later we will
	//Do a depth-first search starting at the root nodes in order to determine in which order to processs the functions
	unordered_set<SgFunctionDefinition*> rootNodes;

	foreach (SgFunctionDefinition* funcDef, funcs)
	{
		if (!functionFilter(funcDef->get_declaration()))
			continue;

		//Keep going up the graph until we hit a cycle or a root
		unordered_set<SgFunctionDefinition*> visited;
		unordered_set<SgFunctionDefinition*> worklist;
		worklist.insert(funcDef);

		while (!worklist.empty())
		{
			SgFunctionDefinition* currFunc = *worklist.begin();
			worklist.erase(worklist.begin());
			visited.insert(currFunc);

			ROSE_ASSERT(graphNodeToFunction.count(currFunc) > 0);
			SgGraphNode* graphNode = graphNodeToFunction[currFunc];

			vector<SgGraphNode*> callers;
			callGraph->getPredecessors(graphNode, callers);

			int callersWithDefinitions = 0;

			//Insert the callers into the worklist. If one of them has already been visited,
			//we have recursion.
			foreach(SgGraphNode* callerNode, callers)
			{
				SgFunctionDeclaration* callerDecl = isSgFunctionDeclaration(callerNode->get_SgNode());
				ROSE_ASSERT(callerDecl != NULL);
				callerDecl = isSgFunctionDeclaration(callerDecl->get_definingDeclaration());
				if (callerDecl == NULL)
					continue;
				SgFunctionDefinition* caller = callerDecl->get_definition();

				callersWithDefinitions++;

				//If we detect a cycle (recursion), just add an arbitrary element of the cycle as a root
				if (visited.count(caller) > 0)
				{
					rootNodes.insert(caller);
					continue;
				}

				//Add the caller to the worklist
				worklist.insert(caller);
			}

			//If this function has no callers, it's a root in the call graph
			if (callersWithDefinitions == 0)
			{
				rootNodes.insert(currFunc);
			}
		}
	}
}