#ifndef DWARFFILEI_R_H
#define DWARFFILEI_R_H
//#include "rose.h"

#include <iostream>
#include <list>
#include "BinAnalyses.h"

class DwarfFileInfo : public BinAnalyses {
 public:
  DwarfFileInfo(){testFlag=false; debug=false;};
  virtual ~DwarfFileInfo(){};
  bool testFlag;
  void run(SgNode* f1, SgNode* f2);
  void test(SgNode* f1, SgNode* f2);
  std::string name();
  std::string getDescription();
  bool twoFiles() {return false;}
  std::map<SgNode*,std::string> getResult(){return result;}
  
 private:
  SgAsmGenericFile *genericF;
  VirtualBinCFG::AuxiliaryInformation* info;
  bool debug;
  BinQGUI *instance;
  std::map<SgNode*,std::string> result;

};



#endif
