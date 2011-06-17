/*

FINISH TEMPFLATPATH CODE

*/




// Original Author (SgGraphTraversal mechanisms): Michael Hoffman
//$id$
#include<omp.h>
#include <boost/regex.hpp> 
#include <iostream>
#include <fstream>
#include <string>

/** 
*@file graphProcessing.h

*Brief Overview of Algorithm:

***********************
*Current Implementation
***********************

*The idea behind the algorithm here is to decompose the given Control Flow Graph into a Tree structure (still stored as a Control Flow Graph, though it may be possible to change this). This tree splits on children of a graph node, but does not connect in the case of multiple parents of a single node. In this we refer to out nodes as "children" and in nodes as "parents". However, we do this from the end node to the start node. This is best because then you get one value on the end node, and that value is the only one we want (however, the function takes a pointer to an empty SgIncidenceDirectedGraph as an argument, this can then be analyzed post traversal if that is wanted.

*Also, following the algorithm explained above, one must realize that the tree has one leaf for EACH path. Thus small programs can lead from a small-ish graph to a VERY large tree. For example, a program with 20 if else statements would end with 2^20 paths, which means the number of nodes in the tree is greater than this. Thus with 32 or 64 you can overflow a 32 or 64 bit integer.

*However, this can be partially resolved by setting the deleteTree boolean to true. This is set to false as the default, however if you don't need the tree then deleteTree will allow you to deal with much larger trees and thus much larger original graphs.

*Realize that because of the potentially massive growth rate of path number, that in large cases path enumeration and counting is extremely prohibitive, as it is not difficult to create a program which has more paths than 2^64, thus an unsigned (signed?) 64 bit integer could not store the number.

*Further, this is still a relatively compact method. You could just as easily force enumeration of paths, which could in some cases drastically increase the number of nodes necessary to store all the information.

*The advantage of the tree structure is two-fold. First it relieves the algorithm of having to keep even more in memory than it has to at the moment, and if you want to deal with GoTo statements, one case can develop that cannot be solved otherwise, e.g.

*Consider the four node tree with nodes a, b, c, d, and edges atb, atc, btc, ctb, btd, ctd. There are FOUR legitimate paths here (a, b, d; a, b, c, d; a, c, d; a, c, b, d), and any other method would recognize this as a loop and, without a special algorithm for loop behavior, this would be ignored.

*The tree structure also allows for very strong parallelization, currently implemented in openMP. This is because depth has meaning in terms of a tree (it doesn't in terms of a general graph) and that you know you can evaluate all nodes at a certain depth if you have solved all the nodes at depth + 1.

**************************
*Further Improvements: TODO
**************************

@todo *One improvement that should be implemented ASAP is changing the algorithm from a recursive algorithm to an iterative algorithm. Keeping the memory requirements down is much easier in this form and would probably increase the size of graph that the algorithm can handle.

@todo *Another improvement that should be implemented when possible is to allow for loop analysis. This could be implemented by simply running the algorithm on the loop, but there would need to be a provision that kept the algorithm from stopping as soon as it starts. This could be done by separating the node into two nodes, one with all the inedges and one with all the outedges. OR one could collect the loops when they are deleted (the whole loop is calculated necessarily), though nested loops would have to be considered further in order to find a way to deal with them.

@todo *It is possible that graph matching algorithms might prove useful to distinguish different types of graphs contained within the CFG and optimize traversal over them. Look up graph matching algorithms or pattern matching (potentially) for more information on such algorithms, though I do not believe there is an existant literature on matching subgraphs for this purpose.

@todo *The parallelism in this program should be optimized by someone experienced in parallelization optimization

***************
*Contact Info
***************

*Finally, blame can be assigned to and questions can be forwarded to the author, though response is not guaranteed

*archangel DOT associate AT gmail DOT com
*or, if I'm still at Lawrence
*hoffman34 AT llnl DOT gov
*@author Michael Hoffman
*/





#include "staticCFG.h"
#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
//#include "graphBot.h"

//This is necessary for technical reasons with regards to the graphnodeinheritedmap



struct Bot {
    std::vector<std::vector<SgGraphNode*> > path;
    std::vector<std::set<SgGraphNode*> > pthloops;
    std::vector<SgGraphNode*> currpth;
    std::vector<std::pair<SgGraphNode*, int> > nodelst;
    bool on;
    bool remove;
};

double timeDifference(const struct timeval& end, const struct timeval& begin)
{
    return (end.tv_sec + end.tv_usec / 1.0e6) - (begin.tv_sec + begin.tv_usec / 1.0e6);
}

static inline timeval getCPUTime() {
  rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  return ru.ru_utime;
}


struct compareSgGraphNode {
    bool operator()(const SgGraphNode* a, const SgGraphNode* b) const
    {
        return a==b;
    }
};


/* The SgGraphTraversal class is utilized specifically for StaticCFG traversals,
though the input must be in terms of a SgIncidenceDirectedGraph*/
template <class InheritedAttributeType, class SynthesizedAttributeType>
class SgGraphTraversal
{
   public:
    std::set<std::map<int, std::set<int> > > subpathmap;
    int loopNum;
    int nullNum;
    std::set<SgDirectedGraphEdge*> nullEdgesOrdered;
    std::map<SgGraphNode*, int> loopNumMap;
    std::map<SgGraphNode*, int> pathValMap;
    int nullloops;
    std::vector<std::vector<SgGraphNode*> > looppaths;
    std::vector<std::vector<SgGraphNode*> > iLoops;
    std::vector<SgGraphNode*> ifstatements;
    virtual ~SgGraphTraversal(); 
    SgGraphTraversal(); 
    // Copy operations
    int nullEdgesPaths;
    int turns;
    SgGraphTraversal(const SgGraphTraversal &);
    const SgGraphTraversal &operator=(const SgGraphTraversal &);
    //This is not used, but will be important if SynthesizedAttributes become useful
    typedef StackFrameVector<SynthesizedAttributeType> SynthesizedAttributesList;
    //one of the most important structures in the algorithm, this attaches SgGraphNode*s to InheritedAttributeTypes so that
    //looking up the values is possible.
    //int numnodes;
    //std::map<SgGraphNode*, InheritedAttributeType> seen;
    int numnodes;
    //InheritedAttributeType pthgraphinherit;
    //StaticCFG::CFG* SgCFG;
    SgGraphNode* nullnode;
    std::map<SgGraphNode*, int> primenode;
    bool done;
    //std::set<SgGraphNode*> startnodes;
    std::set<SgGraphNode*> lstN;
    std::map<SgGraphNode*, std::vector<std::set<int> > > lstordmap;
    std::set<SgGraphNode*> solvedLoops;
    std::map<SgGraphNode*, std::vector<SgGraphNode*> > checkednodes; 
    std::map<SgGraphNode*, std::set<SgGraphNode*> > downed;
   
    //std::map<SgGraphNode*, int> nodeinedgordmap;
    //a value for nodes that have no value, set in the traverse function
    InheritedAttributeType nullInherit;
    //the user invoked function, runs the algorithm
    InheritedAttributeType traverse(SgGraphNode* basenode, SgIncidenceDirectedGraph* g,
            InheritedAttributeType inheritedValue, InheritedAttributeType nullInherit,
            SgGraphNode* endnode, bool insep = false, bool pcHk = false);
    std::set<SgGraphNode*> loopSet;

   protected:
    //User defined functions to do whatever is needed in evaluation
    //All the user gets access to is the node in question
    //and the values of the parent nodes (this should be all that is necessary)
    virtual InheritedAttributeType evaluateInheritedAttribute(SgGraphNode* n,
            std::vector<InheritedAttributeType> inheritedValues) = 0;
    //Not used, but may be useful if SynthesizedAttributes become workable in this context
    virtual SynthesizedAttributeType evaluateSynthesizedAttribute(SgGraphNode* n,
            InheritedAttributeType in,
            SynthesizedAttributesList l) = 0;
    virtual void pathAnalyze(std::vector<SgGraphNode*>& pth, bool loop=false, std::set<std::vector<SgGraphNode*> >& incloops=NULL) = 0;
    //also not used, but important for possible later use of SynthesizedAttributes
    SynthesizedAttributeType defaultSynthesizedAttribute(InheritedAttributeType);
   private:
    double distime;
    //std::set<std::pair<std::pair<SgGraphNode*, SgGraphNode*>, std::pair<SgGraphNode*, SgGraphNode*> > > flpset;
    //std::set<std::pair<std::pair<SgGraphNode*, SgGraphNode*>, std::pair<SgGraphNode*, SgGraphNode*> > > goodset;
    std::set<SgGraphNode*> ploops;
    std::map<SgGraphNode*, std::set<std::vector<SgGraphNode*> > > lpbegins;
    std::map<SgGraphNode*, int> frksLeft;
    int currm;
    int dpMax;
    int repEval;
    bool pathCheck;
    int pathsSize;
    //this constructs the graph tree for computation of inheritedValues


