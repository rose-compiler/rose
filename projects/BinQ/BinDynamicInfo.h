#ifndef DYNAMICINFO_R_H
#define DYNAMICINFO_R_H
#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class DynamicInfo : public BinAnalyses {
 public:
  DynamicInfo(){};
  virtual ~DynamicInfo(){};
  void run();
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return false;}
 private:


};



#endif
