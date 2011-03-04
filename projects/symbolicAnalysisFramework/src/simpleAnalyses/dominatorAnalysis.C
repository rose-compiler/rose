#include "dominatorAnalysis.h"
#include <algorithm>

// ############################
// ##### DominatorLattice #####
// ############################

DominatorLattice::DominatorLattice()
{ level = uninitialized; }
DominatorLattice::DominatorLattice(const DataflowNode& n)
{ 
	domNodes.insert(n);
	level = initialized;
}
DominatorLattice::DominatorLattice(const set<DataflowNode>& nodes);
{
	domNodes = nodes;
	level = initialized;
}
DominatorLattice::DominatorLattice(const DominatorLattice& that);
{
	level = that->level;
	if(level == initialized)
		domNodes = nodes;
}

// Initializes this Lattice to its default state, if it is not already initialized
void DominatorLattice::initialize()
{
	level = initialized;
}

// Returns a copy of this lattice
Lattice* DominatorLattice::copy() const
{
	return new DominatorLattice(*this);
}

// Overwrites the state of this Lattice with that of that Lattice
void DominatorLattice::copy(Lattice* that)
{
	level = that->level;
	if(level == initialized)
		domNodes = nodes;
}

// Called by analyses to create a copy of this lattice. However, if this lattice maintains any 
//    information on a per-variable basis, these per-variable mappings must be converted from 
//    the current set of variables to another set. This may be needed during function calls, 
//    when dataflow information from the caller/callee needs to be transferred to the callee/calleer.
// We do not force child classes to define their own versions of this function since not all
//    Lattices have per-variable information.
// varNameMap - maps all variable names that have changed, in each mapping pair, pair->first is the 
//              old variable and pair->second is the new variable
// func - the function that the copy Lattice will now be associated with
void DominatorLattice::remapVars(const map<varID, varID>& varNameMap, const Function& newFunc)
{
	// Nothing to do. This analysis doesn't maintain per-variable information
}

// Called by analyses to copy over from the that Lattice dataflow information into this Lattice.
// that contains data for a set of variables and incorporateVars must overwrite the state of just
// those variables, while leaving its state for other variables alone.
// We do not force child classes to define their own versions of this function since not all
//    Lattices have per-variable information.
void DominatorLattice::incorporateVars(Lattice* that_arg)
{
	// Nothing to do. This analysis doesn't maintain per-variable information
}

// Returns a Lattice that describes the information known within this lattice
// about the given expression. By default this could be the entire lattice or any portion of it.
// For example, a lattice that maintains lattices for different known variables and expression will 
// return a lattice for the given expression. Similarly, a lattice that keeps track of constraints
// on values of variables and expressions will return the portion of the lattice that relates to
// the given expression. 
// It it legal for this function to return NULL if no information is available.
// The function's caller is responsible for deallocating the returned object
Lattice* DominatorLattice::project(SgExpression* expr)
{
	// Nothing to do. This analysis doesn't maintain per-variable information
}

