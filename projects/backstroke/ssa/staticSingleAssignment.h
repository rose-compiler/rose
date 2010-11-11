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
	/** The project to perform SSA Analysis on.
	 */
	SgProject* project;

public:
	
	/** Vector of SgNode*  */
	typedef std::vector<SgNode*> NodeVec;
	
	/** A compound variable name as used by the variable renaming.  */
	typedef std::vector<SgInitializedName*> VarName;
	
	/** An entry in the rename table mapping a name to a nodeVec.  */
	typedef std::map<VarName, NodeVec> TableEntry;
	
	/** A table storing the name->node mappings for every node in the program. */
	typedef boost::unordered_map<SgNode*, TableEntry> DefUseTable;
	
	/** A table mapping a name to a single node. */
	typedef boost::unordered_map<VarName, SgNode*> FirstDefTable;
	
	/** A list of names. */
	typedef std::vector<VarName> GlobalTable;
	
	/** A vector of SgInitializedName* */
	typedef std::vector<SgInitializedName*> InitNameVec;
	
	/** A filtered CFGNode that is used for DefUse traversal.  */
	typedef FilteredCFGNode<ssa_private::IsDefUseFilter> FilteredCfgNode;
	
	/** A filtered CFGEdge that is used for DefUse traversal.  */
	typedef FilteredCFGEdge<ssa_private::IsDefUseFilter> FilteredCfgEdge;

	/** A vector of cfgNodes. */
	typedef std::vector<FilteredCfgNode> cfgNodeVec;

	/** A vector of cfgEdges. */
	typedef std::vector<FilteredCfgEdge> cfgEdgeVec;
	
	/** An entry in the rename table that maps a node to a number.  */
	typedef std::map<SgNode*, int> NodeNumRenameEntry;

	/** A table that maps a name to it's node->number renamings. */
	typedef boost::unordered_map<VarName, NodeNumRenameEntry> NodeNumRenameTable;
	
	/** An entry in the rename table that maps a number to a node.  */
	typedef std::map<int, SgNode*> NumNodeRenameEntry;
	
	/** A table that maps a name to it's number->node renamings.  */
	typedef boost::unordered_map<VarName, NumNodeRenameEntry> NumNodeRenameTable;


