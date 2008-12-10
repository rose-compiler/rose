#ifndef FORBIDDEN_R_H
#define FORBIDDEN_R_H
#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class ForbiddenFunctionCall : public BinAnalyses,AstSimpleProcessing {
 public:
  ForbiddenFunctionCall(){testFlag=false; debug=true;};
  virtual ~ForbiddenFunctionCall(){};
  bool testFlag;
  void run(SgNode* f1, SgNode* f2);
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return false;}
  
 private:
  SgAsmGenericFile *genericF;
  void visit(SgNode* node);
  void runTraversal(SgNode* project);
  VirtualBinCFG::AuxiliaryInformation* info;
  bool debug;
  BinQGUI *instance;
};



#endif
