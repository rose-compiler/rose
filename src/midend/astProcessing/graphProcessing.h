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

*This implementation uses BOOSTs graph structure to analyze the paths of the graph

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
    void constructPathAnalyzer(CFG* g, bool unbounded=false, Vertex end=0, Vertex begin=0);
    virtual void analyzePath(std::vector<Vertex>& pth) = 0;
    void firstPrepGraph(CFG*& g);
    int stoppedpaths;
    std::set<std::vector<int> >  traversePath(int begin, int end, CFG*& g, bool loop=false);
    std::set<std::vector<int> > uTraversePath(int begin, int end, CFG*& g, bool loop=false);
    void printCFGNode(int& cf, std::ofstream& o);
    void printCFGNodeGeneric(int& cf, std::string prop, std::ofstream& o);
    void printCFGEdge(int& cf, CFG*& cfg, std::ofstream& o);
    void printHotness(CFG*& g);
    void printPathDot(CFG*& g);
    void computeOrder(CFG*& g, const int& begin);
    void computeSubGraphs(const int& begin, const int &end, CFG*& g, int depthDifferential);
    int getTarget(int& n, CFG*& g);
    int getSource(int& n, CFG*& g);
    std::vector<int> sources;
    std::vector<int> sinks;
    std::vector<int>  recursiveLoops; //globalLoopPaths;
    std::set<int> badloop;
    //std::map<int, std::vector<std::vector<int> > > globalLoopPaths;
    std::map<int, std::vector<std::vector<int> > >  totalLoops;
    int pathnum;
    std::map<int, std::string> nodeStrings;
    int sourcenum;
    int evaledpaths;
    int workingthreadnum;
    bool workingthread;
    std::map<int, std::set<std::vector<int> > > loopStore;
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
    bool bound;
    SgGraphTraversal();
    virtual ~SgGraphTraversal();
   SgGraphTraversal( SgGraphTraversal &);
    SgGraphTraversal &operator=( SgGraphTraversal &);


};


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

/**
    Gets the source of an edge
    SgGraphTraversal::getSource
    Input:
    @param[edge] int& integer representation of edge in quesution
    @param[g] CFG*& the CFG used
*/
template<class CFG>
inline int
SgGraphTraversal<CFG>::
getSource(int& edge, CFG*& g)
{
    Edge e = intedgemap[edge];
    Vertex v = boost::source(e, *g);
    return(vertintmap[v]);
}

/**
    Gets the target of an edge
    SgGraphTraversal::getTarget
    Input:
    @param[edge] int& integer representation of edge in quesution
    @param[g] the CFG*& CFG used
*/


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
@param[node] int, integer representation of the node to get the in edges from
@param[g] CFG* g, CFG
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
@param[node] int, integer representation of the node to get the out edges from
@param[g] CFG* g, CFG
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

