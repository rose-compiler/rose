#ifndef GRAPH_IO_H
#define GRAPH_IO_H
#include <iostream>
#include <sstream>
#include <string>
#include <GraphAccess.h>

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

template <class Graph, class OUTPUT>
void write_graph( const Graph& g, OUTPUT& out, const std::string& hint) 
  {
     typename Graph::NodeIterator nodes = g.GetNodeIterator();
     for ( ; !nodes.ReachEnd() ; ++nodes) {
        typename Graph::Node* curnode = *nodes;
        typename Graph::EdgeIterator edges 
                        = g.GetNodeEdgeIterator(curnode,GraphAccess::EdgeOut);
        if (edges.ReachEnd()) 
            out << hint << " " << curnode->toString() << std::endl; 
        else for ( ;!edges.ReachEnd(); ++edges) {
            typename Graph::Edge *e = (*edges);
            typename Graph::Node* n = 
                  g.GetEdgeEndPoint(e, GraphAccess::EdgeIn);
            out << hint << " " << curnode->toString() << " " << n->toString() 
               <<  " " << e->toString() << std::endl; 
        }
     }
  }

template <class Graph, class Node>
void write_graph_node_edge(Graph& g, const Node* curnode, 
                      std::ostream& out, const std::string& hint, 
                      GraphAccess::EdgeDirection dir)
{ 
   typename Graph::EdgeIterator edges 
                   = g.GetNodeEdgeIterator(curnode,dir);
   for ( ;!edges.ReachEnd(); ++edges) {
       typename Graph::Edge *e = (*edges);
       typename Graph::Node* n = 
             g.GetEdgeEndPoint(e, GraphAccess::Reverse(dir));
       out << hint << " " << curnode << " " << n  
          << " " << e->toString() << std::endl; 
   }
}

template <class Graph, class OUTPUT>
void write_graph2( const Graph& g, OUTPUT& out, const std::string& hint) 
  {
     typename Graph::NodeIterator nodes = g.GetNodeIterator();
     for ( ; !nodes.ReachEnd() ; ++nodes) {
        typename Graph::Node* curnode = *nodes;
        out << "node " << curnode << ":" << curnode->toString() << std::endl;
     }
     for ( nodes.Reset(); !nodes.ReachEnd() ; ++nodes) {
        typename Graph::Node* curnode = *nodes;
        write_graph_node_edge(g, curnode, out, hint, GraphAccess::EdgeOut);
     }
  }


template <class Graph>
std::string GraphToString(const Graph& g) 
{
    std::stringstream result;
    write_graph(g, result, std::string("edge"));
    return result.str();
}

template <class Graph, class Node>
std::string GraphNodeToString( const Graph& g, const Node* n) 
   {
     std::stringstream result;
     result << "node" << " " << n->toString() << std::endl;
     write_graph_node_edge(g, n, result, "edge-out", GraphAccess::EdgeOut);
     write_graph_node_edge(g, n, result, "edge-in", GraphAccess::EdgeIn);
     return result.str();
   }

inline std::string DoubleToString(double d) 
{ std::stringstream out; out << d; return out.str(); }

#endif
