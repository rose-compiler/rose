

#ifndef __BinCompass_GraphAnalysisInterface__
#define __BinCompass_GraphAnalysisInterface__

//#include "rose.h"

#include "RoseBin_DataFlowAbstract.h"
#include "RoseBin_DefUseAnalysis.h"

class BC_GraphAnalysisInterface   : public RoseBin_DataFlowAbstract {
 protected:
  std::string name;

 public:
 BC_GraphAnalysisInterface(GraphAlgorithms* algo):RoseBin_DataFlowAbstract(algo) {}
  virtual ~BC_GraphAnalysisInterface() {}

  virtual bool run(std::string& name, SgGraphNode* node,
			      SgGraphNode* previous) =0;


  std::string get_name() {return name;}
  void set_name(std::string n) {name=n;}
};

#endif
