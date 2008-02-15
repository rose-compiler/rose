#ifndef GRAPH_IO_H
#define GRAPH_IO_H
#include <iostream>
#include <sstream>
#include <string>
#include <GraphAccess.h>

template <class Graph>
void read_graph(Graph& g, STD istream& in, const STD string& hint)
  {
    STD string cur;
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

template <class Graph, class OUT>
void write_graph( const Graph& g, OUT& out, const STD string& hint) 
  {
     typename Graph::NodeIterator nodes = g.GetNodeIterator();
     for ( ; !nodes.ReachEnd() ; ++nodes) {
        typename Graph::Node* curnode = *nodes;
        typename Graph::EdgeIterator edges 
                        = g.GetNodeEdgeIterator(curnode,GraphAccess::EdgeOut);
        if (edges.ReachEnd()) 
            out << hint << " " << curnode->toString() << STD endl; 
        else for ( ;!edges.ReachEnd(); ++edges) {
            typename Graph::Edge *e = (*edges);
            typename Graph::Node* n = 
                  g.GetEdgeEndPoint(e, GraphAccess::EdgeIn);
            out << hint << " " << curnode->toString() << " " << n->toString() 
               <<  " " << e->toString() << STD endl; 
        }
     }
  }

template <class Graph, class Node>
void write_graph_node_edge(Graph& g, const Node* curnode, 
                      STD ostream& out, const STD string& hint, 
                      GraphAccess::EdgeDirection dir)
{ 
   typename Graph::EdgeIterator edges 
                   = g.GetNodeEdgeIterator(curnode,dir);
   for ( ;!edges.ReachEnd(); ++edges) {
       typename Graph::Edge *e = (*edges);
       typename Graph::Node* n = 
             g.GetEdgeEndPoint(e, GraphAccess::Reverse(dir));
       out << hint << " " << curnode << " " << n  
          << " " << e->toString() << STD endl; 
   }
}

template <class Graph, class OUT>
void write_graph2( const Graph& g, OUT& out, const STD string& hint) 
  {
     typename Graph::NodeIterator nodes = g.GetNodeIterator();
     for ( ; !nodes.ReachEnd() ; ++nodes) {
        typename Graph::Node* curnode = *nodes;
        out << "node " << curnode << ":" << curnode->toString() << STD endl;
     }
     for ( nodes.Reset(); !nodes.ReachEnd() ; ++nodes) {
        typename Graph::Node* curnode = *nodes;
        write_graph_node_edge(g, curnode, out, hint, GraphAccess::EdgeOut);
     }
  }


template <class Graph>
STD string GraphToString(const Graph& g) 
{
    STD stringstream result;
    write_graph(g, result, STD string("edge"));
    return result.str();
}

template <class Graph, class Node>
STD string GraphNodeToString( const Graph& g, const Node* n) 
   {
     STD stringstream result;
     result << "node" << " " << n->toString() << STD endl;
     write_graph_node_edge(g, n, result, "edge-out", GraphAccess::EdgeOut);
     write_graph_node_edge(g, n, result, "edge-in", GraphAccess::EdgeIn);
     return result.str();
   }

inline STD string DoubleToString(double d) 
{ STD stringstream out; out << d; return out.str(); }

#endif
