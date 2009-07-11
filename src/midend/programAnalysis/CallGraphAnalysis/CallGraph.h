
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

#include <sstream>
#include <iostream>
#include <string>
#include <functional>
#include <queue>

#ifdef HAVE_SQLITE3
#include "sqlite3x.h"
#endif

namespace OutputDot
{
  std::string Translate( std::string r1);
  //! get the name of a vertex
  std::string getVertexName(SgGraphNode* v);

  std::string getEdgeLabel(SgDirectedGraphEdge* e);

  void
  writeToDOTFile(SgIncidenceDirectedGraph* graph,  const std::string& filename, const std::string& graphname, 
     std::string (*getNLabel)(SgGraphNode*) = getVertexName, std::string (*getELabel)(SgDirectedGraphEdge*) = getEdgeLabel );

};

//extern bool var_SOLVE_FUNCTION_CALLS_IN_DB;


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

#include "ClassHierarchyGraph.h"


//AS(090707) Added the CallTargetSet namespace to replace the CallGraphFunctionSolver class
namespace CallTargetSet
{
	typedef Rose_STL_Container<SgFunctionDeclaration *> SgFunctionDeclarationPtrList;
	typedef Rose_STL_Container<SgClassDefinition *> SgClassDefinitionPtrList;
	// returns the list of declarations of all functions that may get called via the specified pointer
	SgFunctionDeclarationPtrList solveFunctionPointerCall ( SgPointerDerefExp *, SgProject * );

	// returns the list of declarations of all functions that may get called via a member function pointer
	SgFunctionDeclarationPtrList solveMemberFunctionPointerCall ( SgExpression *,ClassHierarchyWrapper * );

	// returns the list of declarations of all functions that may get called via a
	// member function (non/polymorphic) call
	SgFunctionDeclarationPtrList solveMemberFunctionCall ( SgClassType *, ClassHierarchyWrapper *,		SgMemberFunctionDeclaration *, bool );
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
          Rose_STL_Container<Properties *> functionListDB;
     //@}
    //@{
    //Interfaces only defined when DB is NOT defined
     public:
          FunctionData ( SgFunctionDeclaration* functionDeclaration, bool hasDef,
			 SgProject *project, ClassHierarchyWrapper * );
	  
	  // Relevant data for call graph
          SgFunctionDeclaration* functionDeclaration;
          Rose_STL_Container<SgFunctionDeclaration*> functionList;
	  Rose_STL_Container<SgMemberFunctionDeclaration*> *findPointsToVirtualFunctions ( SgMemberFunctionDeclaration * );
	  bool compareFunctionDeclarations( SgFunctionDeclaration *f1, SgFunctionDeclaration *f2 );
   //@}
   };


struct dummyFilter : public std::unary_function<bool,SgFunctionDeclaration*>
   {
     bool operator() (SgFunctionDeclaration* node) const;
  }; 

class GetOneFuncDeclarationPerFunction :  public std::unary_function<SgNode*, Rose_STL_Container<SgNode*> >
{
  public:
    result_type operator()(SgNode* node );
};



class CallGraphBuilder
   {
     public:
       CallGraphBuilder( SgProject *proj, bool solveInDb  );
       
       void buildCallGraph();

       template<typename Predicate>
       void buildCallGraph(Predicate pred);

       SgIncidenceDirectedGraph *getGraph(); 
       //void classifyCallGraph();
     private:
       SgProject *project;
       SgIncidenceDirectedGraph *graph;
       bool var_SOLVE_FUNCTION_CALLS_IN_DB;
   };

void GenerateDotGraph ( SgIncidenceDirectedGraph *graph, std::string fileName );


SgGraphNode* 
findNode ( Rose_STL_Container<SgGraphNode*> & nodeList, SgFunctionDeclaration* functionDeclaration, bool var_SOLVE_FUNCTION_CALLS_IN_DB );

SgGraphNode* 
findNode ( Rose_STL_Container<SgGraphNode*> & nodeList, Properties* functionProperties );

SgGraphNode* 
findNode ( Rose_STL_Container<SgGraphNode*> & nodeList, std::string name );

SgGraphNode* 
findNode ( Rose_STL_Container<SgGraphNode*> & nodeList, std::string name, int );

//Iterate over all edges in graph until an edge from->to is found. If not such edge
//exsists return NULL
SgGraphEdge*
findEdge (SgIncidenceDirectedGraph* graph, SgGraphNode* from, SgGraphNode* to);


SgGraphNode* findNode(SgGraph* graph, std::string nid);

