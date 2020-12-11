
#include "sage3basic.h"

#include <algorithm>
#include <numeric>

#include "AdaMaker.h"

#include "Ada_to_ROSE.h"

// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"


namespace sb = SageBuilder;
namespace si = SageInterface;



namespace Ada_ROSE_Translation
{

// anonymous namespace for auxiliary functions
namespace
{
  /// creates a new node by calling new SageNode::createType(args)
  template <class SageNode, class ... Args>
  inline
  SageNode&
  mkTypeNode(Args... args)
  {
    return SG_DEREF(SageNode::createType(args...));
  }

  /// creates fresh type nodes since the containing elements (e.g., expressions)
  ///   cannot be properly unified (
  template <class SageNode, class ... Args>
  inline
  SageNode&
  mkNonSharedTypeNode(Args... args)
  {
    return mkBareNode<SageNode>(args...);
  }

  /// \private
  /// sets the symbol defining decl
  void linkDecls(SgFunctionSymbol& funcsy, SgFunctionDeclaration& func)
  {
    SgFunctionDeclaration& sdcl = SG_DEREF(funcsy.get_declaration());

    // defining and first non-defining must differ
    ROSE_ASSERT(&sdcl != &func);

    sdcl.set_definingDeclaration(&func);
    func.set_firstNondefiningDeclaration(&sdcl);
  }
}


//
// file info related functions

Sg_File_Info& mkFileInfo()
{
  return SG_DEREF( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
}

Sg_File_Info& mkFileInfo(const std::string& file, int line, int col)
{
  Sg_File_Info& sgnode = mkBareNode<Sg_File_Info>(file, line, col);

  sgnode.setOutputInCodeGeneration();
  return sgnode;
}

#if NOT_USED
Sg_File_Info& mkFileInfo(const Sg_File_Info& orig)
{
  return mkBareNode<Sg_File_Info>(orig);
}

void copyFileInfo(const SgLocatedNode& src, SgLocatedNode& tgt)
{
  tgt.set_file_info       (&mkFileInfo(SG_DEREF(src.get_file_info())));
  tgt.set_startOfConstruct(&mkFileInfo(SG_DEREF(src.get_startOfConstruct())));
  tgt.set_endOfConstruct  (&mkFileInfo(SG_DEREF(src.get_endOfConstruct())));
}
#endif /* NOT_USED */


void markCompilerGenerated(SgLocatedNode& n)
{
  // \todo can the file info object be shared
  n.set_file_info       (&mkFileInfo());
  n.set_startOfConstruct(&mkFileInfo());
  n.set_endOfConstruct  (&mkFileInfo());
}


//
// types

SgAdaRangeConstraint&
mkAdaRangeConstraint(SgRangeExp& range)
{
  return mkBareNode<SgAdaRangeConstraint>(&range);
}

SgAdaIndexConstraint&
mkAdaIndexConstraint(SgRangeExpPtrList&& ranges)
{
  SgAdaIndexConstraint& sgnode = mkBareNode<SgAdaIndexConstraint>();

  sgnode.get_indexRanges().swap(ranges);
  // \todo shall the range pointers' parent point to sgnode?
  return sgnode;
}


SgAdaSubtype&
mkAdaSubtype(SgType& superty, SgAdaTypeConstraint& constr)
{
  return mkNonSharedTypeNode<SgAdaSubtype>(&superty, &constr);
}

SgAdaModularType&
mkAdaModularType(SgExpression& modexpr)
{
  return mkNonSharedTypeNode<SgAdaModularType>(&modexpr);
}

SgAdaFloatType&
mkAdaFloatType(SgExpression& digits, SgAdaRangeConstraint* range_opt)
{
  return mkNonSharedTypeNode<SgAdaFloatType>(&digits, range_opt);
}

SgDeclType&
mkExceptionType(SgExpression& n)
{
  return mkNonSharedTypeNode<SgDeclType>(&n);
}


SgTypeDefault&
mkOpaqueType()
{
  return mkTypeNode<SgTypeDefault>();
}

SgTypeTuple&
mkTypeUnion(const std::vector<SgType*>& elemtypes)
{
  SgTypeTuple&   sgnode = mkNonSharedTypeNode<SgTypeTuple>();
  SgTypePtrList& lst    = sgnode.get_types();

  lst.insert(lst.end(), elemtypes.begin(), elemtypes.end());
  return sgnode;
}

SgClassType&
mkRecordType(SgClassDeclaration& dcl)
{
  return mkTypeNode<SgClassType>(&dcl);
}

SgEnumDeclaration&
mkEnumDecl(const std::string& name, SgScopeStatement& scope)
{
  return SG_DEREF(sb::buildEnumDeclaration(name, &scope));
}

SgAdaTaskType&
mkAdaTaskType(SgAdaTaskTypeDecl& dcl)
{
  SgAdaTaskType& sgnode = mkTypeNode<SgAdaTaskType>(&dcl);

  sgnode.set_decl(&dcl);
  return sgnode;
}

SgFunctionType& mkAdaEntryType(SgFunctionParameterList& lst)
{
  return SG_DEREF(sb::buildFunctionType(sb::buildVoidType(), &lst));
}

SgArrayType& mkArrayType(SgType& comptype, SgExprListExp& dimInfo, bool variableLength)
{
  // in Ada, dim_info is used for dimensions, since it can directly represent multi-dimensional arrays
  SgArrayType& sgnode = SG_DEREF(sb::buildArrayType(&comptype, &dimInfo));

  sgnode.set_is_variable_length_array(variableLength);
  //~ dimInfo.set_parent(&sgnode);
  return sgnode;
}

SgType& mkIntegralType()
{
  return SG_DEREF(sb::buildLongLongType());
}

SgType& mkRealType()
{
  return SG_DEREF(sb::buildLongDoubleType());
}


//
// Statements

SgStatement&
mkRaiseStmt(SgExpression& raised)
{
  SgExpression&    raiseop = SG_DEREF( sb::buildThrowOp(&raised, SgThrowOp::throw_expression ) );
  SgExprStatement& sgnode  = SG_DEREF( sb::buildExprStatement(&raiseop) );

  markCompilerGenerated(raiseop);
  return sgnode;
}

SgStatement&
mkRaiseStmt(SgExpression& raised, SgExpression& what)
{
  SgExprListExp& duo = SG_DEREF( sb::buildExprListExp(&raised, &what) );

  markCompilerGenerated(duo);
  return mkRaiseStmt(duo);
}


SgBasicBlock&
mkBasicBlock()
{
  SgBasicBlock& sgnode = SG_DEREF( sb::buildBasicBlock() );

  markCompilerGenerated(sgnode);
  return sgnode;
}

SgWhileStmt&
mkWhileStmt(SgExpression& cond, SgBasicBlock& body)
{
  return SG_DEREF( sb::buildWhileStmt(&cond, &body) );
}

SgAdaLoopStmt&
mkLoopStmt(SgBasicBlock& body)
{
  SgAdaLoopStmt& sgnode = mkBareNode<SgAdaLoopStmt>();

  sg::linkParentChild(sgnode, body, &SgAdaLoopStmt::set_body);
  return sgnode;
}

SgForStatement&
mkForStatement(SgBasicBlock& body)
{
  SgStatement&    test   = SG_DEREF( sb::buildNullStatement() );
  SgExpression&   incr   = SG_DEREF( sb::buildNullExpression() );
  SgForStatement& sgnode = SG_DEREF( sb::buildForStatement(nullptr, &test, &incr, &body) );

  return sgnode;
}

SgImportStatement&
mkWithClause(const std::vector<SgExpression*>& imported)
{
  SgImportStatement&   sgnode = mkBareNode<SgImportStatement>(&mkFileInfo());
  SgExpressionPtrList& lst    = sgnode.get_import_list();

  lst.insert(lst.end(), imported.begin(), imported.end());
  return sgnode;
}

SgUsingDeclarationStatement&
mkUseClause(SgDeclarationStatement& used)
{
  SgUsingDeclarationStatement& sgnode = mkBareNode<SgUsingDeclarationStatement>(&used, nullptr);

  return sgnode;
}


SgAdaExitStmt&
mkAdaExitStmt(SgStatement& loop, SgExpression& cond, bool explicitLoopName)
{
  SgAdaExitStmt& sgnode = mkBareNode<SgAdaExitStmt>(&loop, &cond, explicitLoopName);

  cond.set_parent(&sgnode);
  return sgnode;
}

SgSwitchStatement&
mkAdaCaseStmt(SgExpression& selector, SgBasicBlock& body)
{
  return SG_DEREF(sb::buildSwitchStatement(&selector, &body));
}

SgCaseOptionStmt&
mkWhenPath(SgExpression& cond, SgBasicBlock& blk)
{
  SgCaseOptionStmt& sgnode = SG_DEREF(sb::buildCaseOptionStmt(&cond, &blk));

  sgnode.set_has_fall_through(false);
  return sgnode;
}

SgDefaultOptionStmt&
mkWhenOthersPath(SgBasicBlock& blk)
{
  SgDefaultOptionStmt& sgnode = SG_DEREF(sb::buildDefaultOptionStmt(&blk));

  sgnode.set_has_fall_through(false);
  return sgnode;
}


SgAdaDelayStmt&
mkAdaDelayStmt(SgExpression& timeExp, bool relativeTime)
{
  SgAdaDelayStmt& sgnode = mkBareNode<SgAdaDelayStmt>(&timeExp, relativeTime);

  sg::linkParentChild(sgnode, timeExp, &SgAdaDelayStmt::set_time);
  return sgnode;
}

SgProcessControlStatement&
mkAbortStmt(SgExprListExp& abortList)
{
  SgProcessControlStatement& sgnode = mkBareNode<SgProcessControlStatement>(&abortList);

  abortList.set_parent(&sgnode);
  sgnode.set_control_kind(SgProcessControlStatement::e_abort);
  return sgnode;
}


SgLabelStatement&
mkLabelStmt(const std::string& label, SgStatement& stmt, SgScopeStatement& encl)
{
  SgLabelStatement& sgnode = SG_DEREF( sb::buildLabelStatement(label, &stmt, &encl) );

  sg::linkParentChild(sgnode, stmt, &SgLabelStatement::set_statement);
  return sgnode;
}

SgNullStatement&
mkNullStmt()
{
  return SG_DEREF(sb::buildNullStatement());
}

SgEmptyDeclaration&
mkNullDecl(SgScopeStatement& encl)
{
  SgEmptyDeclaration& sgnode = SG_DEREF(sb::buildEmptyDeclaration());

  sg::linkParentChild(encl, as<SgStatement>(sgnode), &SgScopeStatement::append_statement);
  return sgnode;
}

SgTryStmt&
mkTryStmt(SgBasicBlock& blk)
{
  SgTryStmt& sgnode = SG_DEREF(sb::buildTryStmt(&blk));

  markCompilerGenerated(SG_DEREF(sgnode.get_catch_statement_seq_root()));
  markCompilerGenerated(sgnode);
  return sgnode;
}


//
// declarations

SgTypedefDeclaration&
mkTypeDecl(const std::string& name, SgType& ty, SgScopeStatement& scope)
{
  SgTypedefDeclaration& sgnode = SG_DEREF( sb::buildTypedefDeclaration(name, &ty, &scope) );

  return sgnode;
}

SgClassDeclaration&
mkRecordDecl(const std::string& name, SgClassDefinition& def, SgScopeStatement& scope)
{
  SgClassDeclaration& nondef = SG_DEREF( sb::buildNondefiningClassDeclaration_nfi( name,
                                                                                   SgClassDeclaration::e_struct,
                                                                                   &scope,
                                                                                   false /* template instance */,
                                                                                   nullptr /* template parameter list */
                                                                                 ));

  SgClassDeclaration& sgnode = SG_DEREF( sb::buildNondefiningClassDeclaration_nfi( name,
                                                                                   SgClassDeclaration::e_struct,
                                                                                   &scope,
                                                                                   false /* template instance */,
                                                                                   nullptr /* template parameter list */
                                                                                 ));

  sg::linkParentChild(sgnode, def, &SgClassDeclaration::set_definition);
  sgnode.unsetForward();
  sgnode.set_definingDeclaration(&sgnode);
  nondef.set_definingDeclaration(&sgnode);
  sgnode.set_firstNondefiningDeclaration(&nondef);
  nondef.set_firstNondefiningDeclaration(&nondef);
  return sgnode;
}

SgAdaPackageSpecDecl&
mkAdaPackageSpecDecl(const std::string& name, SgScopeStatement& scope)
{
  SgAdaPackageSpec&     pkgdef = mkLocatedNode<SgAdaPackageSpec>();
  SgAdaPackageSpecDecl& sgnode = mkLocatedNode<SgAdaPackageSpecDecl>(name, nullptr);

  sgnode.set_parent(&scope);
  sgnode.set_firstNondefiningDeclaration(&sgnode);

  sg::linkParentChild(sgnode, pkgdef, &SgAdaPackageSpecDecl::set_definition);

  // add the symbol to the table
  scope.insert_symbol(name, &mkBareNode<SgAdaPackageSymbol>(&sgnode));
  return sgnode;
}

namespace
{
  SgAdaRenamingDecl&
  mkAdaRenamingDeclInternal(const std::string& name, SgDeclarationStatement& dcl, size_t dclIdx, SgScopeStatement& scope)
  {
    SgAdaRenamingDecl& sgnode = mkLocatedNode<SgAdaRenamingDecl>(name, &dcl, dclIdx);

    sgnode.set_parent(&scope);
    sgnode.set_firstNondefiningDeclaration(&sgnode);
    //~ scope.insert_symbol(name, new SgAdaPackageSymbol(&sgnode));
    return sgnode;
  }
}

SgAdaRenamingDecl&
mkAdaRenamingDecl(const std::string& name, SgDeclarationStatement& dcl, SgScopeStatement& scope)
{
  // \todo test that aliased has exactly one declaration
  return mkAdaRenamingDeclInternal(name, dcl, 0, scope);
}

SgAdaRenamingDecl&
mkAdaRenamingDecl(const std::string& name, SgInitializedName& ini, SgScopeStatement& scope)
{
  typedef SgInitializedNamePtrList::iterator Iterator;

  SgVariableDeclaration&    var = sg::ancestor<SgVariableDeclaration>(ini);
  SgInitializedNamePtrList& lst = var.get_variables();
  Iterator                  aa  = lst.begin();
  const size_t              idx = std::distance(aa, std::find(aa, lst.end(), &ini));

  ROSE_ASSERT(idx < lst.size());
  return mkAdaRenamingDeclInternal(name, sg::ancestor<SgVariableDeclaration>(ini), idx, scope);
}


SgAdaPackageBodyDecl&
mkAdaPackageBodyDecl(SgAdaPackageSpecDecl& specdcl, SgScopeStatement& scope)
{
  SgAdaPackageBody&     pkgbody = mkLocatedNode<SgAdaPackageBody>();
  SgAdaPackageBodyDecl& sgnode  = mkLocatedNode<SgAdaPackageBodyDecl>(specdcl.get_name(), &pkgbody);

  pkgbody.set_parent(&sgnode);
  sgnode.set_parent(&scope);

  SgAdaPackageSpec&     pkgspec = SG_DEREF( specdcl.get_definition() );

  pkgspec.set_body(&pkgbody);
  pkgbody.set_spec(&pkgspec);

  // \todo make sure assertion holds
  // ROSE_ASSERT(scope.symbol_exists(specdcl.get_name()));
/*
  if (!scope.symbol_exists(specdcl.get_name()))
    scope.insert_symbol(specdcl.get_name(), new SgAdaPackageSymbol(&sgnode));
*/

/*
  nsdcl.set_parent(&scope);
  sg::linkParentChild(nsdcl, nsdef, &SgNamespaceDeclarationStatement::set_definition);

  // set if first definition
  if (nsdef.get_global_definition() == nullptr)
  {
    SgNamespaceDefinitionStatement&  globdef = SG_DEREF(sb::buildNamespaceDefinition(&nsdcl));

    globdef.set_parent(&scope);
    globdef.set_global_definition(&globdef);
    nsdef.set_global_definition(&globdef);
  }
*/
  return sgnode;
}


namespace
{
  template <class SagaAdaTaskDecl>
  SagaAdaTaskDecl&
  mkAdaTaskDeclInternal(const std::string& name, SgAdaTaskSpec& spec, SgScopeStatement& scope)
  {
    SagaAdaTaskDecl& sgnode = mkLocatedNode<SagaAdaTaskDecl>(name, &spec);

    scope.insert_symbol(name, &mkBareNode<SgAdaTaskSymbol>(&sgnode));

    spec.set_parent(&sgnode);
    return sgnode;
  }
}


SgAdaTaskTypeDecl&
mkAdaTaskTypeDecl(const std::string& name, SgAdaTaskSpec& spec, SgScopeStatement& scope)
{
  return mkAdaTaskDeclInternal<SgAdaTaskTypeDecl>(name, spec, scope);
}

SgAdaTaskSpecDecl&
mkAdaTaskSpecDecl(const std::string& name, SgAdaTaskSpec& spec, SgScopeStatement& scope)
{
  return mkAdaTaskDeclInternal<SgAdaTaskSpecDecl>(name, spec, scope);
}

namespace
{
  struct TaskDeclInfo
  {
    std::string    name;
    SgAdaTaskSpec* spec;
  };

  struct ExtractTaskDeclinfo : sg::DispatchHandler<TaskDeclInfo>
  {
    template <class SageTaskDecl>
    void handleTaskDecl(SageTaskDecl& n)
    {
      res.name = n.get_name();
      res.spec = n.get_definition();
    }

    void handle(SgNode& n)            { SG_UNEXPECTED_NODE(n); }
    void handle(SgAdaTaskSpecDecl& n) { handleTaskDecl(n); }
    void handle(SgAdaTaskTypeDecl& n) { handleTaskDecl(n); }
  };
} // anonymous namespace

SgAdaTaskBodyDecl&
mkAdaTaskBodyDecl(SgDeclarationStatement& tskdecl, SgAdaTaskBody& tskbody, SgScopeStatement& scope)
{
  //~ SgAdaPackageBody&     pkgbody = SG_DEREF( new SgAdaPackageBody() );
  TaskDeclInfo       specinfo = sg::dispatch(ExtractTaskDeclinfo(), &tskdecl);
  SgAdaTaskBodyDecl& sgnode   = mkLocatedNode<SgAdaTaskBodyDecl>(specinfo.name, &tskbody);

  tskbody.set_parent(&sgnode);
  sgnode.set_parent(&scope);

  SgAdaTaskSpec&     tskspec = SG_DEREF( specinfo.spec );

  tskspec.set_body(&tskbody);
  tskbody.set_spec(&tskspec);

  ROSE_ASSERT(scope.symbol_exists(specinfo.name));
  return sgnode;
}


SgAdaTaskBodyDecl&
mkAdaTaskBodyDecl(const std::string& name, SgAdaTaskBody& tskbody, SgScopeStatement& scope)
{
  SgAdaTaskBodyDecl& sgnode = mkLocatedNode<SgAdaTaskBodyDecl>(name, &tskbody);

  tskbody.set_parent(&sgnode);
  sgnode.set_parent(&scope);

  /*
  SgAdaTaskSpec&     tskspec = SG_DEREF( specinfo.spec );

  tskspec.set_body(&tskbody);
  tskbody.set_spec(&tskspec);
  */

  //~ ROSE_ASSERT(scope.symbol_exists(specinfo.name));
  scope.insert_symbol(name, &mkBareNode<SgAdaTaskSymbol>(&sgnode));
  return sgnode;
}

SgAdaTaskSpec&
mkAdaTaskSpec() { return mkLocatedNode<SgAdaTaskSpec>(); }

SgAdaTaskBody&
mkAdaTaskBody() { return mkLocatedNode<SgAdaTaskBody>(); }

SgFunctionParameterList&
mkFunctionParameterList()
{
  SgFunctionParameterList& sgnode = SG_DEREF(sb::buildFunctionParameterList());

  markCompilerGenerated(sgnode);
  return sgnode;
}



namespace
{
  /// \private
  /// helps to create a procedure definition:
  ///   attaches the definition to the declaration and returns the *function body*.
  SgScopeStatement&
  mkProcDef(SgFunctionDeclaration& dcl)
  {
    SgFunctionDefinition& sgnode = mkLocatedNode<SgFunctionDefinition>(&dcl, nullptr);
    SgBasicBlock&         body   = mkBasicBlock();

    sg::linkParentChild(dcl, sgnode, &SgFunctionDeclaration::set_definition);
    sg::linkParentChild(sgnode, body, &SgFunctionDefinition::set_body);
    return sgnode;
  }

  /// \private
  /// helps to create a procedure definition as declaration
  SgScopeStatement&
  mkProcDecl(SgFunctionDeclaration& dcl)
  {
    SgFunctionParameterScope& sgnode = mkLocatedNode<SgFunctionParameterScope>(&mkFileInfo());

    sg::linkParentChild(dcl, sgnode, &SgFunctionDeclaration::set_functionParameterScope);
    return sgnode;
  }

  SgFunctionDeclaration&
  mkProcedureInternal( const std::string& nm,
                       SgScopeStatement& scope,
                       SgType& retty,
                       std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete,
                       SgScopeStatement& (*scopeMaker) (SgFunctionDeclaration&)
                     )
  {
    SgFunctionParameterList& lst       = mkFunctionParameterList();
    SgFunctionDeclaration&   sgnode    = SG_DEREF(sb::buildNondefiningFunctionDeclaration(nm, &retty, &lst, &scope, nullptr));
    SgScopeStatement&        parmScope = scopeMaker(sgnode);

    complete(lst, parmScope);
    ROSE_ASSERT(sgnode.get_type() != nullptr);

    markCompilerGenerated(sgnode);
    return sgnode;
  }
}

SgFunctionDeclaration&
mkProcedure( const std::string& nm,
             SgScopeStatement& scope,
             SgType& retty,
             std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
           )
{
  return mkProcedureInternal(nm, scope, retty, std::move(complete), mkProcDecl);
}

SgFunctionDeclaration&
mkProcedureDef( SgFunctionDeclaration& ndef,
                SgScopeStatement& scope,
                SgType& retty,
                std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
              )
{
  SgName                 nm     = ndef.get_name();
  SgFunctionDeclaration& sgnode = mkProcedureInternal(nm, scope, retty, std::move(complete), mkProcDef);
  SgSymbol*              baseSy = sgnode.search_for_symbol_from_symbol_table();
  SgFunctionSymbol&      funcSy = *SG_ASSERT_TYPE(SgFunctionSymbol, baseSy);

  linkDecls(funcSy, sgnode);
  sgnode.set_definingDeclaration(&sgnode);
  sgnode.unsetForward();

  return sgnode;
}

SgFunctionDeclaration&
mkProcedureDef( const std::string& nm,
                SgScopeStatement& scope,
                SgType& retty,
                std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
              )
{
  SgFunctionDeclaration& ndef = mkProcedure(nm, scope, retty, complete);

  return mkProcedureDef(ndef, scope, retty, std::move(complete));
}

SgAdaEntryDecl&
mkAdaEntryDecl( const std::string& name,
                SgScopeStatement& scope,
                std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
              )
{
  //~ SgFunctionParameterList&  lst    = mkFunctionParameterList();
  SgAdaEntryDecl&           sgnode = mkLocatedNode<SgAdaEntryDecl>(name, nullptr /* entry type */, nullptr /* definition */);
  SgFunctionParameterList&  lst    = SG_DEREF(sgnode.get_parameterList());
  SgFunctionParameterScope& psc    = mkLocatedNode<SgFunctionParameterScope>(&mkFileInfo());

  ROSE_ASSERT(sgnode.get_functionParameterScope() == nullptr);
  sg::linkParentChild<SgFunctionDeclaration>(sgnode, psc, &SgFunctionDeclaration::set_functionParameterScope);

  complete(lst, psc);

  SgFunctionType&           funty  = mkAdaEntryType(lst);

  sgnode.set_type(&funty);

  //~ ROSE_ASSERT(sgnode.get_parameterList() == nullptr);
  //~ sg::linkParentChild<SgFunctionDeclaration>(sgnode, lst, &SgFunctionDeclaration::set_parameterList);

  // not used
  ROSE_ASSERT(sgnode.get_parameterList_syntax() == nullptr);

  //~ SgFunctionSymbol*         funsy  = scope.find_symbol_by_type_of_function<SgAdaEntryDecl>(name, &funty, NULL, NULL);
  SgFunctionSymbol*         funsy  = scope.find_symbol_by_type_of_function<SgFunctionDeclaration>(name, &funty, NULL, NULL);

  ROSE_ASSERT(funsy == nullptr);
  funsy = &mkBareNode<SgFunctionSymbol>(&sgnode);

  scope.insert_symbol(name, funsy);
  sgnode.set_scope(&scope);
  sgnode.set_definingDeclaration(&sgnode);
  sgnode.unsetForward();

  markCompilerGenerated(lst);
  return sgnode;
}

SgAdaAcceptStmt&
mkAdaAcceptStmt(SgExpression& ref, SgExpression& idx)
{
  SgAdaAcceptStmt&          sgnode = mkLocatedNode<SgAdaAcceptStmt>();
  SgFunctionParameterScope& psc    = mkLocatedNode<SgFunctionParameterScope>(&mkFileInfo());
  SgFunctionParameterList&  lst    = mkFunctionParameterList();

  ROSE_ASSERT(sgnode.get_parameterScope() == nullptr);
  sg::linkParentChild(sgnode, psc, &SgAdaAcceptStmt::set_parameterScope);

  ROSE_ASSERT(sgnode.get_parameterList() == nullptr);
  sg::linkParentChild(sgnode, lst, &SgAdaAcceptStmt::set_parameterList);

  sg::linkParentChild(sgnode, ref, &SgAdaAcceptStmt::set_entry);
  sg::linkParentChild(sgnode, idx, &SgAdaAcceptStmt::set_index);
  return sgnode;
}


SgCatchOptionStmt&
mkExceptionHandler(SgInitializedName& parm, SgBasicBlock& body)
{
  SgCatchOptionStmt&     sgnode = SG_DEREF( sb::buildCatchOptionStmt(nullptr, &body) );
  SgVariableDeclaration& exparm = mkVarDecl(parm, sgnode);

  sg::linkParentChild(sgnode, exparm, &SgCatchOptionStmt::set_condition);
  return sgnode;
}

SgInitializedName&
mkInitializedName(const std::string& varname, SgType& vartype, SgExpression* val)
{
  SgAssignInitializer* varinit = val ? sb::buildAssignInitializer(val) : nullptr;
  SgInitializedName&   sgnode = SG_DEREF( sb::buildInitializedName(varname, &vartype, varinit) );

  //~ sgnode.set_type(&vartype);

  if (varinit)
    markCompilerGenerated(*varinit);

  markCompilerGenerated(sgnode);
  return sgnode;
}

SgVariableDeclaration&
mkParameter( const std::vector<SgInitializedName*>& parms,
             SgTypeModifier parmmode,
             SgScopeStatement& scope
           )
{
  SgVariableDeclaration&    parmDecl = mkLocatedNode<SgVariableDeclaration>(&mkFileInfo());
  SgInitializedNamePtrList& names    = parmDecl.get_variables();
  SgDeclarationModifier&    declMods = parmDecl.get_declarationModifier();

  // insert initialized names and set the proper declaration node
  std::for_each( parms.begin(), parms.end(),
                 [&parmDecl, &names](SgInitializedName* prm)->void
                 {
                   prm->set_definition(&parmDecl);
                   names.push_back(prm);
                 }
               );
  declMods.get_typeModifier() = parmmode;

  si::fixVariableDeclaration(&parmDecl, &scope);
  parmDecl.set_parent(&scope);

  ROSE_ASSERT(parmDecl.get_definingDeclaration() == nullptr);
  ROSE_ASSERT(parmDecl.get_firstNondefiningDeclaration() == nullptr);

  parmDecl.set_firstNondefiningDeclaration(&parmDecl);
  return parmDecl;
}

namespace
{
  template <class FwdIterator>
  SgVariableDeclaration&
  mkVarExceptionDeclInternal(FwdIterator aa, FwdIterator zz, SgScopeStatement& scope)
  {
    SgVariableDeclaration&    vardcl = mkLocatedNode<SgVariableDeclaration>(&mkFileInfo());
    SgInitializedNamePtrList& names  = vardcl.get_variables();

    names.insert(names.end(), aa, zz);
    std::for_each( aa, zz,
                   [&](SgInitializedName* var) -> void { var->set_parent(&vardcl); }
                 );

    si::fixVariableDeclaration(&vardcl, &scope);
    vardcl.set_parent(&scope);

    return vardcl;
  }

  template <class FwdIterator>
  SgVariableDeclaration&
  mkVarDeclInternal(FwdIterator aa, FwdIterator zz, SgScopeStatement& scope)
  {
    SgVariableDeclaration&    vardcl = mkVarExceptionDeclInternal(aa, zz, scope);

    ROSE_ASSERT(vardcl.get_definingDeclaration() == nullptr);
    ROSE_ASSERT(vardcl.get_firstNondefiningDeclaration() == nullptr);
    vardcl.set_firstNondefiningDeclaration(&vardcl);

    return vardcl;
  }
} // anonymous namespace

SgVariableDeclaration&
mkVarDecl(const std::vector<SgInitializedName*>& vars, SgScopeStatement& scope)
{
  return mkVarDeclInternal(vars.begin(), vars.end(), scope);
}

SgVariableDeclaration&
mkVarDecl(SgInitializedName& var, SgScopeStatement& scope)
{
  SgInitializedName*  alias = &var;
  SgInitializedName** aa    = &alias;

  return mkVarDeclInternal(aa, aa+1, scope);
}

SgVariableDeclaration&
mkExceptionDecl(const std::vector<SgInitializedName*>& vars, SgScopeStatement& scope)
{
  // \todo revise exception declarations
  return mkVarExceptionDeclInternal(vars.begin(), vars.end(), scope);
}

SgAdaComponentClause&
mkAdaComponentClause(SgVarRefExp& field, SgExpression& offset, SgRangeExp& range)
{
  SgAdaComponentClause& sgnode = mkBareNode<SgAdaComponentClause>(&field, &offset, &range);

  field.set_parent(&sgnode);
  offset.set_parent(&sgnode);
  range.set_parent(&sgnode);
  return sgnode;
}

SgAdaRecordRepresentationClause&
mkAdaRecordRepresentationClause(SgClassType& record, SgExpression& align)
{
  SgAdaRecordRepresentationClause& sgnode = mkLocatedNode<SgAdaRecordRepresentationClause>(&record, &align);

  align.set_parent(&sgnode);
  return sgnode;
}



SgBaseClass&
mkRecordParent(SgClassDeclaration& n)
{
  if (!n.get_definingDeclaration())
  {
    logWarn() << "no defining declaration for base class: " << n.get_name()
              << std::endl;
  }

  return mkBareNode<SgBaseClass>(&n, true /* direct base */);
}

//
// Expression Makers

SgDesignatedInitializer&
mkAdaNamedInitializer(SgExprListExp& components, SgExpression& val)
{
  SgAssignInitializer&     ini = SG_DEREF(sb::buildAssignInitializer(&val));
  SgDesignatedInitializer& sgnode = mkBareNode<SgDesignatedInitializer>(&components, &ini);

  return sgnode;
}


SgExpression&
mkUnresolvedName(const std::string& n, SgScopeStatement& scope)
{
  logWarn() << "ADDING unresolved name: " << n << std::endl;

  return SG_DEREF(sb::buildOpaqueVarRefExp(n, &scope));
}


SgRangeExp&
mkRangeExp(SgExpression& start, SgExpression& end)
{
  SgExpression& stride = SG_DEREF(sb::buildIntVal(1));
  SgRangeExp&   sgnode = SG_DEREF(sb::buildRangeExp(&stride));

  sg::linkParentChild(sgnode, start, &SgRangeExp::set_start);
  sg::linkParentChild(sgnode, end,   &SgRangeExp::set_end);
  return sgnode;
}

SgRangeExp&
mkRangeExp()
{
  SgExpression& start  = SG_DEREF( sb::buildNullExpression() );
  SgExpression& end    = SG_DEREF( sb::buildNullExpression() );

  return mkRangeExp(start, end);
}

SgExpression&
mkOthersExp()
{
  return SG_DEREF(sb::buildVoidVal());
}

SgExpression&
mkExceptionRef(SgInitializedName& exception, SgScopeStatement& scope)
{
  return SG_DEREF( sb::buildVarRefExp(&exception, &scope) );
}

SgDotExp&
mkSelectedComponent(SgExpression& prefix, SgExpression& selector)
{
  return SG_DEREF( sb::buildDotExp(&prefix, &selector) );
}

SgAdaTaskRefExp&
mkAdaTaskRefExp(SgAdaTaskSpecDecl& task)
{
  return mkBareNode<SgAdaTaskRefExp>(&task);
}

SgExpression& mkChoiceExpIfNeeded(std::vector<SgExpression*>&& choices)
{
  ROSE_ASSERT(choices.size() > 0);

  return SG_DEREF( std::accumulate( choices.begin()+1, choices.end(),
                                    choices.front(),
                                    sb::buildCommaOpExp
                                  ));
}


SgUnaryOp&
mkForLoopIncrement(bool forward, SgVariableDeclaration& var)
{
  SgVarRefExp& varref = SG_DEREF( sb::buildVarRefExp(&var) );
  SgUnaryOp*   sgnode = forward ? static_cast<SgUnaryOp*>(sb::buildUnaryAddOp(&varref))
                                : sb::buildMinusOp(&varref)
                                ;

  return SG_DEREF(sgnode);
}


SgTypeTraitBuiltinOperator&
mkAdaExprAttribute(SgExpression& expr, const std::string& ident, SgExprListExp& args)
{
  return SG_DEREF(sb::buildTypeTraitBuiltinOperator(ident, { &expr, &args }));
}

//
// specialized templates

template <>
SgStringVal& mkValue<SgStringVal>(const char* textrep)
{
  ROSE_ASSERT(textrep && *textrep == '"');

  std::string lit{textrep+1};

  ROSE_ASSERT(lit.back() == '"');
  lit.pop_back();

  return mkLocatedNode<SgStringVal>(lit);
}


//
// builder functions

SgRemOp*
buildRemOp(SgExpression* lhs, SgExpression* rhs)
{
  return &mkLocatedNode<SgRemOp>(lhs, rhs, nullptr);
}

SgAbsOp*
buildAbsOp(SgExpression* op)
{
  return &mkLocatedNode<SgAbsOp>(op, nullptr);
}


}
