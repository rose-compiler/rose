/******************************************************************************
 *
 * ROSE Global Database Connection
 *
 * Graph class from "STL Datastructures" Book, page 246, 11.1
 *
 *****************************************************************************/

#ifndef STLGRAPH_H
#define STLGRAPH_H

#include <map>
#include <vector>
#include <stack>
#include <iostream>

//! stl graph class for directed and undirected graphs
template<class VertexType, class EdgeType>
class stlGraph {

        public:

                //! public type interface
                typedef map<int, EdgeType> Successor;
                typedef pair<VertexType, Successor> vertex;
                typedef vector<vertex> GraphType;
                typedef GraphType::iterator iterator;
                typedef GraphType::const_iterator const_iterator;
                typedef enum {unknown, weak, strong} ConnectivityType;
                typedef enum {notVisited, visited, processed} vertexStatus;

                //! init constructor
                stlGraph(bool d, ostream &os = cerr) :
                        directed( d ), pOut( &os ), connectivity( unknown ), cycles(-1), components(-1) { };

                //! access vertex i
                vertex& operator[](int i) {
                        // check i for debugging
                        return C[i];
                };

                //! insert a vertex
                int insertVertex(const VertexType& e);
                
                //! insert an edge between e1 and e2
                void insertEdge(const VertexType& e1, const VertexType& e2, const EdgeType& Value);

                //! add an edge between e1 and e2
                void connectVertices(int i,int j, const EdgeType& Value);

                
                // checking and output functions

                //! output graph information
                void check(ostream& = cout);

                //! determine number of edges
                size_t countEdges();

                //! determine connectivity and cycles
                void cyclesAndConnectivity();

                
                // access functions

                //! size of the vertex vector
                size_t size() const { return C.size(); };

                //! begin iterator
                iterator begin() { return C.begin(); };
                //! end iterator
                iterator end() { return C.end(); };
                
                //! is it a directed graph?
                bool isDirected() const { return directed; };

                //! returns number the connectivity of the graph
                int getConnectivity() const { return connectivity; }

                //! returns number of cycles (-1 if unknown)
                int getCycles() const { return cycles; }

                //! returns number of components (-1 if unknown)
                int getComponents() const { return components; }
                
        protected:
                //! directed graph?
                bool directed;

                //! graph container
                GraphType C;

                //! debug output
                ostream *pOut;
                
                //! connectivity of the graph (determined by cyclesAndConnectivity)
                ConnectivityType connectivity;

                //! no. of cycles (determined by cyclesAndConnectivity)
                int cycles;

                //! no. of components (determined by cyclesAndConnectivity)
                int components;
};


//-----------------------------------------------------------------------------
template<class VertexType, class EdgeType>
int stlGraph<VertexType, EdgeType>::insertVertex(const VertexType& e) {
        for(size_t i=0;i<size(); ++i) {
                if(e == C[i].first) return i;
        }
        // not found -> insert
        C.push_back( vertex(e, Successor()) );
        return size()-1;
}


//-----------------------------------------------------------------------------
template<class VertexType, class EdgeType>
void stlGraph<VertexType, EdgeType>::connectVertices(int pos1, int pos2, const EdgeType& Value) {
        (C[pos1].second)[pos2] = Value;

        // automatically insert other direction for undirected graphs
        if(!directed) (C[pos2].second)[pos1] = Value;
}


//-----------------------------------------------------------------------------
template<class VertexType, class EdgeType>
void stlGraph<VertexType, EdgeType>::insertEdge(
                const VertexType& e1, const VertexType& e2, const EdgeType& Value) {
        //cout << " mDBG new edge from " << e1.get_id() <<" " << " to " << e2.get_id() <<" " << endl; // debug
        int pos1 = insertVertex(e1);
        int pos2 = insertVertex(e2);
        connectVertices(pos1,pos2, Value);
}


//-----------------------------------------------------------------------------
template<class VertexType, class EdgeType>
void stlGraph<VertexType, EdgeType>::check(ostream& os) {
        os << "The graph is ";
        if(!isDirected()) os << "un";

        os << "directed and has " <<
                size() << " vertices and " <<
                countEdges() << " edges \n";
        
        cyclesAndConnectivity();
        if(isDirected()) {
                if(components == 1) 
                        os << "The graph is strongly connected.\n";
                else
                        os << "The graph is not or weakly connected.\n";
        } else {
                os << "The graph has " << components << " component(s). \n";
        }

        os << "It has ";
        if(cycles == 0) os << "no ";
        else os << cycles;
        os << "cycles.\n";
}


//-----------------------------------------------------------------------------
//! add length of all adjacency lists
template<class VertexType, class EdgeType>
size_t stlGraph<VertexType, EdgeType>::countEdges() {
        size_t edges = 0;
        iterator temp = begin();

        while( temp != end() ) {
                edges += (*temp).second.size();
                temp++;
        }

        if(!isDirected()) edges /= 2;
        return edges;
}


//-----------------------------------------------------------------------------
//! traverse graph to check for cycles and components
template<class VertexType, class EdgeType>
void stlGraph<VertexType, EdgeType>::cyclesAndConnectivity() {
        int componentNumber = 0;
        cycles = 0;

        // vertices to be visited 
        stack<int, vector<int> > verticesStack; 
        
        // prevent multiple visits, assign not visited to all vertices
        vector<vertexStatus> vertexState(size(), notVisited);

        // try to reach all other vertices starting from one
        for( size_t i=0; i<size(); ++i) {
                if(vertexState[i] == notVisited) {
                        componentNumber++;
                        // store on stack for further processing
                        verticesStack.push(i);

                        // process stack
                        while(!verticesStack.empty()) {
                                int vert = verticesStack.top();
                                verticesStack.pop();
                                if(vertexState[vert] == visited) {
                                        vertexState[vert] = processed;
                                } else {

                                        if(vertexState[vert] == notVisited) {
                                                vertexState[vert] = visited;

                                                // new vertex, earmark for processed mark
                                                verticesStack.push(vert);

                                                // if one of the new vertices has the processed flag we found a cycle
                                                // earmark successors
                                                Successor::iterator start = operator[](vert).second.begin();
                                                Successor::iterator end   = operator[](vert).second.end();
                                                while(start != end) {
                                                        int succ = (*start).first;
                                                        if(vertexState[succ] == visited) {
                                                                // a cycle...
                                                                ++cycles;
                                                                //(*pOut) << " at least vertex " << operator[](succ).first << " lies in a cycle \n";
                                                        }

                                                        // otherwise vertex was already processed or wasnt visited and is earmarked on the stack
                                                        if(vertexState[succ] == notVisited)
                                                                verticesStack.push(succ);

                                                        ++start;
                                                }
                                        } // not visited

                                }
                        } // stack not empty
                        
                } // not visited
        }

        if(isDirected()) {
                if(componentNumber == 1) connectivity = strong;
                else connectivity = weak;
        } else {
                connectivity = unknown;
        }
        components = componentNumber;
}


//-----------------------------------------------------------------------------
//! 
//template<class VertexType, class EdgeType>
//stlGraph<VertexType, EdgeType>::() {
//}


#endif

