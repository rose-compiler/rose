//Author: Justin Frye <jafrye@tamu.edu>
#pragma once
#include "sage3basic.h"
#include "staticSingleAssignment.h"

namespace ssa_private
{

/** Attribute that describes the variables modified by a given expression. */
class ChildDefsAndUses
{
private:
	/** Stores all of the varRefs that are defined in the current subtree.  */
	std::vector<SgNode*> defs;

	/** Stores all the varRefs that are used in the current subTree. */
	std::vector<SgNode*> uses;

public:

	/** Create the attribute with no refs. 	 */
	ChildDefsAndUses() : defs(), uses() { }

	/** Create the attribute with specified def/use.
	 *
	 * @param defNode The node to add to the list of defs, or NULL
	 * @param useNode The node to add to the list of uses, or NULL
	 */
	ChildDefsAndUses(SgNode* defNode, SgNode* useNode)
	{
		if (defNode)
			defs.push_back(defNode);

		if (useNode)
			uses.push_back(useNode);
	}

	/** Create the attribute with the list of defs and the use.
	 *
	 * @param defTree The vector of defs to add, or an empty vector.
	 * @param useNode The node to add to the list of uses, or NULL.
	 */
	ChildDefsAndUses(const std::vector<SgNode*>& defTree, SgNode* useNode)
	{
		if (defTree.size() > 0)
			defs.assign(defTree.begin(), defTree.end());

		if (useNode)
			uses.push_back(useNode);
	}

	/** Create the attribute with the def and list of uses.
	 *
	 * @param defNode The node to add to the list of defs, or NULL.
	 * @param useTree The vector of uses to add, or an empty vector.
	 */
	ChildDefsAndUses(SgNode* defNode, const std::vector<SgNode*>& useTree)
	{
		if (useTree.size() > 0)
			uses.assign(useTree.begin(), useTree.end());

		if (defNode)
			defs.push_back(defNode);
	}

	/** Create the attribute with the provided uses and defs.
	 *
	 * @param defTree The defs to use in this node, or empty vector.
	 * @param useTree The uses to use in this node, or empty vector.
	 */
	ChildDefsAndUses(const std::vector<SgNode*>& defTree, const std::vector<SgNode*>& useTree)
	{

		if (defTree.size() > 0)
			defs.assign(defTree.begin(), defTree.end());

		if (useTree.size() > 0)
			uses.assign(useTree.begin(), useTree.end());
	}

	/** Get the references for this node and below.
	 *
	 * @return A constant reference to the ref list.
	 */
	std::vector<SgNode*>& getDefs()
	{
		return defs;
	}

	/** Set the defs for this node and below.
	 *
	 * @param newDefs A constant reference to the defs to copy to this node.
	 */
	void setDefs(const std::vector<SgNode*>& newDefs)
	{
		defs.assign(newDefs.begin(), newDefs.end());
	}

	/** Get the uses for this node and below.
	 *
	 * @return A constant reference to the use list.
	 */
	std::vector<SgNode*>& getUses()
	{
		return uses;
	}

	/** Set the uses for this node and below.
	 *
	 * @param newUses A constant reference to the uses to copy to this node.
	 */
	void setUses(const std::vector<SgNode*>& newUses)
	{
		uses.assign(newUses.begin(), newUses.end());
	}
};

/** This class collects all the defs and uses associated with each node in the traversed CFG.
 * Note that this does not compute reachability information; it just records each instance of
 * a variable used or defined. */
class DefsAndUsesTraversal : public AstBottomUpProcessing<ChildDefsAndUses>
{
	StaticSingleAssignment* ssa;

public:

	DefsAndUsesTraversal(StaticSingleAssignment* ssa) : ssa(ssa) { }

	/** Called to evaluate the synthesized attribute on every node.
	 *
	 * This function will handle passing all variables that are defined and used by a given operation.
	 *
	 * @param node The node being evaluated.
	 * @param attr The attributes from the child nodes.
	 * @return The attribute at this node.
	 */
	virtual ChildDefsAndUses evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList attrs);

private:

	/** Mark all the uses as occurring at the specified node. */
	void addUsesToNode(SgNode* node, std::vector<SgNode*> uses);
};

} //namespace ssa_private