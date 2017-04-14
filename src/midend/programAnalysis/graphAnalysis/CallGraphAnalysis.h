


#ifndef __SB_CallAnalysis__
#define __SB_CallAnalysis__

#include "RoseSrc_CallGraphAnalysis.h"
#include "RoseBin_DotGraph.h"
#include "RoseBin_GmlGraph.h"
#include "GraphAlgorithms.h"
#include "RoseBin_abstract.h"
#include "RoseObj.h"
#include "RoseBin_CallGraphAnalysis.h"

class ROSE_DLL_API SB_CallGraph {
 private:
  RoseBin_CallGraphAnalysis* bin_callanalysis;
  RoseSrc_CallGraphAnalysis* src_callanalysis;

 public:
  SB_CallGraph() {
    bin_callanalysis=NULL;
    src_callanalysis=NULL;
  }
  virtual ~SB_CallGraph() {}

  void createCallGraph(SgProject* project) ROSE_DEPRECATED("no longer supported");

  RoseBin_CallGraphAnalysis* getBinaryCallGraph() ROSE_DEPRECATED("no longer supported") {return bin_callanalysis;}
  RoseSrc_CallGraphAnalysis* getSourceCallGraph() ROSE_DEPRECATED("no longer supported") {return src_callanalysis;}


};

#endif
