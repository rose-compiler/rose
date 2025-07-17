
#ifndef _ADA_MAKER_H
#define _ADA_MAKER_H 1

///
/// A set of functions that build Sage nodes for representing Ada

//~ #include "sage3basic.h"

#include <vector>
#include <type_traits>

#include "sageBuilder.h"
#include "sageGeneric.h"

// PP(22/2/24)
// THIS is the ONLY INSTALLED HEADER from the Ada frontend !!!
// MUST NOT include any non-installed headers
//   NO other headers from the Ada frontend.
//   NO headers in ROSE that are not installed.
// \todo consider moving this into SageBuilder..

namespace Ada_ROSE_Translation
{
  //
  // file info objects

  /// creates a default file info object for compiler generated nodes
  // \todo currently generateDefaultCompilerGenerated is used
  //       -> replace with info object that indicates compiler generated.
  /*
  Sg_File_Info& mkFileInfo();

  /// creates a file info object for file location given by \p file,
  /// \p line, and \p col
  Sg_File_Info& mkFileInfo(const std::string& file, int line, int col);
  */

  /// sets three file info objects, file_info, startOfConstruct, and
  ///   endOfConstruct to compiler generated.
  void markCompilerGenerated(SgLocatedNode& n);
  void markCompilerGenerated(SgExpression& n);

  void setDefaultFileInfo(SgPragma& n);
  void setDefaultFileInfo(SgLocatedNode& n);
  void setDefaultFileInfo(SgExpression& n);

  /// sets the symbol table associated with \p n to case insensitive
  void setSymbolTableCaseSensitivity(SgScopeStatement& n);

  /// creates a new node by calling new SageNode(args...)
  template <class SageNode, class ... Args>
  inline
  SageNode&
  mkBareNode(Args... args)
  {
    return SG_DEREF(new SageNode(args...));
  }

  /// creates a new node by calling new SageNode(args...) and marks the
  /// location as compiler generated
  template <class SageNode, class ... Args>
  inline
  SageNode&
  mkLocatedNode(Args... args)
  {
    SageNode& sgnode = mkBareNode<SageNode>(args...);

    setDefaultFileInfo(sgnode);
    return sgnode;
  }

  /// creates a scope statement and sets the symbol table case sensitivity
  template <class SageNode, class ... Args>
  inline
  SageNode&
  mkScopeStmt(Args... args)
  {
    SageNode& sgnode = mkLocatedNode<SageNode>(args...);

    setSymbolTableCaseSensitivity(sgnode);
    return sgnode;
  }


  //
  // Type Makers

  /// builds a range constraint from \p range
  /// \details
  ///   \p range is typically of type SgRangeExp, or SgAdaRangeAttributeExp.
  ///        ?? may also be an SgTypeExpression for a full type range. ??
  SgAdaRangeConstraint&
  mkAdaRangeConstraint(SgExpression& range);

  /// builds an index constraint from \p ranges
  /// \param ranges a sequence of ranges.
  SgAdaIndexConstraint&
  mkAdaIndexConstraint(SgExpressionPtrList ranges);

  /// builds a digits constraint for floating point numbers
  /// \param digits an expression indicating the number of digits
  /// \param constraint_opt an optional floating point constraint
  SgAdaDigitsConstraint&
  mkAdaDigitsConstraint(SgExpression& digits, SgAdaTypeConstraint* constraint_opt);

  /// builds an empty constraint, representing an unconstrained subtype
  SgAdaNullConstraint&
  mkAdaNullConstraint();

  /// builds a delta constraint for floating point numbers
  /// \param digits an expression indicating the number of digits
  /// \param constraint_opt an optional floating point constraint
  SgAdaDeltaConstraint&
  mkAdaDeltaConstraint(SgExpression& digits, bool isDecimal, SgAdaTypeConstraint* constraint_opt);

  /// builds a discriminant constraint from \p discriminants
  /// \param discriminants a sequence of discriminants.
  SgAdaDiscriminantConstraint&
  mkAdaDiscriminantConstraint(SgExpressionPtrList discriminants);

  /// builds a subtype constraint by \p constr
  SgAdaSubtype&
  mkAdaSubtype(SgType& superty, SgAdaTypeConstraint& constr, bool fromRoot = false);

  /// builds a derived type from a \p basetype
  SgAdaDerivedType&
  mkAdaDerivedType(SgType& basetype);

  /// builds a range type of \p basetype
  SgRangeType&
  mkRangeType(SgType& basetype);

  /// builds a qualified type ref.base
  /// \todo this is a stop gap function and should not be in the final code
  // no longer needed
  // SgType& mkQualifiedType(SgExpression& ref, SgType& base);

  /// builds a modular integral type with mod expression \p modexpr.
  SgAdaModularType&
  mkAdaModularType(SgExpression& modexpr);

  /// builds a float type with an optional range constraint \p range_opt
  // \no longer used
  //SgAdaFloatType&
  //mkAdaFloatType(SgExpression& digits, SgAdaRangeConstraint* range_opt);

  /// returns the type of an exception
  // \todo revise Exception representation
  //   Exceptions are currently represented in the following way
  //   A type "Exception" exists, and any declared exception becomes a variable
  //   declaration that has type exception. The type of these declared exceptions
  //   are represented as decl type of a variable reference.
  SgDeclType&
  mkExceptionType(SgExpression& n);

