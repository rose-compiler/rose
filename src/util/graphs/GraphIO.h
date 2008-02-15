#ifndef GRAPH_IO_H
#define GRAPH_IO_H
#include <iostream>
#include <sstream>
#include <string>

template <class Graph>
void read_graph(Graph& g, std::istream& in, const std::string& hint)
  {
    std::string cur;
    in >> cur;
    while (in.good() && cur == hint) {
       in >> cur;
       typename Graph::Node* src = g.add_node(cur);
       in >> cur;
       if (in.good() && cur != hint) {
           typename Graph::Node *snk = g.add_node(cur);
           in >> cur;
           if (in.good() && cur != hint) {
               g.add_edge(src,snk,cur);
               in >> cur;
           }
           else
               g.add_edge(src,snk,"");
       }
    }  
  }

template <class Graph>
void write_graph_node(Graph& g, const typename Graph::Node* curnode, 
                      std::ostream& out, const std::string& hint, 
                      GraphAccess::EdgeDirection dir)
{ 
   typename Graph::EdgeIterator edges 
                   = g.GetNodeEdgeIterator(curnode,dir);
   if (edges.ReachEnd()) {
       out << hint << " " << curnode->ToString() << std::endl;
   }
   else for ( ;!edges.ReachEnd(); ++edges) {
       typename Graph::Edge *e = (*edges);
       typename Graph::Node* n = 
             g.GetEdgeEndPoint(e, GraphAccess::Reverse(dir));
       out << hint << " " << curnode->ToString() << " " << n->ToString()  
          << " " << e->ToString() << std::endl; 
   }
}

template <class Graph>
void write_graph( const Graph& g, std::ostream& out, const std::string& hint) 
  {
     typename Graph::NodeIterator nodes = g.GetNodeIterator();
     for ( ; !nodes.ReachEnd() ; ++nodes) {
        typename Graph::Node* curnode = *nodes;
        write_graph_node(g, curnode, out, hint, GraphAccess::EdgeOut);
     }
  }

template <class Graph>
std::string GraphToString(const Graph& g) 
{
    std::stringstream result;
    write_graph(g, result, "edge");
    return result.str();
}

template <class Graph>
std::string GraphNodeToString( const Graph& g, const typename Graph::Node* n) 
   {
     std::stringstream result;
     write_graph_node(g, n, result, "edge-out", GraphAccess::EdgeOut);
     write_graph_node(g, n, result, "edge-in", GraphAccess::EdgeIn);
     return result.str();
   }

class DefaultToString{
 public:
// DQ (1/7/2006): these function must be static to compile using g++ 4.0.2
  static std::string ToString(const std::string& d) { return d; }
  static std::string ToString(double d) { std::stringstream out; out << d; return out.str(); }
};

class StringWeightGraph 
  : public IDGraphCreateTemplate < GraphNodeTemplate<std::string,const std::string&, DefaultToString>,
                                   GraphEdgeTemplate<double, double, DefaultToString> >
{
  typedef IDGraphCreateTemplate < GraphNodeTemplate<std::string,const std::string&, DefaultToString>,
                                   GraphEdgeTemplate<double, double, DefaultToString> >
      BaseClass;
 public:
  typedef BaseClass::Node Node;
  typedef BaseClass::Edge  Edge;
  typedef BaseClass::NodeIterator NodeIterator;
  typedef BaseClass::EdgeIterator EdgeIterator;

  Node* add_node(const std::string& cur) 
    {   
        Node* n = node(this,cur);
        add_node(n);
        return n;
    }  
  void add_node(Node* n) 
    {   
        CreateBaseNode(n); 
    }  
  void add_edge(Node* src, Node* snk, const std::string& w) 
     { 
       double d = 0;
       sscanf(w.c_str(), "%lf", &d);
       add_edge(src,snk,d);
     }
  void add_edge(Node* src, Node* snk, double d) 
     {
       Edge* e = GraphGetCrossEdge<StringWeightGraph>()(this, src, snk);
        if (e == 0) {
           e = new Edge(this, d);
           CreateBaseEdge(src,snk,e); 
        }
        else
           e->GetInfo() += d;
     }
 private:
  class NodeMap {
    std::map<std::string,Node*> nodemap;
  public:
   bool contain_node( const std::string& name) const
    { return nodemap.find(name) != nodemap.end(); }
   Node* operator()(StringWeightGraph* g, const std::string& name) 
    {
      std::map<std::string,Node*>::const_iterator p = nodemap.find(name);
      if (p == nodemap.end())
         return nodemap[name] = new Node(g, name); 
      else
        return (*p).second;
    }
  };
  NodeMap node;
};

#endif
