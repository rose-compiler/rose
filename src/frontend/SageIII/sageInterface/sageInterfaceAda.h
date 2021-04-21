

#ifndef _SAGEINTERFACE_ADA_H
#define _SAGEINTERFACE_ADA_H 1

#include "sage3basic.hhh"

namespace SageInterface
{

/// Contains Ada-specific functionality
namespace ada
{
  /// defines the result type for \ref flattenArrayType
  typedef std::pair<SgArrayType*, std::vector<SgExpression*> > FlatArrayType;

  /// tests if the declaration \ref dcl defines a type that is completed
  ///   in a private section.
  /// \return true, iff dcl is completed in a private section.
  /// \pre dcl is not null and points to a first-nondefining declaration.
  /// @{
  bool withPrivateDefinition(const SgDeclarationStatement* dcl);
  bool withPrivateDefinition(const SgDeclarationStatement& dcl);
  /// @}

  /// tests if \ref ty is an unconstrained array
  /// \return true iff ty is unconstrained
  /// @{
  bool unconstrained(const SgArrayType* ty);
  bool unconstrained(const SgArrayType& ty);
  /// @}


  /// flattens the representation of Ada array types.
  /// \param   atype the type of the array to be flattened.
  /// \return  iff \ref is not an arraytype, a pair <nullptr, empty vector> is returned
  ///          otherwise a pair of a array pointer, and a vector of index ranges.
  ///          (the expressions are part of the AST and MUST NOT BE DELETED.
  /// \pre     \ref atype is not null.
  /// @{
  FlatArrayType flattenArrayType(SgType* atype);
  //~ FlatArrayType flattenArrayType(SgType& atype);
  /// @}

  /// returns a range for the range attribute \ref rangeAttribute.
  /// \return a range if rangeAttribute is a range attribute and a range expression is in the AST;
  ///         nullptr otherwise.
  /// \throws a std::runtime_error if the rangeAttribute uses an index specification
  ///         that is not an integral constant expression.
  /// \pre    \ref rangeAttribute is not null
  /// @{
  SgRangeExp* range(const SgAdaAttributeExp* rangeAttribute);
  SgRangeExp* range(const SgAdaAttributeExp& rangeAttribute);
  /// @}

  /// returns the declaration node for the package specification
  /// @{
  SgAdaPackageSpecDecl& getSpecificationDeclaration(const SgAdaPackageBodyDecl& bodyDecl);
  SgAdaPackageSpecDecl* getSpecificationDeclaration(const SgAdaPackageBodyDecl* bodyDecl);
  /// @}

  /// returns the declaration node for the package body, if available
  /// @{
  SgAdaPackageBodyDecl& getBodyDeclaration(const SgAdaPackageSpecDecl& specDecl);
  SgAdaPackageBodyDecl* getBodyDeclaration(const SgAdaPackageSpecDecl* specDecl);
  /// @}

  /// returns the iterator to one past the last declaration (the limit)
  /// in the statement sequence.
  /// @{
  SgStatementPtrList::iterator declarationLimit(SgStatementPtrList& list);
  SgStatementPtrList::iterator declarationLimit(SgBasicBlock& block);
  SgStatementPtrList::iterator declarationLimit(SgBasicBlock* block);
  SgStatementPtrList::const_iterator declarationLimit(const SgStatementPtrList& list);
  SgStatementPtrList::const_iterator declarationLimit(const SgBasicBlock& block);
  SgStatementPtrList::const_iterator declarationLimit(const SgBasicBlock* block);
  /// @}

  /// returns true iff \ref n is an Ada try block
  /// \note an ada try block is a function block, whose non-declarative
  ///       range includes exactly one try stmt. In this case, the begin
  ///       and end block-syntax can be omitted.
  /// @{
  bool isFunctionTryBlock(const SgTryStmt& n);
  bool isFunctionTryBlock(const SgTryStmt* n);
  /// @}

  /// converts all Ada style comments to C++ comments
  // \todo mv into Ada to C++ converter
  void convertAdaToCxxComments(SgNode* root, bool cxxLineComments = true);


} // Ada
} // SageInterface

#endif /* _SAGEINTERFACE_ADA_H */
