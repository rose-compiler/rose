#ifndef PROGRAM_INFO_H
#define PROGRAM_INFO_H

#include "ProgramAbstractionLayer.h"
#include <cstdint>
#include "Labeler.h"
#include <string>
#include <map>

class SgFunctionCall;

class ProgramInfo {
 public:
  ProgramInfo(SgProject* root);
  ProgramInfo(CodeThorn::ProgramAbstractionLayer* pal);
  void compute();
  void printDetailed();
  void printCompared(ProgramInfo* other);
  std::string toStringDetailed();
  std::string toCsvStringDetailed();
  bool toCsvFileDetailed(std::string fileName, std::string mode);
  std::string toStringCompared(ProgramInfo* other);
  void writeFunctionCallNodesToFile(std::string fileName, CodeThorn::Labeler* labeler=0);
  
 private:
  enum Element {
    numFunDefs,
    numFunCall,
    numFunPtrCall,
    numForLoop,
    numWhileLoop,
    numDoWhileLoop,
    numLogicOrOp,
    numLogicAndOp,
    numConditionalExp,
    numGlobalVars,
    numLocalVars,
    numArrowOp,
    numDerefOp,
    numStructAccess,
    numArrayAccess,
    NUM
  };
  void initCount();
  bool _validData=false;
  SgNode* root;
  CodeThorn::ProgramAbstractionLayer* _programAbstractionLayer=nullptr;
  std::list<SgFunctionCallExp*> _functionCallNodes;
  std::list<SgFunctionCallExp*> _functionPtrCallNodes;
  uint32_t count[NUM+1];
  std::map<Element,std::string> countNameMap;
};

#endif
