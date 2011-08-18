/*

FINISH TEMPFLATPATH CODE

*/




// Original Author (SgGraphTraversal mechanisms): Michael Hoffman
//$id$
#include<omp.h>
#include <boost/regex.hpp>
#include <iostream>
//#include "rose.h"
#include <fstream>
#include <string>
#include <assert.h>
#include <staticCFG.h>
//#include <graphTemplate.h>

/**
*@file graphProcessing.h

*Brief Overview of Algorithm:

***********************
*Current Implementation
***********************

*This implementation uses BOOSTs graph structure to analyze the paths of the graph and uses an InheritedAttribute solver

*The inheritedAttribute solver calculates a value for CFG nodes determined by its parents, the actual
value determined is specified by the user via evaluateInheritedAttribute

*The path analyzer sends the user paths to be evaluated by the "analyzePath" function that is user defined

**************************
*Further Improvements: TODO
**************************

@todo Parallelism, this algorithm is embarrassingly parallel

@todo utilize BOOST visitors to take advantage of the BOOST graph structures abilities

***************
*Contact Info
***************

*Finally, blame can be assigned to and questions can be forwarded to the author, though response is not guaranteed

*if I'm still at Lawrence
*hoffman34 AT llnl DOT gov
*@author Michael Hoffman
*/


//#include <rose.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/dominator_tree.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/transpose_graph.hpp>
#include <boost/algorithm/string.hpp>



//#include "staticCFG.h"
#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
//#include "graphBot.h"

//This is necessary for technical reasons with regards to the graphnodeinheritedmap



//using namespace Backstroke;


template <class CFG>
class SgGraphTraversal
{
public:
    typedef typename boost::graph_traits<CFG>::vertex_descriptor Vertex;
    typedef typename boost::graph_traits<CFG>:: edge_descriptor Edge;
    std::vector<int> getInEdges(int& node, CFG*& g);
    std::vector<int> getOutEdges(int& node, CFG*& g);
    //virtual void pathAnalyze(std::vector<Vertex>& path);
    void prepareGraph(CFG*& g);
    void findClosuresAndMarkersAndEnumerate(CFG*& g);
    //Vertex nullVertex;
    void constructPathAnalyzer(CFG* g, Vertex end=0, Vertex begin=0);
    virtual void analyzePath(std::vector<Vertex>& pth) = 0;
    void firstPrepGraph(CFG*& g);
    std::set<std::vector<int> >  traversePath(int begin, int end, CFG*& g, bool loop=false);
    void printCFGNode(int& cf, std::ofstream& o);
    void printCFGEdge(int& cf, CFG*& cfg, std::ofstream& o);
    void printHotness(CFG*& g);
    void computeOrder(CFG*& g, const int& begin);
    void computeSubGraphs(const int& begin, const int &end, CFG*& g, int depthDifferential);
    int getTarget(int& n, CFG*& g);
    int getSource(int& n, CFG*& g);
    std::vector<int> sources;
    std::vector<int> sinks;
    std::map<int, std::set<std::vector<int> > > globalLoopPaths;
    std::map<int, std::set<std::vector<int> > > globalEndLoopPaths;
    std::vector<int>  orderOfNodes;
    std::map<Vertex, int> vertintmap;
    std::map<Edge, int> edgeintmap;
    std::map<int, Vertex> intvertmap;
    std::map<int, Edge> intedgemap;
    std::vector<std::map<Vertex, Vertex> > SubGraphGraphMap;
    std::vector<std::map<Vertex, Vertex> > GraphSubGraphMap;
    std::vector<CFG*> subGraphVector;
    void getVertexPath(std::vector<int> path, CFG*& g, std::vector<Vertex>& vertexPath );
    void storeCompact(std::vector<int> path);
    int nextNode;
    int nextEdge;
    std::vector<int> markers;
    std::vector<int> closures;
    std::map<int, int> markerIndex;
    std::map<int, std::vector<int> > pathsAtMarkers;
    typedef typename boost::graph_traits<CFG>::vertex_iterator vertex_iterator;
    typedef typename boost::graph_traits<CFG>::out_edge_iterator out_edge_iterator;
    typedef typename boost::graph_traits<CFG>::in_edge_iterator in_edge_iterator;
    typedef typename boost::graph_traits<CFG>::edge_iterator edge_iterator;
    SgGraphTraversal();
    virtual ~SgGraphTraversal();
   SgGraphTraversal( SgGraphTraversal &);
    SgGraphTraversal &operator=( SgGraphTraversal &);


};
/**
Gets in edges with integer inputs, internal use only
SgGraphTraversal::getInEdges
Input:
@param[node] const int, integer representation of the node to get the in edges from
@param[g] const CFG* g, CFG
*/