  /// returns a type representing all discrete types in Ada
  /// \note used exclusively as formal type constraint
  /// \todo consider renaming to SgAdaDiscreteFormalType
  SgAdaDiscreteType&
  mkAdaDiscreteType();

  /// returns the type produced by an attribute expression
  // \todo consider returning an SgTypeOfType instead of SgDeclType
  SgDeclType&
  mkExprAsType(SgExpression& n);

  /// returns a default type, used to represent an opaque declaration
  SgTypeDefault&
  mkOpaqueType();

  /// returns the void type
  /// \details
  ///   In the ROSE AST, void is used as return type for procedures and to
  ///   indicate unused types (e.g., an entry declaration without family type).
  SgTypeVoid&
  mkTypeVoid();

  /// returns an unknown type, indicating an incomplete AST implementation
  /// \todo should not be in the final AST implementation
  SgTypeUnknown&
  mkTypeUnknown();

  /// makes an unresolved type
  /// \todo should not be in the final version of the translator
  SgTypeUnknown&
  mkUnresolvedType(const std::string& n);

  /// creates a type union for a list of types
  /// \note
  ///   this is used to represent a sequence of exceptions
  SgTypeTuple&
  mkTypeUnion(SgTypePtrList elemtypes);

  /// creates a forward declaration with name \p name in scope \p scope.
  SgEnumDeclaration&
  mkEnumDecl(const std::string& name, SgScopeStatement& scope);

  /// creates an enumeration with name \p name in scope \p scope.
  /// \note uses builder function which looks up the forward declaration if one exists
  SgEnumDeclaration&
  mkEnumDefn(const std::string& name, SgScopeStatement& scope);

  /// creates an ada access type with \p base_type as the type being referenced.
  SgAdaAccessType&
  mkAdaAccessType(SgType& base_type, bool generalAccess = false, bool anonymous = false);

  /// creates a new subroutine type with return type \p retty
  ///   and callback \p complete, which fills in the argument names.
  // \todo not sure if this is the proper way for specifying subroutine types,
  //       because often ROSE types are shared across scopes, and this type cannot be...
  SgAdaSubroutineType&
  mkAdaSubroutineType( SgType& retty,
                       std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete,
                       SgScopeStatement& scope,
                       bool isProtected = false
                     );


  /// creates an entry type from a function parameter list
  // \todo the representation is incomplete and should be replaced
  //       by a new IR node SgAdaEntryType
  // SgFunctionType& mkAdaEntryType(SgFunctionParameterList& lst);

  /// creates an array type with index ranges \p indices and component
  /// type \p comptype.
  /// \param compType the component type
  /// \param dimInfo the dimension information (sets dim_info property)
  /// \param variableLength is true for unconstrained arrays, false otherwise
  ///        (sets is_variable_length_array property)
  SgArrayType& mkArrayType(SgType& comptype, SgExprListExp& dimInfo, bool variableLength = false);

  /// creates the most general integral type
  SgType& mkIntegralType();

  /// creates the most general real type
  SgType& mkRealType();

  /// creates the most general fixed type
  SgType& mkFixedType();

  /// creates a constant type for \p basety
  SgType& mkConstType(SgType& underType);

  /// creates an aliased type for \p basety
  SgType& mkAliasedType(SgType& underType);

  /// creates a not null type for \p basety
  SgType& mkNotNullType(SgType& underType);

  /// creates a function/procedure type with return type \p returnType
  /// \todo add parameter list and ada parameter profiles (types + name)
  SgFunctionType& mkFunctionType(SgType& returnType);

  /// create a formal type
  // SgAdaFormalType& mkAdaFormalType(const std::string& name);

  //~ SgAdaFormalTypeDecl&
  //~ mkAdaFormalTypeDecl(const std::string& name, SgAdaFormalType& ty, SgScopeStatement& scope);

  SgAdaFormalTypeDecl&
  mkAdaFormalTypeDecl(const std::string& name, SgScopeStatement& scope);

  //
  // Statement Makers

  /// wraps an \p expr in an SgExprStatement
  SgExprStatement&
  mkExprStatement(SgExpression& expr);

  /// builds a node representing raising exception \p raised with optional message \p what
  /// \param raised   an expression describing the exception, or a SgNullExpression in case of a re-raise
  /// \param what_opt an expression describing the optional message (i.e., string-expression)
  SgStatement&
  mkRaiseStmt(SgExpression& raised, SgExpression* what_opt = nullptr);

  /// creates a basic block
  SgBasicBlock&
  mkBasicBlock();

  /// creates a while statement with condition \p cond and body \p body.
  SgWhileStmt&
  mkWhileStmt(SgExpression& cond, SgBasicBlock& body);

  /// creates a loop statement with body \p body.
  SgAdaLoopStmt&
  mkAdaLoopStmt(SgBasicBlock& body);

  /// creates a select statement of type \p select_type
  SgAdaSelectStmt&
  mkAdaSelectStmt(SgAdaSelectStmt::select_type_enum select_type);

  /// creates an Ada select alternative statement
  SgAdaSelectAlternativeStmt&
  mkAdaSelectAlternativeStmt(SgExpression& guard, SgBasicBlock& body);

  /// creates a for loop statement with body \p body and an *empty*
  ///   loop header (i.e., init-stmt, test, and increment are nullptr).
  /// to complete an Ada for loop, the init-stmt needs to be set, and
  ///   the increment needs to set the direction (either ++i, --i).
  ///   (see mkForLoopIncrement)
  SgForStatement&
  mkForStatement(SgBasicBlock& body);

