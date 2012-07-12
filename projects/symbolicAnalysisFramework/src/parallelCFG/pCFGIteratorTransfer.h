#ifndef _PCFGITERATORTRANSFER_H
#define _PCFGITERATORTRANSFER_H

#include "pCFG.h"
#include "genericDataflowCommon.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "MPIDepAnalysis.h"
#include "pCFGAnnotations.h"

class IntraPCFGTransferVisitor : public ROSE_VisitorPatternDefaultBase
{
    protected:
    const pCFGNode& pcfg_node;
    unsigned int pSet;
    const Function& func;
    NodeState& state;
    const vector<Lattice*>& dfInfo;
    bool& isDeadPSet;
    bool& isSplitPSet;
    vector<DataflowNode>& splitPSetNodes;
    bool& isSplitPNode;
    bool& isBlockPSet;
    bool& isMergePSet;

    public:
    IntraPCFGTransferVisitor(const pCFGNode& pn, 
                             unsigned int ps, 
                             const Function& f,
                             NodeState& s, 
                             const vector<Lattice*>& dfI, 
                             bool& dPS,
                             bool& sPS, 
                             vector<DataflowNode>& sPSN, 
                             bool& sPN,
                             bool& bPS,
                             bool& mPS) 
        : pcfg_node(pn), pSet(ps), func(f), state(s), dfInfo(dfI), isDeadPSet(dPS), 
        isSplitPSet(sPS), splitPSetNodes(sPSN), isSplitPNode(sPN), isBlockPSet(bPS), isMergePSet(mPS)
    { }

    virtual bool finish() = 0;
    virtual ~IntraPCFGTransferVisitor() { };
};

class pCFGIteratorTransfer : public IntraPCFGTransferVisitor
{
    public:
    bool modified;
    MPIDepAnalysis* mda;
    pCFGIteratorTransfer(const pCFGNode& pnode,
                         unsigned int pset,
                         const Function& func,
                         NodeState& state,
                         const vector<Lattice*>& dfI,
                         bool& deadpset,
                         bool& splitpset,
                         vector<DataflowNode>& splitpsetnodes,
                         bool& splitpnode,
                         bool& blockpset,
                         bool& mergepset,
                         MPIDepAnalysis* mda_arg)
        : IntraPCFGTransferVisitor(pnode, pset, func, state, dfI, deadpset, splitpset, splitpsetnodes, splitpnode, blockpset, mergepset)
    {
        modified = false;
        this->mda = mda_arg;
    }

    bool finish ()
    {
        return modified;
    }

    void initMPIDepAnalysis(MPIDepAnalysis* mda_arg)
    {
        this->mda = mda_arg;
    }

    ~pCFGIteratorTransfer() {}

    bool isMpiDepCond(SgIfStmt*);

    // pattern visitor
    void visit(SgIfStmt* sgifstmt);
    void visit(SgPragmaDeclaration* sgpragma);
    void visit(SgFunctionCallExp* sgfcexp);
};
#endif