template<class CFG>
SgGraphTraversal<CFG>::
SgGraphTraversal()
{
}



template<class CFG>
SgGraphTraversal<CFG> &
SgGraphTraversal<CFG>::
operator=( SgGraphTraversal &other)
{
    return *this;
}

#ifndef SWIG

template<class CFG>
SgGraphTraversal<CFG>::
~SgGraphTraversal()
{
}

#endif

template<class CFG>
inline int
SgGraphTraversal<CFG>::
getSource(int& edge, CFG*& g)
{
    Edge e = intedgemap[edge];
    Vertex v = boost::source(e, *g);
    return(vertintmap[v]);
}

template<class CFG>
inline int
SgGraphTraversal<CFG>::
getTarget(int& edge, CFG*& g)
{
    Edge e = intedgemap[edge];
    Vertex v = boost::target(e, *g);
    return(vertintmap[v]);
}

/**
Gets out edges with integer inputs, internal use only
SgGraphTraversal::getInEdges
Input:
@param[node] const int, integer representation of the node to get the in edges from
@param[g] const CFG* g, CFG
*/

template<class CFG>
std::vector<int>
SgGraphTraversal<CFG>::
getInEdges(int& node, CFG*& g)
{
    Vertex getIns = intvertmap[node];
    std::vector<int> inedges;
    in_edge_iterator i, j;
    for (boost::tie(i, j) = boost::in_edges(getIns, *g); i != j; ++i)
    {
        inedges.push_back(edgeintmap[*i]);
    }
    return inedges;
}

/**
Gets out edges with integer inputs, internal use only
SgGraphTraversal::getOutEdges
Input:
@param[node] const int, integer representation of the node to get the out edges from
@param[g] const CFG* g, CFG
*/



template<class CFG>
std::vector<int>
SgGraphTraversal<CFG>::
getOutEdges(int &node, CFG*& g)
{
    Vertex getOuts = intvertmap[node];
    std::vector<int> outedges;
    out_edge_iterator i, j;
    for (boost::tie(i, j) = boost::out_edges(getOuts, *g); i != j; ++i)
    {
        outedges.push_back(edgeintmap[*i]);
    }
    return outedges;
}


/**
This is the function that is used by the user directly to start the algorithm. It is immediately available to the user

SgGraphTraversal::constructPathAnalyzer
Input:
@param[begin] Vertex, starting node
@param[end] Vertex, endnode
@param[g] CFG* g, CFG calculated previously
@param[keep] bool, true if you want to keep the data available (for multiple analyses)
@param[subgraph] bool, if you want parallel computation 
*/


template<class CFG>
void
SgGraphTraversal<CFG>::
constructPathAnalyzer(CFG* g, Vertex begin, Vertex end) {
    //sets up a few data structures to ease computations in future functions
    prepareGraph(g);
    printHotness(g);
    // if you don't want to calculate in parallel
    bool subgraph = false;
    if (!subgraph) {
        //run traverse on the whole graph
            if (end && begin) {
                traversePath(vertintmap[begin],vertintmap[end], g);
            }
        //if begin and end are not specified, run from all sources to one end node (equivalently run to all end nodes)
            else {
                for (unsigned int j = 0; j < sources.size(); j++) {
                    traversePath(sources[j], -1, g);
                }
           }
    }
    else {
        // construct SubGraphs to allow for parallel traversal
        // this operation is not cheap
        computeSubGraphs(begin, end, g, 10);
        //#pragma omp parallel for 
        for (unsigned int i = 0; i < subGraphVector.size(); i++) {
            //we check all sources. checking all ends is not helpful, as it makes things
            //much more complicated and we can always run to the end
            for (unsigned int j = 0; j < sources.size(); j++) {
                traversePath(sources[j], vertintmap[end], subGraphVector[i]);
            }
         }
    }
}

/** This is a function to construct subgraphs for parallelization
SgGraphTraversal::computeSubGraphs
Input:
@param[begin] const int, starting point
@param[end] const int ending point
@param[g] const CFG*, control flow graph to compute
@param[depthDifferential] int, used to specify how large the subgraph should be
 */

