#ifndef _VECTORIZATION_H
#define _VECTORIZATION_H

#include "rose.h"
#include "sageBuilder.h"
#include <vector>
#include <iostream>
#include <fstream>

namespace vectorization 
{
  void addHeaderFile(SgProject*, std::vector<std::string>&);
  bool isInnermostLoop(SgForStatement*);

  void stripmineLoop(SgForStatement*, int);
  void updateLoopIteration(SgForStatement*, int);

  void vectorizeBinaryOp(SgForStatement*);
  void translateBinaryOp(SgBinaryOp*, SgScopeStatement*, SgName);
  void translateOperand(SgExpression*);
  void translateMultiplyAccumulateOperation(SgForStatement*);
  void generateMultiplyAccumulateFunctionCall(SgBinaryOp*,SgName);

  void insertSIMDDataType(SgGlobal*);
  SgType* getSIMDType(SgType*, SgScopeStatement*);

  bool isStrideOneLoop(SgNode*);
  std::string getSIMDOpSuffix(SgType*);
}

#endif  //_VECTORIZATION_H
