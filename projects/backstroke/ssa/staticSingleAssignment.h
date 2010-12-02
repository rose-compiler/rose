//Author: George Vulov <georgevulov@hotmail.com>
//Based on work by Justin Frye <jafrye@tamu.edu>

#pragma once

#include <sage3basic.h>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <ostream>
#include <fstream>
#include <sstream>
#include <boost/foreach.hpp>
#include "filteredCFG.h"
#include <boost/unordered_map.hpp>
#include "reachingDef.h"
#include "controlPredicate.h"

namespace ssa_private
{

/** Class holding a unique name for a variable. Is attached to varRefs as a persistant attribute.
 * This is used to assign absolute names to VarRefExp nodes during VariableRenaming.
 */
class VarUniqueName : public AstAttribute
{
private:

	/** The vector of initializedNames that uniquely identifies this VarRef.
	 *  The node which this name is attached to should be the last in the list.
	 */
	std::vector<SgInitializedName*> key;

	bool usesThis;

public:

	/** Constructs the attribute with an empty key.
	 */
	VarUniqueName() : key(), usesThis(false) { }

	/** Constructs the attribute with value thisNode.
	 *
	 * The key will consist of only the current node.
	 *
	 * @param thisNode The node to use for the key.
	 */
	VarUniqueName(SgInitializedName* thisNode) : usesThis(false)
	{
		key.push_back(thisNode);
	}

	/** Constructs the attribute using the prefix vector and thisNode.
	 *
	 * The key will first be copied from the prefix value, and then the thisNode
	 * value will be appended.
	 *
	 * @param prefix The prefix of the new name.
	 * @param thisNode The node to append to the end of the new name.
	 */
	VarUniqueName(const std::vector<SgInitializedName*>& prefix, SgInitializedName* thisNode) : usesThis(false)
	{
		key.assign(prefix.begin(), prefix.end());
		key.push_back(thisNode);
	}

	/** Copy the attribute.
	 *
	 * @param other The attribute to copy from.
	 */
	VarUniqueName(const VarUniqueName& other) : usesThis(false)
	{
		key.assign(other.key.begin(), other.key.end());
	}

	VarUniqueName* copy()
	{
		VarUniqueName* newName = new VarUniqueName(*this);
		return newName;
	}

	/** Get a constant reference to the name.
	 *
	 * @return Constant Reference to the name.
	 */
	std::vector<SgInitializedName*>& getKey()
	{
		return key;
	}

	/** Set the value of the name.
	 *
	 * @param newKey The new name to use.
	 */
	void setKey(const std::vector<SgInitializedName*>& newKey)
	{
		key.assign(newKey.begin(), newKey.end());
	}

	bool getUsesThis()
	{
		return usesThis;
	}

	void setUsesThis(bool uses)
	{
		usesThis = uses;
	}

	/** Get the string representing this uniqueName
	 *
	 * @return The name string.
	 */
	std::string getNameString()
	{
		std::string name = "";
		std::vector<SgInitializedName*>::iterator iter;
		if (usesThis)
			name += "this->";
		for (iter = key.begin(); iter != key.end(); ++iter)
		{
			if (iter != key.begin())
			{
				name += ":";
			}
			name += (*iter)->get_name().getString();
		}

		return name;
	}
};

/** Struct containing a filtering function to determine what CFG nodes
 * are interesting during the DefUse traversal.
 */
struct IsDefUseFilter
{