template<class CFG>
void
SgGraphTraversal<CFG>::
computeSubGraphs(const int& begin, const int &end, CFG*& g, int depthDifferential) {
        //Nodes are ordered so that every nodes order value is 1 more than the greatest order value among its parents
        //minDepth is 1 so that orderOfNodes[i-1] in the for loop below exists
        int minDepth = 0;
        //depthDifferential determines how far to descend
        int maxDepth = minDepth + depthDifferential;
        //many subgraphs will be created, currSubGraph indexes them in subGraphVector (variable in SgGraphTraversal)
        int currSubGraph = 0;
        //instantiating the new graph
        CFG* subGraph;
        std::set<int> foundNodes;
        //adding the graph to the vector of graphs
        while (true) {
            Vertex begin = boost::add_vertex(*subGraphVector[currSubGraph]);
            GraphSubGraphMap[currSubGraph][intvertmap[orderOfNodes[minDepth]]] = intvertmap[begin];
            SubGraphGraphMap[currSubGraph][intvertmap[begin]] = intvertmap[orderOfNodes[minDepth]];
        //this will eventually got through all nodes, each node has a unique depth
        for (int i = minDepth; i <= maxDepth; i++) {
                //getting the SubGraph node with respect to particular subgraph and corresponding original CFG node
            Vertex v = GraphSubGraphMap[currSubGraph][intvertmap[orderOfNodes[i]]];
            //We get the outEdges to construct all necessary edges, these will not duplicate
            std::vector<int> outEdges = getOutEdges(orderOfNodes[i], g);
            for (unsigned int j = 0; j < outEdges.size(); j++) {
                Vertex u;
                //we have to check to make sure we haven't already instantiated this node
                if (foundNodes.find(getTarget(outEdges[j], g)) == foundNodes.end()) {
                        u = GraphSubGraphMap[currSubGraph][intvertmap[getTarget(outEdges[j], g)]];
                }
                //otherwise add a new vertex
                else {
                    u = boost::add_vertex(*subGraphVector[currSubGraph]);
                    foundNodes.insert(getTarget(outEdges[j], g));
                    SubGraphGraphMap[currSubGraph][u] = intvertmap[getTarget(outEdges[j], g)];
                    GraphSubGraphMap[currSubGraph][intvertmap[getTarget(outEdges[j], g)]] = u;

                }
                //adding edges, using this method we cannot hit the same edge twice
                Edge edge;
                bool ok;
                boost::tie(edge, ok) = boost::add_edge(v,u,*subGraphVector[currSubGraph]);
            }
        }
        minDepth = maxDepth;
        if ((unsigned int) minDepth == orderOfNodes.size()-1) {
                break;
        }
        maxDepth += depthDifferential;
        if ((unsigned int) maxDepth > orderOfNodes.size()-1)
        {
                maxDepth = orderOfNodes.size()-1;
        }
        CFG* newSubGraph;
        subGraphVector.push_back(newSubGraph);
        currSubGraph++;
        }
        return;
}


/**
This is the function that preps the graph for traversal, can be used in the users program directly
but this requires 

SgGraphTraversal::prepareGraph
Input:
@param[begin] Vertex, starting node
@param[end] Vertex, endnode 
@param[g] CFG*& g, CFG calculated previously
*/


        template<class CFG>
        void
        SgGraphTraversal<CFG>::
        printCFGNode(int& cf, std::ofstream& o)
        {
            std::string nodeColor = "black";
            o << cf << " [label=\"" << " num:" << cf << "\", color=\"" << nodeColor << "\", style=\"" << "solid" << "\"];\n";
        }

        template<class CFG>
        void
        SgGraphTraversal<CFG>::
        printCFGEdge(int& cf, CFG*& cfg, std::ofstream& o)
        {
            int src = getSource(cf, cfg);
            int tar = getTarget(cf, cfg);
            o << src << " -> " << tar << " [label=\"" << src << " " << tar << "\", style=\"" << "solid" << "\"];\n";
        }

        template<class CFG>
        void
        SgGraphTraversal<CFG>::
        printHotness(CFG*& g)
        {
            const CFG* gc = g;
            int currhot = 0;
            std::ofstream mf;
            std::stringstream filenam;
            filenam << "hotness" << currhot << ".dot";
            currhot++;
            std::string fn = filenam.str();
            mf.open(fn.c_str());

            mf << "digraph defaultName { \n";
            vertex_iterator v, vend;
            edge_iterator e, eend;
            for (tie(v, vend) = vertices(*gc); v != vend; ++v)
            {
                printCFGNode(vertintmap[*v], mf);
            }
            for (tie(e, eend) = edges(*gc); e != eend; ++e)
            {
                printCFGEdge(edgeintmap[*e], g, mf);
            }
            mf.close();
        }


