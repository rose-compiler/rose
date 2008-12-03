#ifndef ALIGNFUNCTION_R_H
#define ALIGNFUNCTION_R_H
#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class AlignFunction : public BinAnalyses {
 public:
  AlignFunction(){};
  virtual ~AlignFunction(){};
  void run();
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return true;}
 private:
  bool findPosWhenFunctionsAreNotSync(int& position, int& offset, int& currentPos);
  
};



#endif
