
#include "sage3basic.h"

#include "AdaMaker.h"

#include "Ada_to_ROSE.h"

// turn on all GCC warnings after include files have been processed
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"


namespace sb = SageBuilder;
namespace si = SageInterface;

// anonymous namespace for auxiliary functions
namespace
{
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



namespace Ada_ROSE_Translation
{

//
// file info related functions

Sg_File_Info& mkFileInfo()
{
  return SG_DEREF( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );
}

Sg_File_Info& mkFileInfo(const std::string& file, int line, int col)
{
  Sg_File_Info& sgnode = SG_DEREF( new Sg_File_Info(file, line, col) );

  sgnode.setOutputInCodeGeneration();
  return sgnode;
}

#if NOT_USED
Sg_File_Info& mkFileInfo(const Sg_File_Info& orig)
{
  return SG_DEREF(new Sg_File_Info(orig));
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
  return SG_DEREF( new SgAdaRangeConstraint(&range) );
}

SgAdaSubtype&
mkAdaSubtype(SgType& superty, SgAdaTypeConstraint& constr)
{
  return mkTypeNode<SgAdaSubtype>(&superty, &constr);
}

SgAdaFloatType&
mkAdaFloatType(SgExpression& digits, SgAdaRangeConstraint& range)
{
  return mkTypeNode<SgAdaFloatType>(&digits, &range);
}

SgTypedefType&
mkTypedefType(SgTypedefDeclaration& dcl)
{
  return mkTypeNode<SgTypedefType>(&dcl);
}

SgDeclType&
mkExceptionType(SgExpression& n)
{
  return mkTypeNode<SgDeclType>(&n);
}


SgTypeDefault&
mkDefaultType()
{
  return mkTypeNode<SgTypeDefault>();
}

SgTypeTuple&
mkTypeUnion(const std::vector<SgType*>& elemtypes)
{
  SgTypeTuple&   sgnode = mkTypeNode<SgTypeTuple>();
  SgTypePtrList& lst    = sgnode.get_types();

  lst.insert(lst.end(), elemtypes.begin(), elemtypes.end());
  return sgnode;
}

SgClassType&
mkRecordType(SgClassDeclaration& dcl)
{
  return mkTypeNode<SgClassType>(&dcl);
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

SgArrayType& mkArrayType(SgType& comptype, SgExprListExp& indices)
{
  return SG_DEREF(sb::buildArrayType(&comptype, &indices));
}

SgStatement&
mkRaiseStmt(SgExpression& raised)
{
  SgExpression&    raiseop = SG_DEREF( sb::buildThrowOp(&raised, SgThrowOp::throw_expression ) );
  SgExprStatement& sgnode  = SG_DEREF( sb::buildExprStatement(&raiseop) );

  markCompilerGenerated(raiseop);
  //~ markCompilerGenerated(sgnode);
  return sgnode;
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
  SgWhileStmt& sgnode = SG_DEREF( sb::buildWhileStmt(&cond, &body) );

  //~ markCompilerGenerated(sgnode);
  return sgnode;
}

SgAdaLoopStmt&
mkLoopStmt(SgBasicBlock& body)
{
  SgAdaLoopStmt&  sgnode = SG_DEREF( new SgAdaLoopStmt() );

  sg::linkParentChild(sgnode, body, &SgAdaLoopStmt::set_body);

  //~ markCompilerGenerated(sgnode);
  return sgnode;
}

SgForStatement&
mkForStatement(SgBasicBlock& body)
{
  SgStatement&    test   = SG_DEREF( sb::buildNullStatement() );
  SgExpression&   incr   = SG_DEREF( sb::buildNullExpression() );
  SgForStatement& sgnode = SG_DEREF( sb::buildForStatement(nullptr, &test, &incr, &body) );

  //~ markCompilerGenerated(sgnode);
  return sgnode;
}

SgImportStatement&
mkWithClause(const std::vector<SgExpression*>& imported)
{
  SgImportStatement&   sgnode = SG_DEREF( new SgImportStatement(&mkFileInfo()) );
  SgExpressionPtrList& lst    = sgnode.get_import_list();

  lst.insert(lst.end(), imported.begin(), imported.end());

  markCompilerGenerated(sgnode);
  return sgnode;
}

SgAdaExitStmt&
mkAdaExitStmt(SgStatement& loop, SgExpression& condition, bool explicitLoopName)
{
  SgAdaExitStmt& sgnode = SG_DEREF(new SgAdaExitStmt(&loop, &condition, explicitLoopName));

  //~ markCompilerGenerated(sgnode);
  return sgnode;
}

SgSwitchStatement&
mkAdaCaseStmt(SgExpression& selector, SgBasicBlock& body)
{
  SgSwitchStatement& sgnode = SG_DEREF(sb::buildSwitchStatement(&selector, &body));

  //~ markCompilerGenerated(sgnode);
  return sgnode;
}


SgLabelStatement&
mkLabelStmt(const std::string& label, SgStatement& stmt, SgScopeStatement& encl)
{
  SgLabelStatement& sgnode = SG_DEREF( sb::buildLabelStatement(label, &stmt, &encl) );

  sg::linkParentChild(sgnode, stmt, &SgLabelStatement::set_statement);
  //~ markCompilerGenerated(sgnode);
  return sgnode;
}

SgStatement&
mkNullStmt()
{
  SgStatement& sgnode = SG_DEREF(sb::buildNullStatement());

  //~ markCompilerGenerated(sgnode);
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

  markCompilerGenerated(sgnode);
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
  SgAdaPackageSpec&     pkgdef = mkBareNode<SgAdaPackageSpec>();
  SgAdaPackageSpecDecl& sgnode = SG_DEREF( new SgAdaPackageSpecDecl(name, nullptr) );

  sgnode.set_parent(&scope);
  sgnode.set_firstNondefiningDeclaration(&sgnode);

  sg::linkParentChild(sgnode, pkgdef, &SgAdaPackageSpecDecl::set_definition);

  // add the symbol to the table
  //~ ROSE_ASSERT(!scope.symbol_exists(name));
  scope.insert_symbol(name, new SgAdaPackageSymbol(&sgnode));

  /*
  // set if first definition
  if (nsdef.get_global_definition() == nullptr)
  {
    SgNamespaceDefinitionStatement&  globdef = SG_DEREF(sb::buildNamespaceDefinition(&nsdcl));

    globdef.set_parent(&scope);
    globdef.set_global_definition(&globdef);
    nsdef.set_global_definition(&globdef);
  }
  */

  markCompilerGenerated(pkgdef);
  markCompilerGenerated(sgnode);
  return sgnode;
}

SgAdaRenamingDecl&
mkAdaRenamingDecl(const std::string& name, SgDeclarationStatement& aliased, SgScopeStatement& scope)
{
  SgAdaRenamingDecl& sgnode = mkBareNode<SgAdaRenamingDecl>(name, &aliased);

  sgnode.set_parent(&scope);
  sgnode.set_firstNondefiningDeclaration(&sgnode);
  //~ scope.insert_symbol(name, new SgAdaPackageSymbol(&sgnode));

  markCompilerGenerated(sgnode);
  return sgnode;
}

SgAdaPackageBodyDecl&
mkAdaPackageBodyDecl(SgAdaPackageSpecDecl& specdcl, SgScopeStatement& scope)
{
  SgAdaPackageBody&     pkgbody = SG_DEREF( new SgAdaPackageBody() );
  SgAdaPackageBodyDecl& sgnode  = SG_DEREF( new SgAdaPackageBodyDecl(specdcl.get_name(), &pkgbody) );

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
  markCompilerGenerated(pkgbody);
  markCompilerGenerated(sgnode);
  return sgnode;
}

SgAdaTaskTypeDecl&
mkAdaTaskTypeDecl(const std::string& name, SgAdaTaskSpec& spec, SgScopeStatement& scope)
{
  SgAdaTaskTypeDecl& sgnode = SG_DEREF(new SgAdaTaskTypeDecl(name, &spec));

  scope.insert_symbol(name, new SgAdaTaskSymbol(&sgnode));

  spec.set_parent(&sgnode);
  markCompilerGenerated(sgnode);
  return sgnode;
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
  SgAdaTaskBodyDecl& sgnode   = SG_DEREF( new SgAdaTaskBodyDecl(specinfo.name, &tskbody) );

  tskbody.set_parent(&sgnode);
  sgnode.set_parent(&scope);

  SgAdaTaskSpec&     tskspec = SG_DEREF( specinfo.spec );

  tskspec.set_body(&tskbody);
  tskbody.set_spec(&tskspec);

  ROSE_ASSERT(scope.symbol_exists(specinfo.name));
  return sgnode;
}


SgAdaTaskBodyDecl&
mkAdaTaskBodyDecl(std::string name, SgAdaTaskBody& tskbody, SgScopeStatement& scope)
{
  //~ SgAdaPackageBody&     pkgbody = SG_DEREF( new SgAdaPackageBody() );
  SgAdaTaskBodyDecl& sgnode   = SG_DEREF( new SgAdaTaskBodyDecl(name, &tskbody) );

  tskbody.set_parent(&sgnode);
  sgnode.set_parent(&scope);

  /*
  SgAdaTaskSpec&     tskspec = SG_DEREF( specinfo.spec );

  tskspec.set_body(&tskbody);
  tskbody.set_spec(&tskspec);
  */

  //~ ROSE_ASSERT(scope.symbol_exists(specinfo.name));
  scope.insert_symbol(name, new SgAdaTaskSymbol(&sgnode));
  return sgnode;
}

SgAdaTaskSpec&
mkAdaTaskSpec() { return mkBareNode<SgAdaTaskSpec>(); }

SgAdaTaskBody&
mkAdaTaskBody() { return mkBareNode<SgAdaTaskBody>(); }

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
    SgFunctionDefinition& sgnode = SG_DEREF(new SgFunctionDefinition(&dcl, nullptr));
    SgBasicBlock&         body   = mkBasicBlock();

    sg::linkParentChild(dcl, sgnode, &SgFunctionDeclaration::set_definition);
    sg::linkParentChild(sgnode, body, &SgFunctionDefinition::set_body);
    markCompilerGenerated(sgnode);
    return sgnode;
  }

