#include "placeUIDs.h"

/* Assigns a unique numeric ID to each DataflowNode. IDs are unique only within each function and 
   may be repeated across functions. */

/**********
 * NodeID *
 **********/
int NodeID::getID()
{ return id; }

// The string that represents this object.
// Every line of this string must be prefixed by indent.
// The last character of the returned string must not be '\n', even if it is a multi-line string.
string NodeID::str(string indent)
{
	cout << indent << "[NodeID: id="<<id<<"]";
}

// returns a copy of this node fact
NodeFact* NodeID::copy() const
{
	return new NodeID(id);
}

/******************
 * placeUniqueIDs *
 ******************/

void placeUniqueIDs::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
	//printf("placeUniqueIDs: node=<%s | %s>\n", n.getNode()->class_name().c_str(), n.getNode()->unparseToString().c_str());
	NodeID* newID = new NodeID(curID);
	state.addFact(this, 0, newID);
	curID++;
}


/*********************
 * Utility Functions *
 *********************/
 
static placeUniqueIDs* puids = NULL;

// Runs the placeUniqueIDs analysis to associate a unique ID with each DataflowNode within each function
void runPlaceUniqueIDs()
{
	if(puids==NULL)
	{
		puids = new placeUniqueIDs();
		UnstructuredPassInterAnalysis puids_saau(*puids);
		puids_saau.runAnalysis();
	}
}

// Returns the Unique ID recorded in the given NodeState
int getNodeID(const NodeState& state)
{
	ROSE_ASSERT(puids);
	state.getFact(puids, 0);
}

