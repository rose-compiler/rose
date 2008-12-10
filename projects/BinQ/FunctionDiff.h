#ifndef DIFFCLONE_R_H
#define DIFFCLONE_R_H
#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"


class FunctionDiffAlgo : public BinAnalyses {
 public:
  FunctionDiffAlgo(){testFlag=false;};
  virtual ~FunctionDiffAlgo(){};
  bool testFlag;
  void run(SgNode* f1, SgNode* f2);
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return true;}
  std::map<SgNode*,std::string> getResult(){return result;}
 private:
  std::map<SgNode*,std::string> result;

};


#endif
