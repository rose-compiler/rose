/********************************************************************
* Andreas Saebjoernsen 03/17/06 
* Demonstration on how to iterate over a graph constucted by an
* VirtualGraphCreateTemplate. This type of graph is returned by for
* instance the call graph analysis, data flow analysis and all
* the other program analysis work within ROSE.
********************************************************************/

// Example ROSE Preprocessor
// used for testing ROSE infrastructure
#include "rose.h"
#include <assert.h>
#include <string>

#include <iomanip>
#include "AstConsistencyTests.h"
#include <functional>


#include <AstInterface.h>
#include <GraphDotOutput.h>
#include <VirtualGraphCreate.h>


using namespace std;

class Vertex : public MultiGraphElem
   {
     public:
       std::string name;
       Vertex( std::string n ) : MultiGraphElem( NULL ), name( n ) {}
       virtual std::string toString() const { return name; }
   };

class EdgeImpl : public MultiGraphElem
   {
   public:
         std::string label;
                                                                                                                                                              
         EdgeImpl ( std::string label = "default edge" ) : MultiGraphElem( NULL ), label( label ) {};
                                                                                                                                                              
         void Dump() const { printf ("EDGE: label = %s \n",label.c_str()); }
         virtual std::string toString() const { return label;}
                                                                                                                                                              
   };


template <class Node, class Edge>
class GraphBuilder : public VirtualGraphCreateTemplate<Node, Edge>
   {
     public:
          typedef Node NodeType;
          void addNode ( Node* node );
          void addEdge ( Node *src, Node *snk, Edge* edge );
                                                                                                                                                              
          GraphBuilder () : VirtualGraphCreateTemplate<Node, Edge> (NULL) {}
         ~GraphBuilder() { printf ("Inside of ~GraphBuilder() \n"); }
   };

template <class Node, class Edge> void
GraphBuilder<Node, Edge>::addNode ( Node* node )
   { VirtualGraphCreateTemplate<Node, Edge>::AddNode ( node ); }
                                                                                                                                                              
template <class Node, class Edge> void
GraphBuilder<Node, Edge>::addEdge ( Node *src, Node *snk, Edge* edge )
   { VirtualGraphCreateTemplate<Node, Edge>::AddEdge ( src, snk, edge ); }

typedef GraphBuilder<Vertex, EdgeImpl> GraphBears;


/***********************************************************************************************
 * The function
 *     void TranslateGraph(_GraphStructure& graph, _NodePredicate& _nodePred, _EdgePredicate& _edgePred);
 * performas the action _nodePred to every node and _edgePred to every edge in the graph 'graph'.
 * This work is general and should work on any graph constructed from VirtualGraphCreateTemplate which
 * is the graph all the program analysis work returns.
 ***********************************************************************************************/ 
template<typename _GraphStructure, typename _NodePredicate,typename _EdgePredicate>
void translateGraph(_GraphStructure& graph, _NodePredicate _nodePred, _EdgePredicate _edgePred){
     typename _GraphStructure::NodeIterator it1 =  graph. GetNodeIterator();
     while(!it1.ReachEnd()){
      typename _GraphStructure::Node* x1 = *it1; 
      ROSE_ASSERT(x1!=NULL);
      _nodePred(x1);

      it1++;
      typename _GraphStructure::EdgeIterator it2 =  graph. GetNodeEdgeIterator(x1,GraphAccess::EdgeOut );
      
      while(!it2.ReachEnd()){
         typename _GraphStructure::Edge* y1 = *it2;
         ROSE_ASSERT(y1!=NULL);
         _edgePred(y1);         
         it2++;
      }  
     }
};


struct testPredNodes : public unary_function<Vertex*,void>{
   public:
     void operator()(Vertex* test){
      // std::cout << "Found a Node" << std::endl;
     }
};

struct testPredEdges : public unary_function<EdgeImpl*,void>{
   public:
     void operator()(EdgeImpl* test){
      // std::cout << "Found an Edge" << std::endl;
     }
};

int
main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // SgProject* project = new SgProject(argc, argv); //test.getSgProject();

     GraphBears graph;

     Vertex* goldiLocks = new Vertex("Goldilocks");
     graph.addNode(goldiLocks); 
 
     Vertex* grandPa    = new Vertex("GrandPa");
     graph.addNode(grandPa);

     Vertex* momBear    = new Vertex("Middling Sized Bears");
     graph.addNode(momBear);
     graph.addEdge(grandPa,momBear, new EdgeImpl());

     Vertex* fatherBear = new Vertex("Great Big Bear");
     graph.addNode(fatherBear);

     Vertex* littleBear = new Vertex("Teeny Tiny Bear");
     graph.addNode(littleBear);
     graph.addEdge(momBear,littleBear, new EdgeImpl());
     graph.addEdge(fatherBear,littleBear, new EdgeImpl());

     //Call function to iterate over the graph
     translateGraph(graph,testPredNodes(),testPredEdges());   

     GraphDotOutput<GraphBears> output(graph);
     output.writeToDOTFile("graphTraversal.dot", "GraphBears");

     return 0;
   }
