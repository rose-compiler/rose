#ifndef PLACE_UIDS_H
#define PLACE_UIDS_H

#include "common.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "printAnalysisStates.h"

extern int sgnAnalysisDebugLevel;

/* Assigns a unique numeric ID to each DataflowNode. IDs are unique only within each function and 
   may be repeated across functions. */

class NodeID: public NodeFact
{
	int id;
	public:
	
	NodeID(int id): id(id) {}
	
	int getID();
	
	// The string that represents this object.
	// Every line of this string must be prefixed by indent.
	// The last character of the returned string must not be '\n', even if it is a multi-line string.
	string str(string indent="");
	
	// returns a copy of this node fact
	NodeFact* copy() const;
};

class placeUniqueIDs : public UnstructuredPassIntraAnalysis
{
	int curID;
	public:
	placeUniqueIDs()
	{
		curID = 0;
	}
	
	void visit(const Function& func, const DataflowNode& n, NodeState& state);
};

// Runs the placeUniqueIDs analysis to associate a unique ID with each DataflowNode within each function
void runPlaceUniqueIDs();

// Returns the Unique ID recorded in the given NodeState
int getNodeID(const NodeState& state);

#endif
