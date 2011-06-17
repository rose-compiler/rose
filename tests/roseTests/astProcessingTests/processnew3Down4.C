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


using namespace Backstroke;

struct CFGNodeFilter2
{
	// This function will be moved to a source file.
	bool operator()(const VirtualCFG::CFGNode& cfgNode) const
	{
/*
        

		if (!cfgNode.isInteresting())
			return false;

		SgNode* node = cfgNode.getNode();

		if (isSgValueExp(node))
			return false;
		//if (isSgExpression(node) && isSgExprStatement(node->get_parent()))
		if (isSgExprStatement(node))
			return false;
		if (isSgScopeStatement(node) && !isSgFunctionDefinition(node))
			return false;
		if (isSgCommaOpExp(node->get_parent()) && !isSgCommaOpExp(node))
			return true;

		switch (node->variantT())
		{
			case V_SgVarRefExp:
			case V_SgInitializedName:
			case V_SgFunctionParameterList:
			case V_SgAssignInitializer:
			case V_SgFunctionRefExp:
			case V_SgPntrArrRefExp:
			case V_SgExprListExp:
			case V_SgCastExp:
			case V_SgForInitStatement:
			case V_SgCommaOpExp:
				return false;
			default:
				break;
		}
*/
		return true;
	}
};



typedef CFG<CFGNodeFilter2> CFGforT;

/*
class Int128
{
public:
    
    Int128 operator+( Int128 & rhs)
    {
        Int128 sum;
        sum.high = high + rhs.high;
        sum.low = low + rhs.low;
        // check for overflow of low 64 bits, add carry to high
        if (sum.low < low)
            ++sum.high;
        return sum;
    }
    Int128 operator-( Int128 & rhs)
    {
        Int128 difference;
        difference.high = high - rhs.high;
        difference.low = low - rhs.low;
        // check for underflow of low 64 bits, subtract carry to high
        if (difference.low > low)
            --difference.high;
        return difference;
    }
    Int128 operator=( Int128 & rhs)
    {
        Int128 equal;
        equal.high = rhs.high;
        equal.low = rhs.low;
        return equal;
    }
    bool operator==( Int128 & rhs)
    {
        if (low == rhs.low && high == rhs.high) {
            return true;
        }
        else {
            return false;
        }
    }
    bool operator!=( Int128 & rhs)
    {
        if (low != rhs.low || high != rhs.high) {
            return true;
        }
        else {
            return false;
        }
    }
    bool operator>=( Int128 & rhs) {
        if (high >= rhs.high) {
            return true;
        }
        else if (high < rhs.high) {
            return false;
        }
        else {
            if (low >= rhs.low) {
                return true;
            }
            else {
                return false;
            }
        }
    }
    bool operator<=( Int128 & rhs) {      
        if (high <= rhs.high) {
            return true;
        }
        else if (high > rhs.high) {
            return false;
        }
        else {
            if (low <= rhs.low) {
                return true;
            }
            else {
                return false;
            }
        }
    }
    signed long long  high;
    unsigned long long low;
private:

};
*/

class InheritedAttribute
   {
     public:
      // Depth in AST
         
         
         
 	 //Naming paths code
	 set<vector<CFGforT::Vertex* > > collected_paths;
         double pathcount;
         

       // Specific ructors are required
/*
          InheritedAttribute (void) : maxLinesOfOutput(100000) {paths++;};
          InheritedAttribute (  InheritedAttribute & X ) : maxLinesOfOutput(100000){ paths++; };
*/

        
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









/*
//boost::unordered_map<FilteredCFGNode<CFGNodeFilter2>*, InheritedAttribute, boost::hash<FilteredCFGNode<CFGNodeFilter2>*>, compareSgGraphNode2> visitorTraversal::graphnodeinheritedmap2; 
int visitorTraversal::subPathAnalyze(vector<FilteredCFGNode<CFGNodeFilter2>*> pth, InheritedAttribute defaultAtt) {
    set<vector<FilteredCFGNode<CFGNodeFilter2>*> > empty;
    subnum++;
    return subnum;
}
*/
/*
InheritedAttribute visitorTraversal::subPathAnalyze(std::vector<FilteredCFGNode<CFGNodeFilter2>*> sbpt) {
    std::set<std::vector<FilteredCFGNode<CFGNodeFilter2>*> > lpsNull;
    pathAnalyze(sbpt, false, lpsNull);
}


void visitorTraversal::collectSubPaths(std::pair<InheritedAttribute, std::vector<FilteredCFGNode<CFGNodeFilter2>*> > subs, std::set<std::vector<FilteredCFGNode<CFGNodeFilter2>*> > incloops) {
    pts++;
    sublps += incloops.size();
    return;
}
*/
void visitorTraversal::quickPathAnalyze(vector<int>& pth) {
    pths++;
    //return;
}


void visitorTraversal::pathAnalyze( vector<int>& pth, bool loop,  set<vector<int> >& incloops) {

    if (loop == true) {
        
        tltnodes += pth.size();
        loops++;
    }

   else {
        tltnodes += pth.size();
        pths++;

    }
}

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
            //std::cout << "inh.pathcount: " << inh.pathcount << std::endl;
            //cout << "adding " << inh.pathcount << " to pathcount" << endl;
            tmppathcount += (*i).pathcount;
            if (pathcountMax < tmppathcount) {
                if (pathcountMax > 1) {
                    //cout << "new max paths: " << pathcountMax << endl;
                }
                pathcountMax = tmppathcount;
            }
            }
            }
            InheritedAttribute inhx = InheritedAttribute(tmppathcount, nullPathList);
            return inhx;
    }
    else {
            //std::cout << "no Attributes" << std::endl;
            //graphnodeinheritedordmap2[child] = InheritedAttribute(tmppathcount, nullPathList);

            InheritedAttribute inhx = InheritedAttribute(tmppathcount, nullPathList);
            return inhx;
    }
            //}
     //else {
