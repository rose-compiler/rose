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
        cout << "enumerating paths" << endl;
        for (set<vector<SgGraphNode*> >::iterator it = newPathList.begin(); it != newPathList.end(); it++) {
            vector<SgGraphNode*> cp = *it;
            for (vector<SgGraphNode*>::iterator jt = cp.begin(); jt != cp.end(); jt++) {
                cout << (*jt)->get_name() << " ";
            }
            cout << endl;
        }
        if (loop == true) {
            cout << "Number of Paths through this loop: " << newPathList.size() << endl;
            cout << "This should match: " << tmppathcount << endl;
        }
        else {
        //set<vector<SgGraphNode*> > cp = collect_paths(child, newPathList);
        cout << "Number Of Paths:" << newPathList.size() << endl;
        cout << "This should match:" << tmppathcount << endl;
        
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
SgGraphNode* node25 = new SgGraphNode("node25");
SgGraphNode* node26 = new SgGraphNode("node26");
SgGraphNode* node27 = new SgGraphNode("node27");
SgGraphNode* node28 = new SgGraphNode("node28");
SgGraphNode* node29 = new SgGraphNode("node29");
SgGraphNode* node30 = new SgGraphNode("node30");
SgGraphNode* node31 = new SgGraphNode("node31");
SgGraphNode* node32 = new SgGraphNode("node32");
SgGraphNode* node33 = new SgGraphNode("node33");
SgGraphNode* node34 = new SgGraphNode("node34");
SgGraphNode* node35 = new SgGraphNode("node35");
SgGraphNode* node36 = new SgGraphNode("node36");
SgGraphNode* node37 = new SgGraphNode("node37");
SgGraphNode* node38 = new SgGraphNode("node38");
SgGraphNode* node39 = new SgGraphNode("node39");
SgGraphNode* node40 = new SgGraphNode("node40");
SgGraphNode* node41 = new SgGraphNode("node41");
SgGraphNode* node42 = new SgGraphNode("node42");
SgGraphNode* node43 = new SgGraphNode("node43");
SgGraphNode* node44 = new SgGraphNode("node44");
SgGraphNode* node45 = new SgGraphNode("node45");




SgDirectedGraphEdge* t1s2 = sgDG->addDirectedEdge(node1, node2, "t1s2");
SgDirectedGraphEdge* t2s3 = sgDG->addDirectedEdge(node2, node3, "t2s3");
SgDirectedGraphEdge* t3s4 = sgDG->addDirectedEdge(node3, node4, "t3s4");
SgDirectedGraphEdge* t4s6 = sgDG->addDirectedEdge(node4, node6, "t4s6");
SgDirectedGraphEdge* t5s6 = sgDG->addDirectedEdge(node5, node6, "t5s6");
SgDirectedGraphEdge* t6s8 = sgDG->addDirectedEdge(node6, node8, "t6s8");
SgDirectedGraphEdge* t8s9 = sgDG->addDirectedEdge(node8, node9, "t8s9");
SgDirectedGraphEdge* t6s7 = sgDG->addDirectedEdge(node6, node7, "t6s7");
SgDirectedGraphEdge* t7s9 = sgDG->addDirectedEdge(node7, node9, "t7s9");
SgDirectedGraphEdge* t9s10 = sgDG->addDirectedEdge(node9, node10, "t9s10");
SgDirectedGraphEdge* t2s11 = sgDG->addDirectedEdge(node2, node11, "t2s11");
SgDirectedGraphEdge* t11s12 = sgDG->addDirectedEdge(node11, node12, "t11s12");
SgDirectedGraphEdge* t12s13 = sgDG->addDirectedEdge(node12, node13, "t12s13");
SgDirectedGraphEdge* t13s14 = sgDG->addDirectedEdge(node13, node14, "t13s14");
SgDirectedGraphEdge* t14s21 = sgDG->addDirectedEdge(node14, node21, "t14s21");
SgDirectedGraphEdge* t21s14 = sgDG->addDirectedEdge(node21, node14, "t21s14");
SgDirectedGraphEdge* t2s16 = sgDG->addDirectedEdge(node2, node16, "t2s16");
SgDirectedGraphEdge* t22s16 = sgDG->addDirectedEdge(node22, node16, "t22s16");
SgDirectedGraphEdge* t16s17 = sgDG->addDirectedEdge(node16, node17, "t16s17");
SgDirectedGraphEdge* t17s19 = sgDG->addDirectedEdge(node17, node19, "t17s19");
SgDirectedGraphEdge* t19s18 = sgDG->addDirectedEdge(node19, node18, "t19s18");
SgDirectedGraphEdge* t18s16 = sgDG->addDirectedEdge(node18, node16, "t18s16");
SgDirectedGraphEdge* t16s21 = sgDG->addDirectedEdge(node16, node21, "t16s21");
SgDirectedGraphEdge* t20s22 = sgDG->addDirectedEdge(node20, node22, "t20s22");
SgDirectedGraphEdge* t20s21 = sgDG->addDirectedEdge(node20, node21, "t20s21");
SgDirectedGraphEdge* t21s23 = sgDG->addDirectedEdge(node21, node23, "t21s23");
SgDirectedGraphEdge* t10s23 = sgDG->addDirectedEdge(node10, node23, "t10s23");


SgDirectedGraphEdge* t23s24 = sgDG->addDirectedEdge(node23, node24, "t23s24");
SgDirectedGraphEdge* t24s25 = sgDG->addDirectedEdge(node24, node25, "t24s25");
SgDirectedGraphEdge* t25s26 = sgDG->addDirectedEdge(node25, node26, "t25s26");
SgDirectedGraphEdge* t26s27 = sgDG->addDirectedEdge(node26, node27, "t26s27");
SgDirectedGraphEdge* t27s28 = sgDG->addDirectedEdge(node27, node28, "t27s28");
SgDirectedGraphEdge* t28s29 = sgDG->addDirectedEdge(node28, node29, "t28s29");
SgDirectedGraphEdge* t29s30 = sgDG->addDirectedEdge(node29, node30, "t29s30");
SgDirectedGraphEdge* t30s23 = sgDG->addDirectedEdge(node30, node23, "t30s23");
SgDirectedGraphEdge* t26s31 = sgDG->addDirectedEdge(node26, node31, "t26s31");
SgDirectedGraphEdge* t31s32 = sgDG->addDirectedEdge(node31, node32, "t31s32");

SgDirectedGraphEdge* t32s33 = sgDG->addDirectedEdge(node32, node33, "t32s33");
SgDirectedGraphEdge* t33s34 = sgDG->addDirectedEdge(node33, node34, "t33s34");
SgDirectedGraphEdge* t34s35 = sgDG->addDirectedEdge(node34, node35, "t34s35");
SgDirectedGraphEdge* t36s35 = sgDG->addDirectedEdge(node36, node35, "t36s35");
SgDirectedGraphEdge* t35s38 = sgDG->addDirectedEdge(node35, node38, "t35s38");

SgDirectedGraphEdge* t37s36 = sgDG->addDirectedEdge(node37, node36, "t37s36");
SgDirectedGraphEdge* t32s37 = sgDG->addDirectedEdge(node32, node37, "t32s37");

SgDirectedGraphEdge* t38s39 = sgDG->addDirectedEdge(node38, node39, "t38s39");
SgDirectedGraphEdge* t39s38 = sgDG->addDirectedEdge(node39, node38, "t39s38");
SgDirectedGraphEdge* t39s40 = sgDG->addDirectedEdge(node39, node40, "t39s40");
SgDirectedGraphEdge* t40s41 = sgDG->addDirectedEdge(node40, node41, "t40s41");
SgDirectedGraphEdge* t41s40 = sgDG->addDirectedEdge(node41, node40, "t41s40");
SgDirectedGraphEdge* t41s42 = sgDG->addDirectedEdge(node41, node42, "t41s42");
SgDirectedGraphEdge* t42s43 = sgDG->addDirectedEdge(node42, node43, "t42s43");
SgDirectedGraphEdge* t43s42 = sgDG->addDirectedEdge(node43, node42, "t43s42");
SgDirectedGraphEdge* t43s44 = sgDG->addDirectedEdge(node43, node44, "t43s44");
SgDirectedGraphEdge* t44s38 = sgDG->addDirectedEdge(node44, node38, "t44s38");
SgDirectedGraphEdge* t38s44 = sgDG->addDirectedEdge(node38, node44, "t38s44");
SgDirectedGraphEdge* t44s44 = sgDG->addDirectedEdge(node44, node44, "t44s44");


SgDirectedGraphEdge* t44s45 = sgDG->addDirectedEdge(node44, node45, "t44s45");

      set<vector<SgGraphNode*> > begincp;
  vector<SgGraphNode*> vec;
  
    set<vector<SgGraphNode*> > newPathList2;
    InheritedAttribute nullInherit(0, newPathList2);
    visitorTraversal* vis = new visitorTraversal;
    vec.push_back(node1);
    begincp.insert(vec);
    InheritedAttribute inheritedAttribute(1, begincp);
    vis->traverse(node1, sgDG, inheritedAttribute, nullInherit, 2, false);
    cout << "complete" << endl;
    return 0;
}
