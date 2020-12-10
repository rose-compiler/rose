#ifndef PROGRAM_INFO_H
#define PROGRAM_INFO_H

#include "ProgramAbstractionLayer.h"
#include <cstdint>
#include "Labeler.h"

class SgFunctionCall;

class ProgramInfo {
 public:
  ProgramInfo(SgProject* root);
  ProgramInfo(CodeThorn::ProgramAbstractionLayer* pal);
  void compute();
  std::string toStringDetailed();
  void printDetailed();
  void writeFunctionCallNodesToFile(std::string fileName, CodeThorn::Labeler* labeler=0);
  
 private:
  bool _validData=false;
  CodeThorn::ProgramAbstractionLayer* _programAbstractionLayer=nullptr;
  SgNode* root;
  uint32_t numFunCall=0;
  uint32_t numWhileLoop=0;
  uint32_t numDoWhileLoop=0;
  uint32_t numForLoop=0;
  uint32_t numLogicOrOp=0;
  uint32_t numLogicAndOp=0;
  uint32_t numConditionalExp=0;
  uint32_t numArrowOp=0;
  uint32_t numDerefOp=0;
  uint32_t numStructAccess=0;
  uint32_t numArrayAccess=0;
  std::list<SgFunctionCallExp*> _functionCallNodes;
};

#endif