  /// creates a with clause for imported elements
  // \todo revisit the ASIS frontend representation and revise how
  //       imports are represented in the AST.
  SgImportStatement&
  mkWithClause(SgExpressionPtrList imported);

  /// creates a use declaration for "use packagename" declarations
  /// \todo revisit representation in ROSE (use package seems more similar to using dircetive)
  /// \param used the used declaration
  SgUsingDeclarationStatement&
  mkUseClause(SgDeclarationStatement& used);

  /// creates an exit statement from loop \p loop with condition \p condition.
  ///   \p explicitLoopName is set if the loop was named and the loop name
  ///   specified with the exit statement.
  SgAdaExitStmt&
  mkAdaExitStmt(SgStatement& loop, SgExpression& condition, bool explicitLoopName);

  /// creates an Ada case statement (similar to C switch).
  // \todo
  // maybe it would be good to have a separate case statement for ADA
  //   as ADA is a bit more restrictive in its switch case syntax compared to C++
  SgSwitchStatement&
  mkAdaCaseStmt(SgExpression& selector, SgBasicBlock& body);

  /// builds a when path for a given case
  /// \param cond the condition guarding the expression
  /// \param blk  the body of the path
  /// \note multiple conditions can be combined using SgCommaOp
  /// \todo should we just use an SgExprListExp for multiple conditions?
  SgCaseOptionStmt&
  mkWhenPath(SgExpression& cond, SgBasicBlock& blk);

  /// builds a when others path with a body of \p blk
  /// \note adaic 5.4: A discrete_choice others, if present, shall
  ///                  appear alone and in the last discrete_choice_list.
  SgDefaultOptionStmt&
  mkWhenOthersPath(SgBasicBlock& blk);


  /// creates an Ada delay statement
  /// \param timeExp      delay expression
  /// \param relativeTime true, if the delay is a period,
  ///                     false if it is a point in time (delay until)
  SgAdaDelayStmt&
  mkAdaDelayStmt(SgExpression& timeExp, bool relativeTime);

  /// creates an Ada abort statement
  /// \param abortList a list of aborted tasks
  SgProcessControlStatement&
  mkAbortStmt(SgExprListExp& abortList);

  /// creates an Ada requeue statement
  /// \param abortList a list of aborted tasks
  SgProcessControlStatement&
  mkRequeueStmt(SgExpression& entryexpr, bool withAbort);


  /// creates an Ada labeled statement.
  /// \param label the label name
  /// \param stmt  the labeled statement
  /// \param encl  the enclosing scope (req. by SageBuilder)
  SgLabelStatement&
  mkLabelStmt(const std::string& label, SgStatement& stmt, SgScopeStatement& encl);

  /// creates an Ada NULL statement (represented in code)
  SgNullStatement&
  mkNullStatement();

  /// creates an Ada NULL declaration (represented in code)
  SgEmptyDeclaration&
  mkNullDecl();

  /// creates an Ada Try block
  /// \note in Rose this is represented by a try statement that contains a block
  SgTryStmt&
  mkTryStmt(SgBasicBlock& blk);

  /// creates an Ada Terminate alternative statement
  SgAdaTerminateStmt&
  mkTerminateStmt();

  //
  // Declaration Makers

  /// creates a typedef declaration of type \p ty and name \p name
  ///   in parent scope \p scope.
  SgTypedefDeclaration&
  mkTypeDecl(const std::string& name, SgType& ty, SgScopeStatement& scope);

  // creates a discriminated type decl with parent scope \p scope
  ///  and non-defining declaration \p nondef_opt.
  // The child discriminatedDecl remains undefined (i.e., null).
  // \todo distinguish between defining and nondefining declarations
  SgAdaDiscriminatedTypeDecl&
  mkAdaDiscriminatedTypeDecl(SgScopeStatement& scope, SgAdaDiscriminatedTypeDecl* nondef_opt);

  /// creates a defining record declaration with name \p name for record \p def
  ///   in scope \p scope.
  ///   This function builds both the nondefining and defining declarations
  ///   and returns the defining declarations.
  SgClassDeclaration&
  mkRecordDecl(const std::string& name, SgClassDefinition& def, SgScopeStatement& scope);

  /// builds the body for a record
  SgClassDefinition&
  mkRecordBody();

  /// creates a defining record declaration for the non-defining declaration \p nondef,
  ///   and the body \p body in scope \p scope.
  SgClassDeclaration&
  mkRecordDecl(SgClassDeclaration& nondef, SgClassDefinition& body, SgScopeStatement& scope);

  /// creates a non defining record decl with name \p name
  SgClassDeclaration&
  mkRecordDecl(const std::string& name, SgScopeStatement& scope);

  /// creates an Ada package declaration
  /// \param name   name of the package
  /// \param scope  the scope of which this declaration is a part.
  ///        e.g., LLNL.Rose for a package LLNL.Rose.Ada
  /// \note
  ///    A package LLNL.Rose.Ada can be declared in the global scope. In this case
  ///    the scope of LLNL.Rose is still the parent scope.
  SgAdaPackageSpecDecl&
  mkAdaPackageSpecDecl(const std::string& name, SgScopeStatement& scope);

