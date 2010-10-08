/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/
#include "rose.h"
#include <string>

typedef bool SynthesizedAttribute;

using namespace std;

class InheritedAttribute
   {
     public:
      // Depth in AST
         
         
         
 	 //Naming paths code
	 set<vector<SgGraphNode*> > collected_paths;
         int pathcount;
         

       // Specific constructors are required
/*
          InheritedAttribute (void) : maxLinesOfOutput(100000) {paths++;};
          InheritedAttribute ( const InheritedAttribute & X ) : maxLinesOfOutput(100000){ paths++; };
*/

        
	InheritedAttribute (int p, set<vector<SgGraphNode*> > pa) : pathcount(p), collected_paths(pa) {};
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
          virtual InheritedAttribute evaluateInheritedAttribute(SgGraphNode* n, vector<InheritedAttribute> inheritedAttributeSet);
	  set<vector<SgGraphNode*> > collect_paths(SgGraphNode* n, set<vector<SgGraphNode*> > pathsSoFar);
          virtual SynthesizedAttribute evaluateSynthesizedAttribute(SgGraphNode* n, InheritedAttribute inh, SynthesizedAttributesList childAttributes);
          static boost::unordered_map<SgGraphNode*, InheritedAttribute, boost::hash<SgGraphNode*>, compareSgGraphNode2> graphnodeinheritedmap; 
          //virtual void displayEvaluation(InheritedAttribute inh);
          //virtual InheritedAttribute evaluateCoAttribute(InheritedAttribute i1, InheritedAttribute i2, SgGraphNode* g1, SgGraphNode* g2);

   };

boost::unordered_map<SgGraphNode*, InheritedAttribute, boost::hash<SgGraphNode*>, compareSgGraphNode2> visitorTraversal::graphnodeinheritedmap; 

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
InheritedAttribute visitorTraversal::evaluateInheritedAttribute(SgGraphNode* child, vector<InheritedAttribute> inheritedAttributesSet) {
    set<vector<SgGraphNode*> > nullPathList;
    InheritedAttribute nullInherit(0, nullPathList);
    int tmppathcount = 0;
    
    set<vector<SgGraphNode*> > newPathList;
    
    cout << "Number Of Inherited Attributes In Set:" << inheritedAttributesSet.size() << endl;
    if (inheritedAttributesSet.size() != 0) {
         
        cout << "calculating inheritedAttributes" << endl;
        for (vector<InheritedAttribute>::iterator i = inheritedAttributesSet.begin(); i != inheritedAttributesSet.end(); i++) {
            
            InheritedAttribute inh = *i;
            //if (inh.pathcount != 0) {
            cout << "adding " << inh.pathcount << " to pathcount" << endl;
            tmppathcount += inh.pathcount;
            set<vector<SgGraphNode*> > colpath = inh.collected_paths;
            for (set<vector<SgGraphNode*> >::iterator k = colpath.begin(); k != colpath.end(); k++) {
                cout << "enumerating subpaths" << endl;
                vector<SgGraphNode*> cp1 = *k;
                for (vector<SgGraphNode*>::iterator kt = cp1.begin(); kt != cp1.end(); kt++) {
                    cout << (*kt)->get_name() << " ";
                }
                cout << endl;
            }
                  
            for (set<vector<SgGraphNode*> >::iterator j = colpath.begin(); j != colpath.end(); j++) {
               vector<SgGraphNode*> curpath = *j;
               curpath.push_back(child);
               newPathList.insert(curpath);
            }
            //}
        }
        cout << "enumerating paths" << endl;
        set<string> namearr;
        set<set<string> > namearrs;
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
        set<vector<SgGraphNode*> > cp = collect_paths(child, newPathList);
        cout << "Number Of Paths:" << newPathList.size() << endl;
        cout << "This should match:" << tmppathcount << endl;
        ROSE_ASSERT(tmppathcount == newPathList.size());
        InheritedAttribute inh(tmppathcount, newPathList);
        graphnodeinheritedmap[child] = inh;
        return InheritedAttribute(tmppathcount, newPathList);
    }
    else {
    cout << "no inherited attributes given" << endl;
    graphnodeinheritedmap[child] = nullInherit;
    return nullInherit;
    }
}

SynthesizedAttribute
visitorTraversal::evaluateSynthesizedAttribute(SgGraphNode* child, InheritedAttribute inhatt, SynthesizedAttributesList synthlist) {
  //cout << "evaluatingSynthAtt" << endl;
  //SynthesizedAttribute re = false;
  return false;
}






