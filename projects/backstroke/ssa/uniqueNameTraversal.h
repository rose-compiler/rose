//Author: Justin Frye <jafrye@tamu.edu>
#pragma once
#include "sage3basic.h"

namespace ssa_private
{

/** Attribute that describes the variables modified by a given expression.  */
class VariableReferenceSet
{
private:
	/** Stores all of the varRefs from the subtree.
	 */
	std::vector<SgNode*> refs;

public:
	/** Create the attribute with no refs.
	 */
	VariableReferenceSet():refs(){}

	/** Create the attribute with thisNode.
	 *
	 * @param thisNode The node to add to the list of refs.
	 */
	VariableReferenceSet(SgNode* thisNode)
	{
		refs.push_back(thisNode);
	}

	/** Create the attribute with the subtree and thisNode.
	 *
	 * @param subtree The list of nodes to copy to this attribute.
	 * @param thisNode The node to append to the list of refs.
	 */
	VariableReferenceSet(const std::vector<SgNode*>& subtree, SgNode* thisNode)
	{
		refs.assign(subtree.begin(), subtree.end());
		refs.push_back(thisNode);
	}

	/** Create the attribute with the provided refs.
	 *
	 * @param subtree The refs to use for this attribute.
	 */
	VariableReferenceSet(const std::vector<SgNode*>& subtree)
	{
		refs.assign(subtree.begin(), subtree.end());
	}

	/** Get the references for this node and below.
	 *
	 * @return A constant reference to the ref list.
	 */
	const std::vector<SgNode*>& getRefs() { return refs; }

	/** Set the references for this node and below.
	*
	* @param newRefs A constant reference to the refs to copy to this node.
	*/
	void setRefs(const std::vector<SgNode*>& newRefs) { refs.assign(newRefs.begin(), newRefs.end()); }
};

/** Class to traverse the AST and assign unique names to every varRef. */
class UniqueNameTraversal : public AstBottomUpProcessing<VariableReferenceSet>
{
public:
	/** Called to evaluate the synthesized attribute on every node.
	 *
	 * This function will handle passing all variables that are referenced by a given expression.
	 *
	 * @param node The node being evaluated.
	 * @param attrs The attributes from the child nodes.
	 * @return The attribute at this node.
	 */
	virtual VariableReferenceSet evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList attrs);
};

} //namespace ssa_private