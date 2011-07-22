//Author: George Vulov <georgevulov@hotmail.com>
//Based on work by Justin Frye <jafrye@tamu.edu>

#pragma once

#include <rose.h>
#include <string>
#include <iostream>
#include <map>
#include <set>
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
#include "dataflowCfgFilter.h"
#include "CallGraph.h"
#include "uniqueNameTraversal.h"

namespace ssa_private
{

/** This filter determines which function declarations get processed in the analysis. */
struct FunctionFilter
{
	bool operator()(SgFunctionDeclaration* funcDecl)
	{
		ROSE_ASSERT(funcDecl != NULL);

		//Don't process any built-in functions
		std::string filename = funcDecl->get_file_info()->get_filename();

		//Exclude compiler generated functions, but keep template instantiations
		if (funcDecl->get_file_info()->isCompilerGenerated() && !isSgTemplateInstantiationFunctionDecl(funcDecl)
				&& !isSgTemplateInstantiationMemberFunctionDecl(funcDecl))
			return false;

		//We don't process functions that don't have definitions
		if (funcDecl->get_definingDeclaration() == NULL)
			return false;

		return true;
	}
};

} //namespace ssa_private

/** Static single assignment analysis.
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

	typedef boost::shared_ptr<ReachingDef> ReachingDefPtr;

	/** A map from each variable to its reaching definitions at the current node. */
	typedef std::map<VarName, ReachingDefPtr> NodeReachingDefTable;

	typedef std::map<CFGNode, std::set<VarName> > CFGNodeToVarNamesMap;
	
	typedef std::map<SgNode*, std::set<VarName> > ASTNodeToVarNamesMap;
	
	typedef std::map<SgNode*, std::set<SgVarRefExp*> > ASTNodeToVarRefsMap;
	
	typedef std::map<CFGNode, NodeReachingDefTable> CFGNodeToDefTableMap;
	
private:

	ASTNodeToVarRefsMap astNodeToUses;

	/** Map from each node to all the variables defined at that node. If there are no variables defined at the node,
      * the corresponding entry is empty. */
	CFGNodeToDefTableMap localDefTable;
	
    /** Map from each node to all the reaching definitions at that node. If a definition occurs at the node itself,
     * it is not considered to reach the node. */
	CFGNodeToDefTableMap reachingDefTable;
	
    /** Map from each node to the reaching definitions at the end of the node. If no definitions occur at the node,
     * its entry in this table should be the same as its entry in the reachingDef table. */
    CFGNodeToDefTableMap outgoingDefTable;
    
public:

	StaticSingleAssignment(SgProject* proj) : project(proj) { }

	~StaticSingleAssignment() { }

	/** Run the analysis. If interprocedural analysis is not enabled, function call expressions (SgFunctionCallExp) will not
	 * count as definitions of any variables.
	 * @param interprocedural true to enable interprocedural analysis, false to perform no interprocedural analysis. */
	void run(bool interprocedural);

	static bool getDebug()
	{
		return SgProject::get_verbose() > 0;
	}

	static bool getDebugExtra()
	{
		return SgProject::get_verbose() > 1;
	}

