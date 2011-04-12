
#ifndef CALL_GRAPH_H
#define CALL_GRAPH_H

#include <AstInterface.h>
#include <GraphDotOutput.h>
#include <VirtualGraphCreate.h>

// DQ (7/28/2005): Don't include the data base
#ifdef HAVE_MYSQL
// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
#include <GlobalDatabaseConnection.h>
#endif
#endif

#include "AstDiagnostics.h"

#include <sstream>
#include <iostream>
#include <string>
#include <functional>
#include <queue>
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>

#ifdef HAVE_SQLITE3
#include "sqlite3x.h"
#endif

class Properties;
class FunctionData;

// driscoll6 (1/2011) Use reference-counting shared_ptrs to avoid
// memory leaks. (at the suggestion of George (vulov1))
#include <boost/shared_ptr.hpp>
typedef boost::shared_ptr<Properties> PropertiesPtr;

//Only used when SOLVE_FUNCTION_CALLS_IN_DB is defined
class Properties : public AstAttribute
{
  public:

    bool isPointer, isPolymorphic;
    SgClassDefinition *invokedClass;
    SgFunctionDeclaration *functionDeclaration;
    SgType *functionType;

    std::string nid;
    std::string label;
    std::string type;
    std::string scope;
    std::string functionName;

    bool hasDef;
    bool isPtr;
    bool isPoly;

