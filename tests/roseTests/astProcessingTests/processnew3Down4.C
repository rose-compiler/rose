#include <iostream>
#include <fstream>
#include <rose.h>
//#include "interproceduralCFG.h"
#include <string>
#include <err.h>
#include "graphProcessing.h"

/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/

#include <string>

typedef bool SynthesizedAttribute;

using namespace std;
using namespace boost;
/*
class Int128
{
public:
    
    Int128 operator+(const Int128 & rhs)
    {
        Int128 sum;
        sum.high = high + rhs.high;
        sum.low = low + rhs.low;
        // check for overflow of low 64 bits, add carry to high
        if (sum.low < low)
            ++sum.high;
        return sum;
    }
    Int128 operator-(const Int128 & rhs)
    {
        Int128 difference;
        difference.high = high - rhs.high;
        difference.low = low - rhs.low;
        // check for underflow of low 64 bits, subtract carry to high
        if (difference.low > low)
            --difference.high;
        return difference;
    }
    Int128 operator=(const Int128 & rhs)
    {
        Int128 equal;
        equal.high = rhs.high;
        equal.low = rhs.low;
        return equal;
    }
    bool operator==(const Int128 & rhs)
    {
        if (low == rhs.low && high == rhs.high) {
            return true;
        }
        else {
            return false;
        }
    }
    bool operator!=(const Int128 & rhs)
    {
        if (low != rhs.low || high != rhs.high) {
            return true;
        }
        else {
            return false;
        }
    }
    bool operator>=(const Int128 & rhs) {
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
    bool operator<=(const Int128 & rhs) {      
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
	 set<vector<SgGraphNode*> > collected_paths;
         double pathcount;
         

       // Specific constructors are required
/*
          InheritedAttribute (void) : maxLinesOfOutput(100000) {paths++;};
          InheritedAttribute ( const InheritedAttribute & X ) : maxLinesOfOutput(100000){ paths++; };
*/

        
	InheritedAttribute (double p, set<vector<SgGraphNode*> > cp) : pathcount(p), collected_paths(cp) {};
        InheritedAttribute () {};
	~InheritedAttribute () {};
   };

struct compareSgGraphNode2 {
    bool operator()(const SgGraphNode* a, const SgGraphNode* b) const
    {
        return a==b;
    }
};


class visitorTraversal : public SgGraphTraversal<InheritedAttribute, SynthesizedAttribute>
   {
     public:
       // virtual function must be defined
          int numnodes;
          int th;
          int ifstatements;
          int forstatements;
          int tltnodes;
          //SgGraphNode* endnodeX;
          StaticCFG::CFG* SgCFG;
          std::set<CFGNode> seenifs;
          std::set<CFGNode> seenfors;
          double pathcountMax;
          set<SgGraphNode*> seen;
          vector<SgGraphNode*> fornodes;
          virtual InheritedAttribute evaluateInheritedAttribute(SgGraphNode* n, vector<InheritedAttribute> inheritedAttributeSet);
	  set<vector<SgGraphNode*> > collect_paths(SgGraphNode* n, set<vector<SgGraphNode*> > pathsSoFar);
          virtual SynthesizedAttribute evaluateSynthesizedAttribute(SgGraphNode* n, InheritedAttribute inh, SynthesizedAttributesList childAttributes);
          map<SgGraphNode*, InheritedAttribute> graphnodeinheritedordmap2; 
          virtual void pathAnalyze(vector<SgGraphNode*> pth, bool loop, set<vector<SgGraphNode*> > incloops);
          std::map<SgGraphNode*, int> nodeInts;
          int currInt;
          int pths;
          int loops;
           
          set<vector<SgGraphNode*> > pthstore;
          //virtual void displayEvaluation(InheritedAttribute inh);
          //virtual InheritedAttribute evaluateCoAttribute(InheritedAttribute i1, InheritedAttribute i2, SgGraphNode* g1, SgGraphNode* g2);

   };










//boost::unordered_map<SgGraphNode*, InheritedAttribute, boost::hash<SgGraphNode*>, compareSgGraphNode2> visitorTraversal::graphnodeinheritedmap2; 

