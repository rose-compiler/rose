#include "VirtualFunctionAnalysis.h"
#include <algorithm>
void VirtualFunctionAnalysis::run(){
    
        vector<SgExpression*> callSites = SageInterface::querySubTree<SgExpression> (project, V_SgFunctionCallExp);
        vector<SgExpression*> constrs = SageInterface::querySubTree<SgExpression> (project, V_SgConstructorInitializer);
        
        callSites.insert(callSites.end(), constrs.begin(), constrs.end());
        
        
        unsigned int index;


        resolver.clear();
        for(index = 0; index < callSites.size(); index++) {
            std::vector<SgFunctionDeclaration *> funcs;
            
            CallTargetSet::getPropertiesForExpression(callSites[index], classHierarchy, funcs);
            
            //Virtual Function
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
       
 }

void VirtualFunctionAnalysis::resolveFunctionCall(SgExpression *call_exp, std::vector<SgFunctionDeclaration*> &functions) {
     if(isExecuted == false)
         run();

    ROSE_ASSERT(call_exp != NULL);
    ROSE_ASSERT(resolver.count(call_exp) != 0);
    functions.clear();
    functions.insert(functions.end(), resolver.at(call_exp).begin(), resolver.at(call_exp).end());
}

void VirtualFunctionAnalysis::pruneCallGraph(CallGraphBuilder& builder) {
        SgIncidenceDirectedGraph *graph =  builder.getGraph();
              
        ROSE_ASSERT(graph != NULL);
        boost::unordered_map<SgFunctionDeclaration*, SgGraphNode*>node_mapping =  builder.getGraphNodesMapping();
        
        typedef boost::unordered_map<SgFunctionDeclaration *, SgGraphNode *> map;
        foreach (map::value_type it, node_mapping) {
            SgFunctionDeclaration *defDecl =
                (
                    it.first->get_definition() != NULL ?
                    it.first : isSgFunctionDeclaration(it.first->get_definingDeclaration())
                );
            
            if(defDecl == NULL) continue;
            
            Rose_STL_Container<SgNode*> functionCallExpList = NodeQuery::querySubTree(defDecl, V_SgFunctionCallExp);
            std::vector<SgFunctionDeclaration *> functions;
            foreach(SgNode* functionCallExp, functionCallExpList) {
                functions.insert(functions.end(), resolver.at(isSgExpression(functionCallExp)).begin(), 
                                                resolver.at(isSgExpression(functionCallExp)).end());
            }

            Rose_STL_Container<SgNode*> ctorInitList = NodeQuery::querySubTree(defDecl, V_SgConstructorInitializer);
            foreach(SgNode* ctorInit, ctorInitList) {
                functions.insert(functions.end(), resolver.at(isSgExpression(ctorInit)).begin(), 
                                                resolver.at(isSgExpression(ctorInit)).end());
            }
            
            std::set<SgDirectedGraphEdge *>edges = graph->computeEdgeSetOut(it.second);

            foreach(SgDirectedGraphEdge *edge, edges) {
                SgGraphNode *toNode = edge->get_to();
                SgFunctionDeclaration *toDecl = isSgFunctionDeclaration(toNode->get_SgNode());
                ROSE_ASSERT(toDecl != NULL);
                
                if(toDecl->get_specialFunctionModifier().isConstructor() || toDecl->get_specialFunctionModifier().isDestructor())
                    continue;
                
                if(find(functions.begin(), functions.end(), toDecl) == functions.end()) {
                     graph->removeDirectedEdge(edge);
                     std::cout << "Edge removed from " << defDecl->get_qualified_name().getString() << " to " << 
                             toDecl->get_qualified_name().getString()<< std::endl;
                }
                
            }
          
        }

}

void VirtualFunctionAnalysis::reset() {
     isExecuted = false;
     resolver.clear();
             
}