// The inverse of project(). The call is provided with an expression and a Lattice that describes
// the dataflow state that relates to expression. This Lattice must be of the same type as the lattice
// returned by project(). unProject() must incorporate this dataflow state into the overall state it holds.
// Call must make an internal copy of the passed-in lattice and the caller is responsible for deallocating it.
// Returns true if this causes this to change and false otherwise.
bool DominatorLattice::unProject(SgExpression* expr, Lattice* exprState)
{
	// Nothing to do. This analysis doesn't maintain per-variable information
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool DominatorLattice::meetUpdate(Lattice* that_arg)
{
	DominatorLattice* that = dynamic_cast<DominatorLattice*>(that_arg);
	// The meet is an intersection
	set<DataflowNode> intersection;
	set<DataflowNode>::iterator it=set_intersection (domNodes.begin(), domNodes.end(), this->domNodes.begin(), this->domNodes.end(), intersection.begin());
	bool modified = (domNodes == intersection);
	domNodes = intersection;
}

bool DominatorLattice::operator==(Lattice* that)
{
	DominatorLattice* that = dynamic_cast<DominatorLattice*>(that_arg);
	return (this->level == that->level) &&
	       (this->domNodes == that->domNodes);
}

// Functions used to inform this lattice that a given variable is now in use (e.g. a variable has entered 
//    scope or an expression is being analyzed) or is no longer in use (e.g. a variable has exited scope or
//    an expression or variable is dead).
// It is assumed that a newly-added variable has not been added before and that a variable that is being
//    removed was previously added
// Returns true if this causes the lattice to change and false otherwise.
bool DominatorLattice::addNode(const DataflowNode& n, string indent)
{
	bool modified = (domNodes.find(n) == domNodes.end());
	domNodes.insert(n);
	return modified;
}
bool DominatorLattice::remNode(const DataflowNode& n, string indent)
{
	bool modified = (domNodes.find(n) != domNodes.end());
	domNodes.erase(n);
	return modified;
}

// Returns true if the given node dominates / post-dominates the node associated with this lattice
bool DominatorLattice::isDominator(const DataflowNode& n, string indent)
{
	return (domNodes.find(n) != domNodes.end());
}
		
// The string that represents this object
// If indent!="", every line of this string must be prefixed by indent
// The last character of the returned string should not be '\n', even if it is a multi-line string.
string DominatorLattice::str(string indent)
{
	ostringstream outs;
	
	outs << "<DominatorLattice: domNodes=";
	for(set<DataflowNode>::iterator it=domNodes.begin(); it!=domNodes.end(); ) {
		outs << *it;
		it++;
		if(it!=domNodes.end())
			outs << ", ";
	}
	outs << ">";
	return outs.str();
}

// #############################
// ##### DominatorAnalysis #####
// #############################

DominatorAnalysis::DominatorAnalysis(const DataflowNode& tgtNode, const set<DataflowNode>& allNodes, string indent): 
			tgtNode(tgtNode), allNodes(allNodes)
{
	
}
	
// Generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
void DominatorAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                  vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts)
{
	DominatorLattice* dLat;
	if(tgtNode == n)
		dLat = new DominatorLattice(tgtNode);
	else
		dLat = new DominatorLattice(allNodes);
	initLattices.push_back(dLat);	
}

bool DominatorAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo)
{
	DominatorLattice* dLat = dynamic_cast<DominatorLattice*>(*(dfInfo.begin()));
	bool modified = false;
	
	modified = dLat->addNode(tgtNode) || modified;
	
	return modified;
}


// ################################
// ##### FindAllNodesAnalysis #####
// ################################

FindAllNodesAnalysis(const Function& func, string indent) : func(func)
{}

void FindAllNodesAnalysis::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
	allNodes.insert(n);
}

// #####################################
// ##### Module Interface Routines #####
// #####################################

// Map each DataflowNode to its set of dominators and post-dominators, respectively
map <DataflowNode, set<DataflowNode> > dominators;
map <DataflowNode, set<DataflowNode> > postDominators;
// Set of all DataflowNodes within each function
map <Function, set<DataflowNode> > allFuncNodes;

// Returns the set of DataflowNodes that dominate the given node
const set<DataflowNode>& getDominators(const Function& func, const DataflowNode& n, string indent)
{
	// If we have not yet computed the dominators of the given node
	if(dominators.find(n) == dominators.end())
	{
		// If we have not yet computed the set of all DataflowNodes within this function
		if(allFuncNodes.find(func) == allFuncNodes.end())
		{
			// Run the FindAllNodesAnalysis to compute this set and save it in allFuncNodes[func]
			FindAllNodesAnalysis fana(func);
			UnstructuredPassInterAnalysis upia_fana(fana);
			upia_fana.runAnalysis();
			allFuncNodes[func] = fana.allNodes;
		}
		
		// NOTE: THIS CODE ONLY WORKS FOR SINGLE-FUNCTION APPLICATIONS
		DominatorAnalysis da(n, allfuncNodes[func], indent+"    ");
		UnstructuredPassInterAnalysis upia_da(fana);
		upia_da.runAnalysis();
		
		
		const vector<NodeState*> nodeStates = NodeState::getNodeStates(n);
		dominators[n].
	}
	return dominators[n];
}

// prints the Lattices set by the given LiveDeadVarsAnalysis 
void printDominatorAnalysisStates(DominatorAnalysis* da, string indent)
{
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	printAnalysisStates pas(da, factNames, latticeNames, printAnalysisStates::above, indent);
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();
}
