#pragma once

#include <boost/shared_ptr.hpp>
#include <map>
#include <set>
#include <rose.h>


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
		EXPANDED_DEF,
		
		/** This is not a real definition; this variable is external to the scope being analyzed and
		 * this def represents the existing value of the variable at the beginning of the scope. */
		EXTERNAL_DEF
	};
	
	typedef boost::shared_ptr<ReachingDef> ReachingDefPtr;

private:
	/** The type of this definition. */
	Type defType;

	/** If this is a phi node, here we store all the joined definitions and all the edges
	 * associated with each one. */
	std::map<ReachingDefPtr, std::set<CFGEdge> > parentDefs;

	/** The node at which this definition is attached. If it's not a phi a function, then
	 * this is the defining node of the variable. */
	CFGNode thisNode;

	/** The values are renamed so that they increase monotonically with program flow. 0 is the first value. */
	int renamingNumer;

public:

	//---------CONSTRUCTORS---------

	/** Creates a new reaching def. */
	ReachingDef(const CFGNode& defNode, Type type);

	//---------ACCESSORS---------

	/** Returns true if this is a phi function. */
	bool isPhiFunction() const;

	/** If this is a join node (phi function), get the definitions merged. 
	 * Each definition is paired with the CFG node along which it flows. */
	const std::map<ReachingDefPtr, std::set<CFGEdge> >& getJoinedDefs() const;

	/** If this is not a phi function, returns the actual reaching definition. 
	  * If this is a phi function, returns the node where the phi function appears. */
	const CFGNode& getDefinitionNode() const;

	/** Returns the actual reaching definitions at the current node, expanding all phi functions. */
	std::set<CFGNode> getActualDefinitions() const;

	/** Get the renaming (SSA index) associated with this definition. 0 is the first value,
	 * and numbers increase monotonically with program flow. */
	int getRenamingNumber() const;

	/** Returns true if this is an original definition (i.e. if p.x is defined, p.x is an original defintion
	 * and p is an expanded definition) */
	bool isOriginalDef() const
	{
		return defType == ORIGINAL_DEF;
	}

	bool operator==(const ReachingDef& other) const;
	
	Type getType() const
	{
		return defType;
	}

	//---------MODIFIERS---------

	/** Set the definition node in the AST (only valid if this is not a phi function) */
	void setDefinitionNode(CFGNode defNode);

	/** Add a new join definition (only valid for phi functions). */
	void addJoinedDef(ReachingDefPtr newDef, CFGEdge edge);

	/** Set the renaming number (SSA index) of this def. */
	void setRenamingNumber(int n);
};
