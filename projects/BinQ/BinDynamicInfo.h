#ifndef DYNAMICINFO_R_H
#define DYNAMICINFO_R_H
//#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"


class DynamicInfo : public BinAnalyses,AstSimpleProcessing {
 public:
  DynamicInfo(){testFlag=false; debug=false;};
  virtual ~DynamicInfo(){};
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
  void printOutRelaEntries(SgNode* project);
  bool firstIteration;
  std::map<rose_addr_t, SgAsmElfSymbol*> symbolMap;
  VirtualBinCFG::AuxiliaryInformation* info;
  bool debug;
  BinQGUI *instance;
  std::map<SgNode*,std::string> result;
};



#endif
