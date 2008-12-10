#ifndef DATAFLOW_R_H
#define DATAFLOW_R_H
#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class BinDataFlowAnalysis : public BinAnalyses {
 public:
  BinDataFlowAnalysis(){testFlag=false;};
  virtual ~BinDataFlowAnalysis(){};
  bool testFlag;
  void run(SgNode* f1, SgNode* f2);
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return false;}
  std::map<SgNode*,std::string> getResult(){return result;}
 private:
  std::map<SgNode*,std::string> result;

};



#endif
