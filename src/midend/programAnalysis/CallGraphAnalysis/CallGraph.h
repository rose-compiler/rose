
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

extern bool var_SOLVE_FUNCTION_CALLS_IN_DB;

//Only used when SOLVE_FUNCTION_CALLS_IN_DB is defined
struct Properties
{
  bool isPointer, isPolymorphic;
  SgClassDefinition *invokedClass;
  SgFunctionDeclaration *functionDeclaration;
  SgType *functionType;

  Properties();
};

typedef struct Properties FunctionProperties;

typedef Rose_STL_Container<SgFunctionDeclaration *> SgFunctionDeclarationPtrList;
typedef Rose_STL_Container<SgClassDefinition *> SgClassDefinitionPtrList;

// DQ (1/31/2006): Changed name and made global function type symbol table a static data member.
// extern SgFunctionTypeTable Sgfunc_type_table;

class CallGraphNode: public MultiGraphElem
   {
     private:
       bool hasDefinition;

     public:
       std::string label;

       virtual std::string toString() const;
       bool isDefined ();
       void Dump() const;

   public:

       //@{
      //Interfaces only used when using DB
      // SgFunctionDeclaration* functionDeclaration;

         FunctionProperties *properties;

         CallGraphNode ( std::string label, SgFunctionDeclaration* fctDeclaration, SgType *ty,
         bool hasDef, bool isPtr, bool isPoly, SgClassDefinition *invokedCls );
         CallGraphNode ( std::string label, FunctionProperties *fctProps, bool hasDef );
         //@}
         //@{
      //Interfaces only used when not using database
         SgFunctionDeclaration* functionDeclaration;
         CallGraphNode ( std::string label, SgFunctionDeclaration* fctDeclaration, bool hasDef );
         //@}
   };

class CallGraphEdge : public MultiGraphElem
   {
     public:
       //Properties variable only used when DB is defined
	 FunctionProperties *properties;

         std::string label;
         CallGraphEdge ( std::string label = "default edge" );
         void Dump() const;
         virtual std::string toString() const;
   };


typedef CallGraphEdge ClassHierarchyEdge;


template <class Node, class Edge>
// DQ (9/4/2005): Compiler does not know what IDGraphCreateTemp is!
// class DAGCreate : public IDGraphCreateTemp<Node, Edge>
class DAGCreate : public VirtualGraphCreateTemplate<Node, Edge>
   {
     private:
       // Map subgraph names to id values used in DOT interface
          std::map<std::string,int> subgraphMap;

     public:
       // Map id values to subgraph names
          std::map<int,std::string> subGraphNames;

          void addNode ( Node* node );
          void addEdge ( Node *src, Node *snk, Edge* edge );
          bool edgeExist ( Node *src, Node *snk);
          void DeleteNode(Node* n);

          int size();

       // DQ (9/4/2005): These is no IDGraphCreateTemp (I think it is now called IDGraphCreateTemplate)
       // DAGCreate () : IDGraphCreateTemp<Node, Edge> (NULL) {}
          DAGCreate ();

         ~DAGCreate(); 

          std::map<std::string,int> & getSubGraphMap();
         
   };

#include "ClassHierarchyGraph.h"

template <class Node, class Edge> 
DAGCreate<Node, Edge>::DAGCreate () : VirtualGraphCreateTemplate<Node, Edge> (NULL) 
  {}

template <class Node, class Edge> 
void
DAGCreate<Node, Edge>::DeleteNode(Node* n) 
  {
   VirtualGraphCreateTemplate<Node, Edge>::DeleteNode(n);
  }

template <class Node, class Edge>
DAGCreate<Node, Edge>::~DAGCreate() 
   {
  // printf ("Inside of ~CallGraphCreate() \n"); 
   }

template <class Node, class Edge> void
DAGCreate<Node, Edge>::addNode ( Node* node )
   { VirtualGraphCreateTemplate<Node,Edge>::AddNode ( node ); }

template <class Node, class Edge> void
DAGCreate<Node, Edge>::addEdge ( Node *src, Node *snk, Edge* edge )
   { VirtualGraphCreateTemplate<Node,Edge>::AddEdge ( src, snk, edge ); }


