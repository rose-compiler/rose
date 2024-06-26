#ifndef ROSE_TRANSDEPGRAPHIMPL_H
#define ROSE_TRANSDEPGRAPHIMPL_H

#include <TransDepGraph.h>


template <class Node> 
class TransDepAnalImpl  : public TransInfoOP<DepInfoSet>
{
 public:
  DepInfoSet GetTopInfo( const GraphAccessInterface::Node*, const GraphAccessInterface::Node*) {
    return DepInfoSetGenerator::GetTopInfoSet();
  }
  DepInfoSet GetBottomInfo( const GraphAccessInterface::Node *n1, const GraphAccessInterface::Node *n2)
    {
       int nr = static_cast<const Node*>(n1)->NumOfLoops();
       int nc = static_cast<const Node*>(n2)->NumOfLoops();
       return DepInfoSetGenerator::GetBottomInfoSet(nr, nc, 1);
    }
  DepInfoSet GetIDTransInfo( const GraphAccessInterface::Node *n)
  {
     int nr = static_cast<const Node*>(n)->NumOfLoops();
     return DepInfoSetGenerator::GetIDInfoSet(nr);
  }
  bool IsTop(DepInfoSet t) { return t.IsTop(); }
     
  DepInfoSet  GetTransInfo( const GraphAccessInterface::Edge *e)
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
bool TransDepGraphCreate<Node>::
TransInfoComputed(const GraphAccessInterface::Node *_src, 
                  const GraphAccessInterface::Node *_snk)
   { 
     const Node* src = static_cast<const Node*>(_src);
     const Node* snk = static_cast<const Node*>(_snk);
      return !GraphCrossEdgeIterator<TransDepGraphCreate<Node> >
                 (this, src, snk).ReachEnd();
   }

template <class Node>
DepInfoSet TransDepGraphCreate<Node>::
GetTransInfo( const GraphAccessInterface::Node *_src, 
              const GraphAccessInterface::Node *_snk)
   {
     const Node* src = static_cast<const Node*>(_src);
     const Node* snk = static_cast<const Node*>(_snk);
     DepInfoSetEdge *edge = GraphCrossEdgeIterator<TransDepGraphCreate<Node> >
                                 (this, src, snk).Current(); 
      return edge->GetInfo();
   } 

template <class Node>
void TransDepGraphCreate<Node>::
SetTransInfo( GraphAccessInterface::Node *src1, GraphAccessInterface::Node *snk1, DepInfoSet info) 
   {
 
     Node* src = static_cast<Node*>(src1), *snk =  static_cast<Node*>(snk1);
     GraphCrossEdgeIterator<TransDepGraphCreate<Node> > cross(this, src, snk);
     DepInfoSetEdge *edge = 0;
     if (!cross.ReachEnd()) {
         edge = cross.Current();
         edge->GetInfo() = info;
     }
     else {
         edge = new DepInfoSetEdge(this, info);
         this->AddEdge(src, snk, edge);
     }
   }


template <class Node>
DepInfoSetEdge * TransDepGraphCreate<Node> :: 
GetTransDep( Node *src, Node *snk)
{
  DepInfoSetEdge *edge = 0; 
  GraphCrossEdgeIterator<TransDepGraphCreate<Node> > cross(this, src, snk);
  if (cross.ReachEnd() && anal.ComputeTransInfo(this, src,snk)) {
      edge = (GraphCrossEdgeIterator<TransDepGraphCreate<Node> >(this, src, snk).Current()); 
      assert(edge != 0);
  }
  else
      edge = cross.Current();
  return edge;
}

template <class Node> TransDepGraphCreate<Node> ::  
TransDepGraphCreate(GraphAccessTemplate<Node,DepInfoEdge> *graph,
                    int splitlimit, BaseGraphCreate *bc)
 : VirtualGraphCreateTemplate<Node, DepInfoSetEdge>(bc),
   access(graph),
   anal(&access, analop=new TransDepAnalImpl<Node>(), splitlimit) 
{
  this->AddNodeSet(graph->GetNodeIterator());
}

template <class Node> TransDepGraphCreate<Node> :: ~TransDepGraphCreate()
{
  delete analop;
}

#endif // ROSE_TRANSDEPGRAPHIMPL_H