#ifdef HAVE_SQLITE3
sqlite3x::sqlite3_connection* open_db(std::string gDB  );
void createSchema ( sqlite3x::sqlite3_connection& gDB, std::string dbName );
//Will remove all function and their falls declared in a file from paths that are not in the list
void filterNodesKeepPaths( sqlite3x::sqlite3_connection& gDB, std::vector<std::string> keepDirs );
//Will remove all references to function in the function list
void filterNodesByFunctionName( sqlite3x::sqlite3_connection& gDB, std::vector<std::string> removeFunctions );
void filterNodesByFilename (  sqlite3x::sqlite3_connection& gDB, std::string filterFile );
void filterNodesByFunction (  sqlite3x::sqlite3_connection& gDB, SgFunctionDeclaration *function );
void filterNodesByDirectory (  sqlite3x::sqlite3_connection& gDB, std::string directory );

//Will load all graphs represented in the database into one graph
SgIncidenceDirectedGraph* loadCallGraphFromDB (sqlite3x::sqlite3_connection& gDB);
void writeSubgraphToDB ( sqlite3x::sqlite3_connection& gDB,SgIncidenceDirectedGraph* callGraph );
void solveFunctionPointers (  sqlite3x::sqlite3_connection& gDB);
void solveVirtualFunctions (  sqlite3x::sqlite3_connection& gDB, std::string dbHierarchy );

#endif


