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

//class RoseBin;
//class RoseFile;

class RoseBin_DataFlowAnalysis : public RoseBin_FlowAnalysis {
 private:
  bool writeFile;
  int nrOfNodesVisited;
  bool interprocedural;
  RoseBin_DataFlowAbstract* variableAnalysis;

  rose_hash::hash_set < SgDirectedGraphNode*> visited;

  std::map < SgDirectedGraphNode*,int> visitedCounter;

  rose_hash::hash_map <SgDirectedGraphNode*, SgDirectedGraphNode*> nodeBeforeMap;


  void traverseEdges(RoseBin_DataFlowAbstract* analysis);




  bool existsPath(SgDirectedGraphNode* start, SgDirectedGraphNode* end);


  bool containsHash( rose_hash::hash_set < SgDirectedGraphNode*>& vec, 
		     SgDirectedGraphNode* node);


  RoseBin_DefUseAnalysis* defuse;
  bool printEdges;



  bool exceptionCall(SgAsmx86Instruction* call);


 public:

  RoseBin_DataFlowAnalysis(SgAsmNode* global, bool forward, RoseBin_abstract*, VirtualBinCFG::AuxiliaryInformation* info ):RoseBin_FlowAnalysis(global, info) {
    typeNode="DFG";
    typeEdge="DFG-E";  
    interprocedural = false;
    writeFile=true;
    printEdges = false;
    analysisName = "dfa";
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

  void writeToFile(bool w);

  void traverseNodes(RoseBin_DataFlowAbstract* analysis);

  void init();

  void traverseGraph(std::vector <SgDirectedGraphNode*>& rootNodes,
		     RoseBin_DataFlowAbstract* defuse, 
		     bool interprocedural);


  void init(bool interp, bool pedges) { 
    interprocedural = interp;
    printEdges = pedges;
  }

  void init(bool interp, bool pedges, RoseBin_Graph* g) { 
    interprocedural = interp;
    printEdges = pedges;
    vizzGraph = g;
    //    createInstToNodeTable();
    //defuse->init(vizzGraph, unparser);
    //init();
  }

  int nrOfMemoryWrites() {
    return dynamic_cast<RoseBin_DefUseAnalysis*>(defuse)->getNrOfMemoryWrites();
  }

  int nrOfRegisterWrites() {
    return dynamic_cast<RoseBin_DefUseAnalysis*>(defuse)->getNrOfRegisterWrites() ;
  }

  int nrOfDefinitions() {
    return defuse->getDefinitionSize();
  }

  int nrOfUses() {
    return defuse->getUsageSize();
  }


  RoseBin_DefUseAnalysis* getDefUseAnalysis() { return defuse;}

  void run(RoseBin_Graph* vg, std::string fileN, bool multiedge) ;

  std::set < SgDirectedGraphNode* > 
    getDefFor(SgDirectedGraphNode* node, std::pair<X86RegisterClass, int> initName);
  std::set < SgDirectedGraphNode* > 
    getDefFor( uint64_t inst, std::pair<X86RegisterClass, int> initName);

  std::set < uint64_t > 
    getDefForInst( uint64_t inst, std::pair<X86RegisterClass, int> initName);

  RoseBin_DataFlowAbstract* getVariableAnalysis() {return variableAnalysis;}
};

#endif