int main() {
      SgIncidenceDirectedGraph* sgDG = new SgIncidenceDirectedGraph();

SgGraphNode* node1 = new SgGraphNode("node1");
SgGraphNode* node2 = new SgGraphNode("node2");
SgGraphNode* node3 = new SgGraphNode("node3");
SgGraphNode* node4 = new SgGraphNode("node4");
SgGraphNode* node5 = new SgGraphNode("node5");
SgGraphNode* node6 = new SgGraphNode("node6");
SgGraphNode* node7 = new SgGraphNode("node7");
SgGraphNode* node8 = new SgGraphNode("node8");
SgGraphNode* node9 = new SgGraphNode("node9");
SgGraphNode* node10 = new SgGraphNode("node10");
SgGraphNode* node11 = new SgGraphNode("node11");
SgGraphNode* node12 = new SgGraphNode("node12");
SgGraphNode* node13 = new SgGraphNode("node13");
SgGraphNode* node14 = new SgGraphNode("node14");
SgGraphNode* node15 = new SgGraphNode("node15");
SgGraphNode* node16 = new SgGraphNode("node16");
SgGraphNode* node17 = new SgGraphNode("node17");
SgGraphNode* node18 = new SgGraphNode("node18");
SgGraphNode* node19 = new SgGraphNode("node19");
SgGraphNode* node20 = new SgGraphNode("node20");
SgGraphNode* node21 = new SgGraphNode("node21");
SgGraphNode* node22 = new SgGraphNode("node22");
SgGraphNode* node23 = new SgGraphNode("node23");
SgGraphNode* node24 = new SgGraphNode("node24");

SgDirectedGraphEdge* t1s2 = sgDG->addDirectedEdge(node1, node2, "t1s2");
SgDirectedGraphEdge* t1s3 = sgDG->addDirectedEdge(node1, node3, "t1s3");
SgDirectedGraphEdge* t2s4 = sgDG->addDirectedEdge(node2, node4, "t2s4");
SgDirectedGraphEdge* t3s4 = sgDG->addDirectedEdge(node3, node4, "t3s4");
SgDirectedGraphEdge* t4s5 = sgDG->addDirectedEdge(node4, node5, "t4s5");

SgDirectedGraphEdge* t4s6 = sgDG->addDirectedEdge(node4, node6, "t4s6");

SgDirectedGraphEdge* t4s7 = sgDG->addDirectedEdge(node4, node7, "t4s7");

SgDirectedGraphEdge* t5s8 = sgDG->addDirectedEdge(node5, node8, "t5s8");
SgDirectedGraphEdge* t6s8 = sgDG->addDirectedEdge(node6, node8, "t6s8");
SgDirectedGraphEdge* t7s8 = sgDG->addDirectedEdge(node7, node8, "t7s8");

SgDirectedGraphEdge* t8s9 = sgDG->addDirectedEdge(node8, node9, "t8s9");
SgDirectedGraphEdge* t8s10 = sgDG->addDirectedEdge(node8, node10, "t8s10");
SgDirectedGraphEdge* t8s11 = sgDG->addDirectedEdge(node8, node11, "t8s11");
SgDirectedGraphEdge* t8s12 = sgDG->addDirectedEdge(node8, node12, "t8s12");

SgDirectedGraphEdge* t9s13 = sgDG->addDirectedEdge(node9, node13, "t9s13");
SgDirectedGraphEdge* t10s13 = sgDG->addDirectedEdge(node10, node13, "t10s13");
SgDirectedGraphEdge* t11s13 = sgDG->addDirectedEdge(node11, node13, "t11s13");
SgDirectedGraphEdge* t12s13 = sgDG->addDirectedEdge(node12, node13, "t12s13");

SgDirectedGraphEdge* t13s14 = sgDG->addDirectedEdge(node13, node14, "t13s14");
SgDirectedGraphEdge* t14s14 = sgDG->addDirectedEdge(node14, node14, "t14s14");
SgDirectedGraphEdge* t14s15 = sgDG->addDirectedEdge(node14, node15, "t14s15");
SgDirectedGraphEdge* t15s15 = sgDG->addDirectedEdge(node15, node15, "t15s15");
SgDirectedGraphEdge* t15s16 = sgDG->addDirectedEdge(node15, node16, "t15s16");
SgDirectedGraphEdge* t16s15 = sgDG->addDirectedEdge(node16, node15, "t16s15");
SgDirectedGraphEdge* t16s17 = sgDG->addDirectedEdge(node16, node17, "t16s17");

SgDirectedGraphEdge* t17s18 = sgDG->addDirectedEdge(node17, node18, "t17s18");
SgDirectedGraphEdge* t17s19 = sgDG->addDirectedEdge(node17, node19, "t17s19");
SgDirectedGraphEdge* t17s20 = sgDG->addDirectedEdge(node17, node20, "t17s20");

SgDirectedGraphEdge* t18s21 = sgDG->addDirectedEdge(node18, node21, "t18s21");
SgDirectedGraphEdge* t19s21 = sgDG->addDirectedEdge(node19, node21, "t19s21");
SgDirectedGraphEdge* t20s21 = sgDG->addDirectedEdge(node20, node21, "t20s21");

SgDirectedGraphEdge* t15s22 = sgDG->addDirectedEdge(node15, node22, "t15s22");
SgDirectedGraphEdge* t22s15 = sgDG->addDirectedEdge(node22, node15, "t22s15");
SgDirectedGraphEdge* t15s23 = sgDG->addDirectedEdge(node15, node23, "t15s23");
SgDirectedGraphEdge* t23s15 = sgDG->addDirectedEdge(node23, node15, "t23s15");
SgDirectedGraphEdge* t23s24 = sgDG->addDirectedEdge(node23, node24, "t23s24");
SgDirectedGraphEdge* t24s23 = sgDG->addDirectedEdge(node24, node23, "t24s23");
SgDirectedGraphEdge* t24s12 = sgDG->addDirectedEdge(node24, node12, "t24s12");






  set<vector<SgGraphNode*> > begincp;
  vector<SgGraphNode*> vec;
  SgIncidenceDirectedGraph* gTree = new SgIncidenceDirectedGraph();

    set<vector<SgGraphNode*> > newPathList2;
    InheritedAttribute nullInherit(0, newPathList2);
    visitorTraversal* vis = new visitorTraversal;
    vec.push_back(node1);
    begincp.insert(vec);
    InheritedAttribute inheritedAttribute(1, begincp);
    vis->traverse(node1, sgDG, gTree, inheritedAttribute, nullInherit, 0, false, node21);
    InheritedAttribute inh = vis->graphnodeinheritedmap[node21];
    cout << "pathcount = " << inh.pathcount << endl;
    ROSE_ASSERT(inh.pathcount == 72);
    cout << "complete" << endl;
    return 0;
}
