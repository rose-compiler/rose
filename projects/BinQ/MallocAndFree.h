#ifndef MALLOCFREE_R_H
#define MALLOCFREE_R_H
#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class MallocAndFree : public BinAnalyses, AstSimpleProcessing {
 public:
  MallocAndFree(){testFlag=false; debug=true;};
  virtual ~MallocAndFree(){};
  bool testFlag;
  void run(SgNode* f1, SgNode* f2);
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return false;}
  std::map<SgNode*,std::string> getResult(){return result;}
  
 private:
  SgAsmGenericFile *genericF;
  void visit(SgNode* node);
  void runTraversal(SgNode* project);
  VirtualBinCFG::AuxiliaryInformation* info;
  bool debug;
  BinQGUI *instance;
  std::map<SgNode*,std::string> result;
};



#endif
