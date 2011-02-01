#ifndef GRAPH_DOT_H
#define GRAPH_DOT_H

#include <GraphAccess.h>
#include <string>

template <class Graph>
class GraphDotOutput 
{
 public:
    typedef typename Graph::Node GraphNode;
    typedef typename Graph::Edge GraphEdge;

    void writeToDOTFile(const std::string& filename, const std::string& graphname)
     {
        bool debug = false;
        if(debug) std::cerr << " dot output to " << filename << std::endl; 
        std::ofstream dotfile(filename.c_str());

       //Liao, add "" to enclose the graphname,otherwise syntax error for .dot file. 2/22/2008
        dotfile <<  "digraph \"" << graphname <<"\""<< " {\n";
        for(typename Graph::NodeIterator p=g.GetNodeIterator(); !p.ReachEnd(); ++p) {
            typename Graph::Node* node = *p;
            dotfile << ((long)node) << "[label=\"" << getVertexName(node) << "\" ];" << std::endl;
        }

        if(debug) std::cerr << " finished add node" << std::endl; // debug

        for(typename Graph::NodeIterator p=g.GetNodeIterator(); !p.ReachEnd(); ++p) {
           if(debug) std::cerr << " add edge from node ... " << std::endl; // debug
           typename Graph::Node* node1 = *p;
           typename Graph::EdgeIterator edges = g.GetNodeEdgeIterator(node1, GraphAccess::EdgeOut);
           for( ; !edges.ReachEnd(); ++edges) {
              typename Graph::Edge* e = *edges;
              typename Graph::Node* node2 = g.GetEdgeEndPoint(e, GraphAccess::EdgeIn);
              dotfile << ((long)node1) << " -> " << ((long)node2)
                      << "[label=\"" << getEdgeLabel(e) << "\"];" << std::endl;
           }
        }
        if(debug) std::cerr << " writing content to " << filename << std::endl; // debug
        dotfile <<  "}\n";
     }

 private:
  Graph& g;
  std::string Translate( std::string r1) 
  {
         std::string r2 = "";
         for (unsigned int i = 0; i < r1.size(); ++i) {
            char c = r1[i];
            if (c == '\"')
               r2 = r2 + "\\\"";
            else if (c == '\n') {
               r2 = r2 + "\\n";
           }
            else
               r2 = r2 + c;
         }
         return r2;
  }
 public:
  GraphDotOutput( Graph &_g) : g(_g) {}

     //! get the name of a vertex
     std::string getVertexName(typename Graph::Node* v) 
       { 
         std::string r1 = v->toString();
         std::string r2 = Translate(r1); 
         return r2;
       }

     std::string getEdgeLabel(typename Graph::Edge* e) 
       {
          std::string r1 = e->toString();
          return Translate(r1);
       }


};

#endif















