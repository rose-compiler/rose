

#ifndef __BinCompass_printAsmInstruction__
#define __BinCompass_printAsmInstruction__


#include "../binCompassAnalysisInterface.h"

class PrintAsmInstruction: public BC_AnalysisInterface {

  rose_hash::hash_map<std::string, int> instMap;



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

