/*

FINISH TEMPFLATPATH CODE

*/




#define LP 1
#define PERFDEBUG 0
//#define FULLDEBUG 1
#ifdef _OPENMP
#include <omp.h>
#endif
#include <boost/regex.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <staticCFG.h>

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

@todo utilize BOOST visitors to take advantage of the BOOST graph structures abilities

***************
*Contact Info
***************

*Finally, blame can be assigned to and questions can be forwarded to the author, though response is not guaranteed

*if I'm still at Lawrence
*hoffman34 AT llnl DOT gov
*@author Michael Hoffman
*/


#include <boost/graph/adjacency_list.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/utility.hpp> // required to TIE support
#include <boost/graph/graphviz.hpp>
#include <boost/graph/dominator_tree.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/transpose_graph.hpp>
#include <boost/algorithm/string.hpp>



#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/time.h>






template <class CFG>
class SgGraphTraversal
{
public:

  typedef typename boost::graph_traits<CFG>::vertex_descriptor Vertex;
    typedef typename boost::graph_traits<CFG>:: edge_descriptor Edge;

   void constructPathAnalyzer(CFG* g, bool unbounded=false, Vertex end=0, Vertex begin=0, bool ns = true);
   virtual void analyzePath(std::vector<Vertex>& pth) = 0;
    std::vector<int> getInEdges(int& node, CFG*& g);
    std::vector<int> getOutEdges(int& node, CFG*& g);
    int getTarget(int& n, CFG*& g);
    int getSource(int& n, CFG*& g);
    std::map<Vertex, int> vertintmap;
    std::map<Edge, int> edgeintmap;
    std::map<int, Vertex> intvertmap;
    std::map<int, Edge> intedgemap;
   SgGraphTraversal();
    virtual ~SgGraphTraversal();
   SgGraphTraversal( SgGraphTraversal &);
    SgGraphTraversal &operator=( SgGraphTraversal &);
   int pathnum;


  void firstPrepGraph(CFG*& g);

private:

