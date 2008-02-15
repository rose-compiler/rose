
#ifndef ID_GRAPH_CREATE_H
#define ID_GRAPH_CREATE_H

// pmp 08JUN05
//   fixes changes in template processing starting in gcc 3.4
//   particularly, the following code compiles w/ 3.2 but not w/ 3.4
//   (included iostream header file).
//
// using namespace std;
// 
// template <class T>
// struct Printer
// { void test()
//   { cerr << "Hallo World";
//   }
// };
// 
// #include <iostream>
// 
// int main()
// { Printer<int> p;
// 
//   p.test();
// }
#include <iostream>

#include <PtrMap.h>
#include <BaseGraphCreate.h>
#include <GraphUtils.h>

template <class NodeImpl, class EdgeImpl>
class IDGraphCreateTemplate 
  : public GraphCreate, public GraphAccessTemplate<NodeImpl,EdgeImpl>
{
 public:
  typedef NodeImpl Node;
  typedef EdgeImpl Edge;
  typedef typename GraphAccessTemplate<NodeImpl,EdgeImpl>::NodeIterator NodeIterator;
  typedef typename GraphAccessTemplate<NodeImpl,EdgeImpl>::EdgeIterator EdgeIterator;
  typedef typename GraphAccess::EdgeDirection EdgeDirection;
  GraphAccessTemplate<NodeImpl,EdgeImpl>::GetNodeIterator;
 private:
  BaseGraphCreate *impl;
  PtrMapWrap <Node, BaseGraphCreate::Node> nodeMap;
  PtrMapWrap <Edge, BaseGraphCreate::Edge> edgeMap;

  NodeIterator GetNodeIteratorImpl2() const
     { return new Iterator2ImplTemplate<GraphNode*, Node*, IDNodeIterator>
                    (GetIDNodeIterator()); 
     }
  EdgeIterator GetNodeEdgeIteratorImpl2( const Node *n, EdgeDirection dir) const
    { return  new Iterator2ImplTemplate<GraphEdge*, Edge*, IDEdgeIterator>
                 ( GetIDNodeEdgeIterator( n, dir)); }
  GraphNode* GetEdgeEndPointImpl(const GraphEdge *e,EdgeDirection dir) const
    { return GetEdgeEndPoint(static_cast<const Edge*>(e),dir); }
//Boolean ContainNodeImpl( const GraphNode *n) const
  int ContainNodeImpl( const GraphNode *n) const
   { return ContainNode(static_cast<const Node*>(n)); }
//Boolean ContainEdgeImpl( const GraphEdge *e) const
  int ContainEdgeImpl( const GraphEdge *e) const
   { return ContainEdge(static_cast<const Edge*>(e)); }
 protected:
  BaseGraphCreate::Node* CreateBaseNode( Node *idNode);
  BaseGraphCreate::Edge* CreateBaseEdge( Node *idSrc, Node *idSnk, Edge *idEdge);
  BaseGraphCreate* GetBaseGraph() { return impl; }

  virtual ~IDGraphCreateTemplate();

  IDGraphCreateTemplate(BaseGraphCreate *_impl = 0);
  void CloneNodeSet( NodeIterator iter);
  void CloneGraph( const GraphAccessTemplate<Node,Edge> *dg);
//Boolean DeleteNode(Node* n);
  int DeleteNode(Node* n);
//Boolean DeleteEdge(Edge* e);
  int DeleteEdge(Edge* e);
  void MoveEdgeEndPoint(Edge *e, EdgeDirection dir, Node *n);

 public:
  BaseGraphCreate::Node* GetBaseNode( const Node *id) const;
  BaseGraphCreate::Edge* GetBaseEdge( const Edge *id) const;
  Node *GetIDNode(const BaseGraphCreate::Node *base) const;
  Edge *GetIDEdge(const BaseGraphCreate::Edge *base) const;

//Boolean ContainNode( const Node *n) const
  int ContainNode( const Node *n) const
   { return GetBaseNode(n) != 0; }
//Boolean ContainEdge( const Edge *e) const
  int ContainEdge( const Edge *e) const
   { return GetBaseEdge(e) != 0; }
//Boolean IsBaseNode( const Node* n) const
  int IsBaseNode( const Node* n) const
   { return impl->ContainNode(n); }
//Boolean IsBaseEdge( const Edge* e) const
  int IsBaseEdge( const Edge* e) const
   { return impl->ContainEdge(e); }
  Node* GetEdgeEndPoint( const Edge *e, EdgeDirection dir) const;

  class IDEdgeIterator
   { 
     const IDGraphCreateTemplate<Node,Edge> *gc;
     BaseGraphCreate::EdgeIterator baseIter;
     typedef GraphAccess::EdgeDirection EdgeDirection;
     IDEdgeIterator( const IDGraphCreateTemplate<Node,Edge> *_gc, 
                   const BaseGraphCreate::EdgeIterator &base)
          : gc(_gc), baseIter(base) {}
    public:
     IDEdgeIterator( const IDEdgeIterator &that)
          : gc(that.gc), baseIter(that.baseIter) {}
     IDEdgeIterator& operator = ( const IDEdgeIterator &that)
          { gc = that.gc; baseIter = that.baseIter; return *this; }

     void Reset() { baseIter.Reset(); }
     void Advance() { baseIter.Advance(); }
     void operator ++() { Advance(); }
     void operator ++(int) {Advance(); }
     Edge* Current() const { return gc->GetIDEdge( baseIter.Current() ); }
     Edge* operator*() const { return Current(); }
  // Boolean ReachEnd() const { std::cerr << ""; return baseIter.ReachEnd(); }
     int ReachEnd() const { std::cerr << ""; return baseIter.ReachEnd(); }
    friend class IDGraphCreateTemplate<Node,Edge>;
   };
   IDEdgeIterator GetIDNodeEdgeIterator( const Node *id, EdgeDirection dir) const
       { return IDEdgeIterator(this, impl->GetNodeEdgeIterator( GetBaseNode(id), dir) ); }

   class IDNodeIterator 
   { const IDGraphCreateTemplate<Node,Edge> *gc;
     BaseGraphCreate::NodeIterator baseIter;
     IDNodeIterator( const IDGraphCreateTemplate<Node,Edge> *_gc, const BaseGraphCreate::NodeIterator& base)
         : gc(_gc), baseIter(base) {}
   public:
     IDNodeIterator( const IDNodeIterator &that)
        : gc(that.gc), baseIter(that.baseIter) {}
     IDNodeIterator& operator = ( const IDNodeIterator &that)
          { gc = that.gc; baseIter = that.baseIter; return *this; }
     void Reset() { baseIter.Reset(); }
     void Advance() { baseIter.Advance(); }
     void operator ++() { Advance(); }
     void operator ++(int) {Advance(); }
     Node* Current() const { return gc->GetIDNode( baseIter.Current() ); }
     Node* operator*() const { return Current(); }
  // Boolean ReachEnd() const { std::cerr << ""; return baseIter.ReachEnd(); }
     int ReachEnd() const { std::cerr << ""; return baseIter.ReachEnd(); }
    friend class IDGraphCreateTemplate<Node,Edge>;
   };
   IDNodeIterator GetIDNodeIterator() const
        { return IDNodeIterator(this, impl->GetNodeIterator() ); }
};

