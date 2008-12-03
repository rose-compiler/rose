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
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return false;}
 private:


};



#endif