    std::map<SgGraphNode*, InheritedAttributeType> known;
    std::vector<InheritedAttributeType> connectNodes;
    std::map<SgGraphNode*, bool> solved;
    std::set<SgGraphNode*> solvedset;
    //these two are not used, but will be important if SynthesizedAttributes are made reasonable in this context
    SynthesizedAttributesList *synthesizedAttributes;
    SynthesizedAttributeType traversalResult();
    //finally we have two functions necessary for parallel processing if that is chosen to be used by the user
 

    
  
    std::map<SgGraphNode*, int> nodeInEdgesNum;
    int currprime;
    std::vector<SgGraphNode*> endnodefakes;
    std::map<SgGraphNode*, std::vector<std::vector<SgGraphNode*> > > pathsAtMk;
    std::set<SgGraphNode*> mkloops;
    std::map<SgGraphNode*, std::set<std::vector<SgGraphNode*> > > mkloopmap;
    std::map<SgGraphNode*, std::set<std::vector<SgGraphNode*> > > subPathsAtMk;
    std::vector<SgGraphNode*> mkglobal;
    std::vector<SgGraphNode*> clglobal;
    bool inseparable;
    void solvePaths(SgIncidenceDirectedGraph* g, SgGraphNode* n, SgGraphNode* endnode);
    std::vector<std::set<SgGraphNode*> > closuresVec;
    void evaluatePaths(SgIncidenceDirectedGraph* g, SgGraphNode* realstartnode, SgGraphNode* endnode);
    void evaluatePathsPar(SgIncidenceDirectedGraph* g, SgGraphNode* realstartnode, SgGraphNode* endnode);
    bool disjoint(std::vector<SgGraphNode*>& path, std::vector<SgGraphNode*>& vec2) const;
    std::set<std::vector<SgGraphNode*> > flatpaths;
//    void evalNode(SgIncidenceDirectedGraph* g, SgGraphNode* n);
    bool canSolve(SgIncidenceDirectedGraph* g, SgGraphNode* n);
    std::map<SgGraphNode*, InheritedAttributeType> inhVals;
    std::set<SgDirectedGraphEdge*> seenEdges;
    std::set<SgDirectedGraphEdge*> nullEdges;
    std::set<SgGraphNode*> clsT;
    void computeOrder(SgIncidenceDirectedGraph* g, SgGraphNode* n, SgGraphNode* endnode);
    void computeInheritedOrdered(SgIncidenceDirectedGraph* g, SgGraphNode* n);
    std::pair<bool, SgGraphNode*> getNextPar(SgIncidenceDirectedGraph* g, SgGraphNode* n);
    std::pair<bool, SgGraphNode*> getNextChild(SgIncidenceDirectedGraph* g, SgGraphNode* n);
    bool computable(SgIncidenceDirectedGraph* g, SgGraphNode* n);
    void evalNodeOrdered(SgIncidenceDirectedGraph* g, SgGraphNode* n);
    std::map<SgGraphNode*, int> oVals;
    bool canEval(SgIncidenceDirectedGraph* g, SgGraphNode* n);
    void setPathVal(SgIncidenceDirectedGraph*g, SgGraphNode* n);
    void printNodePlusEdgesForAnalysis(SgIncidenceDirectedGraph* g, SgGraphNode* n, int loopNum, int pathVal, std::ofstream& ss);
    void printNodePlusEdgesForAnalysisPath(SgIncidenceDirectedGraph* g, std::vector<SgGraphNode*> n, int loopNum, int pathVal, std::ofstream& ss);
    void printNodeForAnalysis(SgGraphNode* n, int loopNum, int pathNum, std::ofstream& ss);
    std::set<SgGraphNode*> completedNodesPath;
    std::set<std::pair<SgGraphNode*, SgGraphNode*> > completedEdgesPath;
    void printEdgeForAnalysis(SgDirectedGraphEdge* e, bool isNullEdge, std::ofstream& ss);
    void printEdgeForAnalysisPath(SgGraphNode* g1, SgGraphNode* g2, std::ofstream& ss);
    std::map<int, SgGraphNode*> iVals;
    
    std::set<SgDirectedGraphEdge*> nullEdgesOrderedOut;
    std::set<SgDirectedGraphEdge*> completedEdgesOut;
    std::set<SgDirectedGraphEdge*> completedEdges;
    std::set<SgGraphNode*> compPar;
    std::set<SgGraphNode*> compChild;
    std::set<SgGraphNode*> computedNodes;
    SgGraphNode* st;
    SgGraphNode* en;
    double fllp;
    int loopnum;
    //std::set<SgGraphNode*> solved;
    //InheritedAttributeType findAndReverse(SgGraphNode* n, SgIncidenceDirectedGraph* g);
     //evaluateAllInheritedAttribute(std::vector<InheritedAttributeType> endNodeInhVec, SgGraphNode* endnode, std::vector<SgGraphNode*> nodes, std::vector<InheritedAttributeType> inh);
//std::vector<InheritedAttributeType> getZeroInhs(std::vector<std::vector<std::vector<SgGraphNode*> > > qAnsSetSet, std::vector<InheritedAttributeType> endnodeInhVec, SgGraphNode* node);

};



/*
template <class InheritedAttributeType, class SynthesizedAttributeType>
void
GraphBot<InheritedAttributeType, SynthesizedAttributeType>::
travelDown(SgIncidenceDirectedGraph* g) {
    std::set<SgDirectedGraphEdge*> oedgs = g->computeEdgeSetOut(iAmHere);
    bool taken = false;
    if (oedgs.size() > 1) {
        std::set<SgDirectedGraphEdge*> edgeTrav = clEdgeTrav[iAmHere];
        ROSE_ASSERT(clEdgeTrav.find(iAmHere) != clEdgeTrav.end());
        for (std::set<SgDirectedGraphEdge*>::iterator i = oedgs.begin(); i != oedgs.end(); i++) {
            if (edgTrav.find(*i) == edgTrav.end() || !taken) {
                taken = true;
                iAmHere = (*i)->get_to();
                lastEdge = *i;
            }
        }
    }
    else {
        iAmHere = (*(oedgs.begin())->get_to();
    }
}
*/






/*
***************************
Various Admin Functions
***************************
*/
template<class InheritedAttributeType, class SynthesizedAttributeType>
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
SgGraphTraversal()
  : synthesizedAttributes(new SynthesizedAttributesList())
{
}

#ifndef SWIG

template<class InheritedAttributeType, class SynthesizedAttributeType>
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
~SgGraphTraversal()
{
        ROSE_ASSERT(synthesizedAttributes != NULL);
        delete synthesizedAttributes;
        synthesizedAttributes = NULL;
}

#endif


template<class InheritedAttributeType, class SynthesizedAttributeType>
const SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType> &
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
operator=(const SgGraphTraversal &other)
{

    ROSE_ASSERT(synthesizedAttributes != NULL);
    delete synthesizedAttributes;
    synthesizedAttributes = other.synthesizedAttributes->deepCopy();

    return *this;
}

/**
This is the function that is used by the user directly to start the algorithm. It is immediately available to the user

SgGraphTraversal::traverse
Input:
@param[n] n starting node
@param[g] SgIncidenceDirectedGraph* g, CFG calculated previously
@param[inheritedValue] InheritedAttributeType inheritedValue, value of the starting node
@param[nullI] InheritedAttributeType nullI, value of the null Attribute, i.e. what to attribute to a node with no value\
@param[endnode] SgGraphNode* endnode, final node
@param[insep] boolean to decide inseparability of the analysis function, not yet in use, set automatically to false
@param[pCh] deprecated, set to false
@return InheritedAttributeType, the value of the attribute at the end node

*/


