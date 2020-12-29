
#ifndef _ADA_MAKER_H
#define _ADA_MAKER_H 1

///
/// A set of functions that build Sage nodes for representing Ada

//~ #include "sage3basic.h"

#include <vector>
#include <type_traits>

#include "sageBuilder.h"
#include "sageGeneric.h"

#include "Ada_to_ROSE.h"

namespace Ada_ROSE_Translation
{
  //
  // file info objects

  /// creates a default file info object for compiler generated nodes
  // \todo currently generateDefaultFileInfoForTransformationNode is used
  //       -> replace with info object that indicates compiler generated.
  Sg_File_Info& mkFileInfo();

  /// creates a file info object for file location given by \ref file,
  /// \ref line, and \ref col
  Sg_File_Info& mkFileInfo(const std::string& file, int line, int col);

  /// sets three file info objects, file_info, startOfConstruct, and
  ///   endOfConstruct to compiler generated.
  void markCompilerGenerated(SgLocatedNode& n);

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

    markCompilerGenerated(sgnode);
    return sgnode;
  }


  //
  // Type Makers

  /// builds a range constraint from \ref range
  SgAdaRangeConstraint&
  mkAdaRangeConstraint(SgRangeExp& range);

  /// builds an index constraint from \ref ranges
  /// \param ranges a sequence of ranges.
  ///        the content of the sequence container \ref ranges will be chnage.
  SgAdaIndexConstraint&
  mkAdaIndexConstraint(SgRangeExpPtrList&& ranges);

  /// builds a subtype constraint by \ref constr
  SgAdaSubtype&
  mkAdaSubtype(SgType& superty, SgAdaTypeConstraint& constr);

  /// builds a modular integral type with mod expression \ref modexpr.
  SgAdaModularType&
  mkAdaModularType(SgExpression& modexpr);

  /// builds a float type with an optional range constraint \ref range_opt
  SgAdaFloatType&
  mkAdaFloatType(SgExpression& digits, SgAdaRangeConstraint* range_opt);

  /// returns the type of an exception
  // \todo revise Exception representation
  //   Exceptions are currently represented in the following way
  //   A type "Exception" exists, and any declared exception becomes a variable
  //   declaration that has type exception. The type of these declared exceptions
  //   are represented as decl type of a variable reference.
  SgDeclType&
  mkExceptionType(SgExpression& n);

  /// returns a default type, used to represent an opaque declaration
  SgTypeDefault&
  mkOpaqueType();

  /// creates a type union for a list of types
  /// \note
  ///   this is used to represent a sequence of exceptions
  SgTypeTuple&
  mkTypeUnion(const std::vector<SgType*>& elemtypes);

  /// creates a type that references a record declaration \ref dcl.
  SgClassType&
  mkRecordType(SgClassDeclaration& dcl);

  /// creates an enumeration with name \ref name in scope \ref scope.
  SgEnumDeclaration&
  mkEnumDecl(const std::string& name, SgScopeStatement& scope);

  /// creates a task type that references a task type declaration \ref dcl.
  SgAdaTaskType&
  mkAdaTaskType(SgAdaTaskTypeDecl& dcl);

  /// creates an entry type from a function parameter list
  // \todo the representation is incomplete and should be replaced
  //       by a new IR node SgAdaEntryType
  SgFunctionType& mkAdaEntryType(SgFunctionParameterList& lst);

  /// creates an array type with index ranges \ref indices and component
  /// type \ref comptype.
  /// \param compType the component type
  /// \param dimInfo the dimension information (sets dim_info property)
  /// \param variableLength is true for unconstrained arrays, false otherwise
  ///        (sets is_variable_length_array property)
  SgArrayType& mkArrayType(SgType& comptype, SgExprListExp& dimInfo, bool variableLength = false);

  /// creates the most general integral type
  SgType& mkIntegralType();

  /// creates the most general real type
  SgType& mkRealType();

  //
  // Statement Makers

  /// builds a node representing raising exception \ref raised
  SgStatement&
  mkRaiseStmt(SgExpression& raised);

  /// builds a node representing raising exception \ref raised with message \ref what
  SgStatement&
  mkRaiseStmt(SgExpression& raised, SgExpression& what);

  /// creates a basic block
  SgBasicBlock&
  mkBasicBlock();

  /// creates a while statement with condition \ref cond and body \ref body.
  SgWhileStmt&
  mkWhileStmt(SgExpression& cond, SgBasicBlock& body);

  /// creates a loop statement with body \ref body.
  SgAdaLoopStmt&
  mkLoopStmt(SgBasicBlock& body);

  /// creates a for loop statement with body \ref body and an *empty*
  ///   loop header.
  SgForStatement&
  mkForStatement(SgBasicBlock& body);

  /// creates a with clause for imported elements
  // \todo revisit the ASIS frontend representation and revise how
  //       imports are represented in the AST.
  SgImportStatement&
  mkWithClause(const std::vector<SgExpression*>& imported);

  /// creates a use declaration for "use packagename" declarations
  /// \todo revisit representation in ROSE (use package seems more similar to using dircetive)
  /// \param used the used declaration
  SgUsingDeclarationStatement&
  mkUseClause(SgDeclarationStatement& used);

  /// creates an exit statement from loop \ref loop with condition \ref condition.
  ///   \ref explicitLoopName is set if the loop was named and the loop name
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

  /// builds a when others path with a body of \ref blk
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

  /// creates an Ada labeled statement.
  /// \param label the label name
  /// \param stmt  the labeled statement
  /// \param encl  the enclosing scope (req. by SageBuilder)
  SgLabelStatement&
  mkLabelStmt(const std::string& label, SgStatement& stmt, SgScopeStatement& encl);

  /// creates an Ada NULL statement (represented in code)
  SgNullStatement&
  mkNullStmt();

  /// creates an Ada NULL declaration (represented in code)
  SgEmptyDeclaration&
  mkNullDecl(SgScopeStatement& scope);

  /// creates an Ada Try block
  /// \note in Rose this is represented by a try statement that contains a block
  SgTryStmt&
  mkTryStmt(SgBasicBlock& blk);


  //
  // Declaration Makers

  /// creates a typedef declaration of type \ref ty and name \ref name
  ///   in parent scope \ref scope.
  SgTypedefDeclaration&
  mkTypeDecl(const std::string& name, SgType& ty, SgScopeStatement& scope);

  /// creates a record declaration with name \ref name for record \ref def
  ///   in scope \ref scope.
  ///   This function builds both the nondefining and defining declarations
  ///   and returns the defining declarations.
  // \todo check with Dan
  SgClassDeclaration&
  mkRecordDecl(const std::string& name, SgClassDefinition& def, SgScopeStatement& scope);

  /// creates an Ada package declaration
  SgAdaPackageSpecDecl&
  mkAdaPackageSpecDecl(const std::string& name, SgScopeStatement& scope);

  /// creates an Ada renaming declaration
  /// \param name    the new name
  /// \param aliased the aliased declaration
  /// \param scope   the scope of the renaming decl
  /// \note the idx is assumed to be 0.
  SgAdaRenamingDecl&
  mkAdaRenamingDecl(const std::string& name, SgDeclarationStatement& aliased, SgScopeStatement& scope);

  /// creates an Ada renaming declaration
  /// \param name    the new name
  /// \param aliased the aliased initialized name
  /// \param scope   the scope of the renaming decl
  SgAdaRenamingDecl&
  mkAdaRenamingDecl(const std::string& name, SgInitializedName& ini, SgScopeStatement& scope);

  /// creates an Ada package body declaration
  SgAdaPackageBodyDecl&
  mkAdaPackageBodyDecl(SgAdaPackageSpecDecl& specdcl, SgScopeStatement& scope);

  /// creates an Ada task type declaration
  // \todo revisit Ada task symbol creation
  SgAdaTaskTypeDecl&
  mkAdaTaskTypeDecl(const std::string& name, SgAdaTaskSpec& spec, SgScopeStatement& scope);

  /// creates an Ada task declaration
  // \todo revisit Ada task symbol creation
  SgAdaTaskSpecDecl&
  mkAdaTaskSpecDecl(const std::string& name, SgAdaTaskSpec& spec, SgScopeStatement& scope);

  /// creates an Ada task body declaration
  /// \param tskdecl the corresponding tasl declaration which can either be of type SgAdaTaskSpecDecl
  ///                or of type SgAdaTaskTypeDecl.
  /// \param tskbody the task body
  /// \param scope   the enclosing scope
  SgAdaTaskBodyDecl&
  mkAdaTaskBodyDecl(SgDeclarationStatement& tskdecl, SgAdaTaskBody& tskbody, SgScopeStatement& scope);

  /// creates an independent task body with name \ref name, body \ref body, in scope \ref scope.
  // \todo not sure why a task body can independently exist without prior declaration.
  //       maybe this function is not needed.
  SgAdaTaskBodyDecl&
  mkAdaTaskBodyDecl(const std::string& name, SgAdaTaskBody& tskbody, SgScopeStatement& scope);

  /// creates an empty task specification definition node
  SgAdaTaskSpec&
  mkAdaTaskSpec();

  /// creates an empty task body definition node
  SgAdaTaskBody&
  mkAdaTaskBody();

  /// builds a fresh function parameter list
  SgFunctionParameterList&
  mkFunctionParameterList();

  /// creates a function/procedure declaration
  /// \param nm       name of the function/procedure
  /// \param scope    the enclosing scope
  /// \param retty    return type of a function (SgVoidType for procedures)
  /// \param complete a functor that is called after the function parameter list and
  ///                 the function parameter scope have been constructed. The task of complete
  ///                 is to fill these objects with function parameters.
  SgFunctionDeclaration&
  mkProcedure( const std::string& name,
               SgScopeStatement& scope,
               SgType& retty,
               std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
             );

  /// creates a function/procedure declaration
  /// \param ndef     the non-defining declaration
  /// \param scope    the enclosing scope
  /// \param retty    return type of a function (SgVoidType for procedures)
  /// \param complete a functor that is called after the function parameter list and
  ///                 the function parameter scope have been constructed. The task of complete
  ///                 is to fill these objects with function parameters.
  SgFunctionDeclaration&
  mkProcedureDef( SgFunctionDeclaration& ndef,
                  SgScopeStatement& scope,
                  SgType& retty,
                  std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
                );

  /// creates a function/procedure definition and a corresponding non-defining declaration
  /// \param nm       the function/procedure name
  /// \param scope    the enclosing scope
  /// \param retty    return type of a function (SgVoidType for procedures)
  /// \param complete a functor that is called after the function parameter list and
  ///                 the function parameter scope have been constructed. The task of complete
  ///                 is to fill these objects with function parameters.
  ///                 Note: Here complete is called twice, once for the defining, and once for the
  ///                       non-defining declaration.
  /// \returns the defining declaration
  SgFunctionDeclaration&
  mkProcedureDef( const std::string& name,
                  SgScopeStatement& scope,
                  SgType& retty,
                  std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
                );


  /// creates an Ada entry declaration
  /// \param name     the entry name
  /// \param scope    the enclosing scope
  /// \param complete a functor that is called after the function parameter list and
  ///                 the function parameter list have been constructed. The task of complete
  ///                 is to fill these objects with function parameters.
  SgAdaEntryDecl&
  mkAdaEntryDecl( const std::string& name,
                  SgScopeStatement& scope,
                  std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
                );

  /// creates an Ada accept statement
  SgAdaAcceptStmt&
  mkAdaAcceptStmt(SgExpression& ref, SgExpression& idx);

  /// creates an Ada exception handler
  SgCatchOptionStmt&
  mkExceptionHandler(SgInitializedName& parm, SgBasicBlock& body);

  /// creates an initialized name
  /// \param name the variable name
  /// \param ty   the variable type
  /// \param init the initializer
  SgInitializedName&
  mkInitializedName(const std::string& name, SgType& ty, SgExpression* init);

  /// creates a parameter declaration from a list of initialized names \ref parms.
  /// \param parm      a list of individual parameters that get combined into a single parameter declaration
  /// \param parammode the parameter modifier (e.g., in, out)
  /// \param scope     the scope of the parameter list
  /// \pre the types of all initialized names must be the same
  SgVariableDeclaration&
  mkParameter( const std::vector<SgInitializedName*>& parms,
               SgTypeModifier parmmode,
               SgScopeStatement& scope
             );


  /// combines a list of initialized names into a single declaration
  /// \pre the types of the initialized names must be the same
  SgVariableDeclaration&
  mkVarDecl(const std::vector<SgInitializedName*>& vars, SgScopeStatement& scope);

  /// creates a variable declaration with a single initialized name
  SgVariableDeclaration&
  mkVarDecl(SgInitializedName& var, SgScopeStatement& scope);

  /// creates an exception declaration
  /// \note exceptions in Ada are names, in ROSE each exception is represented
  ///       as a variable of type Exception
  /// \todo revisit exception representation
  SgVariableDeclaration&
  mkExceptionDecl(const std::vector<SgInitializedName*>& vars, SgScopeStatement& scope);

  /// creates a SgBaseClass object for an Ada record's parent
  /// \todo currently only direct base classes are represented in the Ast
  SgBaseClass&
  mkRecordParent(SgClassDeclaration& n);

  /// creates an Ada component clause (part of a record representation clause)
  SgAdaComponentClause&
  mkAdaComponentClause(SgVarRefExp& field, SgExpression& offset, SgRangeExp& range);

  /// creates an Ada Record represtation clause for \ref record aligned at \ref align.
  SgAdaRecordRepresentationClause&
  mkAdaRecordRepresentationClause(SgClassType& record, SgExpression& align);

  //
  // Expression Makers

  /// Creates a named aggregate initializer
  /// \param what the named components that will get initialized
  /// \param the initialized value
  SgDesignatedInitializer&
  mkAdaNamedInitializer(SgExprListExp& components, SgExpression& val);

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
  /// \note currently others is represented by SgNullExpression
  /// \todo consider introducing an SgAdaOthersExp node
  SgExpression&
  mkOthersExp();

  /// Creates a reference to the "exception type"
  /// \todo revisit exception representation
  SgExpression&
  mkExceptionRef(SgInitializedName& exception, SgScopeStatement& scope);

  /// Creates a reference to the "exception type"
  /// \todo revisit exception representation
  SgAdaTaskRefExp&
  mkAdaTaskRefExp(SgAdaTaskSpecDecl& task);

  /// creates a field selection expression (expr.field)
  SgDotExp&
  mkSelectedComponent(SgExpression& prefix, SgExpression& selector);

  /// returns a combined expression representing an Ada choice
  /// \param choices a non-empty sequence of choices
  /// \return if multiple choices: a tree of expressions combined using SgCommaOpExp
  ///         otherwise (exactly one choice): the expression in \ref choices
  SgExpression& mkChoiceExpIfNeeded(std::vector<SgExpression*>&& choices);

  /// returns a representation of an Ada Attribute in expression context
  /// \param exp the attribute's prefix expression
  /// \param ident the attribute identifier
  /// \param args the attribute's arguments
  /// \example
  ///    Arr'Range(1) -> exp'ident(args)
  SgTypeTraitBuiltinOperator&
  mkAdaExprAttribute(SgExpression& exp, const std::string& ident, SgExprListExp& args);

  /// creates an increment/decrement of the variable \ref var
  /// depending on whether the loop uses forward or backward iteration.
  /// \param forward iteration direction
  /// \param var the loop variable
  /// \param scope the for loop's scope
  SgUnaryOp&
  mkForLoopIncrement(bool forward, SgVariableDeclaration& var);

  /// creates a remainder operation (different from SgModOp)
  /// \todo move to SageBuilder
  ///       should SgRemOp be called SgAdaRem?
  SgRemOp*
  buildRemOp(SgExpression* lhs, SgExpression* rhs);

  /// creates an abs function
  /// \todo move to SageBuilder
  ///       should SgAbsOp be called SgAdaAbs?
  SgAbsOp*
  buildAbsOp(SgExpression* op);

  /// creates a value representation of type \ref SageValue for the string \ref textrep.
  /// \tparam SageValue the AST node type to be created
  /// \pre SageValue is derived from SgValueExp
  template <class SageValue>
  inline
  SageValue& mkValue(const char* textrep)
  {
    static_assert( std::is_base_of<SgValueExp, SageValue>::value,
                   "template argument is not derived from SgValueExp"
                 );

    typedef decltype(std::declval<SageValue>().get_value()) rose_rep_t;

    ROSE_ASSERT(textrep);
    return mkLocatedNode<SageValue>(conv<rose_rep_t>(textrep), textrep);
  }

  /// \overload
  /// \note specialized since SgStringVal constructor requires special handling
  template <>
  SgStringVal& mkValue<SgStringVal>(const char* textrep);
} // namespace Ada_ROSE_Translation

#endif /* _ADA_MAKER_H */


