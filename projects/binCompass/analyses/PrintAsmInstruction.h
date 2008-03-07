

#ifndef __BinCompass_printAsmInstruction__
#define __BinCompass_printAsmInstruction__

#include "rose.h"
#include "../binCompassAnalysisInterface.h"

class PrintAsmInstruction: public BC_AnalysisInterface {

  hash_map<std::string, int> instMap;



 public:
  PrintAsmInstruction() {
    instMap.clear();
  }
  ~PrintAsmInstruction() {}

  void visit(SgNode* node);
  void init(SgNode* node);
  void finish(SgNode* node);

};

#endif

