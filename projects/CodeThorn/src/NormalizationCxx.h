
#ifndef NORMALIZATION_CXX_H
#define NORMALIZATION_CXX_H 1

struct SgNode;

namespace CodeThorn
{
  class Normalization;

  /// first normalization phase (runs before C normalization)
  /// \details
  ///   normalizes some C++ concepts by updating the Ast
  ///   - generates constructors and destructors if needed
  ///   - moves member construction and initialization into ctor body
  ///   - moves member destruction into dtor body
  /// \param norm the CodeThorn normalization class
  /// \param node the root node of all subtrees to be normalized
  /// \note the result is no longer "true" C++
  void normalizeCxx1(Normalization& norm, SgNode* node);

  /// second normalization phase (runs after C normalization)
  /// \details
  ///   - inserts ctor and dtor calls at the end of scopes
  void normalizeCxx2(Normalization& norm, SgNode* node);

  /// tests if \ref n requires compiler generation
  bool needsCompilerGeneration(SgMemberFunctionDeclaration& n);

  /// tests if this is constructor creates a temporary
  bool cppCreatesTemporaryObject(const SgExpression* n, bool withCplusplus);

  /// tests if the normalized temporary requires a reference
  bool cppNormalizedRequiresReference(const SgType* varTy, const SgExpression* exp);

  /// tests if \ref n returns a copy
  bool cppReturnValueOptimization(const SgReturnStmt* n, bool withCplusplus);
}

#endif /* NORMALIZATION_CXX_H */
