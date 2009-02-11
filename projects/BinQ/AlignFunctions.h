#ifndef ALIGNFUNCTION_R_H
#define ALIGNFUNCTION_R_H
//#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class AlignFunction : public BinAnalyses {
 public:
  AlignFunction(){testFlag=false;};
  virtual ~AlignFunction(){};
  bool testFlag;
  void run(SgNode* f1, SgNode* f2);
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return true;}
  std::map<SgNode*,std::string> getResult(){return result;}
 private:
  std::map<SgNode*,std::string> result;
  bool findPosWhenFunctionsAreNotSync(int& position, int& offset, int& currentPos);
  
};



#endif