template<class InheritedAttributeType, class SynthesizedAttributeType>
InheritedAttributeType
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
traverse(SgGraphNode* n, SgIncidenceDirectedGraph* g, InheritedAttributeType inheritedValue, InheritedAttributeType nullI, SgGraphNode* endnode, bool insep, bool pCh) {
   //numnodes = 0;
   //primes.clear();
    looppaths.clear();
    iLoops.clear();
    completedEdgesPath.clear();
    pathValMap.clear();
    loopNumMap.clear();
    nullloops = 0;
    nullEdgesPaths = 0;
    fllp = 0.0;
    mkglobal.clear();
    clglobal.clear();
  
    lpbegins.clear();
    //currents.clear();
   inhVals.clear();
   iVals.clear();
   oVals.clear();
   //reservedEdges.clear();
   completedEdges.clear();
   completedEdgesOut.clear();
   //completedNodes.clear();
   computedNodes.clear();
   nullEdgesOrdered.clear();
   nullEdgesOrderedOut.clear();
   loopSet.clear();
   pathsAtMk.clear();
   
   st = n;
   en = endnode;
   distime = 0.0;
   int currm = 1;
   int turns = 0;
   pathsSize = 0;
   done = false;
   numnodes = 1;
   std::cout << "starting traversal" << std::endl;
   pathCheck = pCh;
   currprime = 1;
   inseparable = insep;
   synthesizedAttributes->resetStack();
   ROSE_ASSERT(synthesizedAttributes->debugSize() == 0);
   //SgCFG = cfg;
   inhVals[n] = inheritedValue;
   //GraphBot<InheritedAttributeType, SynthesizedAttributeType>::inhVals[n] = inheritedValue;
   //primes = generatePrimesSieve();
   
  
//   graphnodeinheritedordmap[ncpy] = inheritedValue;
//   nodenodeordmap[ncpy] = n;
//   std::vector<SgGraphNode*> lst;
//   lst.push_back(n);
//   lstordmap[ncpy] = lst;

   nullInherit = nullI;
InheritedAttributeType inh;
   struct timeval t1, t2, t3, t4, t5, t6, t7, t8;
   //else {
       loopnum = 0;
       //InheritedAttributeType inh;
       t1 = getCPUTime();
       
       //this function essentially sets up for the evaluate later, it makes putting together the paths much easier
       solvePaths(g, n, endnode);

       t2 = getCPUTime();

//making sure that endnode hasn't already been evaluated before the traversal starts, unlikely but just in case
       ROSE_ASSERT(inhVals.find(endnode) == inhVals.end());

       std::cout << "solvePaths done" << std::endl;
       double diff = timeDifference(t2, t1);
       t5 = getCPUTime();
       //InheritedAttributeType pthgraphinherit = botTraverse(g, n, endnode);
       oVals[n] = 0;
       iVals[0] = n;
       pathValMap[n] = 1;
//inserting n as a computed node
       computedNodes.insert(n);
//computes the order in which the nodes must be evaluated, makes computeInheritedOrdered much faster
       computeOrder(g, n, endnode);
       std::cout << "order computed" << std::endl;
//computes the nodal inheritance values
       computeInheritedOrdered(g, n);
       std::cout << "inheritance computed" << std::endl;
       ROSE_ASSERT(oVals.find(endnode) != oVals.end());
       ROSE_ASSERT(inhVals.find(endnode) != inhVals.end());
//value at the endnode
       InheritedAttributeType pthgraphinherit = inhVals[endnode];
       //= evaluateGraph(g, n, endnode, inheritedValue);
       t6 = getCPUTime();
       std::cout << "evaluateGraph done" << std::endl;
       double diff3 = timeDifference(t6, t5);
       t3 = getCPUTime();
//actually evaluates every path with a user defined pathAnalyze function
       //for (int i = 0; i < 10; i++) {
       evaluatePaths(g, n, endnode);
       //}
       t4 = getCPUTime();
       
       t7 = getCPUTime();
       //evaluatePathsPar(g, n, endnode);
       t8 = getCPUTime();
       
       std::cout << "evaluatePaths done " << std::endl;
       double diff2 = timeDifference(t4, t3);
       double diff2Par = timeDifference(t8, t7);
       std::cout << "pathsolve took: " << diff << std::endl;
       std::cout << "patheval took: " << diff2 << std::endl;
       std::cout << "parpatheval took: " << diff2Par << std::endl;
       std::cout << "grapheval took: " << diff3 << std::endl;
       std::cout << "entire pathsolve took: " << diff+diff2+diff3+diff2Par << std::endl;
       std::cout << "potential loops: " << nullEdgesOrdered.size() << std::endl;
       std::cout << "nullNum: " << nullNum << std::endl;
       //std::cout << "goodsets: " << goodset.size() << std::endl;
       //std::cout << "flpsets: " << flpset.size() << std::endl;
       std::cout << "mkloops: " << mkloops.size() << std::endl;
       std::cout << "distime: " << distime << std::endl;
       std::cout << "fllp: " << fllp << std::endl;
       return pthgraphinherit;
   //}
   //std::cout << "number of endnodefakes: " << endnodefakes.size() << std::endl;
   //std::cout << "should be number of nodes: " << currprime << std::endl;
   //if (pathanalysis == true) {
     // analyzepaths(endnode, g);
   //}
   //return inh;
   //Currently this is not very useful, but it does nothing if traversalResult is not set.
}

/* WARNING:
   This is not a general is_disjoint. It skips the 
first element of the second set because in the way I assemble
paths the last element of the path and the first element of addend
must be the same. Hence I simply skip the first node
*/
bool is_disjoint(std::set<SgGraphNode*> set1, std::set<SgGraphNode*> set2) {
 
   if (set1.empty() || set2.empty()) {
       return true;
   }
   std::set<SgGraphNode*>::iterator it1 = set1.begin();
   std::set<SgGraphNode*>::iterator it2 = set2.begin();
   std::set<SgGraphNode*>::iterator it1End = set1.end();
   std::set<SgGraphNode*>::iterator it2End = set2.end();

    if (*it1 > *set2.rbegin() || *it2 > *set1.rbegin()) {
        return true;
    }
    
    while (it1 != it1End && it2 != it2End) {
        if (*it1 == *it2) {
            return false;
        }
        if (*it1 < *it2) {
            it1++;
        }
        else {
            it2++;
        }
     }
     return true;
}   



//Checks for disjoint, necessary in computing the paths
template<class InheritedAttributeType, class SynthesizedAttributeType>
bool
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
disjoint(std::vector<SgGraphNode*>& pthloops, std::vector<SgGraphNode*>& vec2) const {
/*
    time_t t1, t2;
    time(&t1);
    int a = 0;
    std::set<SgGraphNode*> s1;
    std::set<SgGraphNode*> s2;
    std::vector<SgGraphNode*> mkloopvec;
    bool goodsetbool;
    bool pbool = true;
    //std::cout << "calculating disjoint" << std::endl;
    ROSE_ASSERT((path.back()).back() == vec2.front());

    //copy(vec2.begin(), vec2.end(), inserter(s2, s2.end()));
/*
    for (int i = 0; i < vec2.size(); i++) {
        if (ploops.find(vec2[i]) != ploops.end()) {
            pbool = false;
        }
    }
    if (pbool) {
        return true;
    }
    if (
*/  //for (int q = 0; q < pthloops->size(); q++) {
        for (int i = 0; i < pthloops.size(); i++) {
            if (find(vec2.begin(), vec2.end(), pthloops[i]) != vec2.end()) {
                return false;
            }
        }
        return true;
}
/*
    if (pbool) {
        time(&t2);
        double diff = difftime(t2, t1);
        distime += diff;
        return true;
    }
    for (unsigned int k = 0; k < path.size(); k++) {
        s1.clear();
*/
/*
        pbool = true;
        for (int p = 0; p < path[k].size(); p++) {
            if (ploops.find(path[k][p]) != ploops.end()) {
                pbool = false;
            }
        }
//        copy(path[k].begin(), path[k].end(), inserter(s1, s1.end()));
        if (!pbool) {
*/
/*
        std::pair<std::pair<SgGraphNode*, SgGraphNode*>, std::pair<SgGraphNode*, SgGraphNode*> > flp;
        flp.second.first = vec2[0];
        flp.second.first = vec2[1];

        flp.first.first = path[k][0];
        flp.first.second = path[k][1];
        if (vec2.front() == vec2.back()) {
        time(&t2);
        double diff = difftime(t2, t1);
        distime += diff;

            return false;
        }
        if (flpset.find(flp) != flpset.end()) {
            //std::cout << "already seen" << std::endl;
        time(&t2);
        double diff = difftime(t2, t1);
        distime += diff;

            return false;
        }
*/
/*
        else if (goodset.find(flp) != goodset.end()) {
            goodsetbool = true;
        }
*/
/*
        if (is_disjoint(s1,s2)) {
            //goodset.insert(flp);
            continue;
        }
        else {
            return false;
        }
*/
/*
       else {
        std::vector<SgGraphNode*> vec1 = path[k];
        
        //for (unsigned int i = 0; i < vec1.size(); i++) {
            for (unsigned int j = 0; j < mkloopvec.size(); j++) {
                std::vector<SgGraphNode*>::iterator q = find(vec1.begin(), vec1.end(), mkloopvec[j]);
                if (q != vec1.end()) {
                    if (*q != vec1[vec1.size() - 1] || j != 0) {
                        
                        flpset.insert(flp);
      //                  std::cout << "not disjoint" << std::endl;
        time(&t2);
        double diff = difftime(t2, t1);
        distime += diff;

                        return false;
                    }
                }
            }
        //}
        //goodset.insert(flp);
    }
    }
    //}
*/

    
/*
    for (unsigned int p = 0; p < vec2.size(); p++) {
        for (unsigned int q = 0; q < vec2.size(); q++) {
            if (p != q) {
                if (vec2[p] == vec2[q]) {
                    return false;
                }
            }
        }
    }
*/
/*
        time(&t2);
        double diff = difftime(t2, t1);
        distime += diff;

    return true;
}
*/
//checks for solvability of a node in nodal analysis

