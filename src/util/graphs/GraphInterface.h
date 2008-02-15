
#ifndef GRAPH_INTERFACE
#define GRAPH_INTERFACE

#include <general.h>
#include <IteratorTmpl.h>
#include <IteratorCompound.h>
#include <ObserveObject.h>
#include <iostream>
#include <string>

class GraphCreate;
class GraphElem 
{
  GraphCreate *gc;
  int count;
 protected:
  GraphElem( GraphCreate *c) : gc(c) { count = 0; }
  virtual ~GraphElem() { assert( count == 0); }

//Boolean UnlinkGraphCreate( const GraphCreate *c)
  int UnlinkGraphCreate( const GraphCreate *c)
       { if (gc != c) 
            count--;
         else
            gc = 0;
         if (gc == 0 && count == 0) {
             delete this;
            return true;
         }
         return false;
       }
  int LinkGraphCreate(const GraphCreate* c)
    { 
      if (gc != c) ++count;
      return count; 
    }
  GraphCreate* GetGraphCreate() const { return gc; }
 public:
  virtual std::string ToString() const { return "";}
 friend class GraphCreate;
};

class GraphEdge : public GraphElem
{
 public:
   GraphEdge(GraphCreate *c) : GraphElem(c) {}
   virtual ~GraphEdge() {}
};

class GraphNode : public GraphElem
{
 public:
   GraphNode(GraphCreate *c) : GraphElem(c) {}
   virtual ~GraphNode() {}
};

class GraphCreate 
{
 protected:
  virtual ~GraphCreate() {}
//Boolean UnlinkElem(GraphElem *n) { return n->UnlinkGraphCreate(this); }
  int UnlinkElem(GraphElem *n) { return n->UnlinkGraphCreate(this); }
  int LinkElem( GraphElem *n) { return n->LinkGraphCreate(this); }
//Boolean LinkedToElem( const GraphElem* e) const
  int LinkedToElem( const GraphElem* e) const
          { return e->GetGraphCreate() == this; }
};

class GraphAccess
{
 public:
   typedef GraphNode Node;
   typedef GraphEdge Edge;
   typedef IteratorImpl<GraphEdge*> EdgeIteratorImpl;
   typedef IteratorImpl<GraphNode*> NodeIteratorImpl;
   typedef IteratorWrap<GraphEdge*,EdgeIteratorImpl> EdgeIterator;
   typedef IteratorWrap<GraphNode*,NodeIteratorImpl> NodeIterator;
   typedef enum {EdgeOut = 1, EdgeIn = 2, BiEdge = 3} EdgeDirection;
   static EdgeDirection Reverse(EdgeDirection d)
    {  switch (d) {
       case EdgeOut: return EdgeIn;
       case EdgeIn: return EdgeOut;
       case BiEdge: return d;
      }
      assert(false);
    }

 protected:
  virtual NodeIterator GetNodeIteratorImpl() const = 0;
  virtual EdgeIterator
        GetNodeEdgeIteratorImpl(const GraphNode *n, EdgeDirection dir) const=0;
  virtual GraphNode*
        GetEdgeEndPointImpl( const GraphEdge *e, EdgeDirection dir) const = 0;
//virtual Boolean ContainNodeImpl(const GraphNode *n) const = 0;
  virtual int ContainNodeImpl(const GraphNode *n) const = 0;
//virtual Boolean ContainEdgeImpl(const GraphEdge *e) const = 0;
  virtual int ContainEdgeImpl(const GraphEdge *e) const = 0;
  virtual ~GraphAccess() {}
 public:
  NodeIterator GetNodeIterator() const { return GetNodeIteratorImpl(); }
  EdgeIterator GetNodeEdgeIterator( const GraphNode *n, EdgeDirection dir) const
   { return GetNodeEdgeIteratorImpl(n,dir); }
  GraphNode* GetEdgeEndPoint( const GraphEdge *e, EdgeDirection dir) const
    { return  GetEdgeEndPointImpl(e, dir); }

//Boolean ContainNode(const GraphNode *n) const { return ContainNodeImpl(n); }
  int ContainNode(const GraphNode *n) const { return ContainNodeImpl(n); }
//Boolean ContainEdge(const GraphEdge *e) const { return ContainEdgeImpl(e); }
  int ContainEdge(const GraphEdge *e) const { return ContainEdgeImpl(e); }

// DQ (9/6/2005): Added function that was part of previous interface
// CI (12/27/2006): EdgeIterator must be in the template class, therefore it has been commented out at this point. Should errors occur due to this, this problem has to be revisited.
// virtual EdgeIterator GetEdgeIterator() const;
};

class GraphObserver
{
 public:
  virtual void UpdateDeleteNode( const GraphNode *n) {}
  virtual void UpdateDeleteEdge( const GraphEdge *e) {}
};

class GraphDeleteNodeInfo : public ObserveInfo<GraphObserver>
{
   GraphNode* n;
 public:
   GraphDeleteNodeInfo( GraphNode *_n) : n(_n) {}
   void UpdateObserver( GraphObserver& o) const
     { o.UpdateDeleteNode( n ); }
};

class GraphDeleteEdgeInfo : public ObserveInfo<GraphObserver>
{
   GraphEdge* n;
 public:
   GraphDeleteEdgeInfo( GraphEdge *_n) : n(_n) {}
   void UpdateObserver( GraphObserver& o) const
     { o.UpdateDeleteEdge( n ); }
};

template <class NodeClass, class EdgeClass>
class GraphAccessTemplate : public GraphAccess
{
  GraphAccess::NodeIterator GetNodeIteratorImpl() const
    { return GetNodeIteratorImpl2(); }
  GraphAccess::EdgeIterator
      GetNodeEdgeIteratorImpl( const GraphNode *n, EdgeDirection dir) const
   { return GetNodeEdgeIteratorImpl2(static_cast<const Node*>(n), dir); }
 public:
  typedef NodeClass Node;
  typedef EdgeClass Edge;
  typedef Iterator2Impl<GraphEdge*,Edge*> EdgeIteratorImpl;
  typedef Iterator2Impl<GraphNode*,Node*> NodeIteratorImpl;
  typedef Iterator2Wrap<GraphEdge*,Edge*> EdgeIterator;
  typedef Iterator2Wrap<GraphNode*,Node*> NodeIterator;
 protected:
  virtual NodeIterator GetNodeIteratorImpl2() const =0;
  virtual EdgeIterator
          GetNodeEdgeIteratorImpl2( const Node *n, EdgeDirection dir) const=0;
 public:
  virtual ~GraphAccessTemplate() {}
  NodeIterator GetNodeIterator() const { return GetNodeIteratorImpl2(); }
  EdgeIterator GetNodeEdgeIterator( const Node *n, EdgeDirection dir) const
   { return GetNodeEdgeIteratorImpl2(n,dir); }
  Node* GetEdgeEndPoint( const Edge *e, EdgeDirection dir) const
    { return  static_cast<Node*>(GetEdgeEndPointImpl(e, dir)); }
//Boolean ContainNode(const GraphNode *n) const
  int ContainNode(const GraphNode *n) const
          { return (n == 0)? false : ContainNodeImpl(n); }
//Boolean ContainEdge(const GraphEdge *e) const
  int ContainEdge(const GraphEdge *e) const
          { return (e == 0)? false : ContainEdgeImpl(e); }
// CI (12/27/2006): The method from GraphAccess needs to return a Iterator2Wrap, therefore GetEdgeIterator needs to be reimplemented here
	//virtual Iterator2Wrap<GraphEdge*,Edge*> GetEdgeIterator2() const;
};

#endif
