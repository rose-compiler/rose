#include "PtrAliasAnalysis.h"
#include <CallGraph.h>
#include "sage3basic.h"
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

 PtrAliasAnalysis:: PtrAliasAnalysis(SgProject* __project) : project(__project) {
        intraAliases.clear();
        classHierarchy = new ClassHierarchyWrapper(project);
        // Create the Call Graph
        cgBuilder = new CallGraphBuilder(project);
        //CallGraphBuilder cg(project);
        cgBuilder->buildCallGraph(FunctionFilter());
        callGraph = cgBuilder->getGraph();
        
        AstDOTGeneration dotgen;
        CallGraphBuilder cg2(project);
        cg2.buildCallGraph();
         
        dotgen.writeIncidenceGraphToDOTFile(cgBuilder->getGraph(), "init_call_graph.dot");
        dotgen.writeIncidenceGraphToDOTFile(cg2.getGraph(), "original_call_graph.dot");
        
        typedef boost::unordered_map<SgFunctionDeclaration*, SgGraphNode*> res_map;    
        foreach (res_map::value_type it, cgBuilder->getGraphNodesMapping()) {
            std::cout << it.first <<" - "<< isSgFunctionDeclaration(it.first->get_definingDeclaration()) << " - " << it.first->get_name().getString() << std::endl;
        }
        
 }
 PtrAliasAnalysis::~PtrAliasAnalysis() {

        typedef boost::unordered_map<SgFunctionDeclaration *, IntraProcAliasAnalysis *> map;
        foreach (map::value_type it, intraAliases) {
                delete ((IntraProcAliasAnalysis *)it.second);
        }
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
        
        // Get all the nodes of the graph
        //VariantVector vv(V_SgFunctionDeclaration);
        //GetOneFuncDeclarationPerFunction defFunc;
        //Rose_STL_Container<SgNode *> allFunctions = NodeQuery::queryMemoryPool(defFunc, &vv);
    
        
        CallGraphBuilder fullCallGraph(project);
        fullCallGraph.buildCallGraph();
        std::set<SgGraphNode *>allNodes =  fullCallGraph.getGraph()->computeNodeSet();

        // Initialize all the functions irrespective of whether they are part of the call graph or not
        std::cout << "Number of Functions: "<< allNodes.size() << std::endl;
        
        foreach(SgGraphNode *node, allNodes) {
            
            SgFunctionDeclaration *funcDecl = isSgFunctionDeclaration(node->get_SgNode());
            
            assert(funcDecl != NULL);
            
            if(funcDecl->get_definingDeclaration() == NULL 
                    || isSgFunctionDeclaration(funcDecl->get_definingDeclaration())->get_definition() == NULL)
                continue;
                
            
            IntraProcAliasAnalysis *intra = new IntraProcAliasAnalysis(funcDecl, classHierarchy, cgBuilder, intraAliases, resolver);
            intra->init();
            intraAliases[funcDecl] = intra;
//                std::cout << "-->>>Function Name :" << funcDef->get_mangled_name().getString() << std::endl;
        }
        
        
         
        TRAVERSAL_TYPE order = TOPOLOGICAL;
        
        // Get the main funciton declaration
        SgFunctionDeclaration *mainDecl = SageInterface::findMain(project);
        assert(mainDecl != NULL);
            
        
        bool change;
        int iteration =0, extra_iteration =0;;
        do {
            change = false;
            std::cout << "Starting InterProcedural iteration: " << iteration << std::endl;
            vector<SgFunctionDeclaration*> processingOrder;        
            
            computeCallGraphNodes(mainDecl, callGraph, processingOrder, order);
            //SortCallGraphNodes(mainDef, callGraph, graphNodeToFunction, processingOrder, order);
            
            
            // Order the graph nodes in alternate fashion
            order =  (order == TOPOLOGICAL) ? REVERSE_TOPOLOGICAL : TOPOLOGICAL;
            
            
            std::cout << "Number of items in ProcessingOrder: "<< processingOrder.size()<< std::endl;
            
           
            foreach (SgFunctionDeclaration* funcDecl, processingOrder) {
                std::cout << funcDecl->get_name().getString() << std::endl;
                change |= (((IntraProcAliasAnalysis *)(intraAliases[funcDecl]))->runCheck());
            }
            iteration++;
            if(!change) {
                     
                typedef boost::unordered_map<SgExpression *, std::vector<SgFunctionDeclaration*> > res_map;
                foreach (res_map::value_type it, resolver) {
                    if(it.second.size() == 0) { change = true; break;}
                }
                extra_iteration++;
                if(extra_iteration >2) break;
            }
        } while (change);
        
        std::cout << "Total InterProcedural iterations: " << iteration << std::endl;
        std::cout << "Extra iterations: " << extra_iteration << std::endl;
        
    
    
    }

void PtrAliasAnalysis::rebuildCallGraph() {
                typedef boost::unordered_map<SgExpression *, std::vector<SgFunctionDeclaration*> > res_map;
                SgGraphNode *fromNode, *toNode;
                foreach (res_map::value_type it, resolver) {
                    SgFunctionDeclaration *fromDecl = SageInterface::getEnclosingFunctionDeclaration(it.first,false);
                    if(fromDecl == NULL) {
                      std::cerr<< "Declaration not found for callSite:\n" ;
                      std::cerr<< it.first->unparseToCompleteString()<< std::endl;
                      continue;
                    }
                    //assert(fromDecl != NULL);

                    if(cgBuilder->getGraphNodesMapping().count(fromDecl) ==0) {
                            fromNode = new SgGraphNode(fromDecl->get_qualified_name().getString());;
                            fromNode->set_SgNode(fromDecl);
                            callGraph->addNode(fromNode);
                            (cgBuilder->getGraphNodesMapping())[fromDecl] = fromNode;
        
                        } else {
                            toNode = (cgBuilder->getGraphNodesMapping())[fromDecl] ;
                        }
                    
                    for(unsigned int index =0; index < it.second.size();index ++){
                            
                        SgFunctionDeclaration *toDecl  = it.second[index];
                        
                        
                        if(cgBuilder->getGraphNodesMapping().count(toDecl) ==0) {
                            toNode = new SgGraphNode(toDecl->get_qualified_name().getString());;
                            toNode->set_SgNode(toDecl);
                            callGraph->addNode(toNode);
                            (cgBuilder->getGraphNodesMapping())[toDecl] = toNode;
        
                        } else {
                            toNode = (cgBuilder->getGraphNodesMapping())[toDecl] ;
                        }
                        
                         if(!(callGraph->checkIfDirectedGraphEdgeExists(fromNode, toNode))) {
                             callGraph->addDirectedEdge(fromNode, toNode);
                         }   
                    }
                    
                }

}