template<class InheritedAttributeType, class SynthesizedAttributeType>
bool
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
canSolve(SgIncidenceDirectedGraph* g, SgGraphNode* n) {
    bool loop = false;
    if (inhVals.find(n) != inhVals.end()) {
        return true;
    }
    std::set<SgDirectedGraphEdge*> oed = g->computeEdgeSetIn(n);
    if (oed.size() == 0) {
        return false;
    }
        for (std::set<SgDirectedGraphEdge*>::iterator i = oed.begin(); i != oed.end(); i++) {
            if (inhVals.find((*i)->get_from()) == inhVals.end() && nullEdges.find(*i) == nullEdges.end()) {
                return false;
            }
        }
   return true;
} 

//this function evaluates values of paths via the user-defined pathAnalyze function

template<class InheritedAttributeType, class SynthesizedAttributeType>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
evaluatePathsPar(SgIncidenceDirectedGraph* g, SgGraphNode* realstartnode, SgGraphNode* endnode) {
std::vector<std::vector<SgGraphNode*> > path;
std::vector<SgGraphNode*> spath;
SgGraphNode* n = realstartnode;
int successes = 0;
int failures = 0;
int j = 0;
std::vector<SgGraphNode*> currpthorg;
int currint = 0;
std::map<SgGraphNode*, int> intPath;
intPath[n] = currint;
currint++;
std::map<SgGraphNode*, int> currents;
SgGraphNode* currnode;
bool step = false;
bool midstep = false;

//note: pathsAtMk is referring to subpaths connected to that marker, a marker is a split in the graph (usually an if statement)

std::vector<std::vector<SgGraphNode*> > pth = pathsAtMk[realstartnode];
std::vector<std::vector<SgGraphNode*> > cpth = pathsAtMk[realstartnode];
    path.clear();
    int disjoints = 0;
    int disjointtrues = 0;
    currpthorg = pth[0];
    intPath[pth[0].front()] = currint;
    std::set<SgGraphNode*> pthloopstmp;
    SgGraphNode* fakenode;
    pthloopstmp.insert(fakenode);
    std::vector<std::set<SgGraphNode*> > pthloops;
    pthloops.push_back(pthloopstmp);
    pthloopstmp.clear();
    currint++;
    
    int stepnum = 0;
    std::vector<SgGraphNode*> rs;
    rs.push_back(realstartnode);
    path.push_back(rs);
    currents.clear();

    step = false;
    std::vector<SgGraphNode*> sub;


    std::set<std::vector<SgGraphNode*> > nullIncLoops;
    std::vector<struct Bot*> todobotlst;
    std::vector<struct Bot*> botlst;
    struct Bot* rootBot = new Bot;
    rootBot->remove = false;
    
    rootBot->path = path;
    rootBot->currpth = currpthorg;
    rootBot->pthloops = pthloops;
    rootBot->on = true;
     botlst.push_back(rootBot);
    int tip = 1;
    int ti = 1;
    std::vector<std::pair<std::vector<SgGraphNode*>, std::vector<std::set<SgGraphNode*> > > > collectedPaths;
    int maxlst = 0;
    while (true) {
    if (todobotlst.size()+botlst.size() > maxlst) {
        maxlst = todobotlst.size()+botlst.size();
        std::cout << "maxlst: " << maxlst << std::endl;
        std::cout << "todobotlst.size(): " << todobotlst.size() << std::endl;
        std::cout << "botlst.size(): " << botlst.size() << std::endl;
    }
    int MAXBOTS = 10000;
    int MINPATHS = 1000;
    int LOCALMAXBOTS = 10;
    int LOCALMAXNODES = 0;
    std::vector<struct Bot*> lstnullbot;
    std::vector<std::vector<struct Bot*> > newbotlsts (MAXBOTS, lstnullbot);
    //std::vector<struct Bot*> newbotlsts (MAXBOTS, lstnullbot);
    //tip = ti;
    //ti = 0;
    ROSE_ASSERT(botlst.size() >= 0);
    if (botlst.size() == 0) {
        if (todobotlst.size() != 0) {
            while (todobotlst.size() > 0 && botlst.size() < MAXBOTS) {
                todobotlst.back()->on = true;
                botlst.push_back(todobotlst.back());
                todobotlst.pop_back();
            }
        }
        else {
            if (collectedPaths.size() > 0) {
                  for (int i = 0; i < collectedPaths.size(); i++) {
                  std::set<std::vector<SgGraphNode*> > incloops;
                  std::vector<std::set<SgGraphNode*> > pthloops = collectedPaths[i].second;
                  for (int q = 0; q < pthloops.size(); q++) {
                  for (std::set<SgGraphNode*>::iterator p = pthloops[q].begin(); p != pthloops[q].end(); p++) {
                    for (std::set<std::vector<SgGraphNode*> >::iterator o = mkloopmap[*p].begin(); o != mkloopmap[*p].end(); o++) {
                        incloops.insert(*o);
                    }
                 }
                 }


        pathAnalyze(collectedPaths[i].first, false, incloops);
    }
    collectedPaths.clear();
            }
            break;
        }
    }
    if (botlst.size() > 0) {
    std::pair<std::vector<SgGraphNode*>, std::vector<std::set<SgGraphNode*> > > nullpr;
    std::vector<std::pair<std::vector<SgGraphNode*>, std::vector<std::set<SgGraphNode*> > > > newpathslst (MAXBOTS, nullpr);
    #pragma omp parallel for
    for (int i = 0; i < botlst.size(); i++) {
        //std::map<SgGraphNode*, std::set<std::vector<SgGraphNode*> > > mkloopmaptmp = mkloopmap;
        std::vector<struct Bot*> localbotlst;
        std::pair<std::vector<SgGraphNode*>, std::vector<std::set<SgGraphNode*> > > localnewpath;
        struct Bot* currBot = botlst[i];
        if (currBot->on) {
        std::vector<SgGraphNode*> currpth = currBot->currpth;
        std::vector<std::vector<SgGraphNode*> > path = currBot->path;
        std::vector<std::set<SgGraphNode*> > pthloops = currBot->pthloops;
        
            if (currpth.back() == endnode) {
            path.push_back(currpth);
            std::vector<SgGraphNode*> flatpath;
            std::set<std::vector<SgGraphNode*> > incloops;
            struct timeval q1, q2;
            ROSE_ASSERT(path.size() == pthloops.size() + 1);
            q1 = getCPUTime();
            for (unsigned int q = 0; q < pthloops.size(); q++) {
                for (unsigned int r = 0; r < path[q].size(); r++) {
                    flatpath.push_back(path[q][r]);
                }

/*
#pragma omp critical
{
                for (std::set<SgGraphNode*>::iterator p = pthloops[q].begin(); p != pthloops[q].end(); p++) {
                    for (std::set<std::vector<SgGraphNode*> >::iterator o = mkloopmap[*p].begin(); o != mkloopmap[*p].end(); o++) {
                        incloops.insert(*o);
                    }
                 }
}
*/

             }

             for (unsigned int pt2 = 0; pt2 < path[path.size()-1].size(); pt2++) {
                     flatpath.push_back(path[path.size()-1][pt2]);
             }             
             q2 = getCPUTime();
             fllp += timeDifference(q2,q1);
             flatpath.push_back(endnode);
//user defined function, run on the final path, gives the user loops that are included via "incloops" a set of vectors that contain the individual loops
/*
             #pragma omp critical (analyze)
{
             pathAnalyze(flatpath, false, incloops);
}
*/
             std::pair<std::vector<SgGraphNode*> , std::vector<std::set<SgGraphNode*> > > newcol;
             newcol.first = flatpath;
             newcol.second = pthloops;
             localnewpath = newcol;
             incloops.clear();

             int pts = pathsSize++;
             pathsSize += 1;
            
             flatpath.clear();
             path.pop_back();
             int rounds = 0;
             bool starter = false;

// This gets a bit complicated so here is an overview:
// This is running down the graph and finding the endnode. Once it finds the endnode it goes back up to the last unevaluated subpath. It does this quickly with an integer that counts how many times that node has been used for a path. If this ends up being the number of outnodes, we don't need that node anymore, so we clear it to zero, then continue up the graph. We HAVE to reset because every time a new pathway is chosen above that node, it needs to have the ability to traverse that node.
/*
              if (currBot->nodelst.size() != 0) {
                   while (path.back().back() != currBot->nodelst.back().first) {
                       ROSE_ASSERT(path.size() != 0);
                       path.pop_back();
                       pthloops.pop_back();
                       
                   }
                   currBot->path = path;
                   currBot->pthloops = pthloops;
                   currBot->currpth = pathsAtMk[(path.back()).back()][currBot->nodelst.back().second];
                   currBot->nodelst.pop_back();
                   localbotlst.push_back(currBot);
             }
             else {
*/
                 currBot->remove = true;
                 localbotlst.push_back(currBot);
             //}
         }
         else {

//this checks first to see if we have any loops in our path. If not it continues down, if there is it goes back to the last nonloop node
        bool disj = true;
        struct timeval tdisb, tdise;
        //tdisb = getCPUTime();
        for (int x = 0; x < pthloops.size(); x++) {
            for (std::set<SgGraphNode*>::iterator j = pthloops[x].begin(); j != pthloops[x].end(); j++) {
                if (find(currpth.begin(), currpth.end(), *j) != currpth.end()) {
                    disj = false;
                }
             }
        }
        //tdise = getCPUTime();
        //distime += timeDifference(tdise, tdisb);
        if (disj) {
            
            disjointtrues++;
            //std::cout << "disjoints: " << disjointtrues << std::endl;
            midstep = false;
std::set<SgGraphNode*> pthloopstmp;
            pthloopstmp.clear();
                for (int i = 0; i < currpth.size(); i++) {
                    //currflat.push_back(currpth[i]);
                    if (mkloops.find(currpth[i]) != mkloops.end()) {
                       pthloopstmp.insert(currpth[i]);
                    }
                }
                pthloops.push_back(pthloopstmp);
                path.push_back(currpth);
                pthloopstmp.clear();
                
                //std::set<std::vector<SgGraphNode*> > lpth;
                std::vector<SgGraphNode*> oldcurrpth = currpth;
                currpth.clear();
                        SgGraphNode* frontnode = (path.back()).front();
                        SgGraphNode* backnode = (path.back()).back();
     
                        ROSE_ASSERT(pathsAtMk.find(backnode) != pathsAtMk.end() || backnode == endnode);
                        ROSE_ASSERT(pathsAtMk.find(frontnode) != pathsAtMk.end());
                        std::vector<std::vector<SgGraphNode*> > tmppths = pathsAtMk[backnode];
                        currBot->currpth = tmppths[0];
                        currBot->path = path;
                        currBot->pthloops = pthloops;
                        //newbotlst.push_back(currBot);
                        for (int tp = 1; tp < tmppths.size(); tp++) {
                            //if (localbotlst.size() < LOCALMAXBOTS) {
/*
                            if (currBot->nodelst.size() < LOCALMAXNODES) {
                                std::pair<SgGraphNode*, int> cur;
                                cur.second = tp;
                                cur.first = path.back().back();
                                currBot->nodelst.push_back(cur);
                            }
                            else {
*/
                                struct Bot* newBot = new Bot;
                                newBot->remove = false;
                                newBot->currpth = tmppths[tp];
                                newBot->path = path;
                                newBot->pthloops = pthloops;
                                localbotlst.push_back(newBot);
                                //ti++;
 //                           }
                        }
                        localbotlst.push_back(currBot);
                        //ti++;
         }          
         else {
/*
               if (currBot->nodelst.size() != 0) {
                   while (path.back().back() != currBot->nodelst.back().first) {
                       ROSE_ASSERT(path.size() != 0);
                       path.pop_back();
                       pthloops.pop_back();
                       
                   }
                   currBot->path = path;
                   currBot->pthloops = pthloops;
                   currBot->currpth =  pathsAtMk[(path.back()).back()][currBot->nodelst.back().second];
                   currBot->nodelst.pop_back();
                   localbotlst.push_back(currBot);
                   //ti++;
             }

             else {
*/
                 currBot->remove = true;
                 localbotlst.push_back(currBot);
                 //delete currBot;
 //            }

        }
     }
 newpathslst[i] = localnewpath;
 newbotlsts[i] = localbotlst;
 }
}
 botlst.clear();
 int num = 0;

 for (int i = 0; i < newbotlsts.size(); i++) {
    if (newpathslst[i].first.size() > 0) {
        collectedPaths.push_back(newpathslst[i]);
    }
    for (int j = 0; j < newbotlsts[i].size(); j++) {
        if (newbotlsts[i][j]->remove == true) {
            delete newbotlsts[i][j];
        }
        else if (num < MAXBOTS) {
            newbotlsts[i][j]->on = true;
            botlst.push_back(newbotlsts[i][j]);
            num++;
        }
        else {
            newbotlsts[i][j]->on = false;
            todobotlst.push_back(newbotlsts[i][j]);
        }
    }
}

if (collectedPaths.size() > MINPATHS) {
          
    for (int i = 0; i < collectedPaths.size(); i++) {
                 std::vector<std::set<SgGraphNode*> > pthloops;   
                std::set<std::vector<SgGraphNode*> > incloops;
                pthloops = collectedPaths[i].second;
                for (int q = 0; q < pthloops.size(); q++) {
                for (std::set<SgGraphNode*>::iterator p = pthloops[q].begin(); p != pthloops[q].end(); p++) {
                    for (std::set<std::vector<SgGraphNode*> >::iterator o = mkloopmap[*p].begin(); o != mkloopmap[*p].end(); o++) {
                        incloops.insert(*o);
                    }
                 }
               }

        pathAnalyze(collectedPaths[i].first, false, incloops);
    }
    collectedPaths.clear();
}
}
 else {
            if (collectedPaths.size() > 0) {
            for (int i = 0; i < collectedPaths.size(); i++) {
                std::set<std::vector<SgGraphNode*> > incloops;
                pthloops = collectedPaths[i].second;
                for (int q = 0; q < pthloops.size(); q++) {
                for (std::set<SgGraphNode*>::iterator p = pthloops[q].begin(); p != pthloops[q].end(); p++) {
                    for (std::set<std::vector<SgGraphNode*> >::iterator o = mkloopmap[*p].begin(); o != mkloopmap[*p].end(); o++) {
                        incloops.insert(*o);
                    }
                 }
               }
 
        pathAnalyze(collectedPaths[i].first, false, incloops);
    }
    }
    collectedPaths.clear();
 break;
 }
}

std::cout << "successes: " << successes << std::endl;
std::cout << "failures: " << failures << std::endl;
std::cout << "maxlst: " << maxlst << std::endl;
return;
}



