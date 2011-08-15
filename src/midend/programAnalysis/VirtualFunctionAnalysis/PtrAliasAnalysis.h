#ifndef PTR_ALIAS_ANALYSIS
#define PTR_ALIAS_ANALYSIS
#include "InterProcDataFlowAnalysis.h"
#include "IntraProcAliasAnalysis.h"
#include "ClassHierarchyGraph.h"

using namespace std;
using namespace boost;

   
class PtrAliasAnalysis : public InterProcDataFlowAnalysis{

protected:
    SgProject *project;
    SgIncidenceDirectedGraph *callGraph;
    
    boost::unordered_map<SgFunctionDeclaration *, IntraProcAliasAnalysis *> intraAliases;
    boost::unordered_map<SgExpression *, std::vector<SgFunctionDeclaration*> > resolver;
    ClassHierarchyWrapper *classHierarchy;
     // Map FunctionDefintion-> GraphNode
    CallGraphBuilder *cgBuilder;

public:
    enum COLOR {WHITE=0, GREY, BLACK};
    enum TRAVERSAL_TYPE {TOPOLOGICAL=0, REVERSE_TOPOLOGICAL};
 
    PtrAliasAnalysis(SgProject *__project);
    ~PtrAliasAnalysis();
    SgIncidenceDirectedGraph * getCallGraph() {return callGraph;}
    virtual void run();
    virtual void rebuildCallGraph();

private:
    void SortCallGraphRecursive(SgFunctionDeclaration* targetFunction, SgIncidenceDirectedGraph* callGraph,
                boost::unordered_map<SgFunctionDeclaration*, SgGraphNode*> &graphNodeToFunction, boost::unordered_map<SgGraphNode*, 
                PtrAliasAnalysis::COLOR> &colors, std::vector<SgFunctionDeclaration*> &processingOrder, 
                PtrAliasAnalysis::TRAVERSAL_TYPE order) ;
    
    void SortCallGraphNodes(SgFunctionDeclaration* targetFunction, SgIncidenceDirectedGraph* callGraph,
                boost::unordered_map<SgFunctionDeclaration*, SgGraphNode*> &graphNodeToFunction,
                std::vector<SgFunctionDeclaration*> &processingOrder, PtrAliasAnalysis::TRAVERSAL_TYPE order);

    void computeCallGraphNodes(SgFunctionDeclaration* targetFunction, SgIncidenceDirectedGraph* callGraph,
                std::vector<SgFunctionDeclaration*> &processingOrder, PtrAliasAnalysis::TRAVERSAL_TYPE order);
    
    
};

#endif