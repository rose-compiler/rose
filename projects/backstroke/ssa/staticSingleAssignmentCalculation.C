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
#include <boost/timer.hpp>
#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>
#include <boost/tuple/tuple.hpp>
#include "uniqueNameTraversal.h"
#include "defsAndUsesTraversal.h"
#include "iteratedDominanceFrontier.h"
#include "controlDependence.h"

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

//#define DISPLAY_TIMINGS

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
	SgScopeStatement* accessingScope = SageInterface::getScope(astNode);
	ROSE_ASSERT(var.size() > 0 && accessingScope != NULL);
	SgScopeStatement* varScope = SageInterface::getScope(var[0]);

	//Work around a ROSE bug that sets incorrect scopes for built-in variables.
	if (isBuiltinVar(var))
	{
		return SageInterface::isAncestor(accessingScope, var[0]);
	}

	if (varScope == accessingScope || SageInterface::isAncestor(varScope, accessingScope))
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

	if (isSgNamespaceDefinitionStatement(varScope) || isSgGlobal(varScope))
	{
		//Variables defined in a namespace or in global scope are always accessible if they're fully qualified
		return true;
	}

	if (isSgInitializedName(astNode) && isSgCtorInitializerList(astNode->get_parent()))
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

	if (SgClassDefinition * varClassScope = isSgClassDefinition(varScope))
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

		//If the variable is accessed by a friend function, then it is available. Check if the function
		//looking to access the var is a friend
		SgFunctionDeclaration* accessingFunction = SageInterface::getEnclosingFunctionDeclaration(astNode, true);
        ROSE_ASSERT(accessingFunction != NULL);
		SgName accessingFunctionName = accessingFunction->get_mangled_name();

		//We'll look at all functions declared inside the variables class and see if any of them is the accessing function
		//and is declared a friend
        foreach(SgDeclarationStatement* varClassMember, varClassScope->get_members())
		{
            if (!isSgFunctionDeclaration(varClassMember))
                continue;
            
            SgFunctionDeclaration* nestedFunction = isSgFunctionDeclaration(varClassMember);

			if (!nestedFunction->get_declarationModifier().isFriend())
				continue;

			if (nestedFunction->get_mangled_name() != accessingFunctionName)
				continue;

			//The accessing function is a friend, so the variable is in scope
			return true;
		}

		//The variable is a class member; see if the accessing function is a member function of the same class
		SgMemberFunctionDeclaration* memFunction = isSgMemberFunctionDeclaration(accessingFunction);
		if (memFunction == NULL)
			return false;

		SgClassDefinition* funcClassScope = memFunction->get_class_scope();
		ROSE_ASSERT(funcClassScope != NULL);

		//If they are members of the same class, we're done
		if (funcClassScope == varClassScope)
		{
			return true;
		}

		//The two are not from the same class. Let's see if there is a friend class declaration
        foreach(SgDeclarationStatement* varClassMember, varClassScope->get_members())
		{
            SgClassDeclaration* nestedDeclaration = isSgClassDeclaration(varClassMember);
            if (nestedDeclaration == NULL)
                continue;
            
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
			foreach(SgBaseClass* grandparentClass, baseClassDefinition->get_inheritances())
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


void StaticSingleAssignment::run(bool interprocedural)
{
	originalDefTable_deleteMe.clear();
	localUseTable.clear();
	expandedDefTable_deleteMe.clear();
	reachingDefsTable.clear();
	astNodeToVarsUsed.clear();
	useTable.clear();
    ssaLocalDefTable.clear();

#ifdef DISPLAY_TIMINGS
	timer time;
#endif
	if (getDebug())
		cout << "Running UniqueNameTraversal...\n";
	UniqueNameTraversal uniqueTrav(SageInterface::querySubTree<SgInitializedName>(project, V_SgInitializedName));
	uniqueTrav.traverse(project);
	if (getDebug())
		cout << "Finished UniqueNameTraversal." << endl;
#ifdef DISPLAY_TIMINGS
	printf("-- Timing: UniqueNameTraversal took %.2f seconds.\n", time.elapsed());
	fflush(stdout);
	time.restart();
#endif
	
	//Get a list of all the functions that we'll process
	unordered_set<SgFunctionDefinition*> interestingFunctions;
	vector<SgFunctionDefinition*> funcs = SageInterface::querySubTree<SgFunctionDefinition> (project, V_SgFunctionDefinition);

	FunctionFilter functionFilter;
	foreach (SgFunctionDefinition* f, funcs)
	{
		if (functionFilter(f->get_declaration()))
			interestingFunctions.insert(f);
	}

	//Generate all local information before doing interprocedural analysis. This is so we know
	//what variables are directly modified in each function body before we do interprocedural propagation
	foreach (SgFunctionDefinition* func, interestingFunctions)
	{
		if (getDebug())
			cout << "Running DefsAndUsesTraversal on function: " << SageInterface::get_name(func) << func << endl;

		CFGNodeToVarNamesMap functionDefs;
		ASTNodeToVarRefsMap functionUses;
		DefsAndUsesTraversal::CollectDefsAndUses(func->get_declaration(), functionDefs, functionUses);

		if (getDebug())
			cout << "Finished DefsAndUsesTraversal..." << endl;
		
		//Insert the new definitions into the local defs table
		foreach(const CFGNodeToVarNamesMap::value_type& nodeVarsPair, functionDefs)
		{
			const CFGNode& definingNode = nodeVarsPair.first;
			NodeReachingDefTable& nodeLocalDefs = localDefTable[definingNode];
			
			foreach(const VarName& definedVar, nodeVarsPair.second)
			{
				ReachingDefPtr originalDef = ReachingDefPtr(new ReachingDef(definingNode, ReachingDef::ORIGINAL_DEF));
				nodeLocalDefs.insert(make_pair(definedVar, originalDef));
			}
		}
		
		//Expand any member variable definition to also define its parents at the same node
		expandParentMemberDefinitions(functionDefs);
		
		ASTNodeToVarNamesMap functionVarNameUses = lookUpNamesForVarRefs(functionUses);

		insertDefsForChildMemberUses(functionDefs, functionVarNameUses);
		
		//Insert definitions at the SgFunctionDefinition for external variables whose values flow inside the function
		insertDefsForExternalVariables(func, functionDefs, functionVarNameUses);

		//Insert phi functions at join points
		vector<CFGNode> functionCfgNodesPostorder = getCfgNodesInPostorder(func);
		multimap< CFGNode, pair<CFGNode, CFGEdge> > controlDependencies = insertPhiFunctions(func, functionCfgNodesPostorder);

		//Renumber all instantiated ReachingDef objects
		renumberAllDefinitions(func, functionCfgNodesPostorder);

		printf("Defs table for %s():\n", func->get_declaration()->get_name().str());
		printDefTable(localDefTable);
		
		printf("\nPhi functions table for %s():\n", func->get_declaration()->get_name().str());
		printDefTable(reachingDefTable);
		
		printf("\nUses table for %s():\n", func->get_declaration()->get_name().str());
		foreach(const ASTNodeToVarNamesMap::value_type& nodeVarsPair, functionVarNameUses)
		{
			printf("%s @ %d: ", nodeVarsPair.first->class_name().c_str(), nodeVarsPair.first->get_file_info()->get_line());
			
			foreach(const VarName& var, nodeVarsPair.second)
			{
				printf("%s ", varnameToString(var).c_str());
			}
			printf("\n");
		}
		
		if (getDebug())
			cout << "Running DefUse Data Flow on function: " << SageInterface::get_name(func) << func << endl;
		runDefUseDataFlow(func);

		//We have all the propagated defs, now update the use table
		//buildUseTable(functionCfgNodesPostorder);
	}
	


	//Interprocedural iterations. We iterate on the call graph until all interprocedural defs are propagated
//	if (interprocedural)
//	{
//		interproceduralDefPropagation(interestingFunctions);
//	}
		



}

void StaticSingleAssignment::expandParentMemberDefinitions(const CFGNodeToVarNamesMap& defs)
{
	foreach(const CFGNodeToVarNamesMap::value_type& cfgNodeVarsPair, defs)
	{
		const CFGNode& definingNode = cfgNodeVarsPair.first;
		const set<VarName>& definedVarsAtNode = cfgNodeVarsPair.second;
		
		NodeReachingDefTable& nodeLocalDefs = localDefTable[definingNode];
		
		foreach(const VarName& definedVar, definedVarsAtNode)
		{
			//We are dealing with a multi-part variable, loop the entry and expand it
			//Start at one so we don't get the same defs in the original and expanded defs
			for (unsigned int i = 1; i < definedVar.size(); i++)
			{
				//Create a new varName vector that goes from beginning to end - i
				VarName newName;
				newName.assign(definedVar.begin(), definedVar.end() - i);

				//Only insert the new definition if it does not already exist in the original def table
				if (definedVarsAtNode.count(newName) == 0)
				{
					ROSE_ASSERT(nodeLocalDefs.count(newName) == 0);
					ReachingDefPtr expandedDef = ReachingDefPtr(new ReachingDef(definingNode, ReachingDef::EXPANDED_DEF));
					nodeLocalDefs.insert(make_pair(newName, expandedDef));
				}
			}
		}
	}
}


StaticSingleAssignment::ASTNodeToVarNamesMap StaticSingleAssignment::lookUpNamesForVarRefs(
		const StaticSingleAssignment::ASTNodeToVarRefsMap& uses)
{
	ASTNodeToVarNamesMap result;
	
	foreach(const ASTNodeToVarRefsMap::value_type& nodeVarRefsPair, uses)
	{
		SgNode* useNode = nodeVarRefsPair.first;
		set<VarName>& usedVarNamesAtNode = result[useNode];
		
		foreach(SgVarRefExp* usedVarRef, nodeVarRefsPair.second)
		{
			const VarName& usedVarName = getVarName(usedVarRef);
			ROSE_ASSERT(!usedVarName.empty());
			
			
			//If we have a use for p.x.y, insert uses for p and p.x as well as p.x.y
			for (size_t i = 1; i <= usedVarName.size(); ++i)
			{
				VarName var(usedVarName.begin(), usedVarName.begin() + i);
				
				usedVarNamesAtNode.insert(var);
			}
		}
	}
	
	return result;
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
		if (getDebugExtra())
			cout << "-------------------------------------------------------------------------" << endl;
		//Get the node to work on
		current = *worklist.begin();
		worklist.erase(worklist.begin());

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
				if (insertedNew && getDebugExtra())
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

	//Special Case: the OUT table at the function definition node actually denotes definitions at the function entry
	//So, if we're propagating to the *end* of the function, we shouldn't update the OUT table
	if (isSgFunctionDefinition(node) && cfgNode == FilteredCfgNode(node->cfgForEnd()))
	{
		return false;
	}
	
	//Create a staging OUT table. At the end, we will check if this table
	//Was the same as the currently available one, to decide if any changes have occurred
	//We initialize the OUT table to the IN table
	NodeReachingDefTable outDefsTable = reachingDefsTable[node].first;

	//Special case: the IN table of the function definition node actually denotes
	//definitions reaching the *end* of the function. So, start with an empty table to prevent definitions
	//from the bottom of the function from propagating to the top.
	if (isSgFunctionDefinition(node) && cfgNode == FilteredCfgNode(node->cfgForBeginning()))
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
					existingDef->addJoinedDef(previousDef, inEdges[i]);
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

void StaticSingleAssignment::buildUseTable(const vector<FilteredCfgNode>& cfgNodes)
{
	foreach(const FilteredCfgNode& cfgNode, cfgNodes)
	{
		SgNode* node = cfgNode.getNode();

		if (astNodeToVarsUsed.count(node) == 0)
			continue;

		foreach(const VarName& usedVar, astNodeToVarsUsed[node])
		{
			//Check the defs that are active at the current node to find the reaching definition
			//We want to check if there is a definition entry for this use at the current node
			if (reachingDefsTable[node].first.count(usedVar) > 0)
			{
				useTable[node][usedVar] = reachingDefsTable[node].first[usedVar];
			}
			else
			{
				// There are no defs for this use at this node, this shouldn't happen
				printf("Error: Found use for the name '%s', but no reaching defs!\n", varnameToString(usedVar).c_str());
				printf("Node is %s:%d in %s\n", node->class_name().c_str(), node->get_file_info()->get_line(),
						node->get_file_info()->get_filename());
				ROSE_ASSERT(false);
			}
		}
	}

	if (getDebug())
	{
		printf("Local uses table:\n");
		//printLocalDefUseTable(astNodeToVarsUsed);
	}
}

/** Returns a set of all the variables names that have uses in the subtree. */
set<StaticSingleAssignment::VarName> StaticSingleAssignment::getVarsUsedInSubtree(SgNode* root) const
{
	ROSE_ASSERT(false);
//	class CollectUsesVarsTraversal : public AstSimpleProcessing
//	{
//	public:
//		const StaticSingleAssignment* ssa;
//
//		//All the varNames that have uses in the function
//		set<VarName> usedNames;
//
//		void visit(SgNode* node)
//		{
//			LocalDefUseTable::const_iterator useEntry = ssa->astNodeToVarsUsed.find(node);
//			if (useEntry != ssa->astNodeToVarsUsed.end())
//			{
//				usedNames.insert(useEntry->second.begin(), useEntry->second.end());
//			}
//
//			LocalDefUseTable::const_iterator defEntry = ssa->originalDefTable_deleteMe.find(node);
//			if (defEntry != ssa->originalDefTable_deleteMe.end())
//			{
//				usedNames.insert(defEntry->second.begin(), defEntry->second.end());
//			}
//		}
//	};
//
//	CollectUsesVarsTraversal usesTrav;
//	usesTrav.ssa = this;
//	usesTrav.traverse(root, preorder);
//	return usesTrav.usedNames;
}

set<StaticSingleAssignment::VarName> getAllVarsUsedOrDefined(
		const StaticSingleAssignment::CFGNodeToVarNamesMap& defs, 
		const StaticSingleAssignment::ASTNodeToVarNamesMap& uses,
		const map<CFGNode, StaticSingleAssignment::NodeReachingDefTable>& localDefTable)
{
	set<StaticSingleAssignment::VarName> usedNames;
	
	foreach(const StaticSingleAssignment::CFGNodeToVarNamesMap::value_type& nodeVarsPair, defs)
	{
		map<CFGNode, StaticSingleAssignment::NodeReachingDefTable>::const_iterator allDefsIter = 
				localDefTable.find(nodeVarsPair.first);
		
		foreach(const StaticSingleAssignment::NodeReachingDefTable::value_type& varDefPair, allDefsIter->second)
		{
			usedNames.insert(varDefPair.first);
		}
		
		//FIXME: Delete this error-checking loop once the new SSA is done
		foreach(const StaticSingleAssignment::VarName& definedVarName, nodeVarsPair.second)
		{
			ROSE_ASSERT(usedNames.count(definedVarName) > 0);
		}
	}
	
	foreach(const StaticSingleAssignment::ASTNodeToVarNamesMap::value_type& nodeVarsPair, uses)
	{
		foreach(const StaticSingleAssignment::VarName& usedVarName, nodeVarsPair.second)
		{
			usedNames.insert(usedVarName);
		}
	}
	
	return usedNames;
}

void StaticSingleAssignment::insertDefsForChildMemberUses(const CFGNodeToVarNamesMap& defs, const ASTNodeToVarNamesMap& uses)
{
	set<VarName> usedNames = getAllVarsUsedOrDefined(defs, uses, localDefTable);
	
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
	
	//Go through each definition and make sure all child names are also defined at each definition point
	foreach(const CFGNodeToVarNamesMap::value_type& nodeVarsPair, defs)
	{
		const CFGNode& definingNode = nodeVarsPair.first;
		NodeReachingDefTable& nodeLocalDefs = localDefTable[definingNode];
		
		foreach(const VarName& definedVarName, nodeVarsPair.second)
		{
			//Find all the children of this variable that should also be defined here
			map<VarName, set<VarName> >::iterator childVars = nameToChildNames.find(definedVarName);
			if (childVars == nameToChildNames.end())
				continue;
			
			//Go over all the child names and define them here also
			foreach (const VarName& childName, childVars->second)
			{
				ROSE_ASSERT(childName.size() > definedVarName.size());
				for (size_t i = 0; i < (childName.size() - definedVarName.size()); i++)
				{
					//Create a new varName vector that goes from beginning to end - i
					VarName newName;
					newName.assign(childName.begin(), childName.end() - i);

					if (nodeLocalDefs.count(newName) == 0)
					{
						ReachingDefPtr expandedDef = ReachingDefPtr(new ReachingDef(definingNode, ReachingDef::EXPANDED_DEF));
						
						nodeLocalDefs.insert(make_pair(newName, expandedDef));
					}
				}
			}
		}
	}
}


/** Insert defs for functions that are declared outside the function scope. */
void StaticSingleAssignment::insertDefsForExternalVariables(SgFunctionDefinition* function, 
		const CFGNodeToVarNamesMap& defs, const ASTNodeToVarNamesMap& uses)
{
	set<VarName> usedNames = getAllVarsUsedOrDefined(defs, uses, localDefTable);
	
	//The function definition should have 4 indices in the CFG. We insert defs for external variables
	//At the very first one
	ROSE_ASSERT(function->cfgIndexForEnd() == 3);
	CFGNode functionEntryNode = function->cfgForBeginning();

	NodeReachingDefTable& functionEntryDefs = localDefTable[functionEntryNode];

	//Iterate over each used variable and check it it is declared outside of the function scope
	foreach(const VarName& usedVar, usedNames)
	{
		ROSE_ASSERT(!usedVar.empty());
		VarName rootName;
		rootName.assign(1, usedVar[0]);

		SgScopeStatement* varScope = SageInterface::getScope(rootName[0]);
		SgScopeStatement* functionScope = function;

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
			if (SageInterface::isAncestor(function, rootName[0]))
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
			VarName newName(usedVar.begin(), usedVar.end() - i);
			
			if (functionEntryDefs.count(newName) > 0)
				continue;
			
			ReachingDefPtr def = ReachingDefPtr(new ReachingDef(functionEntryNode, ReachingDef::EXTERNAL_DEF));
			functionEntryDefs.insert(make_pair(newName, def));
		}
	}
}


multimap< CFGNode, pair<CFGNode, CFGEdge> > 
StaticSingleAssignment::insertPhiFunctions(SgFunctionDefinition* function, const std::vector<CFGNode>& cfgNodesInPostOrder)
{
	if (getDebug())
		printf("Inserting phi nodes in function %s...\n", function->get_declaration()->get_name().str());
	ROSE_ASSERT(function != NULL);

	//First, find all the places where each name is defined
	map<VarName, vector<CFGNode> > nameToDefNodesMap;

	foreach(const CFGNode& cfgNode, cfgNodesInPostOrder)
	{
		CFGNodeToDefTableMap::const_iterator defTableIter = localDefTable.find(cfgNode);
		if (defTableIter == localDefTable.end())
			continue;
		
		const NodeReachingDefTable& definitionsAtNode = defTableIter->second;
		foreach(const NodeReachingDefTable::value_type& varDefPair, definitionsAtNode)
		{
			nameToDefNodesMap[varDefPair.first].push_back(cfgNode);
		}
	}

	//Build an iterated dominance frontier for this function
	map<CFGNode, CFGNode> iPostDominatorMap;
	map<CFGNode, set<CFGNode> > domFrontiers = calculateDominanceFrontiers<CFGNode, CFGEdge>(function, NULL, &iPostDominatorMap);

	//Calculate control dependencies (for annotating the phi functions in the future)
	multimap< CFGNode, pair<CFGNode, CFGEdge> > controlDependencies =
			calculateControlDependence<CFGNode, CFGEdge>(function, iPostDominatorMap);

	//Find the phi function locations for each variable
	map<VarName, vector<CFGNode> >::const_iterator nameToDefNodesIter = nameToDefNodesMap.begin();
	for(; nameToDefNodesIter != nameToDefNodesMap.end(); ++nameToDefNodesIter)
	{
		const VarName& var = nameToDefNodesIter->first;
		const vector<CFGNode>& definitionPoints = nameToDefNodesIter->second;
		
		ROSE_ASSERT(!definitionPoints.empty() && "We have a variable that is not defined anywhere!");

		//Calculate the iterated dominance frontier
		set<CFGNode> phiNodes = calculateIteratedDominanceFrontier(domFrontiers, definitionPoints);

		if (getDebug())
			printf("Variable %s has phi nodes inserted at\n", varnameToString(var).c_str());

		foreach (const CFGNode& phiNode, phiNodes)
		{
			//We don't want to insert phi defs for functions that have gone out of scope
			if (!isVarInScope(var, phiNode.getNode()))
				continue;
			
			ReachingDefPtr phiDef =  ReachingDefPtr(new ReachingDef(phiNode, ReachingDef::PHI_FUNCTION));
			reachingDefTable[phiNode].insert(make_pair(var, phiDef));
		}
	}

	return controlDependencies;
}


void StaticSingleAssignment::renumberAllDefinitions(SgFunctionDefinition* func, const vector<CFGNode>& cfgNodesInPostOrder)
{
	//Map from each name to the next index. Not in map means 0
	map<VarName, int> nameToNextIndexMap;

	//We process nodes in reverse postorder; this provides a natural numbering for definitions
	reverse_foreach(const CFGNode& cfgNode, cfgNodesInPostOrder)
	{
		CFGNodeToDefTableMap::const_iterator reachingDefsIter = reachingDefTable.find(cfgNode);
		if (reachingDefsIter != reachingDefTable.end())
		{		
			//Iterate over all the phi functions inserted at this node.
			foreach (const NodeReachingDefTable::value_type& varDefPair, reachingDefsIter->second)
			{
				const VarName& definedVar = varDefPair.first;
				ReachingDefPtr reachingDef = varDefPair.second;

				//At this point, only phi functions should be in the reaching defs table. Everything else is local defs
				ROSE_ASSERT(reachingDef->isPhiFunction());

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
		
		//Number the real (non-phi) definitions
		CFGNodeToDefTableMap::const_iterator localDefIter = localDefTable.find(cfgNode);
		if (localDefIter != localDefTable.end())
		{
			foreach (const NodeReachingDefTable::value_type& varDefPair, localDefIter->second)
			{
				const VarName& definedVar = varDefPair.first;
				ReachingDefPtr reachingDef = varDefPair.second;

				//At this point, only phi functions should be in the reaching defs table. Everything else is local defs
				ROSE_ASSERT(!reachingDef->isPhiFunction());

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
}

/*static*/
vector<CFGNode> StaticSingleAssignment::getCfgNodesInPostorder(SgFunctionDefinition* func)
{
	struct RecursiveDFS
	{
		static void depthFirstSearch(const CFGNode& cfgNode, set<CFGNode>& visited, vector<CFGNode>& result)
		{
			//First, make sure this node hasn't been visited yet
			if (visited.count(cfgNode) != 0)
				return;
			
			visited.insert(cfgNode);
			
			//Now, visit all the node's successors
			reverse_foreach(const CFGEdge& outEdge, cfgNode.outEdges())
			{
				depthFirstSearch(outEdge.target(), visited, result);
			}
			
			//Add this node to the postorder list
			result.push_back(cfgNode);
		}
	};
	
	ROSE_ASSERT(func != NULL);

	set<CFGNode> visited;
	vector<CFGNode> results;
	CFGNode entry = func->cfgForBeginning();

	RecursiveDFS::depthFirstSearch(entry, visited, results);
	
	return results;
}
