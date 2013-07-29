#ifndef _NORMALIZATION_H
#define _NORMALIZATION_H

#include "rose.h"
#include "sageBuilder.h"

namespace SIMDNormalization
{
//  Swap the lhs and rhs operand
  void swapOperands(SgBinaryOp*);

/******************************************************************************************************************************/
/*
  Normalize the expression to the following format:
  No transformation needed: a * b + c ==> (a * b) + c
  No transformation needed: a * b - c ==> (a * b) - c
  Transformation needed   : c + a * b ==> (a * b) + c 
  Transformation needed   : c - a * b ==> -( (a * b) - c)
*/
/******************************************************************************************************************************/
  void normalizeExpression(SgProject*);
/******************************************************************************************************************************/
/*
  Normalize the CompoundAssignOp:
  a += b ==> a = a + b
  a -= b ==> a = a - b
  a *= b ==> a = a * b
  a /= b ==> a = a / b
  a &= b ==> a = a & b
  a |= b ==> a = a | b
  a ^= b ==> a = a ^ b
*/
/******************************************************************************************************************************/
  void normalizeCompoundAssignOp(SgForStatement*);
}

#endif // _NORMALIZATION_H
