

#ifndef __RoseBin_GraphAlgorithms__
#define __RoseBin_GraphAlgorithms__

#include "RoseBin_Graph.h"

class GraphAlgorithms {
private:

public:
  //remove later!
   //typedef rose_hash::unordered_map <std::string, SgGraphNode*,rose_hash::hash_string,rose_hash::eqstr_string> nodeType;
  //    typedef rose_graph_node_edge_hash_multimap edgeType;
        //      typedef rose_graph_hash_multimap nodeType;

  VirtualBinCFG::AuxiliaryInformation* info;

  //VirtualBinCFG::AuxiliaryInformation* get_bininfo() {return info;}
  //void set_bininfo(VirtualBinCFG::AuxiliaryInformation* in) { info=in;}

 GraphAlgorithms(VirtualBinCFG::AuxiliaryInformation* info):
#ifdef _MSC_VER
//#define __builtin_constant_p(exp) (0)
#endif
  info(info) {ROSE_ASSERT(info);}
  virtual ~GraphAlgorithms() {}

  SgGraphNode* getDefinitionForUsage(RoseBin_Graph* vizzGraph,SgGraphNode* def);

  void getDirectCFGSuccessors(RoseBin_Graph* vizzGraph,SgGraphNode* node, std::vector <SgGraphNode*>& vec );
  void getDirectCFGPredecessors(RoseBin_Graph* vizzGraph,SgGraphNode* node, std::vector <SgGraphNode*>& vec );
   bool isValidCFGEdge(SgGraphNode* sgNode, SgGraphNode* sgNodeBefore);
   bool isDirectCFGEdge(SgGraphNode* sgNode, SgGraphNode* sgNodeBefore);

};


#endif