void visitorTraversal::pathAnalyze(vector<SgGraphNode*> pth, bool loop, set<vector<SgGraphNode*> > incloops) {
/*
    if (loop == true) {
    std::cout << "loop: " << loops << " size: " << pth.size() <<  std::endl;
    //if (pth.size() > 100) {
    //    std::cout << "bigloop" << std::endl;
    for (int i = 0; i < pth.size(); i++) {
         CFGNode childcfg = SgCFG->toCFGNode(pth[i]);
       
        std::cout << childcfg.toString() << " ";
        }
        tltnodes += pth.size();
    //}

        //cout << "node:" << nodeInts[pth[i]] << endl;
         std::cout << std::endl;
   
}
*/
    


    //std::cout << "pth.size(): " << pth.size() << std::endl;

    if (loop == false) {
/*          std::cout << "path: " << pths << " size: " << pth.size() <<  std::endl;
    //if (pth.size() > 100) {
    //    std::cout << "bigloop" << std::endl;
    for (int i = 0; i < pth.size(); i++) {
         CFGNode childcfg = SgCFG->toCFGNode(pth[i]);
       
        std::cout << childcfg.toString() << " ";
        }
*/
        tltnodes += pth.size();
    //}

        //cout << "node:" << nodeInts[pth[i]] << endl;
         //std::cout << std::endl;
        //cout << "pth: " << pths << " contains " << incloops.size() << " potential loops" << endl;
        //tltnodes += pth.size();
        pths++;
        //std::cout << pths << std::endl;

    }
    else {
        //std::cout << "loop" << std::endl;
        loops++;
        
    }
    //cout << "path " << pths << " complete!" << endl;
    
    //ROSE_ASSERT(pths.find(pth) == pths.end());
}
set<vector<SgGraphNode*> > visitorTraversal::collect_paths(SgGraphNode* n, set<vector<SgGraphNode*> > pathsSoFar) {
    set<vector<SgGraphNode*> > newPathsSoFar;
    for (set<vector<SgGraphNode*> >::iterator i = pathsSoFar.begin(); i != pathsSoFar.end(); i++) {
        vector<SgGraphNode*> add = *i;
        newPathsSoFar.insert(add);
    }
    return newPathsSoFar;
}

/*
InheritedAttribute visitorTraversal::evaluateCoAttribute(InheritedAttribute endnodeinh, InheritedAttribute iinh, SgGraphNode* endode, SgGraphNode* inode) {
    int tmp = iinh.pathcount; 
    tmp += endnodeinh.pathcount;
    set<vector<SgGraphNode*> > newpathset;
    for (set<vector<SgGraphNode*> >::iterator i = endnodeinh.collected_paths.begin(); i != endnodeinh.collected_paths.end(); i++) {
            vector<SgGraphNode*> newvec;
            newvec = *i;
            newvec.push_back(inode);
            newpathset.insert(newvec);
    }
    for (set<vector<SgGraphNode*> >::iterator j = iinh.collected_paths.begin(); j != iinh.collected_paths.end(); j++) {
        newpathset.insert(*j);
    } 
    InheritedAttribute inh2 = InheritedAttribute(tmp, newpathset);
    displayEvaluation(inh2);
    return InheritedAttribute(tmp, newpathset);
}

void visitorTraversal::displayEvaluation(InheritedAttribute inh) {
 cout << "enumerating paths" << endl;
        set<string> namearr;
        set<set<string> > namearrs;
        set<vector<SgGraphNode*> > newPathList = inh.collected_paths;
        for (set<vector<SgGraphNode*> >::iterator it = newPathList.begin(); it != newPathList.end(); it++) {
            vector<SgGraphNode*> cp = *it;
            for (vector<SgGraphNode*>::iterator jt = cp.begin(); jt != cp.end(); jt++) {
                cout << (*jt)->get_name() << " ";
               ROSE_ASSERT(namearr.find((*jt)->get_name()) == namearr.end());
                namearr.insert((*jt)->get_name());
            }
            ROSE_ASSERT(namearrs.find(namearr) == namearrs.end());
            namearrs.insert(namearr);
            namearr.clear();
            cout << endl;
        }
        cout << "pathcount: " << inh.pathcount << endl;
        cout << "should be the same: " << inh.collected_paths.size();
}
*/

string printnum(int i) {
    std::string s;
    std::stringstream out;
    out << i;
    s = out.str();
    return s;
}

