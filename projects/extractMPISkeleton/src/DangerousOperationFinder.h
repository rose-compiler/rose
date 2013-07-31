#ifndef __DangerousOperationFinder_H_LOADED__
#define __DangerousOperationFinder_H_LOADED__

#include <assert.h>
#include <numeric>

#include "Utils.h"

class NodeChecker {
    public:
    virtual bool check(SgNode *n) = 0;
};

class DangerousOperationFinder : public AstBottomUpProcessing<bool> {
  private:

    StaticSingleAssignment *ssa;
    NodeChecker *checker;

    SgNode *dangerousOperand(SgNode *n) {
        if(isSgDivideOp(n) || isSgDivAssignOp(n) || isSgPntrArrRefExp(n)) {
            SgBinaryOp *op = isSgBinaryOp(n);
            assert(op);
            return op->get_rhs_operand();
        }
        return NULL;
    }

  public:

    DangerousOperationFinder(StaticSingleAssignment *ssaParam,
                             NodeChecker *chk) {
        ssa = ssaParam;
        checker = chk;
    }

    bool evaluateSynthesizedAttribute(SgNode *n,
      SynthesizedAttributesList childAttrs)
    {
        bool localResult =
            std::accumulate(childAttrs.begin(), childAttrs.end(),
                            false, std::logical_or<bool>());
        SgNode *dangerous = dangerousOperand(n);
        if(dangerous) {
            std::set<SgNode*> defs;
            getNodeVarDefsTransSSA(ssa, dangerous, &defs);
            std::set<SgNode*>::iterator di = defs.begin();
            for(; di != defs.end(); ++di) {
                if(checker->check(*di)) {
                    std::cout << "Dangerous operation" << std::endl
                              << "  " << n->unparseToString()
                              << std::endl
                              << "depends on omitted data" << std::endl
                              << "  " << (*di)->unparseToString()
                              << std::endl;
                    localResult = true;
                }
            }
        }
        return localResult;
    }
};

#endif
