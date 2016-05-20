
#include "stencilFiniteStateMachine.h"

extern bool b_enable_polyopt;
extern bool b_gen_vectorization;

SgExpression* buildStencilSubscript(std::vector<SgExpression*> operand, std::vector<SgExpression*> size, int dimSize);

SgForStatement* 
buildLoopNest(int stencilDimension, SgBasicBlock* & innerLoopBody, 
   SgVariableSymbol* boxVariableSymbol, SgVariableSymbol* srcBoxVariableSymbol,SgVariableSymbol* destBoxVariableSymbol,
   SgVariableSymbol* & indexVariableSymbol_X, SgVariableSymbol* & indexVariableSymbol_Y, SgVariableSymbol* & indexVariableSymbol_Z, 
   SgVariableSymbol* & arraySizeVariableSymbol_X, SgVariableSymbol* & arraySizeVariableSymbol_Y, SgVariableSymbol* & arraySizeVariableSymbol_Z, SgStatement* & anchorStatement, std::vector<SgExpression*> &srcLBList, std::vector<SgExpression*> &destLBList);

// class StencilOffsetFSM;

SgExpression* 
buildStencilPoint (StencilOffsetFSM* stencilOffsetFSM, double stencilCoeficient, int stencilDimension, SgVariableSymbol* variableSymbol, 
   SgVariableSymbol* indexVariableSymbol_X, SgVariableSymbol* indexVariableSymbol_Y, SgVariableSymbol* indexVariableSymbol_Z, 
   SgVariableSymbol* arraySizeVariableSymbol_X, SgVariableSymbol* arraySizeVariableSymbol_Y,SgVariableSymbol* arraySizeVariableSymbol_Z, std::vector<SgExpression*> LBList, bool generateLowlevelCode);

SgExpression* 
buildMultiDimStencilPoint (StencilOffsetFSM* stencilOffsetFSM, double stencilCoeficient, int stencilDimension, SgVariableSymbol* variableSymbol, 
   SgVariableSymbol* indexVariableSymbol_X, SgVariableSymbol* indexVariableSymbol_Y, SgVariableSymbol* indexVariableSymbol_Z, 
   SgVariableSymbol* arraySizeVariableSymbol_X, SgVariableSymbol* arraySizeVariableSymbol_Y, SgVariableSymbol* arraySizeVariableSymbol_Z, bool generateLowlevelCode);

SgExprStatement* 
assembleStencilSubTreeArray(SgExpression* stencil_lhs, std::vector<SgExpression*> & stencilSubTreeArray, int stencilDimension, SgVariableSymbol* destinationVariableSymbol);



