#ifndef BINCALLGRAPH_R_H
#define BINCALLGRAPH_R_H
#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class BinCallGraph : public BinAnalyses {
 public:
  BinCallGraph(){};
  virtual ~BinCallGraph(){};
  void run();
  void test(SgNode* fileA, SgNode* fileB);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return false;}
 private:


};



#endif
