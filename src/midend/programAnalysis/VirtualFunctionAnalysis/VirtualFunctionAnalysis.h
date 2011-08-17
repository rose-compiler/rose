#ifndef VIRTUAL_FUNCTION_ANALYSIS_H
#define VIRTUAL_FUNCTION_ANALYSIS_H
#include "PtrAliasAnalysis.h"
#include "CallGraph.h"
#include "sage3basic.h"

class VirtualFunctionAnalysis : public PtrAliasAnalysis {
    
    bool isExecuted;
public:

    VirtualFunctionAnalysis(SgProject *__project) : PtrAliasAnalysis(__project) {
        isExecuted = false;
    };
    std::vector<SgFunctionDeclaration*> resolveFunctionCall(SgFunctionCallExp * exp){
        if(isExecuted == false)
            run();
        return resolver[exp];
    }
    
    void run();
    
    void pruneCallGraph(CallGraphBuilder &);
    
    /* Resolves any Function Calls/ Member function Call/ Constructors */
    void resolveFunctionCall(SgExpression *, std::vector<SgFunctionDeclaration*> &);
    
    ~VirtualFunctionAnalysis (){
    }
};


#endif
