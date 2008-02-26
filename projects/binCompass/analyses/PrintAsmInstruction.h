

#ifndef __BinCompass_printAsmInstruction__
#define __BinCompass_printAsmInstruction__

#include "rose.h"
#include "../binCompassAnalysisInterface.h"

class PrintAsmInstruction: public BC_AnalysisInterface {

 public:
  PrintAsmInstruction() {}
  ~PrintAsmInstruction() {}

  void visit(SgNode* node);

};

#endif

