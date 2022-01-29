

#ifndef _SAGEINTERFACE_ADA_H
#define _SAGEINTERFACE_ADA_H 1

#include "sage3basic.hhh"

#include <tuple>

namespace SageInterface
{

/// Contains Ada-specific functionality
namespace ada
{
  /// defines the result type for \ref flattenArrayType
  typedef std::pair<SgArrayType*, std::vector<SgExpression*> > FlatArrayType;

  /// tests if the declaration \ref dcl defines a public type that is completed
  ///   in a private section.
  /// \return true, iff dcl is public and completed in a private section.
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

  using StatementRange = std::pair<SgDeclarationStatementPtrList::iterator, SgDeclarationStatementPtrList::iterator>;

  /// returns all statements/declarations in the global scope that were defined
  /// in the source file.
  /// \param   globalScope a reference to the global scope
  /// \param   mainfile    a reference to the main source file (name)
  /// \returns a range [first-in-sequence, limit-of-sequence) of
  ///          global-scope level statements in \ref mainFile
  /// \details
  ///   In other words, the declarations in the with'ed packages are excluded from the returned range.
  /// \{
  StatementRange
  declsInPackage(SgGlobal& globalScope, const std::string& mainFile);

  StatementRange
  declsInPackage(SgGlobal& globalScope, const SgSourceFile& mainFile);
  /// \}


  /// returns a flattened representation of Ada array types.
  /// \param   atype the type of the array to be flattened.
  /// \return  iff \ref is not an arraytype, a pair <nullptr, empty vector> is returned
  ///          otherwise a pair of a array pointer, and a vector of index ranges.
  ///          Index ranges can be specified in terms of a range (SgRangeExp), a type
  ///          attribute (SgAdaAttributeExp), or full type range (SgTypeExpression).
  ///          (the expressions are part of the AST and MUST NOT BE DELETED).
  /// \pre     \ref atype is not null.
  /// @{
  FlatArrayType getArrayTypeInfo(SgType* atype);
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


  /// returns true iff \ref n has an unknown discriminant part
  /// @{
  bool hasUnknownDiscriminants(const SgAdaDiscriminatedTypeDecl& n);
  bool hasUnknownDiscriminants(const SgAdaDiscriminatedTypeDecl* n);
  /// @}

  /// returns the SgAdaDiscriminatedTypeDecl iff \ref n is discriminated
  ///         null otherwise
  /// @{
  SgAdaDiscriminatedTypeDecl* getAdaDiscriminatedTypeDecl(const SgDeclarationStatement& n);
  SgAdaDiscriminatedTypeDecl* getAdaDiscriminatedTypeDecl(const SgDeclarationStatement* n);
  /// @}

  /// returns a package symbol if the declaration \ref n renames a package
  /// returns nullptr otherwise
  /// @{
  SgAdaPackageSymbol* renamedPackageSymbol(const SgAdaRenamingDecl& n);
  SgAdaPackageSymbol* renamedPackageSymbol(const SgAdaRenamingDecl* n);
  /// @}


  /// returns true iff \ref ty refers to a function type (as opposed to procedure)
  /// @{
  bool isFunction(const SgFunctionType& ty);
  bool isFunction(const SgFunctionType* ty);
  /// @}

  //
  // Ada Variant processing

  struct VariantInfo : std::tuple<SgExprListExp*, int>
  {
    using base = std::tuple<SgExprListExp*, int>;
    using base::base;

    /// the exprlist condition
    SgExprListExp* variants() const { return std::get<0>(*this); }

    /// the variant nesting level
    int            depth()    const { return std::get<1>(*this); }
  };

  /// returns basic information about the variant declaration
  VariantInfo
  variantInfo(const SgAdaVariantFieldDecl* n);

  /// get the depth of shared of control variables
  int getSharedControlDepth(const VariantInfo& prev, const VariantInfo& next);

  /// test if \ref prev and \rev next have the same variant condition at position \ref i
  bool haveSameConditionAt(const VariantInfo& prev, const VariantInfo& next, int i);