	/** Determines if the provided CFG node should be traversed during DefUse.
	 *
	 * @param cfgn The node in question.
	 * @return Whether it should be traversed.
	 */
	bool operator() (CFGNode cfgn) const
	{
		SgNode *node = cfgn.getNode();

		//If it is the last node in a function call, keep it
		if (isSgFunctionCallExp(node) && cfgn == node->cfgForEnd())
			return true;

		//The begin edges of basic blocks are not considered interesting, but we would like to keep them
		//This is so we can propagate reachable defs to the top of a basic block
		if (isSgBasicBlock(node) && cfgn == node->cfgForBeginning())
			return true;

		//Remove all non-interesting nodes
		if (!cfgn.isInteresting())
			return false;

		//Remove all non-beginning nodes for initNames
		if (isSgInitializedName(node) && cfgn != node->cfgForBeginning())
			return false;

		//Remove non-beginning nodes for try statements
		if (isSgTryStmt(node) && cfgn != node->cfgForBeginning())
			return false;

		//Remove the midde node for logical operators with short circuiting.
		//E.g. && appears in the CFG between its LHS and RHS operands. We remove it
		//FIXME: This removes some branches in the CFG. There should be a better way to address this
		if (isSgAndOp(node) || isSgOrOp(node))
		{
			if (cfgn != node->cfgForEnd())
				return false;
		}

		//We only want the middle appearance of the teritatry operator - after its conditional expression
		//and before the true and false bodies. This makes it behave as an if statement for data flow
		//purposes
		if (isSgConditionalExp(node))
		{
			return cfgn.getIndex() == 1;
		}

		return true;
	}
};

} //namespace ssa_private

/** Class that defines an VariableRenaming of a program
 *
 * Contains all the functionality to implement variable renaming on a given program.
 * For this class, we do not actually transform the AST directly, rather
 * we perform the analysis and add attributes to the AST nodes so that later
 * optimizations can access the results of this analysis while still preserving
 * the original AST.
 */
class StaticSingleAssignment
{
private:
	/** The project to perform SSA Analysis on. */
	SgProject* project;

public:
	
	/** A compound variable name as used by the variable renaming.  */
	typedef std::vector<SgInitializedName*> VarName;

	/** Describes the defs or uses at each node. This is for local, rather than propagated, information. */
	typedef boost::unordered_map<SgNode*, std::set<VarName> > LocalDefUseTable;

	/** A filtered CFGNode that is used for DefUse traversal.  */
	typedef FilteredCFGNode<ssa_private::IsDefUseFilter> FilteredCfgNode;
	
	/** A filtered CFGEdge that is used for DefUse traversal.  */
	typedef FilteredCFGEdge<ssa_private::IsDefUseFilter> FilteredCfgEdge;

	typedef boost::shared_ptr<ReachingDef> ReachingDefPtr;

	/** A map from each variable to its reaching definitions at the current node. */
	typedef std::map<VarName, ReachingDefPtr> NodeReachingDefTable;

	/** The first table is the IN table. The second table is the OUT table. */
	typedef boost::unordered_map<SgNode*, std::pair<NodeReachingDefTable, NodeReachingDefTable> > GlobalReachingDefTable;

	/** Map from each node to the variables used at that node and their reaching definitions. */
	typedef boost::unordered_map<SgNode*, NodeReachingDefTable> UseTable;

	typedef ssa_private::ControlPredicate<FilteredCfgEdge> CfgPredicate;

private:
	//Private member variables

	/** This is the table of variable definition locations that is generated by
	 * the VarDefUseTraversal. It is later used to populate the actual def/use table.
	 * It maps each node to the variable names that are defined inside that node.
	 */
	 LocalDefUseTable originalDefTable;

	/** This is the table of definitions that is expanded from the original table.
	 * It is used to populate the actual def/use table.
	 * It maps each node to the variable names that are defined inside that node.
	 */
	LocalDefUseTable expandedDefTable;

	/** Maps each node to the reaching definitions at that node.
	 * The table is populated with phi functions using iterated dominance frontiers, and then
	 * is filled through dataflow. */
	GlobalReachingDefTable reachingDefsTable;

	/** This is the table that is populated with all the use information for all the variables
	 * at all the nodes. It is populated during the runDefUse function, and is done
	 * with the steady-state dataflow algorithm.
	 * For each node, the table contains all the variables that were used at that node, and maps them
	 * to the reaching definitions for each use.
	 */
	LocalDefUseTable localUsesTable;

	/** Map from each node to the variables used at that node and their reaching definitions. */
	UseTable useTable;

	/** Local definitions (actual definitions, not phi definitions). */
	boost::unordered_map<SgNode*, NodeReachingDefTable> ssaLocalDefTable;

public:

