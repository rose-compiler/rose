/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : Sep7 07
 * Decription : Data flow Analysis
 ****************************************************/

#ifndef __RoseBin_DataFlowAnalysis__
#define __RoseBin_DataFlowAnalysis__

#include <queue>

//#include "rose.h"
#include "RoseBin_FlowAnalysis.h"
#include "RoseBin_DefUseAnalysis.h"
#include "RoseBin_Emulate.h"
#include "RoseBin_VariableAnalysis.h"
#include "RoseBin_DataFlowAbstract.h"

class RoseBin;

class RoseBin_DataFlowAnalysis : public RoseBin_FlowAnalysis {
 private:
  int nrOfFunctions;
  int nrOfNodesVisited;
  bool interprocedural;


  hash_set < SgDirectedGraphNode*> visited;

  std::map < SgDirectedGraphNode*,int> visitedCounter;

  hash_map <SgDirectedGraphNode*, SgDirectedGraphNode*> nodeBeforeMap;

  typedef hash_map <std::string, SgDirectedGraphNode*> nodeType;
  typedef hash_map < std::string, SgDirectedGraphEdge*> edgeType;

  void traverseEdges(RoseBin_DataFlowAbstract* analysis);


  void init();

  bool existsPath(SgDirectedGraphNode* start, SgDirectedGraphNode* end);


  bool containsHash( hash_set < SgDirectedGraphNode*>& vec, 
		     SgDirectedGraphNode* node);


  RoseBin_DefUseAnalysis* defuse;
  bool printEdges;

  void traverseNodes(RoseBin_DataFlowAbstract* analysis);

  bool exceptionCall(SgAsmx86Call* call);

 public:

  RoseBin_DataFlowAnalysis(SgAsmNode* global, bool forward, RoseBin* ):RoseBin_FlowAnalysis(global) {
    typeNode="DFG";
    typeEdge="DFG-E";  

    interprocedural = false;
    printEdges = false;
    analysisName = "dfa";
    //    roseBin = r;
    //    globalBin = global;
    forward_analysis=forward;
    defuse = new RoseBin_DefUseAnalysis();
  }

  ~RoseBin_DataFlowAnalysis() {
    delete globalBin;
    //delete roseBin;
    delete vizzGraph;

    std::map <std::string, SgAsmFunctionDeclaration* >::iterator it;
    for (it = bin_funcs.begin(); 
	 it!= bin_funcs.end(); it++) {
      delete it->second;
    }
  }

  void traverseGraph(std::vector <SgDirectedGraphNode*>& rootNodes,
		     RoseBin_DataFlowAbstract* defuse, 
		     bool interprocedural);

  void getRootNodes(std::vector <SgDirectedGraphNode*>& rootNodes);

  void init(bool interp, bool pedges) { 
    interprocedural = interp;
    printEdges = pedges;
  }

  void init(bool interp, bool pedges, RoseBin_Graph* g) { 
    interprocedural = interp;
    printEdges = pedges;
    vizzGraph = g;
  }

  RoseBin_DefUseAnalysis* getDefUseAnalysis() { return defuse;}

  void run(RoseBin_Graph* vg, std::string fileN, bool multiedge) ;

  std::set < SgDirectedGraphNode* > 
    getDefFor(SgDirectedGraphNode* node, SgAsmRegisterReferenceExpression::x86_register_enum initName);
  std::set < SgDirectedGraphNode* > 
    getDefFor( uint64_t inst, SgAsmRegisterReferenceExpression::x86_register_enum initName);

  std::set < uint64_t > 
    getDefForInst( uint64_t inst, SgAsmRegisterReferenceExpression::x86_register_enum initName);

};

#endif

