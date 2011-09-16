#ifndef DOMINATOR_ANALYSIS_H
#define DOMINATOR_ANALYSIS_H

#include "genericDataflowCommon.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "printAnalysisStates.h"

extern int dominatorAnalysisDebugLevel;

// Lattice that stores the DataflowNodes that are dominated by or post-dominated by a given DataflowNode
class DominatorLattice : public FiniteLattice
{
        public:
        typedef enum { uninitialized=0, initialized } domLevel;
        domLevel level;
        set<DataflowNode> domNodes;
        DataflowNode n;
        
        public:
        //DominatorLattice();
        DominatorLattice(const DataflowNode& n);
        DominatorLattice(const DataflowNode& n, const DataflowNode& nodes);
        DominatorLattice(const DataflowNode& n, const set<DataflowNode>& nodes);
        DominatorLattice(const DominatorLattice& that);
                
        // Initializes this Lattice to its default state, if it is not already initialized
        void initialize();
        
        // Returns a copy of this lattice
        Lattice* copy() const;
        
        // Overwrites the state of this Lattice with that of that Lattice
        void copy(Lattice* that);
        
        // Overwrites the state of this Lattice with that of that Lattice.
        // Returns true if this causes this Lattice to chance and false otherwise.
        bool copyFrom(DominatorLattice* domLat, string indent="");
        
        // Called by analyses to create a copy of this lattice. However, if this lattice maintains any 
        //    information on a per-variable basis, these per-variable mappings must be converted from 
        //    the current set of variables to another set. This may be needed during function calls, 
        //    when dataflow information from the caller/callee needs to be transferred to the callee/calleer.
        // We do not force child classes to define their own versions of this function since not all
        //    Lattices have per-variable information.
        // varNameMap - maps all variable names that have changed, in each mapping pair, pair->first is the 
        //              old variable and pair->second is the new variable
        // func - the function that the copy Lattice will now be associated with
        void remapVars(const map<varID, varID>& varNameMap, const Function& newFunc);
        
        // Called by analyses to copy over from the that Lattice dataflow information into this Lattice.
        // that contains data for a set of variables and incorporateVars must overwrite the state of just
        // those variables, while leaving its state for other variables alone.
        // We do not force child classes to define their own versions of this function since not all
        //    Lattices have per-variable information.
        void incorporateVars(Lattice* that_arg);
        
        // Returns a Lattice that describes the information known within this lattice
        // about the given expression. By default this could be the entire lattice or any portion of it.
        // For example, a lattice that maintains lattices for different known variables and expression will 
        // return a lattice for the given expression. Similarly, a lattice that keeps track of constraints
        // on values of variables and expressions will return the portion of the lattice that relates to
        // the given expression. 
        // It it legal for this function to return NULL if no information is available.
        // The function's caller is responsible for deallocating the returned object
        Lattice* project(SgExpression* expr);
        
        // The inverse of project(). The call is provided with an expression and a Lattice that describes
        // the dataflow state that relates to expression. This Lattice must be of the same type as the lattice
        // returned by project(). unProject() must incorporate this dataflow state into the overall state it holds.
        // Call must make an internal copy of the passed-in lattice and the caller is responsible for deallocating it.
        // Returns true if this causes this to change and false otherwise.
        bool unProject(SgExpression* expr, Lattice* exprState);
        
        // computes the meet of this and that and saves the result in this
        // returns true if this causes this to change and false otherwise
        bool meetUpdate(Lattice* that_arg);
        
        bool operator==(Lattice* that);
        
        // Functions used to inform this lattice that a given variable is now in use (e.g. a variable has entered 
        //    scope or an expression is being analyzed) or is no longer in use (e.g. a variable has exited scope or
        //    an expression or variable is dead).
        // It is assumed that a newly-added variable has not been added before and that a variable that is being
        //    removed was previously added
        // Returns true if this causes the lattice to change and false otherwise.
        bool addNode(const DataflowNode& n, string indent="");
        bool remNode(const DataflowNode& n, string indent="");
        
        // Returns true if the given node dominates / post-dominates the node associated with this lattice
        bool isDominator(const DataflowNode& n, string indent="");
                        
        // The string that represents this object
        // If indent!="", every line of this string must be prefixed by indent
        // The last character of the returned string should not be '\n', even if it is a multi-line string.
        string str(string indent="");
};

/* Computes the set of DataflowNodes that dominate a given DataflowNode. */
class DominatorAnalysis : public IntraFWDataflow
{
        protected:
        const set<DataflowNode>& allNodes;
        
        public:
        DominatorAnalysis(const set<DataflowNode>& allNodes, string indent="");
        
        // Generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
        void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                          vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
        
        bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
};

/* Computes the set of all DataflowNodes within a given function */
class FindAllNodesAnalysis: public UnstructuredPassIntraAnalysis
{
        public:
        const Function& func;
        set<DataflowNode> allNodes;
        
        FindAllNodesAnalysis(const Function& func, string indent="");
        void visit(const Function& func, const DataflowNode& n, NodeState& state);
};

// Returns the set of DataflowNodes that dominate the given node
const set<DataflowNode>& getDominators(SgProject* project, const Function& func, const DataflowNode& n, string indent="");

// Returns true if node a dominates node b and false otherwise
bool dominates(const DataflowNode& a, const DataflowNode& b, string indent="");

// prints the Lattices set by the given LiveDeadVarsAnalysis 
void printDominatorAnalysisStates(DominatorAnalysis* da, string indent="");

#endif
