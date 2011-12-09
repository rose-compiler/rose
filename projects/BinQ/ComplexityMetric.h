#ifndef COMPLEXITY_R_H
#define COMPLEXITY_R_H
//#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class ComplexityMetric : public BinAnalyses,AstSimpleProcessing {
 public:
  ComplexityMetric(){testFlag=false; debug=false;};
  virtual ~ComplexityMetric(){};
  bool testFlag;
  void run(SgNode* f1, SgNode* f2);
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return false;}
  std::map<SgNode*,std::string> getResult(){return result;}
  
 private:
  bool project;
  SgAsmGenericFile *genericF;
  void visit(SgNode* node);
  void runTraversal(SgNode* project);
  VirtualBinCFG::AuxiliaryInformation* info;
  bool debug;
  BinQGUI *instance;
  std::map<SgNode*,std::string> result;
  SgAsmFunction* lastFunction;
  int complexity;
  int max;

};



#endif