  /// \private
  /// helps to create a procedure definition as declaration
  SgScopeStatement&
  mkProcDecl(SgFunctionDeclaration& dcl)
  {
    SgFunctionParameterScope& sgnode = SG_DEREF(new SgFunctionParameterScope(&mkFileInfo()));

    sg::linkParentChild(dcl, sgnode, &SgFunctionDeclaration::set_functionParameterScope);
    markCompilerGenerated(sgnode);
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
  SgAdaEntryDecl&           sgnode = SG_DEREF(new SgAdaEntryDecl(name, NULL /* entry type */, NULL /* definition */));
  SgFunctionParameterList&  lst    = SG_DEREF(sgnode.get_parameterList());
  SgFunctionParameterScope& psc    = SG_DEREF(new SgFunctionParameterScope(&mkFileInfo()));

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
  funsy = new SgFunctionSymbol(&sgnode);

  scope.insert_symbol(name, funsy);
  sgnode.set_scope(&scope);
  sgnode.set_definingDeclaration(&sgnode);
  sgnode.unsetForward();

  markCompilerGenerated(psc);
  markCompilerGenerated(lst);
  markCompilerGenerated(sgnode);
  return sgnode;
}

SgAdaAcceptStmt&
mkAdaAcceptStmt(SgExpression& ref, SgExpression& idx)
{
  SgAdaAcceptStmt&          sgnode = mkBareNode<SgAdaAcceptStmt>();
  SgFunctionParameterScope& psc    = SG_DEREF(new SgFunctionParameterScope(&mkFileInfo()));
  SgFunctionParameterList&  lst    = mkFunctionParameterList();

  ROSE_ASSERT(sgnode.get_parameterScope() == nullptr);
  sg::linkParentChild(sgnode, psc, &SgAdaAcceptStmt::set_parameterScope);

  ROSE_ASSERT(sgnode.get_parameterList() == nullptr);
  sg::linkParentChild(sgnode, lst, &SgAdaAcceptStmt::set_parameterList);

  sg::linkParentChild(sgnode, ref, &SgAdaAcceptStmt::set_entry);
  sg::linkParentChild(sgnode, idx, &SgAdaAcceptStmt::set_index);

  markCompilerGenerated(psc);
  return sgnode;
}


SgCatchOptionStmt&
mkExceptionHandler(SgInitializedName& parm, SgBasicBlock& body)
{
  SgCatchOptionStmt&     sgnode = SG_DEREF( sb::buildCatchOptionStmt(nullptr, &body) );
  SgVariableDeclaration& exparm = mkVarDecl(parm, sgnode);

  sg::linkParentChild(sgnode, exparm, &SgCatchOptionStmt::set_condition);
  //~ markCompilerGenerated(sgnode);
  return sgnode;
}

SgInitializedName&
mkInitializedName(const std::string& varname, SgType& vartype, SgExpression* varexpr)
{
  SgAssignInitializer* varinit = varexpr ? sb::buildAssignInitializer(varexpr) : nullptr;
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
  SgVariableDeclaration&    parmDecl = SG_DEREF( new SgVariableDeclaration(&mkFileInfo()) );
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

  markCompilerGenerated(parmDecl);
  si::fixVariableDeclaration(&parmDecl, &scope);
  parmDecl.set_parent(&scope);

  ROSE_ASSERT(parmDecl.get_definingDeclaration() == nullptr);
  ROSE_ASSERT(parmDecl.get_firstNondefiningDeclaration() == nullptr);

  parmDecl.set_firstNondefiningDeclaration(&parmDecl);
  return parmDecl;
}

namespace
{
  template <class Const_iterator>
  SgVariableDeclaration&
  mkVarDeclInternal(Const_iterator aa, Const_iterator zz, SgScopeStatement& scope)
  {
    SgVariableDeclaration&    vardcl = SG_DEREF( new SgVariableDeclaration(&mkFileInfo()) );
    SgInitializedNamePtrList& names  = vardcl.get_variables();

    names.insert(names.end(), aa, zz);

    markCompilerGenerated(vardcl);
    si::fixVariableDeclaration(&vardcl, &scope);
    vardcl.set_parent(&scope);

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
  SgVariableDeclaration&    vardcl = SG_DEREF( new SgVariableDeclaration(&mkFileInfo()) );
  SgInitializedNamePtrList& names  = vardcl.get_variables();

  names.insert(names.end(), vars.begin(), vars.end());

  markCompilerGenerated(vardcl);
  si::fixVariableDeclaration(&vardcl, &scope);
  vardcl.set_parent(&scope);
  return vardcl;
}

SgBaseClass&
mkRecordParent(SgClassDeclaration& n)
{
  if (!n.get_definingDeclaration())
  {
    logWarn() << "no defining declaration for base class: " << n.get_name()
              << std::endl;
  }

  return SG_DEREF(new SgBaseClass(&n, true /* direct base */));
}

//
// Expression Makers

SgExpression&
mkUnresolvedName(std::string n, SgScopeStatement& scope)
{
  logWarn() << "ADDING unresolved name: " << n << std::endl;

  return SG_DEREF(sb::buildOpaqueVarRefExp(n, &scope));
}


SgRangeExp&
mkRangeExp(SgExpression& start, SgExpression& end)
{
  SgRangeExp& sgnode = SG_DEREF( sb::buildRangeExp(&start) );

  sgnode.set_end(&end);

  //~ markCompilerGenerated(sgnode);
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
  return SG_DEREF(sb::buildNullExpression());
}

SgExpression&
mkExceptionRef(SgInitializedName& exception, SgScopeStatement& scope)
{
  SgExpression& sgnode = SG_DEREF( sb::buildVarRefExp(&exception, &scope) );

  //~ markCompilerGenerated(sgnode);
  return sgnode;
}

SgDotExp&
mkSelectedComponent(SgExpression& prefix, SgExpression& selector)
{
  SgDotExp& sgnode = SG_DEREF( sb::buildDotExp(&prefix, &selector) );

  //~ markCompilerGenerated(sgnode);
  return sgnode;
}

SgRemOp*
buildRemOp(SgExpression* lhs, SgExpression* rhs)
{
  return &mkBareNode<SgRemOp>(lhs, rhs, nullptr);
}

SgAbsOp*
buildAbsOp(SgExpression* op)
{
  return &mkBareNode<SgAbsOp>(op, nullptr);
}


}
