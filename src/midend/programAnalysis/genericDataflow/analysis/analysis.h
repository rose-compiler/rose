#include <featureTests.h>
#ifdef ROSE_ENABLE_SOURCE_ANALYSIS

#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"

class Analysis;

#include "lattice.h"
#include "nodeState.h"
#include "variables.h"
#include "varSets.h"
#include <vector>
#include <set>
#include <map>

extern int analysisDebugLevel;

class Analysis
{
  public:
    // a filter function to decide which raw CFG node to show (if return true) or hide (otherwise)  
    // This is required to support custom filters of virtual CFG
    // Custom filter is set inside the intra-procedural analysis.
    // Inter-procedural analysis will copy the filter from its intra-procedural analysis during the call to its constructor.
    bool (*filter) (CFGNode cfgn); 
    Analysis(bool (*f)(CFGNode) = defaultFilter):filter(f) {}
};

class InterProceduralAnalysis;

class IntraProceduralAnalysis : virtual public Analysis
{
        protected:
        InterProceduralAnalysis* interAnalysis;
        
        public:
        void setInterAnalysis(InterProceduralAnalysis* interAnalysis)
        { this->interAnalysis = interAnalysis; }
        
        // runs the intra-procedural analysis on the given function, returns true if 
        // the function's NodeState gets modified as a result and false otherwise
        // state - the function's NodeState
        virtual bool runAnalysis(const Function& func, NodeState* state)=0;
        
        virtual ~IntraProceduralAnalysis();
};

class InterProceduralAnalysis : virtual public Analysis
{
        protected:
        IntraProceduralAnalysis* intraAnalysis;
        
        InterProceduralAnalysis(IntraProceduralAnalysis* intraAnalysis)
        {
                this->intraAnalysis = intraAnalysis; 
                // inform the intra-procedural analysis that this inter-procedural analysis will be running it
                intraAnalysis->setInterAnalysis(this);
        }
        
        virtual void runAnalysis()=0;
        
        virtual ~InterProceduralAnalysis();
};

/********************************
 *** UnstructuredPassAnalyses ***
 ********************************/

// A driver class which simply iterates through all CFG nodes of a specified function
class UnstructuredPassIntraAnalysis : virtual public IntraProceduralAnalysis
{
        public:
        // runs the intra-procedural analysis on the given function, returns true if 
        // the function's NodeState gets modified as a result and false otherwise
        // state - the function's NodeState
        bool runAnalysis(const Function& func, NodeState* state);
        
        virtual void visit(const Function& func, const DataflowNode& n, NodeState& state)=0;
};
// A driver class which simply iterates all function definitions one by one and call intra-procedural analysis on each of them.
class UnstructuredPassInterAnalysis : virtual public InterProceduralAnalysis
{
        public:
        UnstructuredPassInterAnalysis(IntraProceduralAnalysis& intraAnalysis) : InterProceduralAnalysis(&intraAnalysis)
        { }
                
        void runAnalysis();
};

#endif
#endif
