
#ifndef NORMALIZATION_CXX_H
#define NORMALIZATION_CXX_H 1

struct SgNode;

namespace CodeThorn
{
  class Normalization;

  struct CxxTransformStats
  {
    int cnt = 0;
  };

  /// first normalization phase (runs before C normalization)
  /// \details
  ///   normalizes some C++ concepts by updating the AST
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

  /// creates ctor/dtor if needed and not available
  void normalizeCtorDtor(SgNode* root, CxxTransformStats& stats);

  /// splits allocation and initialization
  void normalizeAllocInitSplit(SgNode* root, CxxTransformStats& stats);

  /// adds default values to the argument list
  void normalizeDefaultArgument(SgNode* root, CxxTransformStats& stats);

  /// return value optimization
  void normalizeRVO(SgNode* root, CxxTransformStats& stats);

  /// inserts object destruction into blocks
  void normalizeObjectDestruction(SgNode* root, CxxTransformStats& stats);

  /// inserts calls to virtual base class constructor and destructor
  ///   generates full constructors and destructors if needed
  ///   replaces references to constructors
  void normalizeVirtualBaseCtorDtor(SgNode* root, CxxTransformStats& stats);

  /// removes AST nodes from the initializer list
  ///   presupposes that they have been moved into the appropriate destructors
  void normalizeCleanCtorInitlist(SgNode* root, CxxTransformStats& stats);

  /// generates the "this" parameter for member functions
  void normalizeThisParameter(SgNode* root, CxxTransformStats& stats);

  /// checks if any unwanted nodes remain in the AST
  void normalizationCheck(SgNode* root, CxxTransformStats& stats);
}

#endif /* NORMALIZATION_CXX_H */
