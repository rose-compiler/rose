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
#include <graphTemplate.h>

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







//int gos;

std::string printnum(int i)
{
    std::string s;
    std::stringstream out;
    out << i;
    s = out.str();
    return s;
}
/*
void printCFGNode(FilteredCFGNode<CFGNodeFilter2>* cf, CFG* cfg, int nodenum)
{

        SgNode* node = cf->getNode();

//       if (nodenums.find(node) == nodenums.end()) {
//            nodenums[node] = currnodenum;
//            currnodenum++;
//        }

        ROSE_ASSERT(node);

        std::string nodeColor = "black";
        if (isSgStatement(node))
                nodeColor = "blue";
        else if (isSgExpression(node))
                nodeColor = "green";
        else if (isSgInitializedName(node))
                nodeColor = "red";

        std::string label;

        if (SgFunctionDefinition* funcDef = isSgFunctionDefinition(node))
        {
                std::string funcName = funcDef->get_declaration()->get_name().str();
                if (cf->getIndex() == 0)
                        label = "Entry\\n" + funcName;
                else if (cf->getIndex() == 3)
                        label = "Exit\\n" + funcName;
        }

        if (!isSgScopeStatement(node) && !isSgCaseOptionStmt(node) && !isSgDefaultOptionStmt(node))
        {
                std::string content = node->unparseToString();
                content += printnum(nodenum);
                boost::replace_all(content, "\"", "\\\"");
                boost::replace_all(content, "\\n", "\\\\n");
                label += content;
        }
        else
                label += "<" + node->class_name() + ">";

        if (label == "")
                label += "<" + node->class_name() + " " + printnum(nodenum) + " " + printnum(cf->getIndex()) + ">";

        std::cout << label << std::endl;
}

*/

struct Bot
{
    std::vector<std::vector<int> > path;
    std::vector<std::set<int> > pthloops;
    std::vector<int> currpth;
    std::vector<std::pair<int, int> > nodelst;
    bool on;
    bool remove;
};




double timeDifference( struct timeval& end,  struct timeval& begin)
{
    return (end.tv_sec + end.tv_usec / 1.0e6) - (begin.tv_sec + begin.tv_usec / 1.0e6);
}

static inline timeval getCPUTime()
{
    rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return ru.ru_utime;
}

/*
struct compareSgGraphNode {
    bool operator()( int a,  int b)
    {
        return a==b;
    }
};
*/


/* The SgGraphTraversal class is utilized specifically for StaticCFG traversals,
though the input must be in terms of a CFG*/
template <class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
class SgGraphTraversal
{

public:
    //CFG* completeGraph;
    //currvert = 0;
    //gos = 0;
    typedef typename boost::graph_traits<CFG>::vertex_descriptor Vertex;
    typedef typename boost::graph_traits<CFG>:: edge_descriptor Edge;
    void printHotness(const CFG*& g);
    void printCFGEdge(int ed, const CFG*& g, std::ofstream& o);
    void printCFGNode(int n, std::ofstream& o);
    //typedef typename CFG::Vertex Vertex;
    //typedef typename CFG::Edge Edge;
    typedef typename boost::graph_traits<CFG>::vertex_iterator vertex_iterator;
    typedef typename boost::graph_traits<CFG>::out_edge_iterator out_edge_iterator;
    typedef typename boost::graph_traits<CFG>::in_edge_iterator in_edge_iterator;
    typedef typename boost::graph_traits<CFG>::edge_iterator edge_iterator;
    void getEntryExit(const CFG*& cfg);
    std::vector<int> sources;
    std::vector<int> sinks;
    int pathnum;
    std::map<Vertex, int> intVertex;
    std::map<int, Vertex> Vertexint;
    SgGraphTraversal();
    std::set<std::map<int, std::set<int> > > subpathmap;
    int loopNum;
    int nullNum;
    std::set<int> nullEdgesOrdered;
    std::map<int, int> loopNumMap;
    std::map<int, int> pathValMap;
    int nullloops;
    std::vector<std::vector<int> > looppaths;
    std::vector<std::vector<int> > iLoops;
    std::vector<int> ifstatements;
    virtual ~SgGraphTraversal();
    //SgGraphTraversal();
    // Copy operations
    int nullEdgesPaths;
    int turns;
    SgGraphTraversal( SgGraphTraversal &);
    SgGraphTraversal &operator=( SgGraphTraversal &);
    //This is not used, but will be important if SynthesizedAttributes become useful
    typedef StackFrameVector<SynthesizedAttributeType> SynthesizedAttributesList;
    //one of the most important structures in the algorithm, this attaches ints to InheritedAttributeTypes so that
    //looking up the values is possible.
    //int numnodes;
    //std::map<int, InheritedAttributeType> seen;
    int numnodes;
    //InheritedAttributeType pthgraphinherit;
    //StaticCFG::CFG* SgCFG;
    int nullnode;
    std::map<int, int> primenode;
    bool done;
    std::vector<std::vector<int> > senpaths;
    //std::set<int> startnodes;
    std::set<int> lstN;
    std::map<int, std::vector<std::set<int> > > lstordmap;
    std::set<int> solvedLoops;
    std::map<int, std::vector<int> > checkednodes;
    std::map<int, std::set<int> > downed;

    //std::map<int, int> nodeinedgordmap;
    //a value for nodes that have no value, set in the traverse function
    InheritedAttributeType nullInherit;
    int loopdetectnum;

    int pnum;
    //the user invoked function, runs the algorithm
    InheritedAttributeType traverse(Vertex &basenode, CFG* & g,
                                    InheritedAttributeType &inheritedValue, InheritedAttributeType& nullInherit,
                                    Vertex &endnode, bool insep = false, bool pcHk = false);
    std::set<int> loopSet;

protected:
    //User defined functions to do whatever is needed in evaluation
    //All the user gets access to is the node in question
    //and the values of the parent nodes (this should be all that is necessary)
    virtual InheritedAttributeType evaluateInheritedAttribute(Vertex &n,
            std::vector<InheritedAttributeType> &inheritedValues) = 0;
    //Not used, but may be useful if SynthesizedAttributes become workable in this context
    //virtual SynthesizedAttributeType evaluateSynthesizedAttribute( int &n,
    //         InheritedAttributeType &in,
    //         SynthesizedAttributesList &l) = 0;
    virtual void pathAnalyze( std::vector<int> &pth, bool loop=false,  std::set<std::vector<int> >& incloops=NULL) = 0;


    //virtual bool subPathFilter(std::vector<int> &subpath);
    //virtual CFG getChildren(
    //also not used, but important for possible later use of SynthesizedAttributes
    SynthesizedAttributeType defaultSynthesizedAttribute(InheritedAttributeType);
private:
    double distime;
    //CFG* completeGraph;
    //std::set<std::pair<std::pair<int, int>, std::pair<int, int> > > flpset;
    //std::set<std::pair<std::pair<int, int>, std::pair<int, int> > > goodset;
    std::set<int> ploops;
    std::map<int, std::set<std::vector<int> > > lpbegins;
    std::map<int, int> frksLeft;
    int currm;
    int dpMax;
    int repEval;
    bool pathCheck;
    int pathsSize;
    int loopcount;
    //this ructs the graph tree for computation of inheritedValues


    std::map<int, InheritedAttributeType> known;
    std::vector<InheritedAttributeType> connectNodes;
    std::map<int, bool> solved;
    std::set<int> solvedset;
    //these two are not used, but will be important if SynthesizedAttributes are made reasonable in this context
    SynthesizedAttributesList *synthesizedAttributes;
    SynthesizedAttributeType traversalResult();
    //finally we have two functions necessary for parallel processing if that is chosen to be used by the user





    std::map<int, int> nodeInEdgesNum;
    int currprime;
    std::vector<int> endnodefakes;
    std::map<int, std::vector<std::vector<int> > > pathsAtMk;
    std::set<int> mkloops;
    std::map<int, std::set<std::vector<int> > > mkloopmap;
    std::map<int, std::set<std::vector<int> > > subPathsAtMk;
    std::vector<int> mkglobal;
    std::vector<int> clglobal;
    bool inseparable;
    void solvePaths(const CFG* &g, const int &n, const int &endnode);
    void solvePaths2(const CFG* &g, const int &n, const int &endnode);

    std::vector<std::set<int> > closuresVec;
    void evaluatePaths(const CFG* & g, int realstartnode/*, const int &endnode*/);
    //void evaluatePathsPar(CFG* g, int realstartnode, int endnode);
    bool disjoint(std::vector<int>& path, std::vector<int>& vec2) ;
    std::set<std::vector<int> > flatpaths;
//    void evalNode(CFG* g, int n);
    bool canSolve(const CFG* &g, const int &n);
    std::map<int, InheritedAttributeType> inhVals;
    //std::set<CFG::CFGEdgePtr> seenEdges;
    std::set<int> nullEdges;
    std::set<int> clsT;
    void computeOrder(const CFG* &g, const int& n, const int& endnode);
    //void computeOrder(CFG* g, int n, int endnode);

    void computeInheritedOrdered(const CFG* &g, const int &n);
    std::pair<bool, int> getNextPar(const CFG* &g, const int &n);
    std::pair<bool, int> getNextChild(const CFG* &g, const int &n);
    bool computable( const CFG* &g, const int &n);
    void evalNodeOrdered(const CFG* &g, const int &n);
    std::map<int, int> oVals;
    bool canEval(const CFG* &g,const int &n);
    //void setPathVal(CFG* g, int n);
    //void printNodePlusEdgesForAnalysis(CFG* g, int n, int loopNum, int pathVal, std::ofstream& ss);
    //void printNodePlusEdgesForAnalysisPath(CFG* g, std::vector<int> n, int loopNum, int pathVal, std::ofstream& ss);
    //void printNodeForAnalysis(int n, int loopNum, int pathNum, std::ofstream& ss);
    std::set<int> completedNodesPath;
    std::set<int> completedEdgesPath;
    //void printEdgeForAnalysis(CFG::CFGEdgePtr e, bool isNullEdge, std::ofstream& ss);
    //void printEdgeForAnalysisPath(int g1, int g2, std::ofstream& ss);
    std::map<int, int> iVals;

