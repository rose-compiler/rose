#ifndef DIFFCLONE_R_H
#define DIFFCLONE_R_H
#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"


class FunctionDiffAlgo : public BinAnalyses {
 public:
  FunctionDiffAlgo(){};
  virtual ~FunctionDiffAlgo(){};
  void run();
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return true;}

};


#endif
