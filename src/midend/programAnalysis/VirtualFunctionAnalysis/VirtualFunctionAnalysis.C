#include "sage3basic.h"
#include <algorithm>
#include "VirtualFunctionAnalysis.h"
void VirtualFunctionAnalysis::run()
   {

     printf ("In VirtualFunctionAnalysis::run() \n");

        vector<SgExpression*> callSites = SageInterface::querySubTree<SgExpression> (project, V_SgFunctionCallExp);

     printf ("In VirtualFunctionAnalysis::run(): after querySubTree on V_SgFunctionCallExp \n");

        vector<SgExpression*> constrs = SageInterface::querySubTree<SgExpression> (project, V_SgConstructorInitializer);

     printf ("In VirtualFunctionAnalysis::run(): callSites.insert() \n");

        callSites.insert(callSites.end(), constrs.begin(), constrs.end());

     printf ("DONE: VirtualFunctionAnalysis::run(): callSites.insert() \n");

        // Not all SgFunctionCallExp or SgConstructorInitialize nodes appear in functions--some are also in templates (for
        // classes or functions) and we don't want to process those. Templates are not really part of a control flow graph or
        // call graph until after they're instantiated.
        for (vector<SgExpression*>::iterator csi=callSites.begin(); csi!=callSites.end(); ++csi) {
            if (SgFunctionDeclaration *fdecl = SageInterface::getEnclosingNode<SgFunctionDeclaration>(*csi)) {
                if (isSgTemplateMemberFunctionDeclaration(fdecl) || isSgTemplateFunctionDeclaration(fdecl))
                    *csi = NULL;
            }

          printf ("In VirtualFunctionAnalysis::run(): loop 1 \n");

        }

          printf ("In VirtualFunctionAnalysis::run(): erase \n");

        callSites.erase(std::remove(callSites.begin(), callSites.end(), (SgExpression*)NULL), callSites.end());

          printf ("DONE: VirtualFunctionAnalysis::run(): erase \n");

        unsigned int index;
        resolver.clear();

          printf ("DONE: VirtualFunctionAnalysis::run(): clear \n");

        for(index = 0; index < callSites.size(); index++) {
            std::vector<SgFunctionDeclaration *> funcs;

          printf ("In VirtualFunctionAnalysis::run(): getPropertiesForExpression() \n");

            CallTargetSet::getPropertiesForExpression(callSites[index], classHierarchy, funcs);

            //Virtual Function
            if(isSgFunctionCallExp(callSites[index]) && funcs.size() > 1 )
                funcs.clear();
            resolver[callSites[index]] = funcs;

          printf ("In VirtualFunctionAnalysis::run(): loop 2 \n");
        }

          printf ("In VirtualFunctionAnalysis::run(): PtrAliasAnalysis::run() \n");

        PtrAliasAnalysis::run();

          printf ("DONE: VirtualFunctionAnalysis::run(): PtrAliasAnalysis::run() \n");

        for(index = 0; index < callSites.size(); index++) {
            if(resolver.at(callSites[index]).size() == 0  ) {
                std::vector<SgFunctionDeclaration *> funcs;
                CallTargetSet::getPropertiesForExpression(callSites[index], classHierarchy, funcs);
                resolver[callSites[index]] = funcs;
            }

          printf ("In VirtualFunctionAnalysis::run(): loop 3 \n");
        }
        isExecuted = true;

     printf ("Leaving VirtualFunctionAnalysis::run() \n");
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
        for (map::value_type it: node_mapping) {
            SgFunctionDeclaration *defDecl =
                (
                    it.first->get_definition() != NULL ?
                    it.first : isSgFunctionDeclaration(it.first->get_definingDeclaration())
                );

            if(defDecl == NULL) continue;

            Rose_STL_Container<SgNode*> functionCallExpList = NodeQuery::querySubTree(defDecl, V_SgFunctionCallExp);
            std::vector<SgFunctionDeclaration *> functions;
            for(SgNode* functionCallExp: functionCallExpList) {
                functions.insert(functions.end(), resolver.at(isSgExpression(functionCallExp)).begin(),
                                                resolver.at(isSgExpression(functionCallExp)).end());
            }

            Rose_STL_Container<SgNode*> ctorInitList = NodeQuery::querySubTree(defDecl, V_SgConstructorInitializer);
            for(SgNode* ctorInit: ctorInitList) {
                functions.insert(functions.end(), resolver.at(isSgExpression(ctorInit)).begin(),
                                                resolver.at(isSgExpression(ctorInit)).end());
            }

            std::set<SgDirectedGraphEdge *>edges = graph->computeEdgeSetOut(it.second);

            for(SgDirectedGraphEdge *edge: edges) {
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
