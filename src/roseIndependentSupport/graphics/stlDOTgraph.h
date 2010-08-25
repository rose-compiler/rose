/******************************************************************************
 *
 * ROSE Global Database Connection
 *
 * Graph class from "STL Datastructures" Book, page 246, 11.1
 *
 *****************************************************************************/

#ifndef STLDOTGRAPH_H
#define STLDOTGRAPH_H

#include <map>
#include <vector>
#include <stack>
#include <iostream>
#include <fstream>

#include "DOTGraphInterface.h"
#include "stlgraph.h"

#define DOTEdgeType pair<int, EdgeType>
#define DOTVertexType pair<VertexType, map<int, EdgeType> >
#define DOTVertexIterator vector<DOTVertexType>::iterator       
#define DOTEdgeIterator map<int, EdgeType>::iterator    
#define DOTGraph DOTGraphInterface<DOTVertexType, DOTEdgeType, DOTVertexIterator, DOTEdgeIterator> 

//! stl graph class for directed and undirected graphs
template<class VertexType, class EdgeType>
class stlDOTGraph : 
        public stlGraph<VertexType,EdgeType>,
        public DOTGraph
{

        public:

                //! init constructor
                stlDOTGraph(bool d, ostream &os = cerr) :
                        stlGraph<VertexType,EdgeType>::stlGraph(d,os),
                        DOTGraph()      { };

                //! override stlGraph insert functions for nodenames and subgraphs
                int insertVertex(const VertexType& e, string name);
                int insertVertex(const VertexType& e, string name, int subgraphId, string subgraphName);
                
                //! override stlGraph insert functions for nodenames and subgraphs
                void insertEdge(const VertexType& e1, const VertexType& e2, const EdgeType& Value);

                //! write DOT graph to filename
                void _writeToDOTFile(string filename);
                
                //! write the adjacency matrix to a file (only integer matrix entries, raw file format)
                void writeAdjacencyMatrixToFileRaw(string filename);
                
                //! write the adjacency matrix to a file (in MCL raw file format for mcxassemble)
                void writeAdjacencyMatrixToFileMcl(string filename);
                
        protected:
        
                //! DOT representation class
                DOTSubgraphRepresentation< vertex* > mDot;
        
                //! store names for the nodes (based on graph vector index)
                map<int, string> mNodeNames;

                //! store subgraph names
                map<int, string> mSubgraphNames;

                //! store subgraph ids (also based on index) 
                map<int, int> mNodeSubgraphs;



        private:
                // implementation of the DOT output interface

                //! get an iterator for the vertices
                virtual DOTVertexIterator getVertices() { return begin(); }

                //! get the end of the vertices
                virtual DOTVertexIterator getVerticesEnd() { return end(); }

                //! get an iterator for all edges of a vertex
                virtual DOTEdgeIterator getEdges(DOTVertexType &v) { return v.second.begin(); }

                //! get the end of the edges
                virtual DOTEdgeIterator getEdgesEnd(DOTVertexType &v) { return v.second.end(); }

                //! get the name of a vertex
                virtual string getVertexName(DOTVertexType &v) { 
                        int index = -1;
                        for(int i=0;i<(int)size();i++) if(v.first==C[i].first) index=i;
                        if(index>=0) return mNodeNames[index];
                        return string(" "); 
                }

                //! get the target vertex of an edge, NOTE - this function returns a pointer to a vertex for use with vertexToString!
                virtual DOTVertexType *getTargetVertex(DOTEdgeType &e) { return &C[ e.first ]; }

                //! get the label of an edge
                virtual string getEdgeLabel(DOTEdgeType &e) { return string(""); }

                // get the direction of an edge is "target", as is the default

                //! get unique string representation of a vertex, NOTE - this function works with a pointer to the vertex!
                virtual string vertexToString(DOTVertexType *v) { 
                        ostringstream os;
                        //os << v->first.get_id(); 
                        os << ((unsigned int)v);
                        string s = os.str();
                        return s; 
                }

                //! query the subgraph ID of a node
                virtual int getVertexSubgraphId(DOTVertexType &v) {
                        int index = -1;
                        for(int i=0;i<(int)size();i++) if(v.first==C[i].first) index=i;
                        if(index>=0) return mNodeSubgraphs[index];
                        return -1;
                }

                //! get a list of the subgraph names, together with their ID's (see also getVertexSubgraphId)
                virtual map<int,string> getSubgraphList() {
                        return mSubgraphNames;
                }

};

//-----------------------------------------------------------------------------
template<class VertexType, class EdgeType>
int stlDOTGraph<VertexType, EdgeType>::insertVertex(const VertexType& e, string name) {
        int id = stlGraph<VertexType,EdgeType>::insertVertex(e);
        mNodeNames[ id ] = name;
        return id;
}
template<class VertexType, class EdgeType>
int stlDOTGraph<VertexType, EdgeType>::insertVertex(const VertexType& e, string name, int subgraphId, string subgraphName) {
        int id = stlGraph<VertexType,EdgeType>::insertVertex(e);
        mNodeNames[ id ] = name;
        mNodeSubgraphs[ id ] = subgraphId;
        mDot.addSubgraph(subgraphId, subgraphName);
        //assert( subgraphId >= 0 );
        //cerr << " new subgraph " << subgraphId <<" = "<< subgraphName << endl;
        mSubgraphNames[ subgraphId ] = subgraphName;
        return id;
}


