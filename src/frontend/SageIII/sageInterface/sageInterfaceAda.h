

#ifndef _SAGEINTERFACE_ADA_H
#define _SAGEINTERFACE_ADA_H 1

#include "sage3basic.hhh"

#include <tuple>

namespace SageInterface
{

/// Contains Ada-specific functionality
namespace ada
{
  extern const std::string roseOperatorPrefix;
  extern const std::string packageStandardName;

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


  /// returns an integer value for args[0] as used by type attributes first and last
  /// \throws throws an exception if args[0] cannot be constant folded
  /// \note currently only constant values are supported
  /// \{
  int firstLastDimension(SgExprListExp& args);
  int firstLastDimension(SgExprListExp* args);
  /// \}

  /// defines the result type for \ref getArrayTypeInfo
  using FlatArrayType = std::pair<SgArrayType*, std::vector<SgExpression*> >;

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
  /// @}

  /// represents a branch in an if elsif else context (either statement or expression).
  template <class SageLocatedNode>
  struct IfInfo : std::tuple<SgExpression*, SageLocatedNode*>
  {
    using base = std::tuple<SgExpression*, SageLocatedNode*>;
    using base::base;

    SgExpression*    condition()  const { return std::get<0>(*this); }
    SageLocatedNode* trueBranch() const { return std::get<1>(*this); }
    bool             isElse()     const { return condition() == nullptr; }
  };

  /// returns a sequence of if (x) then value
  ///   the last else does not have
  using IfExpressionInfo = IfInfo<SgExpression>;
  using IfStatementInfo  = IfInfo<SgStatement>;

  /// returns a flat representation of if expressions
  std::vector<IfExpressionInfo>
  flattenIfExpressions(SgConditionalExp& n);

  /// returns a flat representation of if-elsif-else statements
  std::vector<IfStatementInfo>
  flattenIfStatements(SgIfStmt& n);

  /// integer constant folding
  /// \returns an integral value for an Ada expression if possible
  /// \throws  an exception otherwise.
  long long int
  staticIntegralValue(SgExpression* n);


  /// returns the expression of an expression statement, or nullptr if s is some other node
  SgExpression*
  underlyingExpr(const SgStatement* s);

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

  /// returns true iff \ref n is an Ada function try block
  /// \note an ada try block is a function block, whose non-declarative
  ///       range includes exactly one try stmt. In this case, the unparser
  ///       can omit the separate begin and end statement.
  /// @{
  bool isFunctionTryBlock(const SgTryStmt& n);
  bool isFunctionTryBlock(const SgTryStmt* n);
  /// @}


  /// returns true iff \ref n is an Ada package try block
  /// \note an ada try block is a function block, whose non-declarative
  ///       range includes exactly one try stmt. In this case, the unparser
  ///       can omit the separate begin and end statement.
  /// @{
  bool isPackageTryBlock(const SgTryStmt& n);
  bool isPackageTryBlock(const SgTryStmt* n);
  /// @}


  /// returns true iff \ref n has an unknown discriminant part
  /// @{
  bool hasUnknownDiscriminants(const SgAdaDiscriminatedTypeDecl& n);
  bool hasUnknownDiscriminants(const SgAdaDiscriminatedTypeDecl* n);
  /// @}


  /// return if the type @ref ty is the corresponding universal type representation in ROSE
  /// @{
  bool isIntegerType(const SgType& ty);
  bool isIntegerType(const SgType* ty);
  bool isFloatingPointType(const SgType& ty);
  bool isFloatingPointType(const SgType* ty);
  bool isDiscreteType(const SgType* ty);
  bool isDiscreteType(const SgType& ty);
  /// @}

  /// returns if the type @ref ty is a fixed point type
  /// \details
  ///    also return true for decimal fixed points
  /// @{
  bool isFixedType(const SgType* ty);
  bool isFixedType(const SgType& ty);
  /// @}

  /// returns if the type @ref ty is a decimal fixed point type
  /// \details
  ///    implementation is incomplete and only detects formal decimal fixed point constraints
  /// @{
  bool isDecimalFixedType(const SgType* ty);
  bool isDecimalFixedType(const SgType& ty);
  /// @}

  /// returns if dcl is a generic declaration
  /// @{
  SgAdaGenericDecl* isGenericDecl(const SgDeclarationStatement& n);
  SgAdaGenericDecl* isGenericDecl(const SgDeclarationStatement* n);
  /// @}


