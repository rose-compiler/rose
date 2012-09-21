#ifndef _VECTORIZATION_H
#define _VECTORIZATION_H

#include "rose.h"
#include "sageBuilder.h"
#include <vector>
#include <iostream>
#include <fstream>

namespace SIMDVectorization 
{
  class conditionalStmts
  {
    SgExpression* lhsExpr;
    SgExpression* trueExpr;
    SgExpression* falseExpr;
    
    public:
    conditionalStmts(SgExpression*,SgExpression*,SgExpression*);
    conditionalStmts();
    ~conditionalStmts();
    void updateFalseStmt(SgExpression*);
    SgExpression* getLhsExpr(); 
    SgExpression* getTrueExpr(); 
    SgExpression* getFalseExpr(); 
  };

//  Add the SIMD header files, rose_simd.h, to the output file.
  void addHeaderFile(SgProject*);

//  Perform strip-mining on a loop.  The integer argument is the vector factor.
  void stripmineLoop(SgForStatement*, int);

//  Perform strip-mining transformation on a vectorizable loop, update its loop stride.
  void updateLoopIteration(SgForStatement*, int);

//  vectorize unary operations insize vectorizable loop
  void vectorizeUnaryOp(SgStatement*);

//  vectorize binary operations insize vectorizable loop
  void vectorizeBinaryOp(SgStatement*);

//  vectorize ifStmt inside vectorizable loop 
  void vectorizeConditionalStmt(SgStatement*);

//  translate ifStmt inside vectorizable loop 
  void translateIfStmt(SgIfStmt*, std::map<SgName,conditionalStmts*>&);

//  translate the binary operator to SIMD intrisic functions
  void translateBinaryOp(SgBinaryOp*, SgScopeStatement*, SgName);

//  translate the operands to use SIMD data types
  void translateOperand(SgExpression*);

//  identify all the multiply-accumulate operations and perform the translation on them. 
  void translateMultiplyAccumulateOperation(SgStatement*);

//  repalce the multiply-accumulate operations to the function call.  The second argument is the name of function.
  void generateMultiplyAccumulateFunctionCall(SgBinaryOp*,SgName);

//  Insert SIMD data types, __SIMD, __SIMDi and __SIMDd, into AST.  
  void insertSIMDDataType(SgGlobal*);

//  get the mapped SIMD data type from the scalar data type
  SgType* getSIMDType(SgType*, SgScopeStatement*);

//  Decide the suffix name of SIMD functions based on the operand's type
  std::string getSIMDOpSuffix(SgType*);

// Promote scalar variable to SIMD varialbe before the loop, or extract scalar from SIMD variable after loop
  void scalarVariableConversion(SgForStatement*, std::set<SgInitializedName*>, std::set<SgInitializedName*>);

// Add expression for true statement into ConditionalStmtTable
  void insertConditionalStmtTable(SgStatement*, std::map<SgName,conditionalStmts*>&);
  void updateConditionalStmtTable(SgStatement*, std::map<SgName,conditionalStmts*>&);
}


#endif  //_VECTORIZATION_H
