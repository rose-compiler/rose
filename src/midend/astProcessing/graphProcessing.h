





// Original Author (SgGraphTraversal mechanisms): Michael Hoffman
//$id$
//#define PERFDEBUG 1
//#define LP 1
#ifdef _OPENMP
#include <omp.h>
#endif
#include <boost/regex.hpp>
#include <iostream>

#include <fstream>
#include <string>
#include <assert.h>
#include <staticCFG.h>





#include <boost/graph/adjacency_list.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
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
    int normals;
    int abnormals;
    bool needssafety;
    int recursed;
    int checkedfound;
    //std::map<int, std::set<std::vector<int> > > loopPaths;
    typedef typename boost::graph_traits<CFG>::vertex_descriptor Vertex;
    typedef typename boost::graph_traits<CFG>:: edge_descriptor Edge;
    std::vector<int> getInEdges(int& node, CFG*& g);
    std::vector<int> getOutEdges(int& node, CFG*& g);
    void prepareGraph(CFG*& g);
    void findClosuresAndMarkersAndEnumerate(CFG*& g);
    void constructPathAnalyzer(CFG* g, bool unbounded=false, Vertex end=0, Vertex begin=0, bool ns = true);
    virtual void analyzePath(std::vector<Vertex>& pth) = 0;
    void firstPrepGraph(CFG*& g);
    int stoppedpaths;
    std::set<std::vector<int> >  traversePath(int begin, int end, CFG*& g, bool loop=false);
    std::set<std::vector<int> > uTraversePath(int begin, int end, CFG*& g, bool loop, std::map<int, std::vector<std::vector<int> > >& localLoops);
    std::vector<std::vector<int> > bfsTraversePath(int begin, int end, CFG*& g, bool loop=false);
    std::vector<int> unzipPath(std::vector<int>&  path, CFG*& g, int start, int end);
    std::vector<int>  zipPath(std::vector<int>& path, CFG*& g, int start, int end);
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
    std::vector<int>  recursiveLoops;
    std::vector<int> recurses;
    bool borrowed;
    std::set<int> badloop;
    std::map<int, std::vector<std::vector<int> > >  totalLoops;
    int pathnum;
    std::map<int, std::string> nodeStrings;
    int sourcenum;
    int evaledpaths;
    int badpaths;
    int workingthreadnum;
    bool workingthread;
    std::map<int, std::set<std::vector<int> > > loopStore;
    std::vector<std::vector<int> >  pathStore;
    std::map<int, std::vector<int> > subpathglobal;
    std::map<std::vector<int>, int> subpathglobalinv;
    int nextsubpath;
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
std::vector<int>
SgGraphTraversal<CFG>::
zipPath(std::vector<int>& pth, CFG*& g, int start, int end) {
                        std::vector<int> subpath;
                        std::vector<int> movepath;
                        movepath.push_back(pth.front());
                        movepath.push_back(pth.back());
                        for (unsigned int qw = 0; qw < pth.size(); qw++) {
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

template<class CFG>
std::vector<int>
SgGraphTraversal<CFG>::
unzipPath(std::vector<int>& pzipped, CFG*& g, int start, int end) {
    // std::cout << "start: " << start << std::endl;
    // std::cout << "end: " << end << std::endl;
    // std::cout << "zipped: " << std::endl;
   //  for (unsigned int k = 0; k < zipped.size(); k++) {
   //      std::cout << ", " << zipped[k];
   //  }
    // std::cout << std::endl;
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


template<class CFG>
std::vector<std::vector<int> >
SgGraphTraversal<CFG>::
bfsTraversePath(int begin, int end, CFG*& g, bool loop) {
    #ifdef PERFDEBUG
    timeval tim;
    gettimeofday(&tim, NULL);
    double tim1 = tim.tv_sec+(tim.tv_usec/1000000.0);
    #endif
    bool recursedloop = loop;
    std::map<int, std::vector<std::vector<int> > > PtP;
    std::set<int> nodes;
    std::vector<std::vector<int> > pathContainer;
    std::vector<std::vector<int> > oldPaths;
    std::vector<std::vector<int> > npc;
    std::vector<int> bgpath;
    bgpath.push_back(begin);
    pathContainer.push_back(bgpath);
    std::vector<std::vector<int> > newPathContainer; 
    std::vector<std::vector<int> > paths;
    std::vector<int> localLoops;
    std::map<int, std::vector<std::vector<int> > > globalLoopPaths;
    //std::map<int, std::map<int, std::vector<std::vector<int> > > > locals;
    //std::map<int, std::map<int, std::vector<std::vector<int> > > > localsnew;
   //std::cout << "starting build" << std::endl;
     while (pathContainer.size() != 0 || oldPaths.size() != 0) {
    //std::cout << "pathContainer.size(): " << pathContainer.size() << std::endl;
   // #pragma omp parallel for schedule(guided)
        //std::cout << "pathContainer.size(): " << pathContainer.size() << std::endl;
        //std::cout << "oldPaths.size(): " << oldPaths.size() << std::endl;
    //   if (paths.size() % 1000000 == 0 && paths.size() != 0) {  
    //   std::cout << "paths.size(): " << paths.size() << std::endl;
    //   }
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
       //#pragma omp parallel for schedule(guided)
       for (unsigned int i = 0; i < pathContainer.size(); i++) {
       std::vector<int> npth = pathContainer[i];
        std::vector<int> oeds = getOutEdges(npth.back(), g);
        std::vector<int> ieds = getInEdges(npth.back(), g);
//if (pathContainer[i].size() > 1 && pathContainer[i].front() != pathContainer[i].back()) {
/*if (oeds.size() == 1 && ieds.size() == 1 && npth.back() != end) {// && !((!recursedloop && ((bound && npth.back() == end && npth.size() != 1) || (!bound && oeds.size() == 0))) || (recursedloop && npth.back() == end && npth.size() != 1))) {
std::vector<int> prepath = pathContainer[i];
std::vector<int> preieds;// = getInEdges(prepath.back(), g);
std::vector<int> preoeds = getOutEdges(prepath.back(), g);
int tgi;
std::vector<int> badvec;
if (preoeds.size() == 1 && prepath.back() != end) {
    tgi = getTarget(preoeds[0], g);
    preieds = getInEdges(tgi, g);
}
else {
    preieds = badvec;
}

//std::vector<int> npth = pathContainer[i];
//std::vector<int> oeds = getOutEdges(npth.back(), g);
if (prepath.size() > 1 && preieds.size() == 1 && preoeds.size() == 1) {
//std::cout << "begin" << std::endl;
preoeds = getOutEdges(prepath.back(), g);
ROSE_ASSERT(preoeds.size() == 1);
int tgi2 = getTarget(preoeds[0], g);
preieds = getInEdges(tgi, g);
ROSE_ASSERT(tgi2 = tgi);
while (preieds.size() == 1 && preoeds.size() == 1 && (prepath.back() != end && tgi != end && find(prepath.begin(), prepath.end(), tgi) == prepath.end())) {
            tgi = getTarget(preoeds[0], g);
  //          std::cout << "tgi: " << tgi << std::endl;
            prepath.push_back(tgi);
            //preieds = getInEdges(tgi, g);
            preoeds = getOutEdges(tgi, g);
            if (preoeds.size() == 1 && prepath.back() != end) {
                tgi = getTarget(preoeds[0], g);
                preieds = getInEdges(tgi, g);
            }
            else {
                preieds = badvec;
            }
}

//if (prepath.back() == end && prepath.size() != 1) {
//    prepath.pop_back();
//}
std::cout << "prepath: " << std::endl;
for (int qk = 0; qk < prepath.size(); qk++) {
std::cout << ", " << prepath[qk];
}
std::cout << std::endl;
if (prepath.size() > 1) {
pathContainer[i] = prepath;
}
}
}
*/


 
       // std::cout << "pathContainer.size(): " << pathContainer.size() << std::endl;
        npth = pathContainer[i];
        oeds = getOutEdges(npth.back(), g);
        if ((!recursedloop && ((bound && npth.back() == end && npth.size() != 1) || (!bound && oeds.size() == 0))) || (recursedloop && npth.back() == end && npth.size() != 1)) {
            std::vector<int> newpth;
            newpth = (pathContainer[i]);
             std::vector<int> movepath = newpth;
             
             if (recursedloop && newpth.back() == end && newpth.size() != 1) {
             //#pragma omp critical
            // {
             paths.push_back(movepath);
            // }
             }
             else if (!recursedloop) {
             if (bound && newpth.size() != 1 && newpth.back() == end) {
            // #pragma omp critical
           // {
             paths.push_back(movepath);
           // }
             }
             else if (!bound) {
           //  #pragma omp critical
           //  {
             paths.push_back(movepath);
           //  }
             }
             }
             
             
              
            
             
        }
        else {
std::vector<int> oeds = getOutEdges(pathContainer[i].back(), g);

        for (unsigned int j = 0; j < oeds.size(); j++) {



            int tg = getTarget(oeds[j], g);
            if (getTarget(oeds[j], g) == end && end != -1 && (!recursedloop || (pathContainer[i].size() != 1))) {
                std::vector<int> pth = (pathContainer[i]);
                pth.push_back(getTarget(oeds[j], g));
               std::vector<int> movepath;
                        for (int qw = 0; qw < pth.size()-1; qw++) {
                            if (qw == 0 && find(markers.begin(), markers.end(), pth[qw]) == markers.end()) {
                                movepath.push_back(pth[qw]);
                            }
                            if (find(markers.begin(), markers.end(), pth[qw]) != markers.end()) {
                               //std::pair<int, int> mpcheck;
                               movepath.push_back(pth[qw]);
                               if (find(markers.begin(), markers.end(), pth[qw+1]) == markers.end() && qw+1 < pth.size()) {
                               movepath.push_back(pth[qw+1]);
                               }
                            }
                        }
                #pragma omp critical
                {
                if (find(paths.begin(), paths.end(), movepath) == paths.end()) {
                if (recursedloop && (pth.front() == pth.back())) {
                paths.push_back(movepath);
                }
                else if (!recursedloop) {
                if (bound && pth.front() == begin && pth.back() == end) {
                paths.push_back(movepath);
                }
                else if (!bound) {
                paths.push_back(movepath);
                }
                }
                }
                }
                
            }
            else {
*/
int tg = getTarget(oeds[j], g);
            
>>>>>>> Some traversal issues fixed. Support for interprocedural CFGs added.

            std::vector<int> newpath = (pathContainer[i]);
            if (nodes.find(tg) != nodes.end() && find(newpath.begin(), newpath.end(), tg) == newpath.end() && tg != end) {
                if (PtP.find(tg) == PtP.end()) {
                    std::vector<int> nv;
                    nv.push_back(tg);
                    newPathContainer.push_back(nv);
                    PtP[tg].push_back(/*zipPath(*(*/newpath);//, g, newpath.front(), newpath.back()));
                }
                else {
                   // ROSE_ASSERT(find(PtP[tg].begin(), PtP[tg].end(), newpath) == PtP[tg].end())
                    PtP[tg].push_back(/*zipPath(*/newpath);//, g, newpath.front(), newpath.back()));
                }
            }
            else if (find(newpath.begin(), newpath.end(), getTarget(oeds[j], g)) == newpath.end() || getTarget(oeds[j], g) == end) {
                newpath.push_back(tg);
                std::vector<int> ieds = getInEdges(tg, g);
                if (ieds.size() > 1) {//find(closures.begin(), closures.end(), tg) != closures.end()) {
                nodes.insert(tg);
                }
               // }
             //   #pragma omp critical
             //   {
                newPathContainer.push_back(newpath);
             //   }
            }
            else if (tg == end  && recursedloop) {
                newpath.push_back(tg);
                //if (find(closures.begin(), closures.end(), tg) != closures.end()) {
                //nodes.insert(tg);
               // }
               // #pragma omp critical
               // {
                newPathContainer.push_back(newpath);
               // }
            }
            else if (find(newpath.begin(), newpath.end(), tg) != newpath.end() && tg != end) { 
                std::vector<int> ieds = getInEdges(tg, g);
                if (ieds.size() > 1/*find(closures.begin(), closures.end(), tg) != closures.end()*/ && find(localLoops.begin(), localLoops.end(), tg) == localLoops.end() && find(recurses.begin(), recurses.end(), tg) == recurses.end()) {
                 //  #pragma omp critical
                 //  {
                   localLoops.push_back(tg);
                 //  }
                   nodes.insert(tg);
                }
                else if (find(recurses.begin(), recurses.end(), tg) != recurses.end()) {
                    //std::cout << "found recursed: " << tg << std::endl;
                }
           }
           else {
               std::cout << "problem" << std::endl;
               ROSE_ASSERT(false);
           }
        }
        }
        }
       
        pathContainer = newPathContainer;
        newPathContainer.clear();
        }
        pathContainer.clear();
    std::vector<std::vector<int> > finnpts;
    std::vector<std::vector<int> > npts;
   // std::vector<std::vector<int> > ntg;
   // std::vector<std::vector<int> > tgpaths;
    //std::cout << "completedbuild, begin while" << std::endl;
    while (true) {
    //   std::cout << "paths.size(): " << paths.size() << std::endl;
        if (paths.size() > 10000000) {
           std::cout << "too many paths, consider a subgraph" << std::endl;
           ROSE_ASSERT(false);
       }
       //int normals = 0;
       //int abnormals = 0;
       //#pragma omp parallel for schedule(guided)
       for (unsigned int qq = 0; qq < paths.size(); qq++) {
       // while (true) {
            std::vector<int> pq = paths[qq];
            std::vector<int> qp;
           // #pragma omp critical
           // {
            //std::cout << "pq: " << std::endl;
           // for (int j = 0; j < pq.size(); j++) {
           //     std::cout << pq[j] << ", ";
           // }
           // std::cout << "end path" << std::endl;
           // }
          //  if (tgpaths.size() != 0) {
          //  qp = tgpaths[qq];
          //  }
            int ppf = paths[qq].front();
            if (PtP.find(ppf) != PtP.end()) {
                for (unsigned int kk = 0; kk < PtP[ppf].size(); kk++) {
                    std::vector<int> newpath = /*unzipPath(*/PtP[ppf][kk];//, g, PtP[ppf][kk][0], PtP[ppf][kk][1]);
                    bool good = true;
                    if (newpath.back() == newpath.front() && newpath.front() != begin && newpath.size() > 1) {
                        good = false;
                        //#pragma omp critical
                       // {
                       // std::cout << "shouldn't occur" << std::endl;
                       // }
                    }
                    else {

                    if (find(pq.begin(), pq.end(), newpath.front()) != pq.end() && newpath.front() != begin) {
                         //#pragma omp critical
                        // {
                        // normals++;
                         //std::cout << "normal" << std::endl;
                        // }
                          good = false;
                    }


                    else {
                    for (unsigned int kk1 = 0; kk1 < newpath.size(); kk1++) {
                       //if (tgpaths.size() == 0) {
                       //if (find(pq.begin(), pq.end(), newpath[kk1]) != pq.end() && newpath[kk1] != begin) {
                      //     good = true;
                      //     break;
                       //}
                       //}
                       
                       //else {
                      /*
                       if (newpath.front() == newpath.back()) {
                           //std::cout << "bf" << std::endl;
                           good = false;
                           break;
                       }
                       else */if (find(pq.begin(), pq.end(), newpath[kk1]) != pq.end() && newpath[kk1] != begin) {
                           //std::cout << "badpath" << std::endl;
                           //for (int i = 0; i < newpath.size(); i++) {
                           //    std::cout << newpath[i] << ", ";
                          // }i
                          // std::cout << "end path" << std::endl;
                          // std::cout << "nobf" << std::endl;
                           good = false;
                           //#pragma omp critical
                          // {
                          // abnormals++;
                           ///std::cout << "abnormal" << std::endl;
                          // }
                           break;
                          
                       }
                       }
                    }
                   // }
                    }
                    if (good) {
                       newpath.insert(newpath.end(), pq.begin(), pq.end());
                       #pragma omp critical
                       {
                       npts.push_back(newpath);
                       //std::vector<int> qpp = qp;
                       //qpp.push_back(newpath.front());
                       //ntg.push_back(qpp);
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
            //tgpaths = ntg;
            paths = npts;
            //ntg.clear();
            npts.clear();
        }
    }
    //std::cout << "completewhile" << std::endl;
    paths = finnpts;    
    finnpts.clear(); 
    //for (unsigned int k1 = 0; k1 < localLoops.size(); k1++) {
    //    recurses.push_back(localLoops[k1]);
   // }
    for (unsigned int k = 0; k < localLoops.size(); k++) {
        int lk = localLoops[k];
        std::vector<std::vector<int> > loopp;
        
        
       
       
        std::map<int, std::vector<std::vector<int> > > localLoopPaths;
        
        recurses.push_back(lk);
        loopp = bfsTraversePath(lk, lk, g, true);
       // recurses.pop_back();
        //}
        //globalLoopPaths[localLoops[k]] = loop;
        for (unsigned int ik = 0; ik < loopp.size(); ik++) {
                
                
                
                
                
                
                if (find(globalLoopPaths[lk].begin(), globalLoopPaths[lk].end(), loopp[ik]) == globalLoopPaths[lk].end()) {
                globalLoopPaths[localLoops[k]].push_back(loopp[ik]);
                
                
                
               
               
                
                }
        }
        
    
    

 
        
    }
    
    
    borrowed = true;
   
    std::vector<std::vector<int> > lps2;
    unsigned int pathdivisor = 100;
    unsigned int maxpaths = paths.size()/pathdivisor;

    if (maxpaths < 100) {
        pathdivisor = 1;
        maxpaths = paths.size();
    }
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
            
              npaths.push_back(paths.back());
              paths.pop_back();
       
       
       
        }

        pathStore = npaths;
        npaths.clear();
    //pathStore = paths;
    //std::vector<std::vector<int> > lps2;
    if (!recursedloop) {
    uTraversePath(begin, end, g, false, globalLoopPaths);
    }
    else {
    //const std::map<int, std::vector<std::vector<int> > > glp = globalLoopPaths;
    //std::cout << "recursed: " << recursed << std::endl;
    recursed++;
    
    std::set<std::vector<int> > lps = uTraversePath(begin, end, g, true, globalLoopPaths);
    recursed--;
    
    for (std::set<std::vector<int> >::iterator ij = lps.begin(); ij != lps.end(); ij++) {
    std::vector<int> ijk = (*ij);
    
   
    
    lps2.push_back(*ij);
    }
    //lps.clear();
    } 
    }
    //std::cout << "evaledpaths: " << evaledpaths << std::endl;
    //}
    #ifdef PERFDEBUG
    std::cout << "begin: " << begin << " end: " << end << std::endl;
    gettimeofday(&tim, NULL);
    double tim2 = tim.tv_sec+(tim.tv_usec/1000000);
    double timeRet = tim2 - tim1;
    std::cout << "bfs time elapsed: " << timeRet << std::endl;
    #endif
    //std::cout << "normals: " << normals << " abnormals: " << abnormals << std::endl;
    return lps2;
    

}
             
        
std::vector<int> increment(std::vector<int> toInc, std::vector<int> help) {
  int i = 0;
  toInc[0] += 1;
  ROSE_ASSERT(toInc.size() == help.size());
  while (true) {
      if (i == toInc.size()) {
          toInc.back() = -10;
          break;
      }
      else if (toInc[i] == help[i]-1) {
         //if (i == toInc.size()) {
         //    toInc.back() = -10;
        // }
        // else {
         toInc[i+1] += 1;
         for (int q = 0; q < i+1; q++) {
             toInc[q] = -1;
         }
        // }
      }
      else {
          break;
      }
      i++;
  }
 // std::cout << "toInc: " << std::endl;
 // for (int k = 0; k < toInc.size(); k++) {
 //     std::cout << ", " << toInc[k];
 // }
 // std::cout << std::endl;
  return toInc;
}
    


template<class CFG>
std::set<std::vector<int> >
SgGraphTraversal<CFG>::
uTraversePath(int begin, int end, CFG*& g, bool loop, std::map<int, std::vector<std::vector<int> > >& globalLoopPaths) {
    //std::set<std::vector<int> > movepaths;
    //std::set<std::vector<int> > movepaths2;
    //int rP = 0;
    //int bA = 0;
    //int tl = 0;
    #ifdef LP
    if (loopStore.find(begin) != loopStore.end()) {
        return loopStore[begin];
    }
    #endif
    #ifdef PERFDEBUG
    timeval tim;
    gettimeofday(&tim, NULL);
    double t1 = tim.tv_sec+(tim.tv_usec/1000000);
    #endif
    std::set<std::vector<int> > newpaths;
    //int checkedfound = 0;
    //workingthread = -1;:
    //int lpsToGo = 0;
    //std::cout << "beginning : " << begin << " end: " << end << std::endl;
    std::set<std::vector<int> > npaths;
    //int pathcount = 0;
    //int npathnum = 1;
    pathnum = 0;
    //double globalMin = 1;
    std::vector<int> path;
    std::vector<std::vector<int> > paths;
    int truepaths = 0;
    
    std::vector<std::vector<int> > checkpaths;
    //int repeats = 1;
    std::vector<std::vector<int> > npathchecker;
    
    
     
    std::map<int, int> currents;
    int nnumpaths = 0;
    std::set<std::vector<int> > loopPaths;
    //int rounds = 0;
    //int oldsize = 0;
    bool threadsafe = true;
    bool done = false;
    std::set<std::vector<int> > fts;

    while (true) {
        
        
         
        
        if (paths.size() > 100000) {
            std::cout << "nearly 1 million paths with no loops, stopping" << std::endl;
            return loopPaths;
            std::cout << "ended early" << std::endl;
        }
        if (done || borrowed) {
     
                if (borrowed) {
                    paths = pathStore;
                    pathStore.clear(); 
                }   
 
                if (paths.size() != 0) {
                //std::cout << "in done: front = " << begin << " back = " << end << " paths.size() = " << paths.size() << std::endl;
                }
                else {
                return loopPaths;
                }
             
                //std::set<std::vector<int> > movepathscheck; 
         
                #pragma omp parallel
                {
                #pragma omp for schedule(guided) 
                for (unsigned int qqq = 0; qqq < paths.size(); qqq++) {
                 
                
                std::set<std::vector<int> > movepaths;
                //std::cout << "nnumpaths: " << nnumpaths << std::endl;
                //std::cout << "pathnum: " << qqq << std::endl;
                std::vector<int> path;// = paths[qqq];
                //#pragma omp critical
               // {
                path = paths[qqq];//unzipPath(paths[qqq], g, begin, end);
               // }
                //std::cout << "unzipping" << std::endl;,
                //std::vector<int> path = unzipPath(prepath, g, begin, end);
                //std::cout << "unzipped path: " << std::endl;
                //for (int jw = 0; jw < path.size(); jw++) {
                //    std::cout << path[jw] << ", ";
               // }
                //std::cout << "end" << std::endl;
                //if (paths.size() == 1 && paths[0].size() == 1) {
                //    std::set<std::vector<int> > nullvec;
                    //return nullvec;
                //}
                //movepaths.clear();
                truepaths++;
                //std::vector<std::vector<int> > nPaths;
                //std::vector<int> subpath;
                //subpath.push_back(path[0]);
                //nPaths.push_back(subpath);
                //std::vector<std::vector<int> > newNPaths;
                   int permnums = 1;
                   std::vector<int> perms;
                    std::vector<unsigned int> qs;
                    std::map<int, std::vector<std::vector<int> > > localLoops;
                    std::vector<int> takenLoops;
                    bool taken = false;
                    
                    //std::cout << "forming dumpedNodes" << std::endl;
                     std::vector<int> help;
                     std::vector<int> pL;
                    for (unsigned int q = 0; q < path.size(); q++) {
                    //if (dumpedNodes.find(path[q]) == dumpedNodes.end()) {
                    if (q != 0 && q != path.size()-1 && globalLoopPaths.find(path[q]) != globalLoopPaths.end() /*&& find(lloops.begin(), lloops.end(), path[q]) != lloops.end()*/ && globalLoopPaths[path[q]].size() != 0 /*&& path[q] != begin && path[q] != end*/) {
                    //    for (int qL = 0; qL < globalLoopPaths[path[q]].size(); qL++) {
                    //        for (int qL2 = 0; qL2 < globalLoopPaths[path[q]][qL].size(); qL2++) {
                    //            dumpedNodes.insert(globalLoopPaths[path[q]][qL][qL2]);
                    //        }
                    //   }
                        for (unsigned int qp1 = 0; qp1 < globalLoopPaths[path[q]].size(); qp1++) {
                       //     std::vector<int> gp = unzipPath(globalLoopPaths[path[q]][qp1], g, path[q]);
                            //if (gp.back() != gp.front()) {
                            //    taken = true;
                           // }
                           // if (gp.back() != gp.front()) {
                           //     gp.push_back(gp.front());
                           // }
               //            (!taken) {
                            //std::cout << "looped path unzipped: " << std::endl;
                            //for (int jw1 = 0; jw1 < gp.size(); jw1++) {
                            //    std::cout << gp[jw1] << ", ";
                           // }
                           // std::cout << "end" << std::endl;
                             
                            std::vector<int> gp = unzipPath(globalLoopPaths[path[q]][qp1],g,path[q],path[q]);

                            for (unsigned int qp2 = 0; qp2 < takenLoops.size(); qp2++) {
                              
                                if (find(gp.begin(),gp.end(), takenLoops[qp2]) != gp.end()) {
                                    taken = true;
                                    break;
                                }
                            }

                            if (!taken) {
                                //std::cout << "loop being taken" << std::endl;
                                localLoops[path[q]].push_back(gp);
                            }
                            else {
                               //tl++;
                               //std::cout << "already taken" << std::endl;
                               taken = false;
                            }
                        }
                       
                        if (localLoops[path[q]].size() != 0) {
                        takenLoops.push_back(path[q]);
                        //permnums *= (localLoops[path[q]].size()+1);
                        
                        help.push_back(localLoops[path[q]].size()+1);
                        if (loop) {
                            help.back() -= 1;
                        }
                        //perms.push_back(permnums);
                        qs.push_back(q);
                        pL.push_back(-1);
                        }
                    }
                    }
                    ROSE_ASSERT(qs.size() == pL.size());
                    //std::cout << "permnums: " << permnums << std::endl;
                    
                    //std::cout << "completedFillingDumpedNodes" << std::endl;
                    //permnums -= 1;
                    //if (permnums != 1) {
                    //std::cout << "permnums: " << permnums << std::endl;
                    //}
                    //#pragma omp parallel
                   // {
                  //  #pragma omp for
                    //std::set<std::vector<int> > movepaths2;
                    std::set<std::vector<int> > movepathscheck;
                    std::vector<int> init;
                    //for (int i = 0; i < qs.size(); i++) {
                    //    pL.push_back(-1);
                    //}
                    bool fd = false;
                    bool nten = false;
                    while (!fd) {//pL.size() != 0 && pL.back() != -10) {
                        if (pL.size() == 0) {
                            fd = true;
                        }
                        else if (pL.back() == -10) {
                            nten = true;
                            break;
                        }
                    //for (int i = 1; i <= permnums; i++) {

                     

                        bool run = false;
                        bool goodthread = false;
                        
                        std::vector<int> loopsTaken;
                        bool stop = false;
                        unsigned int j = 0;
                        std::vector<int> npath;
//                        while (true) {
//                            if (j == perms.size() || perms[j] > i) {
//                                break;
//                            }
//                            else {
//                                j++;
//                            }
//                        }
//                        int pn = i;
//                        std::vector<int> pL;
//                        for (unsigned int j1 = 0; j1 <= j; j1++) {
 //                           pL.push_back(-1);
 //                       }
 //                          for (unsigned int k = j; k > 0; k--) {
 //                              int l = 0;
  //                             while (perms[k-1]*l < pn) {
   //                                l++;
   //                           }
   //                            pL[k] = l;
    //                           pn -= (perms[k-1]*(l));
    //                       }
     //                   pL[0] = pn-2;

                        unsigned int q2 = 0;
                        //std::vector<int> npathi;
                        if (qs.size() != 0) {
                        for (unsigned int q1 = 0; q1 < path.size(); q1++) {
                            
                            
                            if (qs.size() != 0 && q1 == qs[q2] && q2 != pL.size()) {
                               
                               if (pL[q2] == -1) {
                                   npath.push_back(path[q1]);
                               }
                               else {
 
                                  if (!stop) {
                                   npath.insert(npath.end(), localLoops[path[q1]][pL[q2]].begin(), localLoops[path[q1]][pL[q2]].end());
                                   
                                   }
                                   
                                   
                                   
                                   
                                  
                               }
                               q2++;
                            }
                            else {
                               npath.push_back(path[q1]);
                            }
                            
                            
                            
                               
                               
                                
                                
                                
                               
                               
                               
                                
                                
                            
                        }
                        #pragma omp critical
                        {
                        pL = increment(pL, help);
                        }
                        }
                        else {
                          nten = true;
                            npath = path;
                        }
                        //std::vector<Vertex> verts;
                        //std::vector<int> movepath;
                        /* 
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
                    */
//                    std::vector<int>  movepath;
//
//                    if (!loop) {
//                    #pragma omp critical
//                    {
//                    movepath = zipPath(npath, g); 
//                    }
//                    }
                    //std::vector<std::vector<bool> > movepath
/*
                    for (int qw = 0; qw < npath.size()-1; qw++) {
                        movepath[npath[qw]][npath[qw+1]] = true;
                    }
*/
//                    if (movepaths.find(movepath) != movepaths.end() && !loop) {
//                        stop = true;
//                        #pragma omp atomic
//                        badpaths++;
//                    }
//                    else if (!loop) {
 //                       movepaths.insert(movepath);
 //                   }  
/*
                        std::cout << "begin move: ";
                        for (int qe = 0; qe < movepath.size(); qe++) {
                            std::cout << ", " << movepath[qe];
                        }
                        std::cout << "end" << std::endl;
                        std::cout << "begin orig: ";
                        for (int qe2 = 0; qe2 < npath.size(); qe2++) {
                            std::cout << ", " << npath[qe2];
                        }
*/
                        //std::cout << "end" << std::endl;
 
                        bool addit = false;
                        //if (movepath.back() != npath[npath.size()-1]) {
                        //    movepath.push_back(npath[npath.size()-1]);
                        // 
                        if (!stop) {
                        if (loop && npath.front() == npath.back()) {
                            addit = true;
                        }
                        else if (!loop && bound && npath.front() == begin && npath.back() == end && npath.size() != 1) {
                            addit = true;
                        }
                        else if (!loop && !bound) {
                            addit = true;
                        }
                        else {
                            //bA++;
                        }
                       // #pragma omp critical
                       // {
                        //if (movepaths.find(movepath) == movepaths.end() && !stop) {
                       //     if (!loop) { 
                       //     movepaths.insert(movepath);
                       //     }
                       run = false;
                       if (addit) {
                       //#pragma omp critical
                      // {
                            if (movepathscheck.find(npath) == movepathscheck.end()) {
                          //   movepaths2.insert(npath);
                             run = true;
                            movepathscheck.insert(npath);
                           }
                           // else {
                               //#pragma omp critical
                                //{
                               // if (nten) {
                                //std::cout << "nten" << std::endl;
                                //}
                               // std::cout << "npath.front(): " << npath.front() << ", npath.back() " << npath.back() << std::endl; 
                               // std::cout << "path[" << qqq << "] of " << paths.size() << std::endl;//<< " permnum: " << i << " of " << permnums <<  std::endl;
                                
                              // rP++;
                               // }
                           // }
                           // else {
                           // #pragma omp atomic
                           // checkedfound++;
                            //std::cout << "checkedfound: " << checkedfound << std::endl;
                           // }
                       //}
                       }
                            
                            //if (!workingthread || threadsafe) {
                            //if ((newpaths.size() > 1 /*|| i == permnums*/ || threadsafe)) {
                                //std::cout << "newpaths.size(): " << newpaths.size() << std::endl;
                        //        workingthread = true;
                          //      goodthread = true;
                            //    workingthreadnum = omp_get_thread_num();
                             //   movepaths2 = newpaths;
                             //   newpaths.clear();
                           // }
                           // }
                               
                          
                        
                        
                            
                          
                       
                        }
                       // }
                        //int th_id = omp_get_thread_num();
                        if (run)
                        {
                        if (run) //|| i == permnums || threadsafe)
                        {
                        
                        
                        
                        
                        
                    
                    //if (!loop) {
                    //}
                    //int evaledpaths = 0;
                    //for (std::set<std::vector<int> >::iterator qr = movepaths2.begin(); qr != movepaths2.end(); qr++) {
                       // std::vector<int> npath = *qr;
                        //std::cout << "start";
                        //for (int jw2 = 0; jw2 < npath1.size(); jw2++) {
                        //    std::cout << ", " << npath[jw2];
                       // }
                       // std::cout << "end" << std::endl;
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

                      
                      
                      
                      //nnumpaths++;
                      //bool analyzed = false;
                      //if (evaledpaths % 100000 == 0 && evaledpaths != 0) {
                      //std::cout << "evaled paths: " << evaledpaths << std::endl;
                     // }
                     // #pragma omp critical
                     // {
                     // #pragma omp critical
                    //  {
                      //std::cout << "path: " << std::endl;
                      //for (int qq = 0; qq < npath.size(); qq++) {
                      //    std::cout << npath[qq] << ", ";
                    //  }
                     // std::cout << std::endl;
                      evaledpaths++; 
                      if (evaledpaths % 1000000 == 0 && evaledpaths != 0) {
                          std::cout << "evaled paths: " << evaledpaths << std::endl;
                          //std::cout << "badpaths: " << badpaths << std::endl;
                      }
                      //}
                      if (!loop) { //&& (bound && npath.front() == begin &&  npath.back() == end && npath.size() > 1)) {
                           //nnumpaths++;
                          std::vector<Vertex> verts;
                          getVertexPath(npath, g, verts);
                        //  if (needssafety) {
                              #pragma omp critical
                              {
                              analyzePath(verts);
                              }
                        // }
                        // else { 
                        //  #pragma omp critical
                        //  { 
                        //  analyzePath(verts);
                        //  }
                        // }
                          //analyzed = true;
                          //pathnum++;
                      //}
                      //else if (!loop && !bound) {
                          //nnumpaths++;
                      //    std::vector<Vertex> verts;
                      //    getVertexPath(npath, g, verts);
                         // #pragma omp critical
                         // {
                       //   analyzePath(verts);
                         // }
                         // analyzed = true;
                          //pathnum++;
                      }
                      else if (loop)
                      { //&& npath.front() == npath.back() && loopPaths.find(npath) == loopPaths.end()) {
                          std::vector<int> zpth = zipPath(npath, g, npath.front(), npath.back());
                          #pragma omp critical
                          {
                          loopPaths.insert(zpth);//zipPath(npath, g, npath.front(), npath.back()));
                          }
                       
                      }
                      //else {
                         // std::cout << "rejected path: " << std::endl;
                         // for (int qwe = 0; qwe < npath.size(); qwe++) {
                         //     std::cout << npath[qwe] << ", ";
                         // }
                         // std::cout << std::endl;
                     // }
                      //if (!analyzed) {
                      //    badpaths++;
                     // }
                     // }
/*
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
                      
                      
                     
                     

                      
                   //}
                   }
                   //movepaths2.clear();
                   //workingthread = false; 
                   //workingthreadnum = -1;
                   //movepaths2.clear();
                  }  
                  }
                  }
                  } 
if (newpaths.size() != 0) {
                    std::cout << "went too far" << std::endl; 
                    //int evaledpaths = 0;
                    for (std::set<std::vector<int> >::iterator qw = newpaths.begin(); qw != newpaths.end(); qw++) {
                    std::vector<int> npath = *qw;
                    bool analyzed = false;
                   *//* 
                    #pragma omp critical
                     {
                      bool analyzed = false;
                      if (evaledpaths % 1000 == 0 && evaledpaths != 0) {
                      std::cout << "evaled paths: " << evaledpaths << std::endl;
                      }
                      evaledpaths++;
                     }
                    */
                      /*if (!loop && (bound && npath.front() == begin &&  npath.back() == end)) {
                           nnumpaths++;
                          std::vector<Vertex> verts;
                          getVertexPath(npath, g, verts);
                          analyzePath(verts);
                          analyzed = true;
                      }*//*
                      if (!loop) {
                          nnumpaths++;
                          std::vector<Vertex> verts;
                          getVertexPath(npath, g, verts);
                          analyzePath(verts);
                          analyzed = true;
                      }
                      else if (loop *//*&& npath.front() == npath.back() && loopPaths.find(npath) == loopPaths.end()*//*) {
                          std::vector<int> nn = zipPath(npath, g, npath.front(), npath.back());
                          if (loopPaths.find(nn) != loopPaths.end()) {
                              badpaths++;
                          }
                          else {
                          loopPaths.insert(nn);
                          } 
                          analyzed = true;
                      }
                      else {
                          
                          
                          
                         
                         
                      }
                   if (!analyzed) {
                   badpaths++;
                   }
                   }
                   
                   }
                   */
                   
                   
                   }
                    

                    
                   // std::cout << "nnumpaths: " << nnumpaths << std::endl;
                    #ifdef PERFDEBUG
                    gettimeofday(&tim, NULL);
                    double t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
                    double tX = t2 - t1;
                    std::cout << "begin: " << begin << " end: " << end << std::endl;
                    std::cout << "uTraverse time: " << tX << std::endl; 
                    #endif
                    #ifdef LP
                    if (loop) {
                    std::cout << "loopPaths: " << loopPaths.size() << std::endl;
                    loopStore[begin] = loopPaths;
                    }
                    #endif
                    //std::cout << "bA: " << bA << ", rP: " << rP << ", tl: " << tl << std::endl;
                    //std::cout << "stoppedpaths: " << stoppedpaths << std::endl;
                    return loopPaths;
                    
                    
                    }
         
         
                    
                    
                     
                    
                                                                                                                                                                                                   



            
        


}
                
            






template<class CFG>
void
SgGraphTraversal<CFG>::
constructPathAnalyzer(CFG* g, bool unbounded, Vertex begin, Vertex end, bool ns) {
    //sets up a few data structures to ease computations in future functions
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
    std::cout << "markers: " << markers.size() << std::endl;
    std::cout << "closures: " << closures.size() << std::endl;
    std::cout << "sources: " << sources.size() << std::endl;
    std::cout << "sinks" << sinks.size() << std::endl;
    printHotness(g);
    
    bool subgraph = false;
    if (!subgraph) {
        
            if (!unbounded) {
                bound = true;
                
                
                recursiveLoops.clear();
                recurses.clear();
                std::vector<std::vector<int> > spaths = bfsTraversePath(vertintmap[begin], vertintmap[end], g);
                std::cout << "spaths: " << spaths.size() << std::endl;
                
                
                
                
                
               
                
                
            }
        
            else {
                std::set<int> usedsources;
                bound = false;
                 std::vector<int> localLps;
                for (unsigned int j = 0; j < sources.size(); j++) {
                    //totalLoops.clear();
                    //globalLoopPaths.clear();
                    //ROSE_ASSERT(usedsources.find(sources[j]) == usedsources.end());
                    //usedsources.insert(sources[j]);
                    sourcenum = sources[j];
                    //std::cout << "sources left: " << sources.size() - j << std::endl;
                    recursiveLoops.clear();
                    recurses.clear();
                    
                    std::vector<std::vector<int> > spaths = bfsTraversePath(sources[j], -1, g);
             
             
             
             
             
              
               
              //std::cout << "spaths.size(): " << spaths.size() << std::endl;
                    
                  
                }
           }
           //std::cout << "badpaths: " << badpaths << std::endl;
    }
   // std::cout << "normals: " << normals << " abnormals: " << abnormals << std::endl;
/*
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
*/
    std::cout << "checkedfound: " << checkedfound << std::endl;
    //printPathDot(g);
}



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






template<class CFG>
void
SgGraphTraversal<CFG>::
prepareGraph(CFG*& g) {
    nextNode = 1;
    nextEdge = 1;
    findClosuresAndMarkersAndEnumerate(g);
   
}


 


template<class CFG>
void
SgGraphTraversal<CFG>::
firstPrepGraph(CFG*& g) {
    nextNode = 1;
    nextEdge = 1;
    findClosuresAndMarkersAndEnumerate(g);
    
}







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



template<class CFG>
void
SgGraphTraversal<CFG>::
getVertexPath(std::vector<int> path, CFG*& g, std::vector<Vertex>& vertexPath) {
        
        for (unsigned int i = 0; i < path.size(); i++) {
                            vertexPath.push_back(intvertmap[path[i]]);
        }

       
       
        
}

/**
NCurrently unused, may eventually be modified for optimal storage purposes
SgGraphTraversal::storeCompact
@param[compactPath] path to be compactified
*/
template<class CFG>
void
SgGraphTraversal<CFG>::
storeCompact(std::vector<int> compactPath) {
return;
}




                  