private:
	//Private member variables

	/** This is the table of variable definition locations that is generated by
	 * the VarDefUseTraversal. It is later used to populate the actual def/use table.
	 * It maps each node to the variable names that are defined inside that node.
	 */
	boost::unordered_map<SgNode*, std::set<VarName> >  originalDefTable;

	/** This is the table of definitions that is expanded from the original table.
	 * It is used to populate the actual def/use table.
	 * It maps each node to the variable names that are defined inside that node.
	 */
	boost::unordered_map<SgNode*, std::set<VarName> > expandedDefTable;

	/** This is the table that is populated with all the def information for all the variables
	 * at all the nodes. It is populated during the runDefUse function, and is done
	 * with the steady-state dataflow algorithm.
	 * For each node, the table contains all the reaching definitions at that node.
	 */
	DefUseTable reachingDefsTable;

	/** This is the table that is populated with all the use information for all the variables
	 * at all the nodes. It is populated during the runDefUse function, and is done
	 * with the steady-state dataflow algorithm.
	 */
	DefUseTable useTable;

	/** Holds a list of the locations that a particular name is first
	 * defined.
	 *
	 * This helps when backwards-inserting definitions of member variables,
	 * so that we can simply insert the definition at the first definition.
	 */
	FirstDefTable firstDefList;

	/** This holds the mapping between variables and the nodes where they are renumbered.
	 * Given a name and a node, we can get the number of the name that is defined at that node.
	 * Nodes which do not define a name are not in the table.
	 */
	NodeNumRenameTable nodeRenameTable;

	/** This holds the mapping between variables and the nodes where they are renumbered.
	 * Given a name and a number, we can get the node where that number is defined.
	 * Nodes which do not define a name are not in the table.
	 */
	NumNodeRenameTable numRenameTable;

	/** A list of all the global varibales in the program.
	 */
	GlobalTable globalVarList;

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
	bool defUse(FilteredCfgNode node, bool *memberRefInserted, NodeVec &changedNodes);

	/** Add an entry to the renumbering table for the given var and node.
	 *
	 * This will place a new entry into the renaming table that renumbers
	 * var at node. This new definition will have the next available numbering
	 * of var. If the var @ node combination already exists, the number will
	 * be returned.
	 *
	 * @param var The variable to renumber.
	 * @param node The node that defines the new number.
	 * @return The renumbering assigned to ver @ node.
	 */
	int addRenameNumberForNode(const VarName& var, SgNode* node);

	/** Locate all global varibales and add them to the table.
	 */
	void findGlobalVars();

	bool isBuiltinVar(const VarName& var);

	/** Called to merge the defs from previous nodes in the CFG to this one.
	 *
	 * This will merge the def tables from all previous CFG nodes, merge in the
	 * defs at this node, and update this node's table if needed. If it locates a def that
	 * is not in the table, it will attempt to find the location where that member
	 * was first defined and insert a definition there. It will then set the outParameter
	 * to indicate that it back-inserted a def.
	 *
	 * @param curNode The node to merge defs onto.
	 * @return Whether the defs were different from those already on the node.
	 */
	bool mergeDefs(FilteredCfgNode curNode);

	/** Called to update the uses on the current node.
	 *
	 * This will update the uses at this node to point to the defs that were propogated
	 * from previous nodes. If it locates a use of a def that did not exist, it
	 * will attempt to locate the definition site of the base variable and insert a def
	 * there. It will then set the outParameter to indicate that it back-inserted a def.
	 *
	 * @param curNode The node to resolve uses on.
	 * @param memberRefInserted Reference that indicates whether the function back-inserted a definition.
	 */
	void resolveUses(FilteredCfgNode curNode, bool *memberRefInserted, NodeVec &changedNodes);

	/** Trace backwards in the cfg one step and return an aggregate of all previous defs.
	 *
	 * @param curNode Node to traverse backwards from.
	 * @param results TableEntry reference where results are stored.
	 */
	void aggregatePreviousDefs(FilteredCfgNode curNode, TableEntry& results);

	/** Inserts definition points for all global variables.
	 * 2. At every function call expression.
	 */
	void insertGlobalVarDefinitions();

	/** Expand all member definitions (chained names) to define every name in the chain.
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
	 *
	 * @param curNode The node to expand the definitions on.
	 */
	void expandMemberDefinitions(FilteredCfgNode curNode);

	/** Insert defs for member uses (chained names) that do not have an explicit def.
	 *
	 * When a member of a struct/class is used and that member does not have a propogated
	 * def on the current node, this will find the closest definition of a member
	 * in the ref chain and insert a definition for this use at that member's def.
	 *
	 * ex. Obj o;         //Declare o of type o
	 *     o.a = 5;       //Def for o.a
	 *     int i = o.b.x; //Def for i, use for o.b.x
	 *     int j = o.a.x; //Def for j, use for o.a.x
	 *
	 * This function will insert the following:
	 *     Obj o;         //Declare o of type o, Def for o.a, def for o.b, def for o.b.x
	 *     o.a = 5;       //Def for o.a, use for o, use for o.a, def for o.a.x
	 *     int i = o.b.x; //Def for i, use for o.b.x
	 *     int j = o.a.x; //Def for j, use for o.a.x
	 *
	 * @param curNode The node to expand the uses on.
	 * @param name The variableName to expand the uses for.
	 * @return Whether any new defs were inserted.
	 */
	bool insertExpandedDefsForUse(FilteredCfgNode curNode, VarName name, NodeVec &changedNodes);

	/** Expand all member uses (chained names) to explicitly use every name in the chain.
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
	void expandMemberUses(FilteredCfgNode curNode);

	void printToDOT(SgSourceFile* file, std::ofstream &outFile);
	void printToFilteredDOT(SgSourceFile* file, std::ofstream &outFile);

public:
	//External static helper functions/variables
	/** Tag to use to retrieve unique naming key from node.
	 */
	static std::string varKeyTag;

	/** This represents the initializedName for the 'this' keyword.
	 *
	 * This will allow the this pointer to be versioned inside member functions.
	 */
	static SgInitializedName* thisDecl;

	static VarName emptyName;

	static NumNodeRenameTable emptyRenameTable;

	static NumNodeRenameEntry emptyRenameEntry;


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

	/** Get a string representation of a varName.
	 *
	 * @param vec varName to get string for.
	 * @return String for given varName.
	 */
	static std::string keyToString(const VarName& vec);

	void printDefs(SgNode* node);

	void printOriginalDefs(SgNode* node);

	void printOriginalDefTable();

	void printUses(SgNode* node);

	void printRenameTable();

	void printRenameTable(const VarName& var);

	static void printRenameTable(const NodeNumRenameTable& table);

	static void printRenameTable(const NumNodeRenameTable& table);

	static void printRenameEntry(const NodeNumRenameEntry& entry);

	static void printRenameEntry(const NumNodeRenameEntry& entry);

	static void printUses(const TableEntry& table);
	static void printDefs(const TableEntry& table);


	/*
	 *   Def/Use Table Access Functions
	 */

	/** Get the table of definitions for every node.
	 * These definitions are NOT propagated.
	 *
	 * @return Definition table.
	 */
	boost::unordered_map<SgNode*, std::set<VarName> >& getOriginalDefTable()
	{
		return originalDefTable;
	}

	/** Get the defTable containing the propogated definition information.
	 *
	 * @return Def table.
	 */
	DefUseTable& getPropDefTable()
	{
		return reachingDefsTable;
	}

	/** Get the table of uses for every node.
	 *
	 * @return Use Table.
	 */
	DefUseTable& getUseTable()
	{
		return useTable;
	}

	/** Get the listing of global variables.
	 *
	 * @return Global Var List.
	 */
	GlobalTable& getGlobalVarList()
	{
		return globalVarList;
	}



	/*
	 *   Rename Table Access Functions
	 */

	/** Get the rename number for the given variable and the given node.
	 *
	 * This will return the number of the given variable as it is defined on the given
	 * node. If the provided node does not define the variable, the function will
	 * return -1.
	 *
	 * @param var The variable to get the renumbering for.
	 * @param node The defining node to get the renumbering at.
	 * @return The number of var @ node, or -1 if node does not define var.
	 */
	int getRenameNumberForNode(const VarName& var, SgNode* node) const;

	/** Get the node that defines the given number of the given variable.
	 *
	 * This will return the node that defines the 'num' value of var.
	 * It will be the defining node for the variable renumbered with num of the variable
	 * var. If the provided number does not exist for var, it will return NULL.
	 *
	 * @param var The variable to get the defining node for.
	 * @param num The renumbering of the defining node to get.
	 * @return The defining node of var:num, or NULL if the renumbering does not exist.
	 */
	SgNode* getNodeForRenameNumber(const VarName& var, int num) const;

	/** Get the number of the last rename of the given variable.
	 *
	 * This will return the number of the last renaming of the given variable.
	 * If the given variable has no renamings, it will return -1.
	 *
	 * @param var The variable to get the last renaming for.
	 * @return The highest renaming number, or -1 if var is not renamed.
	 */
	int getMaxRenameNumberForName(const VarName& var) const;



	/*
	 *   Variable Renaming Access Functions
	 */

	/** Retrieve a list of nodes that use the var:num specified.
	 *
	 * This will retrieve a list of nodes that use the specified var:num combo.
	 * ex.  int i = s2.y1;  //Search for s:y1 will yield varRef for y1, as well as
	 *                      //the DotExpr and the AssignOp
	 *
	 * @param var The variable name to find.
	 * @param num The revision of the variable to find.
	 * @return A vector containing the usage nodes of the variable. Empty vector otherwise.
	 */
	NodeVec getAllUsesForDef(const VarName& var, int num);

	/** Retrieve a list of nodes of type T that use the var:num specified.
	 *
	 * This will retrieve a list of nodes of type T that use the specified var:num combo.
	 * ex.  int i = s2.y1;  //Search for s:y1,AssignOp will yield the AssignOp
	 *
	 * @param var The variable name to find.
	 * @param num The revision of the variable to find.
	 * @return A vector containing the usage nodes of the variable. Empty vector otherwise.
	 */
	template<typename T> inline std::vector<T*> getAllUsesForDef(const VarName& var, int num)
	{
		NodeVec vec = getAllUsesForDef(var, num);
		std::vector<T*> res;
		T* temp = NULL;

		BOOST_FOREACH(NodeVec::value_type& val, vec)
		{
			temp = dynamic_cast<T*> (val);
			if (temp != NULL)
			{
				//Push back if it casts correctly.
				res.push_back(temp);
			}
		}

		return res;
	}

	/** Get name:num mappings for all reaching definitions of all variables at the node.
	 *
	 * @param node The node to retrieve reaching definitions for.
	 * @return A table mapping VarName->(num, defNode). Empty table otherwise.
	 */
	NumNodeRenameTable getReachingDefsAtNode(SgNode* node);

	/** Get name:num mapping for all reaching definitions of the given variable at the node.
	 *
	 * @param node The node to retrieve reaching definitions for.
	 * @param var The variable to retrieve definitions of.
	 * @return A table of (num, defNode) for the given variable. Empty table otherwise.
	 */
	NumNodeRenameEntry getReachingDefsAtNodeForName(SgNode* node, const VarName& var);

	/** Get the final versions if all variables at the end of the given scope.
	 *
	 * @param bb The scope to get variables for.
	 * @return A table of VarName->(num, defNode) for all variables at the end of the scope. Empty table otherwise.
	 */
	NumNodeRenameTable getReachingDefsAtScopeEnd(SgScopeStatement* scope);

	/** Get the final versions if all variables at the end of the given function.
	 *
	 * @param node The function to get variables for.
	 * @return A table of VarName->(num, defNode) for all variables at the end of the function. Empty table otherwise.
	 */
	NumNodeRenameTable getReachingDefsAtFunctionEnd(SgFunctionDefinition* node);

	/** Get the versions of a variable at the end of the given function.
	 *
	 * @param node The function definition to get definitions for.
	 * @param var The varName to get definitions for.
	 * @return A table of (num, defNode) for the given variable. Empty table otherwise.
	 */
	NumNodeRenameEntry getReachingDefsAtFunctionEndForName(SgFunctionDefinition* node, const VarName& var);

	/** Gets the versions of all variables reaching a statment before its execution. Notice that this method and 
	 * getReachingDefsAtNode potentially return different values for loops. With loops, variable values from the body
	 * of the loop flow to the top; hence getReachingDefsAtNode returns definitions from the loop body. On the other hand,
	 * getReachingDefsAtStatementStart does not return definitions coming in from a loop body.
	 * 
	 * @param statement
	 * @return A table of VarName->(num, defNode) for all variables at the beginning of the statement
	 */
	NumNodeRenameTable getReachingDefsAtStatementStart(SgStatement* statement);

	/** Get the versions of all variables at the start of the given function.
	 *
	 * @param node The function to get variables for.
	 * @return A table of VarName->(num, defNode) for all variables at the start of the function. Empty table otherwise.
	 */
	NumNodeRenameTable getReachingDefsAtFunctionStart(SgFunctionDefinition* node);

	/** Get the versions of a variable at the start of the given function.
	 *
	 * @param node The function definition to get definitions for.
	 * @param var The varName to get definitions for.
	 * @return A table of (num, defNode) for the given variable. Empty table otherwise.
	 */
	NumNodeRenameEntry getReachingDefsAtFunctionStartForName(SgFunctionDefinition* node, const VarName& var);

	/** Get name:num mappings for all uses at this node. For example, if p.x appears,
	 * there will be a use for both p and p.x
	 *
	 * @param node The node to get uses for.
	 * @return A table mapping VarName->(num, defNode) for every varName used at node. Empty table otherwise.
	 */
	NumNodeRenameTable getUsesAtNode(SgNode* node);

	/** Get name:num mappings for the original uses at this node. For example, if p.x appears,
	 * there will be a use for p.x, but not for p.
	 *
	 * @param node The node to get uses for.
	 * @return A table mapping VarName->(num, defNode) for every varName used at node. Empty table otherwise.
	 */
	NumNodeRenameTable getOriginalUsesAtNode(SgNode* node);

	/** Get name:num mapping for use of the given variable at this node.
	 *
	 * @param node The node to get uses for.
	 * @param var The varName to get the uses for.
	 * @return  A table of (num, defNode) for the given varName used at node. Empty table otherwise.
	 */
	NumNodeRenameEntry getUsesAtNodeForName(SgNode* node, const VarName& var);

	/** Get name:num mapping for all defs at the given node.
	 *
	 * This will return the combination of original and expanded defs on this node.
	 *
	 * ex. s.x = 5;  //This will return s.x and s  (s.x is original & s is expanded)
	 *
	 * @param node The node to get defs for.
	 * @return A table mapping VarName->(num, defNode) for every varName defined at node. Empty table otherwise.
	 */
	NumNodeRenameTable getDefsAtNode(SgNode* node);

	/** Get name:num mapping for def of the specified variable at the given node.
	 *
	 * This will return the combination of original and expanded defs of the given variable on this node.
	 *
	 * ex. s.x = 5;   //(s is expanded & s.x is original)
	 *     Looking for s will return this node, even though s is an expanded definition.
	 *
	 * @param node The node to get defs for.
	 * @param var The variable to get defs for.
	 * @return A table mapping VarName->(num, defNode) for every varName defined at node. Empty table otherwise.
	 */
	NumNodeRenameEntry getDefsAtNodeForName(SgNode* node, const VarName& var);

	/** Get name:num mapping for all original defs at the given node.
	 *
	 * This will return the original defs on this node.
	 *
	 * ex. s.x = 5;  //This will return s.x  (s.x is original & s is expanded)
	 *
	 * @param node The node to get defs for.
	 * @return A table mapping VarName->(num, defNode) for every varName originally defined at node. Empty table otherwise.
	 */
	NumNodeRenameTable getOriginalDefsAtNode(SgNode* node);

	/** Get name:num mapping for def of the specified variable at the given node.
	 *
	 * This will return the combination of original defs of the given variable on this node.
	 *
	 * ex. s.x = 5;   //(s is expanded & s.x is original)
	 *     Looking for s.x will return this node, s will return empty
	 *
	 * @param node The node to get defs for.
	 * @param var The variable to get defs for.
	 * @return A table mapping VarName->(num, defNode) for every varName defined at node. Empty table otherwise.
	 */
	NumNodeRenameEntry getOriginalDefsAtNodeForName(SgNode* node, const VarName& var);

	/** Get name:num mapping for all expanded defs at the given node.
	 *
	 * This will return the expanded defs on this node.
	 *
	 * ex. s.x = 5;  //This will return s  (s.x is original & s is expanded)
	 *
	 * @param node The node to get defs for.
	 * @return A table mapping VarName->(num, defNode) for every varName defined via expansion at node. Empty table otherwise.
	 */
	NumNodeRenameTable getExpandedDefsAtNode(SgNode* node);

	/** Get name:num mapping for def of the specified variable at the given node.
	 *
	 * This will return the combination of expanded defs of the given variable on this node.
	 *
	 * ex. s.x = 5;   //(s is expanded & s.x is original)
	 *     Looking for s will return this node, s.x will return empty
	 *
	 * @param node The node to get defs for.
	 * @param var The variable to get defs for.
	 * @return A table mapping VarName->(num, defNode) for every varName defined at node. Empty table otherwise.
	 */
	NumNodeRenameEntry getExpandedDefsAtNodeForName(SgNode* node, const VarName& var);

	/** Get all definitions for the subtree rooted at this node. If m.x is defined,
	 * the resulting table will also include a definition for m.
	 *
	 * @param node The root of the subtree to get definitions for.
	 * @return The table mapping VarName->(num, node) for every definition.
	 */
	NumNodeRenameTable getDefsForSubtree(SgNode* node);

	/** Get all original definitions for the subtree rooted at this node. No expanded definitions
	 * will be included - for example, if m.x is defined, there will be no definition for the structure m.
	 *
	 * @param node The root of the subtree to get definitions for.
	 * @return The table mapping VarName->(num, node) for every definition.
	 */
	NumNodeRenameTable getOriginalDefsForSubtree(SgNode* node);

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

};