/*            
            set<vector<FilteredCFGNode<CFGNodeFilter2>*> > colpath = inh.collected_paths;
            for (set<vector<FilteredCFGNode<CFGNodeFilter2>*> >::iterator k = colpath.begin(); k != colpath.end(); k++) {
            //    cout << "enumerating subpaths" << endl;
                vector<FilteredCFGNode<CFGNodeFilter2>*> cp1 = *k;
                for (vector<FilteredCFGNode<CFGNodeFilter2>*>::iterator kt = cp1.begin(); kt != cp1.end(); kt++) {
                   // cout << (*kt)->get_name() << " ";
                }
             //   cout << endl;
            }
                  
            for (set<vector<FilteredCFGNode<CFGNodeFilter2>*> >::iterator j = colpath.begin(); j != colpath.end(); j++) {
 //              vector<FilteredCFGNode<CFGNodeFilter2>*> curpath = *j;
 //              curpath.push_back(child);
 //              newPathList.insert(curpath);
            }
            //}
        }
        //cout << "enumerating paths" << endl;
        set<string> namearr;
        set<set<string> > namearrs;
        for (set<vector<FilteredCFGNode<CFGNodeFilter2>*> >::iterator it = newPathList.begin(); it != newPathList.end(); it++) {
            vector<FilteredCFGNode<CFGNodeFilter2>*> cp = *it;
            for (vector<FilteredCFGNode<CFGNodeFilter2>*>::iterator jt = cp.begin(); jt != cp.end(); jt++) {
       //         cout << (*jt)->get_name() << " ";
 //              ROSE_ASSERT(namearr.find((*jt)->get_name()) == namearr.end());
//                namearr.insert((*jt)->get_name());
            }
//            ROSE_ASSERT(namearrs.find(namearr) == namearrs.end());
//            namearrs.insert(namearr);
//            namearr.clear();
       //     cout << endl;
        }
     //   set<vector<FilteredCFGNode<CFGNodeFilter2>*> > cp = collect_paths(child, newPathList);
     //   cout << "Number Of Paths:" << newPathList.size() << endl;
     //   cout << "This should match:" << tmppathcount << endl;
       //    cout << "Number Of Paths:" << tmppathcount << endl;
//        ROSE_ASSERT(tmppathcount == newPathList.size());
        InheritedAttribute inh(tmppathcount, newPathList);
        graphnodeinheritedmap[child] = inh;
        return InheritedAttribute(tmppathcount, newPathList);
    }
    else {
 //   cout << "no inherited attributes given" << endl;
    graphnodeinheritedmap[child] = nullInherit;
*/
//    return nullInherit;
    //}
}

/*
SynthesizedAttribute
visitorTraversal::evaluateSynthesizedAttribute(FilteredCFGNode<CFGNodeFilter2>* child, InheritedAttribute inhatt, SynthesizedAttributesList synthlist) {
  //cout << "evaluatingSynthAtt" << endl;
  //SynthesizedAttribute re = false;
  return false;
}
*/
int main(int argc, char *argv[]) {
    double totalpaths;
    totalpaths = 0;
    
  //vector<string> arraystr;
  // Build the AST used by ROSE
  
//std::cout << "argc: " << argc << std::endl;
//std::cout << "argv: " << argv << std::endl;
int one = 1;
int zero = 0;

  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL); 

  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);

  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();

   //CFGforT*  cfg;
   //cfg.build(mainDef);
   
 //cfg.buildFullCFG();
