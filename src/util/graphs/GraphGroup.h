 
#ifndef GROUP_GRAPH_H
#define GROUP_GRAPH_H

#include <PtrSet.h>
#include <VirtualGraphCreate.h>
#include <GraphScope.h>

class GroupGraphNode : public MultiGraphElem
{
 public:
  typedef GraphAccessInterface::Node Node;
  GroupGraphNode( MultiGraphCreate *g) : MultiGraphElem(g) {}
  ~GroupGraphNode() {}
  typedef PtrSetWrap<Node>::const_iterator const_iterator;
  bool ContainNode( const Node *n) const 
       { return nodeSet.IsMember(const_cast<Node*>(n) ); }
  virtual void AddNode(const Node* n) 
       { nodeSet.insert(const_cast<Node*>(n) ); }
  const_iterator begin() const { return nodeSet.begin(); }
  const_iterator end() const { return nodeSet.end(); }
 private:
  PtrSetWrap <Node> nodeSet;
};

class GroupGraphEdge : public MultiGraphElem
{
 public:
  typedef GraphAccessInterface::Edge Edge;
  GroupGraphEdge( MultiGraphCreate *g) : MultiGraphElem(g) {}
  ~GroupGraphEdge() {}
  typedef PtrSetWrap<Edge>::const_iterator const_iterator;
  void AddEdge( Edge *e) { edgeSet.insert(e); }
  bool ContainEdge( const Edge *e) const 
       { return edgeSet.IsMember(const_cast<Edge*>(e)); }
  const_iterator begin() const { return edgeSet.begin(); }
  const_iterator end() const { return edgeSet.end(); }
 private:
  PtrSetWrap <Edge> edgeSet;
};

class GroupGraphCreate 
   : public VirtualGraphCreateTemplate<GroupGraphNode,GroupGraphEdge>
{
 public:
  void AddGroupNode( GroupGraphNode *n)
    { AddNode(n); }
  void AddGroupEdge( GroupGraphNode *src, GroupGraphNode *snk, GroupGraphEdge *e)
    { AddEdge(src, snk, e); }
  GroupGraphCreate( BaseGraphCreate *_impl=0)
    :VirtualGraphCreateTemplate<GroupGraphNode,GroupGraphEdge>(_impl) {};
  ~GroupGraphCreate() {}
};

class GroupNodeSelect  : public GraphSelect<GraphAccessInterface>
{
   GroupGraphNode *node;
  public:
   GroupNodeSelect(GraphAccessInterface* g, GroupGraphNode *n) 
     : GraphSelect<GraphAccessInterface >(g), node(n) {}
   NodeIterator GetNodeIterator() const 
     { return new IteratorImplTemplate<Node*, GroupGraphNode::const_iterator>
                 (node->begin()); 
     }
   bool ContainNode(const Node* n) const { return node->ContainNode(n); }
   bool ContainEdge(const Edge* e) const  
       { return node->ContainNode(impl->GetEdgeEndPoint(e,EdgeOut)) &&
                node->ContainNode(impl->GetEdgeEndPoint(e,EdgeIn)); 
       }
};

#endif
