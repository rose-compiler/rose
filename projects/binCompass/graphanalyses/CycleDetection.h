

#ifndef __BinCompass_cycledetection__
#define __BinCompass_cycledetection__


#include "../GraphAnalysisInterface.h"


class CycleDetection: public BC_GraphAnalysisInterface {

 private:
  bool debug;
  std::vector<SgDirectedGraphNode*> successors;
  std::set<SgDirectedGraphNode*> visited;
  std::map<SgDirectedGraphNode*,SgDirectedGraphNode*> cycleFound;

 public:
  CycleDetection() {debug=false;}
  ~CycleDetection() {}

  bool run(std::string& name, SgDirectedGraphNode* node,
	   SgDirectedGraphNode* previous);

  bool runEdge(SgDirectedGraphNode* node, SgDirectedGraphNode* next) {
    return false;
  }
  bool checkIfValidCycle(SgDirectedGraphNode* node,
			 SgDirectedGraphNode* next);

  void init(RoseBin_Graph* vg) {
    vizzGraph = vg;
  }

};

#endif

