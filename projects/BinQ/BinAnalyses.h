#ifndef BINANALYSES_R_H
#define BINANALYSES_R_H
#include "rose.h"

#include <iostream>


class  BinAnalyses {
 public:
  static RoseBin_DataFlowAnalysis* dfanalysis;
  static RoseBin_Graph* graph;
  BinAnalyses(){dfanalysis=NULL; graph=NULL;};
  virtual ~BinAnalyses(){};
  virtual void run(SgNode* f1, SgNode* f2)=0;
  virtual void test(SgNode* fileA, SgNode* fileB)=0;
  virtual std::string name()=0;
  virtual std::string getDescription()=0;
  virtual bool twoFiles()=0;
  virtual std::map<SgNode*,std::string> getResult()=0;
};



#endif
