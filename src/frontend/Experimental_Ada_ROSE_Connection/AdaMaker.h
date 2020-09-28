
#ifndef _ADA_MAKER_H
#define _ADA_MAKER_H 1

///
/// A set of functions that build Sage nodes for representing Ada

#include "sage3basic.h"

#include <vector>
#include <type_traits>

#include "sageBuilder.h"
#include "sageGeneric.h"

#include "AdaUtil.h"

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

  /// creates a new node by calling new SageNode(args)
  template <class SageNode, class ... Args>
  inline
  SageNode&
  mkBareNode(Args... args)
  {
    SageNode& sgnode = SG_DEREF(new SageNode(args...));

    markCompilerGenerated(sgnode);
    return sgnode;
  }

  /// creates a new node by calling new SageNode::createType(args)
  template <class SageNode, class ... Args>
  inline
  SageNode&
  mkTypeNode(Args... args)
  {
    return SG_DEREF(SageNode::createType(args...));
  }


  //
  // Type Makers

  /// builds a range constraint to \ref range
  SgAdaRangeConstraint&
  mkAdaRangeConstraint(SgRangeExp& range);

  /// builds a subtype constraint by \ref constr
  SgAdaSubtype&
  mkAdaSubtype(SgType& superty, SgAdaTypeConstraint& constr);

  /// builds a float type with a range constraint \ref range
  SgAdaFloatType&
  mkAdaFloatType(SgExpression& digits, SgAdaRangeConstraint& range);

  /// creates an SgType node referencing \ref dcl
  // \todo is SgTypedefType the right kind?
  SgTypedefType&
  mkTypedefType(SgTypedefDeclaration& dcl);

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
  mkDefaultType();

  /// creates a type union for a list of types
  /// \note
  ///   this is used to represent a sequence of exceptions
  SgTypeTuple&
  mkTypeUnion(const std::vector<SgType*>& elemtypes);

  /// creates a type that references a record declaration \ref dcl.
  SgClassType&
  mkRecordType(SgClassDeclaration& dcl);

  /// creates a task type that references a task type declaration \ref dcl.
  SgAdaTaskType&
  mkAdaTaskType(SgAdaTaskTypeDecl& dcl);

  /// creates an entry type from a function parameter list
  // \todo the representation is incomplete and should be replaced
  //       by a new IR node SgAdaEntryType
  SgFunctionType& mkAdaEntryType(SgFunctionParameterList& lst);

  /// creates an array type with index ranges \ref indices and component
  /// type \ref comptype.
  SgArrayType& mkArrayType(SgType& comptype, SgExprListExp& indices);


  //
  // Statement Makers

  /// builds a node representing raising exception \ref raised
  SgStatement&
  mkRaiseStmt(SgExpression& raised);

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

  /// creates an Ada labeled statement.
  /// \param label the label name
  /// \param stmt  the labeled statement
  /// \param encl  the enclosing scope (req. by SageBuilder)
  SgLabelStatement&
  mkLabelStmt(const std::string& label, SgStatement& stmt, SgScopeStatement& encl);

  /// creates an Ada NULL statement (represented in code)
  SgStatement&
  mkNullStmt();

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
  SgAdaRenamingDecl&
  mkAdaRenamingDecl(const std::string& name, SgDeclarationStatement& aliased, SgScopeStatement& scope);

  /// creates an Ada package body declaration
  SgAdaPackageBodyDecl&
  mkAdaPackageBodyDecl(SgAdaPackageSpecDecl& specdcl, SgScopeStatement& scope);

  /// creates an Ada task type declaration
  // \todo revisit Ada task symbol creation
  SgAdaTaskTypeDecl&
  mkAdaTaskTypeDecl(const std::string& name, SgAdaTaskSpec& spec, SgScopeStatement& scope);

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
  mkAdaTaskBodyDecl(std::string name, SgAdaTaskBody& tskbody, SgScopeStatement& scope);

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
  ///                 the function parameter list have been constructed. The task of complete
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
  ///                 the function parameter list have been constructed. The task of complete
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
  ///                 the function parameter list have been constructed. The task of complete
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

  //
  // Expression Makers

  /// creates an expression for an unresolved name (e.g., imported names)
  SgExpression&
  mkUnresolvedName(std::string n, SgScopeStatement& scope);

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

  /// creates a field selection expression (expr.field)
  SgDotExp&
  mkSelectedComponent(SgExpression& prefix, SgExpression& selector);

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
    static_assert( std::is_base_of<SgValueExp, SageValue>::value ,
                   "template argument is not derived from SgValueExp"
                 );

    typedef decltype(std::declval<SageValue>().get_value()) rose_rep_t;

    return mkBareNode<SageValue>(conv<rose_rep_t>(textrep), textrep);
  }
} // namespace Ada_ROSE_Translation

#endif /* _ADA_MAKER_H */


