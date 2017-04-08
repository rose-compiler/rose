
#ifndef __SB_SrcCallAnalysis__
#define __SB_SrcCallAnalysis__

#include "SB_Graph.h"

class RoseSrc_CallGraphAnalysis {
 private:
         SB_DirectedGraph* graph;
 public:
         RoseSrc_CallGraphAnalysis() {
                 graph = new SB_DirectedGraph();
  }
  virtual ~RoseSrc_CallGraphAnalysis() {}
  SB_DirectedGraph* getGraph() ROSE_DEPRECATED("no longer supported") {return graph;}


};

#endif


