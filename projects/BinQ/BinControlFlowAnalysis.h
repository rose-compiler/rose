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
  std::string name();
  std::string getDescription();
 private:


};



#endif
