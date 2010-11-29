#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>
#include <sage3basic.h>



class ReachingDef
{
public:
	enum Type
	{
		/** 'Fake' definition that joins two different reaching definitions. */
		PHI_FUNCTION,

		/** Definition for the actual variable in question, not a parent or child. */
		ORIGINAL_DEF,

		/** Definition for the parent or child of this variable. (E.g. if x.b is the variable
		 * in question, this def could be for x or x.b.a). */
		EXPANDED_DEF
	};

private:
	/** The type of this definition. */
	Type defType;

	/** If this is a phi node. */
	std::vector< boost::shared_ptr<ReachingDef> > parentDefs;

	/** The node at which this definition is attached. If it's not a phi a function, then
	 * this is the defining node of the variable. */
	SgNode* thisNode;

	/** The values are renamed so that they increase monotonically with program flow. 0 is the first value. */
	int renamingNumer;

public:

	//---------CONSTRUCTORS---------

	/** Creates a new reaching def. */
	ReachingDef(SgNode* defNode, Type type);

	//---------ACCESSORS---------

	/** Returns true if this is a phi function. */
	bool isPhiFunction() const;

	/** If this is a join node (phi function), get the definitions merged. */
	const std::vector < boost::shared_ptr<ReachingDef> >& getJoinedDefs() const;

	/** If this is not a phi function, returns the actual reaching definition. */
	SgNode* getDefinitionNode() const;

	/** Returns the actual reaching definitions at the current node, expanding all phi functions. */
	std::set<SgNode*> getActualDefinitions() const;

	/** Get the renaming (SSA index) associated with this definition. 0 is the first value,
	 * and numbers increase monotonically with program flow. */
	int getRenamingNumber() const;

	bool operator==(const ReachingDef& other) const;

	//---------MODIFIERS---------

	/** Set the definition node in the AST (only valid if this is not a phi function) */
	void setDefinitionNode(SgNode* defNode);

	/** Add a new join definition (only valid for phi functions).
	  * Returns true if the function was added, false if it was already present. */
	bool addJoinedDef(boost::shared_ptr<ReachingDef> newDef);

	/** Set the renaming number (SSA index) of this def. */
	void setRenamingNumber(int n);
};