//cout << "build CFG" << endl;
  //Path* pc = new Path();
  //Path* pcR = new Path();
//    ROSE_ASSERT(cfgc != NULL);
 //   ROSE_ASSERT(cfgb != NULL);
    
/*\\\\\\\\\\\
        CFGforT::Vertex*  nod = *cfgNode;
        CFGforT*  gr = *cfg;
        FilteredCFGNode<CFGNodeFilter2>* cf = &(*gr[nod]);
*/
    CFGforT* cfggraph = new CFGforT;
    //CFGforT*  cfggraph = new CFGforT;
    cfggraph->build(mainDef);
    //ROSE_ASSERT(cfggraph != NULL);
    //CFGforT::Vertex*  cfgb;
    //CFGforT*  gr = *cfggraph;
    CFGforT::Vertex* cfgb/* = new CFGforT::Vertex*/;
    cfgb = &cfggraph->entry_;
    //ROSE_ASSERT(cfgbV != NULL);
    
    CFGforT::Vertex*  cfgc/* = new CFGforT::Vertex*/;
    cfgc = &cfggraph->exit_;
    //FilteredCFGNode<CFGNodeFilter2>* cfgb = &(*gr[cfgbV]);
    //FilteredCFGNode<CFGNodeFilter2>* cfgc = &(*gr[cfgcV]);

    //ROSE_ASSERT(cfgc != NULL);

    //SgIncidenceDirectedGraph* cfggraph2 = new SgIncidenceDirectedGraph();
    std::set<std::vector<CFGforT::Vertex* > > begincp;
    std::vector<CFGforT::Vertex* > vec;
    vec.push_back(cfgb);
    set<vector<CFGforT::Vertex* > > newPathList2;
    InheritedAttribute nullInherit(0, newPathList2);
    
    begincp.insert(vec);
    InheritedAttribute inheritedAttribute(1.0, begincp);
    InheritedAttribute inheritedAttribute2(1.0, begincp);
    InheritedAttribute nullInherit2(0.0, newPathList2);
    
    
    //cout << "got graph" << endl;
    visitorTraversal* vis = new visitorTraversal;
    visitorTraversal* vis2 = new visitorTraversal;
    visitorTraversal* visLoop = new visitorTraversal;
    //vis->accesses = zero;
    //cout << "Directed" << endl;
    //set<SgDirectedGraphEdge*> edgeout = cfggraph->computeEdgeSetOut(cfgb);
    //set<SgDirectedGraphEdge*> edgein = cfggraph->computeEdgeSetIn(cfgb);
    //cout << "size of edgeout " << edgeout.size() << endl;
    //cout << "size of edgein " << edgein.size() << endl;
    string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    cfggraph->toDot("dotcheck.dot");
  string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";

    // Dump out the full CFG, including bookkeeping nodes
   //cfg.buildFullCFG();
   //cfg.buildFilteredCFG();
   //cfggraph = cfg.getGraph();
   //cfggraph2 = cfg.getGraph();
/*
   std::set<SgDirectedGraphEdge*> ied = cfggraph->computeEdgeSetIn(cfgc);
       ROSE_ASSERT(ied.size() >= 1);
   std::set<SgDirectedGraphEdge*> oed = cfggraph->computeEdgeSetOut(cfgc);
       ROSE_ASSERT(oed.size() == 0);
*/
   
   //   cfg.buildFilteredCFG();
      //cfg.cfgToDot(mainDef, dotFileName1);

    // Dump out only those nodes which are "interesting" for analyses

