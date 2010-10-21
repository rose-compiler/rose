#ifndef AST_GRAPH_H
#define AST_GRAPH_H

#include "DOTRepresentation.h"
// AST Graph support developed by Andreas

// template <class T> class DOTRepresentation;

namespace AST_Graph {

//options for handling null pointers
enum pointerHandling
{
       graph_NULL,
       do_not_graph_NULL
};

//options for traversal types
enum traversalType 
{
       memory_pool_traversal,
       whole_graph_AST
};



//BEGIN MY_PAIR

/***********************************************************************************
 *  Define custom return type from the filter functionals. This is an experiment
 *  to see if a std::pair like data structure where the variables names are not
 *  std::pair<>::first and std::pair<>::second. This data structure is expected to
 *  change as the code develops. 
 ***********************************************************************************/

template <class _T1, class _T2, class _T3>
struct my_pair {
  typedef _T1 first_type;    ///<  @c first_type is the first bound type
  typedef _T2 second_type;   ///<  @c second_type is the second bound type
  typedef _T2 third_type;   ///<  @c second_type is the second bound type

  _T1 addToGraph;                 ///< @c first is a copy of the first object
  _T2 DOTOptions;                ///< @c second is a copy of the second object
  _T3 DOTLabel;
  
  /** The default constructor creates @c first and @c second using their
   *  respective default constructors.  */
  my_pair() : addToGraph(), DOTOptions() , DOTLabel(){}
  /** Two objects may be passed to a @c my_pair constructor to be copied.  */
  my_pair(const _T1& a, const _T2& b, const _T3& c) : addToGraph(a), DOTOptions(b), DOTLabel(c) {}