template<class CFG>
void
SgGraphTraversal<CFG>::
prepareGraph(CFG*& g) {
    nextNode = 1;
    nextEdge = 1;
    findClosuresAndMarkersAndEnumerate(g);
   // computeOrder(g, sources[0]);
}


template<class CFG>
void
SgGraphTraversal<CFG>::
firstPrepGraph(CFG*& g) {
    nextNode = 1;
    nextEdge = 1;
    findClosuresAndMarkersAndEnumerate(g);
    //computeOrder(g, sources[0]);
}

/**
This calculates nodes with more than one in edge or more than one out edge

SgGraphTraversal::findClosuresAndMarkers
Input:
@param[g] CFG*& g, CFG calculated previously
*/


//internal use only
template<class CFG>
void
SgGraphTraversal<CFG>::
findClosuresAndMarkersAndEnumerate(CFG*& g)
{
    edge_iterator e, eend;
    for (tie(e, eend) = edges(*g); e != eend; ++e) {
        intedgemap[nextEdge] = *e;
        edgeintmap[*e] = nextEdge;
        nextEdge++;
    }
    vertex_iterator v1, vend1;
    for (tie(v1, vend1) = vertices(*g); v1 != vend1; ++v1)
    {
        vertintmap[*v1] = nextNode;
        intvertmap[nextNode] = *v1;
        nextNode++;
    }
    vertex_iterator v, vend;
    for (tie(v, vend) = vertices(*g); v != vend; ++v) {
        std::vector<int> outs = getOutEdges(vertintmap[*v], g);
        std::vector<int> ins = getInEdges(vertintmap[*v], g);
        if (outs.size() > 1)
        {
            //vertintmap transforms the vertex input to integer representation, constructed in enumerate
            markers.push_back(vertintmap[*v]);
            markerIndex[vertintmap[*v]] = markers.size()-1;
            for (unsigned int i = 0; i < outs.size(); i++) {
                pathsAtMarkers[vertintmap[*v]].push_back(getTarget(outs[i], g));
            }
        }
        if (ins.size() > 1)
        {
            //vertintmap transforms the vertex input to integer representation, constructed in enumerate
            closures.push_back(vertintmap[*v]);
        }
        if (outs.size() == 0) {
            sinks.push_back(vertintmap[*v]);
        }
        if (ins.size() == 0) {
            sources.push_back(vertintmap[*v]);
        }
    }
    return;
}

template<class CFG>
void
SgGraphTraversal<CFG>::
computeOrder(CFG*& g, const int& begin) {
        std::vector<int> currentNodes;
        std::vector<int> newCurrentNodes;
        currentNodes.push_back(begin);
        std::map<int, int> reverseCurrents;
        orderOfNodes.push_back(begin);
        std::set<int> heldBackNodes;
        while (currentNodes.size() != 0) {
                for (unsigned int j = 0; j < currentNodes.size(); j++) {
                        //std::cout << "currentNodes[j]: " << currentNodes[j] << std::endl;
                       
                        std::vector<int> inEdges = getInEdges(currentNodes[j], g);
                        if (inEdges.size() > 1) {
                        if (reverseCurrents.find(currentNodes[j]) == reverseCurrents.end()) {
                            reverseCurrents[currentNodes[j]] = 0;
                        }
                        if ((unsigned int) reverseCurrents[currentNodes[j]] == inEdges.size() - 1) {
                                heldBackNodes.erase(currentNodes[j]);
                                reverseCurrents[currentNodes[j]]++;
                                std::vector<int> outEdges = getOutEdges(currentNodes[j], g);
                                for (unsigned int k = 0; k < outEdges.size(); k++) {
                                      //  std::cout << "outEdges[k]: " << outEdges[k] << std::endl;
                                        newCurrentNodes.push_back(getTarget(outEdges[k], g));
                                        orderOfNodes.push_back(getTarget(outEdges[k], g));
                                }
                        }
                        else if (reverseCurrents[currentNodes[j]] < reverseCurrents.size()) {
                                reverseCurrents[currentNodes[j]]++;
                                if (heldBackNodes.find(currentNodes[j]) == heldBackNodes.end()) {
                                    heldBackNodes.insert(currentNodes[j]);
                                }
                        }
                        }
                        else {
                                std::vector<int> outEdges = getOutEdges(currentNodes[j], g);
                                for (unsigned int k = 0; k < outEdges.size(); k++) {
                                      //  std::cout << "outEdges[k]: " << outEdges[k] << std::endl;
                                        newCurrentNodes.push_back(getTarget(outEdges[k], g));
                                        orderOfNodes.push_back(getTarget(outEdges[k], g));

                                }
                        }
                }
                if (newCurrentNodes.size() == 0 && heldBackNodes.size() != 0) {
                    for (std::set<int>::iterator q = heldBackNodes.begin(); q != heldBackNodes.end(); q++) {
                        int qint = *q;
                        std::vector<int> heldBackOutEdges = getOutEdges(qint, g);
                        for (unsigned int p = 0; p < heldBackOutEdges.size(); p++) {
                            newCurrentNodes.push_back(getTarget(heldBackOutEdges[p], g));
                        }
                   }
                   heldBackNodes.clear();
                }
                currentNodes = newCurrentNodes;
                newCurrentNodes.clear();
        }
        return;
}