  /// returns the SgAdaDiscriminatedTypeDecl iff \ref n is discriminated
  ///         null otherwise
  /// @{
  SgAdaDiscriminatedTypeDecl* getAdaDiscriminatedTypeDecl(const SgDeclarationStatement& n);
  SgAdaDiscriminatedTypeDecl* getAdaDiscriminatedTypeDecl(const SgDeclarationStatement* n);
  /// @}

  /// tests if the declaration \ref decl corresponds to a stub (aka separated unit)
  /// @{
  bool hasSeparatedBody(const SgDeclarationStatement& dcl);
  bool hasSeparatedBody(const SgDeclarationStatement* dcl);
  /// @}

  /// returns true iff \ref n is a unit definition that has been separated
  /// @{
  bool isSeparatedBody(const SgDeclarationStatement& n);
  bool isSeparatedBody(const SgDeclarationStatement* n);
  /// @}

  /// returns the most fundamental type
  ///   after skipping derived types, subtypes, typedefs, etc.
  /// @{
  SgType* typeRoot(SgType&);
  SgType* typeRoot(SgType*);
  SgType* typeRoot(SgExpression&);
  SgType* typeRoot(SgExpression*);
  /// @}


  /// takes a function name as used in ROSE and converts it to a name in Ada
  ///   (i.e., '"' + operator_text + '"').
  ///   if \ref nameInRose does not name an operator, then the name is returned as is.
  std::string convertRoseOperatorNameToAdaName(const std::string& nameInRose);

  /// takes a function name as used in ROSE and converts it to an operator in Ada
  ///   (i.e., operator_text).
  ///   if \ref nameInRose does not name an operator, an empty string is returned
  std::string convertRoseOperatorNameToAdaOperator(const std::string& nameInRose);

  /// Details of expression aggregates
  struct AggregateInfo : std::tuple< SgAdaAncestorInitializer*,
                                     SgExpressionPtrList::const_iterator,
                                     SgExpressionPtrList::const_iterator
                                   >
  {
    using base = std::tuple< SgAdaAncestorInitializer*,
                             SgExpressionPtrList::const_iterator,
                             SgExpressionPtrList::const_iterator
                           >;
    using base::base;

    /// returns the ancestor initializer iff it exists, otherwise null
    SgAdaAncestorInitializer* ancestor() const { return std::get<0>(*this); }

    /// returns the remaining range-begin without the ancestor initializer (if it existed)
    SgExpressionPtrList::const_iterator begin() const { return std::get<1>(*this); }

    /// returns the underlying's list end iterator
    SgExpressionPtrList::const_iterator end() const { return std::get<2>(*this); }

    /// returns if the remaining range (w/o the ancestor initializer) indicates a null record.
    bool nullRecord() const { return begin() == end(); }
  };

  /// returns the ancestor initializer, if \ref exp refers to an extension aggregate
  ///         null otherwise
  /// @{
  AggregateInfo splitAggregate(const SgExprListExp& exp);
  AggregateInfo splitAggregate(const SgExprListExp* exp);
  /// @}


  /// returns a package spec decl if the declaration \ref n renames a package
  /// returns nullptr otherwise
  /// @{
  SgAdaPackageSpecDecl* renamedPackage(const SgAdaRenamingDecl& n);
  SgAdaPackageSpecDecl* renamedPackage(const SgAdaRenamingDecl* n);
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

  /// converts text to constant values
  /// \{
  long long int convertIntegerLiteral(const char* img);

  std::string convertStringLiteral(const char* img);

  long double convertRealLiteral(const char* img);

  char convertCharLiteral(const char* img);
  /// \}


  /// converts all Ada style comments to C++ comments
  // \todo mv into Ada to C++ converter
  void convertAdaToCxxComments(SgNode* root, bool cxxLineComments = true);

  /// converts all symbol tables from case insensitive to case sensitive
  // \todo mv into Ada to C++ converter
  void convertToCaseSensitiveSymbolTables(SgNode* root);

  /// converts AST from a function call representation to operator form
  ///   for fundamental operator declarations.
  // \todo mv into Ada to C++ converter
  void convertToOperatorRepresentation(SgNode* root);
} // Ada
} // SageInterface

#endif /* _SAGEINTERFACE_ADA_H */
