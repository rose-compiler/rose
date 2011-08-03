#pragma once
#include <rose.h>
#include <staticSingleAssignment.h>
#include <uniqueNameTraversal.h>
#include <map>
#include <virtualCFG.h>

namespace ssa_private
{

/** Attribute that describes the variables used by a given expression. */
class ChildUses
{
private:
	/** An assignment to the current expression in the AST would define this variable */
	SgVarRefExp* currentVar;

	/** Stores all the varRefs that are used in the current subTree. */
	std::set<SgVarRefExp*> uses;

public:

	/** Create the attribute with no refs. 	 */
	ChildUses() : currentVar(NULL)
	{
	}

	ChildUses(SgVarRefExp* useNode, SgVarRefExp* var)
	{
		uses.insert(useNode);
		currentVar = var;
	}

	/** Create the attribute with the def and list of uses.
	 *
	 * @param useTree The vector of uses to add, or an empty vector.
	 */
	ChildUses(const std::set<SgVarRefExp*>& useTree, SgVarRefExp* var = NULL)
	{
		uses = useTree;
		currentVar = var;
	}

	/** Get the uses for this node and below.
	 *
	 * @return A constant reference to the use list.
	 */
	std::set<SgVarRefExp*>& getUses()
	{
		return uses;
	}

	/** Set the uses for this node and below.
	 *
	 * @param newUses A constant reference to the uses to copy to this node.
	 */
	void setUses(const std::set<SgVarRefExp*>& newUses)
	{
		uses = newUses;
	}

	SgVarRefExp* getCurrentVar() const
	{
		return currentVar;
	}
};

/** This class collects all the defs and uses associated with each node in the traversed CFG.
 * Note that this does not compute reachability information; it just records each instance of
 * a variable used or defined. */
class DefsAndUsesTraversal : private AstBottomUpProcessing<ChildUses>
{
public:
	
	typedef std::map<CFGNode, std::set<UniqueNameTraversal::VarName> > CFGNodeToVarsMap;

	/** Call this method to collect defs and uses for a subtree. The additional defs and uses discovered
	 * in the tree will be inserted in the passed data structures. */
	static void CollectDefsAndUses(SgNode* traversalRoot, CFGNodeToVarsMap& defs, 
		std::map<SgNode*, std::set<SgVarRefExp*> >& uses);
	
private:
	//! Map from each CFG node that contains a definition to all the variables defined at the node
	CFGNodeToVarsMap cfgNodeToDefinedVars;

	//! Maps from each CFG node to the variables used by that node.
	std::map<SgNode*, std::set<SgVarRefExp*> > astNodeToUsedVars;

public:

	/** Called to evaluate the synthesized attribute on every node.
	 *
	 * This function will handle passing all variables that are defined and used by a given operation.
	 *
	 * @param node The node being evaluated.
	 * @param attr The attributes from the child nodes.
	 * @return The attribute at this node.
	 */
	virtual ChildUses evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList attrs);

private:

	/** Mark all the uses as occurring at the specified node. */
	void addUsesToNode(SgNode* node, std::set<SgVarRefExp*> uses);
	
	//! Indicate a place where a variable is defined
	void addDefForVarAtNode(SgVarRefExp* var, const CFGNode& node);
	
};

} //namespace ssa_private