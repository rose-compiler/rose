

#ifndef __BinCompass_printAsmFunctions__
#define __BinCompass_printAsmFunctions__


#include "../binCompassAnalysisInterface.h"

class PrintAsmFunctions: public BC_AnalysisInterface {

 public:
  PrintAsmFunctions() {}
  ~PrintAsmFunctions() {}

  void visit(SgNode* node);
  void init(SgNode* node);
  void finish(SgNode* node) {};

};

#endif