  /// creates an Ada generic instantiation
  /// \param name    name of the instantiation
  /// \param gendecl the underlying generic declaration (or renamed generic declaration)
  /// \param scope   the parent scope
  SgAdaGenericInstanceDecl&
  mkAdaGenericInstanceDecl(const std::string& name, SgDeclarationStatement& gendecl, SgScopeStatement& scope);


  /// creates an Ada formal package declaration
  /// \param name    name of the instantiation
  /// \param gendecl the underlying generic declaration (or renamed generic declaration)
  /// \param args    the generic arguments
  /// \param scope   the parent scope
  SgAdaFormalPackageDecl&
  mkAdaFormalPackageDecl(const std::string& name, SgDeclarationStatement& gendecl, SgExprListExp& args, SgScopeStatement& scope);

  /// creates an Ada generic declaration
  SgAdaGenericDecl&
  mkAdaGenericDecl(const std::string& name, SgScopeStatement& scope);

  /// creates an Ada renaming declaration
  /// \param name    the new name
  /// \param renamed an expression indicating a renamed expression.
  /// \param ty_opt  the type, if any of the declaration
  /// \param scope   the scope of the renaming decl
  /// \details
  ///   renamed is an expression to accomodate references to to array subsections
  ///   in addition to variables, variables, packages, exceptions, etc.
  SgAdaRenamingDecl&
  mkAdaRenamingDecl(const std::string& name, SgExpression& renamed, SgType& ty_opt, SgScopeStatement& scope);

  /// creates a nondefining Ada package body declaration.
  /// \param specdcl    the package specification
  /// \details
  ///    Used to create package body stubs. Other nondefining package body declarations
  ///    are automatically created.
  SgAdaPackageBodyDecl&
  mkAdaPackageBodyDecl_nondef(SgAdaPackageSpecDecl& specdcl, SgScopeStatement& scope);

  /// creates an Ada package body declaration
  /// \param specdcl    the package specification
  /// \param nondef_opt an optional nondefining declaration
  /// \details
  ///    if nondef_opt == nullptr, a nondefining declaration will be automatically created (
  ///    \todo THIS IS NOT YET IMPLEMENTED.
  SgAdaPackageBodyDecl&
  mkAdaPackageBodyDecl(SgAdaPackageSpecDecl& specdcl, SgAdaPackageBodyDecl* nondef_opt, SgScopeStatement& scope);

  /// creates an Ada task type declaration
  // \todo revisit Ada task symbol creation
  // \{
  SgAdaTaskTypeDecl&
  mkAdaTaskTypeDecl(const std::string& name, SgAdaTaskSpec* spec_opt, SgScopeStatement& scope);

  SgAdaTaskTypeDecl&
  mkAdaTaskTypeDecl(SgAdaTaskTypeDecl& nondef, SgAdaTaskSpec& spec, SgScopeStatement& scope);
  // \}


  /// creates an Ada task declaration
  // \todo revisit Ada task symbol creation
  SgAdaTaskSpecDecl&
  mkAdaTaskSpecDecl(const std::string& name, SgAdaTaskSpec& spec, SgScopeStatement& scope);

  /// creates an Ada task body declaration
  /// \param tskdecl     the corresponding tasl declaration which can either be of type SgAdaTaskSpecDecl
  ///                    or of type SgAdaTaskTypeDecl.
  /// \param scope       the logically enclosing scope
  SgAdaTaskBodyDecl&
  mkAdaTaskBodyDecl_nondef(SgDeclarationStatement& tskdecl, SgScopeStatement& scope);

  /// creates an Ada task body declaration
  /// \param tskdecl     the corresponding tasl declaration which can either be of type SgAdaTaskSpecDecl
  ///                    or of type SgAdaTaskTypeDecl.
  /// \param nondef_opt  optionally, an earlier nondefining declaration (separate body stub).
  /// \param tskbody     the optional task body. If no task body is specified, the declaration will be created
  ///                    as nondefining declaration (i.e., for separated body stubs)
  /// \param scope       the logically enclosing scope
  SgAdaTaskBodyDecl&
  mkAdaTaskBodyDecl( SgDeclarationStatement& tskdecl,
                     SgAdaTaskBodyDecl* nondef_opt,
                     SgAdaTaskBody& tskbody,
                     SgScopeStatement& scope
                   );

  /// creates an empty task specification definition node
  SgAdaTaskSpec&
  mkAdaTaskSpec();

  /// creates an empty task body definition node
  SgAdaTaskBody&
  mkAdaTaskBody();

  /// creates an Ada protected object type declaration
  // \todo revisit Ada protected object symbol creation
  // \{
  SgAdaProtectedTypeDecl&
  mkAdaProtectedTypeDecl(const std::string& name, SgAdaProtectedSpec* spec_opt, SgScopeStatement& scope);

  SgAdaProtectedTypeDecl&
  mkAdaProtectedTypeDecl(SgAdaProtectedTypeDecl& nondef, SgAdaProtectedSpec& spec, SgScopeStatement& scope);
  // \}

  /// creates an Ada protected object declaration
  // \todo revisit Ada protected object symbol creation
  SgAdaProtectedSpecDecl&
  mkAdaProtectedSpecDecl(const std::string& name, SgAdaProtectedSpec& spec, SgScopeStatement& scope);

