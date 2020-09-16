
#ifndef NORMALIZATION_CXX_H
#define NORMALIZATION_CXX_H 1

struct SgNode;

namespace CodeThorn
{
  struct Normalization;
  
  /// normalizes some C++ concepts by updating the Ast
  /// - generates constructors and destructors if needed
  /// - moves member construction and initialization into ctor body
  /// - moves member destruction into dtor body
  /// \param norm the CodeThorn normalization class
  /// \param node the root node of all subtrees to be normalized
  /// \note the result is no longer "true" C++
  void normalizeCxx(Normalization& norm, SgNode* node);
  
  /// tests if @ref n requires compiler generation
  bool needsCompilerGeneration(SgMemberFunctionDeclaration& n);
}

#endif /* NORMALIZATION_CXX_H */