//    cfg.cfgToDot(mainDef, dotFileName2);
    //std::set<CFGNode*> bl
    //vis->seenifs = bl;
    vis->loops = 0;
    vis->incloopsnum = 0;
    vis->loopnodes = 0;
    vis->ifstatements = 0;
    vis->forstatements = 0;
    vis->maxPthSize = 0;
    //vis->SgCFG = &cfg;
    //vis->tltsubpaths = 0;
    //cout << "made it to traversed!" << endl;
    //cout << "looking for ifs" << endl;
    vis->numnodes = 0;
    vis->th = 1;
    vis->pathcountMax = 0.0;
    vis->pths = 0;
    vis2->ifstatements = 0;
    vis2->forstatements = 0;
    vis2->pths = 0;
    
    //vis->SgCFG = &cfg;
    //cout << "made it to traversed!" << endl;
    //cout << "looking for ifs" << endl;
    vis2->numnodes = 0;
    vis2->th = 1;
    vis2->pathcountMax = 0.0;
  
    //CFGNode cfgnode = cfg.toCFGNode(cfgb);
    //boost::unordered_map<int, std::set<FilteredCFGNode<CFGNodeFilter2>*> > depth;
    time_t start, end, start1, end1;
    vis->currInt = 0;
    vis2->currInt = 0;

    //time(&start);
    //cout << "recursive" << endl;
    //InheritedAttribute inhcount = vis->traverse(cfgb, cfggraph, inheritedAttribute, nullInherit, cfgc, true, false);
    //cout << "recursive complete" << endl;
    //time(&end);
    //cout << "recursive solve: " << inhcount.pathcount << endl;
    //cout << "pathevals: " << vis->numnodes << endl;
    time (&start1);
    cout << "path" << endl;
    //vis2->endNodeX = cfgc;
    inheritedAttribute2.pathcount = 1.0;
    vis->tltnodes = 0;
    InheritedAttribute inhcountP = vis->traverse(*cfgb, cfggraph, inheritedAttribute2, nullInherit2, *cfgc, false, true);
    //cout << "path complete" << endl;
    cout << "path solve: " << inhcountP.pathcount << endl;
    cout << "maxpathCount: " << vis->pathcountMax;
    //cout << "pathevals: " << vis->numnodes << endl;
    //cout << "numifs: " << vis->ifstatements << endl;
    //cout << "numfors: " << vis->forstatements << endl;
    cout << "total nodes in all paths combined with repetition: " << vis->tltnodes << std::endl;
    cout << "longest path size: " << vis->maxPthSize << std::endl;
    time(&end1);

    cout << "path creation solution: " << vis->pths << std::endl;
    cout << "path creation loop solution: " << vis->loops << std::endl;
    cout << "path creation incloop solution: " << vis->incloopsnum << std::endl;
    //cout << "noninterior loops path creation: " << vis->looppaths.size() << std::endl;
    //cout << "path solve loop solution: " << vis->nullEdgesPaths << std::endl;
    double diff = difftime(end, start);
    double diffP = difftime(end1, start1);
    //std::set<FilteredCFGNode<CFGNodeFilter2>*> lps = vis->loopSet;
//ROSE_ASSERT(inhcountP.pathcount == vis->pths);
ROSE_ASSERT(inhcountP.pathcount != 0);
    //cout << "loops: " << lps.size() << endl;
    int interiorloops = 0;

//       string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());

  //fstring dotFileNameA=fileName+"."+ mainDef->get_declaration()->get_name() +"Analysis.dot";
  //cfg.analysis = true;
  //cfg.cfgToDot(mainDef, dotFileNameA);
/*
    for (std::set<std::pair<FilteredCFGNode<CFGNodeFilter2>*, FilteredCFGNode<CFGNodeFilter2>*> >::iterator i = lps.begin(); i != lps.end(); i++) {
        visitorTraversal* visLoop = new visitorTraversal;
        time_t start1L, end1L;
        visLoop->ifstatements = 0;
        visLoop->forstatements = 0;
        //vis->SgCFG = &cfg;
        //cout << "made it to traversed!" << endl;
        //cout << "looking for ifs" << endl;
        visLoop->numnodes = 0;
        visLoop->th = 1;
        visLoop->pathcountMax = 0.0;
        visLoop->pths = 0;
        time_t start, end, start1, end1;
        visLoop->currInt = 0;
   InheritedAttribute inheritedAttribute3(1.0, begincp);
    InheritedAttribute nullInherit3(0.0, newPathList2);
        InheritedAttribute inhL = visLoop->traverse((*i).first, cfggraph, inheritedAttribute3, nullInherit3, (*i).second, false, true);
        if (inhL.pathcount > 1) {
            std::cout << "inhL extra loops: " << inhL.pathcount << std::endl;
            interiorloops++;
        }
            //time (&start1L);
    //cout << "path" << endl;
    //vis2->endNodeX = cfgc;
    //cout << "path complete" << endl;
    //cout << "path solve: " << inhL.pathcount << endl;
    //cout << "pathevals: " << visLoop->numnodes << endl;
    //cout << "maxCount: " << visLoop->pathcountMax << std::endl;
    //cout << "numifs: " << visLoop->ifstatements << endl;
    //cout << "numfors: " << visLoop->forstatements << endl;
    //time(&end1L);

    }
    int tl = interiorloops + lps.size();
    std::cout << "total loops: " << tl << std::endl;
 

*/  
    return 0;
}