template<class CFG>
void
SgGraphTraversal<CFG>::
getVertexPath(std::vector<int> path, CFG*& g, std::vector<Vertex>& vertexPath) {
        //std::cout << "path size: " << path.size() << std::endl;
        for (unsigned int i = 0; i < path.size(); i++) {
                            vertexPath.push_back(intvertmap[path[i]]);
        }

       
       //std::cout << std::endl;
        
}

template<class CFG>
void
SgGraphTraversal<CFG>::
storeCompact(std::vector<int> compactPath) {
return;
}





template<class CFG>
std::set<std::vector<int> > 
SgGraphTraversal<CFG>::
traversePath(int begin, int end, CFG*& g, bool loop) {
        //std::cout << "begin: " << begin << std::endl;
        //std::cout << "end: " << end << std::endl;
    //setting the current node variable and grabbing edges
    int pathcount = 0;
    std::vector<int> path;
    path.push_back(begin);
    //std::map<int, std::set<std::vector<int> > > globalLoopPaths;
    std::map<int, int> currents;
    std::set<std::vector<int> > loopPaths;
    int rounds = 0;
/* this loop should go down the graph until it hits an endnode or the specified endnode
or a node with a greater max depth than is allowed (that is, the number that says if all loops
traversed maximally once what is the greatest number of steps taken to reach the desired node */
        while (path.size() != 0) {
            rounds++;
            if (rounds % 10000 == 0) {
            std::cout << "round: " << rounds << std::endl;
            }
                        //std::cout << "path.back().first: " << path.back().first << std::endl;  
                        //std::cout << "path.back().second: " << path.back().second << std::endl;
            //if (path.back() == end && end != -1) {
            //std::cout << "partial path: " << std::endl;
            //for (unsigned int j = 0; j < path.size(); j++) {
            //          std::cout << path[j] << ", ";
           // }
            
           // std::cout << std::endl;
            
            //std::cout << "path.size(): " << path.size() << std::endl;
            std::vector<int> outEdges = getOutEdges(path.back(), g);
            if (outEdges.size() == 0 || (path.back() == end && path.size() != 1 && end != -1)) {
                std::vector<std::vector<int> > nPaths;
                std::vector<int> subpath;
                subpath.push_back(path[0]);
                nPaths.push_back(subpath);
                std::vector<std::vector<int> > newNPaths;
                    for (unsigned int q = 0; q < path.size(); q++) {
                    if (globalLoopPaths.find(path[q]) != globalLoopPaths.end() && (path[q] != begin || begin != end)) {
                    //subpath.insert(subpath.end(), path.begin(), path.begin()+q);
                    //ROSE_ASSERT(*(path.begin()+q) == path[q]);
                    //std::cout << "nPaths.size(): " << nPaths.size() << std::endl;
                    for (unsigned int qN = 0; qN < nPaths.size(); qN++) {
                    std::vector<int> newPath = nPaths[qN];
                    //for (unsigned int q = 0; q < path.size(); q++) {
                            std::set<std::vector<int> > qLoops = globalLoopPaths[path[q]];
                            for (std::set<std::vector<int> >::iterator qL = qLoops.begin(); qL != qLoops.end(); qL++) {
                                std::vector<int> tmpPath = newPath;
                                for (unsigned int k2 = 0; k2 < (*qL).size(); k2++) {  
                                    tmpPath.push_back((*qL)[k2]);
                                }
                                newNPaths.push_back(tmpPath);
                            }
                            std::vector<int> tmpNoLoopPath = newPath;
                            tmpNoLoopPath.push_back(path[q]);
                            newNPaths.push_back(tmpNoLoopPath); 
                            std::vector<int> newPEnd = nPaths[qN];
                            std::set<std::vector<int> > qEndLoops = globalEndLoopPaths[path[q]];
                            ROSE_ASSERT(qEndLoops.size() == 0);
                            if (qEndLoops.size() != 0) {
                               for (std::set<std::vector<int> >::iterator qE = qEndLoops.begin(); qE != qEndLoops.end(); qE++) {
                                  std::vector<int> tmpPEnd = newPEnd;
                                  //if ((*qE).back() == end || end == -1) {
                                  for (unsigned int k4 = 0; k4 < (*qE).size(); k4++) {
                                      tmpPEnd.push_back((*qE)[k4]);
                                  }
                                  //std::cout << "endLoop path: " << std::endl;
                                  //for (int i = 0; i < tmpPEnd.size(); i++) {
                                  //    std::cout << tmpPEnd[i] << ", ";
                                 // }
                                  //std::cout << std::endl;
                                  std::vector<Vertex> vertexPath;
                                  getVertexPath(tmpPEnd, g, vertexPath);
                                  //std::cout << "endPath: " << std::endl;
                                  //for (int i1 = 0; i1 < tmpPEnd.size(); i1++) {
                                  //   std::cout << tmpPEnd[i1] << ", ";
                                 // }
                                  //std::cout << std::endl;
                                  analyzePath(vertexPath);
                                  tmpPEnd.clear();
                                  }
                              }
                          
                          
                                  
                             
                        
                        }
                        nPaths = newNPaths;
                        newNPaths.clear();
                   }
                   else if (q == 0) {
                   }
                   else {
                   for (unsigned int q3 = 0; q3 < nPaths.size(); q3++) {
                           std::vector<int> tmpNewPath = nPaths[q3];
                           tmpNewPath.push_back(path[q]);
                           newNPaths.push_back(tmpNewPath);
                    }
                    nPaths = newNPaths;
                    newNPaths.clear();
                    }
                    }
                    for (unsigned int nP = 0; nP < nPaths.size(); nP++) {
                        if (!loop) {
                         std::cout << "assumed path: " << std::endl;
                        for (unsigned int k5 = 0; k5 < nPaths[nP].size(); k5++) {
                            std::cout << nPaths[nP][k5] << ", ";
                        }
                        std::cout << std::endl;
                        std::vector<Vertex> vertPath;
                        getVertexPath(nPaths[nP], g, vertPath);
                        //std::cout << "vertPath size: " << vertPath.size() << std::endl; 
                        analyzePath(vertPath);
                       // std::cout << "pathcount: " << pathcount << std::endl;
                        pathcount++;
                        }
                        else {
                            std::vector<int> tmpNp;
                           // std::cout << "loop: " << std::endl;
                            //for (int k6 = 0; k6 < nPaths[nP].size(); k6++) {
                             //   std::cout << nPaths[nP][k6] << ", ";
                                //tmpNp.push_back(nPaths[nP][k6]);
                            //}
                            //std::cout << std::endl;
                            if (nPaths[nP].back() == end && nPaths[nP].size() > 1) {
                                if (loopPaths.find(nPaths[nP]) == loopPaths.end()) {
                                    loopPaths.insert(nPaths[nP]);
                                }
                             //   std::cout << "loopPaths.size(): " << loopPaths.size() << std::endl;
                            }
                            //else {
                                //std::cout << "nonloop path found: " << std::endl;
                                //for (int i = 0; i < nPaths[nP].size(); i++) {
                                //    std::cout << nPaths[nP][i] << ", ";
                               // }
                                //loopPaths.insert(nPaths[nP]);
                                //std::cout << std::endl;
                          // }
                        }
                    }
                    if (loop) {
                    //std::cout << "loopPaths.size(): " << loopPaths.size() << std::endl;
                    }
                    nPaths.clear();
                    path.pop_back();
                    if (path.size() == 0) {
                        return loopPaths;
                    }
                    std::vector<int> oeds = getOutEdges(path.back(), g);
                    while ((unsigned int) (currents[path.back()] >= oeds.size() && path.size() != 0) || find(path.begin(), path.end(), getTarget(oeds[currents[path.back()]], g)) != path.end()) {
                        if ((unsigned int) currents[path.back()] >= oeds.size()) {
                        // if (find(path.begin(), path.end(), getTarget(oeds[currents[path.back()]], g)) == path.end()) {
                        currents[path.back()] = 0;
                        path.pop_back();
                        oeds = getOutEdges(path.back(), g);
                        }
                        else {
                             //currents[path.back()]++;
                             path.pop_back();
                             oeds = getOutEdges(path.back(), g);
                         }
                   }
                        
                     
                    
                    if (path.size() == 0) {
                        return loopPaths;
                    }
                    int oldBack = path.back();
                    path.push_back(getTarget(oeds[currents[path.back()]], g));
                    currents[oldBack]++;
           }
             //else if (loop && path.back() == end && path.size() == 1) {
             //    return loopPaths;
            // }
             else {
                // std::vector<int> outEdges = getOutEdges(path.back(), g);
                 //if (outEdges.size() == 1) {
                 //    path.push_back(getTarget(outEdges[currents[path.back()]], g));
                 //    currents[path.back()]++;
                // }
                // else {
                 std::vector<int> outEdges = getOutEdges(path.back(), g);
                 if (currents.find(path.back()) == currents.end()) {
                     currents[path.back()] = 0;
                 }
                 if ((unsigned int) currents[path.back()] < outEdges.size()) { 
                 int currn = getTarget(outEdges[currents[path.back()]], g);
                 //std::vector<int> ieds = getInEdges(currn, g);
                 //if (ieds.size() <= 1) {
                 //    currents[path.back()]++;
                 //    path.push_back(currn);
                // }
                 if (find(path.begin(), path.end(), currn) == path.end() || (currn == begin && currn == end)) {
                     currents[path.back()]++;
                     path.push_back(currn);
  
                 }
                 else {
                     //std::cout << "looped currn: " << currn << std::endl;
                     currents[path.back()]++;
                     if (globalLoopPaths.find(currn) == globalLoopPaths.end() && (currn != begin)) {
                         std::set<std::vector<int> > tmplps;
                         globalLoopPaths[currn] = tmplps;
                         std::set<std::vector<int> > lps = traversePath(currn, currn, g, true);
                         //currents[currn]++;
                         std::vector<int> ieds = getInEdges(currn, g);
                         //ROSE_ASSERT(ieds.size() > 1);
                         for (std::set<std::vector<int> >::iterator i = lps.begin(); i != lps.end(); i++) {
                             if ((*i).back() == currn) {
                                 globalLoopPaths[currn].insert(*i);
                             }
                             else {
                                 //globalEndLoopPaths[currn].insert(*i);
                             }
                         }
                     }
                     //ROSE_ASSERT(path.back() == currn);
                     std::vector<int> ods2 = getOutEdges(path.back(), g);
                     if ((unsigned int) currents[path.back()] >= ods2.size()) {
                         while ((unsigned int) currents[path.back()] >= ods2.size() && path.size() != 0) {
                             currents[path.back()] = 0;
                             path.pop_back();
                             ods2 = getOutEdges(path.back(), g);
                         }
                     
                     if (path.size() == 0) {
                         return loopPaths;
                     }
                     else {
                         int oldback = path.back();
                         path.push_back(getTarget(ods2[currents[path.back()]], g));
                         currents[oldback]++;
                     }
               
               }
               else {
                  int oldback = path.back();
                  std::vector<int> ods2 = getOutEdges(path.back(), g);
                  path.push_back(getTarget(ods2[currents[path.back()]], g));
                  currents[oldback]++;
              
              }
              }
           }
           else {
               std::vector<int> qds = getOutEdges(path.back(), g);
               while ((unsigned int)currents[path.back()] >= qds.size() && path.size() != 0) {
                   currents[path.back()] = 0;
                   path.pop_back();
                   qds = getOutEdges(path.back(), g);
               }
               if (path.size() == 0) {
                   return loopPaths;
               }
               qds = getOutEdges(path.back(), g);
               int oldback = path.back();
               path.push_back(getTarget(qds[currents[path.back()]], g));
               currents[oldback]++;
          }
     //}
     }
     }
     return loopPaths;
}
                  
