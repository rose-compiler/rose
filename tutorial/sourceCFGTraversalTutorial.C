#include <iostream>
#include <fstream>
#include <rose.h>
//#include "interproceduralCFG.h"
#include <string>
#include <err.h>
#include "graphProcessing.h"
#include "graphTemplate.h"

/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/

typedef bool SynthesizedAttribute;

using namespace std;
using namespace boost;

//this particular CFG is defined in the Backstroke project
using namespace Backstroke;

//a filter for the definition of the CFG
struct CFGNodeFilter2
{
	bool operator()(const VirtualCFG::CFGNode& cfgNode) const
	{
		return true;
	}
};


//typedef-ed CFG for use in this traversal
typedef CFG<CFGNodeFilter2> CFGforT;

//InheritedAttribute Type utilized in evaluateInheritedAttribute

class InheritedAttribute
   {
     public:
	 set<vector<CFGforT::Vertex* > > collected_paths;
         double pathcount;
	InheritedAttribute (double p, set<vector<CFGforT::Vertex* > > cp) : pathcount(p), collected_paths(cp) {};
        InheritedAttribute () {};
	~InheritedAttribute () {};
   };


struct compareSgGraphNode2 {
    bool operator()( FilteredCFGNode<CFGNodeFilter2>* a,  FilteredCFGNode<CFGNodeFilter2>* b) 
    {
        return a==b;
    }
};


class visitorTraversal : public SgGraphTraversal<InheritedAttribute, SynthesizedAttribute, CFGforT>
   {
     public:
       // virtual function must be defined
          //visitorTraversal();
//          virtual ~visitorTraversal();
          int numnodes;
          int th;
          int ifstatements;
          int forstatements;
          int tltnodes;
          int maxPthSize;
          //FilteredCFGNode<CFGNodeFilter2>* endnodeX;
          //StaticCFG::CFG* SgCFG;
          std::set<CFGNode> seenifs;
          std::set<CFGNode> seenfors;
          double pathcountMax;
          set<CFGforT::Vertex* > seen;
          vector<FilteredCFGNode<CFGNodeFilter2>*> fornodes;
          virtual InheritedAttribute evaluateInheritedAttribute(CFGforT::Vertex  &n, vector<InheritedAttribute> &inheritedAttributeSet);
	  //set<vector<CFGforT::Vertex* > > collect_paths(CFGforT::Vertex  n, set<vector<CFGforT::Vertex > > pathsSoFar);
          //virtual SynthesizedAttribute evaluateSynthesizedAttribute(FilteredCFGNode<CFGNodeFilter2>* n, InheritedAttribute inh, SynthesizedAttributesList childAttributes);
          //map<FilteredCFGNode<CFGNodeFilter2>*, InheritedAttribute> graphnodeinheritedordmap2; 
          virtual void pathAnalyze( vector<int>& pth, bool loop,  set<vector<int> >& incloops);
          virtual void quickPathAnalyze(vector<int>& pth);
          //virtual InheritedAttribute subPathAnalyze(std::vector<FilteredCFGNode<CFGNodeFilter2>*>);
          //virtual void collectSubPaths(std::pair<InheritedAttribute, std::vector<FilteredCFGNode<CFGNodeFilter2>*> > subs, std::set<std::vector<FilteredCFGNode<CFGNodeFilter2>*> > incloops);
          //virtual int subPathAnalyze(vector<FilteredCFGNode<CFGNodeFilter2>*> pth);
          std::map<FilteredCFGNode<CFGNodeFilter2>*, int> nodeInts;
          int currInt;
          int tltsubpaths;
          int pths;
          int pts;
          int sublps;
          int loops;
          int incloopsnum;
          int loopnodes;
          int subnum;
          int numbots;
          set<vector<FilteredCFGNode<CFGNodeFilter2>*> > pthstore;
          //virtual void displayEvaluation(InheritedAttribute inh);
          //virtual InheritedAttribute evaluateCoAttribute(InheritedAttribute i1, InheritedAttribute i2, FilteredCFGNode<CFGNodeFilter2>* g1, FilteredCFGNode<CFGNodeFilter2>* g2);

   };

//currently not in use, to be used for possible subpath analysis
void visitorTraversal::quickPathAnalyze(vector<int>& pth) {
    pths++;
}

//the user defined function to be run on every path returned by the traversal algorithm
//The boolean "loop" gives whether or not the current path is a loop. In order
//to avoid many many paths, the loops are analyzed separated. incloops is deprecated

void visitorTraversal::pathAnalyze( vector<int>& pth, bool loop,  set<vector<int> >& incloops) {

// counts the number of loops and adds the nodes into the tltnodes variable
    if (loop == true) {
        
        tltnodes += pth.size();
        loops++;
    }
//counts the number of paths
   else {
        tltnodes += pth.size();
        pths++;

    }
}