template<class InheritedAttributeType, class SynthesizedAttributeType>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
evaluatePaths(SgIncidenceDirectedGraph* g, SgGraphNode* realstartnode, SgGraphNode* endnode) {
//std::set<SgGraphNode*> seen;
//for (std::map<SgGraphNode*, std::vector<std::vector<SgGraphNode*> > >::iterator i = pathsAtMk.begin(); i != pathsAtMk.end(); i++) {
/*    
    std::vector<std::vector<SgGraphNode*> > tocheck = (*i).second;
    for (int j = 0; j < tocheck.size(); j++) {
        for (int k = 0; k < tocheck[j].size(); k++) {
            if (seen.find(tocheck[j][k]) != seen.end()) {
                ploops.insert(tocheck[j][k]);
            }
            else {
                seen.insert(tocheck[j][k]);
            }
        }
    }
}
*/     
std::vector<std::vector<SgGraphNode*> > path;
std::vector<SgGraphNode*> spath;
SgGraphNode* n = realstartnode;
int successes = 0;
int failures = 0;
int j = 0;
std::vector<SgGraphNode*> currpth;
int currint = 0;
std::map<SgGraphNode*, int> intPath;
intPath[n] = currint;
currint++;
std::map<SgGraphNode*, int> currents;
SgGraphNode* currnode;
bool step = false;
bool midstep = false;

//note: pathsAtMk is referring to subpaths connected to that marker, a marker is a split in the graph (usually an if statement)

std::vector<std::vector<SgGraphNode*> > pth = pathsAtMk[realstartnode];
std::vector<std::vector<SgGraphNode*> > cpth = pathsAtMk[realstartnode];
    path.clear();
    int disjoints = 0;
    int disjointtrues = 0;
    currpth = pth[0];
    intPath[pth[0].front()] = currint;
    std::set<SgGraphNode*> pthloopstmp;
    SgGraphNode* fakenode;
    pthloopstmp.insert(fakenode);
    std::vector<std::set<SgGraphNode*> > pthloops;
    pthloops.push_back(pthloopstmp);
    pthloopstmp.clear();
    currint++;
    
    int stepnum = 0;
    std::vector<SgGraphNode*> rs;
    rs.push_back(realstartnode);
    path.push_back(rs);
    //currflat.push_back(realstartnode);
    currents.clear();

    step = false;
    //std::vector<SgGraphNode*> currflat;
    std::vector<SgGraphNode*> sub;

/*
    std::ofstream mz;
    mz.open("pathanalysis.dot");
    mz << "digraph defaultName { \n";
*/
    std::set<std::vector<SgGraphNode*> > nullIncLoops;

/*
    for (unsigned int p = 0; p < looppaths.size(); p++) {
        std::vector<SgGraphNode*> lp = looppaths[p];

        for (unsigned int i = 0; i < lp.size()-1; i++) {
            for (unsigned int l = i+1; l < lp.size(); l++) {
                if (lp[i] == lp[l] && lp[i] != realstartnode && lp[i] != endnode) {
                std::vector<SgGraphNode*> interiorloop;
                interiorloop.clear();
                for (unsigned int j = i; j < l+1; j++) {
                    interiorloop.push_back(lp[j]);
                }
                if (interiorloop.size() > 2) {
                }
                if (interiorloop.size() > 2 && interiorloop.back() != endnode) {
                if (find(iLoops.begin(), iLoops.end(), interiorloop) == iLoops.end()) {
                    if (find(looppaths.begin(), looppaths.end(), interiorloop) == looppaths.end()) {
                        iLoops.push_back(interiorloop);
                        loopnum++;
                        for (unsigned int k = 0; k < interiorloop.size(); k++) {
                             loopNumMap[interiorloop[k]] = loopnum;
                        }
                        lpbegins[interiorloop.front()].insert(interiorloop);
                        pathAnalyze(interiorloop, true, nullIncLoops);
                        
                    }
                }
                }
                }
            }
        }
        if (lp.size() > 2) {   
            lpbegins[lp.front()].insert(lp);      
            pathAnalyze(lp, true, nullIncLoops);
            //for (unsigned int i = 1; i < lp.size(); i++) {
            //    printNodePlusEdgesForAnalysisPath(g, lp, p, p, mz);
            //}
        }
    }
*/
    while (step == false) {
        stepnum++;
        
        if (currpth.back() == endnode) {
            path.push_back(currpth);
            //for (int i = 0; i < currpth.size(); i++) {
            //    currflat.push_back(currpth[i]);
            //}
            std::vector<SgGraphNode*> flatpath;
            //std::vector<SgGraphNode*> sub;
            std::set<std::vector<SgGraphNode*> > incloops;
            struct timeval q1, q2;
            //std::cout << "path.size(): " << path.size() << std::endl;
            //std::cout << "pthloops.size(): " << pthloops.size() << std::endl;
            ROSE_ASSERT(path.size() == pthloops.size() + 1);
            q1 = getCPUTime();
            for (unsigned int q = 0; q < pthloops.size(); q++) {
                //sub = path[q];
                //sub.pop_back();
                for (unsigned int r = 0; r < path[q].size(); r++) {
                    flatpath.push_back(path[q][r]);
                }
                for (std::set<SgGraphNode*>::iterator p = pthloops[q].begin(); p != pthloops[q].end(); p++) {
                    for (std::set<std::vector<SgGraphNode*> >::iterator o = mkloopmap[*p].begin(); o != mkloopmap[*p].end(); o++) {
                        incloops.insert(*o);
                    }
                 }
             }
             for (unsigned int pt2 = 0; pt2 < path[path.size()-1].size(); pt2++) {
                     flatpath.push_back(path[path.size()-1][pt2]);
             }
             
             q2 = getCPUTime();
             fllp += timeDifference(q2,q1);
             flatpath.push_back(endnode);
/* 
            for (unsigned int ps = 0; ps < flatpath.size(); ps++) {
                 if (lpbegins.find(flatpath[ps]) != lpbegins.end()) {
                     for (std::set<std::vector<SgGraphNode*> >::iterator sv = lpbegins[flatpath[ps]].begin(); sv != lpbegins[flatpath[ps]].end(); sv++) {
                         incloops.insert(*sv);
                     }
                 }
             }
*/
//user defined function, run on the final path, gives the user loops that are included via "incloops" a set of vectors that contain the individual loops
             pathAnalyze(flatpath, false, incloops);
             incloops.clear();
                //printNodePlusEdgesForAnalysisPath(g, flatpath, -1, -1, mz);

             int pts = pathsSize++;
             pathsSize += 1;
            
             flatpath.clear();
             path.pop_back();
             int rounds = 0;
             bool starter = false;

// This gets a bit complicated so here is an overview:
// This is running down the graph and finding the endnode. Once it finds the endnode it goes back up to the last unevaluated subpath. It does this quickly with an integer that counts how many times that node has been used for a path. If this ends up being the number of outnodes, we don't need that node anymore, so we clear it to zero, then continue up the graph. We HAVE to reset because every time a new pathway is chosen above that node, it needs to have the ability to traverse that node.


              while (true) {
                   rounds++;
                   ROSE_ASSERT(pathsAtMk.find((path.back()).back()) != pathsAtMk.end());
                   if ((path.back()).front() == realstartnode) {
                       starter = true;
                   }
                   if (currents[(path.back()).back()] < (pathsAtMk[(path.back()).back()].size()) /*|| (path.back()).front() == realstartnode*/) {
                              std::vector<std::vector<SgGraphNode*> > cpths = pathsAtMk[(path.back()).back()];
                              currpth = cpths[currents[(path.back()).back()]];
                              currents[(path.back()).back()]++;
                              break;
                   }
                   else {
                       currents[(path.back()).back()] = 0;
                       path.pop_back();
                       pthloops.pop_back();
                   }
                   if (starter == true) {
                       step = true;
                       break;
                   }

           }
        }
        else {

//this checks first to see if we have any loops in our path. If not it continues down, if there is it goes back to the last nonloop node
        bool disj = true;
        struct timeval tdisb, tdise;
        tdisb = getCPUTime();
        for (int i = 0; i < pthloops.size(); i++) {
            for (std::set<SgGraphNode*>::iterator j = pthloops[i].begin(); j != pthloops[i].end(); j++) {
                if (find(currpth.begin(), currpth.end(), *j) != currpth.end()) {
                    disj = false;
                }
             }
        }
/*
        #pragma omp parallel for num_threads(4) private(i,j)
        for (i = 0; i < pthloops.size(); i++) {
            if (disj) {
            for (std::set<SgGraphNode*>::iterator j = pthloops[i].begin(); j != pthloops[i].end(); j++) {
                if (find(currpth.begin(), currpth.end(), *j) != currpth.end()) {
                    disj = false;
                    //j = pthloops[i].size();
                }
            }
            }
            
        }
*/
        tdise = getCPUTime();
        distime += timeDifference(tdise, tdisb);
        if (disj) {
            
            disjointtrues++;
            //std::cout << "disjoints: " << disjointtrues << std::endl;
            midstep = false;
            std::set<SgGraphNode*> pthloopstmp;
            pthloopstmp.clear();
                for (int i = 0; i < currpth.size(); i++) {
                    //currflat.push_back(currpth[i]);
                    if (mkloops.find(currpth[i]) != mkloops.end()) {
                       pthloopstmp.insert(currpth[i]);
                    }
                }
                pthloops.push_back(pthloopstmp);
                path.push_back(currpth);
                pthloopstmp.clear();
                
                //std::set<std::vector<SgGraphNode*> > lpth;
                std::vector<SgGraphNode*> oldcurrpth = currpth;
                currpth.clear();
                if (currents.find((path.back()).back()) == currents.end()) {
                    currents[(path.back()).back()] = 0;
                }
                        SgGraphNode* frontnode = (path.back()).front();
                        SgGraphNode* backnode = (path.back()).back();
     
                        ROSE_ASSERT(pathsAtMk.find(backnode) != pathsAtMk.end() || backnode == endnode);
                        ROSE_ASSERT(pathsAtMk.find(frontnode) != pathsAtMk.end());
                        if (currents.find(backnode) == currents.end()) {
                            currents[backnode] = 0;
                        }
                        else {
                            ROSE_ASSERT(currents[backnode] == 0);
                        }
                        std::vector<std::vector<SgGraphNode*> > tmppths = pathsAtMk[backnode];
                       
                        currpth = tmppths[currents[backnode]];
                        ROSE_ASSERT(currpth != oldcurrpth);
                        currents[backnode]++;
                    }        
     else {
        disjoints++;
        //std::cout << "disjoint false: " << s << std::endl;
        
        while (true) {
            if (currents[(path.back()).back()] < pathsAtMk[(path.back()).back()].size() || path.back().back() == realstartnode) {
                break;
            }
            currents[(path.back()).back()] = 0;
            path.pop_back();
            pthloops.pop_back();
        }
        if ((path.back()).back() != realstartnode) {
        currpth = (pathsAtMk[(path.back()).back()])[currents[(path.back()).back()]];
        currents[(path.back()).back()]++;
        }
        else {
        step = true;
        }
    }
 }
 }
std::cout << "successes: " << successes << std::endl;
std::cout << "failures: " << failures << std::endl;

return;
}
    
        
//these are debugging functions, used to visually ascertain where the paths are going to check to make sure everything is evaluated


