#ifndef SEQUENCE_STRUCT_ANALYSIS_H
#define SEQUENCE_STRUCT_ANALYSIS_H

#include "common.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "divAnalysis.h"
#include "ConstrGraph.h"
#include "ConstrGraphAnalysis.h"
#include "liveDeadVarAnalysis.h"
#include "dominatorAnalysis.h"
#include "printAnalysisStates.h"

extern int sequenceStructAnalysisDebugLevel;

// Lattice that stores the structure of the sequence iterated by a variable
// or an array access
class SeqStructLattice : public FiniteLattice
{
        public:
        // The sequence's starting and ending points
        varID vInit;
        varID vFin;
        // The sequence's stride
        int s;
        
        public:
        // The different levels of this lattice
        typedef enum {
                // This object is uninitialized
                uninitialized=0,
                // No information is known about the sequence
                bottom=1,
                // The starting point of the sequence is known (vInit==vFin and s==0)
                startKnown=2,
                // The structure of the sequence is known (s!=0)
                seqKnown=3,
                // The sequence of values that this variable passes through is more complex than what 
                // can be represented using a lower/upper bound and a constant stride
                top=4} seqLevel;
        
        private:
        // This object's current level in the lattice: (uninitialized, bottom, startKnown, seqKnown, top)
        seqLevel level;
        
        // The constraint graph associated with the same DataflowNode as this lattice and that holds
        // the relationships between vInit, vFin and other variables.
        ConstrGraph* cg;
        
        // The analysis that identifies the dominator relationships between DataflowNodes
        DominatorAnalysis* dom;
        
        // The DataflowNode that this lattice corresponds to
        const DataflowNode& n;
        
        public:
        // Initializes the lattice to level=uninitialized
        SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, string indent="");
        
        // Initializes the lattice to level=startKnown and this->vInit=this->vFin=vInit
        SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, const varID& vInit, int startOffset, string indent="");
        // Initializes the lattice to level=startKnown and this->vInit=this->vFin=init
        SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, int initVal, string indent="");
        
        // Initializes the lattice to level=seqKnown and this->vInit=initV this->vFin=finV, this->s=s
        SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, const varID& initV, const varID& finV, int s, string indent="");
        // Initializes the lattice to level=seqKnown and this->vInit=initV this->vFin=finV, this->s=s
        SeqStructLattice(ConstrGraph* cg, const DataflowNode& n, int initV, int finV, int s, string indent="");
                
        protected:      
        // Common initialization code for SeqStructLattice
        void init(string indent="");
                
        public:
        // Copy constructor
        SeqStructLattice(const SeqStructLattice& that, string indent="");
        
        protected:      
        // Copies the state from That sequence to This. Returns true if this causes This to change and false otherwise.
        bool copyFrom(const SeqStructLattice& that, string indent="");
        
        public:
        // Initializes this Lattice to its default state, if it is not already initialized
        void initialize();
        
        // Returns a copy of this lattice
        Lattice* copy() const;
        
        // Overwrites the state of this Lattice with that of that Lattice
        void copy(Lattice* that);
        
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
        
        // Computes the meet of this and that and saves the result in this
        // returns true if this causes this to change and false otherwise
        bool meetUpdate(Lattice* that_arg);
        
        // Set the level of this object to Bottom.
        // Return true if this causes ths Lattice to change and false otherwise.
        bool setToBottom();
        
        // Set the level of this object to Top.
        // Return true if this causes ths Lattice to change and false otherwise.
        bool setToTop();
        
        // Set the level of this object to startKnown, with the given variable starting point (vInit + startOffset).
        // Return true if this causes ths Lattice to change and false otherwise.
        bool setToStartKnown(varID vInit, int initOffset);
                
        // Set the level of this object to startKnown, with the given constant starting point.
        // Return true if this causes ths Lattice to change and false otherwise.
        bool setToStartKnown(int initVal);
        
        // Set the level of this object to seqKnown, with the given final point and stride
        // Return true if this causes ths Lattice to change and false otherwise.
        bool setToSeqKnown(varID vFin, int finOffset, int stride);
        
        // Return this lattice's level
        seqLevel getLevel() const;
        
        bool operator==(Lattice* that);
                        
        // The string that represents this object
        // If indent!="", every line of this string must be prefixed by indent
        // The last character of the returned string should not be '\n', even if it is a multi-line string.
        string str(string indent="");
};

class SeqStructAnalysis : public IntraFWDataflow
{
        protected:
        static map<varID, Lattice*> constVars;
        static bool constVars_init;
        
        // The LiveDeadVarsAnalysis that identifies the live/dead state of all application variables.
        // Needed to create a FiniteVarsExprsProductLattice.
        LiveDeadVarsAnalysis* ldva;
        
        // The DivAnalysis that computes information required for the construction of constraint graphs
        DivAnalysis* divAnalysis;
        
        // The analysis that will be executed concurrently with this analysis and will maintain the relationships 
        // between variables as well as SeqStructLattice initial and final values using ConstrGraphs
        ConstrGraphAnalysis* cgAnalysis;
        
        public:
        SeqStructAnalysis(LiveDeadVarsAnalysis* ldva, DivAnalysis* divAnalysis): IntraFWDataflow()
        {       
                this->ldva = ldva;
                this->divAnalysis   = divAnalysis;
                cgAnalysis = new ConstrGraphAnalysis(ldva, divAnalysis);
        }
        
        // generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
        //vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state);
        void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                          vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);
        
        // Returns a map of special constant variables (such as zeroVar) and the lattices that correspond to them
        // These lattices are assumed to be constants: it is assumed that they are never modified and it is legal to 
        //    maintain only one copy of each lattice may for the duration of the analysis.
        //map<varID, Lattice*>& genConstVarLattices() const;
                
        bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);
};

// Prints the Lattices set by the given SeqStructAnalysis  
void printSeqStructAnalysisStates(SeqStructAnalysis* ssa, string indent="");


#endif
