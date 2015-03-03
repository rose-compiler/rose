
#include "stencilFiniteStateMachine.h"

SgForStatement* 
buildLoopNest(int stencilDimension, SgBasicBlock* & innerLoopBody, SgVariableSymbol* boxVariableSymbol,
   SgVariableSymbol* & indexVariableSymbol_X, SgVariableSymbol* & indexVariableSymbol_Y, SgVariableSymbol* & indexVariableSymbol_Z, 
   SgVariableSymbol* & arraySizeVariableSymbol_X, SgVariableSymbol* & arraySizeVariableSymbol_Y, SgStatement* & anchorStatement);

// class StencilOffsetFSM;

SgExpression* 
buildStencilPoint (StencilOffsetFSM* stencilOffsetFSM, double stencilCoeficient, int stencilDimension, SgVariableSymbol* variableSymbol, 
   SgVariableSymbol* indexVariableSymbol_X, SgVariableSymbol* indexVariableSymbol_Y, SgVariableSymbol* indexVariableSymbol_Z, 
   SgVariableSymbol* arraySizeVariableSymbol_X, SgVariableSymbol* arraySizeVariableSymbol_Y, bool generateLowlevelCode);

SgExprStatement* 
assembleStencilSubTreeArray(SgExpression* stencil_lhs, std::vector<SgExpression*> & stencilSubTreeArray, int stencilDimension, SgVariableSymbol* destinationVariableSymbol);