template<class CFG>
std::set<std::vector<int> >
SgGraphTraversal<CFG>::
uTraversePath(int begin, int end, CFG*& g, bool loop) {
    std::set<std::vector<int> > movepaths;
    //std::set<std::vector<int> > movepaths2;
    std::set<std::vector<int> > newpaths;
    //workingthread = -1;:
    int lpsToGo = 0;
    std::cout << "beginning : " << begin << " end: " << end << std::endl;
    std::set<std::vector<int> > npaths;
    int pathcount = 0;
    int npathnum = 1;
    pathnum = 0;
    double globalMin = 1;
    std::vector<int> path;
    std::vector<std::vector<int> > paths;
    int truepaths = 0;
    path.push_back(begin);
    std::vector<std::vector<int> > checkpaths;
    int repeats = 1;
    std::vector<std::vector<int> > npathchecker;
    std::map<int, std::vector<std::vector<int> > > globalLoopPaths;
    //int stoppedpaths = 0;
    std::map<int, int> currents;
    int nnumpaths = 0;
    std::set<std::vector<int> > loopPaths;
    int rounds = 0;
    int oldsize = 0;
    bool done = false;
    std::set<std::vector<int> > fts;

    while (true) {
        if (loop && loopStore.find(begin) != loopStore.end()) {
            std::cout << "loopStore find: " << begin << std::endl;
            return loopStore[begin];
        }
        if (paths.size() > 200000) {
            std::cout << "over 200,000 paths with no loops, stopping" << std::endl;
            return loopPaths;
        }
        if (done) {
     
       
   
 
                if (paths.size() != 0) {
                std::cout << "in done: front = " << paths[0].front() << " back = " << paths[0].back() << " paths.size() = " << paths.size() << std::endl;
                }
             
           
         
       
                for (int qqq = 0; qqq < paths.size(); qqq++) {
                path = paths[qqq];
                if (paths.size() == 1 && paths[0].size() == 1) {
                    std::set<std::vector<int> > nullvec;
                    return nullvec;
                }
                //movepaths.clear();
                truepaths++;
                std::vector<std::vector<int> > nPaths;
                std::vector<int> subpath;
                subpath.push_back(path[0]);
                nPaths.push_back(subpath);
                std::vector<std::vector<int> > newNPaths;
                   int permnums = 1;
                    std::vector<int> perms;
                    std::vector<int> qs;
                    std::map<int, std::vector<std::vector<int> > > localLoops;
                    std::vector<int> takenLoops;
                    bool taken = false;
                    //std::cout << "forming dumpedNodes" << std::endl;
                    for (unsigned int q = 0; q < path.size(); q++) {
                    //if (dumpedNodes.find(path[q]) == dumpedNodes.end()) {
                    if (globalLoopPaths.find(path[q]) != globalLoopPaths.end() && globalLoopPaths[path[q]].size() != 0 /*&& path[q] != begin && path[q] != end*/) {
                    //    for (int qL = 0; qL < globalLoopPaths[path[q]].size(); qL++) {
                    //        for (int qL2 = 0; qL2 < globalLoopPaths[path[q]][qL].size(); qL2++) {
                    //            dumpedNodes.insert(globalLoopPaths[path[q]][qL][qL2]);
                    //        }
                    //   }
                        for (int qp1 = 0; qp1 < globalLoopPaths[path[q]].size(); qp1++) {
                            for (int qp2 = 0; qp2 < takenLoops.size(); qp2++) {
                                if (find(globalLoopPaths[path[q]][qp1].begin(), globalLoopPaths[path[q]][qp1].end(), takenLoops[qp2]) != globalLoopPaths[path[q]][qp1].end()) {
                                    taken = true;
                                }
                            }
                            if (!taken) {
                                localLoops[path[q]].push_back(globalLoopPaths[path[q]][qp1]);
                            }
                            else {
                                taken = false;
                            }
                        }
                        if (localLoops[path[q]].size() != 0) {
                        takenLoops.push_back(path[q]);
                        permnums *= (localLoops[path[q]].size()+1);
                        perms.push_back(permnums);
                        qs.push_back(q);
                        }
                    }
                    }
                    
                    //std::cout << "completedFillingDumpedNodes" << std::endl;
                    //permnums -= 1;
                    //if (permnums != 1) {
                    //std::cout << "permnums: " << permnums << std::endl;
                    //}
                    #pragma omp parallel
                    {
                    #pragma omp for schedule(guided)
                    for (int i = 1; i <= permnums; i++) {
                        bool goodthread = false;
                        std::set<std::vector<int> > movepaths2;
                        std::vector<int> loopsTaken;
                        bool stop = false;
                        int j = 0;
                        std::vector<int> npath;
                        while (true) {
                            if (j == perms.size() || perms[j] > i) {
                                break;
                            }
                            else {
                                j++;
                            }
                        }
                        int pn = i;
                        std::vector<int> pL;
                        for (int j1 = 0; j1 <= j; j1++) {
                            pL.push_back(-1);
                        }
                           for (int k = j; k > 0; k--) {
                               int l = 1;
                               while (perms[k-1]*l < pn) {
                                   l++;
                              }
                               pL[k] = l-2;
                               pn -= (perms[k-1]*(l-1));
                           }
                        pL[0] = pn-2;

                        int q2 = 0;
                        //std::vector<int> npathi;
                        for (int q1 = 0; q1 < path.size(); q1++) {
                            //std::cout << "q1: " << q1 << " q2 " << q2 << std::endl;
                            //std::cout << "q1: " << q1 << " qs[q2]: " << qs[q2] << std::endl;
                            if (qs.size() != 0 && q1 == qs[q2] && q2 != pL.size()) {
                               //if (pL[q2] >= globalLoopPaths[path[q1]].size()) {
                               if (pL[q2] == -1) {
                                   npath.push_back(path[q1]);
                               }
                               else {
/*
                                   bool stop = false;
                                   for (int kk = 0; kk < globalLoopPaths[path[q1]][pL[q2]].size(); kk++) {
                                       if (find(npath.begin(), npath.end(), globalLoopPaths[path[q1]][pL[q2]][kk]) != npath.end()) {
                                          stop = true;
                                           //stoppedpaths++;
                                           //std::cout << "stopped: " << repeats << std::endl;
                                           //repeats++;
                                           //break;
                                       }
                                   }

                                   if (!stop) {
                                   //ROSE_ASSERT(globalLoopPaths[path[q1]].size() > pL[q2]);
                                   for (int qwe = 0; qwe < loopsTaken.size(); qwe++) {
                                       if (find(npath.begin(), npath.end(), loopsTaken[qwe]) != loopsTaken.end()) {
                                           stop = true;
                                       }
                                   }
*/ 
                                  if (!stop) {
                                   npath.insert(npath.end(), localLoops[path[q1]][pL[q2]].begin(), localLoops[path[q1]][pL[q2]].end());
                                   loopsTaken.push_back(path[q1]);
                                   }
                                   //}
                                   //else {
                                   //stop = false;
                                   //npath.push_back(path[q1]);
                                  // }
                               }
                               q2++;
                            }
                            else {
                               npath.push_back(path[q1]);
                            }
                            //if (stop) {
                            //    #pragma omp atomic
                            //    stoppedpaths++;
                               // #pragma omp critical
                               // {
                                //std::cout << "stopped" << std::endl;
                                //for (int qt = 0; qt < npath.size(); qt++) {
                                //std::cout << npath[qt] << ", ";
                               // }
                               // std::cout << std::endl;
                               // } 
                                //break;
                                
                            //}
                        }
                        
                        //std::vector<Vertex> verts;
                        std::vector<int> movepath;
                        
                        for (int qw = 0; qw < npath.size()-1; qw++) {
                            if (qw == 0 && find(markers.begin(), markers.end(), npath[qw]) == markers.end()) {
                                movepath.push_back(npath[qw]);
                            } 
                            if (find(markers.begin(), markers.end(), npath[qw]) != markers.end()) {
                               //std::pair<int, int> mpcheck;
                               movepath.push_back(npath[qw]);
                               if (find(markers.begin(), markers.end(), npath[qw+1]) == markers.end() && qw+1 < npath.size()) {
                               movepath.push_back(npath[qw+1]);
                               }
                            }
                        }
                       
                        bool addit = false;
                        //if (movepath.back() != npath[npath.size()-1]) {
                        //    movepath.push_back(npath[npath.size()-1]);
                       // }
                        if (loop && npath.front() == npath.back()) {
                            addit = true;
                        }
                        else if (!loop && bound && npath.front() == begin && npath.back() == end) {
                            addit = true;
                        }
                        else if (!loop && !bound) {
                            addit = true;
                        }
                        #pragma omp critical
                        {
                        //if (movepaths.find(movepath) == movepaths.end() && !stop) {
                        //    if (!loop) { 
                        //    movepaths.insert(movepath);
                        //    }
                            if (addit) {
                            newpaths.insert(npath);
                            }
                            
                            if (!workingthread) {
                            if ((newpaths.size() > 100 || i == permnums)) {
                                workingthread = true;
                                goodthread = true;
                                //workingthreadnum = omp_get_thread_num();
                                movepaths2 = newpaths;
                                newpaths.clear();
                            }
                            }
                               
                           // }
                       // }
                       // else {
                       //     stop = true;
                       //     stoppedpaths++;
                       // }
                        }
                        
                        //int th_id = omp_get_thread_num();
                        if (goodthread)
                        {
                        if (movepaths2.size() > 100 || i == permnums)
                        {
                        
                        
                        
                        
                        
                    
                    //if (!loop) {
                    //}
                    //int evaledpaths = 0;
                    for (std::set<std::vector<int> >::iterator qr = movepaths2.begin(); qr != movepaths2.end(); qr++) {
                        std::vector<int> npath = *qr;
/*
                        
                        npath.push_back((*qr)[0]);
                        int qa = 0;
                        //if ((*q)[0] != begin) {
                        //npath.push_back(begin);
                        //} 
                         
                        while(qa < (*qr).size()-1) {
                            int currn = npath.back(); 
                            std::vector<int> oeds = getOutEdges(currn, g);
                            if (oeds.size() > 1) {
                                ROSE_ASSERT((*qr)[qa] == currn);
                                //npath.push_back((*qr)[qa]);
                                int getN = (*qr)[qa+1];
                                //std::vector<int> oeds2 = getOutEdges(getN, g);
                                npath.push_back(getN);
                               // if (oeds2.size() < 2) {
                               // npath.push_back((*qr)[qa+1]);
                                 
                                
                                //}
                                qa++;
 
                
                            }
                            else {
                            int currn = npath.back();
                            bool stopend = false;
                            while (find(markers.begin(), markers.end(), currn) == markers.end() && !stopend) {
                                //npath.push_back(currn);
                                
                                std::vector<int> outEdges = getOutEdges(currn, g);
                                 
                                if (outEdges.size() == 0) {
                                stopend = true;
                                }
                                else {
                                ROSE_ASSERT(outEdges.size() == 1);
                                npath.push_back(getTarget(outEdges[0], g));
                                }
                                currn = npath.back();
                            }
                            qa++;
                            
                            }
                      }i

                      if (npath.back() != (*qr)[(*qr).size()-1]) {
                          npath.push_back((*qr)[(*qr).size()-1]);
                      }

                      
                           
                           ADD VERTEX PATH
*/
                      
                      
                      //nnumpaths++;
                      bool analyzed = false;
                      if (evaledpaths % 100000 == 0 && evaledpaths != 0)
                      std::cout << "evaled paths: " << evaledpaths << std::endl;
                      evaledpaths++; 
                      if (!loop && (bound && npath.front() == begin &&  npath.back() == end)) {
                           nnumpaths++;
                          std::vector<Vertex> verts;
                          getVertexPath(npath, g, verts); 
                          analyzePath(verts);
                          analyzed = true;
                          pathnum++;
                      }
                      else if (!loop && !bound) {
                          nnumpaths++;
                          std::vector<Vertex> verts;
                          getVertexPath(npath, g, verts);
                          analyzePath(verts);
                          analyzed = true;
                          pathnum++;
                      }
                      else if (loop && npath.front() == npath.back() && loopPaths.find(npath) == loopPaths.end()) {
                          loopPaths.insert(npath);
                          analyzed = true;
                      }
                      else {
                      //    std::cout << "rejected path: " << std::endl;
                      //    for (int qwe = 0; qwe < npath.size(); qwe++) {
                      //        std::cout << npath[qwe] << ", ";
                      //    }
                       //   std::cout << std::endl;
                      }
                      if (analyzed) {
                          if (!loop) {
                              std::set<int> nodes;
                              for (int j = 0; j < npath.size(); j++) {
                                  bool lp = false;
                                  if (nodes.find(npath[j]) != nodes.end()) {
                                      lp = true;
                                  }
                                  else {
                                      nodes.insert(npath[j]);
                                  }
                                  std::stringstream ss1;
                                  std::string ss;
                                  if (nodeStrings.find(npath[j]) != nodeStrings.end()) {
                                      ss = nodeStrings[npath[j]];
                                  }
                                  else {
                                      ss = "";
                                  }
                                  ss1 << ss;
                                  if (lp) {
                                  ss1 << ", L" << pathnum;
                                  }
                                  else {
                                  ss1 <<  ", " << pathnum;
                                  }
                                  if (!bound) {
                                      ss1 << ":" << sourcenum;
                         
                                  }
                                  ss = ss1.str();
                                  nodeStrings[npath[j]] = ss;
                              }
                          }
                      }
                      
                   }
                   }
                   workingthread = false; 
                   workingthreadnum = -1;
                   //movepaths2.clear();
                  }  
                  }
                  }
                  } 
                  
if (newpaths.size() != 0) {
                    
                    //int evaledpaths = 0;
                    for (std::set<std::vector<int> >::iterator qw = newpaths.begin(); qw != newpaths.end(); qw++) {
                    std::vector<int> npath = *qw;
                      if (evaledpaths % 100000 == 0 && evaledpaths != 0) {
                      std::cout << "evaled paths: " << evaledpaths << std::endl;
                      }
                      evaledpaths++;
                      if (!loop && (bound && npath.front() == begin &&  npath.back() == end)) {
                           nnumpaths++;
                          std::vector<Vertex> verts;
                          getVertexPath(npath, g, verts);
                          analyzePath(verts);
                      }
                      else if (!loop && !bound) {
                          nnumpaths++;
                          std::vector<Vertex> verts;
                          getVertexPath(npath, g, verts);
                          analyzePath(verts);
                      }
                      else if (loop && npath.front() == npath.back() && loopPaths.find(npath) == loopPaths.end()) {
                          loopPaths.insert(npath);
                      }
                      else {
                          //std::cout << "rejected path: " << std::endl;
                          //for (int qwe = 0; qwe < npath.size(); qwe++) {
                          //    std::cout << npath[qwe] << ", ";
                         // }
                         // std::cout << std::endl;
                      }
                   }
                   
                   
                   }
                    

                    
                    std::cout << "nnumpaths: " << nnumpaths << std::endl;
                    
                    
                    if (loop) {
                    std::cout << "loopPaths: " << loopPaths.size() << std::endl;
                    loopStore[begin] = loopPaths;
                    }
                    std::cout << "stoppedpaths: " << stoppedpaths << std::endl;
                    return loopPaths;
                    std::cout << "end of done" << std::endl; 
                    
                    }
         
         
                    
                    
                     
                    
                                                                                                                                                                                                   



            //done code
        
        else {
            if (path.size() == 0) {
                done = true;
            }
            else {
            std::vector<int> outEdges = getOutEdges(path.back(), g);
            if (currents.find(path.back()) == currents.end()) {
                currents[path.back()] == 0;
            }
            if (outEdges.size() == 0 || (path.back() == end && path.size() != 1)) {
                if (find(paths.begin(), paths.end(), path) == paths.end()) {
                if (loop && path.back() == path.front()) {
                    paths.push_back(path);
                } 
                else if (!bound && !loop) {
                    //if (!loop || (path.back() == path.front())) {
                    paths.push_back(path);
                }
                
                else if (bound && path.front() == begin && path.back() == end) {
                    paths.push_back(path);
                }
                }
                path.pop_back();
            }
            else if (currents.find(path.back()) == currents.end()) {
                 currents[path.back()] = 1;
                 path.push_back(getTarget(outEdges[0], g));
            } 
            else if (currents[path.back()] == outEdges.size()) {
                while (currents[path.back()] == outEdges.size() && path.size() != 0) {
                currents[path.back()] = 0;
                path.pop_back();
                outEdges = getOutEdges(path.back(), g);
               }
            }
            else if (find(path.begin(), path.end(), getTarget(outEdges[currents[path.back()]], g)) != path.end() && getTarget(outEdges[currents[path.back()]], g) != end) {
                currents[path.back()]++;
                int currn = getTarget(outEdges[currents[path.back()]-1], g);
                if (find(recursiveLoops.begin(), recursiveLoops.end(), currn) == recursiveLoops.end() && find(closures.begin(), closures.end(), currn) != closures.end()/*&& totalLoops.find(currn) == totalLoops.end()*/ && (globalLoopPaths.find(currn) == globalLoopPaths.end() || globalLoopPaths[currn].size() == 0)) {
                    recursiveLoops.push_back(currn);
                    //std::cout << "leaving: " << begin << ", " << end << " to loop: " << currn << std::endl;
                    std::set<std::vector<int> > lps;
                    //if (totalLoops.find(currn) == totalLoops.end()) {
                    lps = uTraversePath(currn, currn, g, true);
                   
                    //}
                    //else {
                    //    globalLoopPaths[currn] = totalLoops[currn];
                   // }
                    //std::cout << "reentry at: " << begin << ",  " << end << std::endl;
                    bool tot = true;
                    if (totalLoops.find(currn) != totalLoops.end()) {
                        tot = false;
                    }
                    recursiveLoops.pop_back();
                    for (std::set<std::vector<int> >::iterator jj = lps.begin(); jj != lps.end(); jj++) {
                        bool stop = false;
                        for (int kk = 0; kk < (*jj).size(); kk++) {
                            if (find(recursiveLoops.begin(), recursiveLoops.end(), ((*jj)[kk])) != recursiveLoops.end()) {
                                stop = true;
                                break;
                            }
                        }
                        if (!stop) {
                        globalLoopPaths[currn].push_back(*jj);
                        if (tot) {
                        totalLoops[currn].push_back(*jj);
                        }
                        }
                    }
                    //lpsToGo += lps.size();
                   // std::cout << "loops to traverse now: " << lpsToGo << std::endl;
                    //if (lps.size() != 0) {
                    //}
                    //}
                    //recursiveLoops.pop_back();
                }
                //recursiveLoops.clear();  
            }
            else if (currents[path.back()] < outEdges.size()) {
                currents[path.back()]++;
                path.push_back(getTarget(outEdges[currents[path.back()]-1], g));
            }
            else {
                ROSE_ASSERT(false);
            }
        }
    }
}
ROSE_ASSERT(false);
}
                
            



