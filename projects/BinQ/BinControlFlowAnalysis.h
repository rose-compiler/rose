#ifndef CONTROLFLOW_R_H
#define CONTROLFLOW_R_H
#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class BinControlFlowAnalysis : public BinAnalyses {
 public:
  BinControlFlowAnalysis(){};
  virtual ~BinControlFlowAnalysis(){};
  void run();
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return false;}
 private:


};



#endif
