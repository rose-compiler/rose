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

class visitorTraversal : public SgGraphTraversal<InheritedAttribute, SynthesizedAttribute>
   {
     public:
       // virtual function must be defined
          virtual InheritedAttribute evaluateInheritedAttribute(SgGraphNode* n, vector<InheritedAttribute> inheritedAttributeSet, bool loop);
	  set<vector<SgGraphNode*> > collect_paths(SgGraphNode* n, set<vector<SgGraphNode*> > pathsSoFar);
          virtual SynthesizedAttribute evaluateSynthesizedAttribute(SgGraphNode* n, InheritedAttribute inh, SynthesizedAttributesList childAttributes);
   };

set<vector<SgGraphNode*> > visitorTraversal::collect_paths(SgGraphNode* n, set<vector<SgGraphNode*> > pathsSoFar) {
    set<vector<SgGraphNode*> > newPathsSoFar;
    for (set<vector<SgGraphNode*> >::iterator i = pathsSoFar.begin(); i != pathsSoFar.end(); i++) {
        vector<SgGraphNode*> add = *i;
        newPathsSoFar.insert(add);
    }
    return newPathsSoFar;
}

        
InheritedAttribute visitorTraversal::evaluateInheritedAttribute(SgGraphNode* child, vector<InheritedAttribute> inheritedAttributesSet, bool loop) {
    set<vector<SgGraphNode*> > nullPathList;
    InheritedAttribute nullInherit(0, nullPathList);
    if (loop == true) {
        cout << "in loop" << endl;
    }
    int tmppathcount = 0;
    
    set<vector<SgGraphNode*> > newPathList;
    
    cout << "Number Of Inherited Attributes In Set:" << inheritedAttributesSet.size() << endl;
    if (inheritedAttributesSet.size() != 0) {
         
        cout << "calculating inheritedAttributes" << endl;
        for (vector<InheritedAttribute>::iterator i = inheritedAttributesSet.begin(); i != inheritedAttributesSet.end(); i++) {
            
            InheritedAttribute inh = *i;
            cout << "adding " << inh.pathcount << " to pathcount" << endl;
            tmppathcount += inh.pathcount;
            set<vector<SgGraphNode*> > colpath = inh.collected_paths;
            cout << "enumerating subpaths" << endl;
            for (set<vector<SgGraphNode*> >::iterator k = colpath.begin(); k != colpath.end(); k++) {
                
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
        cout << "enumerating paths" << endl;
        set<string> namearr;
        for (set<vector<SgGraphNode*> >::iterator it = newPathList.begin(); it != newPathList.end(); it++) {
            vector<SgGraphNode*> cp = *it;
            for (vector<SgGraphNode*>::iterator jt = cp.begin(); jt != cp.end(); jt++) {
                cout << (*jt)->get_name() << " ";
                ROSE_ASSERT(namearr.find((*jt)->get_name()) == namearr.end());
                namearr.insert((*jt)->get_name());
            }
            namearr.clear();
            cout << endl;
        }
        if (loop == true) {
            cout << "Number of Paths through this loop: " << newPathList.size() << endl;
            cout << "This should match: " << tmppathcount << endl;
            ROSE_ASSERT(tmppathcount == newPathList.size());
        }
        else {
        set<vector<SgGraphNode*> > cp = collect_paths(child, newPathList);
        cout << "Number Of Paths:" << newPathList.size() << endl;
        cout << "This should match:" << tmppathcount << endl;
        ROSE_ASSERT(tmppathcount == newPathList.size());
        
        }
        return InheritedAttribute(tmppathcount, newPathList);
    }
    else {
    cout << "no inherited attributes given" << endl;
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


SgDirectedGraphEdge* t15s16 = sgDG->addDirectedEdge(node15, node16, "t15s16");
SgDirectedGraphEdge* t16s17 = sgDG->addDirectedEdge(node16, node17, "t16s17");
SgDirectedGraphEdge* t17s18 = sgDG->addDirectedEdge(node17, node18, "t17s18");
SgDirectedGraphEdge* t18s16 = sgDG->addDirectedEdge(node18, node16, "t18s16");
SgDirectedGraphEdge* t16s19 = sgDG->addDirectedEdge(node16, node19, "t16s19");
SgDirectedGraphEdge* t15s17 = sgDG->addDirectedEdge(node15, node17, "t15s17");
SgDirectedGraphEdge* t17s15 = sgDG->addDirectedEdge(node17, node15, "t17s15");


    set<vector<SgGraphNode*> > newPathList2;
    InheritedAttribute nullInherit(0, newPathList2);
    visitorTraversal* vis = new visitorTraversal;
    vec.push_back(node1);
    begincp.insert(vec);
    InheritedAttribute inheritedAttribute(1, begincp);
    cout << "#1" << endl;
    vis->traverse(node1, sgDG, inheritedAttribute, nullInherit, 2, false);
    cout << "#2" << endl;
    vis->traverse(node6, sgDG, inheritedAttribute, nullInherit, 2, false);
    cout << "#3" << endl;
    vis->traverse(node10, sgDG, inheritedAttribute, nullInherit, 2, false);
    cout << "#4" << endl;
    vis->traverse(node15, sgDG, inheritedAttribute, nullInherit, 2, false);

    cout << "complete" << endl;
    return 0;
}
