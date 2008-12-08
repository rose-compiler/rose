#ifndef DYNAMICINFO_R_H
#define DYNAMICINFO_R_H
#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

#if 0
class DI_traversal   : public AstSimpleProcessing {
 public:
  DI_traversal() {
  }
  virtual ~DI_traversal() {}
  virtual void visit(SgNode* node) =0;
};
#endif

class DynamicInfo : public BinAnalyses,AstSimpleProcessing {
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
  SgAsmGenericFile *genericF;
  void visit(SgNode* node);
  void printOutRelaEntries(SgNode* project);
};



#endif