    int normals;
    int abnormals;
    bool needssafety;
    int recursed;
    int checkedfound;
 //   typedef typename boost::graph_traits<CFG>::vertex_descriptor Vertex;
 //   typedef typename boost::graph_traits<CFG>:: edge_descriptor Edge;
   // std::vector<int> getInEdges(int& node, CFG*& g);
   // std::vector<int> getOutEdges(int& node, CFG*& g);
    void prepareGraph(CFG*& g);
    void findClosuresAndMarkersAndEnumerate(CFG*& g);
  //  void constructPathAnalyzer(CFG* g, bool unbounded=false, Vertex end=0, Vertex begin=0, bool ns = true);
  //  virtual void analyzePath(std::vector<Vertex>& pth) = 0;
  //  void firstPrepGraph(CFG*& g);
    int stoppedpaths;
    std::set<std::vector<int> >  traversePath(int begin, int end, CFG*& g, bool loop=false);
    std::set<std::vector<int> > uTraversePath(int begin, int end, CFG*& g, bool loop, std::map<int, std::vector<std::vector<int> > >& localLoops);
    std::vector<std::vector<int> > bfsTraversePath(int begin, int end, CFG*& g, bool loop=false);
    std::vector<int> unzipPath(std::vector<int>&  path, CFG*& g, int start, int end);
    std::vector<int>  zipPath(std::vector<int>& path, CFG*& g, int start, int end);
    std::vector<int> zipPath2(std::vector<int>& path, CFG*& g);
    void printCFGNode(int& cf, std::ofstream& o);
    void printCFGNodeGeneric(int& cf, std::string prop, std::ofstream& o);
    void printCFGEdge(int& cf, CFG*& cfg, std::ofstream& o);
    void printHotness(CFG*& g);
    void printPathDot(CFG*& g);
    void computeOrder(CFG*& g, const int& begin);
    void computeSubGraphs(const int& begin, const int &end, CFG*& g, int depthDifferential);
    //int getTarget(int& n, CFG*& g);
    //int getSource(int& n, CFG*& g);
    std::vector<int> sources;
    std::vector<int> sinks;
    std::vector<int>  recursiveLoops;
    std::vector<int> recurses;
    std::map<int, int> ptsNum;
    bool borrowed;
    std::set<int> badloop;
    std::map<int, std::vector<std::vector<int> > >  totalLoops;
//    int pathnum;
    std::map<int, std::string> nodeStrings;
    int sourcenum;
    unsigned long long evaledpaths;
    int badpaths;
    int workingthreadnum;
    bool workingthread;
    std::map<int, std::set<std::vector<int> > > loopStore;
    std::vector<std::vector<int> >  pathStore;
    std::map<int, std::vector<int> > subpathglobal;
    std::map<std::vector<int>, int> subpathglobalinv;
    int nextsubpath;
    std::vector<int>  orderOfNodes;
//    std::map<Vertex, int> vertintmap;
//    std::map<Edge, int> edgeintmap;
//    std::map<int, Vertex> intvertmap;
//    std::map<int, Edge> intedgemap;
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
//    SgGraphTraversal();
//    virtual ~SgGraphTraversal();
//   SgGraphTraversal( SgGraphTraversal &);
//    SgGraphTraversal &operator=( SgGraphTraversal &);


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
    @param[edge] int& integer representation of edge in question
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

/**
Condenses paths, currently deprecated...
Input:
@param[pth] std::vector<int> the original path
@param[g] CFG*, the ambient graph
Output:
zipped path
*/

template<class CFG>
inline
std::vector<int>
SgGraphTraversal<CFG>::
zipPath2(std::vector<int>& pth, CFG*& g) {
    std::vector<int> npth;
    npth.push_back(pth[0]);
    for (int i = 1; i < pth.size()-1; i++) {
       if (find(closures.begin(), closures.end(), pth[i]) != closures.end()) {
           npth.push_back(pth[i]);
       }
    }
    npth.push_back(pth.back());
    return npth;
}

/**
Condenses paths to simply the first and last node and the ordered set of edges
taken at nodes with more than 1 outedge
Input:
@param[pth] std::vector<int>, the original path
@param[g] CFG*, the ambient graph
@param[start] integer representation of the first node
@param[end] integer representation of the last node
*/


template<class CFG>
std::vector<int>
SgGraphTraversal<CFG>::
zipPath(std::vector<int>& pth, CFG*& g, int start, int end) {
                        std::vector<int> subpath;
                        std::vector<int> movepath;
                        movepath.push_back(pth.front());
                        movepath.push_back(pth.back());
                        for (unsigned int qw = 0; qw < pth.size()-1; qw++) {
                           if (find(markers.begin(), markers.end(), pth[qw]) != markers.end()) {
                               std::vector<int> oeds = getOutEdges(pth[qw], g);
                               for (unsigned int i = 0; i < oeds.size(); i++) {
                                   if (getTarget(oeds[i], g) == pth[qw+1]) {
                                       movepath.push_back(oeds[i]);
                                   }
                               }
                            }
                         }
                         return movepath;
             }






/**
unzips the paths zipped by zipPath
Input:
@param[pzipped] the zipped path
@param[CFG] the ambient graph
@param[start] the integer representation of the first node (used to check that zipPath is working correctly)
@param[end] the integer representation of the end node
*/


template<class CFG>
std::vector<int>
SgGraphTraversal<CFG>::
unzipPath(std::vector<int>& pzipped, CFG*& g, int start, int end) {
     ROSE_ASSERT(pzipped[0] == start && (pzipped[1] == end || end == -1));
     std::vector<int> zipped;
     for (unsigned int i = 2; i < pzipped.size(); i++) {
         zipped.push_back(pzipped[i]);
     }
     std::vector<int> unzipped;
     unzipped.push_back(start);
     std::vector<int> oeds = getOutEdges(start, g);
     if (oeds.size() == 0) {
         return unzipped;
     }
     for (unsigned int i = 0; i < zipped.size(); i++) {
         oeds = getOutEdges(unzipped.back(), g);
         while (oeds.size() == 1) {
             if (getTarget(oeds[0], g) == end && unzipped.size() != 1) {
                  unzipped.push_back(end);
                  return unzipped;
             }
             unzipped.push_back(getTarget(oeds[0], g));
             oeds = getOutEdges(unzipped.back(), g);
         }
         if (oeds.size() == 0) {
             return unzipped;
         }
         if (oeds.size() > 1 && (unzipped.back() != end || (unzipped.size() == 1 && unzipped.back() == end))) {
             ROSE_ASSERT(getSource(zipped[i], g) == unzipped.back());
             unzipped.push_back(getTarget(zipped[i], g));
         }
         
     }
     std::vector<int> oeds2 = getOutEdges(unzipped.back(), g);
     if (unzipped.back() != end && oeds2.size() != 0) { 
         while (oeds2.size() == 1 && unzipped.back() != end) {
             unzipped.push_back(getTarget(oeds2[0], g));
             oeds2 = getOutEdges(unzipped.back(), g);
         }
     }
     return unzipped;
}

/*
Example Time

    Example:
             timeval tim;
             gettimeofday(&tim, NULL);
             double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
             do_something_long();
             gettimeofday(&tim, NULL);
             double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
             printf("%.6lf seconds elapsed\n", t2-t1);

*/

/**
The function responsible for collecting all paths without loops, and all paths within lops that do not include other loops
then sending those to uTraverse to assemble them into all paths with any combination of loops
Input:
@param[begin] integer representation of the first node
@param[end] integer representation of the last node (or -1 if its not bounded)
@param[g] CFG*, the ambient CFG
@param[loop] boolean expressing whether or not we are calculating paths contained within a loop
*/


template<class CFG>
std::vector<std::vector<int> >
SgGraphTraversal<CFG>::
bfsTraversePath(int begin, int end, CFG*& g, bool loop) {
//perfdebug allows for examining the speed of traversal
    #ifdef PERFDEBUG
    //timeval tim;
    //gettimeofday(&tim, NULL);
    //double tim1 = tim.tv_sec+(tim.tv_usec/1000000.0);
    #endif
    bool recursedloop = loop;
    std::map<int, std::vector<std::vector<int> > > PtP;
    std::set<int> nodes;
    std::vector<std::vector<int> > pathContainer;
    //std::vector<std::vector<int> > oldPaths;
    std::vector<int> completedLoops;
    std::vector<std::vector<int> > npc;
    std::vector<int> bgpath;
    bgpath.push_back(begin);
    pathContainer.push_back(bgpath);
    std::vector<std::vector<int> > newPathContainer; 
    std::vector<std::vector<int> > paths;
    std::vector<int> localLoops;
    std::map<int, std::vector<std::vector<int> > > globalLoopPaths;
     //std::cout << "at the while" << std::endl;
//To keep
     while (pathContainer.size() != 0 /*|| oldPaths.size() != 0*/) {
/*
       unsigned int mpc = 50000;
       if (pathContainer.size() == 0) {
           unsigned int mxl = 0; 
           if (oldPaths.size() > mpc) {
               mxl = mpc/2;
           }
           else {
               mxl = oldPaths.size();
           }
           for (unsigned int k = 0; k < mxl; k++) {
               pathContainer.push_back(oldPaths.back());
               oldPaths.pop_back();
           }
       }
       if (pathContainer.size() > mpc) {
           unsigned int j = 0;
           while (j < mpc) {
                npc.push_back(pathContainer.back());
                pathContainer.pop_back();
                j++;
           }
           oldPaths.insert(oldPaths.end(), pathContainer.begin(), pathContainer.end());   
           pathContainer = npc;
           npc.clear();
       }
*/

//iterating through the currently discovered subpaths to build them up
       for (unsigned int i = 0; i < pathContainer.size(); i++) {
       std::vector<int> npth = pathContainer[i];
        std::vector<int> oeds = getOutEdges(npth.back(), g);
        std::vector<int> ieds = getInEdges(npth.back(), g);
 
        npth = pathContainer[i];
        oeds = getOutEdges(npth.back(), g);

        if ((!recursedloop && ((bound && npth.back() == end && npth.size() != 1) || (!bound && oeds.size() == 0))) || (recursedloop && npth.back() == end && npth.size() != 1)) {
            std::vector<int> newpth;
            newpth = (pathContainer[i]);
             std::vector<int> movepath = newpth;//zipPath(newpth, g);
             if (recursedloop && newpth.back() == end && newpth.size() != 1) {
             paths.push_back(movepath);
             }
             else if (!recursedloop) {
             if (bound && newpth.size() != 1 && newpth.back() == end) {
             paths.push_back(movepath);
             }
             else if (!bound) {
             paths.push_back(movepath);
             }
             }
             
        }
        else {
std::vector<int> oeds = getOutEdges(pathContainer[i].back(), g);

        for (unsigned int j = 0; j < oeds.size(); j++) {


int tg = getTarget(oeds[j], g);
            

            std::vector<int> newpath = (pathContainer[i]);
           //we split up paths into pieces so that they don't take up a lot of memory, basically this is when we run into a path
           //more than once, so we attach all paths that go to that path to that particular node via PtP
            if (nodes.find(tg) != nodes.end() && find(newpath.begin(), newpath.end(), tg) == newpath.end() && tg != end) {
                if (PtP.find(tg) == PtP.end()) {
                    std::vector<int> nv;
                    nv.push_back(tg);
                    newPathContainer.push_back(nv);
                    PtP[tg].push_back(/*zipPath(*(*/newpath);//, g, newpath.front(), newpath.back()));
                }
                else {
                    PtP[tg].push_back(/*zipPath(*/newpath);//, g, newpath.front(), newpath.back()));
                }
            }
            else if (find(newpath.begin(), newpath.end(), getTarget(oeds[j], g)) == newpath.end() || getTarget(oeds[j], g) == end) {
                newpath.push_back(tg);
                std::vector<int> ieds = getInEdges(tg, g);
                if (ieds.size() > 1) {//find(closures.begin(), closures.end(), tg) != closures.end()) {
                nodes.insert(tg);
                }
                newPathContainer.push_back(newpath);
            }
            else if (tg == end  && recursedloop) {
                newpath.push_back(tg);
                newPathContainer.push_back(newpath);
            }
            else {//if (find(newpath.begin(), newpath.end(), tg) != newpath.end() && tg != end) { 
                std::vector<int> ieds = getInEdges(tg, g);
                if (ieds.size() > 1/*find(closures.begin(), closures.end(), tg) != closures.end()*/ && find(completedLoops.begin(), completedLoops.end(), tg) == completedLoops.end() /*&& find(localLoops.begin(), localLoops.end(), tg) == localLoops.end()*/ && find(recurses.begin(), recurses.end(), tg) == recurses.end()) {
                   localLoops.push_back(tg);
                   nodes.insert(tg);
                }
               // else if (find(recurses.begin(), recurses.end(), tg) != recurses.end()) {
               // }
           }
           //else {
           //    std::cout << "problem" << std::endl;
           //    ROSE_ASSERT(false);
          // }
        }
        }
        }
        pathContainer = newPathContainer;
        newPathContainer.clear();
        }
       // std::cout << "done while" << std::endl;
        pathContainer.clear();
    std::vector<std::vector<int> > finnpts;
    std::vector<std::vector<int> > npts;
    while (true) {
        if (paths.size() > 1000000) {
           std::cout << "too many paths, consider a subgraph" << std::endl;
           ROSE_ASSERT(false);
       }
       //#pragma omp parallel for schedule(guided)
       for (unsigned int qq = 0; qq < paths.size(); qq++) {
            std::vector<int> pq = paths[qq];
            std::vector<int> qp;
            int ppf = paths[qq].front();
            if (PtP.find(ppf) != PtP.end()) {
                for (unsigned int kk = 0; kk < PtP[ppf].size(); kk++) {
                    std::vector<int> newpath = /*unzipPath(*/PtP[ppf][kk];//, g, PtP[ppf][kk][0], PtP[ppf][kk][1]);
                    bool good = true;
                    if (newpath.back() == newpath.front() && newpath.front() != begin && newpath.size() > 1) {
                        good = false;
                    }
                    else {

                 //   if (find(pq.begin(), pq.end(), newpath.front()) != pq.end() && newpath.front() != begin) {
                 //         good = false;
                 //   }


                   // else {
                    for (unsigned int kk1 = 0; kk1 < newpath.size(); kk1++) {
                       
                      /*
                       if (newpath.front() == newpath.back()) {
                           good = false;
                           break;
                       }
                       else */if (find(pq.begin(), pq.end(), newpath[kk1]) != pq.end() && newpath[kk1] != begin) {
                           good = false;
                           break;
                          
                       }
                       }
                    //}
                    }
                    if (good) {
                       newpath.insert(newpath.end(), pq.begin(), pq.end());
                       #pragma omp critical
                       {
                       npts.push_back(newpath);
                       }
                    }
                }
            }
            else {
                std::vector<int> ppq = pq;// zipPath(pq, g, pq.front(), pq.back());
                #pragma omp critical
                {
                finnpts.push_back(ppq);
                }
            }
        }
        if (npts.size() == 0) {
            break;
        }
        else {
            paths = npts;
            npts.clear();
        }
    }
    paths = finnpts;    
    finnpts.clear(); 
    for (unsigned int k = 0; k < localLoops.size(); k++) {
        int lk = localLoops[k];
        std::vector<std::vector<int> > loopp;
        if (loopStore.find(localLoops[k]) != loopStore.end()) {
            loopp.insert(loopp.end(), loopStore[localLoops[k]].begin(), loopStore[localLoops[k]].end());
        }
        else {
        std::map<int, std::vector<std::vector<int> > > localLoopPaths;
        completedLoops.push_back(lk);
        recurses.push_back(lk);
        loopp = bfsTraversePath(lk, lk, g, true);
        recurses.pop_back();
        }
        for (unsigned int ik = 0; ik < loopp.size(); ik++) {
                
                if (find(globalLoopPaths[lk].begin(), globalLoopPaths[lk].end(), loopp[ik]) == globalLoopPaths[lk].end()) {
                globalLoopPaths[localLoops[k]].push_back(loopp[ik]);
                }
        }
    

 
    }
    borrowed = true;
    std::vector<std::vector<int> > lps2;
    unsigned int maxpaths = 1000;
    unsigned int pathdivisor = 1;//paths.size()/maxpaths;///paths.size();

    //if (pathdivisor < 1) {
        pathdivisor = 1;
        maxpaths = paths.size();
   // }
/*
    for (unsigned int j = 0; j < pathdivisor+1; j++) {
        std::vector<std::vector<int> > npaths;
        std::vector<int> dummyvec;
        unsigned int mxpths;
        if (j < pathdivisor) {
            mxpths = maxpaths;
        }
        else {
            mxpths = paths.size() % pathdivisor;
        }
        for (unsigned int k = 0; k < mxpths; k++) {
              npaths.push_back(paths.back());//unzipPath(paths.back(), g, begin, end));
              paths.pop_back();
        }
*/
        pathStore = paths;
        paths.clear();
    if (!recursedloop) {
    uTraversePath(begin, end, g, false, globalLoopPaths);
    }
    else {
    recursed++;
    
    std::set<std::vector<int> > lps = uTraversePath(begin, end, g, true, globalLoopPaths);
    recursed--;
    for (std::set<std::vector<int> >::iterator ij = lps.begin(); ij != lps.end(); ij++) {
    std::vector<int> ijk = (*ij);
    
    lps2.push_back(*ij);
    }
    } 
    //}
    #ifdef PERFDEBUG
 //   timeval tim; 
    //std::cout << "begin: " << begin << " end: " << end << std::endl;
    //gettimeofday(&tim, NULL);
    //double tim2 = tim.tv_sec+(tim.tv_usec/1000000);
    //double timeRet = tim2 - tim1;
    //std::cout << "bfs time elapsed: " << timeRet << std::endl;
    #endif
    return lps2;
    
}
             
        
/**
This function calculates all the permutations of loops on paths
it also throws away duplicate paths
Input:
@param[begin] integer representation of first node
@param[end] integer representation of the final node
@param[g] ambient CFG
@param[globalLoopPaths] connects an integer representation of a node to all possible loops starting at that node
*/


template<class CFG>
std::set<std::vector<int> >
SgGraphTraversal<CFG>::
uTraversePath(int begin, int end, CFG*& g, bool loop, std::map<int, std::vector<std::vector<int> > >& globalLoopPaths) {
    //std::cout << "uTraverse" << std::endl;
    //int doubledpaths = 0;
    int newmil = 1;
    //#ifdef LP
    //if (loop && loopStore.find(begin) != loopStore.end()) {
    //    return loopStore[begin];
    //}
    //#endif
    #ifdef PERFDEBUG
    //timeval tim;
    //gettimeofday(&tim, NULL);
    //double t1 = tim.tv_sec+(tim.tv_usec/1000000);
    #endif
    std::set<std::vector<int> > newpaths;
    std::set<std::vector<int> > npaths;
    pathnum = 0;
    std::vector<int> path;
    std::vector<std::vector<int> > paths;
    int truepaths = 0;
    std::vector<std::vector<int> > checkpaths;
    std::vector<std::vector<int> > npathchecker;
    std::map<int, int> currents;
    //int nnumpaths = 0;
    std::set<std::vector<int> > loopPaths;
    //bool threadsafe = true;
    bool done = false;
    std::set<std::vector<int> > fts;
    //double ttfors = 0;
    //double tperms = 0;
    while (true) {
        //std::cout << "paths.size() " << paths.size() << std::endl;
        if (paths.size() > 1000000) {
            std::cout << "nearly 1 million paths with no loops, stopping" << std::endl;
            return loopPaths;
            std::cout << "ended early" << std::endl;
        }
        if (done || borrowed) {
     
                if (borrowed) {
                    paths = pathStore;
                    pathStore.clear(); 
                }   
                //std::cout << "paths.size(): " << paths.size() << std::endl; 
                if (paths.size() != 0) {
                }
                else {
                return loopPaths;
                }
         
               // #pragma omp parallel
               // {
                #pragma omp parallel for schedule(guided) 
                for (unsigned int qqq = 0; qqq < paths.size(); qqq++) {
  //             std::cout << "pathcheck" << std::endl;
                //int pathevals = 0;
                //std::vector<int> zpt = zipPath2(paths[qqq], g); 
                //std::set<std::vector<int> > boxpaths;
                std::set<std::vector<int> > movepaths;
                std::vector<int> path;// = paths[qqq];
                path = paths[qqq];//unzipPath(paths[qqq], g, begin, end);
                truepaths++;
                   int permnums = 1;
                   std::vector<int> perms;
                    std::vector<unsigned int> qs;
                    std::map<int, std::vector<std::vector<int> > > localLoops;
                    std::vector<int> takenLoops;
                    takenLoops.push_back(path[0]);
                    bool taken = false;
                    //timeval timfor;
                    int lost = 0;
                    //gettimeofday(&timfor, NULL);
                    //double t1for = timfor.tv_sec + (timfor.tv_usec/1000000); 
                    for (unsigned int q = 1; q < path.size()-1; q++) {
                    //if (find(closures.begin(), closures.end(), path[q]) != closures.end()) {
                    if (globalLoopPaths.find(path[q]) != globalLoopPaths.end() /*&& find(lloops.begin(), lloops.end(), path[q]) != lloops.end()*/ && globalLoopPaths[path[q]].size() != 0 /*&& path[q] != begin && path[q] != end*/) {
                        for (unsigned int qp1 = 0; qp1 < globalLoopPaths[path[q]].size(); qp1++) {
                            
                            std::vector<int> gp = globalLoopPaths[path[q]][qp1]; //unzipPath(globalLoopPaths[path[q]][qp1],g,path[q],path[q]);
                           // std::vector<int> zgp = zipPath2(globalLoopPaths[zpt[q]][qp1], g);
                            for (unsigned int qp2 = 0; qp2 < takenLoops.size(); qp2++) {
                                if (find(gp.begin(),gp.end(), takenLoops[qp2]) != gp.end()) {
                                    taken = true;
                                }
                            }

                            if (!taken) {
                                localLoops[path[q]].push_back(gp);
                            }
                            else {
                               lost++;
                               taken = false;
                            }
                        }
                        if (localLoops[path[q]].size() != 0) {
                        takenLoops.push_back(path[q]);
                        permnums *= (localLoops[path[q]].size()+1);
                        perms.push_back(permnums);
                        qs.push_back(path[q]);
                        }
                    }
                    }
                    
                    //}
                    //if (loop) {
                    //std::cout << "lostloop: " << lost << std::endl;
                    //}
                    //else {
                    //std::cout << "lostpath: " << lost << std::endl;
                    //}
                    //std::cout << "endpathcheck" << std::endl;
                    //std::cout << "rest" << std::endl;
                    //std::cout << "permnums: " << permnums << std::endl;
                    //gettimeofday(&timfor, NULL);
                    //double t2for = timfor.tv_sec + (timfor.tv_usec/1000000);
                    //double ttfor = t2for - t1for;
                    //#pragma omp atomic
                    //ttfors += ttfor;
                    
                    //std::set<std::vector<int> > movepaths2;
                    std::set<std::vector<int> > movepathscheck;
                    //timeval timperms;
                    //gettimeofday(&timperms, NULL);
                   // double t1perm = timperms.tv_sec + (timperms.tv_usec/1000000);
                    std::vector<int> nvec;
                    std::vector<std::vector<int> > boxpaths(permnums, nvec);
                    //#pragma omp parallel for schedule(guided)
                    for (int i = 1; i <= permnums; i++) {
                        //bool goodthread = false;
                        std::vector<int> loopsTaken;
                        //bool stop = false;
                        unsigned int j = 0;
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
                        for (unsigned int j1 = 0; j1 <= j; j1++) {
                            pL.push_back(-1);
                        }
                           for (unsigned int k = j; k > 0; k--) {
                               int l = 1;
                               while (perms[k-1]*l < pn) {
                                   l++;
                              }
                               pL[k] = l-2;
                               pn -= (perms[k-1]*(l-1));
                           }
                        pL[0] = pn-2;

                        unsigned int q2 = 0;
                        for (unsigned int q1 = 0; q1 < path.size(); q1++) {
                            if (q2 < qs.size()) {
                                if (qs.size() != 0 && (unsigned)path[q1] == qs[q2] && (size_t)q2 != pL.size()) {
                                    if (pL[q2] == -1) {
                                        npath.push_back(path[q1]);
                                    }
                                    else {
                                        //   if (!stop) {
                                        npath.insert(npath.end(), localLoops[path[q1]][pL[q2]].begin(),
                                                     localLoops[path[q1]][pL[q2]].end());
                                        //  }
                                    }
                                    q2++;
                                }
                                else {
                                    npath.push_back(path[q1]);
                                }
                            }
                            else {
                                npath.push_back(path[q1]);
                            }
                        }
                        #ifdef FULLDEBUG
                        std::cout << "path: " << std::endl;
                        for (int qe = 0; qe < npath.size(); qe++) {
                            std::cout << ", " << npath[qe];
                        }
                        std::cout << std::endl;
                        std::cout << "permnum: " << i << std::endl; 
                        #endif
                      //  bool addit = false;
                        //if (!stop) {
                      //  if (loop && npath.front() == npath.back()) {
                      //      addit = true;
                      //  }
                      //  else if (!loop && bound && npath.front() == begin && npath.back() == end && npath.size() != 1) {
                      //      addit = true;
                      //  }
                      //  else if (!loop && !bound) {
                      //      addit = true;
                      //  }
                      // if (!addit) {
                      //     std::cout << "bad path" << std::endl;
                      // }
                       //bool extra = false;
                       //if (addit && !loop) {
                            //if (movepathscheck.find(npath) == movepathscheck.end()) {
                            //int mpc = movepathscheck.size();
                            //std::set<std::vector<int> > movepathspre = movepathscheck;
                    //        movepaths2.insert(npath);
                            //movepathscheck.insert(npath);
                            //ROSE_ASSERT(movepathscheck.size() == mpc || movepathspre.find(npath) == movepathspre.end());
                            //if (movepathscheck.size() == mpc) {
                            //    extra = true;
                           // }
                       
                            //}
                            //else {
                            //#pragma omp atomic
                           // doubledpaths++;
                           // }
                       //}
                            
                            //if (!workingthread || threadsafe) {
                            //if ((newpaths.size() > 1 || i == permnums || threadsafe)) {
                           // }
                          // }
                               
                       // }
                        //if (!extra)
                       // {
                        //if (movepaths2.size() > 0) //|| i == permnums || threadsafe)
                       // #pragma omp critical
                      // {
                       boxpaths[i-1] = npath;
                      // } 
                      // }
                       //std::cout << "endrest" << std::endl;
                       }
                        
                       evaledpaths += boxpaths.size();
                       if (evaledpaths > newmil*100000ull) {
                           //std::cout << "evaledpaths: " << evaledpaths << std::endl;
                           newmil++;
                       }
                       // #pragma omp critical
                      // {
                       if (!loop) {
                       for (std::vector<std::vector<int> >::iterator box = boxpaths.begin(); box != boxpaths.end(); box++) {
                       std::vector<Vertex> verts;
                       getVertexPath((*box), g, verts);
                       #pragma omp critical
                       { 
                       analyzePath(verts);
                       }
                       }
                       }
                       else {
                       #pragma omp critical
                       {
                       loopPaths.insert(boxpaths.begin(), boxpaths.end());;
                       }
                       }
                       }
                       }
                       //} 
                       
/* 
                      #pragma omp atomic
                      evaledpaths++; 
                      //pathevals++;
                      if (evaledpaths % 10000 == 0 && evaledpaths != 0) {
                          std::cout << "evaled paths: " << evaledpaths << std::endl;
                      }
                      if (!loop) {
                          std::vector<Vertex> verts;
                          getVertexPath(npath, g, verts);
                              #pragma omp critical
                              {
                              #ifdef FULLDEBUG
                              for (unsigned int aa = 0; aa < npath.size(); aa++) {
                                  if (ptsNum.find(npath[aa]) != ptsNum.end()) {
                                  ptsNum[npath[aa]] += 1;
                                  }
                                  else {
                                  ptsNum[npath[aa]] = 1;
                                  }
                              }
                              #endif
                              analyzePath(verts);
                              }
                      }
                      else if (loop)
                      { 
                          //std::vector<int> zpth = zipPath(npath, g, npath.front(), npath.back());
                          #pragma omp critical
                          {
                          loopPaths.insert(npath);//zipPath(npath, g, npath.front(), npath.back()));
                          }
                      }
                      else {
                      }
                      
                   }
*/

                  // movepaths2.clear();
                  
                 // std::cout << "permnums: " << permnums << std::endl;
                //  std::cout << "evaledpaths final: " << pathevals << std::endl;  
                  //gettimeofday(&timperms, NULL);
                  //double t2perm = timperms.tv_sec+(timperms.tv_usec/1000000);
                  //#pragma omp atomic
                  //tperms += t2perm - t1perm;
                 // }
                  //}
                  //}
                  //}
                  
                  
                  
                    

                    
                    #ifdef PERFDEBUG
                    //gettimeofday(&tim, NULL);
                   // double t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
                   // double tperm = t2 - t1perm
                    //double tX = t2 - t1;
                    //std::cout << "begin: " << begin << " end: " << end << std::endl;
                   // std::cout << "uTraverse time: " << tX << std::endl;
                   // std::cout << "tperms: " << tperms << std::endl;
                   // std::cout << "ttfors: " << ttfors << std::endl;
                   // std::cout << "doubledpaths: " << doubledpaths << std::endl;
                    #endif
                    #ifdef LP
                    if (loop) {
                   #ifdef PERFDEBUG
                   //  std::cout << "loopPaths: " << loopPaths.size() << std::endl;
                   #endif
                    loopStore[begin] = loopPaths;
                    }
                    #endif
                    return loopPaths;
                    
                    }
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
constructPathAnalyzer(CFG* g, bool unbounded, Vertex begin, Vertex end, bool ns) {
    abnormals = 0;
    normals = 0;
    if (ns) {
        needssafety = true;
    }
    else {
        needssafety = false;
    }
    checkedfound = 0;
    recursed = 0;
    nextsubpath = 0;
    borrowed = true;
    stoppedpaths = 0;
    evaledpaths = 0;
    badpaths = 0;
    sourcenum = 0;
    prepareGraph(g);
    workingthread = false;
    workingthreadnum = -1;
    //std::cout << "markers: " << markers.size() << std::endl;
    //std::cout << "closures: " << closures.size() << std::endl;
    //std::cout << "sources: " << sources.size() << std::endl;
    //std::cout << "sinks" << sinks.size() << std::endl;
//    printHotness(g);
    bool subgraph = false;
    if (!subgraph) {
            if (!unbounded) {
                bound = true;
                recursiveLoops.clear();
                recurses.clear();
                std::vector<std::vector<int> > spaths = bfsTraversePath(vertintmap[begin], vertintmap[end], g);
      //          std::cout << "spaths: " << spaths.size() << std::endl;
            }
            else {
                std::set<int> usedsources;
                bound = false;
                 std::vector<int> localLps;
                for (unsigned int j = 0; j < sources.size(); j++) {
                    sourcenum = sources[j];
                    recursiveLoops.clear();
                    recurses.clear();
                    std::vector<std::vector<int> > spaths = bfsTraversePath(sources[j], -1, g);
               
                    
                }
           }
    }
    //std::cout << "checkedfound: " << checkedfound << std::endl;
    printHotness(g);
}

/** DEPRECATED 
This is a function to construct subgraphs for parallelization
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
        int minDepth = 0;
        int maxDepth = minDepth + depthDifferential;
        int currSubGraph = 0;
        CFG* subGraph;
        std::set<int> foundNodes;
        while (true) {
            Vertex begin = boost::add_vertex(*subGraphVector[currSubGraph]);
            GraphSubGraphMap[currSubGraph][intvertmap[orderOfNodes[minDepth]]] = intvertmap[begin];
            SubGraphGraphMap[currSubGraph][intvertmap[begin]] = intvertmap[orderOfNodes[minDepth]];
        for (int i = minDepth; i <= maxDepth; i++) {
            Vertex v = GraphSubGraphMap[currSubGraph][intvertmap[orderOfNodes[i]]];
            std::vector<int> outEdges = getOutEdges(orderOfNodes[i], g);
            for (unsigned int j = 0; j < outEdges.size(); j++) {
                Vertex u;
                if (foundNodes.find(getTarget(outEdges[j], g)) == foundNodes.end()) {
                        u = GraphSubGraphMap[currSubGraph][intvertmap[getTarget(outEdges[j], g)]];
                }
                else {
                    u = boost::add_vertex(*subGraphVector[currSubGraph]);
                    foundNodes.insert(getTarget(outEdges[j], g));
                    SubGraphGraphMap[currSubGraph][u] = intvertmap[getTarget(outEdges[j], g)];
                    GraphSubGraphMap[currSubGraph][intvertmap[getTarget(outEdges[j], g)]] = u;

                }
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

       
/*
These should NOT be used by the user. They are simply for writing interesting information on the DOT graphs of the CFG
*/


 
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
            #ifdef FULLDEBUG
            int pts = ptsNum[cf];
            std::string nodeColor = "black";
            o << cf << " [label=\"" << " pts: " << pts <<  "\", color=\"" << nodeColor << "\", style=\"" << "solid" << "\"];\n";
            #endif
            #ifndef FULLDEBUG
            std::string nodeColor = "black";
            o << cf << " [label=\"" << " num:" << cf << "\", color=\"" << nodeColor << "\", style=\"" << "solid" << "\"];\n";
            #endif

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
            for (boost::tie(v, vend) = vertices(*gc); v != vend; ++v)
            {
                printCFGNode(vertintmap[*v], mf);
            }
            for (boost::tie(e, eend) = edges(*gc); e != eend; ++e)
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
            for (boost::tie(v, vend) = vertices(*gc); v != vend; ++v)
            {
                if (nodeStrings.find(vertintmap[*v]) != nodeStrings.end()) {
                    int nn = vertintmap[*v];
                    printCFGNodeGeneric(vertintmap[*v], nodeStrings[nn], mf);
                }
                else {
                    printCFGNodeGeneric(vertintmap[*v], "noprop", mf);
                }
            }
           for (boost::tie(e, eend) = edges(*gc); e != eend; ++e)
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
}


/**
DEPRECATED 
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
}

/**
This calculates nodes with more than one in edge or more than one out edge

SgGraphTraversal::findClosuresAndMarkers
Input:
@param[g] CFG*& g, CFG calculated previously
*/




template<class CFG>
void
SgGraphTraversal<CFG>::
findClosuresAndMarkersAndEnumerate(CFG*& g)
{
    edge_iterator e, eend;
    for (boost::tie(e, eend) = edges(*g); e != eend; ++e) {
        intedgemap[nextEdge] = *e;
        edgeintmap[*e] = nextEdge;
        nextEdge++;
    }
    vertex_iterator v1, vend1;
    for (boost::tie(v1, vend1) = vertices(*g); v1 != vend1; ++v1)
    {
        vertintmap[*v1] = nextNode;
        intvertmap[nextNode] = *v1;
        nextNode++;
    }
    vertex_iterator v, vend;
    for (boost::tie(v, vend) = vertices(*g); v != vend; ++v) {
        std::vector<int> outs = getOutEdges(vertintmap[*v], g);
        std::vector<int> ins = getInEdges(vertintmap[*v], g);
        if (outs.size() > 1)
        {
            markers.push_back(vertintmap[*v]);
            
            markerIndex[vertintmap[*v]] = markers.size()-1;
            for (unsigned int i = 0; i < outs.size(); i++) {
                pathsAtMarkers[vertintmap[*v]].push_back(getTarget(outs[i], g));
            }
        }
        if (ins.size() > 1)
        {
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



/** DEPRECATED 
Currently unused but will be necessary for parallelization in progress
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
        for (unsigned int i = 0; i < path.size(); i++) {
                            vertexPath.push_back(intvertmap[path[i]]);
        }

       
        
}

/**
DEPRECATED
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




                  
