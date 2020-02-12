
#ifndef CALL_GRAPH_H
#define CALL_GRAPH_H

#include <AstInterface.h>
#include <GraphDotOutput.h>
#include <VirtualGraphCreate.h>

#include "AstDiagnostics.h"

#include <sstream>
#include <iostream>
#include <string>
#include <functional>
#include <queue>
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>

class FunctionData;

typedef Rose_STL_Container<SgFunctionDeclaration *> SgFunctionDeclarationPtrList;
typedef Rose_STL_Container<SgClassDefinition *> SgClassDefinitionPtrList;

// DQ (1/31/2006): Changed name and made global function type symbol table a static data member.
// extern SgFunctionTypeTable Sgfunc_type_table;
// This header has to be here since it uses type SgFunctionDeclarationPtrList 
#include "ClassHierarchyGraph.h"


//AS(090707) Added the CallTargetSet namespace to replace the CallGraphFunctionSolver class
namespace CallTargetSet
{
  typedef Rose_STL_Container<SgFunctionDeclaration *> SgFunctionDeclarationPtrList;
  typedef Rose_STL_Container<SgClassDefinition *> SgClassDefinitionPtrList;
  // returns the list of declarations of all functions that may get called via the specified pointer
  std::vector<SgFunctionDeclaration*> solveFunctionPointerCall ( SgPointerDerefExp *, SgProject * );

  // returns the list of declarations of all functions that may get called via a member function pointer
  std::vector<SgFunctionDeclaration*> solveMemberFunctionPointerCall ( SgExpression *,ClassHierarchyWrapper * );
  Rose_STL_Container<SgFunctionDeclaration*> solveFunctionPointerCallsFunctional(SgNode* node, SgFunctionType* functionType );

  // returns the list of declarations of all functions that may get called via a
  // member function (non/polymorphic) call
  std::vector<SgFunctionDeclaration*> solveMemberFunctionCall ( 
          SgClassType *, ClassHierarchyWrapper *, SgMemberFunctionDeclaration *, bool , bool includePureVirtualFunc = false );

  //! Returns the list of all constructors that may get called via an initialization.
  //! FIXME: There is a bug in this function. 
  //! Consider the inheritance hierarchy A -> B -> C (C inherits from B, B inherits from A).
  //! Let C have an explicit constructor, without explictly calling B's constructor. We will only return the constructor for C
  std::vector<SgFunctionDeclaration*> solveConstructorInitializer ( SgConstructorInitializer* sgCtorInit);

  // Populates functionList with Properties of all functions that may get called.
  ROSE_DLL_API void getPropertiesForExpression(SgExpression* exp,
                                               ClassHierarchyWrapper* classHierarchy,
                                               Rose_STL_Container<SgFunctionDeclaration*>& propList,
                                               bool includePureVirtualFunc = false);

  //! Populates functionList with definitions of all functions that may get called. This
  //! is basically a wrapper around getPropertiesForExpression that extracts the
  //! SgFunctionDefinition from the Properties object. This returns only callees that have definitions -
  //! to get all possible callees, use getDeclarationsForExpression
  void getDefinitionsForExpression(SgExpression* exp,
                                    ClassHierarchyWrapper* classHierarchy,
                                    Rose_STL_Container<SgFunctionDefinition*>& calleeList);

  //! Populates functionList with declarations of all functions that may get called. This
  //! is basically a wrapper around getPropertiesForExpression.
  void getDeclarationsForExpression(SgExpression* exp,
                                    ClassHierarchyWrapper* classHierarchy,
                                    Rose_STL_Container<SgFunctionDeclaration*>& calleeList,
                                    bool includePureVirtualFunc = false);

