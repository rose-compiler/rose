
#include <general.h>
#include <TransDepGraph.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/8/2006): Since this is not used in a header file it is OK here!
#define Boolean int

template <class Node> 
class TransDepAnalImpl  : public TransInfoOP<DepInfoSet>
{
 public:
  DepInfoSet GetTopInfo( const GraphNode *n1, const GraphNode *n2)
  { return DepInfoSetGenerator::GetTopInfoSet(); }
  DepInfoSet GetBottomInfo( const GraphNode *n1, const GraphNode *n2)
    {
       int nr = static_cast<const Node*>(n1)->NumOfLoops();
       int nc = static_cast<const Node*>(n2)->NumOfLoops();
       return DepInfoSetGenerator::GetBottomInfoSet(nr, nc, 1);
    }
  DepInfoSet GetIDTransInfo( const GraphNode *n)
  {
     int nr = static_cast<const Node*>(n)->NumOfLoops();
     return DepInfoSetGenerator::GetIDInfoSet(nr);
  }
     
  DepInfoSet  GetTransInfo( const GraphEdge *e)
    {
      const DepInfoEdge *edge = static_cast<const DepInfoEdge*>(e);
      DepInfoSet result(DepInfoSetGenerator::GetTopInfoSet());
      result.AddDep(edge->GetInfo());
      return result;
    }
  void UnionWith(DepInfoSet& e1, DepInfoSet e2)
    { e1 |= e2; }
  DepInfoSet  Composite( DepInfoSet e1, DepInfoSet e2)
    { return e1 * e2; }
  DepInfoSet  Closure( DepInfoSet e)
    { return ::Closure(e); }
};

template <class Node>
Boolean TransDepGraphCreate<Node>::
TransInfoComputed(const GraphNode *src, const GraphNode *snk)
   { 
      return GraphGetCrossEdge<GraphAccess>()(this, src, snk) != 0;
   }

template <class Node>
DepInfoSet TransDepGraphCreate<Node>::
GetTransInfo( const GraphNode *src, const GraphNode *snk)
   {
     DepInfoSetEdge *edge = static_cast<DepInfoSetEdge*>
              (GraphGetCrossEdge<GraphAccess>()(this, src, snk)); 
      return edge->GetInfo();
   } 

template <class Node>
void TransDepGraphCreate<Node>::
SetTransInfo( GraphNode *src1, GraphNode *snk1, DepInfoSet info) 
   {
      Node* src = static_cast<Node*>(src1), *snk =  static_cast<Node*>(snk1);
     DepInfoSetEdge *edge = static_cast<DepInfoSetEdge*>
              (GraphGetCrossEdge<GraphAccess>()(this, src, snk)); 
      if (edge == 0) {
         edge = new DepInfoSetEdge(this, info);
         CreateBaseEdge(src, snk, edge);
      }
      else 
         edge->GetInfo() = info;
    }


template <class Node>
DepInfoSetEdge * TransDepGraphCreate<Node> :: 
GetTransDep( Node *src, Node *snk)
{
  DepInfoSetEdge *edge = static_cast<DepInfoSetEdge*>
              (GraphGetCrossEdge<GraphAccess>()(this, src, snk)); 
  if (edge == 0 && anal.ComputeTransInfo(this, src,snk)) {
      edge = static_cast<DepInfoSetEdge*>
              (GraphGetCrossEdge<GraphAccess>()(this, src, snk)); 
      assert(edge != 0);
  }
  return edge;
}

template <class Node> TransDepGraphCreate<Node> ::  
TransDepGraphCreate(GraphAccessTemplate<Node,DepInfoEdge> *graph,
                    int splitlimit, BaseGraphCreate *bc)
 : IDGraphCreateTemplate<Node, DepInfoSetEdge>(bc),
   anal(graph, analop=new TransDepAnalImpl<Node>(), splitlimit) 
{
  CloneNodeSet(graph->GetNodeIterator());
}

template <class Node> TransDepGraphCreate<Node> :: ~TransDepGraphCreate()
{
  delete analop;
}

#define TEMPLATE_ONLY
#include <TransAnalysis.C>
#include <vector>
#ifndef TRANSDEPGRAPH_TEMPLATE_ONLY
template class TransInfoGraph<DepInfoSet>;
template class vector <TransAnalSCCGraphNode<DepInfoSet>::TwinNode>;
#endif

// DQ (1/7/2006): Does not compile using g++ 4.0.2
// template TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct * __uninitialized_copy_aux<TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct *, TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct *>(TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct *, TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct *, TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct *, __false_type);
// template TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct * __uninitialized_fill_n_aux<TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct *, unsigned int, TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct>(TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct *, unsigned int, TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct const &, __false_type);
// template TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct * __uninitialized_copy_aux<TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct const *, TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct *>(TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct const *, TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct const *, TransAnalSCCGraphNode<DepInfoSet>::TwinNodeStruct *, __false_type);

