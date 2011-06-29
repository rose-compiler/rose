
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
  void getPropertiesForExpression(SgExpression* exp,
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
};

class FunctionData
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

//! A function object to be used as a predicate to filter out functions in a call graph
struct dummyFilter : public std::unary_function<bool,SgFunctionDeclaration*>
{
  bool operator() (SgFunctionDeclaration* node) const;
}; 

class CallGraphBuilder
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
  private:
    SgProject *project;
    SgIncidenceDirectedGraph *graph;
};
//! Generate a dot graph named 'fileName' from a call graph
void GenerateDotGraph ( SgIncidenceDirectedGraph *graph, std::string fileName );

class GetOneFuncDeclarationPerFunction :  public std::unary_function<SgNode*, Rose_STL_Container<SgNode*> >
{
  public:
    result_type operator()(SgNode* node );
};

template<typename Predicate>
  void
CallGraphBuilder::buildCallGraph(Predicate pred)
{
    Rose_STL_Container<FunctionData> callGraphData;

    //AS (09/23/06) Query the memory pool instead of subtree of project
    VariantVector vv(V_SgFunctionDeclaration);
    GetOneFuncDeclarationPerFunction defFunc;
    Rose_STL_Container<SgNode *> allFunctions = NodeQuery::queryMemoryPool(defFunc, &vv);

    ClassHierarchyWrapper classHierarchy(project);
    Rose_STL_Container<SgNode *>::iterator i = allFunctions.begin();

    //Iterate through all the functions found and resolve all the call expressions in each function with a body
    while (i != allFunctions.end())
    {
        SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
        ROSE_ASSERT(functionDeclaration != NULL);

        // determining the in-class declaration
        if (isSgMemberFunctionDeclaration(functionDeclaration))
        {
            // always saving the in-class declaration, so we need to find that one
            SgDeclarationStatement *nonDefDeclInClass =
                    isSgMemberFunctionDeclaration(functionDeclaration->get_firstNondefiningDeclaration());
            // functionDeclaration is outside the class (so it must have a definition)
            if (nonDefDeclInClass)
                functionDeclaration = isSgMemberFunctionDeclaration(nonDefDeclInClass);
        }
        else
        {
            // we need to have only one declaration for regular functions as well
            SgFunctionDeclaration *nonDefDecl = isSgFunctionDeclaration(functionDeclaration->get_firstNondefiningDeclaration());
            if (nonDefDecl)
                functionDeclaration = nonDefDecl;
        }

        //AS(032806) Filter out functions based on criteria in predicate
        if (pred(functionDeclaration) == true)
        {
            FunctionData functionData(functionDeclaration, project, &classHierarchy);
            ROSE_ASSERT(functionData.functionDeclaration != NULL);

            callGraphData.push_back(functionData);
        }
        i++;
    }

    // Build the graph
    SgIncidenceDirectedGraph *returnGraph = new SgIncidenceDirectedGraph();
    ROSE_ASSERT(returnGraph != NULL);

    //We map each function to the corresponding graph node
    boost::unordered_map<SgFunctionDeclaration*, SgGraphNode*> graphNodes;
    
    //Instantiate all the nodes in the graph, one for each function we found
    BOOST_FOREACH(FunctionData& currentFunction, callGraphData)
    {
        std::string functionName;
        ROSE_ASSERT(currentFunction.functionDeclaration);
        functionName = currentFunction.functionDeclaration->get_mangled_name().getString();

        // Generate a unique name to test against later
        SgFunctionDeclaration* id = currentFunction.functionDeclaration;
        SgDeclarationStatement *nonDefDeclInClass = isSgMemberFunctionDeclaration(id->get_firstNondefiningDeclaration());
        if (nonDefDeclInClass)
            ROSE_ASSERT(id == nonDefDeclInClass);
        SgGraphNode* graphNode = new SgGraphNode(functionName);
        graphNode->set_SgNode(currentFunction.functionDeclaration);

        if (SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL)
        {
            std::cout << "Function: "
                    << currentFunction.functionDeclaration->get_scope()->get_qualified_name().getString() +
                    currentFunction.functionDeclaration->get_mangled_name().getString()
                    << " has declaration " << currentFunction.isDefined() << "\n";
        }
        
        graphNodes[currentFunction.functionDeclaration] = graphNode;

        returnGraph->addNode(graphNode);
    }

    if (SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL)
        std::cout << "NodeList size: " << graphNodes.size() << "\n";

    //We have all the nodes. Now instantiate all the graph edges
    int totEdges = 0;
    BOOST_FOREACH(FunctionData& currentFunction, callGraphData)
    {
        ROSE_ASSERT(currentFunction.functionDeclaration != NULL);
        
        boost::unordered_map<SgFunctionDeclaration*, SgGraphNode*>::iterator iter
                = graphNodes.find(currentFunction.functionDeclaration);
        ROSE_ASSERT(iter != graphNodes.end());
        SgGraphNode* startingNode = iter->second;

        Rose_STL_Container<SgFunctionDeclaration*> & functionCallees = currentFunction.functionList;

        BOOST_FOREACH(SgFunctionDeclaration* calleeDeclaration, functionCallees)
        {
                        ROSE_ASSERT(calleeDeclaration != NULL);

                        //This function has been filtered out
                        if (pred(calleeDeclaration) == false)
                        {
                                continue;
                        }

                        iter = graphNodes.find(calleeDeclaration);
                        ROSE_ASSERT(iter != graphNodes.end());
                        SgGraphNode *endNode = iter->second;

                        if (returnGraph->checkIfDirectedGraphEdgeExists(startingNode, endNode) == false)
                        {
                                ROSE_ASSERT(startingNode != NULL && endNode != NULL);
                                returnGraph->addDirectedEdge(startingNode, endNode);
                        }
                        else if (SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL)
                        {
                                std::cout << "Did not add edge since it already exist" << std::endl;
                                std::cout << "\tEndNode " << calleeDeclaration->get_name().str() << "\n";
                        }

            totEdges++;
        }
    }
    
    if (SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL)
        std::cout << "Total number of edges: " << totEdges << "\n";

    graph = returnGraph;
};

// endif for CALL_GRAPH_H
#endif

