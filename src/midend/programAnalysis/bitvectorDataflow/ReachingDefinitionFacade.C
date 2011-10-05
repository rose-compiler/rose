#include "ReachingDefinitionFacade.h"
#include "DefUseChain.h"
#include "GraphDotOutput.h"

ReachingDefinitionFacade::ReachingDefinitionFacade(SgNode* head)  {
        functionDefinition = isSgFunctionDefinition(head);
        ROSE_ASSERT(functionDefinition != NULL );
        
}


void ReachingDefinitionFacade::run(){

    SgBasicBlock *stmts = functionDefinition->get_body();  
    AstInterfaceImpl scope(stmts);
    AstInterface fa(&scope);

    AstNodePtrImpl astNodePtr(functionDefinition);
    ReachingDefinitionAnalysis r;
    r(fa, astNodePtr);
    
    StmtVarAliasCollect alias;
    alias(fa, astNodePtr);
    
    graph = new DefaultDUchain();
    graph->build(fa, r, alias);
}


void ReachingDefinitionFacade::toDot(std::string fileName) {
    
     GraphDotOutput<DefaultDUchain>output(*graph);
     output.writeToDOTFile(fileName,"ReachingDefinitionAnalysis"); 

}
