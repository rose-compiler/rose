#ifndef VIRTUAL_FUNCTION_ANALYSIS_H
#define VIRTUAL_FUNCTION_ANALYSIS_H
/**
 *  To resolve virtual funcitons correctly we need to answer the question
 * "What are the possible types that an object point to at a call site?"
 *  So we basically need to perform Pointer Aliasing Analysis. For this 
 * project we opted to do InterProcedural FlowSensitive DataFlow Analysis
 *  as described in 
 * 
 * "Flow-Sensitive Interprocedural Type Analysis for C++”, TechReport ‘95  Paul Carini Harini Srinivasan
 * 
 * So we perform Pointer Alias Analysis and Virtual Function Analysis simultenously 
 *  which in turn complements each other to be more precise. The basic  idea is
 * as follows:
 *  
 * 1. Compute a call graph ignoring all the virtual function calls.
 * 2. Iterate over the nodes of the call graph topologically and reverse topologically
 *    in alternative iteration until no Alias information is modified.
 *      i. For each function:
 *            -  Compute a Control Flow Graph consists of nodes which can generate new aliases eg.
 *               SgAssignOp, SgAssignInitializer, SgFunctionCallExp, SgConstructorInitializer etc.
 *            -  Compute the alias relations for each node
 *      ii. Perform IntraProcDataFlowAnalysis util they stabilizes
 *            - Propagate the Alias relation over the ControlFlow Graph to generate entry and exit 
 *              CompactRepresentation for each node.    
 *     iii. Using the Alias Information resolve the possible function callsites within this function
 *             - Handle the Function Parameters, return types and Constructor parameters correctly.
 *     iv. Compute Entry for all the functions called from this function         
 *
 *
 * 
 *  
 */ 
#include "PtrAliasAnalysis.h"
#include "CallGraph.h"



//! Virutal Function Analysis and PointerAliasAnalysis are
//! dependent on each other for their accuracy. That's why
//! they are done together
class ROSE_DLL_API VirtualFunctionAnalysis : public PtrAliasAnalysis {
    
    bool isExecuted;
public:

    VirtualFunctionAnalysis(SgProject *__project) : PtrAliasAnalysis(__project) {
        isExecuted = false;
    };
    
    
    void run();
    
    //! DataFlow based Virtual function resolve technique is expensive. So 
    //! it's memorized so that subsequent resolve calls can be answered. This
    //! function reset the memorization.
    void reset();
    
    //! Using the DataFlow based Virtual Function Analysis it prunes the
    //! call graph to generate more precise and accurate call graph
    void pruneCallGraph(CallGraphBuilder &);
    
    //! Given a Callsite(SgFunctionCallExp or SgConstructionInitializer) it resolves
    //! the function calls. For Virtual Functions it returns the minimal set of 
    //! functions that may be execute at runtime.
    void resolveFunctionCall(SgExpression *, std::vector<SgFunctionDeclaration*> &);
    
    ~VirtualFunctionAnalysis (){
    }
};


#endif