template <class Node, class Edge> bool
DAGCreate<Node, Edge>::edgeExist ( Node *src, Node *snk)
   { 
     bool edge_exist = false;     
     for (typename DAGCreate<Node, Edge>::EdgeIterator i = this->GetNodeEdgeIterator(src,GraphAccess::EdgeOut); !i.ReachEnd(); ++i) 
        {
	      Edge* currentEdge = i.Current();
              Node* endPoint= GetEdgeEndPoint(currentEdge, GraphAccess::EdgeIn);
	      
	      if(snk == endPoint)
            {
           // std::cout << "NODE EXISTS" << std::endl;
              edge_exist = true;
		        break;
	         }
           else
            {
           // std::cout << "NODE DO NOT EXIST" << std::endl;
            }
        }

     
     
     return edge_exist; 
   }


template <class Node, class Edge> int
DAGCreate<Node, Edge>::size()
   {
     int numberOfNodes = 0;
     for (typename DAGCreate<Node, Edge>::NodeIterator i = this->GetNodeIterator(); !i.ReachEnd(); ++i) 
        {
          numberOfNodes++;
        }

     return numberOfNodes;
   }

template <class Node, class Edge> std::map<std::string, int> & 
DAGCreate<Node, Edge>::getSubGraphMap()
   {
     return subgraphMap;
   }


typedef DAGCreate<CallGraphNode, CallGraphEdge> CallGraphCreate;


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
	  FunctionProperties *properties;
	  
	  // Relevant data for call graph
          //SgFunctionDeclaration* functionDeclaration;
          Rose_STL_Container<FunctionProperties *> functionListDB;
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

class CallGraphBuilder
   {
     public:
       CallGraphBuilder( SgProject *proj );
       
       void buildCallGraph();

       template<typename Predicate>
       void buildCallGraph(Predicate pred);

       CallGraphCreate *getGraph(); 
       void classifyCallGraph();
     private:
       SgProject *project;
       CallGraphCreate *graph;
   };


void GenerateDotGraph ( CallGraphCreate *graph, std::string fileName );


CallGraphNode* 
findNode ( Rose_STL_Container<CallGraphNode*> & nodeList, SgFunctionDeclaration* functionDeclaration );

CallGraphNode* 
findNode ( Rose_STL_Container<CallGraphNode*> & nodeList, FunctionProperties* functionProperties );

CallGraphNode* 
findNode ( Rose_STL_Container<CallGraphNode*> & nodeList, std::string name );

CallGraphNode* 
findNode ( Rose_STL_Container<CallGraphNode*> & nodeList, std::string name, int );

class CallGraphDotOutput : public GraphDotOutput <CallGraphCreate>
   {
  // Keep a reference to the current graph
     CallGraphCreate &callGraph;

     public:
    // CallGraphDotOutput ( GraphAccess & graph ) : callGraph(graph), GraphDotOutput(graph) {}

    // DQ (9/4/2005): Swapped order to avoid compiler waring (and reflect proper order of initialization)
    // CallGraphDotOutput( CallGraphCreate & graph ) : callGraph(graph), GraphDotOutput(graph) {}
       CallGraphDotOutput( CallGraphCreate & graph ); 
       virtual ~CallGraphDotOutput() {}

       virtual int getVertexSubgraphId ( GraphNode & v );

// DQ (7/28/2005): Don't include the data base
#ifdef HAVE_SQLITE3

       int writeToDB ( int i = 0, std::string dbName = "" );
       void writeSubgraphToDB ( sqlite3x::sqlite3_connection& gDB );
       CallGraphCreate *loadGraphFromDB ( std::string dbName );
       int GetCurrentMaxSubgraph ( sqlite3x::sqlite3_connection& gDB );
       void filterNodesByDB ( std::string dbName, std::string fiterDB = "__filter.db" );
       void filterNodesByFilename ( std::string dbName, std::string filterFile );
       void filterNodesByFunction ( std::string dbName, SgFunctionDeclaration *function );
       void filterNodesByDirectory ( std::string dbName, std::string directory );
       void solveFunctionPointers ( std::string dbName );
       void solveVirtualFunctions ( std::string dbName, std::string dbHierarchy );
     private:
       void createCallGraphSchema ( sqlite3x::sqlite3_connection& gDB, std::string dbName );

#endif
   };


// endif for CALL_GRAPH_H
#endif

