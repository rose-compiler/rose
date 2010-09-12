
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

#ifdef HAVE_SQLITE3
#include "sqlite3x.h"
#endif

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
  std::vector<Properties*> solveMemberFunctionCall ( SgClassType *, ClassHierarchyWrapper *,		SgMemberFunctionDeclaration *, bool );

  // returns the list of Properties of all constructors that may get called via 
  // an initialization.
  std::vector<Properties*> solveConstructorInitializer ( SgConstructorInitializer* sgCtorInit);

  // populates functionList with declarations of all functions that may get
  // called via an SgFunctionCallExp node.
  void getPropertiesForExpression(SgExpression* exp,
                                    ClassHierarchyWrapper* classHierarchy,
                                    Rose_STL_Container<Properties *>& functionList);
#if 0
  void getCallLikeExpsForFunctionDefinition(SgFunctionDefinition* def, Rose_STL_Container<SgExpression*>& calls);
  void getFunctionDefinitionsForCallLikeExp(SgExpression* exp, Rose_STL_Container<SgFunctionDefinition*>& defs);
#endif
};

class FunctionData
{
  public:

    bool hasDefinition;

    bool isDefined (); 

    //@{
    //Interfaces only used when DB is defined
  public:
    FunctionData ( SgFunctionDeclaration* functionDeclaration, SgProject *project,
        ClassHierarchyWrapper * );
    Properties *properties;

