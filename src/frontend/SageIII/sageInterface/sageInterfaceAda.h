

#ifndef _SAGEINTERFACE_ADA_H
#define _SAGEINTERFACE_ADA_H 1

#include "sage3basic.hhh"
#include "sageInterface.h"

#include <tuple>

namespace SageInterface
{

/// Contains Ada-specific functionality
namespace Ada
{

namespace
{
  inline
  SgVariableSymbol&
  symOf(const SgVarRefExp& n)
  {
    ASSERT_not_null(n.get_symbol());

    return *n.get_symbol();
  }

  inline
  SgFunctionSymbol&
  symOf(const SgFunctionRefExp& n)
  {
    ASSERT_not_null(n.get_symbol());

    return *n.get_symbol();
  }

  inline
  SgFunctionDeclaration&
  declOf(const SgFunctionSymbol& n)
  {
    ASSERT_not_null(n.get_declaration());

    return *n.get_declaration();
  }

  inline
  SgFunctionDeclaration&
  declOf(const SgFunctionRefExp& n)
  {
    return declOf(symOf(n));
  }

  inline
  SgInitializedName&
  declOf(const SgVarRefExp& n)
  {
    SgVariableSymbol& sy = symOf(n);

    ASSERT_not_null(sy.get_declaration());
    return *sy.get_declaration();
  }

  inline
  SgDeclarationStatement&
  declOf(const SgAdaUnitRefExp& n)
  {
    ASSERT_not_null(n.get_decl());

    return *n.get_decl();
  }



/*
  inline
  SgVariableDeclaration& declOf(const SgVariableSymbol& n)
  {
    SgNode* varnode = SG_DEREF(n.get_declaration()).get_parent();

    return SG_DEREF(isSgVariableDeclaration(varnode));
  }

  inline
  SgVariableDeclaration& declOf(const SgVarRefExp& n)
  {
    return declOf(symOf(n));
  }
*/

  inline
  SgAdaRenamingDecl&
  declOf(const SgAdaRenamingRefExp& n)
  {
    ASSERT_not_null(n.get_decl());

    return *n.get_decl();
  }


  inline
  SgName
  nameOf(const SgSymbol& sy)
  {
    return sy.get_name();
  }

  inline
  SgName
  nameOf(const SgVarRefExp& n)
  {
    return nameOf(symOf(n));
  }

  inline
  SgName
  nameOf(const SgFunctionRefExp& n)
  {
    return nameOf(symOf(n));
  }

  inline
  SgName
  nameOf(const SgEnumVal& n)
  {
    return n.get_name();
  }

  inline
  SgName
  nameOf(const SgAdaRenamingDecl& n)
  {
    return n.get_name();
  }

  inline
  SgName
  nameOf(const SgAdaRenamingRefExp& n)
  {
    return nameOf(declOf(n));
  }

  inline
  SgName
  nameOf(const SgAdaUnitRefExp& n)
  {
    return SageInterface::get_name(n.get_decl());
  }


/*
  inline
  SgName nameOf(const SgImportStatement& import)
  {
    const SgExpressionPtrList& lst = import.get_import_list();
    ROSE_ASSERT(lst.size() == 1);

    return nameOf(SG_DEREF(isSgVarRefExp(lst.back())));
  }
*/

} // anononymous namespace for convenience functions


  extern const std::string roseOperatorPrefix;
  extern const std::string packageStandardName;
  extern const std::string durationTypeName;
  extern const std::string exceptionName;

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
  SgAdaPackageBodyDecl& getPackageBodyDeclaration(const SgAdaPackageSpecDecl& specDecl);
  SgAdaPackageBodyDecl* getPackageBodyDeclaration(const SgAdaPackageSpecDecl* specDecl);
  /// @}

  /// returns the declaration node (either SgAdaTaskSpecDecl or SgAdaTaskTypeDecl) for the task specification
  /// @{
  SgDeclarationStatement& getSpecificationDeclaration(const SgAdaTaskBodyDecl& bodyDecl);
  SgDeclarationStatement* getSpecificationDeclaration(const SgAdaTaskBodyDecl* bodyDecl);
  /// @}

  /// returns the declaration node (either SgAdaProtectedSpecDecl or SgAdaProtectedTypeDecl)
  ///   for the protected object specification
  /// @{
  SgDeclarationStatement& getSpecificationDeclaration(const SgAdaProtectedBodyDecl& bodyDecl);
  SgDeclarationStatement* getSpecificationDeclaration(const SgAdaProtectedBodyDecl* bodyDecl);
  /// @}

  /// returns the body scope (aka definition) of a task, package, or protected object.
  /// \param  scope a task, package, protected object spec or body
  /// \return the corresponding body scope (i.e., SgAdaProtectedBody, SgAdaPackageBody, SgAdaTaskBody)
  ///         returns null when a body cannot be found
  /// \details
  ///   given a spec or body, the function returns the body, null otherwise
  ///   e.g., getBodyScope(SgAdaPackageSpec) -> SgAdaPackageBody
  ///         getBodyScope(SgAdaPackageBody) -> SgAdaPackageBody
  ///         getBodyScope(SgFunctionDefinition) -> nullptr
  const SgScopeStatement* correspondingBody(const SgScopeStatement* scope);

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
  bool isModularType(const SgType& ty);
  bool isModularType(const SgType* ty);
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