/* DEBUGGING */

template<class InheritedAttributeType, class SynthesizedAttributeType>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
printNodePlusEdgesForAnalysis(SgIncidenceDirectedGraph* g, SgGraphNode* n, int loopNum, int pathVal, std::ofstream& ss) {
  printNodeForAnalysis(n, loopNum, pathVal, ss);
  std::set<SgDirectedGraphEdge*> outEdges = g->computeEdgeSetOut(n);
  for (std::set<SgDirectedGraphEdge*>::iterator i = outEdges.begin(); i != outEdges.end(); i++) {
    if (nullEdgesOrdered.find(*i) == nullEdgesOrdered.end()) {
        printEdgeForAnalysis(*i, false, ss);
    }
    else {
        printEdgeForAnalysis(*i, true, ss);
    }   
  }
}

template<class InheritedAttributeType, class SynthesizedAttributeType>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
printNodePlusEdgesForAnalysisPath(SgIncidenceDirectedGraph* g, std::vector<SgGraphNode*> n, int loopNum, int pathVal, std::ofstream& ss) {
  for (unsigned int i = 0; i < n.size()-1; i++) {
     if (completedNodesPath.find(n[i]) == completedNodesPath.end()) {
         printNodeForAnalysis(n[i], loopNum, pathVal, ss);
         completedNodesPath.insert(n[i]);
     }
     std::pair<SgGraphNode*, SgGraphNode*> prnod;
     prnod.first = n[i+1];
     prnod.second = n[i];
     if (completedEdgesPath.find(prnod) == completedEdgesPath.end()) {
         printEdgeForAnalysisPath(n[i+1], n[i], ss);
         completedEdgesPath.insert(prnod);
     }
  }
  if (completedNodesPath.find(n[n.size() - 1]) == completedNodesPath.end()) {
     printNodeForAnalysis(n[n.size()-1], loopNum, pathVal, ss);
     completedNodesPath.insert(n[n.size() - 1]);
  }

}


