

#ifndef __BinCompass_printAsmFunctions__
#define __BinCompass_printAsmFunctions__

#include "rose.h"
#include "../AnalysisInterface.h"

class PrintAsmFunctions: public BC_AnalysisInterface {

 public:
  PrintAsmFunctions() {}
  ~PrintAsmFunctions() {}

  void visit(SgNode* node);

};

#endif

