#include "AstUtilInterface.h"
#include "StmtInfoCollect.h"
#include "AstInterface_ROSE.h"


bool AstUtilInterface::ComputeAstSideEffects(SgNode* ast, SgNode* scope,
                   std::function<bool(SgNode*, SgNode*)>* collect_mod_set,
                   std::function<bool(SgNode*, SgNode*)>* collect_read_set,
                   std::function<bool(SgNode*, SgNode*)>* collect_call_set) {
    AstInterfaceImpl astImpl(scope);
    AstInterface fa(&astImpl);
    StmtSideEffectCollect<SgNode*> collect_operator(fa);
    return collect_operator(ast, collect_mod_set, collect_read_set, collect_call_set);
}

