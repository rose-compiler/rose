/******************************************************************************
 *
 * ROSE Global Database Connection
 *
 * Interface for DOT output of graphs
 *
 *****************************************************************************/

#ifndef DOTGRAPHINTERFACE_H
#define DOTGRAPHINTERFACE_H

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

#include <map>
#include <iterator>
#include <stack>
#include <iostream>
#include <string>

#include "DOTSubgraphRepresentation.h"

//! stl graph class for directed and undirected graphs
template<class VertexType, class EdgeType, class VertexIterator, class EdgeIterator>
class DOTGraphInterface
   {
     public:

		//! public type interface

		//! destructor
		virtual ~DOTGraphInterface() { printf ("Inside of ~DOTGraphInterface() \n"); };

		//! this is the function that can be used to ouput the graph to a DOT file
		void writeToDOTFile(std::string filename);

     protected:

		//! directions of an edge: no direction, towards target, towards source  or bidirectional
		typedef enum {
			none = 1,
			target,
			source,
			both
		} EdgeDirection;

		
		// ---------
		// iterators

		//! get an iterator for the vertices
		virtual VertexIterator getVertices() = 0;

		//! get the end of the vertices
		virtual VertexIterator getVerticesEnd() = 0;

		//! get an iterator for all edges of a vertex
		virtual EdgeIterator getEdges(VertexType &v) = 0;

		//! get the end of the edges
		virtual EdgeIterator getEdgesEnd(VertexType &v) = 0;

		
		// ----------------
		// vertex functions
		
		//! get unique string representation of a vertex, NOTE - this function works with a pointer to the vertex!
		virtual std::string vertexToString(VertexType *v) = 0;

		//! get the name of a vertex
		virtual std::string getVertexName(VertexType &v) = 0;

		// -------------
		// edge functions

		//! get the target vertex of an edge, NOTE - this function returns a pointer to a vertex for use with vertexToString!
		virtual VertexType *getTargetVertex(EdgeType &e) = 0;

		
		// ----------------------------------------------
		// optional functions to improve the graph output
		// these do not necessarily have to be implemented

		//! query the subgraph ID of a node
		virtual int getVertexSubgraphId(VertexType &v) {
			// assume there are no subgraphs
			return -1;
		}

		//! get a list of the subgraph names, together with their ID's (see also getVertexSubgraphId)
		//  this function should construct a map containing the ID a the name as a string
		//  subgraph Id's have to be >0 to be valid
		//  only subgraphs entered in this list will be displayed (the name may also be empty)
		virtual std::map<int,std::string> getSubgraphList() {
			// none by default...
			std::map<int,std::string> temp;
			return temp;
		}
		
		//! get the label of an edge
		virtual std::string getEdgeLabel(EdgeType &e) { 
			// the default is no edge label
			return std::string(""); 
		}

		//! get the direction of an edge
		virtual EdgeDirection getEdgeDirection(EdgeType &e) { 
			// normally its from the source to target
			return target; 
		}

     public:
      // DQ (8/14/2004): Added support for subgraphs (also make this public)
	   //! add new subgraph, returns id of the subgraph
	   virtual int addSubgraph(std::string name) 
         {
           return mDotRep.addSubgraph(name);
         }

     private:

		//! dot representation object
		DOTSubgraphRepresentation< std::string > mDotRep;
   };

//-----------------------------------------------------------------------------

template<class VertexType, class EdgeType, class VertexIterator, class EdgeIterator>
void
DOTGraphInterface<VertexType, EdgeType, VertexIterator, EdgeIterator>::writeToDOTFile ( std::string filename )
   {
     bool debug = false;

     if (debug == true)
          std::cerr << " dot output to " << filename << std::endl; // debug

     mDotRep.clear();

  // init subgraphs (this might just be empty, that also works)
     std::map<int,std::string> subgraphs( getSubgraphList() );
     for(std::map<int,std::string>::iterator s=subgraphs.begin(); s!=subgraphs.end(); s++)
        {
       // cerr << " adding subgrph " << endl;
       // cerr << " adding " << (*s).first <<" as "<< (*s).second <<endl;
          mDotRep.addSubgraph( (*s).first, (*s).second );
        }
	
  // create all nodes
     for(VertexIterator i=getVertices(); i!=getVerticesEnd(); i++)
        {
          int subgraphId = getVertexSubgraphId( *i );
       // printf ("In writeToDOTFile(): subgraphId = %d \n",subgraphId);
          if(subgraphId>=0)
             {
               mDotRep.addNode( vertexToString(&(*i)), getVertexName( (*i) ), "", subgraphId );
             }
            else
             {
               mDotRep.addNode( vertexToString(&(*i)), getVertexName( (*i) ), "" );
             }

          if(debug)
             {
               std::cerr << " add node " << vertexToString(&(*i)) << " as " << getVertexName( (*i) ) << std::endl; // debug
             }
        }

  // create edges
     for (VertexIterator i=getVertices(); i!=getVerticesEnd(); i++)
        {
       // sort nodes? or problem of the user?

          if (debug)
               std::cerr << " add edge vertices ... " << std::endl; // debug

          EdgeIterator estart = getEdges(*i);
          EdgeIterator eend = getEdgesEnd(*i);

          for (EdgeIterator j=estart; j!=eend; j++)
             {
               EdgeType& e = *j;
               std::string label = getEdgeLabel( e );

            // AS(032006) Fixed it so that the corect edge is made. Earlier a call was made towards
            // mDotRep.addEdge( vertexToString(&(*i)),vertexToString(getTargetVertex(e)),"" ) which
            // would result in the second paramater implemented as a label and third as a node id. 
            // Therefore I decided to simplify the implementation
               mDotRep.addEdge( vertexToString(&(*i)), label, vertexToString(getTargetVertex(e)) , "");
               if (debug)
                    std::cerr << " add edge from " << vertexToString(&(*i)) << " to " << vertexToString(getTargetVertex(e)) << " as " << label << std::endl; // debug
#if 0
            // AS(032006) There is no way to implement this logic in the current implementation of DOTSubgraphRepresentation.
            // Also as a design-desition I think it would be better to create an edge without a label when the label is of length
            // zero in the DOTSubgraphRepresentation. From my opinion that is a better design.

               if (label.length()>0)
                  {
                 // insert with label
                    mDotRep.addEdge( vertexToString(&(*i)), label, vertexToString(getTargetVertex(e)) , "");
                    if(debug)
                         std::cerr << " add edge from " << vertexToString(&(*i)) << " to " << vertexToString(getTargetVertex(e)) << " as " << label << std::endl; // debug
                  }
                 else
                  {
                 // insert without label
                 // AS(032006) Fixed it so that the corect edge is made. Earlier a call was made towards
                 // mDotRep.addEdge( vertexToString(&(*i)),vertexToString(getTargetVertex(e)),"" ) which
                 // would result in the second paramater implemented as a label and third as a node id. 
                    mDotRep.addEdge( vertexToString(&(*i)),"", vertexToString(getTargetVertex(e)),"" );
                    if (debug)
                         std::cerr << " add edge from " << vertexToString(&(*i)) << " to " << vertexToString(getTargetVertex(e)) << " without label " << std::endl; // debug
                  }
#endif
               if (debug)
                    std::cerr << " edge added " << std::endl; // debug
             } // edges

          if (debug)
               std::cerr << " edge for vertex added " << std::endl; // debug
        } // nodes

     if (debug)
          std::cerr << " writing content to " << filename << std::endl; // debug

     mDotRep.writeToFileAsGraph( filename );

     if (debug)
          std::cerr << " output to " << filename << " done " << std::endl; // debug
   }

#endif
