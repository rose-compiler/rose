
#ifndef GRAPH_SCOPE_H
#define GRAPH_SCOPE_H

#include <GraphAccess.h>
#include <assert.h>

template <class Graph>
class GraphReverseEdge 
  : public GraphAccessTemplate<typename Graph::Node, typename Graph::Edge>
{
  Graph* impl;
 public:
  typedef typename Graph::Node Node;
  typedef typename Graph::Edge Edge;
  typedef typename Graph::NodeIterator NodeIterator;
  typedef typename Graph::EdgeIterator EdgeIterator;
  GraphReverseEdge(Graph* _impl) : impl(_impl) {}
  ~GraphReverseEdge() {}
  typename Graph::EdgeIterator 
   GetNodeEdgeIterator(const typename Graph::Node *n, GraphAccess::EdgeDirection dir) const
   {return impl->GetNodeEdgeIterator(n, GraphAccess::Reverse(dir) ); }
  typename Graph::Node* 
    GetEdgeEndPoint( const typename Graph::Edge *e, GraphAccess::EdgeDirection dir) const
   { return impl->GetEdgeEndPoint(e, GraphAccess::Reverse(dir) ); }

  typename Graph::NodeIterator GetNodeIterator() const { return impl->GetNodeIterator(); }
  bool ContainEdge( const typename Graph::Edge* e) const {return impl->ContainEdge(e); }
  bool ContainNode( const typename Graph::Node* n) const { return impl->ContainNode(n); }
};

template <class Graph>
class GraphSelect 
  : public GraphAccessTemplate<typename Graph::Node, typename Graph::Edge>
{
 protected:
  const Graph* impl;
 public:
  GraphSelect(const Graph* _impl) : impl(_impl) {}
  ~GraphSelect() {}

  class GraphSelectNodeIterator 
       : public IteratorImplTemplate<typename Graph::Node*, 
                                     typename Graph::NodeIterator> 
      {
        typedef IteratorImplTemplate<typename Graph::Node*, 
                                     typename Graph::NodeIterator> BaseClass;
        const GraphSelect<Graph>* graphsel;  
        void SetIterator() 
         {
           for ( ; !BaseClass::impl.ReachEnd(); BaseClass::impl.Advance()) {
              if (graphsel->ContainNode(BaseClass::impl.Current()))
                 break;
           }
         }
      public:
        GraphSelectNodeIterator(const GraphSelect<Graph>* _graphsel,
                                const typename Graph::NodeIterator& _impl)
              : IteratorImplTemplate<typename Graph::Node*,
                                     typename Graph::NodeIterator>(_impl), 
                graphsel(_graphsel)
             { SetIterator(); }
        virtual ~GraphSelectNodeIterator() {} 
        void Advance() { BaseClass::impl.Advance(); SetIterator(); } 
        void Reset() { BaseClass::impl.Reset(); SetIterator(); } 
        IteratorImpl<typename Graph::Node*>* Clone() const
           { return new GraphSelectNodeIterator(*this); }
    } ;
  class GraphSelectEdgeIterator 
       : public IteratorImplTemplate<typename Graph::Edge*, 
                                     typename Graph::EdgeIterator> 
      {
       typedef IteratorImplTemplate<typename Graph::Edge*, 
                                     typename Graph::EdgeIterator> BaseClass;
        const GraphSelect<Graph>* graphsel;  
        void SetIterator() 
         {
           for ( ; !BaseClass::impl.ReachEnd(); BaseClass::impl.Advance()) {
              if (graphsel->ContainEdge(BaseClass::impl.Current()))
                 break;
           }
         }
      public:
        GraphSelectEdgeIterator(const GraphSelect<Graph>* _graphsel,
                                const typename Graph::EdgeIterator& _impl)
              : IteratorImplTemplate<typename Graph::Edge*,
                                     typename Graph::EdgeIterator>(_impl), 
                graphsel(_graphsel)
             { SetIterator(); }
        virtual ~GraphSelectEdgeIterator() {} 
        void Advance() { BaseClass::impl.Advance(); SetIterator(); } 
        void Reset() { BaseClass::impl.Reset(); SetIterator(); } 
        IteratorImpl<typename Graph::Edge*>* Clone() const
           { return new GraphSelectEdgeIterator(*this); }
     };

  typename Graph::NodeIterator GetNodeIterator() const 
    { return new GraphSelectNodeIterator(this, impl->GetNodeIterator()); }
  typename Graph::EdgeIterator 
  GetNodeEdgeIterator(const typename Graph::Node *n, 
                              GraphAccess::EdgeDirection dir) const
   {return new GraphSelectEdgeIterator(this,impl->GetNodeEdgeIterator(n,dir)); }
  typename Graph::Node* GetEdgeEndPoint(const typename Graph::Edge* e, 
                                        GraphAccess::EdgeDirection dir) const
   { return impl->GetEdgeEndPoint(e, dir); }
};

#endif