class ShadowGraphCreate : public IDGraphCreateTemplate<GraphNode,GraphEdge>
{
 public:

// DQ (12/31/2005): This causes a warning when compiled with g++, but I don't see how to fix it.
  ShadowGraphCreate(BaseGraphCreate *_impl = 0) 
   : IDGraphCreateTemplate<GraphNode,GraphEdge>(_impl){}

  BaseGraphCreate::Node* CloneGraphNode( GraphNode *n) 
      { return CreateBaseNode(n); }
  BaseGraphCreate::Edge* CloneGraphEdge( const GraphAccess* that, GraphEdge *e) 
      { return CreateBaseEdge(that->GetEdgeEndPoint(e, EdgeOut), 
                              that->GetEdgeEndPoint(e, EdgeIn), e); }
  void CloneGraphNodeSet( GraphAccess::NodeIterator iter)
      { 
        for ( ; !iter.ReachEnd(); ++iter) 
          CloneGraphNode(*iter);
      }
  void CloneGraphEdgeSet( const GraphAccess* g, GraphAccess::EdgeIterator iter)
      { 
        for ( ; !iter.ReachEnd(); ++iter) 
          CloneGraphEdge(g, *iter);
      }
  void CloneGraph( const GraphAccess *dg) 
      { 
        CloneGraphNodeSet(dg->GetNodeIterator());
        CloneGraphEdgeSet(dg, GraphGetEdgeIterator<GraphAccess>()(dg)); 
      }
//Boolean DeleteNode(GraphNode* n) { IDGraphCreateTemplate<GraphNode,GraphEdge>::DeleteNode(n); return false; }
  int DeleteNode(GraphNode* n) { IDGraphCreateTemplate<GraphNode,GraphEdge>::DeleteNode(n); return false; }
//Boolean DeleteEdge(GraphEdge* e) { IDGraphCreateTemplate<GraphNode,GraphEdge>::DeleteEdge(e); return false; }
  int DeleteEdge(GraphEdge* e) { IDGraphCreateTemplate<GraphNode,GraphEdge>::DeleteEdge(e); return false; }
};

// PC (9/24/2006): Prevent multiple instantiations
#define IDGRAPH_TEMPLATE_ONLY
// DQ (9/4/2005): The standard way of including any template definition 
// is to include it at the base of the header file (if at all).
#include <IDGraphCreate.C>

#endif