    //std::set<CFG::CFGEdgePtr> nullEdgesOrderedOut;
    std::set<int> completedEdgesOut;
    std::set<int> completedEdges;
    std::set<int> compPar;
    std::set<int> compChild;
    std::set<int> computedNodes;
    int st;
    int en;
    double fllp;
    int loopnum;
    //std::map<int, std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> > badEdgesIn;
    //std::map<int, std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> > badEdgesOut;
    int currmkloopint;
    std::map<int, int> mkLoopInt;

    std::vector<std::vector<int> > mkloopmapstore;
    std::vector<int> getOutEdges(const int& n, const CFG*& g);
    std::vector<int> getInEdges(const int& n, const CFG*& g);
    std::vector<int> getInNodes(const int& n, const CFG*& g);
    std::vector<int> getOutNodes(const int &n, const CFG*& g);
    int getSource(const int& e, const CFG*& g);
    int getTarget(const int &e, const CFG*& g);
    std::map<int, Vertex> intvertmap;
    std::map<int, Edge> intedgemap;
    std::map<Edge, int> edgeintmap;
    std::map<Vertex, int> vertintmap;
    void numerate(const CFG*& g);
    void findclmk(const CFG*& g);
    std::set<int> cls;
    std::set<int> mks;
    std::set<std::vector<int> > collected_loops;
    //std::set<int> solved;
    //InheritedAttributeType findAndReverse(int n, CFG* g);
    //evaluateAllInheritedAttribute(std::vector<InheritedAttributeType> endNodeInhVec, int endnode, std::vector<int> nodes, std::vector<InheritedAttributeType> inh);
//std::vector<InheritedAttributeType> getZeroInhs(std::vector<std::vector<std::vector<int> > > qAnsSetSet, std::vector<InheritedAttributeType> endnodeInhVec, int node);

};



/*
template <class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
void
GraphBot<InheritedAttributeType, SynthesizedAttributeType>::
travelDown(CFG* g) {
    std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> oedgs = g->out_edges(iAmHere);
    bool taken = false;
    if (oedgs.size() > 1) {
        std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> edgeTrav = clEdgeTrav[iAmHere];
        ROSE_ASSERT(clEdgeTrav.find(iAmHere) != clEdgeTrav.end());
        for (std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr>::iterator i = oedgs.begin(); i != oedgs.end(); i++) {
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


template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
SgGraphTraversal()
    : synthesizedAttributes(new SynthesizedAttributesList())
{
}


#ifndef SWIG

template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
~SgGraphTraversal()
{
    ROSE_ASSERT(synthesizedAttributes != NULL);
    delete synthesizedAttributes;
    synthesizedAttributes = NULL;
}

#endif


template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG> &
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
operator=( SgGraphTraversal &other)
{

    ROSE_ASSERT(synthesizedAttributes != NULL);
    delete synthesizedAttributes;
    synthesizedAttributes = other.synthesizedAttributes->deepCopy();

    return *this;
}

template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
printCFGNode(int cf, std::ofstream& o)
{
    std::string nodeColor = "black";
    o << cf << " [label=\"" << " num:" << cf << "\", color=\"" << nodeColor << "\", style=\"" << "solid" << "\"];\n";
    /*
        std::string label;
            if (preVal.find(cf) == preVal.end()) {
                preVal[cf] = 0;
            }
            if (hotVal.find(cf) == hotVal.end()) {
                hotVal[cf] = 0;
            }
        o << cf << " [label=\"" << "preV:" << preVal[cf] << " hotV:" << hotVal[cf] << " num:" << cf << "\", color=\"" << nodeColor << "\", style=\"" << "solid" << "\"];\n";
    */
}

template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
printCFGEdge(int cf, const CFG*& cfg, std::ofstream& o)
{
    int src = getSource(cf, cfg);
    int tar = getTarget(cf, cfg);
    o << src << " -> " << tar << " [label=\"" << src << " " << tar << "\", style=\"" << "solid" << "\"];\n";
}

template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
printHotness(const CFG*& g)
{
    //vertices(*g);
    //edges(*g);
    std::ofstream mf;
    mf.open("hotness.dot");
    mf << "digraph defaultName { \n";
    vertex_iterator v, vend;
    edge_iterator e, eend;
    for (tie(v, vend) = vertices(*g); v != vend; ++v)
    {
        printCFGNode(vertintmap[*v], mf);
    }
    for (tie(e, eend) = edges(*g); e != eend; ++e)
    {
        printCFGEdge(edgeintmap[*e], g, mf);
    }
    mf.close();
}



/**
This is the function that is used by the user directly to start the algorithm. It is immediately available to the user

SgGraphTraversal::traverse
Input:
@param[n] n starting node
@param[g] CFG* g, CFG calculated previously
@param[inheritedValue] InheritedAttributeType inheritedValue, value of the starting node
@param[nullI] InheritedAttributeType nullI, value of the null Attribute, i.e. what to attribute to a node with no value\
@param[endnode] int endnode, final node
@param[insep] boolean to decide inseparability of the analysis function, not yet in use, set automatically to false
@param[pCh] deprecated, set to false
@return InheritedAttributeType, the value of the attribute at the end node

*/

template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
inline void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
findclmk(const CFG*& g)
{
    //vertices(*g);
    //edges(*g);
    int nextEdge = 1;
    int nextNode = 1;
    vertex_iterator v, vend;
    for (tie(v, vend) = vertices(*g); v != vend; ++v)
    {
        //out_edge_iterator i, j;
        //in_edge_iterator k, l;
        //boost::tie(i, j) = boost::out_edges(*v, *g);
        //boost::tie(k, l) = boost::in_edges(*v, *g);
        std::vector<int> outs = getOutEdges(vertintmap[*v], g);
        std::vector<int> ins = getInEdges(vertintmap[*v], g);
        if (outs.size() > 1)
        {
            mks.insert(vertintmap[*v]);
        }
        if (ins.size() > 1)
        {
            cls.insert(vertintmap[*v]);
            mkloops.insert(vertintmap[*v]);
        }
    }
    std::cout << "cls's: " << cls.size() << std::endl;
    std::cout << "mks's: " << mks.size() << std::endl;
    for (std::set<int>::iterator i = mks.begin(); i != mks.end(); i++)
    {
        std::cout << *i << ", ";
    }
    std::cout << std::endl;
    std::cout << "completed find clmk" << std::endl;
}