template<class InheritedAttributeType, class SynthesizedAttributeType>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
printNodeForAnalysis(SgGraphNode* n, int loopNum, int pathNum, std::ofstream &ss) {
  int id = n->get_index();
  std::string nodeColor = "black";
  if (loopNum != 0) {
  ss << id << " [label=\"" << "LoopNumS" << loopNum << "\", color=\"" << "green" << "\", style=\"" << "solid" << "\"];\n";
  }
  else {
  ss << id << " [label=\"" << "pathNumS" << pathNum << "\", color=\"" << "black" << "\", style=\"" << "dotted" << "\"];\n";
  }
  
}
template<class InheritedAttributeType, class SynthesizedAttributeType>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
printEdgeForAnalysis(SgDirectedGraphEdge* e, bool isNullEdge, std::ofstream &ss) {
  if (isNullEdge) {
      ss << e->get_from()->get_index() << " -> " << e->get_to()->get_index() << " [label=\"" << "NullEdge" << "\", style=\"" << "dotted" << "\"];\n";
  }
  else {
      ss << e->get_from()->get_index() << " -> " << e->get_to()->get_index() << " [label=\"" << "\", style=\"" << "solid" << "\"];\n";
  }
}
template<class InheritedAttributeType, class SynthesizedAttributeType>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
printEdgeForAnalysisPath(SgGraphNode* g1, SgGraphNode* g2, std::ofstream &ss) {
      ss << g2->get_index() << " -> " << g1->get_index() << " [label=\"" << "Edge" << "\", style=\"" << "solid" << "\"];\n";
}

/* END DEBUGGING */

//This function sets up the graph so that the evaluatePath function can easily traverse the paths

template<class InheritedAttributeType, class SynthesizedAttributeType>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
solvePaths(SgIncidenceDirectedGraph* g, SgGraphNode* n, SgGraphNode* endnode) {
    bool done = false;
    bool edges = true;
    bool tookone = false;
    std::vector<SgGraphNode*> mkpath;
    std::vector<SgGraphNode*> marks;
    marks.push_back(n);
    mkglobal.push_back(n);
    SgGraphNode* currn = n;
    SgGraphNode* took;
    std::set<SgDirectedGraphEdge*> taken;
    std::vector<SgGraphNode*> toTake;
    std::vector<SgGraphNode*> path;
    path.push_back(n);
    mkpath.push_back(n);
    int itr = 0;
    int bifurcations = 0;
    std::map<SgGraphNode*, bool> completed;
    while (done == false) {
             ROSE_ASSERT(currn != NULL);
//check to see if we've hit the endnode or if we're done, if not continue, if so push the subpath into the "pathsAtMk" repository            
               if (currn == endnode || completed.find(currn) != completed.end()) {
                if (pathsAtMk.find(marks.back()) == pathsAtMk.end()) {
                    std::vector<std::vector<SgGraphNode*> > emptypath;
                    pathsAtMk[marks.back()] = emptypath;
                }
                edges = false;
                pathsAtMk[marks.back()].push_back(mkpath);
                //for (int mk = 0; mk < mkpath.size(); mk++) {
                 //   std::set<SgDirectedGraphEdge*> iedg = g->computeEdgeSetIn(mkpath[mk]);
                    //if (iedg.size() > 1) {
                    //    ploops.insert(mkpath[mk]);
                   // }
                //}
                ROSE_ASSERT(mkpath.front() == marks.back());
                if (marks.size() == 0) {
                    return;
                }
                mkpath.clear();
                bool y = true;
                bool haventtaken = false;
                bool p = true;
                int place;
                bool found = false;
                while (found == false) {
                    if (marks.size() == 0) {
                        return;
                    }
                    SgDirectedGraphEdge* tooked;
                    SgGraphNode* mark1 = marks.back();
                    std::set<SgDirectedGraphEdge*> oedg = g->computeEdgeSetOut(mark1);
                    ROSE_ASSERT(oedg.size() > 1 || mark1 == n);
                        for (std::set<SgDirectedGraphEdge*>::iterator j = oedg.begin(); j != oedg.end(); j++) {
                            if (taken.find(*j) == taken.end() && haventtaken == false) {
                                tooked = *j;
                                haventtaken = true;
                            }
                        }
                        if (haventtaken == true) {
                            if (marks.back() == n) {
                                path.clear();
                            }
                            path.push_back(marks.back());
                            if ( mkpath.empty() || (mkpath.back() != marks.back()) ) {
                                ROSE_ASSERT(!marks.empty());
                                mkpath.push_back(marks.back());
                            }
                            taken.insert(tooked);
                            took = tooked->get_to();
                            found = true;
                        }
                        else {
                            completed[marks.back()] = true;
                            bifurcations++;
                            marks.pop_back();
                        }
                    }
                    if (marks.size() == 0) {
                        return;
                    } 
                    haventtaken = false;
                    found = false; 
                                
             }
//if we haven't reached the endnode or completed, continue down the graph
             else {
            std::set<SgDirectedGraphEdge*> oedg = g->computeEdgeSetOut(currn);
            std::set<SgDirectedGraphEdge*> iedg = g->computeEdgeSetIn(currn);
            if (oedg.size() > 1) {
                     if (mkpath.back() != currn) {
                     mkpath.push_back(currn);
                     }
                     pathsAtMk[marks.back()].push_back(mkpath);
                     mkpath.clear();
                     mkpath.push_back(currn);
                     marks.push_back(currn);
                     if (find(mkglobal.begin(), mkglobal.end(), currn) == mkglobal.end()) {
                         mkglobal.push_back(currn);
                     }
                     for (std::set<SgDirectedGraphEdge*>::iterator i = oedg.begin(); i != oedg.end(); i++) {
                          if (taken.find(*i) == taken.end() && tookone == false) {
                               taken.insert(*i);
                               tookone = true;
                               took = (*i)->get_to();
                          }
                          else if (taken.find(*i) == taken.end() && tookone == true) {
                               //toTake.push_back((*i)->get_to());
                          }
                   }
                   tookone = false;
           }
           else {
                took = (*(oedg.begin()))->get_to();
           }
           }
           itr++;
           
           if (find(path.begin(), path.end(), took) == path.end()) {
               mkpath.push_back(took);
               path.push_back(took);
               currn = took;
           }
           else {
               mkloops.insert(took);
               std::vector<SgGraphNode*> lptemp;
               lptemp.clear();
               lptemp.push_back(took);
               while (path.back() != took) {
                   
                   path.pop_back();
                  
                   lptemp.push_back(path.back());
                   
               }
               (mkloopmap[took]).insert(lptemp);
/*
               if (lptemp.size() > 1) {
               if (find(looppaths.begin(), looppaths.end(), lptemp) == looppaths.end() && find(lptemp.begin(), lptemp.end(), st) == lptemp.end() && find(lptemp.begin(), lptemp.end(), endnode) == lptemp.end()) {
                   looppaths.push_back(lptemp);
                   loopnum++;
                   for (unsigned int i = 0; i < lptemp.size(); i++) {
                       loopNumMap[lptemp[i]] = loopnum;
                   }
               }
               }
*/
               path.push_back(took);
               currn = path.back();
               mkpath.push_back(took);
           }
           
               
}
           return;
}

//not currently useful
template <class InheritedAttributeType, class SynthesizedAttributeType>
SynthesizedAttributeType
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
defaultSynthesizedAttribute(InheritedAttributeType inh)
{
    SynthesizedAttributeType s = SynthesizedAttributeType();
    return s;
}


//computes the order in which to evaluate the nodes in nodal analysis so that you don't evaluate a node before you evaluate its parents