  /// creates a nondefining Ada protected object body declaration
  /// \param podecl     the corresponding tasl declaration which can either be of type SgAdaProtectedSpecDecl
  ///                   or of type SgAdaProtectedTypeDecl.
  /// \param scope      the logically enclosing scope
  SgAdaProtectedBodyDecl&
  mkAdaProtectedBodyDecl_nondef(SgDeclarationStatement& podecl, SgScopeStatement& scope);

  /// creates a defining Ada protected object body declaration
  /// \param podecl     the corresponding tasl declaration which can either be of type SgAdaProtectedSpecDecl
  ///                   or of type SgAdaProtectedTypeDecl.
  /// \param nondef_opt Optionally, an earlier nondefining declaration (separate body stub).
  /// \param pobody     an optional protected body. If no body is specified, the declaration will be created
  ///                   as nondefining declaration (i.e., for separated body stubs)
  /// \param scope      the logically enclosing scope
  /// \details
  ///    \todo CREATE A NONDEFINING DECLARATION if nondef == nullptr
  SgAdaProtectedBodyDecl&
  mkAdaProtectedBodyDecl( SgDeclarationStatement& podecl,
                          SgAdaProtectedBodyDecl* nondef_opt,
                          SgAdaProtectedBody&     pobody,
                          SgScopeStatement&       scope
                        );

  /// creates an empty protected object specification definition node
  SgAdaProtectedSpec&
  mkAdaProtectedSpec();

  /// creates an empty protected object body definition node
  SgAdaProtectedBody&
  mkAdaProtectedBody();

  /// builds a fresh function parameter list
  SgFunctionParameterList&
  mkFunctionParameterList();

  /// creates a nondefining function/procedure declaration
  /// \param nm       name of the function/procedure
  /// \param scope    the enclosing scope
  /// \param retty    return type of a function (SgTypeVoid for procedures)
  /// \param complete a functor that is called after the function parameter list and
  ///                 the function parameter scope have been constructed. The task of complete
  ///                 is to fill these objects with function parameters.
  SgFunctionDeclaration&
  mkProcedureDecl_nondef( const std::string& name,
                          SgScopeStatement& scope,
                          SgType& retty,
                          std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
                        );

  /// creates a secondary function/procedure declaration
  /// \param ndef     the first nondefining declaration
  /// \param scope    the enclosing scope
  /// \param retty    return type of a function (SgTypeVoid for procedures)
  /// \param complete a functor that is called after the function parameter list and
  ///                 the function parameter scope have been constructed. The task of complete
  ///                 is to fill these objects with function parameters.
  SgFunctionDeclaration&
  mkProcedureDecl_nondef( SgFunctionDeclaration& ndef,
                          SgScopeStatement& scope,
                          SgType& retty,
                          std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
                        );

  /// creates a defining function/procedure declaration
  /// \param ndef     the first nondefining declaration
  /// \param scope    the enclosing scope
  /// \param retty    return type of a function (SgTypeVoid for procedures)
  /// \param complete a functor that is called after the function parameter list and
  ///                 the function parameter scope have been constructed. The task of complete
  ///                 is to fill these objects with function parameters.
  SgFunctionDeclaration&
  mkProcedureDecl( SgFunctionDeclaration& ndef,
                   SgScopeStatement& scope,
                   SgType& retty,
                   std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
                 );

  /// creates a defining function/procedure definition and a corresponding non-defining declaration
  /// \param nm       the function/procedure name
  /// \param scope    the enclosing scope
  /// \param retty    return type of a function (SgTypeVoid for procedures)
  /// \param complete a functor that is called after the function parameter list and
  ///                 the function parameter scope have been constructed. The task of complete
  ///                 is to fill these objects with function parameters.
  ///                 Note: Here complete is called twice, once for the defining, and once for the
  ///                       non-defining declaration.
  /// \returns the defining declaration
  SgFunctionDeclaration&
  mkProcedureDecl( const std::string& name,
                   SgScopeStatement& scope,
                   SgType& retty,
                   std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
                 );


  /// creates an Ada renaming declaration
  /// \param name       the name of this declaration
  /// \param scope      the enclosing scope
  /// \param retty      return type of a function (SgTypeVoid for procedures)
  /// \param complete   a functor that is called after the function parameter list and
  ///                   the function parameter scope have been constructed. The task of complete
  ///                   is to fill these objects with function parameters.
  /// \param nondef_opt when this is a renaming-as-body, then nondef_opt indicates the nondefining
  ///                   declaration that gets defined by this renaming.
  SgAdaFunctionRenamingDecl&
  mkAdaFunctionRenamingDecl( const std::string& name,
                             SgScopeStatement& scope,
                             SgType& retty,
                             std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete,
                             SgAdaFunctionRenamingDecl* nondef_opt = nullptr
                           );


  /// creates an Ada entry declaration
  /// \param name      the entry name
  /// \param scope     the enclosing scope
  /// \param complete  a functor that is called after the function parameter list and
  ///                  the function parameter list have been constructed. The task of complete
  ///                  is to fill these objects with function parameters.
  /// \param indexType the type of the index. If no index exists, the type shall be SgTypeVoid.
  SgAdaEntryDecl&
  mkAdaEntryDecl( const std::string& name,
                  SgScopeStatement& scope,
                  std::function<void(SgFunctionParameterList&, SgScopeStatement&)> completeParams,
                  SgType& indexType
                );