  /// returns if the type @ref ty resolves to a fixed point type
  /// \details
  ///    also return true for decimal fixed points
  /// @{
  bool resolvesToFixedType(const SgType* ty);
  bool resolvesToFixedType(const SgType& ty);
  /// @}

  /// returns if the type @ref ty is a decimal fixed point type
  /// \details
  ///    implementation is incomplete and only detects formal decimal fixed point constraints
  /// @{
  bool isDecimalFixedType(const SgType* ty);
  bool isDecimalFixedType(const SgType& ty);
  /// @}

  /// Returns the SgAdaGenericDecl node that makes a declaration (either function/procedure or package)
  /// generic.
  /// \param n a declaration that is possibly part of a generic declaration.
  /// \returns the generic declaration of \ref n, where \ref n is a either function/procedure or package
  ///          that is declared directly under an SgAdaGenericDecl;
  ///          nullptr otherwise.
  /// \details
  ///   For a subtree SgAdaGenericDecl->SgAdaGenericDefn->n, the SgAdaGenericDecl node is returned.
  /// @{
  SgAdaGenericDecl* isGenericDecl(const SgDeclarationStatement& n);
  SgAdaGenericDecl* isGenericDecl(const SgDeclarationStatement* n);
  /// @}

  /// Returns SgAdaGenericDecl for a given SgAdaGenericInstanceDecl.
  /// \details
  ///    skips over intermediate renaming declarations.
  /// @{
  SgAdaGenericDecl& getGenericDecl(const SgAdaGenericInstanceDecl& n);
  SgAdaGenericDecl* getGenericDecl(const SgAdaGenericInstanceDecl* n);
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

  /// returns true iff \ref n is a separated function definition is separated
  /// @{
  bool isSeparatedDefinition(const SgFunctionDeclaration& n);
  bool isSeparatedDefinition(const SgFunctionDeclaration* n);
  /// @}

  struct TypeDescription : std::tuple<SgType*, bool>
  {
    using base = std::tuple<SgType*, bool>;
    using base::base;

    SgType* typerep()     const { return std::get<0>(*this); }
    SgType& typerep_ref() const;
    bool    polymorphic() const { return std::get<1>(*this); }
  };

  /// returns the most fundamental type
  ///   after skipping subtypes, typedefs, etc.
  /// @{
  TypeDescription typeRoot(SgType&);
  TypeDescription typeRoot(SgType*);
  /// @}

  /// returns the type of an expression
  ///   corrects for some peculiarities in the AST
  /// \todo could be integrated into SgExpression::get_type ...
  /// @{
  TypeDescription typeOfExpr(SgExpression&);
  TypeDescription typeOfExpr(SgExpression*);
  /// @}

  /// returns the scope where an operator with name \ref opname and argument types
  ///    in \ref argtypes should be defined.
  /// \param opname   the operarator name
  /// \param argtypes a list of argument types
  /// \{
  SgScopeStatement* operatorScope(std::string opname, SgTypePtrList argtypes);
  /// \}

  /// describes properties of imported units
  struct ImportedUnitResult : std::tuple<std::string, const SgDeclarationStatement*, const SgAdaRenamingDecl*>
  {
    using base = std::tuple<std::string, const SgDeclarationStatement*, const SgAdaRenamingDecl*>;
    using base::base;

    const std::string&            name()         const { return std::get<0>(*this); }

    const SgDeclarationStatement& decl()         const
    {
      ASSERT_not_null(std::get<1>(*this));
      return *(std::get<1>(*this));
    }

    const SgAdaRenamingDecl*      renamingDecl() const { return std::get<2>(*this); }
  };

  /// queries properties of an imported unit
  ImportedUnitResult
  importedUnit(const SgImportStatement& impdcl);

  /// returns the imported element (i.e., the first entry in n's import_list
  const SgExpression&
  importedElement(const SgImportStatement& n);


  /// do not use, this is temporary
  SgScopeStatement* pkgStandardScope();

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

  /// returns true iff \ref ty refers to an object renaming
  /// @{
  bool isObjectRenaming(const SgAdaRenamingDecl* dcl);
  bool isObjectRenaming(const SgAdaRenamingDecl& dcl);
  /// @}

  /// returns true iff \ref ty refers to an exception renaming
  /// @{
  bool isExceptionRenaming(const SgAdaRenamingDecl* dcl);
  bool isExceptionRenaming(const SgAdaRenamingDecl& dcl);
  /// @}

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

