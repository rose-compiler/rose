#ifndef _VECTORIZATION_H
#define _VECTORIZATION_H

#include "rose.h"
#include "sageBuilder.h"
#include <vector>
#include <iostream>
#include <fstream>

namespace SIMDVectorization 
{
//  Add the SIMD header files, rose_simd.h, to the output file.
  void addHeaderFile(SgProject*);
//  Perform strip-mining on a loop.  The integer argument is the vector factor.
  void stripmineLoop(SgForStatement*, int);
//  Perform strip-mining transformation on a vectorizable loop, update its loop stride.
  void updateLoopIteration(SgForStatement*, int);
//  vectorize the innermost loop by translating binary operations into SIMD intrinsic function calls
  void vectorizeBinaryOp(SgForStatement*);
//  translate the binary operator to SIMD intrisic functions
  void translateBinaryOp(SgBinaryOp*, SgScopeStatement*, SgName);
//  translate the operands to use SIMD data types
  void translateOperand(SgExpression*);
//  identify all the multiply-accumulate operations and perform the translation on them. 
  void translateMultiplyAccumulateOperation(SgForStatement*);
//  repalce the multiply-accumulate operations to the function call.  The second argument is the name of function.
  void generateMultiplyAccumulateFunctionCall(SgBinaryOp*,SgName);

//  Insert SIMD data types, __SIMD, __SIMDi and __SIMDd, into AST.  
  void insertSIMDDataType(SgGlobal*);
//  get the mapped SIMD data type from the scalar data type
  SgType* getSIMDType(SgType*, SgScopeStatement*);
//  Decide the suffix name of SIMD functions based on the operand's type
  std::string getSIMDOpSuffix(SgType*);
}

#endif  //_VECTORIZATION_H