/**
This is the function that is used by the user directly to start the algorithm. It is immediately available to the user

SgGraphTraversal::constructPathAnalyzer
Input:
@param[begin] Vertex, starting node
@param[end] Vertex, endnode
@param[g] CFG* g, CFG calculated previously
*/


template<class CFG>
void
SgGraphTraversal<CFG>::
constructPathAnalyzer(CFG* g, bool unbounded, Vertex begin, Vertex end) {
    //sets up a few data structures to ease computations in future functions
    stoppedpaths = 0;
    evaledpaths = 0;
    sourcenum = 0;
    prepareGraph(g);
    workingthread = false;
    workingthreadnum = -1;
    std::cout << "markers: " << markers.size() << std::endl;
    std::cout << "closures: " << closures.size() << std::endl;
    std::cout << "sources: " << sources.size() << std::endl;
    std::cout << "sinks" << sinks.size() << std::endl;
    printHotness(g);
    // if you don't want to calculate in parallel
    bool subgraph = false;
    if (!subgraph) {
        //run traverse on the whole graph
            if (!unbounded) {
                bound = true;
                //totalLoops.clear();
                //globalLoopPaths.clear();
                recursiveLoops.clear();
                uTraversePath(vertintmap[begin],vertintmap[end], g);
            }
        //if begin and end are not specified, run from all sources to one end node (equivalently run to all end nodes)
            else {
                bound = false;
                for (unsigned int j = 0; j < sources.size(); j++) {
                    //totalLoops.clear();
                    //globalLoopPaths.clear();
                    sourcenum = sources[j];
                    std::cout << "sources left: " << sources.size() - j << std::endl;
                    recursiveLoops.clear();
                    //std::cout << "j: " << j << std::endl;
                    uTraversePath(sources[j], -1, g);
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
                uTraversePath(sources[j], vertintmap[end], subGraphVector[i]);
            }
         }
    }
    //printPathDot(g);
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

//internal functions
        
        template<class CFG>
        void
        SgGraphTraversal<CFG>::
        printCFGNodeGeneric(int &cf, std::string prop, std::ofstream& o) {
            std::string nodeColor = "black";
            o << cf << " [label=\"" << " num:" << cf << " prop: " << prop <<  "\", color=\"" << nodeColor << "\", style=\"" << "solid" << "\"];\n";
        }

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
        printPathDot(CFG*& g)
        {
            const CFG* gc = g;
            std::ofstream mf;
            std::stringstream filenam;
            filenam << "pathnums.dot";
            std::string fn = filenam.str();
            mf.open(fn.c_str());

            mf << "digraph defaultName { \n";
            vertex_iterator v, vend;
            edge_iterator e, eend;
            for (tie(v, vend) = vertices(*gc); v != vend; ++v)
            {
                if (nodeStrings.find(vertintmap[*v]) != nodeStrings.end()) {
                    int nn = vertintmap[*v];
                    printCFGNodeGeneric(vertintmap[*v], nodeStrings[nn], mf);
                }
                else {
                    printCFGNodeGeneric(vertintmap[*v], "noprop", mf);
                }
            }
           for (tie(e, eend) = edges(*gc); e != eend; ++e)
            {
                printCFGEdge(edgeintmap[*e], g, mf);
            }

            mf.close();
        }



/**
This is the function that preps the graph for traversal

SgGraphTraversal::prepareGraph
Input:
@param[g] CFG*& g, CFG calculated previously
*/


template<class CFG>
void
SgGraphTraversal<CFG>::
prepareGraph(CFG*& g) {
    nextNode = 1;
    nextEdge = 1;
    findClosuresAndMarkersAndEnumerate(g);
   // computeOrder(g, sources[0]);
}


/** 
This is the function that preps the graph for traversal, currently this one isn't used but for many traversals on one visitor
may necessitate
 
SgGraphTraversal::firstPrepGraph 
Input: 
@param[g] CFG*& g, CFG calculated previously 
*/ 


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



/** Currently unused but will be necessary for parallelization in progress
SgGraphTraversal::computeOrder
@param[g] CFG* cfg in question
@parm[begin] const int, integer representation of source node
*/
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

/**
Converts the path calculated by this algorithm to Vertices so users can
access data
SgGraphTraversal::getVertexPath
@param[path] integer representation of path
@param[g] CFG*, cfg in question
@param[vertexPath] for some reason this can't be a return value so it is changed via pass by reference
*/

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

/**
Currently unused, may eventually be modified for optimal storage purposes
SgGraphTraversal::storeCompact
@param[compactPath] path to be compactified
*/
template<class CFG>
void
SgGraphTraversal<CFG>::
storeCompact(std::vector<int> compactPath) {
return;
}




/**
Traversal function
SgGraphTraversal::traversePath
@param[begin] integer representation of the source node
@param[end] the final node
@param[loop] bool, tells the program whether or not it's traversing a loop
so that loops aren't returned by themselves as paths but rather are incorporated
into complete paths
*/



template<class CFG>
std::set<std::vector<int> > 
SgGraphTraversal<CFG>::
traversePath(int begin, int end, CFG*& g, bool loop) {
        //std::cout << "begin: " << begin << std::endl;
        //std::cout << "end: " << end << std::endl;
    //setting the current node variable and grabbing edges
    int pathcount = 0;
    int npathnum = 1;
    double globalMin = 1;
    std::vector<int> path;
    std::vector<std::vector<int> > paths;
    int truepaths = 0;
    path.push_back(begin);
    std::vector<std::vector<int> > checkpaths;
    int repeats = 1;
    std::vector<std::vector<int> > npathchecker;
    std::map<int, std::vector<std::vector<int> > > globalLoopPaths;
    std::map<int, int> currents;
    int nnumpaths = 0;
    std::set<std::vector<int> > loopPaths;
    int rounds = 0;
    int oldsize = 0;
    bool done = false;
    std::set<std::vector<int> > fts;
/* this loop should go down the graph until it hits an endnode or the specified endnode
or a node with a greater max depth than is allowed (that is, the number that says if all loops
traversed maximally once what is the greatest number of steps taken to reach the desired node */
        while (true) {
            rounds++;
            //std::cout << "path.size(): " << path.size() << std::endl;
            if (rounds % 1000000 == 0) {
            std::cout << "round: " << rounds << std::endl;
            //std::cout << "paths.size(): " << paths.size() << std::endl;
            
            }
            if (!loop && paths.size() % 100 == 0 && paths.size() > 0) {
            std::cout << "paths.size(): " << paths.size() << std::endl;
            if (paths.size() != oldsize) {
                oldsize = paths.size();
                std::cout << "new path size: " << paths.size() << std::endl;
            }
            }
            //std::cout << "paths.size(): " << paths.size() << std::endl;
            //std::cout << "path.size(): " << path.size() << std::endl;
            //std::cout << "recursiveLoops.size(): " << recursiveLoops.size() << std::endl;
            //}          //std::cout << "path.back().first: " << path.back().first << std::endl;  
                        //std::cout << "path.back().second: " << path.back().second << std::endl;
            //if (path.back() == end && end != -1) {
            
           // if (path.front() == 526) {
           // std::cout << "partial path: " << std::endl;
           // for (unsigned int j = 0; j < path.size(); j++) {
           //           std::cout << path[j] << ", ";
          // }
          // std::cout << std::endl;
          //            if (fts.find(path) != fts.end()) {
          //                std::cout << "copy" << std::endl;
          //            }
          //            else {
          //                fts.insert(path);
          //            }
            
          //  }
           
            
          //  std::cout << std::endl;
            
            //std::cout << "path.size(): " << path.size() << std::endl;
            std::vector<int> outEdges = getOutEdges(path.back(), g);
            if (path.size() == 0) {
                done = true;
            }
            if (done) /*outEdges.size() == 0 || (path.back() == end && path.size() != 1 && end != -1) || (path.back() == begin && path.size() != 1  && begin != -1)*/ {
                std::cout << "in done" << std::endl;
                for (int qqq = 0; qqq < paths.size(); qqq++) {
                path = paths[qqq];
                std::cout << "qqq: " << qqq << std::endl;
                bool stop = false;
                //ROSE_ASSERT(path.size() != 2 || path.back() != begin);
                ROSE_ASSERT(find(checkpaths.begin(), checkpaths.end(), path) == checkpaths.end());
                checkpaths.push_back(path);
               // std::cout << "truepaths: " << truepaths << std::endl;
                truepaths++;
                //std::cout << "path.size(): " << path.size() << std::endl; 
                std::vector<std::vector<int> > nPaths;
                std::vector<int> subpath;
                subpath.push_back(path[0]);
                nPaths.push_back(subpath);
                std::vector<std::vector<int> > newNPaths;
                   int permnums = 1;
                   // std::set<std::vector<int> > pLs;
                    std::vector<int> perms;
                    std::vector<int> qs;
                    for (int i = 0; i < path.size(); i++) {
                        double minPaths = 1;
                        std::vector<int> ieds = getInEdges(path[i], g);
                        if (ieds.size() > 1) {
                            minPaths *= ieds.size();
                        }
                        if (minPaths > globalMin) {
                            globalMin = minPaths;
                            //std::cout << "globalMin: " << globalMin << std::endl;
                        }
                    }
                    for (unsigned int q = 0; q < path.size(); q++) {
                    if (globalLoopPaths.find(path[q]) != globalLoopPaths.end() && globalLoopPaths[path[q]].size() != 0 /*&& path[q] != begin && path[q] != end*/) {
                        permnums *= (globalLoopPaths[path[q]].size()+1);
                        perms.push_back(permnums);
                        qs.push_back(q);
                    }
                    }
                    //permnums -= 1;i
                    if ((paths.size() - qqq) % 100 == 0) {
                    std::cout << "paths left: " << paths.size() - qqq << std::endl;
                    }
                    //#pragma omp parallel for
                    for (int i = 1; i <= permnums; i++) {
                        int j = 0;
                        std::vector<int> npath;
                        while (true) {
                            if (j == perms.size() || perms[j] > i) {
                                break;
                            }
                            else {
                                j++;
                            }
                        }
                        int pn = i;
                        std::vector<int> pL;
                        for (int j1 = 0; j1 <= j; j1++) {
                            pL.push_back(-1);
                        }
                       // if (j < perms.size()) {
                           for (int k = j; k > 0; k--) {
                               int l = 1;
                               while (perms[k-1]*l < pn) {
                                   l++;
                               }
                               pL[k] = l-2;
                               pn -= (perms[k-1]*(l-1));
                           }
                        pL[0] = pn-2;
                       // ROSE_ASSERT(pLs.find(pL) == pLs.end());
                       // pLs.insert(pL);
                        //}
                        
                        //else {
                        //    for (int k1 = 0; k1 < pL.size(); k1++) {
                        //        ROSE_ASSERT(pL.size() == path.size());
                        //        pL[k1] = path[k1];
                        //    }
                        //}
                        //std::cout << "pL: " << std::endl;
                        //for (int q3 = 0; q3 < pL.size(); q3++) {
                        //    std::cout << pL[q3] << ", ";
                        //}
                        //std::cout << std::endl;
   
                        int q2 = 0;
                        //std::vector<int> npath;
                        for (int q1 = 0; q1 < path.size(); q1++) {
                            //std::cout << "q1: " << q1 << " q2 " << q2 << std::endl;
                            //std::cout << "q1: " << q1 << " qs[q2]: " << qs[q2] << std::endl;
                            if (qs.size() != 0 && q1 == qs[q2] && q2 != pL.size()) {
                               //if (pL[q2] >= globalLoopPaths[path[q1]].size()) {
                               if (pL[q2] == -1) {
                                   npath.push_back(path[q1]);
                               }
                               else {
                        //           bool stop = false;
                                   for (int kk = 0; kk < globalLoopPaths[path[q1]][pL[q2]].size(); kk++) {
                                       if (find(npath.begin(), npath.end(), globalLoopPaths[path[q1]][pL[q2]][kk]) != npath.end()) {
                                           stop = true;
                                           //std::cout << "stopped: " << repeats << std::endl;
                                           repeats++;
                                           break;
                                       }
                                   }
                                   //if (!stop) {
                                  // ROSE_ASSERT(globalLoopPaths[path[q1]].size() > pL[q2]);
                                   npath.insert(npath.end(), globalLoopPaths[path[q1]][pL[q2]].begin(), globalLoopPaths[path[q1]][pL[q2]].end());
                                  // }
                                  // else {
                                  // stop = false;
                                  // npath.push_back(path[q1]);
                                  // }
                               }
                               q2++;
                            }
                            else {
                               npath.push_back(path[q1]);
                            }
                            if (stop) {
                                break;
                            }
                        }
                        std::vector<Vertex> verts;
                        //if (npathnum % 10000 == 0) {
                        //std::cout << "path: " << npathnum << std::endl;
                       // }
                       // npathnum++;
                        //for (int j2 = 0; j2 < path.size(); j2++) {
                        //    std::cout << path[j2] << ", ";
                       // }
                       // std::cout << std::endl;
                       // std::cout << "npath" << std::endl;
                       // for (int j3 = 0; j3 < npath.size(); j3++) {
                       //     std::cout << npath[j3] << ", ";
                       // }
                       // std::cout << std::endl;
                        if (!loop && !stop) {
                        if ((bound && npath.front() == begin && npath.back() == end && !stop)) {
                        getVertexPath(npath, g, verts);
/*
                        if (find(npathchecker.begin(), npathchecker.end(), npath) != npathchecker.end()) {
                            std::cout << "repeat: " << repeats << std::endl;
                            repeats++;
                        //    for (int ww = 0; ww < npath.size(); ww++) {
                        //        std::cout << npath[ww] << ", ";
                        //    }
                         //   std::cout << std::endl;
                        }
                         else {
                         npathchecker.push_back(npath);
                         //std::cout << "npathchecker.size(): " << npathchecker.size() << std::endl;
                         }
*/
                        analyzePath(verts);
                        nnumpaths++;
                        if (nnumpaths % 10000 == 0) {
                        std::cout << "nnumpaths: " << nnumpaths << std::endl;
                        }
                        }
                        else if (!bound && !stop) {
                        //ROSE_ASSERT(find(npathchecker.begin(), npathchecker.end(), npath) == npathchecker.end());
/*
                        if (find(npathchecker.begin(), npathchecker.end(), npath) != npathchecker.end()) {
                           std::cout << "repeat: " << repeats << std::endl;
                           repeats++;
                        }
                        else {
                        // for (int ww = 0; ww < npath.size(); ww++) {
                        //     std::cout << npath[ww] << ", ";
                        //    }
                        //    std::cout << std::endl;
                        //} 
                        npathchecker.push_back(npath);
                        }
*/
                        getVertexPath(npath, g, verts);
                        nnumpaths++;
                        if (nnumpaths % 10000 == 0) {
                        std::cout << "nnumpaths: " << nnumpaths << std::endl;
                        }
                        analyzePath(verts);
                        //std::cout << "npathchecker.size(): " << npathchecker.size() << std::endl;
                        }
                        }
                        else if (!stop) {
                        if (loopPaths.find(npath) == loopPaths.end() && !stop) {
                        //npath.push_back(npath.front());
                        if (npath.front() == npath.back() && npath.back() == end /*&& npath.size() > 2*/) {
                            //std::cout << "loop" << std::endl;
                       
                            loopPaths.insert(npath);
                        }
                        }
                        else {
                            //std::cout << "repeat: " << repeats << std::endl;
                            //repeats++;

                        }
                        }
                        if (stop) {
                            stop = false;
                        }
                        
                        npath.clear();
                    }
                    }
                    return loopPaths;
                    }
                    //std::cout << "should start here" << std::endl;
std::vector<int> outEdges2 = getOutEdges(path.back(), g);
if ((outEdges2.size() == 0) || (path.back() == end && path.size() != 1 && end != -1) || (path.back() == begin && path.size() != 1  && begin != -1)) {
                   if (find(paths.begin(), paths.end(), path) == paths.end()) {
                   if (bound && !loop) {
                       if (path.front() == begin && path.back() == end) {
                            
                           paths.push_back(path);
                           
                       }
                   }
                   else if (!bound && !loop) {
                       paths.push_back(path);
                   }
                   else if (loop) {
                       if (path.front() == begin && path.back() == end) {
                           paths.push_back(path);
                       }
                   }
                   
                   else {
                       ROSE_ASSERT(false);
                   }
                   }
                   if (path.size() == 0) {
                       done = true;
                   }
                   else  {
                   
                   path.pop_back();
                   }
                        
                    if (path.size() == 0) {
                        done = true;
                    }
                   // else {
                    std::vector<int> oeds = getOutEdges(path.back(), g);
                    std::set<int> loopfind;
                    while ((path.size() != 0 && (unsigned int) (currents[path.back()] >= oeds.size())) || (path.size() != 0 && find(path.begin(), path.end(), getTarget(oeds[currents[path.back()]], g)) != path.end())) {
                        if ((unsigned int) currents[path.back()] >= oeds.size()) {
                        // if (find(path.begin(), path.end(), getTarget(oeds[currents[path.back()]], g)) == path.end()) {
                        if (loopfind.find(path.back()) == loopfind.end()) {
                            //currents[path.back()] = 0;
                        
                        if (find(path.begin(), path.end(), getTarget(oeds[currents[path.back()]], g)) != path.end()) {
                           loopfind.insert(getTarget(oeds[currents[path.back()]], g));
                        }   
                        else {
                            currents[path.back()] = 0;
                        }
                        //currents[path.back()] = 0;
                        path.pop_back();
                        oeds = getOutEdges(path.back(), g);
                        }
                        else {
                            //loopfind.erase(path.back());
                            path.pop_back();
                            oeds = getOutEdges(path.back(), g);
                        }
                        }
                        else {
                             //currents[path.back()]++;
                             path.pop_back();
                             oeds = getOutEdges(path.back(), g);
                         }
                   }
                   loopfind.clear();
                        
                     
                    
                    if (path.size() == 0) {
                        done = true;
                    }
                    else {
                    int oldBack = path.back();
                    path.push_back(getTarget(oeds[currents[path.back()]], g));
                    currents[oldBack]++;
                    }
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
                 if (find(path.begin(), path.end(), currn) == path.end() || (currn == begin && currn == end) || (currn == begin && currn == end && path.size() != 1)) {
                     currents[path.back()]++;
                     path.push_back(currn);
                     std::vector<int> oeds1 = getOutEdges(currn, g);
                  //   while (oeds1.size() == 1 && currents[path.back()] != 1) {
                   //      currents[path.back()]++;
                   //      path.push_back(getTarget(oeds1[0], g));
                   //      oeds1 = getOutEdges(path.back(), g);
                         
                   //  }
                         
                          
  
                 }
                 else {
                     //std::cout << "looped currn: " << currn << std::endl;
                     bool nogo = false;
                     currents[path.back()]++;
                     //for (int k = 1; k < path.size()-1; k++) {
                     //    if (find(recursiveLoops.begin(), recursiveLoops.end(), path[k]) != recursiveLoops.end()) {
                     //       nogo = true;
                     //    }
                     //} 
                     if ((!nogo && ((globalLoopPaths.find(currn) == globalLoopPaths.end()) /*|| (!nogo && globalLoopPaths[currn].size() == 0)(*/)  && find(recursiveLoops.begin(), recursiveLoops.end(), currn) == recursiveLoops.end() && (currn != begin || (currn == begin && currn != end)))) {
                         
                         std::vector<std::vector<int> > tmplps;
                         globalLoopPaths[currn] = tmplps;
                         recursiveLoops.push_back(currn);
                         std::cout << "solving a loop at " << currn << std::endl;
                         std::set<std::vector<int> > lps = traversePath(currn, currn, g, true);
                         
                         std::cout << "loops found: " << lps.size() << std::endl;
                         std::cout << "completed loop " << currn << std::endl;
                         recursiveLoops.pop_back();
                         //currents[currn]++;
                         std::vector<int> ieds = getInEdges(currn, g);
                         //ROSE_ASSERT(ieds.size() > 1);
                         for (std::set<std::vector<int> >::iterator i = lps.begin(); i != lps.end(); i++) {
                             if ((*i).back() == currn) {
                                // for (int k = 1; k < (*i).size()-1; k++) {
                                     //if (globalLoopPaths.find(path[k]) != globalLoopPaths.end() && globalLoopPaths[path[k]].size() != 0 && find(recursiveLoops.begin(), recursiveLoops.end(), path[k]) == recursiveLoops.end()) {
                                     //    nogo = true;
                                    // }
                                // }
                                 if (!nogo) {
                                     globalLoopPaths[currn].push_back(*i);
                                 }
                                 nogo = false;
                             }
                             else {
                                 //globalEndLoopPaths[currn].insert(*i);
                             }
                         }
                     recursiveLoops.clear();
                     }
                     //nogo = false;
                     //ROSE_ASSERT(path.back() == currn);
                     std::vector<int> ods2 = getOutEdges(path.back(), g);
                     if ((unsigned int) currents[path.back()] >= ods2.size()) {
                         while ((unsigned int) currents[path.back()] >= ods2.size() && path.size() != 0) {
                             currents[path.back()] = 0;
                             path.pop_back();
                             ods2 = getOutEdges(path.back(), g);
                         }
                     
                     if (path.size() == 0) {
                         done = true;
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
               std::set<int> loopskips;
               while ((unsigned int)currents[path.back()] >= qds.size() && path.size() != 0) {
               //    std::cout << "in loopskips while" << std::endl;
                   int ppb = path.back();
                   if (loopskips.find(path.back()) == loopskips.end()) {
                   //currents[path.back()] = 0;
                   //}
                   //else {
                   //loopskips.erase(path.back());
                   //}
                   //int ppb = path.back();
                   path.pop_back();
                   if (find(path.begin(), path.end(), ppb) != path.end()) {
                       loopskips.insert(ppb);
                   }
                   else {
                       currents[ppb] = 0;
                   }
                   }
                   
                   //}
                   else {
                       //loopskips.erase(path.back());
                       path.pop_back();
                   }
                   //path.pop_back();
                   qds = getOutEdges(path.back(), g);
               }
               //std::cout << "completed loopskips while" << std::endl;
               if (path.size() == 0) {
                   done = true;
               }
               else {
               qds = getOutEdges(path.back(), g);
               int oldback = path.back();
               path.push_back(getTarget(qds[currents[path.back()]], g));
               currents[oldback]++;
               }
          }
     
     }
     } 
     
     return loopPaths;
}
                  
