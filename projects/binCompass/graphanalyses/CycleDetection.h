

#ifndef __BinCompass_cycledetection__
#define __BinCompass_cycledetection__


#include "../GraphAnalysisInterface.h"


class CycleDetection: public BC_GraphAnalysisInterface {

 private:
  bool debug;
  std::vector<SgGraphNode*> successors;
  std::set<SgGraphNode*> visited;
  std::map<SgGraphNode*,SgGraphNode*> cycleFound;

 public:
  CycleDetection(GraphAlgorithms* algo):BC_GraphAnalysisInterface(algo) {debug=false;}
  ~CycleDetection() {}

  bool run(std::string& name, SgGraphNode* node,
           SgGraphNode* previous);

  bool runEdge(SgGraphNode* node, SgGraphNode* next) {
    return false;
  }
  bool checkIfValidCycle(SgGraphNode* node,
                         SgGraphNode* next);

  void init(RoseBin_Graph* vg) {
    vizzGraph = vg;
  }

};

#endif

