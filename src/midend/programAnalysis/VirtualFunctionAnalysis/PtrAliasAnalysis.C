#include "sage3basic.h"
#include <CallGraph.h>
#include "PtrAliasAnalysis.h"

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH


struct FunctionFilter
{
        bool operator()(SgFunctionDeclaration* funcDecl)
        {
                ROSE_ASSERT(funcDecl != NULL);

                //Don't process any built-in functions
                std::string filename = funcDecl->get_file_info()->get_filename();
                if (filename.find("include") != string::npos)
                        return false;

                //Exclude compiler generated functions, but keep template instantiations
                if (funcDecl->get_file_info()->isCompilerGenerated() && !isSgTemplateInstantiationFunctionDecl(funcDecl)
                                && !isSgTemplateInstantiationMemberFunctionDecl(funcDecl))
                        return false;

                //We don't process functions that don't have definitions
                if (funcDecl->get_definingDeclaration() == NULL || 
                            isSgFunctionDeclaration(funcDecl->get_definingDeclaration())->get_definition() == NULL)
                        return false;
                
                // If it's a virtual function we don't add it to the call graph initially
                if(funcDecl->get_functionModifier().isVirtual() == true)
                    return false;
                
                return true;
        }
};

struct OnlyNonCompilerGenerated : public std::unary_function<bool, SgFunctionDeclaration*>
{

    bool operator() (SgFunctionDeclaration * node) const
    {
        // false will filter out ALL nodes
        bool filterNode = true;

        SgFunctionDeclaration *fct = isSgFunctionDeclaration(node);
        if (fct != NULL)
        {
            bool ignoreFunction = (fct->get_file_info()->isCompilerGenerated() == true);
            if (ignoreFunction == true)
                filterNode = false;
        }

        return filterNode;
    }
};

 PtrAliasAnalysis:: PtrAliasAnalysis(SgProject* __project) : InterProcDataFlowAnalysis(__project) {
        intraAliases.clear();
        classHierarchy = new ClassHierarchyWrapper(project);
        // Create the Call Graph
        cgBuilder = new CallGraphBuilder(project);
        //CallGraphBuilder cg(project);
        cgBuilder->buildCallGraph(FunctionFilter());
        callGraph = cgBuilder->getGraph();
        
        AstDOTGeneration dotgen;
        dotgen.writeIncidenceGraphToDOTFile(cgBuilder->getGraph(), "init_call_graph.dot");
#if 0        
        typedef boost::unordered_map<SgFunctionDeclaration*, SgGraphNode*> res_map;    
        foreach (res_map::value_type it, cg2.getGraphNodesMapping()) {
            std::cout << it.first <<" - "<< isSgFunctionDeclaration(it.first->get_definingDeclaration()) << " - " << it.first->get_name().getString() << std::endl;
        }
#endif        
 }
 PtrAliasAnalysis::~PtrAliasAnalysis() {

        typedef boost::unordered_map<SgFunctionDeclaration *, IntraProcAliasAnalysis *> map;
        foreach (map::value_type it, intraAliases) {
                delete ((IntraProcAliasAnalysis *)it.second);
        }
}
 
 bool PtrAliasAnalysis::runAndCheckIntraProcAnalysis(SgFunctionDeclaration* funcDecl) {
     if(intraAliases.count(funcDecl) == 0) {
         std::cerr << funcDecl->get_qualified_name().getString() << " not found in IntraProcAnalysis.\n";
         ROSE_ASSERT(false);
     }
     return (((IntraProcAliasAnalysis *)(intraAliases[funcDecl]))->runCheck());
 }
 
 void PtrAliasAnalysis::getFunctionDeclarations(std::vector<SgFunctionDeclaration*> &processingOrder) {

     // Get the main function declaration
        SgFunctionDeclaration *mainDecl = SageInterface::findMain(project);
        ROSE_ASSERT (mainDecl->get_definingDeclaration () == mainDecl);
     // Liao 1/23/2013
     // Call graph generation will get the first nondefining declaration func as the function node by default, see CallGraph.h line 198.
     // In ROSE using EDG 4.4, main() function has a hidden prototype, which will be stored in call graph.
     // We have to use the prototype or it complains the defining main() does not exist in the call graph.
        SgFunctionDeclaration * nondef_main = isSgFunctionDeclaration(mainDecl->get_firstNondefiningDeclaration());
        ROSE_ASSERT (nondef_main);
        computeCallGraphNodes(nondef_main, callGraph, processingOrder, order);
            
        // Order the graph nodes in alternate fashion
        order =  (order == TOPOLOGICAL) ? REVERSE_TOPOLOGICAL : TOPOLOGICAL;

 
 }