//-----------------------------------------------------------------------------
template<class VertexType, class EdgeType>
void stlDOTGraph<VertexType, EdgeType>::insertEdge(
                const VertexType& e1, const VertexType& e2, const EdgeType& Value) {
        stlGraph<VertexType,EdgeType>::insertEdge(e1,e2,Value);
}



//-----------------------------------------------------------------------------
// deprecated...
template<class VertexType, class EdgeType>
void stlDOTGraph<VertexType, EdgeType>::_writeToDOTFile(string filename) {

        cerr << "stlDOTGraph Warning: USING OLD OUTPUT MECHANISM!!!" << endl;

        mDot.clear();

        // create all nodes
        for(size_t i=0; i<size(); i++) {
                if(mNodeNames.find(i) == mNodeNames.end() ) {
                        cerr << "stlDOTGraph Warning: uninitialized node name for node "<<i << endl;
                }
                mDot.addNode( &C[i], mNodeNames[ i ], "" , mNodeSubgraphs[ i ] );
                //cout << " mDot Node    "<< mNodes[i]->name <<endl; //debug
        }

        //still works???
        // create edges
        for(size_t i=0; i<size(); i++) {
                //FIXME sort( mNodes[i]->successors.begin(), mNodes[i]->successors.end() );

                // print edges only once with number?
                //vector<bool> edgePrinted( mNodes[i]->successors.size() );
                //for(unsigned int j=0; j< mNodes[i]->successors.size(); j++) edgePrinted[j] = false;

                //for(unsigned int j=0; j< mNodes[i]->successors.size(); j++) {
                for(Successor::iterator j= C[i].second.begin(); j!=C[i].second.end(); j++) {
                        //if( !edgePrinted[j] ) {
                                int multiplicity = 0;
                                //for(unsigned int l=0; l< mNodes[i]->successors.size(); l++) {
                                        //if( mNodes[i]->successors[l] == mNodes[i]->successors[j] ) {
                                                //multiplicity++;
                                                //edgePrinted[l] = true;
                                        //}
                                //}
                                multiplicity = 1; // FIXME

                                // add DOT edge
                                if(multiplicity == 1) {
                                        mDot.addEdge( &C[i], &C[ (*j).first ] );
                                } else {
                                        ostringstream multstr;
                                        multstr << multiplicity;
                                        mDot.addEdge( &C[i], multstr.str(), &C[ (*j).first ] , "");
                                }
                                //cout << " mDot Edge    "<< mNodes[i]->name <<" "<< mNodes[i]->successors[j]->name <<endl; //debug
                        //} // edge printed?
                } // edges

        } // nodes

        mDot.writeToFileAsGraph( filename+"2" );
        

        // DEBUG output
        /*for(size_t i=0;i<size();i++) {
                cout << " mDBG node "<< C[i].first.get_id() <<" " << mNodeNames[ C[i].first.get_id() ] << endl;
        }
        for(size_t i=0;i<size();i++) {
                for(Successor::iterator j=C[i].second.begin(); j!= C[i].second.end(); j++) {
                        cout << " mDBG edge from "<< i <<" to " << (*j).first << endl;
                }
        }*/
        // DEBUG output
}

//-----------------------------------------------------------------------------
// write the adjacency matrix to a file (only integer matrix entries, raw file format)
template<class VertexType, class EdgeType>
void stlDOTGraph<VertexType, EdgeType>::writeAdjacencyMatrixToFileRaw(string filename)
{
        int edges[ size() ];
        ofstream outFile;
        outFile.open(filename.c_str(), ios::out);
        //cout << " ADJMAT " << std::endl;
        
        // loop over all nodes
        for(size_t i=0; i<size(); i++) {
                // count edges to all other nodes
                for(size_t j=0; j<size(); j++) edges[j] = 0;
                for(Successor::iterator s= C[i].second.begin(); s!=C[i].second.end(); s++) {
                        edges[ (*s).first ]++;
                }

                for(size_t j=0; j<size(); j++) {                        
                        outFile << edges[j] << "\t";
                }
                outFile << std::endl;
                
        }

        //cout << " DEON " << std::endl;
        cout << "Wrote adjacency matrix to file " << filename <<", size: " << size()<<"x"<<size() << endl;
        outFile.close();
}
                

//-----------------------------------------------------------------------------
// write the adjacency matrix to a file (MCL)
template<class VertexType, class EdgeType>
void stlDOTGraph<VertexType, EdgeType>::writeAdjacencyMatrixToFileMcl(string filename)
{
        int edges[ size() ];
        ofstream outFile;
        outFile.open(filename.c_str(), ios::out);
        //cout << " ADJMAT " << std::endl;
        outFile << "(mclheader"<< endl <<
                "mcltype matrix" << endl <<
                "dimensions " << size()<<"x"<<size() << endl << 
                ")" << endl << endl <<
                "(mclmatrix " << endl << "begin " << endl ;
        
        // loop over all nodes
        for(size_t i=0; i<size(); i++) {
                outFile << i << "\t";

                // count edges to all other nodes
                for(size_t j=0; j<size(); j++) edges[j] = 0;
                for(Successor::iterator s= C[i].second.begin(); s!=C[i].second.end(); s++) {
                        edges[ (*s).first ]++;
                }

                for(size_t j=0; j<size(); j++) {                        
                        outFile << j<<":"<<edges[j] << "\t";
                }
                outFile <<" $"<< endl;
                
        }
        outFile <<" )" << endl;

        //cout << " DEON " << std::endl;
        cout << "Wrote adjacency matrix to file " << filename <<", size: " << size()<<"x"<<size() << endl;
        outFile.close();
}
                

#endif

