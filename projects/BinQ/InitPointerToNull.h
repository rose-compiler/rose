#ifndef POINTERNULL_R_H
#define POINTERNULL_R_H
//#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class InitPointerToNull : public BinAnalyses,AstSimpleProcessing {
 public:
  InitPointerToNull(){testFlag=false; debug=false;};
  virtual ~InitPointerToNull(){};
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
  std::set<rose_addr_t> memoryWrites;
  std::set<rose_addr_t> memoryRead;
};



#endif
