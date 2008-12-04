#ifndef DYNAMICINFO_R_H
#define DYNAMICINFO_R_H
#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class DynamicInfo : public BinAnalyses {
 public:
  DynamicInfo(){testFlag=false;};
  virtual ~DynamicInfo(){};
  bool testFlag;
  void run(SgNode* f1, SgNode* f2);
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return false;}
 private:


};



#endif