//The basic InheritedAttribute evaluation method
//InheritedAttributes are explained further in the documentation, but to quickly summarize
//The InheritedAttribute is determined via the InheritedAttributes of its "parents" (nodes connected via in-edges) that are not in loops.
//One must use this method with regards to loops because otherwise you have the pathological case of a node that is it's own ancestor
//Consider

// A -->  B
//   <--

//B is an out node of A and A is an outnode of B, so by the definition of InheritedAttribute these are interconnected, thus making a definition
//of parent node quite difficult.

//this particular implementation calculates the number of paths
InheritedAttribute visitorTraversal::evaluateInheritedAttribute(CFGforT::Vertex  &child, vector<InheritedAttribute> &inheritedAttributesSet) {
    set<vector<CFGforT::Vertex* > > nullPathList;
    InheritedAttribute nullInherit(0, nullPathList);
    double tmppathcount = 0;
    numnodes++;
    
    set<vector<CFGforT::Vertex* > > newPathList;
    
    if (inheritedAttributesSet.size() != 0) {

        for (vector<InheritedAttribute>::iterator i = inheritedAttributesSet.begin(); i != inheritedAttributesSet.end(); i++) {
            
            InheritedAttribute inh = *i;
            
            if ((*i).pathcount != 0) {
            tmppathcount += (*i).pathcount;
            if (pathcountMax < tmppathcount) {
                if (pathcountMax > 1) {
                }
                pathcountMax = tmppathcount;
            }
            }
            }
            InheritedAttribute inhx = InheritedAttribute(tmppathcount, nullPathList);
            return inhx;
    }
    else {
            InheritedAttribute inhx = InheritedAttribute(tmppathcount, nullPathList);
            return inhx;
    }
}

int main(int argc, char *argv[]) {
    double totalpaths;
    totalpaths = 0;
    

  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL); 

  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);

  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();

//data structure for the graph to be traversed, typedef-ed above
    CFGforT* cfggraph = new CFGforT;
//builds the graph for analysis, based on the function definition given above
    cfggraph->build(mainDef);
//used in the traversal method for a node to start with
    CFGforT::Vertex* cfgb;
    cfgb = &cfggraph->entry_;
//the end node    
    CFGforT::Vertex*  cfgc;
    cfgc = &cfggraph->exit_;

//one could construct all the paths with this using a slightly altered evaluation method, hold over from an earlier version
    std::set<std::vector<CFGforT::Vertex* > > begincp;
    std::vector<CFGforT::Vertex* > vec;
    vec.push_back(cfgb);
    set<vector<CFGforT::Vertex* > > newPathList2;
//this is the attribute assigned to any nodes which are their own ancestors
    InheritedAttribute nullInherit(0, newPathList2);
    
    begincp.insert(vec);
//initial value
    InheritedAttribute inheritedAttribute(1.0, begincp);
    
    visitorTraversal* vis = new visitorTraversal;

//this retrieves a dot file later utilizable for analyzing the graph visually via something like zgrviewer
    string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    cfggraph->toDot("dotcheck.dot");
  string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";

//some variables used in the various used defined functions
    vis->loops = 0;
    vis->incloopsnum = 0;
    vis->loopnodes = 0;
    vis->ifstatements = 0;
    vis->forstatements = 0;
    vis->maxPthSize = 0;
    vis->numnodes = 0;
    vis->th = 1;
    vis->pathcountMax = 0.0;
    vis->pths = 0;
    vis->tltnodes = 0;
    vis->currInt = 0;
    

    time_t start, end, start1, end1;

    time (&start1);
    //returns the InheritedAttribute of the endnode
    InheritedAttribute inhcountP = vis->traverse(*cfgb, cfggraph, inheritedAttribute, nullInherit, *cfgc, false, true);
    //the inheritedAttribute deteremined number of paths, should agree with the path creation solution, various other results obtained via user defined path and inheritedAttribute analysis
    cout << "path solve: " << inhcountP.pathcount << endl;
    cout << "maxpathCount: " << vis->pathcountMax;
    cout << "total nodes in all paths combined with repetition: " << vis->tltnodes << std::endl;
    cout << "longest path size: " << vis->maxPthSize << std::endl;
    time(&end1);

    cout << "path creation solution: " << vis->pths << std::endl;
    cout << "path creation loop solution: " << vis->loops << std::endl;
    double diff = difftime(end, start);
    double diffP = difftime(end1, start1);
ROSE_ASSERT(inhcountP.pathcount != 0);

    return 0;
}