  /// creates a defining declaration for an Ada entry
  /// \param name     the entry name
  /// \param scope    the enclosing scope, where the entry gets declared
  /// \param complete a function that is called after the function parameter list and
  ///                 the function parameter list have been constructed. The task of complete
  ///                 is to fill these objects with function parameters.
  /// \param genIndex a function that generates the index variable if present. If an entry does
  ///                 not have an index variable, the generated SgInitializedName shall have
  ///                 no name, "", and the type shall be SgTypeVoid.
  SgAdaEntryDecl&
  mkAdaEntryDefn( SgAdaEntryDecl& nondef,
                  SgScopeStatement& scope,
                  std::function<void(SgFunctionParameterList&, SgScopeStatement&)> completeParams,
                  std::function<SgInitializedName&(SgScopeStatement&)> genIndex
                );


  /// creates an Ada accept statement
  /// \param ref a reference to the corresponding entry declaration
  /// \param idx the entry index
  SgAdaAcceptStmt&
  mkAdaAcceptStmt(SgExpression& ref, SgExpression& idx);

  /// creates an Ada exception handler
  SgCatchOptionStmt&
  mkExceptionHandler(SgInitializedName& parm, SgBasicBlock& body, SgTryStmt& tryStmt);

  /// creates an initialized name
  /// \param name     the variable name
  /// \param ty       the variable type
  /// \param init_opt the initial value (if any).
  SgInitializedName&
  mkInitializedName(const std::string& name, SgType& ty, SgExpression* init_opt = nullptr);

  /// creates a parameter declaration from a list of initialized names \p parms.
  /// \param parm      a list of individual parameters that get combined into a single parameter declaration
  /// \param parammode the parameter modifier (e.g., in, out)
  /// \param scope     the scope of the parameter list
  /// \pre the types of all initialized names must be the same
  /// \note in contrast to other mk* functions, mkParameter attaches the
  ///       new node to the scope (using fixVariableDeclaration).
  SgVariableDeclaration&
  mkParameter( const SgInitializedNamePtrList& parms,
               SgTypeModifier parmmode,
               SgScopeStatement& scope
             );


  /// combines a list of initialized names into a single declaration
  /// \pre the types of the initialized names must be the same
  SgVariableDeclaration&
  mkVarDecl(const SgInitializedNamePtrList& vars, SgScopeStatement& scope);

  /// creates a variable declaration with a single initialized name
  SgVariableDeclaration&
  mkVarDecl(SgInitializedName& var, SgScopeStatement& scope);

  /// creates an exception declaration
  /// \note exceptions in Ada are objects (*), in ROSE each exception is represented
  ///       as a variable of type Exception.
  ///       (*) https://learn.adacore.com/courses/intro-to-ada/chapters/exceptions.html#exception-declaration
  SgVariableDeclaration&
  mkExceptionDecl(const SgInitializedNamePtrList& vars, SgScopeStatement& scope);

  /// creates an enumerator declaration for \p enumdef
  ///   with name \p ident, type \p ty, and representative value \p repval.
  ///   A symbol for the enumerator is created in \p enumdef.get_declarationScope().
  ///   An alias symbol is created in enumdef.get_scope().
  ///   The scope of the returned value is set to enumdef.get_declarationScope().
  SgInitializedName&
  mkEnumeratorDecl(SgEnumDeclaration& enumdef, const std::string& ident, SgType& ty, SgExpression& repval);

  /// creates a SgBaseClass object for an Ada type \p n
  /// \param n a representation of the base type (often an SgClassType, but could be any type in principle.
  ///          e.g., often expression based, wrapped in an SgTypeExpression)
  /// \return  a node representing the base class (SgBaseClass when n is SgClassType, SgExpBaseClass otherwise).
  /// \note currently only direct base classes are represented in the Ast
  /// \todo should we represent base classes and other base types uniformly
  ///       e.g., BaseClassExp(TypeExpression(class_type)) ??
  SgBaseClass&
  mkRecordParent(SgType& n);

  /// creates an Ada variant declaration statement
  /// \param discr the discriminant indicating which variant is valid
  /// \return a node representing an Ada variant part of a variant record.
  SgAdaVariantDecl&
  mkAdaVariantDecl(SgExpression& discr);

  /// creates an Ada variant when statement
  /// \param choices a list of choices that indicate if the children
  ///        of the Ada variant when statement are valid.
  /// \return a node representing an Ada variant when part of a variant record.
  SgAdaVariantWhenStmt&
  mkAdaVariantWhenStmt(SgExprListExp& choices);

  /// creates an Ada component clause (part of a record representation clause)
  SgAdaComponentClause&
  mkAdaComponentClause(SgVarRefExp& field, SgExpression& offset, SgRangeExp& range);

  /// creates an Ada Record representation clause for \p record aligned at \p align.
  /// \details
  ///   the type of \p record should be either SgClassType or SgTypedefType
  SgAdaRepresentationClause&
  mkAdaRepresentationClause(SgType& record, SgExpression& align, bool isAtClause = false);

  /// creates an Ada Enum representation clause for \p enumtype and
  ///   enumerator initializations \p initlst.
  /// \pre isSgEnumType(enumtype) || isSgTypedefType(enumtype)
  /// \details
  ///   the type of \p enumtype should be either SgEnumType or SgTypedefType
  SgAdaEnumRepresentationClause&
  mkAdaEnumRepresentationClause(SgType& enumtype, SgExprListExp& initlst);