template <class InheritedAttributeType, class SynthesizedAttributeType>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
computeOrder(SgIncidenceDirectedGraph* g, SgGraphNode* n, SgGraphNode* endnode) {
    std::map<SgGraphNode*, int> incomputables;
    std::set<SgGraphNode*> lpposs;
    //std::set<SgGraphNode*> lps;
    SgGraphNode* currn;
    currn = n;
    int orders = 0; 
    while (true) {
        if (orders % 10000 == 0) {
            std::cout << "orders: " << orders << std::endl;
        }
        orders++;
        if (currn == endnode) {
        }
        if (computable(g, currn) || currn == n) {
            int mp;
            if (oVals.find(currn) == oVals.end()) {
                 oVals[currn] = currm++;
                 iVals[currm++] = currn;
                 currm += 1;
            }
            if (currn == endnode) {
                
                break;
            }
            std::pair<bool, SgGraphNode*> pbs = getNextChild(g, currn);
            computedNodes.insert(currn);
            ROSE_ASSERT(pbs.first == true);
            currn = pbs.second;
        }
        else {
            std::pair<bool, SgGraphNode*> pbp = getNextPar(g, currn);
            ROSE_ASSERT(pbp.first == true);
            currn = pbp.second;
            
        }
        
    }
    std::cout << "required orders" << orders << std::endl;
    std::cout << "incomputables.size() " << incomputables.size() << std::endl;
}

//simple fucntion to check the computability under nodal analysis
template <class InheritedAttributeType, class SynthesizedAttributeType>
bool
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
computable(SgIncidenceDirectedGraph* g, SgGraphNode* n) {
    if (computedNodes.find(n) != computedNodes.end()) {
        return true;
    }
    std::set<SgDirectedGraphEdge*> ed = g->computeEdgeSetIn(n);
    bool comp = true;
    for (std::set<SgDirectedGraphEdge*>::iterator i = ed.begin(); i != ed.end(); i++) {
        if (oVals.find((*i)->get_from()) == oVals.end() && nullEdgesOrdered.find(*i) == nullEdgesOrdered.end()) {
            comp = false;
        }
    }
    return comp;
}


//computes the inherited attribute values in nodal analysis

template <class InheritedAttributeType, class SynthesizedAttributeType>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
computeInheritedOrdered(SgIncidenceDirectedGraph* g, SgGraphNode* n) {
    int runs = 0;
//    std::ofstream mf;
//    mf.open("analysis.dot");
//    mf << "digraph defaultName { \n";
    for (std::map<int, SgGraphNode*>::iterator i = iVals.begin(); i != iVals.end(); i++) {
        runs++;
        ROSE_ASSERT(canEval(g, (*i).second));
        setPathVal(g, n);
        //printNodePlusEdgesForAnalysis(g, (*i).second, loopNumMap[(*i).second], pathValMap[(*i).second], mf);
        evalNodeOrdered(g, (*i).second);
    }
}

//checks to see if evaluation is possible under nodal analysis
template <class InheritedAttributeType, class SynthesizedAttributeType>
bool
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
canEval(SgIncidenceDirectedGraph* g, SgGraphNode* n) {
    bool evaled = true;
    if (inhVals.find(n) == inhVals.end()) {
    std::set<SgDirectedGraphEdge*> ins = g->computeEdgeSetIn(n);
    for (std::set<SgDirectedGraphEdge*>::iterator i = ins.begin(); i != ins.end(); i++) {
        if (inhVals.find((*i)->get_from()) == inhVals.end() && nullEdgesOrdered.find(*i) == nullEdgesOrdered.end()) {
            evaled = false;
        }
    }
    }
    return evaled;
}


//actually does the evaluation
template <class InheritedAttributeType, class SynthesizedAttributeType>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
evalNodeOrdered(SgIncidenceDirectedGraph* g, SgGraphNode* n) {
    if (inhVals.find(n) != inhVals.end()) {
        return;
    }
    std::set<SgDirectedGraphEdge*> par = g->computeEdgeSetIn(n);
    std::vector<InheritedAttributeType> inh;
    for (std::set<SgDirectedGraphEdge*>::iterator i = par.begin(); i != par.end(); i++) {
        if (inhVals.find((*i)->get_from()) != inhVals.end()) {
            inh.push_back(inhVals[(*i)->get_from()]);
        }
    }
    
    if (n != st || inh.size() > 0) {
        InheritedAttributeType inhX;
        inhX = evaluateInheritedAttribute(n, inh);
        inhVals[n] = inhX;
    }
    //std::cout << "num of inhVals: " << inh.size() << std::endl;
    
}


//debugging function, currently not useful for the end user
template <class InheritedAttributeType, class SynthesizedAttributeType>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
setPathVal(SgIncidenceDirectedGraph* g, SgGraphNode* currn) {
       if (pathValMap.find(currn) != pathValMap.end()) {
           return;
       }
       std::set<SgDirectedGraphEdge*> ined = g->computeEdgeSetIn(currn);
       int tmppathcount = 0;
       for (std::set<SgDirectedGraphEdge*>::iterator i = ined.begin(); i != ined.end(); i++) {
            ROSE_ASSERT(pathValMap.find((*i)->get_from()) != pathValMap.end() /*|| nullEdgesOrdered.find(*i) != nullEdgesOrdered.end()*/);
            //if (nullEdgesOrdered.find(*i) != nullEdgesOrdered.end()) {
            //    pathValMap[(*i)->get_from()] = 0;
           // }
            int pv = pathValMap[(*i)->get_from()];
            if (pv != 0) {
            tmppathcount += pv;
            }
        }
        pathValMap[currn] = tmppathcount;
        return;
    }

//computes the next child to be analyzed in nodal analysis
template <class InheritedAttributeType, class SynthesizedAttributeType>
std::pair<bool, SgGraphNode*>
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
getNextChild(SgIncidenceDirectedGraph* g, SgGraphNode* n) {
    bool nullPoss = false;
    //std::cout << "nextChild" << std::endl;
    std::set<SgDirectedGraphEdge*> outs = g->computeEdgeSetOut(n);
    //std::cout << "outs.size(): " << outs.size() << std::endl;
    //std::cout << "outs: " << outs.size() << std::endl;
    SgGraphNode* nextNode;
    SgGraphNode* nullNode;
    bool completed = false;
    bool completeNull = false;
    
    for (std::set<SgDirectedGraphEdge*>::iterator i = outs.begin(); i != outs.end(); i++) {

        if (outs.size() == 1) {
            nextNode = (*i)->get_to();
            if (nullEdgesOrdered.find(*i) != nullEdgesOrdered.end()) {
                nullNum++;
            }
            //completedEdges.insert(*i);
            completed = true;
        }
        else if (completed == false && computedNodes.find((*i)->get_to()) == computedNodes.end()) {
            completed = true;
            nextNode = (*i)->get_to();
            if (nullEdgesOrdered.find(*i) != nullEdgesOrdered.end()) {
                nullNum++;
            }
            completedEdgesOut.insert(*i);
        }
        
            
    }
    std::pair<bool, SgGraphNode*> pr;
    ROSE_ASSERT (completed == true || completeNull == true);
    if (completed == true) {
        pr.first = completed;
        pr.second = nextNode;
        return pr;
    }
    else {
        pr.first = true;
        pr.second = nullNode;
        return pr;
    } 
    
}

//computes the next parent to be analyzed in nodal analysis
template <class InheritedAttributeType, class SynthesizedAttributeType>
std::pair<bool, SgGraphNode*>
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType>::
getNextPar(SgIncidenceDirectedGraph* g, SgGraphNode* n) {
    std::set<SgDirectedGraphEdge*> ins = g->computeEdgeSetIn(n);
    SgGraphNode* nextPar;
    SgDirectedGraphEdge* nullEdgeO;
    bool completed = false;
    bool completeNull = false;
    for (std::set<SgDirectedGraphEdge*>::iterator i = ins.begin(); i != ins.end(); i++) {

        if (ins.size() == 1 /*&& completedEdges.find(*i) == completedEdges.end()*/) {
            completed = true;
            completedEdges.insert(*i);
            nextPar = (*i)->get_from();
        }

        else if (completedEdges.find(*i) == completedEdges.end() && completed == false) {
            completed = true;
            nextPar = (*i)->get_from();
            completedEdges.insert(*i);
        }

        else if (completedEdges.find(*i) != completedEdges.end() && computedNodes.find((*i)->get_from()) == computedNodes.end() && completed == false /*&& nullEdgesOrdered.find(*i) == nullEdgesOrdered.end()*/) {
            completeNull = true;
            std::pair<SgGraphNode*, SgGraphNode*> lpp;
            nextPar = n;
            nullEdgesOrdered.insert(*i);
            nullEdgesPaths++;
            
        }
    }
    ROSE_ASSERT(completed == true || completeNull == true);
    std::pair<bool, SgGraphNode*> pr;
    pr.first = completed;
    pr.second = nextPar;

    if (completeNull == true && completed == false) {
        pr.first = completeNull;
        pr.second = nextPar;
    }
   
    return pr;
}
 
    






 
    

  
        
    
            