template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
InheritedAttributeType
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
traverse(Vertex &nVert, CFG* &g, InheritedAttributeType &inheritedValue, InheritedAttributeType &nullI, Vertex &endnodeVert, bool insep, bool binary)
{
    //numnodes = 0;
    //primes.clear();
    //std::set<std::vector<int> > collected_paths;
    int loopdetectnum = 0;
    const CFG* gc = g;
    numerate(gc);
    findclmk(gc);
    printHotness(gc);
    getEntryExit(gc);
    if (nVert == NULL)
    {
        nVert = sources[0];
    }
    if (endnodeVert == NULL)
    {
        endnodeVert = sinks[0];
    }
    const Vertex nVertc = nVert;
    const Vertex endnodevertc = endnodeVert;
    int endnode = vertintmap[endnodeVert];
    const int endnodec = endnode;
    int n = vertintmap[nVert];
    const int nc = n;
    intvertmap[n] = nVert;
    intvertmap[endnode] = endnodeVert;
    pathnum = 0;
    //int n = intvertmap[endnodeVert];
    //intvertmap[0] = nVert;
    //vertintmap[nVert] = 0;
    //intvertmap[-1] = endnodeVert;
    //intvertmap[endnodeVert] = -1;
    loopcount = 0;
    currmkloopint = 0;
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
    CFG* gU = g;
    int nU = n;
    int endnodeU = endnode;
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
//   nullEdgesOrderedOut.clear();
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
//   pathCheck = pCh;
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
//   std::vector<int> lst;
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
    solvePaths2(gc, nc, endnodec);

    t2 = getCPUTime();

//making sure that endnode hasn't already been evaluated before the traversal starts, unlikely but just in case
//       ROSE_ASSERT(inhVals.find(endnode) == inhVals.end());

    //std::cout << "solvePaths done" << std::endl;
    double diff = timeDifference(t2, t1);
    t5 = getCPUTime();
    //InheritedAttributeType pthgraphinherit = botTraverse(g, n, endnode);
    oVals[n] = 0;
    iVals[0] = n;
    pathValMap[n] = 1;
    pnum = 0;
//inserting n as a computed node
    computedNodes.insert(n);
//computes the order in which the nodes must be evaluated, makes computeInheritedOrdered much faster
    computeOrder(gc, nc, endnodec);
    std::cout << "order computed" << std::endl;
//computes the nodal inheritance values
    computeInheritedOrdered(gc, nc);
    std::cout << "inheritance computed" << std::endl;
    //ROSE_ASSERT(oVals.find(endnode) != oVals.end());
    //ROSE_ASSERT(inhVals.find(endnode) != inhVals.end());
//value at the endnode
    InheritedAttributeType pthgraphinherit = inhVals[endnode];
    //= evaluateGraph(g, n, endnode, inheritedValue);
    t6 = getCPUTime();
    std::cout << "evaluateGraph done" << std::endl;
    double diff3 = timeDifference(t6, t5);
    t3 = getCPUTime();
//actually evaluates every path with a user defined pathAnalyze function
    //for (int i = 0; i < 10; i++) {
    for (int i = 0; i < sources.size(); i++)
    {
        const int ni = sources[i];
        std::vector<int> si = getOutEdges(ni, gc);
        if (si.size() != 0)
        {
            evaluatePaths(gc, ni/*, endnodec*/);
        }
    }
    //}
    t4 = getCPUTime();

    t7 = getCPUTime();
    //evaluatePathsPar(g, n, endnode);
    t8 = getCPUTime();
    std::cout << "loops found: " << collected_loops.size() << std::endl;
    std::cout << "evaluatePaths done " << std::endl;
    std::cout << "pnum: " << pnum << std::endl;
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
    for (std::set<int>::iterator i = mkloops.begin(); i != mkloops.end(); i++)
    {
        std::cout << *i << ", ";
    }
    std::cout << std::endl;
    int lnum = 1;
    for (std::set<std::vector<int> >::iterator i = collected_loops.begin(); i != collected_loops.end(); i++) {
        std::cout << "loop num: " << lnum << std::endl;
        for (int j = 0; j < (*i).size(); j++) {
            std::cout << (*i)[j] << ", ";
        }
        lnum++;
    }

    std::cout << "distime: " << distime << std::endl;
    std::cout << "fllp: " << fllp << std::endl;
    std::cout << "/*************************************************" << std::endl;
    std::cout << "pathsAtMk: " << std::endl;
    for (std::set<int>::iterator i = mks.begin(); i != mks.end(); i++)
    {

        if (pathsAtMk.find(*i) != pathsAtMk.end())
        {
            std::cout << "at: " << *i << std::endl;
            for (int j = 0; j < pathsAtMk[*i].size(); j++)
            {
                for (int k = 0; k < pathsAtMk[*i][j].size(); k++)
                {
                    std::cout << pathsAtMk[*i][j][k] << ", ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }


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
/*
bool is_disjoint(std::set<int> set1, std::set<int> set2) {

   if (set1.empty() || set2.empty()) {
       return true;
   }
   std::set<int>::iterator it1 = set1.begin();
   std::set<int>::iterator it2 = set2.begin();
   std::set<int>::iterator it1End = set1.end();
   std::set<int>::iterator it2End = set2.end();

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
*/



/**

::canSolve

This function checks to see if all the inedge nodes are either valued or are in a loop and thus
in the InheritedAttribute model have no value

Input:
@param[n] n node to check solvability
@param[g] CFG* g, CFG calculated previously
@return bool, determines whether or not the node is sovable

*/


template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
bool
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
canSolve(const CFG* &g, const  int &n)
{
    bool loop = false;
    if (inhVals.find(n) != inhVals.end())
    {
        return true;
    }
    //std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> oed = boost::in_edges(*n, *g);
    //if (oed.first == oed.second) {
    //    return false;
    // }
    //in_edge_iterator i, j;
    std::vector<int> inds = getInNodes(n, g);
    for (int i = 0; i < inds.size(); i++)
    {
        //CFG* gra = *g;
        //Edge* ed = *i;
        //Edge* edF = &(*gra[ed]);
        if (inhVals.find(inds[i]) == inhVals.end() && nullEdges.find(inds[i]) == nullEdges.end())
        {
            return false;
        }
    }
    return true;
}


/**
runs the users evaluation function on all paths, thus is responsible
for getting the paths to the user

SgGraphTraversal::evaluatePaths
Input:
@param[realstartnode] realstartnode, starting node
@param[g] CFG* g, CFG calculated previously
@param[endnode] final node calculated previously
@return void

*/

template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
evaluatePaths(const CFG* &g, int n/*, const  int &endn*/)
{
//std::set<int> seen;
//for (std::map<int, std::vector<std::vector<int> > >::iterator i = pathsAtMk.begin(); i != pathsAtMk.end(); i++) {
    /*
        std::vector<std::vector<int> > tocheck = (*i).second;
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

//currents.clear();
    //std::set<std::vector<int> > collected_loops;
    std::vector<std::vector<int> > path;
    std::vector<int> spath;
//int n = realstartnode;
//int endn = endnode;
    int successes = 0;
    int failures = 0;
    int j = 0;
    std::vector<int> currpth;
    int currint = 0;
    std::map<int, int> intPath;
//intPath[n] = currint;
    currint++;
    std::map<int, int> currents;
    currents.clear();
    int currnode;
    bool step = false;
    bool midstep = false;
//int pnum = 0;
//note: pathsAtMk is referring to subpaths connected to that marker, a marker is a split in the graph (usually an if statement)
    if (pathsAtMk[n].size() == 0)
    {
        return;
    }
    std::vector<std::vector<int> > pth = pathsAtMk[n];
    std::vector<std::vector<int> > cpth = pathsAtMk[n];
    path.clear();
    int disjoints = 0;
    int disjointtrues = 0;
    currpth = pth[0];
    intPath[pth[0].front()] = currint;
    std::set<int> pthloopstmp;
    int fakenode;
    pthloopstmp.insert(fakenode);
    std::vector<std::set<int> > pthloops;
    pthloops.push_back(pthloopstmp);
    pthloopstmp.clear();
    currint++;

    int stepnum = 0;
    std::vector<int> rs;
    rs.push_back(n);
    path.push_back(rs);
    //currflat.push_back(realstartnode);
    currents.clear();

    step = false;
    //std::vector<int> currflat;
    std::vector<int> sub;

    /*
        std::ofstream mz;
        mz.open("pathanalysis.dot");
        mz << "digraph defaultName { \n";
    */
    //std::set<std::vector<int> > nullIncLoops;

/*
    for (unsigned int p = 0; p < looppaths.size(); p++)
    {
        std::vector<int> lp = looppaths[p];

        for (unsigned int i = 0; i < lp.size()-1; i++)
        {
            for (unsigned int l = i+1; l < lp.size(); l++)
            {
                if (lp[i] == lp[l]/* && lp[i] != n && lp[i] != endn*///)
/*
                {
                    std::vector<int> interiorloop;
                    //std::vector<Vertex> interiorloopToSolve;
                    interiorloop.clear();
                    //interiorloopToSolve.clear();
                    for (unsigned int j = i; j < l+1; j++)
                    {
                        //interiorloopToSolve.push_back(vertintmap[lp[j]]);
                        interiorloop.push_back(lp[j]);
                    }
                    if (interiorloop.size() > 2 /*&& interiorloop.back() != endn*///)
/*
                    {
                        if (find(iLoops.begin(), iLoops.end(), interiorloop) == iLoops.end())
                        {
                            if (find(looppaths.begin(), looppaths.end(), interiorloop) == looppaths.end())
                            {
                                iLoops.push_back(interiorloop);
                                loopnum++;
                                for (unsigned int k = 0; k < interiorloop.size(); k++)
                                {
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
        std::vector<int> lpt;
        if (lp.size() > 2)
        {
            lpbegins[lp.front()].insert(lp);
            //for (int i = 0; i < lp.size(); i++) {
            //    lpt.push_back(vertintmap[lp[i]]);
            //}
            pathAnalyze(lp, true, nullIncLoops);
            //for (unsigned int i = 1; i < lp.size(); i++) {
            //    printNodePlusEdgesForAnalysisPath(g, lp, p, p, mz);
            //}
        }
    }
*/

    while (step == false)
    {
        stepnum++;
        //std::cout << "currpth: " << std::endl;
        //for (int i = 0; i < currpth.size(); i++)
       // {
       //     std::cout << currpth[i] << ", ";
       // }
        //std::cout << std::endl;
       // std::cout << "stepnum: " << stepnum << std::endl;

        std::vector<int> onds = getOutEdges(currpth.back(), g);
        if (onds.size() == 0)
        {
            path.push_back(currpth);
            //for (int i = 0; i < currpth.size(); i++) {
            //    currflat.push_back(currpth[i]);
            //}
            std::vector<int> flatpath;
            //std::vector<int> sub;
            std::set<std::vector<int> > incloops;
            //std::set<std::vector<Vertex> > incloopsToSolve;
            struct timeval q1, q2;
            //std::cout << "path.size(): " << path.size() << std::endl;
            //std::cout << "pthloops.size(): " << pthloops.size() << std::endl;
            ROSE_ASSERT(path.size() == pthloops.size() + 1);
            q1 = getCPUTime();

            for (unsigned int q = 0; q < pthloops.size(); q++)
            {
                //sub = path[q];
                //sub.pop_back();
                //for (unsigned int r = 0; r < path[q].size(); r++) {
                flatpath.insert(flatpath.end(), path[q].begin(), path[q].end());
                //flatpath.push_back(intvertmap[path[q][r]]);
                //}
                for (std::set<int>::iterator p = pthloops[q].begin(); p != pthloops[q].end(); p++)
                {
                    for (std::set<std::vector<int> >::iterator o = mkloopmap[*p].begin(); o != mkloopmap[*p].end(); o++)
                    {
                        incloops.insert(*o);
                        std::vector<Vertex> incpre;
                        //for (int ni = 0; ni < (*o).size(); ni++) {
                        //    incpre.push_back(intvertmap[(*o)[ni]]);
                        // }
                        //incloopsToSolve.insert(incpre);
                    }
                }
            }
            /*
                         for (int pt2 = 0; pt2 < path.size(); pt2++) {
                             for (int pt3 = 0; pt3 < path[pt2].size(); pt3++) {
                                 flatpath.push_back(intvertmap[path[pt2][pt3]])
                             }
                         }
            */

            for (unsigned int pt2 = 0; pt2 < path[path.size()-1].size(); pt2++)
            {
                flatpath.push_back(path[path.size()-1][pt2]);
            }


            q2 = getCPUTime();
            fllp += timeDifference(q2,q1);
            //std::cout << "sent path: ";
            //std::vector<int> senpath;
            //for (int i = 0; i < flatpath.size(); i++) {
            //    std::cout << ", " << vertintmap[flatpath[i]];
            //    senpath.push_back(vertintmap[flatpath[i]]);
            //}
            //ROSE_ASSERT(find(senpaths.begin(), senpaths.end(), senpath) == senpaths.end());
            //senpaths.push_back(senpath);
            //std::cout << "\n path num: " << pathnum << std::endl;
            //pathnum++;
            flatpath.push_back(currpth.back());

/*            for (unsigned int ps = 0; ps < flatpath.size(); ps++)
            {
                if (lpbegins.find(flatpath[ps]) != lpbegins.end())
                {
                    for (std::set<std::vector<int> >::iterator sv = lpbegins[flatpath[ps]].begin(); sv != lpbegins[flatpath[ps]].end(); sv++)
                    {
                        incloops.insert(*sv);
                    }
                }
            }
*/
//user defined function, run on the final path, gives the user loops that are included via "incloops" a set of vectors that contain the individual loops
            //std::cout << "analyzing pth: " << pnum << std::endl;
            pathAnalyze(flatpath, false, incloops);
            if (pnum % 1000 == 0) {
            std::cout << "pth: " << pnum << std::endl;
            //for (int qe = 0; qe < flatpath.size(); qe++)
           // {
           //     std::cout << flatpath[qe] << ", ";
            //}
            //std::cout << std::endl;
            std::cout << "completed analysis" << std::endl;
            }
            pnum++;
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


            while (true)
            {
                rounds++;
                ROSE_ASSERT(pathsAtMk.find((path.back()).back()) != pathsAtMk.end());
                std::vector<int> inEd = getInEdges(path.back().front(), g);
                if (inEd.size() == 0 && currents[(path.back()).back()] >= (pathsAtMk[(path.back()).back()].size()))
                {
                    ROSE_ASSERT(path.back().front() == n);
                    starter = true;
                }
                if (currents[(path.back()).back()] < (pathsAtMk[(path.back()).back()].size()) /*|| (path.back()).front() == realstartnode*/)
                {
                    std::vector<std::vector<int> > cpths = pathsAtMk[(path.back()).back()];
                    currpth = cpths[currents[(path.back()).back()]];
                    //if (currpth.back() != currpth.front()) {
                    currents[(path.back()).back()]++;
                    //}
                    break;
                }
                else
                {
                    currents[(path.back()).back()] = 0;
                    path.pop_back();
                    pthloops.pop_back();
                }
                if (starter == true)
                {
                    step = true;
                    break;
                }

            }
        }
        else
        {
            //std::cout << "current subpath" << std::endl;
            //std::vector<int> senpath;
            //for (int i = 0; i < path.size(); i++) {
            //    for (int j = 0; j < path[i].size(); j++)
            //    std::cout << ", " << vertintmap[path[i][j]];
            //senpath.push_back(vertintmap[path[i][j]]);
            //}
            //std::cout << std::endl;
//this checks first to see if we have any loops in our path. If not it continues down, if there is it goes back to the last nonloop node
            //  int disj = 0;
            ROSE_ASSERT(currpth.front() == path.back().back());
            bool disj = true;
            int disjI = -1;
            int disjNode = -1;
            struct timeval tdisb, tdise;
            tdisb = getCPUTime();
            //while (true) {
            
            //std::cout << std::endl;
/*
            for (int i = 0; i < pthloops.size(); i++)
            {
                for (std::set<int>::iterator j = pthloops[i].begin(); j != pthloops[i].end(); j++)
                {
                    if (find(currpth.begin()+1, currpth.end()-1, *j) != currpth.end() && disj)
                    {
*/
           for (int i = 0; i < path.size(); i++) {
               if (disj) {
               for (int j = 0; j < currpth.size(); j++) {
                   if (find(path[i].begin(), path[i].end(), currpth[j]) != path[i].end()) {
                       if (currpth[j] != path.back().back()) {
                           if (j != 0) {
                        //disjo = *j;
                            disj = false;
                            disjI = i;
                            disjNode = currpth[j];
                            //std::cout << "loop detected" << std::endl;
                            //std::cout << "loop detect: " << loopdetectnum << std::endl;
                            loopdetectnum++;
                            }
                       }
                    }
               }
               }
          }
/*
            if (disj == false && pathsAtMk[currpth.front()].size() <= currents[currpth.front()] || disj == true) {
                break;
            }
            else {
                //std::cout << "disjoint" << std::endl;
                std::vector<int> ocpth = currpth;
                //std::cout << "currpth is: " << std::endl;
                for (int k = 0; k < currpth.size(); k++) {
                //    std::cout << currpth[k] << ", ";
                }

                currpth = pathsAtMk[currpth.front()][currents[currpth.front()]];
                ROSE_ASSERT(currpth != ocpth);
                currents[currpth.front()]++;
                disj = true;
            }
*/
            //}
            /*
                    #pragma omp parallel for num_threads(4) private(i,j)
                    for (i = 0; i < pthloops.size(); i++) {
                        if (disj) {
                        for (std::set<int>::iterator j = pthloops[i].begin(); j != pthloops[i].end(); j++) {
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
            if (disj)
            {
                //      std::cout << "path: " << std::endl;
                //      for (int w = 0; w < path.size(); w++) {
                //          for (int v = 0; v < path[w].size(); v++) {
                //              std::cout << path[w][v] << ", ";
                //          }
                //       }
                //       std::cout << std::endl;

                disjointtrues++;
                //std::cout << "disjoints: " << disjointtrues << std::endl;
                midstep = false;
                std::set<int> pthloopstmp;
                pthloopstmp.clear();
                for (int i = 0; i < currpth.size(); i++)
                {
                    //currflat.push_back(currpth[i]);
                    if (mkloops.find(currpth[i]) != mkloops.end())
                    {
                        pthloopstmp.insert(currpth[i]);
                    }
                }
                pthloops.push_back(pthloopstmp);
                path.push_back(currpth);
                pthloopstmp.clear();

                //std::set<std::vector<int> > lpth;
                std::vector<int> oldcurrpth = currpth;
                currpth.clear();
                if (currents.find((path.back()).back()) == currents.end())
                {
                    currents[(path.back()).back()] = 0;
                }
                int frontnode = (path.back()).front();
                int backnode = (path.back()).back();

                //ROSE_ASSERT(pathsAtMk.find(backnode) != pathsAtMk.end() || backnode == endnode);
                //ROSE_ASSERT(pathsAtMk.find(frontnode) != pathsAtMk.end());
                //if (currents.find(backnode) == currents.end()) {
                //    currents[backnode] = 0;
                // }
                //if (currents[backnode] != 0) {
                //    currents[backnode] = 0;
                // }
                //std::cout << "backnode: " << backnode << std::endl;
                std::vector<std::vector<int> > tmppths = pathsAtMk[backnode];

                currpth = tmppths[currents[backnode]];
                //std::cout << "used backnode" << std::endl;
                ROSE_ASSERT(currpth != oldcurrpth);
                //if (frontnode != backnode) {
                currents[backnode]++;
                //}
            }
            else
            {
                std::vector<int> lpq;
                disjoints++;
                //std::cout << "disjoint false: " << s << std::endl;
                ROSE_ASSERT(currpth.front() == path.back().back());
                //path.push_back(currpth);
                
                //ROSE_ASSERT(path[disjI][d] == disjNode);
                int ps = path.size();
                for (int p1 = ps-1; p1 >= disjI+1; p1--) {
                    lpq.insert(lpq.end(), path[p1].begin(), path[p1].end());
                    currents[path.back().back()] = 0;
                    
path.pop_back();
                    pthloops.pop_back();
                }
                //std::cout << "lpq.size(): " << lpq.size() << std::endl;
                //std::cout << "disjNode: " << disjNode << std::endl;
                //std::cout << "path + currpth: " << std::endl;
                //for (int i = 0; i < path.size(); i++) {
                //    for (int j = 0; j < path[i].size(); j++) {
                //        std::cout << path[i][j] << ", ";
                //    }
                //}
                //std::cout << std::endl;
                //std::cout << "currpth" << std::endl;
                //for (int k = 0; k < currpth.size(); k++) {
                //    std::cout << currpth[k] << ", ";
               // }
                //std::cout << std::endl;
                int d = path[disjI].size()-1;
                while (path[disjI][d] != disjNode) {
                    d--;
                    lpq.push_back(path[disjI][d]);
                }
                currents[path.back().back()] = 0;
                path.pop_back();
                pthloops.pop_back();
                std::set<std::vector<int> > nullincloops;
                pathAnalyze(lpq, true, nullincloops);
                collected_loops.insert(lpq);
                int k = path.size() - 1;
                while (pathsAtMk[path[k].back()].size() <= currents[path[k].back()]) {
                    currents[path[k].back()] = 0;
                    path.pop_back();
                    pthloops.pop_back();
                    k--;                    
                }
                

          
                currpth = pathsAtMk[path.back().back()][currents[path.back().back()]];
                //path.pop_back();
                currents[path.back().back()]++;
            }
/*                
                

                    //std::vector<int> ieds = getInEdges(path.back().back(), g);
                    if (currents[(path.back()).back()] < pathsAtMk[(path.back()).back()].size()*//* || ieds.size() == 0*///)
                    //{
                    //    break;
                    //}
                    //currents[(path.back()).back()] = 0;
                    //int prevback = pth.back().back();
                   // path.pop_back();
                    //if (prevback ==
                   // pthloops.pop_back();
               // }
/*
                std::vector<int> ieds = getInEdges(path.back().back(), g);
                if (ieds.size() != 0)
                {
                    bool add = true;
                    currpth = (pathsAtMk[(path.back()).back()])[currents[(path.back()).back()]];
                    //if (currpth.back() == currpth.front()) {
                    //    add = false;
                    // }
                    //if (add) {
                    currents[(path.back()).back()]++;
                    //}
                }
                else
*/
                std::vector<int> ieds = getInEdges(path.back().back(), g);
                if (ieds.size() == 0)
                {
                    step = true;
                }
            }
        }
        std::cout << "loops: " << collected_loops.size() << std::endl;
        std::cout << "detected loops: " << loopdetectnum << std::endl;
        

    
//std::cout << "successes: " << successes << std::endl;
//std::cout << "failures: " << failures << std::endl;
//std::cout << "pnum: " << pnum << std::endl;
    return;
}


template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
inline void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
numerate(const CFG*& g)
{
    //vertices(*g);
    //edges(*g);
    int nextEdge = 1;
    int nextNode = 1;
    vertex_iterator v, vend;
    edge_iterator e, eend;
    for (tie(v, vend) = vertices(*g); v != vend; ++v)
    {
        intvertmap[nextNode] = *v;
        vertintmap[*v] = nextNode;
        nextNode++;
    }
    for (tie(e, eend) = edges(*g); e != eend; ++e)
    {
        intedgemap[nextEdge] = *e;
        edgeintmap[*e] = nextEdge;
        nextEdge++;
    }
}

template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
getEntryExit(const CFG*& g)
{
    vertex_iterator v, vend;
    //edge_iterator e, eend;
    out_edge_iterator i, j;
    in_edge_iterator k, l;
    for (tie(v, vend) = vertices(*g); v != vend; ++v)
    {
        //boost::tie(i, j) = boost::out_edges(*v, *g);
        //boost::tie(k, l) = boost::in_edges(*v, *g);
        std::vector<int> ot = getOutEdges(vertintmap[*v], g);
        std::vector<int> in = getInEdges(vertintmap[*v], g);
        if (ot.size() == 0)
        {
            sinks.push_back(vertintmap[*v]);
        }
        if (in.size() == 0)
        {
            sources.push_back(vertintmap[*v]);
        }

    }
    std::cout << "sinks: " << sinks.size() << std::endl;
    std::cout << "sources: " << sources.size() << std::endl;
}

template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
inline std::vector<int>
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
getOutNodes(const int& node, const CFG*& g)
{
    std::vector<int> outNodes;
    Vertex getOuts = vertintmap[node];
    out_edge_iterator i, j;
    for (boost::tie(i, j) = boost::out_edges(getOuts, *g); i != j; ++i)
    {
        Vertex iv = boost::target(*i, *g);
        //if (intvertmap.find(iv) == intvertmap.end()) {
        //    intvertmap[nextVertInt] = iv;
        //    vertintmap[iv] = nextVertInt;
        //    nextVertInt++;
        //}
        outNodes.push_back(intvertmap[iv]);
    }
    return outNodes;
}




template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
inline std::vector<int>
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
getOutEdges(const int &node, const CFG*& g)
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




template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
inline std::vector<int>
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
getInNodes(const int& node, const CFG*& g)
{
    std::vector<int> inNodes;
    Vertex getIns = vertintmap[node];
    in_edge_iterator i, j;
    for (boost::tie(i, j) = boost::in_edges(getIns, *g); i != j; ++i)
    {
        Vertex iv = boost::source(*i, *g);
        inNodes.push_back(intvertmap[iv]);
    }
    return inNodes;
}


template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
inline std::vector<int>
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
getInEdges(const int& node, const CFG*& g)
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

template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
inline int
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
getSource(const int& edge, const CFG*& g)
{
    Edge e = intedgemap[edge];
    Vertex v = boost::source(e, *g);
    return(vertintmap[v]);
}

template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
inline int
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
getTarget(const int& edge, const CFG*& g)
{
    Edge e = intedgemap[edge];
    Vertex v = boost::target(e, *g);
    return(vertintmap[v]);
}




/**
Runs preliminary analysis of the graph to allow for quick path calculations

SgGraphTraversal::solvePaths
Input:
@param[n] n starting node
@param[g] CFG* g, CFG calculated previously
@param[endnode] final node
@return void

*/

template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
solvePaths2(const CFG* &g, const int &n, const int &endnode)
{
    std::set<int> seennodes;
    for (int j = 0; j < sources.size(); j++)
    {
        const int sj = sources[j];
        std::vector<int> oeds = getOutEdges(sources[j], g);
        if (oeds.size() != 0)
        {
            mks.insert(sources[j]);
        }
    }
    for (std::set<int>::iterator i = mks.begin(); i != mks.end(); i++)
    {
        //seennodes.insert(*i);
        std::vector<int> ondsE = getOutEdges(*i, g);
        std::vector<int> onds;
        for (int q = 0; q < ondsE.size(); q++)
        {
            onds.push_back(getTarget(ondsE[q], g));

        }
        //std::cout << "mk: " << *i << std::endl;
        //std::cout << "onds: " << std::endl;
        //for (int q = 0; q < onds.size(); q++)
        //{
        //    std::cout << onds[q] << ", ";
        //}
        //std::cout << std::endl;
        //if (onds.size() == 0) {
        //std::vector<int> cr;
        //cr.push_back(*i);
        //pathsAtMk[*i].push_back(cr);
        //}
        //else {
        std::vector<int> currpth;
        int currn;
        for (int j = 0; j < onds.size(); j++)
        {
            currpth.clear();
            currpth.push_back(*i);
            currpth.push_back(onds[j]);
            int currn = onds[j];
            while (mks.find(currn) == mks.end())
            {
                /*
                                if (seennodes.find(currn) != seennodes.end()) {
                                    //mkloops.insert(currn);
                                }
                                else {
                                    seennodes.insert(currn);
                                }
                */
                //std::cout << "currn: " << currn << std::endl;
                std::vector<int> ons = getOutEdges(currn, g);
                //std::cout << "ons.size(): " << ons.size() << std::endl;
                ROSE_ASSERT(ons.size() == 1 || ons.size() == 0);
                if (ons.size() == 0)
                {
                    break;
                }
                else
                {
                    /*
                                        if (seennodes.find(getTarget(ons[0], g)) != seennodes.end()) {
                                            //mkloops.insert(getTarget(ons[0], g));
                                        }
                                        else {
                                            seennodes.insert(getTarget(ons[0], g));
                                        }
                                    }
                                    if (mks.find(getTarget(ons[0], g)) != mks.end()) {
                                        currpth.push_back(getTarget(ons[0], g));
                                        break;
                                    }
                                    else if (find(currpth.begin(), currpth.end(), getTarget(ons[0], g)) != currpth.end()) {
                                        currpth.push_back(getTarget(ons[0], g));
                                        break;
                                    }
                                    else {
                    */
                    currpth.push_back(getTarget(ons[0], g));
                    currn = getTarget(ons[0], g);
                    //seennodes.insert(currn);
                    //}

                }
            }
            if (currpth.front() != currpth.back() || currpth.size() == 1) {
                pathsAtMk[*i].push_back(currpth);
            }
        }
        //}
    }
    //std::cout << "pnum: " << pnum;
}





template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
solvePaths(const CFG* &g, const int &n, const int &endnode)
{
    bool done = false;
    bool edges = true;
    bool tookone = false;

    //currn = _cast<int> (n);
    //currn = _cast<int> (n);
    std::vector<int> mkpath;
    //loopcount = 0;
    std::vector<int> marks;
    marks.push_back(n);
    mkglobal.push_back(n);
    // int currn; /*= new int/;
    // currn = _cast<int> (n);
    //int took = new Vertex;
    int tookv;
    std::set<int> taken;
    std::vector<int> toTake;
    std::vector<int> path;
    path.push_back(n);
    mkpath.push_back(n);
    int itr = 0;
    int bifurcations = 0;
    int runs = 0;
    int currnodenum = 0;
    bool first = true;
    //std::map<FilteredCFGNode<CFGNodeFilter2>*, int> nodenums;
    std::map<int, bool> completed;
    int currn = n;
    int took;
    //std::cout << "n: " << n << std::endl;
    while (done == false)
    {
        /*
                     runs++;
                     CFG gra = *g;
                     FilteredCFGNode<CFGNodeFilter2>* tq;
                     Vertex tookv = *currn;
                     tq = &(*gra[tookv]);
                     if (nodenums.find(tq) == nodenums.end()) {
                         nodenums[tq] = currnodenum++;
                     }
                     printCFGNode(tq,g, nodenums[tq]);
        */
        //std::cout << "currn: " << currn << std::endl;
        //std::cout << "runs: " << runs << std::endl;
        //if (nodenums.find(currn) == nodenums.end()) {
        //    nodenums[currn] = currnodenum++;
        //}

        //ROSE_ASSERT(currn != NULL);
        /*
                     out_edge_iterator k1, l1;
                     boost::tie(k1, l1) = boost::out_edges(*currn, *g);
                     in_edge_iterator k2, l2;
                     boost::tie(k2, l2) = boost::in_edges(*currn, *g);
        */

        /*


                     //std::cout << "\n";
                     std::cout << "l1-k1" << l1 - k1 << std::endl;
                     std::cout << "l2-k2" << l2-k2 << std::endl;
                     if (l1 - k1 > 1 || l2 - k2 > 1) {
                         //printCFGNode(currn,g,nodenums[took]);

                         //std::cout << "outedges: " << l1 - k1 << std::endl;
                         //std::cout << "targets: " << std::endl;
                        for (boost::tie(k1, l1) = boost::out_edges(*currn, *g); k1 != l1; ++k1) {
                             //Vertex tookvPre = boost::target(*k1, *g);
                             *took = boost::target(*k1, *g);

                             //if (nodenums.find(took) == nodenums.end()) {
                             //    nodenums[took] = currnodenum++;
                            // }

                           //  printCFGNode(took,g, nodenums[took]);
                         }
                         //std::cout << "inedges: " << l2 - k2 << std::endl;
                         //std::cout << "sources" << std::endl;
                         for (boost::tie(k2, l2) = boost::in_edges(*currn, *g); k2 != l2; ++k2) {
                             //Vertex tookvPre = boost::source(*k2, *g);
                             *took = boost::source(*k2, *g);
                             //int vt = tookv;
                             //CFG* gra = *g;

                             //took = &(*gra[tookv]);
                     //if (nodenums.find(took) == nodenums.end()) {
                       //  nodenums[took] = currnodenum++;
                     //}

                            // printCFGNode(took,g,nodenums[took]);
                         }


                     }
                     else {
                         //std::cout << "simplenode" << std::endl;
                         //printCFGNode(currn, g, -1);
                     }
                     //std::cout << "\n";
        */




        //std::cout << "runs" << runs << std::endl;
        //ROSE_ASSERT(currn != NULL);

//check to see if we've hit the endnode or if we're done, if not continue, if so push the subpath into the "pathsAtMk" repository
        if (currn == endnode || completed.find(currn) != completed.end() || (mkpath.size() > 1 && mkpath.front() == mkpath.back()))
        {

            if (currn == endnode)
            {
                //std::cout << "found endnode" << std::endl;
            }
            if (completed.find(currn) != completed.end())
            {
                //std::cout << "currn completed" << std::endl;
            }
            if (mkpath.size() > 1 && mkpath.front() == mkpath.back())
            {
                //std::cout << "mkpathsize > 1, it is " << mkpath.size() << " and mkpath.front() == mkpath.back()" << std::endl;
            }
            if (pathsAtMk.find(marks.back()) == pathsAtMk.end())
            {
                std::vector<std::vector<int> > emptypath;
                pathsAtMk[marks.back()] = emptypath;
            }
            edges = false;
            pathsAtMk[marks.back()].push_back(mkpath);
            //std::cout << "marks.back(): " << std::endl;
            //printCFGNode(marks.back(), g);
            //for (int mk = 0; mk < mkpath.size(); mk++) {
            //   std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> iedg = g->in_edges(mkpath[mk]);
            //if (iedg.size() > 1) {
            //    ploops.insert(mkpath[mk]);
            // }
            //}
            ROSE_ASSERT(mkpath.size() != 0);
            //std::cout << "mkpath: ";
            //for (int i = 0; i < mkpath.size(); i++) {
            //  std::cout << mkpath[i] << ", ";
            //}
            //std::cout << "end" << std::endl;

            //std::cout << "marks: " << marks.back() << std::endl;
            ROSE_ASSERT(marks.size() != 0);
            //for (int i = 0; i < marks.size(); i++)
            //{
            //    std::cout << marks[i] << " ,";
            //}
            //std::cout << std::endl;
            ROSE_ASSERT(mkpath.front() == marks.back());
            if (marks.size() == 0)
            {
                //std::cout << "marks size == 0" << std::endl;
                return;
            }
            /*
                            if (mkpath.front() == mkpath.back() && mkpath.size() > 1) {
                                Edge* ede = boost::edge(*mkpath[mkpath.size()-2],*mkpath[mkpath.size()-1], *g).first;
                                Edge* ed = &ede;
                                Edge* ed2e = boost::edge(*mkpath[0],*mkpath[1], *g).first;
                                Edge* ed2 = &ed2e;
                                std::set<Edge*> totake;
                                totake.insert(ed);
                                std::set<Edge*> toout;
                                toout.insert(ed2);
                                //badEdgesIn[mkpath[mkpath.size()-1]].insert(*totake.begin());
                                //badEdgesOut[mkpath[0]].insert(*toout.begin());
                                ROSE_ASSERT(totake.size() < 2 && toout.size() < 2);
                                taken.insert(*totake.begin());
                            }
            */

            mkpath.clear();
            bool y = true;
            bool haventtaken = false;
            bool p = true;
            int place;
            bool found = false;
            while (found == false)
            {

                if (marks.size() == 0)
                {
                    //std::cout << "marks.size() == 0, former" << std::endl;
                    return;
                }
                int tooked;
                int ted;
                int mark1 = marks.back();
                std::vector<int> ode = getOutEdges(mark1, g);

                //std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> oedg = boost::out_edges(mark1, *g);
                //std::cout << "oedg.first - oedg.second: " << oedg.first - oedg.second << std::endl;
//                    int ted;
                ROSE_ASSERT(ode.size() > 1 || mark1 == n);
                int ik;
                for (int i = 0; i < ode.size(); i++)
                {
                    ik = ode[i];
                    if (taken.find(ik) == taken.end() && haventtaken == false)
                    {
                        tooked = ik;
                        ted = ode[i];
                        haventtaken = true;
                    }
                }
                if (haventtaken == true)
                {
                    if (marks.back() == n)
                    {
                        path.clear();
                    }
                    //if (path.back() != marks.back()) {
                    path.push_back(marks.back());
                    //}
                    if (mkpath.back() != marks.back())
                    {
                        mkpath.push_back(marks.back());
                    }
                    taken.insert(tooked);
                    took = getTarget(ted, g);
                    //Vertex tookPre = boost::target(*ted,*g);
                    //took = &tookPre;
                    //int vt = tookv;
                    //CFG* gra = *g;
                    //took = tookv;
                    found = true;
                }
                else
                {
                    completed[marks.back()] = true;
                    bifurcations++;
                    marks.pop_back();
                }
            }
            if (marks.size() == 0)
            {
                //std::cout << "marks.size() == 0, latter" << std::endl;
                return;
            }
            haventtaken = false;
            found = false;

        }
//if we haven't reached the endnode or completed, continue down the graph
        else
        {
//            std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> oedg = boost::out_edges(currn, *g);
//            std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> iedg = boost::in_edges(currn, *g);
            //ROSE_ASSERT(currn != NULL);
            std::vector<int> ode = getOutEdges(currn, g);
            /*

                                    for (boost::tie(k, l) = boost::out_edges(*currn, *g); k != l; ++k) {
                                        ode++;
                                    }
            */

            if (ode.size() > 1) /*out_edges(*currn,*g).second != boost::out_edges(*currn,*g).first)*/
            {
                if (mkpath.back() != currn)
                {
                    mkpath.push_back(currn);
                }
                pathsAtMk[marks.back()].push_back(mkpath);
                mkpath.clear();
                mkpath.push_back(currn);
                marks.push_back(currn);
                if (find(mkglobal.begin(), mkglobal.end(), currn) == mkglobal.end())
                {
                    mkglobal.push_back(currn);
                }
                int ik;
                for (int i = 0; i < ode.size(); i++)
                {
                    ik = ode[i];
                    if (taken.find(ik) == taken.end() && tookone == false)
                    {
                        taken.insert(ik);
                        tookone = true;
                        //Vertex tookpre;
                        //tookpre = boost::target(*i,*g);
                        //took = &tookpre;
                        took = getTarget(ik, g);
                        //int vt = tookv;
                        //CFG* gra = *g;
                        //took = &(*gra[tookv]);
                    }
                    else if (taken.find(ik) == taken.end() && tookone == true)
                    {
                        //toTake.push_back((*i)->get_to());
                    }
                }
                tookone = false;
            }
            else
            {
                for (int i = 0; i < ode.size(); i++)
                {
                    //tookpre = boost::target(*i, *g);
                    //took = &tookpre;
                    took = getTarget(ode[i], g);
                    //int vt = tookv;
                    //CFG* gra = *g;
                    //took = &(*gra[tookv]);
                }
                /*
                               for (boost::tie(i, j) = boost::out_edges(*currn, *g); i != j; ++i) {
                                   int tookv = &boost::target(*i,*g);
                                   took = &tookv;
                               }
                */

            }
        }
        itr++;
        //ROSE_ASSERT(took != NULL);
        //std::cout << "path so far: " << std::endl;
//               FilteredCFGNode<CFGNodeFilter2>* tq;
//               CFG gra;
//               Vertex tookv;

        //for (int i = 0; i < path.size(); i++) {
//                   gra = *g;
//                   tookv = *path[i];
//                   tq = &(*gra[tookv]);
        //   std::cout << path[i] << ", ";
        //}
        //std::cout << std::endl;

        if (find(path.begin(), path.end(), took) == path.end())
        {
            //std::cout << "no loop" << std::endl;
            //for (int i = 0; i < path.size(); i++) {
            //    printCFGNode(path[i], g);
            //}
            //std::cout << "printed path" << std::endl;
            mkpath.push_back(took);
            path.push_back(took);
            //int currn = new Vertex;
            currn = took;
        }
        else
        {
            //std::cout << "loop" << std::endl;

            //std::cout << "currn" << std::endl;
            //if (nodenums.find(currn) == nodenums.end()) {
            //  nodenums[took] = currnodenum++;
            // }
            //  printCFGNode(currn, g, nodenums[currn]);

            //    std::cout << "took" << std::endl;
            //if (nodenums.find(took) == nodenums.end()) {
            //    nodenums[took] = currnodenum++;
            //}
            // printCFGNode(took, g, nodenums[took]);
            //std::cout << "printing took: " << std::endl;
            //printCFGNode(took, g);
            //std::cout << "printing path" << std::endl;
            //for (int i = 0; i < path.size(); i++) {
            //CFG* g1 = *g;
            //int c2 = &(*g1[took]);
            //    if (i != path.size() - 1) {
            //    if (path[i] == path[i+1]) {
            //        std::cout << "self loop" << std::endl;
            //    }
            //    }
            //    printCFGNode(path[i], g);
            //}
            //std::cout << "finding loop" << std::endl;
            mkloops.insert(took);
            std::vector<int> lptemp;
            lptemp.clear();
            lptemp.push_back(took);
            int loopstep = 0;
            while (path.back() != took)
            {
                //std::cout << "loopstep = " << loopstep << "\n";
                loopstep++;
                path.pop_back();

                lptemp.push_back(path.back());

            }
            if (find(mkloopmapstore.begin(), mkloopmapstore.end(), lptemp) == mkloopmapstore.end())
            {
                mkloopmapstore.push_back(lptemp);
                for (int lpt = 0; lpt < lptemp.size(); lpt++)
                {
                    if (mkLoopInt.find(lptemp[lpt]) == mkLoopInt.end())
                    {
                        mkLoopInt[lptemp[lpt]] = currmkloopint;
                        currmkloopint++;
                    }
                    //std::cout << "--> " << mkLoopInt[lptemp[lpt]];
                }
                //std::cout << "found loop" << std::endl;
                //std::cout << "\n";
                (mkloopmap[took]).insert(lptemp);
                loopcount++;
                lptemp.clear();
                //std::cout << "num loops: " << loopcount << std::endl;

            }
            else
            {
                //std::cout << "found repeat loop" << std::endl;
                for (int lpt = 0; lpt < lptemp.size(); lpt++)
                {
                    if (mkLoopInt.find(lptemp[lpt]) == mkLoopInt.end())
                    {
                        mkLoopInt[lptemp[lpt]] = currmkloopint;
                        currmkloopint++;
                    }
                    //    std::cout << "--> " << mkLoopInt[lptemp[lpt]];
                }
                //std::cout << "\n";
            }


            if (lptemp.size() > 1)
            {
                if (find(looppaths.begin(), looppaths.end(), lptemp) == looppaths.end() && find(lptemp.begin(), lptemp.end(), st) == lptemp.end() && find(lptemp.begin(), lptemp.end(), endnode) == lptemp.end())
                {
                    looppaths.push_back(lptemp);
                    loopnum++;
                    for (unsigned int i = 0; i < lptemp.size(); i++)
                    {
                        loopNumMap[lptemp[i]] = loopnum;
                    }
                }
            }
            //int currn = new Vertex;
            path.push_back(took);
            currn = took;
            mkpath.push_back(took);
            /*
                                          std::cout << "end path" << std::endl;
                               std::cout << "took" << std::endl;
                               CFG gra = *g;
                               Vertex tookv = *took;
                               FilteredCFGNode<CFGNodeFilter2>* tq;
                               tq = &(*gra[tookv]);
                               //if (nodenums.find(tq) == nodenums.end()) {
                               //    nodenums[tq] = currnodenum++;
                              // }
                               printCFGNode(tq,g, 0);
            */


        }


    }
    return;
}

/* END DEBUGGING */

//This function sets up the graph so that the evaluatePath function can easily traverse the paths
/*
template<class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
solvePaths(CFG* g, int n, int endnode) {
    bool done = false;
    bool edges = true;
    bool tookone = false;
    std::vector<int> mkpath;
    std::vector<int> marks;
    marks.push_back(n);
    mkglobal.push_back(n);
    int currn = n;
    int took;
    std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> taken;
    std::vector<int> toTake;
    std::vector<int> path;
    path.push_back(n);
    mkpath.push_back(n);
    int itr = 0;
    int bifurcations = 0;
    std::map<int, bool> completed;
    while (done == false) {
             ROSE_ASSERT(currn != NULL);
//check to see if we've hit the endnode or if we're done, if not continue, if so push the subpath into the "pathsAtMk" repository
               if (currn == endnode || completed.find(currn) != completed.end()) {
                if (pathsAtMk.find(marks.back()) == pathsAtMk.end()) {
                    std::vector<std::vector<int> > emptypath;
                    pathsAtMk[marks.back()] = emptypath;
                }
                edges = false;
                pathsAtMk[marks.back()].push_back(mkpath);
                //for (int mk = 0; mk < mkpath.size(); mk++) {
                 //   std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> iedg = g->in_edges(mkpath[mk]);
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
                    CFG::CFGEdgePtr tooked;
                    int mark1 = marks.back();
                    std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> oedg = g->out_edges(mark1);
                    ROSE_ASSERT(oedg.size() > 1 || mark1 == n);
                        for (std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr>::iterator j = oedg.begin(); j != oedg.end(); j++) {
                            if (taken.find(*j) == taken.end() && haventtaken == false) {
                                tooked = *j;
                                haventtaken = true;
                            }
                        }
                       // if (mkpath.back() == mkpath.front() && mkpath.size() > 1) {
                       //      ROSE_ASSERT(false);
                       // }
                        if (haventtaken == true) {
                            if (marks.back() == n) {
                                path.clear();
                            }
                            path.push_back(marks.back());
                            if (mkpath.back() != marks.back()) {
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
            std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> oedg = g->out_edges(currn);
            std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> iedg = g->in_edges(currn);
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
                     for (std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr>::iterator i = oedg.begin(); i != oedg.end(); i++) {
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
               std::vector<int> lptemp;
               lptemp.clear();
               lptemp.push_back(took);
               while (path.back() != took) {

                   path.pop_back();

                   lptemp.push_back(path.back());

               }
               (mkloopmap[took]).insert(lptemp);

               if (lptemp.size() > 1) {
               if (find(looppaths.begin(), looppaths.end(), lptemp) == looppaths.end() && find(lptemp.begin(), lptemp.end(), st) == lptemp.end() && find(lptemp.begin(), lptemp.end(), endnode) == lptemp.end()) {
                   looppaths.push_back(lptemp);
                   loopnum++;
                   for (unsigned int i = 0; i < lptemp.size(); i++) {
                       loopNumMap[lptemp[i]] = loopnum;
                   }
               }
               }

               path.push_back(took);
               currn = path.back();
               mkpath.push_back(took);
           }


}
           return;
}
*/
//not currently useful
template <class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
SynthesizedAttributeType
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
defaultSynthesizedAttribute(InheritedAttributeType inh)
{
    SynthesizedAttributeType s = SynthesizedAttributeType();
    return s;
}


//computes the order in which to evaluate the nodes in nodal analysis so that you don't evaluate a node before you evaluate its parents

/**
This is the function that is used by the user directly to start the algorithm. It is immediately available to the user

SgGraphTraversal::computeOrder
Input:
@param[n] n starting node
@param[g] CFG* g, CFG calculated previously
@param[endnode] endnode, final node
@return void

*/


template <class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
computeOrder(const CFG*& g, const int& n, const int& endnode)
{
    std::map<int, int> incomputables;
    std::set<int> lpposs;
    //std::set<int> lps;
    int currn;
    currn = n;
    int orders = 0;
    while (true)
    {
        //std::cout << "currn = " << currn;
        //fg
//std::vector<InheritedAttributeType> inh;
        //evaluateInheritedAttribute(currn, inh);
        //if (orders % 10000 == 0) {
        //  std::cout << "orders: " << orders << std::endl;
        //}
        orders++;
        if (currn == endnode)
        {
        }
        // int currnC = currn;
        if (computable(g, currn) || currn == n)
        {
            int mp;
            if (oVals.find(currn) == oVals.end())
            {
                oVals[currn] = currm++;
                iVals[currm++] = currn;
                currm += 1;
            }
            if (currn == endnode)
            {
                ROSE_ASSERT(computable(g, endnode));
                //oVals[endnode] = currm++;
                //iVals[currm++] = endnode;
                //currm += 1;
                //std::cout << "currm: " << currm << std::endl;
                break;
            }
            std::pair<bool, int> pbs = getNextChild(g, currn);
            computedNodes.insert(currn);
            ROSE_ASSERT(pbs.first == true);
            currn = pbs.second;
        }
        else
        {
            std::pair<bool, int> pbp = getNextPar(g, currn);
            ROSE_ASSERT(pbp.first == true);
//            ROSE_ASSERT(pbp.second != NULL);
            currn = pbp.second;

        }

    }
    std::cout << "required orders" << orders << std::endl;
    //std::cout << "incomputables.size() " << incomputables.size() << std::endl;
}










//simple fucntion to check the computability under nodal analysis

/**
Checks whether or not the node can be computed for InheritedAttribute evaluation

SgGraphTraversal::computable
Input:
@param[n] n node in question
@param[g] CFG* g, CFG calculated previously
@return bool, whether or not the node is calculable
*/


template <class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
bool
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
computable(const CFG* &g, const int &n)
{
    if (computedNodes.find(n) != computedNodes.end())
    {
        //std::cout << "already known value" << std::endl;
        return true;
    }
    //std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> ed = boost::in_edges(g->getVertex**ForNode(*n), *g);
    bool comp = true;
    //in_edge_iterator i, j;
    int took;
    //int* vt;
    //CFG* gra = *g;
    //int* took;
    //Edge* ted;
    std::vector<int> ieds = getInEdges(n, g);
    for (int i = 0; i < ieds.size(); i++)
    {
        took = getSource(ieds[i], g);
        //vt = tookv;
        //took = &(*gra[&boost::source(*i, *g)]);
        //ted = &(*gra[*i]);
        //std::pair<int*, int*> pairnodes;
        //*ted = *i;
        if (oVals.find(took) == oVals.end() && nullEdgesOrdered.find(ieds[i]) == nullEdgesOrdered.end())
        {
            comp = false;
        }
    }
    if (comp)
    {
        //std::cout << "computable" << std::endl;
    }
    else
    {
        //std::cout << "incomputable" << std::endl;
        //std::cout << "node is:" << std::endl;
        //printCFGNode(n, g, -2);
    }
    return comp;
}


//computes the inherited attribute values in nodal analysis

/**
computes the InheritedAttribute values on each of the nodes

SgGraphTraversal::computeInheritedOrdered
Input:
@param[n] n node in question
@param[g] CFG* g, CFG calculated previously
@return void

*/


template <class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
computeInheritedOrdered(const CFG* &g, const int &n)
{
    int runs = 0;
//    std::ofstream mf;
//    mf.open("analysis.dot");
//    mf << "digraph defaultName { \n";
    //std::cout << "iVals.size(): " << iVals.size() << std::endl;
    for (std::map<int, int>::iterator i = iVals.begin(); i != iVals.end(); i++)
    {
        runs++;
        ROSE_ASSERT(canEval(g, (*i).second));
        //setPathVal(g, n);
        //printNodePlusEdgesForAnalysis(g, (*i).second, loopNumMap[(*i).second], pathValMap[(*i).second], mf);
        evalNodeOrdered(g, (*i).second);
    }
}

/**
Checks whether or not the node can be evaluated (deprecated)

SgGraphTraversal::canEval
Input:
@param[n] n node in question
@param[g] CFG* g, CFG calculated previously
@return void

*/

//checks to see if evaluation is possible under nodal analysis
template <class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
bool
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
canEval(const CFG* & g, const int & n)
{
    bool evaled = true;
    //Edge* ted;
    if (inhVals.find(n) == inhVals.end())
    {
        //std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> ins = boost::in_edges(g->getVertex**ForNode(*n), *g);
        std::vector<int> ieds = getInEdges(n, g);
        //in_edge_iterator i, j;
        for (int i = 0; i < ieds.size(); i++)
        {
            //int* tookv = &boost::source(*i, *g);
            //int* vt = tookv;
            //CFG* gra = *g;
            int bs = getSource(ieds[i], g);
            if (inhVals.find(bs) == inhVals.end() && nullEdgesOrdered.find(ieds[i]) == nullEdgesOrdered.end())
            {
                evaled = false;
            }
        }
    }
    return evaled;
}



//actually does the evaluation
/**
evaluates the node directly, used within computeInheritedOrdered
SgGraphTraversal::evalNodeOrdered
Input:
@param[n] n node in question
@param[g] CFG* g, CFG calculated previously
@return void

*/

template <class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
evalNodeOrdered(const CFG* &g, const int &n)
{
    if (inhVals.find(n) != inhVals.end())
    {
        return;
    }
    InheritedAttributeType inhX;
    //std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> par = boost::in_edges(g->getVertex**ForNode(*n), *g);
    std::vector<int> ieds = getInEdges(n, g);
    std::vector<InheritedAttributeType> inh;
    for (int i = 0; i < ieds.size(); i++)
    {
        //int* tookv = &boost::source(*i, *g);
        //int* vt = tookv;
        int bs;
        bs = getSource(ieds[i], g);

        if (inhVals.find(bs) != inhVals.end())
        {
            inh.push_back(inhVals[bs]);
        }
        else
        {
            //std::cout << "no inhval found" << std::endl;
        }
    }

    if (n != st || inh.size() > 0)
    {
        Vertex v = intvertmap[n];
        inhX = evaluateInheritedAttribute(v, inh);
        inhVals[n] = inhX;
    }
    //std::cout << "num of inhVals: " << inh.size() << std::endl;

}


//debugging function, currently not useful for the end user
/*
template <class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
void
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
setPathVal(CFG* g, int* currn) {
       if (pathValMap.find(currn) != pathValMap.end()) {
           return;
       }
       //std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> ined = boost::in_edges(g->getVertex**ForNode(*currn), *g);
       int tmppathcount = 0;
       in_edge_iterator i, j;
       //cout << "inedges = " << j - i << std::endl;
       for (boost::tie(i, j) = boost::in_edges(g->getVertex**ForNode(*currn), *g); i != j; ++i) {
            //int* tookv = &boost::source(*i, *g);
            //int* vt = tookv;
            CFG* gra = *g;
            int* bs = &(*gra[&boost::source(*i, *g)]);

           ROSE_ASSERT(pathValMap.find(bs) != pathValMap.end() /*|| nullEdgesOrdered.find(*i) != nullEdgesOrdered.end());
            //if (nullEdgesOrdered.find(*i) != nullEdgesOrdered.end()) {
            //    pathValMap[(*i)->get_from()] = 0;
           // }
            int pv = pathValMap[bs];
            if (pv != 0) {
            tmppathcount += pv;
            }
        }
        pathValMap[currn] = tmppathcount;
        return;
    }
*/

//computes the next child to be analyzed in nodal analysis
/**
computes the next child node to evaluate, used in computing inheritedAttributes, picks the child
that is not already calculated

SgGraphTraversal::getNextChild
Input:
@param[n] n node in question
@param[g] CFG* g, CFG calculated previously
@return void

*/

template <class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
std::pair<bool, int>
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
getNextChild(const CFG* &g, const int &n)
{
    bool nullPoss = false;
    //std::cout << "nextChild" << std::endl;
    //std::pair<CFG::CFGEdgePtr, CFG::CFGEdgePtr> outs = boost::out_edges(g->getVertex**ForNode(*n), *g);
    //std::cout << "outs.size(): " << outs.size() << std::endl;
    //std::cout << "outs: " << outs.size() << std::endl;
    //int* nextNode;
    int nullNode = -1;
    int prepNextNode;
    //Edge* ted;

    bool completed = false;
    bool completeNull = false;
    //int* nN;
    //CFG* gra = *g;
    //out_edge_iterator i, j;
    //int* vN = g->getVertex**ForNode(*n);
    //boost::tie(i, j) = boost::out_edges(vN, *g);
    //printCFGNode(n, g, -1);
    //std::cout << "outedges" << j - i << std::endl;
    //std::pair<int*, int*> pairnodes;
    int nextNode;
    std::vector<int> oeds = getOutEdges(n, g);
    for (int i = 0; i < oeds.size(); i++)
    {
        prepNextNode = getTarget(oeds[i], g);
        if (oeds.size() == 1)
        {
            //nextNode = &(*gra[&boost::target(*i,*g)]);
            //std::pair<int*, int*> pairnodes;
            //pairnodes.first = n;
            //pairnodes.second = prepNextNode;
            //*ted = *i;
            if (nullEdgesOrdered.find(oeds[i]) != nullEdgesOrdered.end())
            {
                nullNum++;
            }
            nextNode = getTarget(oeds[i], g);
            //completedEdges.insert(*i);
            completed = true;
        }
        else if (completed == false && computedNodes.find(prepNextNode) == computedNodes.end())
        {
            completed = true;
            //nextNode = &(*gra[&boost::target(*i,*g)]);
            //std::pair<int*, int*> pairnodes;
            //*ted = *i;
            if (nullEdgesOrdered.find(oeds[i]) != nullEdgesOrdered.end())
            {
                nullNum++;
            }
            nextNode = getTarget(oeds[i], g);
            completedEdgesOut.insert(oeds[i]);
            completed = true;
        }


    }
    std::pair<bool, int> pr;
    ROSE_ASSERT (completed == true || completeNull == true);
    if (completed == true)
    {
        //std::cout << "completed" << std::endl;
        pr.first = completed;
        pr.second = nextNode;
        return pr;
    }
    else
    {
        //std::cout << "bad node" << std::endl;
        pr.first = true;
        pr.second = nullNode;
        return pr;
    }

}

/**
computes the next parent node to calculate
similar to getNextChild

SgGraphTraversal::getNextPar
Input:
@param[n] n node in question
@param[g] CFG* g, CFG calculated previously
@return void

*/
//computes the next parent to be analyzed in nodal analysis
template <class InheritedAttributeType, class SynthesizedAttributeType, class CFG>
std::pair<bool, int>
SgGraphTraversal<InheritedAttributeType, SynthesizedAttributeType, CFG>::
getNextPar(const CFG* &g, const int &n)
{

    int prepNextNode;
    int nextPar;
    //Edge* ted;
    bool completed = false;
    bool completeNull = false;
    std::vector<int> ieds = getInEdges(n, g);
    //std::cout << "inedges: " << ieds.size() << std::endl;
    //in_edge_iterator i, j;
    for (int i = 0; i < ieds.size(); i++)
    {
        prepNextNode = getSource(ieds[i], g);
        //*ted = *i;
        if (ieds.size() == 1 /*&& completedEdges.find(ieds[i]) == completedEdges.end()*/)
        {
            //std::pair<int*, int*> pairnodes;
            completed = true;
            completedEdges.insert(ieds[i]);
            nextPar = prepNextNode;
        }

        else if (completedEdges.find(ieds[i]) == completedEdges.end() && completed == false)
        {
            completed = true;
            completedEdges.insert(ieds[i]);
            nextPar = prepNextNode;
        }
        else if (completedEdges.find(ieds[i]) != completedEdges.end() && computedNodes.find(prepNextNode) == computedNodes.end() && completed == false)
        {
            completeNull = true;
            nextPar = n;
            nullEdgesOrdered.insert(ieds[i]);
            nullEdgesPaths++;

        }
    }
    ROSE_ASSERT(completed == true || completeNull == true);
    std::pair<bool, int> pr;
    pr.first = completed;
    pr.second = nextPar;
    if (completeNull == true && completed == false)
    {
        pr.first = completeNull;
        pr.second = nextPar;
    }

    return pr;
}