	StaticSingleAssignment(SgProject* proj) : project(proj) { }

	~StaticSingleAssignment() { }

	void run();

	static bool getDebug()
	{
		return SgProject::get_verbose() > 0;
	}

	static bool getDebugExtra()
	{
		return SgProject::get_verbose() > 1;
	}

private:
	void runDefUseDataFlow(SgFunctionDefinition* func);

	/** Returns true if the variable is implicitly defined at the function entry by the compiler. */
	bool isBuiltinVar(const VarName& var);

	/** Expand all member definitions (chained names) to define every name in the chain
	 * that is shorter than the originally defined name.
	 *
	 * When a member of a struct/class is referenced, this will insert definitions
	 * for every member referenced to access the currently referenced one.
	 *
	 * ex.   Obj o;         //Declare o of type Obj
	 *       o.a.b = 5;     //Def for o.a.b
	 *
	 * In the second line, this function will insert the following:
	 *
	 *       o.a.b = 5;     //Def for o.a.b, o.a, o
	 */
	void expandParentMemberDefinitions(SgFunctionDeclaration* function);

	/** Expand all member uses (chained names) to explicitly use every name in the chain that is a
	 * parent of the original use.
	 *
	 * When a member of a struct/class is used, this will insert uses for every
	 * member referenced to access the currently used one.
	 *
	 * ex.   Obj o;         //Declare o of type Obj
	 *       int i;         //Declare i of type int
	 *       i = o.a.b;     //Def for i, use for o.a.b
	 *
	 * In the third line, this function will insert the following:
	 *
	 *       i = o.a.b;     //Def for i, use for o.a.b, o.a, o
	 *
	 * @param curNode
	 */
	void expandParentMemberUses(SgFunctionDeclaration* function);

	/** Find all uses of compound variable names and insert expanded defs for them when their
	 * parents are defined. E.g. for a.x, all defs of a will have a def of a.x inserted.
	 * Note that there might be other child expansions of a, such as a.y, that we do not insert since
	 * they have no uses. */
	void insertDefsForChildMemberUses(SgFunctionDeclaration* function);

	/** Insert defs for functions that are declared outside the function scope. */
	void insertDefsForExternalVariables(SgFunctionDeclaration* function);

	/** Returns a set of all the variables names that have uses in the subtree. */
	std::set<VarName> getVarsUsedInSubtree(SgNode* root);

	/** Find where phi functions need to be inserted and insert empty phi functions at those nodes.
	 * This updates the IN part of the reaching def table with Phi functions.
	 * @returns the control dependencies. */
	std::multimap< FilteredCfgNode, std::pair<FilteredCfgNode, FilteredCfgEdge> > insertPhiFunctions(SgFunctionDefinition* function);

	/** Create ReachingDef objects for each local def and insert them in the local def table. */
	void populateLocalDefsTable(SgFunctionDeclaration* function);

	/** Give numbers to all the reachingDef objects. Should be called after phi functions are inserted
	 * and the local def table is populated, but before dataflowp propagates the definitions. */
	void renumberAllDefinitions(SgFunctionDefinition* function);

	/** Take all the outgoing defs from previous nodes and merge them as the incoming defs
	 * of the current node. */
	void updateIncomingPropagatedDefs(FilteredCfgNode cfgNode);

	/** Performs the data-flow update for one individual node, populating the reachingDefsTable for that node.
	 * @returns true if the OUT defs from the node changed, false if they stayed the same. */
	bool propagateDefs(FilteredCfgNode cfgNode);

	/** Once all the reaching def information has been propagated, uses the reaching def information and the local
	 * use information to match uses to their reaching defs. */
	void buildUseTable(SgFunctionDefinition* func);

	void annotatePhiNodeWithConditions(SgFunctionDefinition* function,
			const std::multimap< FilteredCfgNode, std::pair<FilteredCfgNode, FilteredCfgEdge> > & controlDependencies);

