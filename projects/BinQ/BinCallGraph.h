#ifndef BINCALLGRAPH_R_H
#define BINCALLGRAPH_R_H
//#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class BinCallGraph : public BinAnalyses {
 public:
  BinCallGraph(){testFlag=false;};
  virtual ~BinCallGraph(){};
  bool testFlag;
  void run(SgNode* f1, SgNode* f2);
  void test(SgNode* fileA, SgNode* fileB);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return false;}
  std::map<SgNode*,std::string> getResult(){return result;}
 private:
  std::map<SgNode*,std::string> result;
 


};



#endif