    // Relevant data for call graph
    //SgFunctionDeclaration* functionDeclaration;
    Rose_STL_Container<Properties *> functionList;
    //@}
    //@{
    //Interfaces only defined when DB is NOT defined
  public:
    // Relevant data for call graph
    SgFunctionDeclaration* functionDeclaration;
    //     Rose_STL_Container<SgFunctionDeclaration*> functionList;
    Rose_STL_Container<SgMemberFunctionDeclaration*> *findPointsToVirtualFunctions ( SgMemberFunctionDeclaration * );
    bool compareFunctionDeclarations( SgFunctionDeclaration *f1, SgFunctionDeclaration *f2 );
    //@}
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

//Iterate over all edges in graph until an edge from->to is found. If not such edge
//exists return NULL
SgGraphEdge*
findEdge (SgIncidenceDirectedGraph* graph, SgGraphNode* from, SgGraphNode* to);

SgGraphNode* findNode(SgGraph* graph, std::string nid);


//! Find a matching graph node based on a function declaration
SgGraphNode* 
findNode ( Rose_STL_Container<SgGraphNode*> & nodeList, SgFunctionDeclaration* functionDeclaration);

SgGraphNode* 
findNode ( Rose_STL_Container<SgGraphNode*> & nodeList, Properties* functionProperties );

SgGraphNode* 
findNode ( Rose_STL_Container<SgGraphNode*> & nodeList, std::string name );

SgGraphNode* 
findNode ( Rose_STL_Container<SgGraphNode*> & nodeList, std::string name, int );

#ifdef HAVE_SQLITE3
sqlite3x::sqlite3_connection* open_db(std::string gDB  );
void createSchema ( sqlite3x::sqlite3_connection& gDB, std::string dbName );

//Will load all graphs represented in the database into one graph
SgIncidenceDirectedGraph* loadCallGraphFromDB (sqlite3x::sqlite3_connection& gDB);
void writeSubgraphToDB ( sqlite3x::sqlite3_connection& gDB,SgIncidenceDirectedGraph* callGraph );
void solveFunctionPointers (  sqlite3x::sqlite3_connection& gDB);
void solveVirtualFunctions (  sqlite3x::sqlite3_connection& gDB, std::string dbHierarchy );

#endif

class GetOneFuncDeclarationPerFunction :  public std::unary_function<SgNode*, Rose_STL_Container<SgNode*> >
{
  public:
    result_type operator()(SgNode* node );
};

template<typename Predicate>
  void
CallGraphBuilder::buildCallGraph (Predicate pred)
{
  Rose_STL_Container<FunctionData *> callGraphData;

  //AS (09/23/06) Query the memory pool instead of subtree of project
  VariantVector vv( V_SgFunctionDeclaration );
  GetOneFuncDeclarationPerFunction defFunc;
  Rose_STL_Container<SgNode *> functionList = NodeQuery::queryMemoryPool(defFunc, &vv );


  //   list<SgNode *> functionList = NodeQuery::querySubTree ( project, V_SgFunctionDeclaration );

  ClassHierarchyWrapper classHierarchy( project );
  Rose_STL_Container<SgNode *>::iterator i = functionList.begin();

  // printf ("Inside of buildCallGraph functionList.size() = %zu \n",functionList.size());

  Rose_STL_Container<SgNode *> resultingFunctions;


  while ( i != functionList.end() )
  {
    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration( *i );
    ROSE_ASSERT ( functionDeclaration != NULL );

    // determining the in-class declaration
    if ( isSgMemberFunctionDeclaration( functionDeclaration ) )
    {
      // always saving the in-class declaration, so we need to find that one
      SgDeclarationStatement *nonDefDeclInClass =
        isSgMemberFunctionDeclaration( functionDeclaration->get_firstNondefiningDeclaration() );
      // functionDeclaration is outside the class (so it must have a definition)
      if ( nonDefDeclInClass )
        functionDeclaration = isSgMemberFunctionDeclaration( nonDefDeclInClass );
    }
    else
    {
      // we need to have only one declaration for regular functions as well
      SgFunctionDeclaration *nonDefDecl =
        isSgFunctionDeclaration( functionDeclaration->get_firstNondefiningDeclaration() );
      if ( nonDefDecl )
        functionDeclaration = nonDefDecl;
    }
    FunctionData* functionData = new FunctionData( functionDeclaration, project, &classHierarchy );
    //*i = functionDeclaration;

    ROSE_ASSERT(functionData->properties->functionDeclaration != NULL);
    //AS(032806) Filter out functions baced on criteria in predicate
    if(pred(functionDeclaration)==true)
    {
      resultingFunctions.push_back(*i);
      callGraphData.push_back( functionData );
    }
    i++;
  }

  functionList = resultingFunctions;
  // Build the graph
  SgIncidenceDirectedGraph *returnGraph = new SgIncidenceDirectedGraph();
  ROSE_ASSERT (returnGraph != NULL);

  Rose_STL_Container<FunctionData *>::iterator j = callGraphData.begin();

  printf ("Build the node list callGraphData.size() = %zu \n",callGraphData.size());

  Rose_STL_Container<SgGraphNode*> nodeList;
  while ( j != callGraphData.end() )
  {
    std::string functionName;
    ROSE_ASSERT ( (*j)->properties->functionDeclaration );
    functionName = (*j)->properties->functionDeclaration->get_mangled_name().getString();

    // Generate a unique name to test against later
    SgFunctionDeclaration* id = (*j)->properties->functionDeclaration;
    SgDeclarationStatement *nonDefDeclInClass =
      isSgMemberFunctionDeclaration( id->get_firstNondefiningDeclaration() );
    if ( nonDefDeclInClass )
      ROSE_ASSERT ( id == nonDefDeclInClass );
    SgGraphNode* node = new SgGraphNode( functionName);
    node->set_SgNode((*j)->properties->functionDeclaration);

    node->addNewAttribute("Properties",(*j)->properties );

    if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
    {
      std::cout << "Function: "
	<< (*j)->properties->functionDeclaration->get_scope()->get_qualified_name().getString() +
	(*j)->properties->functionDeclaration->get_mangled_name().getString()
	<< " has declaration " << (*j)->isDefined() << "\n";
    }
    nodeList.push_back( node );
    /*
    // show graph
    std::cout << "Function " << functionName << "   " << id << " has pointers to:\n";
    list <Properties *> &fL = (*j)->functionList;
    list<Properties *>::iterator k = fL.begin();
    while (k != fL.end())
    {
    std::cout << "\tfunction: " << *k << "\n";
    k++;
    }
     */
    ROSE_ASSERT ( (*j)->properties->functionType );
    returnGraph->addNode( node );
    j++;
  }

  j = callGraphData.begin();

  if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
    std::cout << "NodeList size: " << nodeList.size() << "\n";

  int totEdges = 0;
  while (j != callGraphData.end())
  {

    //                printf ("Calling findNode in outer loop (*j)->functionDeclaration->get_name() = %s \n",(*j)->functionDeclaration->get_name().str());
    ROSE_ASSERT( (*j)->properties->functionDeclaration != NULL );
    SgGraphNode* startingNode = findNode( nodeList, (*j)->properties->functionDeclaration);
    ROSE_ASSERT (startingNode != NULL);

    Rose_STL_Container<Properties *> & functionList = (*j)->functionList;
    Rose_STL_Container<Properties *>::iterator k = functionList.begin();

    while ( k != functionList.end() )
    {
      ROSE_ASSERT ( (*k)->functionType );
      std::string label = "POINTER";

#if 0
      CallGraphEdge* edge = new CallGraphEdge( " " );
      if ( (*k)->functionDeclaration )
        edge->label = (*k)->functionDeclaration->get_mangled_name().getString();
      ROSE_ASSERT ( edge != NULL );
      edge->properties = *k;
#endif
      // if we have a pointer (no function declaration) or a virtual function, create dummy node

      //  ROSE_ASSERT((*k)->functionDeclaration != NULL);

      if (   !( (*k)->functionDeclaration )/* || (*k)->isPolymorphic */ )
      {
        SgGraphNode *dummy;
        dummy = new SgGraphNode( "DUMMY" );

        Properties* newProp = new Properties(*k);

        dummy->set_SgNode(newProp->functionDeclaration);
        dummy->addNewAttribute("Properties",newProp );
        if ( (*k)->functionDeclaration && (*k)->functionDeclaration->get_definingDeclaration() )
          newProp->hasDef =  true;
        else
          newProp->hasDef =  false;


        returnGraph->addNode( dummy );
        returnGraph->addDirectedEdge( startingNode, dummy, " " );
      }
      else
      {

        //This function has been filtered out
        if(pred((*k)->functionDeclaration)==false)
        {
          k++;
          continue;
        }


        SgGraphNode *endNode = findNode( nodeList, ( *k )->functionDeclaration);

        ROSE_ASSERT ( endNode );
        if(findEdge(returnGraph,startingNode,endNode)==NULL)
        {
          ROSE_ASSERT(startingNode != NULL && endNode != NULL);
          returnGraph->addDirectedEdge( startingNode, endNode, " " );
	}else if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL ){
	  std::cout << "Did not add edge since it already exist" << std::endl;
	  std::cout << "\tEndNode " << (*k)->functionDeclaration->get_name().str() 
	    << "\t" << (*k)->hasDef << "\n";
	}
      }
      totEdges++;
      k++;
    }
    j++;
  }

  if( SgProject::get_verbose() >= DIAGNOSTICS_VERBOSE_LEVEL )
    std::cout << "Total number of edges: " << totEdges << "\n";
  // printf ("Return graph \n");

  graph = returnGraph;


};

// endif for CALL_GRAPH_H
#endif

