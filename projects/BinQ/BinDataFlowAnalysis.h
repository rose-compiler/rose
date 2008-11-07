#ifndef DATAFLOW_R_H
#define DATAFLOW_R_H
#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class BinDataFlowAnalysis : public BinAnalyses {
 public:
  BinDataFlowAnalysis(){};
  virtual ~BinDataFlowAnalysis(){};
  void run();
  std::string name();
  std::string getDescription();
 private:


};



#endif
