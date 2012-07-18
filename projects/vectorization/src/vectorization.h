#ifndef _VECTORIZATION_H
#define _VECTORIZATION_H

#include "rose.h"
#include "sageBuilder.h"
#include <vector>

namespace vectorization 
{
  void addHeaderFile(SgProject*, std::vector<std::string>&);
  void analyzeLoop(SgForStatement*);  
  int  determineVF(SgForStatement*); 
  void updateSIMDDeclaration(SgFunctionDeclaration*); 
  void replaceSIMDOperator(SgForStatement*);
  void fixLoopBound(SgForStatement*); 
}

#endif  //_VECTORIZATION_H
