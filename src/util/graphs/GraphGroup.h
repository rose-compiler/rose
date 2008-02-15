 
#ifndef GROUP_GRAPH_H
#define GROUP_GRAPH_H

#include <PtrSet.h>
#include <IDGraphCreate.h>

class GroupGraphNode : public GraphNode
{
  PtrSetWrap <GraphNode> nodeSet;
 public:
  GroupGraphNode( GraphCreate *g) : GraphNode(g) {}
  ~GroupGraphNode() {}
  typedef PtrSetWrap<GraphNode>::Iterator Iterator;
  std::string ToString() const
    { 
      std::string res;
      PtrSetWrap<GraphNode>::Iterator iter=nodeSet.GetIterator();
      for (GraphNode* n; (n = iter.Current()); iter++)
         res = res + "; " + n->ToString();
      res = res + "\n";
      return res;
    }
//Boolean ContainNode( const GraphNode *n) const 
  int ContainNode( const GraphNode *n) const 
       { return nodeSet.IsMember(const_cast<GraphNode*>(n) ); }
  virtual void AddNode(const GraphNode* n) 
       { nodeSet.Add(const_cast<GraphNode*>(n) ); }
  Iterator GetIterator() const { return nodeSet.GetIterator(); }
};

class GroupNodeSelect
{
   GroupGraphNode *node;
  public:
   typedef GroupGraphNode::Iterator Iterator;
   GroupNodeSelect(GroupGraphNode *n) : node(n) {}
// Boolean operator()(const GraphNode* n) const { return node->ContainNode(n);}
   int operator()(const GraphNode* n) const { return node->ContainNode(n);}
  Iterator GetIterator() const { return node->GetIterator(); }
};

class GroupGraphEdge : public GraphEdge
{
  PtrSetWrap <GraphEdge> edgeSet;
 public:
  GroupGraphEdge( GraphCreate *g) : GraphEdge(g) {}
  ~GroupGraphEdge() {}
  typedef PtrSetWrap<GraphEdge>::Iterator Iterator;
  std::string ToString() const 
    { 
      std::string res;
      Iterator iter=edgeSet.GetIterator();
      for (GraphEdge* e; (e = iter.Current()); iter++)
         res = res + "; " + e->ToString();
      res = res + "\n";
      return res;
    }
  void AddEdge( GraphEdge *e) { edgeSet.Add(e); }
//Boolean ContainEdge( const GraphEdge *e) const 
  int ContainEdge( const GraphEdge *e) const 
       { return edgeSet.IsMember(const_cast<GraphEdge*>(e)); }
  Iterator GetIterator() const { return edgeSet.GetIterator(); }
};

class GroupEdgeSelect
{
   GroupGraphEdge *edge;
  public:
   GroupEdgeSelect(GroupGraphEdge *e) : edge(e) {}
// Boolean operator()(const GraphEdge* e) const { return edge->ContainEdge(e);}
   int operator()(const GraphEdge* e) const { return edge->ContainEdge(e);}
};

typedef GraphAccessTemplate<GroupGraphNode,GroupGraphEdge> GroupGraph;

class GroupGraphCreate : public IDGraphCreateTemplate<GroupGraphNode,GroupGraphEdge>
{
 public:
  void AddGroupNode( GroupGraphNode *n)
    { CreateBaseNode(n); }
  void AddGroupEdge( GroupGraphNode *src, GroupGraphNode *snk, GroupGraphEdge *e)
    { CreateBaseEdge(src, snk, e); }
  GroupGraphCreate( BaseGraphCreate *_impl=0)
    :IDGraphCreateTemplate<GroupGraphNode,GroupGraphEdge>(_impl) {};
  ~GroupGraphCreate() {}
};

#endif