template<typename Predicate>
void
CallGraphBuilder::buildCallGraph (Predicate pred)
{
  if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
  {
  Rose_STL_Container<FunctionData *> callGraphData;

  //AS (09/23/06) Query the memory pool instead of subtree of project
  VariantVector vv( V_SgFunctionDeclaration );
  GetOneFuncDeclarationPerFunction defFunc;
  Rose_STL_Container<SgNode *> functionList = NodeQuery::queryMemoryPool(defFunc, &vv );


  //   list<SgNode *> functionList = NodeQuery::querySubTree ( project, V_SgFunctionDeclaration );

  ClassHierarchyWrapper classHierarchy( project );
  Rose_STL_Container<SgNode *>::iterator i = functionList.begin();

  printf ("Inside of buildCallGraph functionList.size() = %zu \n",functionList.size());

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
      callGraphData.push_back( functionData );
    i++;
  }

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

    std::cout << "Function: "
      << (*j)->properties->functionDeclaration->get_scope()->get_qualified_name().getString() +
      (*j)->properties->functionDeclaration->get_mangled_name().getString()
      << " has declaration " << (*j)->isDefined() << "\n";
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
  std::cout << "NodeList size: " << nodeList.size() << "\n";
  int totEdges = 0;
  while (j != callGraphData.end())
  {

    //                printf ("Calling findNode in outer loop (*j)->functionDeclaration->get_name() = %s \n",(*j)->functionDeclaration->get_name().str());
    ROSE_ASSERT( (*j)->properties->functionDeclaration != NULL );
    SgGraphNode* startingNode = findNode( nodeList, (*j)->properties->functionDeclaration, var_SOLVE_FUNCTION_CALLS_IN_DB );
    ROSE_ASSERT (startingNode != NULL);

    Rose_STL_Container<Properties *> & functionList = (*j)->functionListDB;
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

      if ( var_SOLVE_FUNCTION_CALLS_IN_DB == true  && (  !( (*k)->functionDeclaration ) || (*k)->isPolymorphic ) )
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


        std::cout << "DUMMY " << std::endl;
        returnGraph->addNode( dummy );
        returnGraph->addDirectedEdge( startingNode, dummy, " " );
      }
      else
      {
        SgGraphNode *endNode = findNode( nodeList, ( *k )->functionDeclaration, var_SOLVE_FUNCTION_CALLS_IN_DB );
        ROSE_ASSERT ( endNode );
        if(findEdge(returnGraph,startingNode,endNode)==NULL)
        {
          std::cout << "Added edge" << std::endl;

          ROSE_ASSERT(startingNode != NULL && endNode != NULL);
          returnGraph->addDirectedEdge( startingNode, endNode, " " );
        }else 
          std::cout << "Did not add edge since it already exist" << std::endl;
        std::cout << "\tEndNode "
          << (*k)->functionDeclaration->get_name().str() << "\t" << (*k)->hasDef << "\n";
      }
      totEdges++;
      k++;
    }
       j++;
  }

  std::cout << "Total number of edges: " << totEdges << "\n";
  // printf ("Return graph \n");

  graph = returnGraph;


    }else{

    Rose_STL_Container<FunctionData*> callGraphData;

    // list<SgNode*> functionList = NodeQuery::querySubTree ( project, V_SgFunctionDeclaration );
    //AS (09/23/06) Query the memory pool instead of subtree of project
    VariantVector vv(V_SgFunctionDeclaration);
    GetOneFuncDeclarationPerFunction defFunc;
    Rose_STL_Container<SgNode *> functionList = NodeQuery::queryMemoryPool(defFunc, &vv );


    ClassHierarchyWrapper classHierarchy( project );
    Rose_STL_Container<SgNode*>::iterator i = functionList.begin();

    printf ("Inside of buildCallGraph functionList.size() = %zu \n",functionList.size());

    while (i != functionList.end())
    {
      SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
      ROSE_ASSERT (functionDeclaration != NULL);
      bool hasDef = false;

      if ( functionDeclaration->get_definition() != NULL )
      {
        // printf ("Insert function declaration containing function definition \n");
        hasDef = true;
      }

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

      FunctionData* functionData = new FunctionData( functionDeclaration, hasDef, project, &classHierarchy );
//      *i = functionDeclaration;


      //AS(032806) Filter out functions baced on criteria in predicate
      if(pred(functionDeclaration)==true) 
        callGraphData.push_back( functionData );
      i++;
    }

    // Build the graph
    SgIncidenceDirectedGraph *returnGraph = new SgIncidenceDirectedGraph();
    ROSE_ASSERT (returnGraph != NULL);

    Rose_STL_Container<FunctionData*>::iterator j = callGraphData.begin();

    // printf ("Build the node list callGraphData.size() = %zu \n",callGraphData.size());

    Rose_STL_Container<SgGraphNode*> nodeList;
    while (j != callGraphData.end())
    {
      std::string functionName = (*j)->functionDeclaration->get_name().str();

      // Generate a unique name to test against later
      SgFunctionDeclaration* id = (*j)->functionDeclaration;

      SgDeclarationStatement *nonDefDeclInClass =
        isSgMemberFunctionDeclaration( id->get_firstNondefiningDeclaration() );
      if ( nonDefDeclInClass )
        ROSE_ASSERT ( id == nonDefDeclInClass );

      SgGraphNode* node = new SgGraphNode( functionName);
      ROSE_ASSERT((*j) != NULL );
      ROSE_ASSERT((*j)->properties != NULL );
      ROSE_ASSERT((*j)->properties->functionDeclaration != NULL );

      node->set_SgNode((*j)->properties->functionDeclaration);

      node->addNewAttribute("Properties",(*j)->properties );

      nodeList.push_back(node);
      // show graph

      {
         std::cout << "Function " << functionName << " " << id << " has pointers to:\n";
         Rose_STL_Container<SgFunctionDeclaration *> &fL1 = (*j)->functionList;
         Rose_STL_Container<SgFunctionDeclaration*>::iterator kItr = fL1.begin();
         while (kItr != fL1.end())
         {
         std::cout << "\tfunction: " << ( *kItr )->get_name().str() << ( *kItr ) << "\n";
         kItr++;
         }
      }

        returnGraph->addNode(node);
      j++;
    }

    j = callGraphData.begin();
    // std::cout << "NodeList size: " << nodeList.size() << "\n";
    int totEdges = 0;
    while (j != callGraphData.end())
    {
//       printf ("Calling findNode in outer loop (*j)->functionDeclaration->get_name() = %s \n",(*j)->functionDeclaration->get_name().str());

      SgGraphNode* startingNode = findNode( nodeList, (*j)->properties->functionDeclaration, var_SOLVE_FUNCTION_CALLS_IN_DB );

      ROSE_ASSERT (startingNode != NULL);

      Rose_STL_Container<SgFunctionDeclaration*> & functionList = (*j)->functionList;
      Rose_STL_Container<SgFunctionDeclaration*>::iterator k = functionList.begin();

       printf ("Now iterate over the list (size = %d) for function %s \n",
       (int)functionList.size(),(*j)->functionDeclaration->get_name().str());
      while (k != functionList.end())
      {
        SgGraphNode   *endingNode = findNode( nodeList, *k, var_SOLVE_FUNCTION_CALLS_IN_DB );

        /*
           if ( !endingNode )
           endingNode = findNode( nodeList,
           ( *k )->get_qualified_name().getString() +
           ( *k )->get_mangled_name().getString(), 1 );
         */
        if ( endingNode )
        {
          ROSE_ASSERT (endingNode != NULL);

            if(findEdge(returnGraph,startingNode,endingNode)==NULL)

            returnGraph->addDirectedEdge(startingNode,endingNode, isSgFunctionDeclaration(endingNode->get_SgNode())->get_name().getString());
          totEdges++;
        }
        else
        {
          std::cout << "COULDN'T FIND: " << ( *k )->get_qualified_name().str() << "\n";
          //isSgFunctionDeclaration( *k )->get_file_info()->display("AVOIDED CALL");
        }
        k++;
      }
      j++;
    }

    std::cout << "Total number of edges: " << totEdges << "\n";
    // printf ("Return graph \n");

    graph = returnGraph;
  }
};

// endif for CALL_GRAPH_H
#endif