  /// creates an Ada length clause for attribute \p attr aligned and length \p size.
  SgAdaAttributeClause&
  mkAdaAttributeClause(SgAdaAttributeExp& attr, SgExpression& size);

  /// creates an Ada pragma declaration
  SgPragmaDeclaration&
  mkPragmaDeclaration(const std::string& name, SgExprListExp& args, SgStatement* assocStmt);

  /// creates a declaration scope
  /// \note
  ///    called internally in AdaMaker (\todo revisit)
  ///    AND from the translator when a pragma or aspect requires a scope for
  ///    unresolved identifiers.
  SgDeclarationScope&
  mkDeclarationScope(SgScopeStatement& /* not used */);

  //
  // Expression Makers

  /// creates a variable reference expression to the only! variable
  ///   declared within the declaration of \p var.
  SgVarRefExp&
  mkVarRefExp(SgVariableDeclaration& var);

  /// creates a variable reference expression to the variable
  ///   identified by \p var.
  SgVarRefExp&
  mkVarRefExp(SgInitializedName& var);

  /// Creates a named aggregate initializer
  /// \param what the named components that will get initialized
  /// \param the initialized value
  SgDesignatedInitializer&
  mkAdaNamedInitializer(SgExprListExp& components, SgExpression& val);

  /// creates a parent initializer for extension record aggregates
  SgAdaAncestorInitializer&
  mkAdaAncestorInitializer(SgExpression& par);

  /// creates an aggregate initializer for arrays and records
  SgAggregateInitializer&
  mkAggregateInitializer(SgExprListExp& components, SgType& resultType = mkTypeUnknown());

  /// creates an assign initializer
  SgAssignInitializer&
  mkAssignInitializer(SgExpression& val, SgType& resultType);

  /// creates an expression for an unresolved name (e.g., imported names)
  /// \note unresolved names are an indication for an incomplete AST
  /// \todo remove this function, once translation is complete
  SgExpression&
  mkUnresolvedName(const std::string& n, SgScopeStatement& scope);

  /// creates a range expression from the bounds
  /// \param start lower bound
  /// \param end   upper bound
  SgRangeExp&
  mkRangeExp(SgExpression& start, SgExpression& end);

  /// adds an empty range (with both expressions represented by SgNullExpression)
  /// \todo remove this function, once translation is complete
  SgRangeExp&
  mkRangeExp();

  /// Creates an Ada others expression (for case and expression switches)
  SgAdaOthersExp&
  mkAdaOthersExp();

  /// makes an empty if expression (aka SgConditionalExp)
  SgConditionalExp&
  mkIfExpr();

  /// Creates a new expression
  /// \param ty the type of the allocation
  /// \param args_opt an optional aggregate to initialize the type
  /// \post
  ///    result.get_type() == &ty
  SgNewExp&
  mkNewExp(SgType& ty, SgExprListExp* args_opt = nullptr);

  /// Creates an Ada box expression (indicating default value)
  SgExpression&
  mkAdaBoxExp();

  /// Creates a node that references labels
  SgLabelRefExp&
  mkLabelRefExp(const SgLabelStatement& tgt);

  /// Creates a reference to the exception object \p exception
  SgExpression&
  mkExceptionRef(SgInitializedName& exception, SgScopeStatement& scope);

  /// Creates a reference to a task \p task
  SgAdaTaskRefExp&
  mkAdaTaskRefExp(SgAdaTaskSpecDecl& task);

  /// Creates a reference to a protected object \p po
  SgAdaProtectedRefExp&
  mkAdaProtectedRefExp(SgAdaProtectedSpecDecl& task);

  /// Creates a reference to a package \p unit
  SgAdaUnitRefExp&
  mkAdaUnitRefExp(SgDeclarationStatement& unit);

  /// Creates a reference to an Ada renaming declaration \p decl
  SgAdaRenamingRefExp&
  mkAdaRenamingRefExp(SgAdaRenamingDecl& decl);

  /// creates a field selection expression for: prefix.selector
  /// \details
  ///    - when prefix is just a function name, insert a call expression.
  ///      on top of \p prefix.
  ///      This works around an Asis representation issue
  ///      ASIS_FUNCTION_REF_ISSUE_1.
  ///    - inserts a compiler-generated pointer deref expression on top
  ///      of \p prefix if \p prefix has an access type.
  SgDotExp&
  mkDotExp(SgExpression& prefix, SgExpression& selector);

  /// creates a pointer dereference expression for: e.all
  SgPointerDerefExp&
  mkPointerDerefExp(SgExpression& e);

  /// creates an array index expression for: prefix(indices).
  /// \details
  ///    inserts a compiler-generated pointer deref expression on top
  ///    of \p prefix if \p prefix has an access to array type.
  /// \note
  ///    to attain a homogeneous representation, an SgExprListExp object
  ///    is required to represent the indices, even when only a single
  ///    dimension or slice is used.
  SgPntrArrRefExp&
  mkPntrArrRefExp(SgExpression& prefix, SgExprListExp& indices);

  /// returns a combined expression representing an Ada choice
  /// \param choices a non-empty sequence of choices
  /// \return if multiple choices: a tree of expressions combined using SgCommaOpExp
  ///         otherwise (exactly one choice): the expression in \p choices
  SgExpression&
  mkChoiceExpIfNeeded(const SgExpressionPtrList& choices);

  /// creates a type conversion of expression \p expr to type \p ty.
  SgCastExp&
  mkCastExp(SgExpression& expr, SgType& ty);