  // Gets a vector of SgExpressions that are associated with the current SgFunctionDefinition.
  // This functionality is necessary for virtual, interprocedural control flow graphs. However, 
  // it is costly and should be used infrequently (or optimized!).
  void getExpressionsForDefinition(SgFunctionDefinition* targetDef, 
                                   ClassHierarchyWrapper* classHierarchy,
                                   Rose_STL_Container<SgExpression*>& exps);
  
  // Gets the latest implementation of the member function from the ancestor hierarchy
  SgFunctionDeclaration * getFirstVirtualFunctionDefinitionFromAncestors(SgClassType *crtClass, 
                                   SgMemberFunctionDeclaration *memberFunctionDeclaration, 
                                   ClassHierarchyWrapper *classHierarchy);
  
};

class ROSE_DLL_API FunctionData
{
  public:

    bool hasDefinition;

    bool isDefined (); 

    FunctionData(SgFunctionDeclaration* functionDeclaration, SgProject *project, ClassHierarchyWrapper * );

    //! All the callees of this function
    Rose_STL_Container<SgFunctionDeclaration *> functionList;

    SgFunctionDeclaration* functionDeclaration;

    Rose_STL_Container<SgMemberFunctionDeclaration*> *findPointsToVirtualFunctions ( SgMemberFunctionDeclaration * );
    bool compareFunctionDeclarations( SgFunctionDeclaration *f1, SgFunctionDeclaration *f2 );
};

//! A function object to be used as a predicate to filter out functions in a call graph: it does not filter out anything.
struct dummyFilter : public std::unary_function<bool,SgFunctionDeclaration*>
{
  bool operator() (SgFunctionDeclaration* node) const; // always return true
}; 

//! A function object to filter out builtin functions in a call graph (only non-builtin functions will be considered)
// Liao, 6/17/2012
struct ROSE_DLL_API builtinFilter : public std::unary_function<bool,SgFunctionDeclaration*>
{
  bool operator() (SgFunctionDeclaration* node) const;
}; 

class ROSE_DLL_API CallGraphBuilder
{
  public:
    CallGraphBuilder( SgProject *proj);
    //! Default builder filtering nothing in the call graph
    void buildCallGraph();
    //! Builder accepting user defined predicate to filter certain functions
    template<typename Predicate>
      void buildCallGraph(Predicate pred);
    //! Grab the call graph built
    SgIncidenceDirectedGraph *getGraph(); 
    //void classifyCallGraph();

    //We map each function to the corresponding graph node
    boost::unordered_map<SgFunctionDeclaration*, SgGraphNode*>& getGraphNodesMapping(){ return graphNodes; }

    //! Retrieve the node matching a function declaration (using mangled name to resolve accross translation units)
    SgGraphNode * getGraphNodeFor(SgFunctionDeclaration * fdecl) const;

  private:
    SgProject *project;
    SgIncidenceDirectedGraph *graph;
    //We map each function to the corresponding graph node
    typedef boost::unordered_map<SgFunctionDeclaration*, SgGraphNode*> GraphNodes;
    GraphNodes graphNodes;

};
//! Generate a dot graph named 'fileName' from a call graph 
//TODO this function is    not defined? If so, need to be removed. 
// AstDOTGeneration::writeIncidenceGraphToDOTFile() is used instead in the tutorial. Liao 6/17/2012
void GenerateDotGraph ( SgIncidenceDirectedGraph *graph, std::string fileName );

class ROSE_DLL_API GetOneFuncDeclarationPerFunction :  public std::unary_function<SgNode*, Rose_STL_Container<SgNode*> >
{
  public:
    result_type operator()(SgNode* node );
};

