#ifndef RANK_NP_DEP_IF_MEET_H
#define RANK_NP_DEP_IF_MEET_H

#include <sstream>
#include <iostream>
#include <string>
#include <functional>
#include <queue>

#include "common.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "rankDepAnalysis.h"


#include "ConstrGraph.h"

/*********************
 ***   IfMeetLat   ***
 *********************/
class IfMeetLat : public virtual FiniteLattice
{
	friend class IfMeetDetector;
	bool initialized;
	// Binary information about the given CFG node's location inside its surrounding
	// if statements, with with 0 corresponding to the left branch and 1 corresponding 
	// to the right branch. The first entry in ifHist corresponds to the outer-most if 
	// statement and the last corresponds to the inner-most. Thus, 0, 1, 1 corresponds
	// to the * inside the following:
	// if() { if() {} else { if() {} else { * } } }
	vector<bool> ifHist;
	bool isIfMeet;
	// Set by the transfer function of IfMeetDetector to identify each CFG node n that is the
	// first node inside a branch of an if statement, the branch that they're on:
	//    left: ifLeft == n
	//    right: ifRight == n
	DataflowNode ifLeft;
	DataflowNode ifRight;
	
	// The dataflow node of the immediately surrounding if statement
	DataflowNode parentNode;
	
	public:
	IfMeetLat(DataflowNode parentNode);
	
	IfMeetLat(const IfMeetLat &that);
	
	// initializes this Lattice to its default state
	void initialize();

	// returns a copy of this lattice
	Lattice* copy() const;
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool meetUpdate(Lattice* that);
	
	bool operator==(Lattice* that);
	
	// The string that represents this object
	// If indent!="", every line of this string must be prefixed by indent
	// The last character of the returned string should not be '\n', even if it is a multi-line string.
	string str(string indent="");
	
	bool getIsIfMeet();
};

/**********************
 *** IfMeetDetector ***
 **********************/
class IfMeetDetector  : public virtual IntraFWDataflow
{
	protected:
	vector<Lattice*> initState;
	
	public:
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	//vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state);
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                     vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
			
	// the transfer function that is applied to every node
	// n - the dataflow node that is being processed
	// state - the NodeState object that describes the state of the node, as established by earlier 
	//         analysis passes
	// dfInfo - the Lattices that this transfer function operates on. The function takes these lattices
	//          as input and overwrites them with the result of the transfer.
	// Returns true if any of the input lattices changed as a result of the transfer function and
	//    false otherwise.
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
};

// prints the Lattices set by the given DivAnalysis 
void printIfMeetDetectorStates(IfMeetDetector* ifmd, string indent);

// Runs the IfMeetDetector analysis pass
void runIfMeetDetector(bool printStates=false);

// returns true if the given dataflow node is a meet point for an if statement and false otherwise
bool isIfMeetNode(const DataflowNode& n);

/************************
 *** RankDepIfMeetLat ***
 ************************/
class RankDepIfMeetLat : public virtual FiniteLattice
{
	friend class RankDepIfMeetDetector;
	bool initialized;
	// Binary information about the given CFG node's location inside its surrounding
	// if statements, with with 0 corresponding to the left branch and 1 corresponding 
	// to the right branch. The first entry in ifHist corresponds to the outer-most if 
	// statement and the last corresponds to the inner-most. Thus, 0, 1, 1 corresponds
	// to the * inside the following:
	// if() { if() {} else { if() {} else { * } } }
	vector<bool> ifHist;
	bool isIfMeet;
	// Set by the transfer function of IfMeetDetector to identify each CFG node n that is the
	// first node inside a branch of an if statement, the branch that they're on:
	//    left: ifLeft == n
	//    right: ifRight == n
	DataflowNode ifLeft;
	DataflowNode ifRight;
	
	// The dataflow node of the immediately surrounding if statement
	DataflowNode parentNode;
	
	public:
	RankDepIfMeetLat(DataflowNode parentNode);
	
	RankDepIfMeetLat(const RankDepIfMeetLat &that);
	
	// initializes this Lattice to its default state
	void initialize();

	// returns a copy of this lattice
	Lattice* copy() const;
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool meetUpdate(Lattice* that);
	
	bool operator==(Lattice* that);
	
	// The string that represents this object
	// If indent!="", every line of this string must be prefixed by indent
	// The last character of the returned string should not be '\n', even if it is a multi-line string.
	string str(string indent="");
	
	bool getIsIfMeet();
};

/*******************************************************************************************
 *******************************************************************************************
 *******************************************************************************************/

/*****************************
 *** RankDepIfMeetDetector ***
 *****************************/
class RankDepIfMeetDetector  : public virtual IntraFWDataflow
{
	protected:
	vector<Lattice*> initState;
	
	public:
	// generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
	//vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state);
	void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
	                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
			
	// the transfer function that is applied to every node
	// n - the dataflow node that is being processed
	// state - the NodeState object that describes the state of the node, as established by earlier 
	//         analysis passes
	// dfInfo - the Lattices that this transfer function operates on. The function takes these lattices
	//          as input and overwrites them with the result of the transfer.
	// Returns true if any of the input lattices changed as a result of the transfer function and
	//    false otherwise.
	bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
};

// prints the Lattices set by the given DivAnalysis 
void printRankDepIfMeetDetectorStates(RankDepIfMeetDetector* ifmd, string indent);

// Runs the IfMeetDetector analysis pass
void runRankDepIfMeetDetector(bool printStates=false);

// returns true if the given dataflow node is a meet point for an if statement and false otherwise
bool isRankDepIfMeetNode(const DataflowNode& n);

#endif