void PtrAliasAnalysis:: SortCallGraphRecursive(SgFunctionDeclaration* targetFunction, SgIncidenceDirectedGraph* callGraph,
               unordered_map<SgFunctionDeclaration*, SgGraphNode*> &graphNodeToFunction, unordered_map<SgGraphNode*, COLOR> &colors,
               vector<SgFunctionDeclaration*> &processingOrder, TRAVERSAL_TYPE order) {
        
                //If the function is already in the list of functions to be processed, don't add it again.
        if (find(processingOrder.begin(), processingOrder.end(), targetFunction) != processingOrder.end())
                return;

        if (graphNodeToFunction.count(targetFunction) == 0)
        {
          printf("The function %s has no vertex in the call graph!\n", targetFunction->get_name().str());
          printf("graphNodeToFunction contains:\n");
          unordered_map<SgFunctionDeclaration*, SgGraphNode*>::const_iterator iter;
          for (iter= graphNodeToFunction.begin(); iter != graphNodeToFunction.end(); iter ++)
          {
            SgFunctionDeclaration* func = (*iter).first;
            cout<<func<<"\n\t"<<func->unparseToString()<<endl;
          }
          ROSE_ASSERT(false);
        }

        SgGraphNode* graphNode = graphNodeToFunction.at(targetFunction);
        
        
        assert(graphNode != NULL);
        
        colors[graphNode] = GREY;

        if (order == TOPOLOGICAL)
                processingOrder.push_back(targetFunction);
        
        
        vector<SgGraphNode*> callees;
        callGraph->getSuccessors(graphNode, callees);

        //Recursively process all the callees before adding this function to the list
        foreach(SgGraphNode* calleeNode, callees)
        {
            if(colors.at(calleeNode)  == WHITE) {
                SgFunctionDeclaration* calleeDecl = isSgFunctionDeclaration(calleeNode->get_SgNode());
                ROSE_ASSERT(calleeDecl != NULL);
                
                SortCallGraphRecursive(calleeDecl, callGraph, graphNodeToFunction, colors, processingOrder, order);
            }
        }

        //If the function is already in the list of functions to be processed, don't add it again.
        //Some of our callees might have added us due to recursion

        if (find(processingOrder.begin(), processingOrder.end(), targetFunction) == processingOrder.end() 
                && order == REVERSE_TOPOLOGICAL)
                processingOrder.push_back(targetFunction);

        colors[graphNode] = BLACK;
    
    }
    
/**
 * SortCallGraph: Process a call graph in arbitrary order
 * @param targetFunction: Function to be processed
 * @param callGraph:     Computed CallGraph
 * @param graphNodeToFunction: A mapping of SgFunctionDefintion* to SgGraphNode*
 * @param processingOrder: The order of processing the nodes
 * @param order: 0-topological, 1- reverse
 */
void PtrAliasAnalysis::SortCallGraphNodes(SgFunctionDeclaration* targetFunction, SgIncidenceDirectedGraph* callGraph,
                unordered_map<SgFunctionDeclaration*, SgGraphNode*> &graphNodeToFunction,
                vector<SgFunctionDeclaration*> &processingOrder, TRAVERSAL_TYPE order) {
        
        unordered_map<SgGraphNode*, COLOR> colors;
        typedef unordered_map<SgFunctionDeclaration*, SgGraphNode*> my_map;
        
        foreach(my_map::value_type item, graphNodeToFunction) {
            colors[item.second] = WHITE;
        }
        
         SortCallGraphRecursive(targetFunction, callGraph,
                graphNodeToFunction, colors,
                processingOrder, order);
    
    }
void PtrAliasAnalysis::computeCallGraphNodes(SgFunctionDeclaration* targetFunction, SgIncidenceDirectedGraph* callGraph,
                vector<SgFunctionDeclaration*> &processingOrder, TRAVERSAL_TYPE order) {

        boost::unordered_map<SgFunctionDeclaration*, SgGraphNode*> graphNodeToFunction ;

        SortCallGraphNodes(targetFunction, callGraph, cgBuilder->getGraphNodesMapping(), processingOrder, order);

}

void PtrAliasAnalysis::run()  {

#if 0
     printf ("In PtrAliasAnalysis::run(): TOP \n");
#endif
        CallGraphBuilder fullCallGraph(project);
        fullCallGraph.buildCallGraph();
        std::set<SgGraphNode *>allNodes =  fullCallGraph.getGraph()->computeNodeSet();

        int counter = 0;
        foreach(SgGraphNode *node, allNodes) {
#if 0
          printf ("In PtrAliasAnalysis::run(): in loop body: counter = %d \n",counter);
#endif
            counter++;
        
            SgFunctionDeclaration *funcDecl = isSgFunctionDeclaration(node->get_SgNode());
            
            assert(funcDecl != NULL);
            
            if(funcDecl->get_definingDeclaration() == NULL || isSgFunctionDeclaration(funcDecl->get_definingDeclaration())->get_definition() == NULL)
                continue;
#if 0
          printf ("In PtrAliasAnalysis::run(): in loop body: calling IntraProcAliasAnalysis \n");
#endif                
            IntraProcAliasAnalysis *intra = new IntraProcAliasAnalysis(funcDecl, classHierarchy, cgBuilder, intraAliases, resolver);
#if 0
          printf ("In PtrAliasAnalysis::run(): in loop body: AFTER calling IntraProcAliasAnalysis \n");
#endif
            intra->init();
#if 0
          printf ("In PtrAliasAnalysis::run(): in loop body: AFTER calling init() \n");
#endif
            intraAliases[funcDecl] = intra;

        }
        
#if 0
     printf ("In PtrAliasAnalysis::run(): after loop \n");
#endif
        order = TOPOLOGICAL;
        
        // Get the main funciton declaration
        SgFunctionDeclaration *mainDecl = SageInterface::findMain(project);
        
        ROSE_ASSERT(mainDecl != NULL);
        
        InterProcDataFlowAnalysis::run();

#if 0
     printf ("Leaving PtrAliasAnalysis::run(): TOP \n");
#endif
 }