  /// creates a call for: target(arglist).
  /// \details
  ///    inserts a compiler-generated pointer deref expression on top
  ///    of \p target if \p target has an access type
  ///    (\todo check for access to subroutine).
  /// \param target the expression denoting the callee
  /// \param arglist the arguments
  /// \param usesOperatorSyntax true if call should be unparsed
  ///        using operator syntax instead of function call syntax
  /// \param usesObjectCallSyntax true if call should be unparsed
  ///        using object call syntax.
  ///        i.e., x.init instead of init(x)
  SgFunctionCallExp&
  mkFunctionCallExp(SgExpression& target, SgExprListExp& arglst, bool usesOperatorSyntax = false, bool usesObjectCallSyntax = false);

  /// creates a qualified expression for \p expr and type qualification \p ty.
  /// \todo consider whether the explicit representation in code is necessary
  ///       or whether it can be reproduced by the backend.
  SgExpression&
  mkQualifiedExp(SgExpression& expr, SgType& ty);

  /// returns a representation of an Ada Attribute in expression context
  /// \param objOpt the attribute's prefix expression; a partially constructed
  ///               attribute may be created by passing in a nullptr.
  /// \param ident  the attribute identifier
  /// \param args   the attribute's arguments
  /// \example
  ///    Arr'Range(1) -> exp'ident(args)
  SgAdaAttributeExp&
  mkAdaAttributeExp(SgExpression* objOpt, const std::string& ident, SgExprListExp& args);

  /// creates an increment/decrement of the variable \p var
  /// depending on whether the loop uses forward or backward iteration.
  /// \param forward iteration direction
  /// \param var the loop variable
  /// \param scope the for loop's scope
  SgUnaryOp&
  mkForLoopIncrement(bool forward, SgVariableDeclaration& var);

  /// creates a loop test
  SgExprStatement&
  mkForLoopTest(SgVariableDeclaration& var);

  /// creates an  expression list from \p exprs
  SgExprListExp&
  mkExprListExp(const SgExpressionPtrList& exprs = {});

  /// creates an expression wrapper for type \p ty
  SgTypeExpression&
  mkTypeExpression(SgType& ty);

  /// creates an SgNullExpression
  SgNullExpression&
  mkNullExpression();

  /// builds a reference to an enumerator in form of an SgEnumValue
  SgExpression&
  mkEnumeratorRef(SgEnumDeclaration&, SgInitializedName&);

  /// creates a function ref expression
  SgFunctionRefExp&
  mkFunctionRefExp(SgFunctionDeclaration& dcl);

  /// creates and if statement
  SgIfStmt&
  mkIfStmt(bool elseIfPath = false);


  /// creates an assignment statement for: lhs := rhs;
  SgExprStatement&
  mkAssignStmt(SgExpression& lhs, SgExpression& rhs);

  /// builds a return statement for: return [expr_opt];
  SgReturnStmt&
  mkReturnStmt(SgExpression* expr_opt = nullptr);


  //
  // special Ada symbols

  /// creates a symbol for the inherited function \p fn for inherited type \p assocType.
  ///   adds the symbol to the scope \p scope of the derived type.
  /// \param basesym   the symbol from which the new symbol is derived from
  /// \param assocType the newly derived type that triggered the derivation
  /// \param scope     the scope where the new symbol needs to be inserted
  /// \note
  ///    currently derived type can either be an Ada derived type or an extension record ("class").
  /// \todo
  ///    support derived enum types
  SgAdaInheritedFunctionSymbol&
  mkAdaInheritedFunctionSymbol(SgFunctionSymbol& basesym, SgNamedType& assocType, SgScopeStatement& scope);

  //
  // conversions


  /// converts a value of type V to a value of type U via streaming
  /// \tparam  V input value type
  /// \tparam  U return value type
  /// \param   val the value to be converted
  /// \returns \p val converted to type \p U
  template <class U, class V>
  inline
  U conv(V& img)
  {
    U                 res;
    std::stringstream buf;

    buf << img;
    buf >> res;

    return res;
  }

  /// converts text to constant values
  /// \{
  template <class T>
  inline
  T convAdaLiteral(const char* img)
  {
    return conv<T>(img);
  }

  template <>
  long double convAdaLiteral<long double>(const char* img);

  template <>
  char convAdaLiteral<char>(const char* img);
  /// \}


  /// creates a value representation of type \p SageValue for the string \p textrep.
  /// \tparam SageValue the AST node type to be created
  /// \pre SageValue is derived from SgValueExp
  template <class SageValue>
  inline
  SageValue& mkValue(const char* textrep)
  {
    static_assert( std::is_base_of<SgValueExp, SageValue>::value,
                   "template argument is not derived from SgValueExp"
                 );

    using rose_rep_t = decltype(std::declval<SageValue>().get_value());

    ASSERT_not_null(textrep);
    return mkLocatedNode<SageValue>(convAdaLiteral<rose_rep_t>(textrep), textrep);
  }

  /// \overload
  /// \note specialized since SgStringVal constructor requires special handling
  template <>
  SgStringVal& mkValue<SgStringVal>(const char* textrep);

  /// creates a signed integral literal node of the appropriate size (e.g., short, int, long, long long)
  SgValueExp&
  mkAdaIntegerLiteral(const char* textrep);
} // namespace Ada_ROSE_Translation

#endif /* _ADA_MAKER_H */