template<typename Predicate>
void
CallGraphBuilder::buildCallGraph(Predicate pred)
{
    // Adds additional constraints to the predicate. It makes no sense to analyze non-instantiated templates.
    struct isSelected {
        Predicate &pred;
        isSelected(Predicate &pred): pred(pred) {}
        bool operator()(SgNode *node) {
            SgFunctionDeclaration *f = isSgFunctionDeclaration(node);
         // TV (10/26/2018): FIXME ROSE-1487
         // assert(!f || f==f->get_firstNondefiningDeclaration()); // node uniqueness test
#if 0
         // DQ (8/25/2016): This is not a meaningful test since all functions will be in the memory pool, including template functions and template member functions.
            if(isSgTemplateFunctionDeclaration(f)||isSgTemplateMemberFunctionDeclaration(f)) {
              std::cerr<<"Error: CallGraphBuilder: call referring to node "<<f->class_name()<<" :: function-name:"<<f->get_qualified_name()<<std::endl;
            }
#endif
            return f && f==f->get_firstNondefiningDeclaration() &&  !isSgTemplateMemberFunctionDeclaration(f) && !isSgTemplateFunctionDeclaration(f) && pred(f);
        }
    };

    // Add nodes to the graph by querying the memory pool for function declarations, mapping them to unique declarations
    // that can be used as keys in a map (using get_firstNondefiningDeclaration()), and filtering according to the predicate.
    graph = new SgIncidenceDirectedGraph();
    std::vector<FunctionData> callGraphData;
    ClassHierarchyWrapper classHierarchy(project);
    graphNodes.clear();
    VariantVector vv(V_SgFunctionDeclaration);
    GetOneFuncDeclarationPerFunction defFunc;
    std::vector<SgNode*> fdecl_nodes = NodeQuery::queryMemoryPool(defFunc, &vv);
    BOOST_FOREACH(SgNode *node, fdecl_nodes) {
        SgFunctionDeclaration *fdecl = isSgFunctionDeclaration(node);
        SgFunctionDeclaration *unique = isSgFunctionDeclaration(fdecl->get_firstNondefiningDeclaration());
#if 0
        printf ("In buildCallGraph(): loop over functions from memory pool: fdecl  = %p = %s name = %s \n",fdecl,fdecl->class_name().c_str(),fdecl->get_name().str());
        printf ("In buildCallGraph(): loop over functions from memory pool: unique = %p = %s name = %s \n",unique,unique->class_name().c_str(),unique->get_name().str());
#endif
        if (isSelected(pred)(unique) && getGraphNodeFor(unique) == NULL)
           {
#if 0
            printf ("Collect function calls in unique function: unique = %p \n",unique);
#endif
            FunctionData fdata(unique, project, &classHierarchy); // computes functions called by unique
            callGraphData.push_back(fdata);
            std::string functionName = unique->get_qualified_name().getString();
            SgGraphNode *graphNode = new SgGraphNode(functionName);
            graphNode->set_SgNode(unique);
            graphNodes[unique] = graphNode;
            graph->addNode(graphNode);
          }
         else
          {
#if 0
            printf ("Function not selected for processing: unique = %p \n",unique);
            printf ("   --- isSelected(pred)(unique) = %s \n",isSelected(pred)(unique) ? "true" : "false");
            printf ("   --- graphNodes.find(unique)==graphNodes.end() = %s \n",graphNodes.find(unique)==graphNodes.end() ? "true" : "false");
#endif
          }
    }

    // Add edges to the graph
    BOOST_FOREACH(FunctionData &currentFunction, callGraphData) {
        SgGraphNode * srcNode = getGraphNodeFor(currentFunction.functionDeclaration);
        ROSE_ASSERT(srcNode != NULL);
        std::vector<SgFunctionDeclaration*> & callees = currentFunction.functionList;
        BOOST_FOREACH(SgFunctionDeclaration * callee, callees) {
            if (isSelected(pred)(callee)) {
                SgGraphNode * dstNode = getGraphNodeFor(callee);
                ROSE_ASSERT(dstNode != NULL);
                if (graph->checkIfDirectedGraphEdgeExists(srcNode, dstNode) == false)
                    graph->addDirectedEdge(srcNode, dstNode);
            }
        }
    }
}

// endif for CALL_GRAPH_H
#endif

