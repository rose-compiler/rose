#ifndef _MATLAB_SIMPLE_TRANSFORMER_H
#define _MATLAB_SIMPLE_TRANSFORMER_H 1

class SgProject;

namespace MatlabToCpp
{
  /**
   * Convert
   *   for i = range.getMatrix()
   *   end
   * or
   *   for i = matrix
   * to
   *   for(i = range.getMatrix().begin(); i != range.getMatrix().end(); ++i)
   *   {
   *     //update every references to i inside loop with *i
   *   }
   */
  void transformForloop(SgProject* project);

  /**
   * This class looks at each argument in a function call
   * if the argument is a matrix expression, then semantic transformation is done
   *
   * Convert Matrix([1, 2], [2, 4]) ==> Matrix({1, 2}, {2, 4})
   * This conversion will map to an overloaded operator () in the Matrix wrapper
   */
  void transformMatrixOnFunctionCallArguments(SgProject* project);

  /**
   * This transformer goes through every SgRangeExp and changes it to a call to a
   * getMatrix function
   * ex: x = 1:2:3
   * becomes
   * Range<int> r;
   * r.set(1,2,3)
   * x = r.getMatrix();
  */
  void transformRangeExpression(SgProject* project);

  /**
   * Transforms selected calls    *
   * (1) disp({'x', num2str(n)}) -> disp('x', num2str(n))
   * (2) rand()                  -> rand0()
   */
  void transformSelectedCalls(SgProject* project);

  /// \todo
  void transformReturnStatement(SgProject* project);

  /// \todo
  void transformTranspose(SgProject* project);

  /// \todo documentation, test, and implementation
  /// \note CURRENTLY NOT USED
  void transformForLoopColon(SgProject* project);

  /**
   * This class performs AST transformation in which an SgMatrix is present on either LHS or RHS of AssignOp
   * Example: [a, b, c] = doSth()
   * or A = [1, 2, 3]
   */
  void transformMatrixOnAssignOp(SgProject *project);

  /**
   * Adds a function return attribute for each function that returns something.
   */
  void transformReturnListAttribute(SgProject* project);
}

#endif /* _MATLAB_SIMPLE_TRANSFORMER_H */