  /// returns a list of arguments with named arguments placed at the correct position
  /// \note defaulted arguments are not represented and the list may contain holes (nullptr)
  /// \throws std::logic_error when there is no function associated with the call,
  ///         or when named arguments cannot be identified.
  ///         For an example for the latter, see test case ancestors.adb
  /// \{
  SgExpressionPtrList
  normalizedCallArguments(const SgFunctionCallExp& n);
  /// \}

  /// returns the parameter position of arg in the callee, after the parameters have been
  /// normalized.
  /// \throws a std::logic_error when a position cannot be found.
  /// \details
  ///    assume declaration: procedure X(left, right : integer := 1);
  ///              and call: X(right => 3)
  ///    then normalizedArgumentPosition(call, "3") -> 1
  std::size_t
  normalizedArgumentPosition(const SgFunctionCallExp& call, const SgExpression& arg);



  /// finds the symbol with @ref name in the context of @ref scope or its logical parents in the range
  ///   [scope, limit).
  /// @param  name   an identifier of the sought symbol
  /// @param  scope  current scope
  /// @param  limit  the scope in the chain that terminates the search (this scope will not be considered)
  /// @return a tuple of scope and first symbol inthat scope with the name @ref name.
  ///         or <nullptr, nullptr> if no such symbol exists in the context.
  /// @details
  ///         a scope can contain multiple symbols with the same name, so the caller
  ///         may iterate over the remaining names using the scope's accessor functions
  ///         (next_XXX_symbol).
  /// @{
  std::tuple<const SgScopeStatement*, const SgSymbol*>
  findSymbolInContext(std::string id, const SgScopeStatement& scope, const SgScopeStatement* limit = nullptr);
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


  /// returns the logical parent scope of a scope @ref s.
  /// \details
  ///    Compared to si::getEnclosingScope, this function
  ///    returns the spec as the logical parent for Ada bodies
  ///    (e.g., packages, tasks, protected objects). Otherwise,
  ///    the parent scope is the same as returned by si::getEnclosingScope.
  /// @{
  const SgScopeStatement*
  logicalParentScope(const SgScopeStatement& s);

  const SgScopeStatement*
  logicalParentScope(const SgScopeStatement* s);
  /// @}


  /// returns the associated declaration for symbol @ref n
  ///   or nullptr if there is none.
  SgDeclarationStatement* associatedDeclaration(const SgSymbol& n);

  /// returns the decl where \ref ty was defined
  ///   nullptr if no such declaration can be found.
  /// \todo remove after integrating functionality into SgType...
  /// \details
  ///    Skips over intermediate derived types, subtypes, etc. until a SgNamedType is found.
  ///    Returns the declaration of said type.
  /// \{
  SgDeclarationStatement* associatedDeclaration(const SgType& ty);
  SgDeclarationStatement* associatedDeclaration(const SgType* ty);
  /// \}

  /// returns the base type of a type \ref ty
  /// \details
  ///   In the following type hierarchy baseTypes(Y) would return { X, S1, S2 } but not Integer.
  ///   \code
  ///   type X is new Integer;
  ///   subtype S1 is X range 0..X'Last;
  ///   subtype S2 is S1 range 10..20;
  ///   type Y is new S1;
  ///   \endcode
  /// \todo
  ///    extend for discriminated types.
  /// \{
  SgType*
  baseType(const SgType& ty);

  SgType*
  baseType(const SgType* ty);
  /// \}
/*
  /// finds the declaration associated with ty's base type.
  /// \returns returns the first named base declaration of ty
  ///          nullptr if no declaration can be found.
  /// \details
  ///    finds the declaration assocated with \ref ty, and returns
  ///    the declaration of ty's base type.
  ///    i.e., associatedDecl(baseType(associatedDecl(ty)))
  /// \{
  SgDeclarationStatement*
  baseDeclaration(const SgType& ty);

  SgDeclarationStatement*
  baseDeclaration(const SgType* ty);
  /// \}
*/
  /// finds the underlying enum declaration of a type \ref ty
  /// \returns an enum declaration associated with ty
  ///          nullptr if no declaration can be found
  /// \details
  ///    in contrast to baseDeclaration, baseEnumDeclaration skips
  ///    over intermediate SgTypedefDeclarations that introduce a new type or a subtype.
  /// \{
  SgEnumDeclaration*
  baseEnumDeclaration(SgType* ty);

  SgEnumDeclaration*
  baseEnumDeclaration(SgType& ty);
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
  positionalArgumentLimit(const SgExpressionPtrList& arglst);

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
  /// \param root                  the subtree is traversed to find operator calls (using the traversal mechanism)
  /// \param convertCallSyntax     false, only convert those calls where get_uses_operator_syntax() returns false
  ///                              true,  convert all calls (may result in invalid Ada)
  /// \param convertNamedArguments not relevant, when \ref convertCallSyntax == false
  ///                              true, named arguments are resolved
  ///                              false, named arguments are preserved
  // \todo mv into Ada to C++ converter
  void convertToOperatorRepresentation(SgNode* root, bool convertCallSyntax = false, bool convertNamedArguments = false);
} // Ada
} // SageInterface

#endif /* _SAGEINTERFACE_ADA_H */