  /** There is also a templated copy ctor for the @c my_pair class itself.  */
  template <class _U1, class _U2, class _U3>
  my_pair(const my_pair<_U1, _U2,_U3>& p) : addToGraph(p.addToGraph), DOTOptions(p.DOTOptions), DOTLabel(p.label) {}
    };
/// Two my_pairs of the same type are equal iff their members are equal.
template <class _T1, class _T2, class _T3>
inline bool operator==(const my_pair<_T1, _T2,_T3>& x, const my_pair<_T1, _T2,_T3>& y)
{ 
  return x.addToGraph == y.addToGraph && x.DOTOptions == y.DOTOptions && x.DOTLabel == y.DOTLabel; 
}

/// <http://gcc.gnu.org/onlinedocs/libstdc++/20_util/howto.html#my_pairlt>
template <class _T1, class _T2, class _T3>
inline bool operator<(const my_pair<_T1, _T2,_T3>& x, const my_pair<_T1, _T2,_T3>& y)
{ 
  return x.addToGraph < y.addToGraph || 
         (!(y.addToGraph < x.addToGraph) && x.DOTOptions < y.DOTOptions); 
}

/// Uses @c operator== to find the result.
template <class _T1, class _T2, class _T3>
inline bool operator!=(const my_pair<_T1, _T2, _T3>& x, const my_pair<_T1, _T2,_T3>& y) {
  return !(x == y);
}

/// Uses @c operator< to find the result.
template <class _T1, class _T2, class _T3>
inline bool operator>(const my_pair<_T1, _T2, _T3>& x, const my_pair<_T1, _T2,_T3>& y) {
  return y < x;
}

/// Uses @c operator< to find the result.
template <class _T1, class _T2, class _T3>
inline bool operator<=(const my_pair<_T1, _T2, _T3>& x, const my_pair<_T1, _T2,_T3>& y) {
  return !(y < x);
}

/// Uses @c operator< to find the result.
template <class _T1, class _T2, class _T3>
inline bool operator>=(const my_pair<_T1, _T2, _T3>& x, const my_pair<_T1, _T2,_T3>& y) {
  return !(x < y);
}

//END MY_PAIR

// Functor implemtation to support filtering of the generated AST graphs

// Build a simpler type to simplify the code
typedef std::pair<SgNode*,std::string> NodeType;
#if 0
typedef struct{ 
          bool addToGraph;
          std::string DOTOptions;
         } FunctionalReturnType;
#endif
typedef my_pair<bool,std::string,std::string> FunctionalReturnType;

typedef std::vector<NodeType> NodeTypeVector;


/***************************************************************************************
 * The functional
 *   struct defaultFilterUnary
 * is an example filter on nodes.
 **************************************************************************************/
struct defaultFilterUnary: public std::unary_function<NodeType,FunctionalReturnType >
   {
  // This functor filters SgFileInfo objects and IR nodes from the GNU compatability file
     result_type operator() (argument_type x );
   };

/***************************************************************************************
 * The functional
 *   struct defaultFilterBinary
 * is an example filter on edges.
 **************************************************************************************/
struct defaultFilterBinary: public std::binary_function<SgNode*,NodeType,FunctionalReturnType >
   {
  // This functor filters SgFileInfo objects and IR nodes from the GNU compatability file
     result_type operator() ( first_argument_type x, second_argument_type y);
   };




// This functor is derived from the STL functor mechanism
struct nodePartOfGraph: public std::unary_function< std::pair< SgNode*, std::string>&,FunctionalReturnType >
   {
     result_type operator() ( argument_type x );
   };


// This functor is derived from the STL functor mechanism
struct filterSgFileInfo: public std::unary_function< std::pair< SgNode*, std::string>&,FunctionalReturnType >
   {
  // This functor filters SgFileInfo objects from being built in the generated graph
     result_type operator() ( argument_type x );
   };


// This functor is derived from the STL functor mechanism
struct filterSgFileInfoAndGnuCompatabilityNode: public std::unary_function< std::pair< SgNode*, std::string>&, FunctionalReturnType >
   {
  // This functor filters SgFileInfo objects and IR nodes from the GNU compatability file
     result_type operator() ( argument_type x );
   };


// DQ (2/23/2006): Andreas' new work
/*******************************************************************************************************************************
 * The class
 *   class DataMemberPointersToIR
 * implements two different ways of constructing a graph from the AST. 
 *     * The memory pool traversal which is why the class inherits from ROSE_VisitTraversal
 *     * Whole AST traversal through traversing on nodes
 * This design decision is peculiar out from a inheritance perspective, but makes sence to 
 * bridge between the iheritance style of Marcus and an STL algorithm style. Caveats:
 *     * one member functions is only used by the memory pool traversal
 *           -visit(..)
 *       The member function generateGraphFromMemoryPool(..) is required to achieve
 *       STL style algorithms where the bace-class uses implemenatation by virtual functions.
 * Great things:
 *     * avoid duplicate implementation
 *     * allows a very simple implementation
 ******************************************************************************************************************************/
template<typename Functional1, typename Functional2>
class DataMemberPointersToIR: public DOTRepresentation<SgNode*>, private ROSE_VisitTraversal
{
public:
     DataMemberPointersToIR(const Functional1& addNodeFunctional, const Functional2& addEdgeFunctional,traversalType tT, pointerHandling graphEmpty);
     virtual ~DataMemberPointersToIR(){};
   private:
    //Every node which is graphed is put into the NodeExists list to avoid graphing it twice.
     std::list<SgNode*> NodeExists;  


     //In order to use the memory pool traversal as an STL style algorithm these
     //two variables must be introduced. For the whole AST traversal they are NULL.
     //These are the functionals which impelemts the conditions on which nodes and edges
     //are filtered out
     Functional1 nodeFunctional;
     Functional2 edgeFunctional;
     //Define which traversal type is used
     traversalType whichTraversal;
    //Specify if NULL pointers should be represented in the graph
     pointerHandling graphNull;


   public:
     //Generates a graph from the AST using either the whole graph traversal or the
     //memory pool traversal. The 'depth' paramater is ONLY VALID FOR THE WHOLE AST 
     //TRAVERSAL. In the case of the memory pool traversal set (depth<0).
     //If the 'depth' paramater is
     //            * (depth<0) then recursively follow all pointers in 'graphNode',
     //             the nodes 'graphNode' points to etc.
     //            * (depth>0) then follow pointers just depth steps out from 'graphNode'
     void generateGraph(SgNode* graphNode, int depth);