	CfgPredicate getConditionsForNodeExecution(SgNode* node, const std::vector<FilteredCfgEdge>& stopEdges,
		const std::multimap<SgNode*, FilteredCfgEdge> & controlDependencies, std::set<SgNode*>& visited);

	void printToDOT(SgSourceFile* file, std::ofstream &outFile);
	void printToFilteredDOT(SgSourceFile* file, std::ofstream &outFile);

public:
	//External static helper functions/variables
	/** Tag to use to retrieve unique naming key from node.  */
	static std::string varKeyTag;

	static VarName emptyName;

	/*
	 *  Printing functions.
	 */

	/** Print the CFG with any UniqueNames and Def/Use information visible.
	 *
	 * @param fileName The filename to save graph as. Filenames will be prepended.
	 */
	void toDOT(const std::string fileName);

	/** Print the CFG with any UniqueNames and Def/Use information visible.
	 *
	 * This will only print the nodes that are of interest to the filter function
	 * used by the def/use traversal.
	 *
	 * @param fileName The filename to save graph as. Filenames will be prepended.
	 */
	void toFilteredDOT(const std::string fileName);

	void printOriginalDefs(SgNode* node);
	void printOriginalDefTable();


	/*
	 *   Def/Use Table Access Functions
	 */

	/** Get the table of definitions for every node.
	 * These definitions are NOT propagated.
	 *
	 * @return Definition table.
	 */
	LocalDefUseTable& getOriginalDefTable()
	{
		return originalDefTable;
	}

	LocalDefUseTable& getUseTable()
	{
		return localUsesTable;
	}

	/** Returns the reaching definitions at the given node. If there is a definition at the node itself,
	  * e.g. SgAssignOp, it is considered to reach the node. */
	const NodeReachingDefTable getReachingDefsAtNode(SgNode* node) const;

	/** Returns a list of all the variables used at this node. Note that uses don't propagate past an SgStatement.
	  * Each use is mapped to the reaching definition to which the use corresponds. */
	const NodeReachingDefTable getUsesAtNode(SgNode* node) const;
	
	/*
	 *   Static Utility Functions
	 */

	/** Find if the given prefix is a prefix of the given name.
	 *
	 * This will return whether the given name has the given prefix inside it.
	 *
	 * ex. a.b.c has prefix a.b, but not a.c
	 *
	 * @param name The name to search.
	 * @param prefix The prefix to search for.
	 * @return Whether or not the prefix is in this name.
	 */
	static bool isPrefixOfName(VarName name, VarName prefix);

	/** Get the uniqueName attribute for the given node.
	 *
	 * @param node Node to get the attribute from.
	 * @return The attribute, or NULL.
	 */
	static ssa_private::VarUniqueName* getUniqueName(SgNode* node);

	/** Get the variable name of the given node.
	 *
	 * @param node The node to get the name for.
	 * @return The name, or empty name.
	 */
	static VarName getVarName(SgNode* node);

	/** Gets whether or not the initializedName is from a library.
	 *
	 * This method checks if the variable is compiler generated, and if its
	 * filename has "/include/" in it. If so, it will return true. Otherwise, it returns
	 * false.
	 *
	 * @param initName The SgInitializedName* to check.
	 * @return true if initName is from a library, false if otherwise.
	 */
	static bool isFromLibrary(SgNode* node);

	/** Get an AST fragment containing the appropriate varRefs and Dot/Arrow ops to access the given variable.
	 *
	 * @param var The variable to construct access for.
	 * @param scope The scope within which to construct the access.
	 * @return An expression that access the given variable in the given scope.
	 */
	static SgExpression* buildVariableReference(const VarName& var, SgScopeStatement* scope = NULL);

	/** Finds the scope of the given node, and returns true if the given
	 * variable is accessible there. False if the variable is not accessible. */
	static bool isVarInScope(const VarName& var, SgNode* scope);

	/** Get a string representation of a varName.
	 *
	 * @param vec varName to get string for.
	 * @return String for given varName.
	 */
	static std::string varnameToString(const VarName& vec);

	static void printLocalDefUseTable(const LocalDefUseTable& table);
};


