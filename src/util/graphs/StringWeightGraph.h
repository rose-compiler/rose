#ifndef STRING_WEIGHT_GRAPH_H
#define STRING_WEIGHT_GRAPH_H

#include <IDGraphCreate.h>
#include <string>
#include <iostream>
#include <map>

template <class NodeImpl, class EdgeImpl, class Weight>
class StringWeightGraph 
  : public IDGraphCreateTemplate <NodeImpl,EdgeImpl>
{
  typedef StringWeightGraph<NodeImpl, EdgeImpl, Weight> MyClass;
 public:
  typedef NodeImpl Node;
  typedef EdgeImpl Edge;
  typedef typename IDGraphCreateTemplate<Node,Edge>::NodeIterator NodeIterator;
  typedef typename IDGraphCreateTemplate<Node,Edge>::EdgeIterator EdgeIterator;
  StringWeightGraph(const string& h) : hint(h) {}

  void add_node(Node* n) 
    {   
        CreateBaseNode(n); 
    }  
  void add_edge(Node* src, Node* snk, Weight w) 
     { 
        Edge* e = GraphGetCrossEdge<MyClass>()(this, src, snk);
        if (e == 0) {
           e = new Edge(this, w);
           CreateBaseEdge(src,snk,e); 
        }
        else
           e->GetInfo() += w;
     }

  void read_from( istream& in)
  {
    string cur;
    in >> cur;
    while (in.good() && cur == hint) {
       in >> cur;
       Node* src = node(this,cur);
       add_node(src);
       in >> cur;
       if (in.good() && cur != hint) {
           Node *snk = node(this,cur);
           add_node(snk);
           in >> cur;
           if (in.good() && cur != hint) {
               double weight;
               sscanf(cur.c_str(), "%lf", &weight); 
               add_edge(src,snk,weight);
               in >> cur;
           }
           else
               add_edge(src,snk,0);
       }
    }  
  }
  void write_to( ostream& out) const
  {
     for (NodeIterator nodes = GetNodeIterator();
           !nodes.ReachEnd() ; ++nodes) {
        Node* curnode = *nodes;
        for (EdgeIterator edges = GetNodeEdgeIterator(curnode,EdgeOut);
             !edges.ReachEnd(); ++edges) {
            Edge *e = (*edges);
            Node* n = GetEdgeEndPoint(e, GraphAccess::EdgeIn);
            out << hint << " " << curnode->GetInfo() << " " << n->GetInfo()  
               << " " << e->GetInfo() << endl; 
        }
     }
  }
  void Dump() const
   { write_to(cerr); }

  bool contain_node(const string& name) const
   { return node.contain_node(name); }
 private:
  string hint;
  class NodeMap {
    map<string,Node*> nodemap;
  public:
   bool contain_node( const string& name) const
    { return nodemap.find(name) != nodemap.end(); }
   Node* operator()(StringWeightGraph* g, const string& name) 
    {
      typename map<string,Node*>::const_iterator p = nodemap.find(name);
      if (p == nodemap.end())
         return nodemap[name] = new NodeImpl(g, name); 
      else
        return (*p).second;
    }
  };
  NodeMap node;
};

#endif