//InheritedAttribute visitorTraversal::loopSolve(std::set<std::pair<SgGraphNode* begin, SgGraphNode* end> > loops) {
//    for (std::set<std::pair<SgGraphNode*, SgGraphNode*> >::iterator i = loops.begin(); i != loops.end(); i++) {
//        InheritedAttribute inh = traverse(cfgb, cfggraph, inheritedAttribute2, nullInherit2, cfgc, false, true)

InheritedAttribute visitorTraversal::evaluateInheritedAttribute(SgGraphNode* child, vector<InheritedAttribute> inheritedAttributesSet) {
    set<vector<SgGraphNode*> > nullPathList;
    InheritedAttribute nullInherit(0, nullPathList);
    double tmppathcount = 0;
    numnodes++;
    //std::cout << "executions: " << numnodes << std::endl;

    CFGNode childcfg = SgCFG->toCFGNode(child);
    //std::cout << "nodes: " << childcfg.toString() << std::endl;
    regex expression("SgIfStmt");
    regex expression2("SgForStatement");
    //cout << "node type: " << childcfg.toString() << endl;
    bool match2 = regex_search(childcfg.toString(), expression2);
    bool match = regex_search(childcfg.toString(), expression);
    //if (match) {
    //    cout << "found if statement" << endl;
    //    cout << childcfg.toString() << endl;
    //}

    if (match) {
    //cout << "found if statement" << endl;
    ifstatements += 1;
    //seenifs.insert(childcfg);
    }
    if (match2) {
    //cout << "found if statement" << endl;
    forstatements += 1;
    //seenfors.insert(childcfg);
    }


    //else {
    //cout << "I've seen this somewhere before..." << endl;
    //}
    //cout << "name of node: " << child->toString() << endl;
    //cout << "node name: " << child->id() << endl;
//    if (numnodes > 1000*th) {
//        cout << "numnodes = " << numnodes;
//        th += 1;
//    }
    
    set<vector<SgGraphNode*> > newPathList;
    
    //cout << "Number Of Inherited Attributes In Set:" << inheritedAttributesSet.size() << endl;
    if (inheritedAttributesSet.size() != 0) {
         
       // cout << "calculating inheritedAttributes" << endl;

        for (vector<InheritedAttribute>::iterator i = inheritedAttributesSet.begin(); i != inheritedAttributesSet.end(); i++) {
            
            InheritedAttribute inh = *i;
            
            if (inh.pathcount != 0) {
            //std::cout << "inh.pathcount: " << inh.pathcount << std::endl;
            //cout << "adding " << inh.pathcount << " to pathcount" << endl;
            tmppathcount += inh.pathcount;
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
            set<vector<SgGraphNode*> > colpath = inh.collected_paths;
            for (set<vector<SgGraphNode*> >::iterator k = colpath.begin(); k != colpath.end(); k++) {
            //    cout << "enumerating subpaths" << endl;
                vector<SgGraphNode*> cp1 = *k;
                for (vector<SgGraphNode*>::iterator kt = cp1.begin(); kt != cp1.end(); kt++) {
                   // cout << (*kt)->get_name() << " ";
                }
             //   cout << endl;
            }
                  
            for (set<vector<SgGraphNode*> >::iterator j = colpath.begin(); j != colpath.end(); j++) {
 //              vector<SgGraphNode*> curpath = *j;
 //              curpath.push_back(child);
 //              newPathList.insert(curpath);
            }
            //}
        }
        //cout << "enumerating paths" << endl;
        set<string> namearr;
        set<set<string> > namearrs;
        for (set<vector<SgGraphNode*> >::iterator it = newPathList.begin(); it != newPathList.end(); it++) {
            vector<SgGraphNode*> cp = *it;
            for (vector<SgGraphNode*>::iterator jt = cp.begin(); jt != cp.end(); jt++) {
       //         cout << (*jt)->get_name() << " ";
 //              ROSE_ASSERT(namearr.find((*jt)->get_name()) == namearr.end());
//                namearr.insert((*jt)->get_name());
            }
//            ROSE_ASSERT(namearrs.find(namearr) == namearrs.end());
//            namearrs.insert(namearr);
//            namearr.clear();
       //     cout << endl;
        }
     //   set<vector<SgGraphNode*> > cp = collect_paths(child, newPathList);
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

SynthesizedAttribute
visitorTraversal::evaluateSynthesizedAttribute(SgGraphNode* child, InheritedAttribute inhatt, SynthesizedAttributesList synthlist) {
  //cout << "evaluatingSynthAtt" << endl;
  //SynthesizedAttribute re = false;
  return false;
}

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

   StaticCFG::CFG cfg(mainDef);
   
 //cfg.buildFullCFG();
//cout << "build CFG" << endl;
  //Path* pc = new Path();
  //Path* pcR = new Path();
SgGraphNode* cfgb = new SgGraphNode();
    cfgb = cfg.getEntry();
    SgGraphNode* cfgc = new SgGraphNode();
    cfgc = cfg.getExit();
    SgIncidenceDirectedGraph* cfggraph = new SgIncidenceDirectedGraph();
    SgIncidenceDirectedGraph* cfggraph2 = new SgIncidenceDirectedGraph();
    std::set<std::vector<SgGraphNode*> > begincp;
    std::vector<SgGraphNode*> vec;
    vec.push_back(cfgb);
    set<vector<SgGraphNode*> > newPathList2;
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

  string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";

    // Dump out the full CFG, including bookkeeping nodes
   //cfg.buildFullCFG();
   //cfg.buildFilteredCFG();
   cfggraph = cfg.getGraph();
   cfggraph2 = cfg.getGraph();
   //   cfg.buildFilteredCFG();
      cfg.cfgToDot(mainDef, dotFileName1);

    // Dump out only those nodes which are "interesting" for analyses

//    cfg.cfgToDot(mainDef, dotFileName2);
    //std::set<CFGNode*> bl
    //vis->seenifs = bl;
    vis->loops = 0;
    vis->ifstatements = 0;
    vis->forstatements = 0;
    //vis->SgCFG = &cfg;
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
    //boost::unordered_map<int, std::set<SgGraphNode*> > depth;
    time_t start, end, start1, end1;
    vis->currInt = 0;
    vis2->currInt = 0;

    time(&start);
    //cout << "recursive" << endl;
    //InheritedAttribute inhcount = vis->traverse(cfgb, cfggraph, inheritedAttribute, nullInherit, cfgc, true, false);
    //cout << "recursive complete" << endl;
    time(&end);
    //cout << "recursive solve: " << inhcount.pathcount << endl;
    //cout << "pathevals: " << vis->numnodes << endl;
    time (&start1);
    //cout << "path" << endl;
    //vis2->endNodeX = cfgc;
    inheritedAttribute2.pathcount = 1.0;
    vis->tltnodes = 0;
    InheritedAttribute inhcountP = vis->traverse(cfgb, cfggraph, inheritedAttribute2, nullInherit2, cfgc, false, true);
    //cout << "path complete" << endl;
    //cout << "path solve: " << inhcountP.pathcount << endl;
    //cout << "maxpathCount: " << vis->pathcountMax;
    //cout << "pathevals: " << vis->numnodes << endl;
    //cout << "numifs: " << vis->ifstatements << endl;
    //cout << "numfors: " << vis->forstatements << endl;
    //cout << "total nodes in all paths combined with repetition: " << vis->tltnodes << std::endl;
    time(&end1);

    //cout << "path creation solution: " << vis->pths << std::endl;
    //cout << "path creation loop solution: " << vis->loops << std::endl;
    //cout << "noninterior loops path creation: " << vis->looppaths.size() << std::endl;
    //cout << "path solve loop solution: " << vis->nullEdgesPaths << std::endl;
    double diff = difftime(end, start);
    double diffP = difftime(end1, start1);
    std::set<SgGraphNode*> lps = vis->loopSet;
ROSE_ASSERT(inhcountP.pathcount == vis->pths);
ROSE_ASSERT(inhcountP.pathcount != 0);
    //cout << "loops: " << lps.size() << endl;
    int interiorloops = 0;

//       string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());

  //fstring dotFileNameA=fileName+"."+ mainDef->get_declaration()->get_name() +"Analysis.dot";
  //cfg.analysis = true;
  //cfg.cfgToDot(mainDef, dotFileNameA);
/*
    for (std::set<std::pair<SgGraphNode*, SgGraphNode*> >::iterator i = lps.begin(); i != lps.end(); i++) {
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