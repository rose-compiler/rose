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

static boost::unordered_map<SgGraphNode*, InheritedAttribute, boost::hash<SgGraphNode*>, compareSgGraphNode2> graphnodeinheritedmap; 

class visitorTraversal : public SgGraphTraversal<InheritedAttribute, SynthesizedAttribute>
   {
     public:
       // virtual function must be defined
          virtual InheritedAttribute evaluateInheritedAttribute(SgGraphNode* n, vector<InheritedAttribute> inheritedAttributeSet);
	  set<vector<SgGraphNode*> > collect_paths(SgGraphNode* n, set<vector<SgGraphNode*> > pathsSoFar);
          virtual SynthesizedAttribute evaluateSynthesizedAttribute(SgGraphNode* n, InheritedAttribute inh, SynthesizedAttributesList childAttributes);
          virtual void displayEvaluation(InheritedAttribute inh);
          virtual InheritedAttribute evaluateCoAttribute(InheritedAttribute i1, InheritedAttribute i2, SgGraphNode* g1, SgGraphNode* g2);
          static boost::unordered_map<SgGraphNode*, InheritedAttribute, boost::hash<SgGraphNode*>, compareSgGraphNode2> graphnodeinheritedmap; 

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
}

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
            if (inh.pathcount != 0) {
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
            }
        }
        cout << "enumerating paths" << endl;
        vector<string> namearr;
        set<vector<string> > namearrs;
        for (set<vector<SgGraphNode*> >::iterator it = newPathList.begin(); it != newPathList.end(); it++) {
            vector<SgGraphNode*> cp = *it;
            for (vector<SgGraphNode*>::iterator jt = cp.begin(); jt != cp.end(); jt++) {
                cout << (*jt)->get_name() << " ";
               ROSE_ASSERT(find(namearr.begin(), namearr.end(), (*jt)->get_name()) == namearr.end());
                namearr.push_back((*jt)->get_name());
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
        graphnodeinheritedmap[child] = InheritedAttribute(tmppathcount, newPathList);
        return InheritedAttribute(tmppathcount, newPathList);
    }
    else {
    //cout << "no inherited attributes given" << endl;
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
     set<vector<SgGraphNode*> > begincp;
     vector<SgGraphNode*> vec;
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
SgDirectedGraphEdge* t2s3 = sgDG->addDirectedEdge(node2, node3, "t2s3");
SgDirectedGraphEdge* t3s4 = sgDG->addDirectedEdge(node3, node4, "t3s4");
SgDirectedGraphEdge* t4s2 = sgDG->addDirectedEdge(node4, node2, "t4s2");
SgDirectedGraphEdge* t2s5 = sgDG->addDirectedEdge(node2, node5, "t2s5");

SgDirectedGraphEdge* t6s7 = sgDG->addDirectedEdge(node6, node7, "t6s7");
SgDirectedGraphEdge* t7s8 = sgDG->addDirectedEdge(node7, node8, "t7s8");
SgDirectedGraphEdge* t8s7 = sgDG->addDirectedEdge(node8, node7, "t8s7");
SgDirectedGraphEdge* t8s9 = sgDG->addDirectedEdge(node8, node9, "t8s9");

SgDirectedGraphEdge* t10s11 = sgDG->addDirectedEdge(node10, node11, "t10s11");
SgDirectedGraphEdge* t11s12 = sgDG->addDirectedEdge(node11, node12, "t11s12");
SgDirectedGraphEdge* t10s13 = sgDG->addDirectedEdge(node10, node13, "t10s13");
SgDirectedGraphEdge* t13s12 = sgDG->addDirectedEdge(node13, node12, "t13s12");
SgDirectedGraphEdge* t12s14 = sgDG->addDirectedEdge(node12, node14, "t12s14");

SgDirectedGraphEdge* t20s15 = sgDG->addDirectedEdge(node20, node15, "t20s15");
SgDirectedGraphEdge* t15s16 = sgDG->addDirectedEdge(node15, node16, "t15s16");
SgDirectedGraphEdge* t16s17 = sgDG->addDirectedEdge(node16, node17, "t16s17");
SgDirectedGraphEdge* t17s18 = sgDG->addDirectedEdge(node17, node18, "t17s18");
SgDirectedGraphEdge* t18s16 = sgDG->addDirectedEdge(node18, node16, "t18s16");
SgDirectedGraphEdge* t18s162 = sgDG->addDirectedEdge(node18, node16, "t18s162");

SgDirectedGraphEdge* t18s18 = sgDG->addDirectedEdge(node18, node18, "t18s18");
SgDirectedGraphEdge* t16s19 = sgDG->addDirectedEdge(node16, node19, "t16s19");
SgDirectedGraphEdge* t15s17 = sgDG->addDirectedEdge(node15, node17, "t15s17");
SgDirectedGraphEdge* t17s15 = sgDG->addDirectedEdge(node17, node15, "t17s15");

SgDirectedGraphEdge* t21s22 = sgDG->addDirectedEdge(node21, node22, "t21s22");
SgDirectedGraphEdge* t21s23 = sgDG->addDirectedEdge(node21, node23, "t21s23");
SgDirectedGraphEdge* t23s24 = sgDG->addDirectedEdge(node23, node24, "t23s24");
SgDirectedGraphEdge* t22s24 = sgDG->addDirectedEdge(node22, node24, "t22s24");
SgDirectedGraphEdge* t22s23 = sgDG->addDirectedEdge(node22, node23, "t22s23");
SgDirectedGraphEdge* t23s22 = sgDG->addDirectedEdge(node23, node22, "t23s22");






    vector<SgGraphNode*> vec1;
    vector<SgGraphNode*> vec2;
    set<vector<SgGraphNode*> > tmppath; 

    set<vector<SgGraphNode*> > newPathList2;
    InheritedAttribute nullInherit(0, newPathList2);
    visitorTraversal* vis = new visitorTraversal;
    vec.push_back(node1);
    begincp.insert(vec);
    InheritedAttribute inheritedAttribute(1, begincp);
    //cout << "#1" << endl;
    SgIncidenceDirectedGraph* gTree1 = new SgIncidenceDirectedGraph();
    vis->traverse(node1, sgDG, gTree1, inheritedAttribute, nullInherit, 0, false, node5);
    //node1 node2 node5
    vec1.push_back(node1);
    vec1.push_back(node2);
    vec1.push_back(node5);
    tmppath.insert(vec1);
    ROSE_ASSERT((vis->graphnodeinheritedmap[node5]).pathcount == 1);
    ROSE_ASSERT((vis->graphnodeinheritedmap[node5]).collected_paths == tmppath);
    vec.clear();
    vec1.clear();
    tmppath.clear();
    vec.push_back(node6);
    begincp.clear();
    begincp.insert(vec);
    InheritedAttribute inheritedAttribute2(1, begincp);
    //cout << "#2" << endl;
    SgIncidenceDirectedGraph* gTree2 = new SgIncidenceDirectedGraph();
    visitorTraversal* vis2 = new visitorTraversal;
    vis2->traverse(node6, sgDG, gTree2, inheritedAttribute2, nullInherit, 0, false, node9);
    //node6 node7 node8 node9 
    vec1.push_back(node6);
    vec1.push_back(node7);
    vec1.push_back(node8);
    vec1.push_back(node9);
    tmppath.insert(vec1);
    ROSE_ASSERT((vis2->graphnodeinheritedmap[node9]).pathcount == 1);
    ROSE_ASSERT((vis2->graphnodeinheritedmap[node9]).collected_paths == tmppath);
    //cout << "#3" << endl;
    vec.clear();
    vec1.clear();
    tmppath.clear();
    vec.push_back(node10);
    begincp.clear();
    begincp.insert(vec);
    SgIncidenceDirectedGraph* gTree3 = new SgIncidenceDirectedGraph();
    visitorTraversal* vis3 = new visitorTraversal;
    InheritedAttribute inheritedAttribute3(1, begincp);
    
    vis3->traverse(node10, sgDG, gTree3, inheritedAttribute3, nullInherit, 0, false, node14);
    ROSE_ASSERT((vis3->graphnodeinheritedmap[node14]).pathcount == 2);
    
    //node10 node11 node12 node14 
    //node10 node13 node12 node14 
    vec1.push_back(node10);
    vec1.push_back(node11);
    vec1.push_back(node12);
    vec1.push_back(node14);
    vec2.push_back(node10);
    vec2.push_back(node13);
    vec2.push_back(node12);
    vec2.push_back(node14);
    tmppath.insert(vec1);
    tmppath.insert(vec2);
    ROSE_ASSERT(tmppath == (vis3->graphnodeinheritedmap[node14]).collected_paths);
    
    //cout << "#4" << endl;
    
    
    vec.clear();
    vec.push_back(node20);
    begincp.clear();
    begincp.insert(vec);
    InheritedAttribute inheritedAttribute4(1, begincp);
    SgIncidenceDirectedGraph* gTree4 = new SgIncidenceDirectedGraph();

    visitorTraversal* vis4 = new visitorTraversal;
    vis4->traverse(node20, sgDG, gTree4, inheritedAttribute4, nullInherit, 0, false, node19);
    vec.clear();
    vec1.clear();
    vec2.clear();
    tmppath.clear();
    //node20 node15 node16 node19 
    //node20 node15 node17 node18 node16 node19 
    vec1.push_back(node20);
    vec1.push_back(node15);
    vec1.push_back(node16);
    vec1.push_back(node19);
    vec2.push_back(node20);
    vec2.push_back(node15);
    vec2.push_back(node17);
    vec2.push_back(node18);
    vec2.push_back(node16);
    vec2.push_back(node19);
    tmppath.insert(vec1);
    tmppath.insert(vec2);
    ROSE_ASSERT((vis4->graphnodeinheritedmap[node19]).collected_paths == tmppath);

    ROSE_ASSERT((vis4->graphnodeinheritedmap[node19]).pathcount == 2);

    //cout << "#5" << endl;
    vec.clear();
    vec.push_back(node21);
    begincp.clear();
    begincp.insert(vec);
    InheritedAttribute inheritedAttribute5(1, begincp);
    SgIncidenceDirectedGraph* gTree5 = new SgIncidenceDirectedGraph();

    visitorTraversal* vis5 = new visitorTraversal;
    vis5->traverse(node21, sgDG, gTree5, inheritedAttribute5, nullInherit, 0, false, node24);
    tmppath.clear();
    vec1.clear();
    vec2.clear();
    vector<SgGraphNode*> vec3;
    vector<SgGraphNode*> vec4;
    /*
    node21 node22 node23 node24 
    node21 node22 node24 
    node21 node23 node22 node24 
    node21 node23 node24 
    */
    vec1.push_back(node21);
    vec1.push_back(node22);
    vec1.push_back(node23);
    vec1.push_back(node24);
    vec2.push_back(node21);
    vec2.push_back(node22);
    vec2.push_back(node24);
    vec3.push_back(node21);
    vec3.push_back(node23);
    vec3.push_back(node22);
    vec3.push_back(node24);
    vec4.push_back(node21);
    vec4.push_back(node23);
    vec4.push_back(node24);
    tmppath.insert(vec1);
    tmppath.insert(vec2);
    tmppath.insert(vec3);
    tmppath.insert(vec4);
    ROSE_ASSERT((vis5->graphnodeinheritedmap[node24]).pathcount == 4);
    ROSE_ASSERT((vis5->graphnodeinheritedmap[node24]).collected_paths == tmppath);

    cout << "complete" << endl;
    return 0;
}