private:
	/** Once all the local definitions have been inserted in the ssaLocalDefsTable and phi functions have been inserted
	  * in the reaching defs table, propagate reaching definitions along the CFG. */
	void runDefUseDataFlow(SgFunctionDefinition* func);

	/** Returns true if the variable is implicitly defined at the function entry by the compiler. */
	static bool isBuiltinVar(const VarName& var);

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
	void expandParentMemberDefinitions(const CFGNodeToVarNamesMap& defs);
	
	/** Find all uses of compound variable names and insert expanded defs for them when their
	 * parents are defined. E.g. for a.x, all defs of a will have a def of a.x inserted.
	 * Note that there might be other child expansions of a, such as a.y, that we do not insert since
	 * they have no uses. */
	void insertDefsForChildMemberUses(const CFGNodeToVarNamesMap& defs, const std::set<VarName>& usedNames);

	/** Insert defs for functions that are declared outside the function scope. */
	void insertDefsForExternalVariables(SgFunctionDefinition* function, const std::set<VarName>& usedNames);

	/** Find where phi functions need to be inserted and insert empty phi functions at those nodes.
	 * This updates the IN part of the reaching def table with Phi functions.
	 * 
	 * @param cfgNodesInPostOrder all the CFG nodes of the function
	 * @returns the control dependencies. */
	std::multimap< CFGNode, std::pair<CFGNode, CFGEdge> > insertPhiFunctions(SgFunctionDefinition* function,
						const std::vector<CFGNode>& cfgNodesInPostOrder);

	/** Give numbers to all the reachingDef objects. Should be called after phi functions are inserted
	 * and the local def table is populated, but before dataflow propagates the definitions. 
	 * 
	 * @param cfgNodesInPostOrder a list of all the CFG nodes in the function, in postorder. */
	void renumberAllDefinitions(SgFunctionDefinition* func, const std::vector<CFGNode>& cfgNodesInPostOrder);

	/** Take all the outgoing defs from previous nodes and merge them as the incoming defs
	 * of the current node. */
	void updateIncomingPropagatedDefs(const CFGNode& cfgNode);

	/** Performs the data-flow update for one individual node, populating the reachingDefsTable for that node.
	 * @returns true if the OUT defs from the node changed, false if they stayed the same. */
	bool propagateDefs(const CFGNode& cfgNode);

	/** Iterates all the CFG nodes in the function and returns them in postorder, according to depth-first search.
	 * Reverse postorder is the most efficient order for dataflow propagation. */
	static std::vector<CFGNode> getCfgNodesInPostorder(SgFunctionDefinition* func);

	//------------ INTERPROCEDURAL ANALYSIS FUNCTIONS ------------ //

	/** Insert definitions at function call sites for all variables defined interprocedurally. Iterates on the
	 * call graph until the definitions converge (hence it works with recursion).
	 * @param interestinFunctions all functions that should be analyzed. */
	void interproceduralDefPropagation(const boost::unordered_set<SgFunctionDefinition*>& interestingFunctions);

	/** This function returns the order in which functions should be processed so that callees are processed before
	 * callers whenever possible (this is sometimes not possible due to recursion). Internally, it builds a call graph
	 * and constructs a depth-first ordering of it. */
	std::vector<SgFunctionDefinition*> calculateInterproceduralProcessingOrder(
					const boost::unordered_set<SgFunctionDefinition*>& interestingFunctions);

	/** Add all the callees of the function to the processing list, then add the function itself. Does nothing
	  * if the function is already in the list. */
	void processCalleesThenFunction(SgFunctionDefinition* targetFunction, SgIncidenceDirectedGraph* callGraph,
		boost::unordered_map<SgFunctionDefinition*, SgGraphNode*> graphNodeToFunction,
		std::vector<SgFunctionDefinition*> &processingOrder);

	/** Add definitions at function call expressions for variables that are modified interprocedurally.
	 * The definitions are inserted in the original def table.
	 * @param funcDef function whose body should be queries for function calls
	 * @param processed all the functions completely processed by SSA. If a callee is one of these functions,
	 *			we can use exact information.
	 * @return true if new defs were inserted, false otherwise. */
	bool insertInterproceduralDefs(SgFunctionDefinition* funcDef, const boost::unordered_set<SgFunctionDefinition*>& processed,
		ClassHierarchyWrapper* classHierarchy);

	/** Insert the interprocedural defs at a particular call site for a particular callee. This function may be called
	 * multiple times for the same call site with different callees (e.g. in the case of virtual functions).
	 * The call site should either be a SgFunctionCallExp or SgConstructorInitializer
	 * @param processed functions already processed by SSA */
	void processOneCallSite(SgExpression* callSite, SgFunctionDeclaration* callee,
				const boost::unordered_set<SgFunctionDefinition*>& processed, ClassHierarchyWrapper* classHierarchy);

	/** Given a variable that is in a callee's scope, returns true if the caller can access the same variable, false otherwise.
	 * @param callSite either a SgFunctionCallExp or SgConstructorInitializer. */
	static bool isVarAccessibleFromCaller(const VarName& var, SgExpression* callSite, SgFunctionDeclaration* callee);

	/** Returns true if the variable is a nonstatic class variable, so it hass to be accessed by the
	 * "this" pointer. */
	static bool varRequiresThisPointer(const VarName& var);

	/** Returns true if the callee is acting on the same object instance as the caller. */
	static bool isThisPointerSameInCallee(SgFunctionCallExp* callSite, SgMemberFunctionDeclaration* callee);

	/** Returns true of the given expression evaluates to the 'this' pointer. False otherwise.
	 * This function is conservative; it will return false if it cannot statically determine that the
	 * expression is equivalent to the 'This' pointe. */
	static bool isThisPointer(SgExpression* expression);

	//! True if the type is a const pointer pointing to a const object.
	//! Expanded recursively
	static bool isDeepConstPointer(SgType* type);

	//! True if the type is a pointer pointing to a const object.
	//! Expanded recursively.
	static bool isPointerToDeepConst(SgType* type);

	/** Returns true if the given formal parameter is a reference or a nonconst pointer, so that it
	 * its value is aliased between function calls. */
	static bool isArgumentNonConstReferenceOrPointer(SgInitializedName* formalArgument);

	//------------ GRAPH OUTPUT FUNCTIONS ------------ //

	void printToDOT(SgNode* root, std::ostream &outFile);
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


	//------------ DEF/USE TABLE ACCESS FUNCTIONS ------------ //



	//------------ STATIC UTILITY FUNCTIONS FUNCTIONS ------------ //


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
	static const VarName& getVarName(SgNode* node);

	/** If an expression evaluates to a reference of a variable, returns that variable.
	 * Handles casts, comma ops, address of ops, etc. For example,
	 * Given the expression (...., &a), this method would return the VarName for a. */
	static const VarName& getVarForExpression(SgNode* node);

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
	
	static void printNodeDefTable(const NodeReachingDefTable& table);
	static void printFullDefTable(const CFGNodeToDefTableMap& defTable);
};


