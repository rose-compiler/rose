#ifndef GRAPH_DOT_H
#define GRAPH_DOT_H

#include <GraphInterface.h>
#include <string>
#include <DOTGraphInterface.h>

class DotNodeIterator : public GraphAccess::NodeIterator
{
 public:
   DotNodeIterator( const GraphAccess::NodeIterator& that)
     : GraphAccess::NodeIterator(that) {}
   bool operator ==(  const DotNodeIterator& that) const
      { return (IsEmpty() && that.ReachEnd()) ||
               (ReachEnd() && that.IsEmpty()) ||
               GraphAccess::NodeIterator::operator==(that); }
   bool operator != (  const DotNodeIterator& that) const
      { return ! operator ==(that); }
   GraphNode& operator *() const { return * GraphAccess::NodeIterator::operator *(); }
};

class DotEdgeIterator : public GraphAccess::EdgeIterator
{
 public:
   DotEdgeIterator( const GraphAccess::EdgeIterator& that)
     : GraphAccess::EdgeIterator(that) {}
   bool operator ==(  const DotEdgeIterator& that) const 
      { return (IsEmpty() && that.ReachEnd()) ||
               (ReachEnd() && that.IsEmpty()) ||
               GraphAccess::EdgeIterator::operator==(that); }
   bool operator != ( const DotEdgeIterator& that) const
      { return ! operator ==(that); }
   GraphEdge& operator *() const { return * GraphAccess::EdgeIterator::operator *(); }
};

class GraphDotOutput 
  : public DOTGraphInterface<GraphNode, GraphEdge, DotNodeIterator, DotEdgeIterator>
{
  GraphAccess& g;
  std::string Translate( std::string r1) 
  {
         std::string r2 = "";
         for (unsigned int i = 0; i < r1.size(); ++i) {
            char c = r1[i];
            if (c == '\"')
               r2 = r2 + "\\\"";
            else
               r2 = r2 + c;
         }
         return r2;
  }
 public:
  GraphDotOutput( GraphAccess &_g) : g(_g) {}
  virtual DotNodeIterator getVertices() 
         { return g.GetNodeIterator(); }

  //! get the end of the vertices
  virtual DotNodeIterator getVerticesEnd() 
         { return GraphAccess::NodeIterator(); }

  //! get an iterator for all outgoing edges of a vertex
  virtual DotEdgeIterator getEdges(GraphNode &v) 
    { return g.GetNodeEdgeIterator(&v, GraphAccess::EdgeOut); }

   //! get the end of the outgoing edges
  virtual DotEdgeIterator getEdgesEnd(GraphNode &v) 
   { return GraphAccess::EdgeIterator(); }



   //! get unique string representation of a vertex, NOTE - this function works with a pointer to the vertex!
   virtual std::string vertexToString(GraphNode *v) 
      { 
        char buf[100];
        sprintf(buf, "%p", v);
        return buf;
      }

     //! get the name of a vertex
    virtual std::string getVertexName(GraphNode &v) 
       { 
         std::string r1 = v.ToString();
         std::string r2 = Translate(r1); 
         return r2;
       }

     virtual std::string getEdgeLabel(GraphEdge &e) 
       {
          std::string r1 = e.ToString();
          return Translate(r1);
       }


     //! get the target vertex of an edge, NOTE - this function returns a pointer to a vertex for use with vertexToString!
     virtual GraphNode *getTargetVertex(GraphEdge &e) 
      { return g.GetEdgeEndPoint(&e, GraphAccess::EdgeIn); }

		//! query the subgraph ID of a node
		virtual int getVertexSubgraphId(GraphNode &v) {
			// assume there are no subgraphs
			return -2;
		}

#if 0
	   //! add new subgraph, returns id of the subgraph
      virtual int addSubgraph(std::string name) 
         {
           return mDotRep.addSubgraph(name);
         }
#endif
};

#endif















