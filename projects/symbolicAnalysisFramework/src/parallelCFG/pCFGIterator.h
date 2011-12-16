#ifndef _PCFGITERATOR_H
#define _PCFGITERATOR_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "pCFG.h"
#include "genericDataflowCommon.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "printAnalysisStates.h"
#include "liveDeadVarAnalysis.h"
#include "MPIDepAnalysis.h"
#include "pCFGAnnotations.h"
#include "pCFGIteratorTransfer.h"

#include "boost/random.hpp"
#include <ctime>

extern int pCFGIteratorDebugLevel;

class pCFGIterator : public pCFG_FWDataflow
{
    protected:
    // required to create ConstrGraph
    // required for FiniteVarsExprsProductLattice
    LiveDeadVarsAnalysis *ldva;

    // mpi rank dep analysis
    MPIDepAnalysis *mda;

    int npcfgnodes;

    ostringstream nodeouts;
    ostringstream edgeouts;

    vector<string> pSetColors;

    //static boost::mt19937 randgen(static_cast<unsigned int> (std::time(0)));

    public:
    // default constructor
    pCFGIterator(LiveDeadVarsAnalysis* ldva_arg, MPIDepAnalysis* mda_arg)
    {
        this->ldva = ldva_arg;
        this->mda = mda_arg;
        this->npcfgnodes = 0;
    }

    // copy constructor
    pCFGIterator(const pCFGIterator& that) : pCFG_FWDataflow((const pCFG_FWDataflow&) that)
    {
        this->ldva = that.ldva;
        this->mda = that.mda;
    }

    ~pCFGIterator() { }

    // functions any analysis should implement

    void genInitState(const Function& func, const pCFGNode& n, const NodeState& state,                      
                      vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);

    void copyPSetState(const Function& func, const pCFGNode& n, 
                       unsigned int srcPSet, unsigned int tgtPSet, NodeState& state,
                       vector<Lattice*>& lattices, vector<NodeFact*>& facts, 
                       ConstrGraph* partitionCond, bool omitRankSet);

    void resetPSet(unsigned int pSet, vector<Lattice*>& dfInfo);

    bool transfer(const pCFGNode& n, unsigned int pSet, const Function& func,
                  NodeState& state, const vector<Lattice*>& dfInfo,
                  bool& deadPSet, bool& splitPSet, vector<DataflowNode>& splitPSetNodes,
                  bool& splitPNode, vector<ConstrGraph*>& splitConditions, bool& blockPSet);

    bool transfer(const pCFGNode& n, unsigned int pSet, const Function& func,
                  NodeState& state, const vector<Lattice*>& dfInfo,
                  bool& deadPSet, bool& splitPSet, vector<DataflowNode>& splitPSetNodes,
                  bool& splitPNode, bool& blockPSet, bool& mergePSet);

    bool initPSetDFfromPartCond(const Function& func, const pCFGNode& n, unsigned int pSet,
                                const vector<Lattice*>& dfInfo, const vector<NodeFact*>& facts,
                                ConstrGraph* partitionCond);

    // NOTE : Not sure if this is used
    void mergePCFGStates(const list<unsigned int>& pSetsToMerge, const pCFGNode& n, const Function& func,
                         NodeState& staet, const vector<Lattice*>& dfInfo, map<unsigned int, unsigned int>& pSetMigrations);

    
    void matchSendsRecvs(const pCFGNode& n, const vector<Lattice*>& dfInfo, NodeState* state, 
                         // Set by analysis to identify the process set that was split
                         unsigned int& splitPSet,
                         vector<ConstrGraph*>& splitConditions, 
                         vector<DataflowNode>& splitPSetNodes,
                         // for each split process set, true if its active and false if it is blocked
                         vector<bool>&         splitPSetActive,
                         // for each process set that was not split, true if becomes active as a result of the match,
                         // false if its status doesn't change
                         vector<bool>&         pSetActive,
                         const Function& func, NodeState* fState);

    bool runAnalysis_pCFG(const Function& func, NodeState* fState, pCFG_Checkpoint* chkpt);
    
    // update descNode
    void performPSetSplit(int curPSet, const pCFGNode& curNode, pCFGNode& descNode, vector<DataflowNode>& splitPSetNodes, set<unsigned int>&);

    void printNode(pCFGNode&);
    void printEdge(pCFGNode&, pCFGNode&, string color="black", string style="solid");
    void printSubGraphInit(unsigned int);
    void printSubGraphEnd();

    void writeToDot(string filename);

    void filterSendRecv(const pCFGNode&, set<unsigned int>&, set<unsigned int>&, set<unsigned int>&);
    bool matchSendRecv(const pCFGNode&, set<unsigned int>&, set<unsigned int>&, set<unsigned int>&, set<unsigned int>&, set<unsigned int>&);

    string genRandColor(unsigned int);
};
   
#endif