    Properties();
    Properties(Properties* prop);
    Properties(SgFunctionDeclaration* inputFunctionDeclaration);
    Properties(std::string nid, std::string label, std::string type, std::string scope,
        bool hasDef, bool isPtr, bool isPoly);

};


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
  std::vector<Properties*> solveFunctionPointerCall ( SgPointerDerefExp *, SgProject * );

  // returns the list of declarations of all functions that may get called via a member function pointer
  std::vector<Properties*> solveMemberFunctionPointerCall ( SgExpression *,ClassHierarchyWrapper * );
  Rose_STL_Container<SgFunctionDeclaration*> solveFunctionPointerCallsFunctional(SgNode* node, SgFunctionType* functionType );

  // returns the list of declarations of all functions that may get called via a
  // member function (non/polymorphic) call
  std::vector<Properties*> solveMemberFunctionCall ( SgClassType *, ClassHierarchyWrapper *,            SgMemberFunctionDeclaration *, bool );

  // returns the list of Properties of all constructors that may get called via 
  // an initialization.
  std::vector<Properties*> solveConstructorInitializer ( SgConstructorInitializer* sgCtorInit);

  // Populates functionList with Properties of all functions that may get called.
  void getPropertiesForExpression(SgExpression* exp,
                                    ClassHierarchyWrapper* classHierarchy,
                                    Rose_STL_Container<Properties*>& propList);

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
                                    Rose_STL_Container<SgFunctionDeclaration*>& calleeList);

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

    //@{
    //Interfaces only used when DB is defined
  public:
    FunctionData ( SgFunctionDeclaration* functionDeclaration, SgProject *project, ClassHierarchyWrapper * );
    Properties* properties;

    // Relevant data for call graph
    Rose_STL_Container<Properties *> functionList;

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
    Rose_STL_Container<SgNode *> functionCallees = NodeQuery::queryMemoryPool(defFunc, &vv);

    ClassHierarchyWrapper classHierarchy(project);
    Rose_STL_Container<SgNode *>::iterator i = functionCallees.begin();

    // printf ("Inside of buildCallGraph functionList.size() = %zu \n",functionList.size());

    Rose_STL_Container<SgNode *> resultingFunctions;

    //Iterate through all the functions found and resolve all the call expressions in each function with a body
    while (i != functionCallees.end())
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
            ROSE_ASSERT(functionData.properties->functionDeclaration != NULL);

            resultingFunctions.push_back(*i);
            callGraphData.push_back(functionData);
        }
        i++;
    }

    functionCallees = resultingFunctions;
    // Build the graph
    SgIncidenceDirectedGraph *returnGraph = new SgIncidenceDirectedGraph();
    ROSE_ASSERT(returnGraph != NULL);

    //Rose_STL_Container<FunctionData>::iterator currentFunction = callGraphData.begin();

    //printf ("Build the node list callGraphData.size() = %zu \n",callGraphData.size());

    //We map each function to the corresponding graph node
    boost::unordered_map<SgFunctionDeclaration*, SgGraphNode*> graphNodes;
    
    //Instantiate all the nodes in the graph, one for each function we found
    BOOST_FOREACH(FunctionData& currentFunction, callGraphData)
    {
        std::string functionName;
        ROSE_ASSERT(currentFunction.properties->functionDeclaration);
        functionName = currentFunction.properties->functionDeclaration->get_mangled_name().getString();

        // Generate a unique name to test against later
        SgFunctionDeclaration* id = currentFunction.properties->functionDeclaration;
        SgDeclarationStatement *nonDefDeclInClass = isSgMemberFunctionDeclaration(id->get_firstNondefiningDeclaration());
        if (nonDefDeclInClass)
            ROSE_ASSERT(id == nonDefDeclInClass);
        SgGraphNode* graphNode = new SgGraphNode(functionName);
        graphNode->set_SgNode(currentFunction.properties->functionDeclaration);

        graphNode->addNewAttribute("Properties", currentFunction.properties);

        if (SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL)
        {
            std::cout << "Function: "
                    << currentFunction.properties->functionDeclaration->get_scope()->get_qualified_name().getString() +
                    currentFunction.properties->functionDeclaration->get_mangled_name().getString()
                    << " has declaration " << currentFunction.isDefined() << "\n";
        }
        
        graphNodes[currentFunction.properties->functionDeclaration] = graphNode;

        ROSE_ASSERT(currentFunction.properties->functionType);
        returnGraph->addNode(graphNode);
    }



    if (SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL)
        std::cout << "NodeList size: " << graphNodes.size() << "\n";

    //We have all the nodes. Now instantiate all the graph edges
    int totEdges = 0;
    BOOST_FOREACH(FunctionData& currentFunction, callGraphData)
    {
        ROSE_ASSERT(currentFunction.properties->functionDeclaration != NULL);
        
        boost::unordered_map<SgFunctionDeclaration*, SgGraphNode*>::iterator iter
                = graphNodes.find(currentFunction.properties->functionDeclaration);
        ROSE_ASSERT(iter != graphNodes.end());
        SgGraphNode* startingNode = iter->second;

        Rose_STL_Container<Properties*> & functionCallees = currentFunction.functionList;

        BOOST_FOREACH(Properties* callee, functionCallees)
        {
            ROSE_ASSERT(callee->functionType);

            // if we have a pointer (no function declaration) or a virtual function, create dummy node
            if (!(callee->functionDeclaration))
            {
                ROSE_ASSERT(false); //I'm not sure if this case ever happens
                SgGraphNode *dummy;
                dummy = new SgGraphNode("DUMMY");

                Properties* newProp = new Properties(callee);

                dummy->set_SgNode(newProp->functionDeclaration);
                dummy->addNewAttribute("Properties", newProp);
                if (callee->functionDeclaration && callee->functionDeclaration->get_definingDeclaration())
                    newProp->hasDef = true;
                else
                    newProp->hasDef = false;

                returnGraph->addNode(dummy);
                returnGraph->addDirectedEdge(startingNode, dummy);
            }
            else
            {
                //This function has been filtered out
                if (pred(callee->functionDeclaration) == false)
                {
                    continue;
                }

                iter = graphNodes.find(callee->functionDeclaration);
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
                    std::cout << "\tEndNode " << callee->functionDeclaration->get_name().str()
                            << "\t" << callee->hasDef << "\n";
                }
            }
            totEdges++;
        }
    }

    //Now, we must clean up all the Properties* objects that we didn't use
    BOOST_FOREACH(FunctionData& currentFunction, callGraphData)
    {
        BOOST_FOREACH(Properties* calleeProperty, currentFunction.functionList)
        {
            delete calleeProperty;
        }
    }
    
    if (SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL)
        std::cout << "Total number of edges: " << totEdges << "\n";

    graph = returnGraph;
};

// endif for CALL_GRAPH_H
#endif

