#ifndef VIRTUAL_FUNCTION_ANALYSIS_H
#define VIRTUAL_FUNCTION_ANALYSIS_H
#include "PtrAliasAnalysis.h"
#include "CallGraph.h"

class VirtualFunctionAnalysis : public PtrAliasAnalysis {
    
    bool isExecuted;
public:

    VirtualFunctionAnalysis(SgProject *__project) : PtrAliasAnalysis(__project) {
        std::cout << "Inside Virtual Funciton Resolver :->" << std::endl;
        isExecuted = false;
    };
    std::vector<SgFunctionDeclaration*> resolveFunctionCall(SgFunctionCallExp * exp){
        if(isExecuted == false)
            run();
        return resolver[exp];
    }
    
    void run() {
        vector<SgExpression*> callSites = SageInterface::querySubTree<SgExpression> (project, V_SgFunctionCallExp);
        vector<SgExpression*> constrs = SageInterface::querySubTree<SgExpression> (project, V_SgConstructorInitializer);
        std::cout << "Inside VFA RUn ~:>" << callSites.size() << " " << constrs.size()<< " ";
        
        callSites.insert(callSites.end(), constrs.begin(), constrs.end());
        
        std::cout<< callSites.size() << std::endl;
        
        unsigned int index;


        resolver.clear();
        for(index = 0; index < callSites.size(); index++) {
            std::vector<SgFunctionDeclaration *> funcs;
            
            CallTargetSet::getPropertiesForExpression(callSites[index], classHierarchy, funcs);
            if(isSgFunctionCallExp(callSites[index]) && funcs.size() > 1 )
                funcs.clear();
            resolver[callSites[index]] = funcs;
        }
        
        PtrAliasAnalysis::run();
        for(index = 0; index < callSites.size(); index++) {
            if(resolver.at(callSites[index]).size() == 0  ) {
                std::vector<SgFunctionDeclaration *> funcs;
                CallTargetSet::getPropertiesForExpression(callSites[index], classHierarchy, funcs);
                resolver[callSites[index]] = funcs;
            }
        }
        isExecuted = true;
        
        rebuildCallGraph();
        // Generate call graph in dot format
        AstDOTGeneration dotgen;
        dotgen.writeIncidenceGraphToDOTFile(callGraph, "call_graph.dot");
        
    }
    
    ~VirtualFunctionAnalysis (){
    }
};


#endif