  struct VariantEntry : std::tuple<SgVarRefExp*, SgExprListExp*>
  {
    using base = std::tuple<SgVarRefExp*, SgExprListExp*>;
    using base::base;

    SgVarRefExp*   control()    const { return std::get<0>(*this); }
    SgExprListExp* conditions() const { return std::get<1>(*this); }
  };

  /// get the control/conditions of the \ref i th entry.
  VariantEntry getVariant(const VariantInfo& prev, int i);

  /// finds the next statement in the range [\ref begin, \ref end) that has a different
  ///   variant condition than \ref lastVariant
  SgDeclarationStatementPtrList::const_iterator
  findVariantConditionChange( SgDeclarationStatementPtrList::const_iterator begin,
                              SgDeclarationStatementPtrList::const_iterator end,
                              const SgAdaVariantFieldDecl* lastVariant
                            );

  //


  struct PrimitiveParameterDesc : std::tuple<size_t, const SgInitializedName*>
  {
    using base = std::tuple<size_t, const SgInitializedName*>;
    using base::base;

    /// the position within the parameter list
    size_t
    pos()  const { return std::get<0>(*this); }

    /// the parameters name in form of an SgInitializedName
    const SgInitializedName*
    name() const { return std::get<1>(*this); }
  };

  /// returns the descriptions for parameters that make an operations primitive
  /// @{
  std::vector<PrimitiveParameterDesc>
  primitiveParameterPositions(const SgFunctionDeclaration&);

  std::vector<PrimitiveParameterDesc>
  primitiveParameterPositions(const SgFunctionDeclaration*);
  /// @}

  /// returns the overriding scope of a primitive function based on the
  ///   associated arguments as defined by the argument list \ref args and
  ///   the primitive argument positions defined by \ref primitiveArgs.
  /// \return the scope of an overriding argument (incl. the original associated type);
  ///         nullptr if no such scope can be found.
  /// \note does not resolve conflicting scopes
  /// \see also \ref primitiveParameterPositions
  /// @{
  SgScopeStatement*
  overridingScope(const SgExprListExp& args, const std::vector<PrimitiveParameterDesc>& primitiveArgs);

  SgScopeStatement*
  overridingScope(const SgExprListExp* args, const std::vector<PrimitiveParameterDesc>& primitiveArgs);
  /// @}

  /// finds the type declaration of a type
  /// \details
  ///    Skips over intermediate derived types, subtypes, etc. until a SgNamedType is found.
  ///    Returns the declaration of said type.
  /// \{
  SgDeclarationStatement*
  baseDeclaration(SgType& ty);

  SgDeclarationStatement*
  baseDeclaration(SgType* ty);
  /// \}

  /// returns true, iff \ref fndef is the body of an explicit null procedure
  bool explicitNullProcedure(const SgFunctionDefinition& fndef);

  /// returns true, iff \ref recdef is the body of an explicit null record
  bool explicitNullRecord(const SgClassDefinition& recdef);

  /// finds the one past the last positional argument (aka the first named argument position).
  /// \return one past the index of the last positional argument.
  /// \details
  ///   examples:
  ///   - argument lists without positional arguments => 0
  ///   - argument lists without named arguments => args.size()
  ///   - empty argument lists => 0
  ///   - (1, 2, LEN => 3) => 2
  /// @{
  size_t
  positionalArgumentLimit(const SgExprListExp& args);

  size_t
  positionalArgumentLimit(const SgExprListExp* args);
  /// @}


  /// converts all Ada style comments to C++ comments
  // \todo mv into Ada to C++ converter
  void convertAdaToCxxComments(SgNode* root, bool cxxLineComments = true);

  /// converts all symbol tables from case insensitive to case sensitive
  void convertToCaseSensitiveSymbolTables(SgNode* root);

  /// converts text to constant values
  /// \{
  long long int convertIntegerLiteral(const char* img);

  std::string convertStringLiteral(const char* img);

  long double convertRealLiteral(const char* img);

  char convertCharLiteral(const char* img);
  /// \}



} // Ada
} // SageInterface

#endif /* _SAGEINTERFACE_ADA_H */
