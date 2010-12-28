//Author: Justin Frye <jafrye@tamu.edu>
#pragma once
#include "rose.h"
#include <vector>

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


/** Attribute that describes the variables modified by a given expression.  */
class VariableReferenceSet
{
private:
	/** Stores all of the varRefs from the subtree. */
	std::vector<SgNode*> refs;

public:

	/** Create the attribute with no refs. */
	VariableReferenceSet() : refs()
	{
	}

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
	const std::vector<SgNode*>& getRefs()
	{
		return refs;
	}

	/** Set the references for this node and below.
	 *
	 * @param newRefs A constant reference to the refs to copy to this node.
	 */
	void setRefs(const std::vector<SgNode*>& newRefs)
	{
		refs.assign(newRefs.begin(), newRefs.end());
	}
};


/** Class to traverse the AST and assign unique names to every varRef. */
class UniqueNameTraversal : public AstBottomUpProcessing<VariableReferenceSet>
{
	/** All the initialized names in the project. */
	std::vector<SgInitializedName*> allInitNames;

	/** Finds initialized names that are "fake" (refer to p_prev_decl_item in the SgInitializedName docs)
	 * and replaces them with the true declaration. */
	SgInitializedName* resolveTemporaryInitNames(SgInitializedName* name);

public:

	UniqueNameTraversal(const std::vector<SgInitializedName*>& allNames) : allInitNames(allNames)
	{
	}

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