   private:
     //Implementation of the virtual visit function for the memory pool traversal
     //ONLY VALID FOR THE MEMORY POOL TRAVERSAL.
     void visit ( SgNode* node);

};

/*********************************************************************************************
 *  The function 
 *     void AST_Graph::writeGraphOfMemoryPoolToFile(std::string filename, Functional1, 
 *                 Functional2, bool graphNullPointers);
 * will output a graph of the whole memory pool to a new file called 'filename'. The second argument
 * is a custom functional on the form
 *          unary_function<std::pair<SgNode*,std:string>,bool >
 * where the first template argument is a node and it's name while the second template argument is the return
 * type (see defaultFilterUnary for an example). 
 * The third argument is a custom functional to filter edges on the form
 *          binary_function<SgNode*,std::pair<SgNode*,std:string>,bool >
 * where the edge goes from the vertex in the first template argument to the vertex in the second template 
 * argument (see defaultFilterBinary for an example). 
 * 
 * If the third argument is true a node and an edge is made to any NULL pointer. If the third argument
 * is true the nodes and edges representing NULL pointers are filtered out (default).
 **********************************************************************************************/
template<typename Functional1, typename Functional2>
void writeGraphOfMemoryPoolToFile(std::string filename, AST_Graph::pointerHandling, Functional1, Functional2);



/*********************************************************************************************
 * The function
 *    void writeGraphOfMemoryPoolToFile(std::string filename, bool graphNullPointers = false);
 * will output a graph of the whole memory pool to a new file called 'filename'. This function
 * does exactly the same as the function 
 *    void AST_Graph::writeGraphOfMemoryPoolToFile(std::string filename, Functional1,
 *                 Functional2, bool graphNullPointers);
 * except there is no filters Filter1 and Filter2 on nodes and edges.
 **********************************************************************************************/
void writeGraphOfMemoryPoolToFile(std::string filename, AST_Graph::pointerHandling);

/*********************************************************************************************
 *  The function 
 *     void AST_Graph::writeGraphOfMemoryPoolToFile(std::string filename, Functional1, bool graphNullPointers);
 * will output a graph of the whole memory pool to a new file called 'filename'. This function
 * does exactly the same as the function 
 *    void AST_Graph::writeGraphOfMemoryPoolToFile(std::string filename, Functional1,
 *                 Functional2, bool graphNullPointers);
 * except there is no filter Filter2 so all edges are kept.
 **********************************************************************************************/

template<typename Functional1>
void writeGraphOfMemoryPoolToFile(std::string filename, AST_Graph::pointerHandling, Functional1);


/*************************************************************************************************
 * The functon
 *   void writeGraphOfAstSubGraphToFile(std::string filename, SgNode*, Functional1, Functional2, int, bool);
 * will output a graph of the subgraph of the AST SgNode in the second argument into a file 'filename'.
 * The third arguemtn is a custom functional on the form
 *          unary_function<std::pair<SgNode*,std:string>,bool >
 * where the first template argument is a node and it's name while the second template argument is the return
 * type (see defaultFilterUnary for an example).
 * The fourth argument is a custom functional to filter edges on the form
 *          binary_function<SgNode*,std::pair<SgNode*,std:string>,bool >
 * where the edge goes from the vertex in the first template argument to the vertex in the second template
 * argument (see defaultFilterBinary for an example).
 *
 * If the third argument is true a node and an edge is made to any NULL pointer. If the third argument
 * is true the nodes and edges representing NULL pointers are filtered out (default).
 **********************************************************************************************/
template<typename Functional1, typename Functional2>
void writeGraphOfAstSubGraphToFile(std::string filename, SgNode* node, AST_Graph::pointerHandling, Functional1 addNode, Functional2 addEdge, int depth = -1 );


/************************************************************************************************************
 * The function
 *     void writeGraphOfAstSubGraphToFile(std::string filename,SgNode*, Functional1, int depth, bool);
 * does the same as the function
 * void writeGraphOfAstSubGraphToFile(std::string,SgNode*, Functional1, Functional2, int, bool);
 * except there is no filter Functional2 to filter on edges.
 ************************************************************************************************************/
template<typename Functional1>
void writeGraphOfAstSubGraphToFile(std::string filename, SgNode* node, AST_Graph::pointerHandling, Functional1 addNode, int depth = -1);

/************************************************************************************************************
 * The function
 *     void writeGraphOfAstSubGraphToFile(std::string filename,SgNode*, int depth, bool);
 * does the same as the function
 * void writeGraphOfAstSubGraphToFile(std::string,SgNode*, Functional1, Functional2, int, bool);
 * except there is no filters Functional1 and Functional2 to filter on nodes and edges.
 ************************************************************************************************************/
void writeGraphOfAstSubGraphToFile(std::string filename, SgNode* node, AST_Graph::pointerHandling graphNullPointers,int depth = -1 );

} //End namespace AST_Graph

// DQ (2/22/2006): Include the template functions implementing the 
// generation of graphs of the AST (uses memory pools to see all AST IR nodes).
#include "astGraphTemplateImpl.h"

#endif
