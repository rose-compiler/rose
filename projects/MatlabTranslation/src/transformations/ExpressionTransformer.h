#ifndef _EXPRESSION_TRANSFORMER_H
#define _EXPRESSION_TRANSFORMER_H 1

class SgProject;

namespace MatlabToCpp
{
  /// Transforms A \ B to mldivide(A, B)
  void transformLeftDivide(SgProject* project);

  /// Changes A ^ n to power(A, n) call
  /// power is overloaded for scalars and matrix
  void transformPowerOp(SgProject* project);

  /// transform element-by-element right divide A ./ B
  ///   into rdivide(A, B).
  void transformElementwiseRightDivide(SgProject* project);

  /// transform element-by-element multiplication A * B
  ///   into times(A, B).
  void transformElementwiseMultiply(SgProject* project);

  /// For matrices A and B, transform A / B to mrdivide(A, B)
  void transformRightDivide(SgProject* project);
}

#endif /* _EXPRESSION_TRANSFORMER_H */
