#include "sage3basic.h"

#include <algorithm>
#include <numeric>
#include <limits>
#include <cmath>

#include <boost/algorithm/string/replace.hpp>

#include "AdaMaker.h"

#include "Ada_to_ROSE.h"
#include "sageInterfaceAda.h"

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
    ADA_ASSERT(&sdcl != &func);

    sdcl.set_definingDeclaration(&func);
    func.set_firstNondefiningDeclaration(&sdcl);
  }
}


//
// file info related functions

Sg_File_Info& mkFileInfo()
{
  Sg_File_Info& sgnode = SG_DEREF( Sg_File_Info::generateDefaultFileInfoForTransformationNode() );

  sgnode.setOutputInCodeGeneration();

#if NOT_USED
  Sg_File_Info& sgnode = SG_DEREF( Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode() );

  //~ sgnode.setOutputInCodeGeneration();
  sgnode.unsetTransformation();
#endif /* NOT_USED */
  return sgnode;
}

Sg_File_Info& mkFileInfo(const std::string& file, int line, int col)
{
  Sg_File_Info& sgnode = mkBareNode<Sg_File_Info>(file, line, col);

  sgnode.setOutputInCodeGeneration();
  //~ sgnode.unsetTransformation();
  return sgnode;
}

#if NOT_USED
Sg_File_Info& mkFileInfo(const Sg_File_Info& orig)
{
  return mkBareNode<Sg_File_Info>(orig);
}

void copyFileInfo(const SgLocatedNode& src, SgLocatedNode& tgt)
{
  //~ tgt.set_file_info       (&mkFileInfo(SG_DEREF(src.get_file_info())));
  tgt.set_startOfConstruct(&mkFileInfo(SG_DEREF(src.get_startOfConstruct())));
  tgt.set_endOfConstruct  (&mkFileInfo(SG_DEREF(src.get_endOfConstruct())));
}
#endif /* NOT_USED */


void markCompilerGenerated(SgLocatedNode& n)
{
  n.set_startOfConstruct(&mkFileInfo());
  n.set_endOfConstruct  (&mkFileInfo());

  n.unsetTransformation();
  n.setCompilerGenerated();
}

void markCompilerGenerated(SgExpression& n)
{
  n.set_operatorPosition(&mkFileInfo());

  markCompilerGenerated(static_cast<SgLocatedNode&>(n));
}


void setSymbolTableCaseSensitivity(SgScopeStatement& n)
{
  SgSymbolTable& sytable = SG_DEREF(n.get_symbol_table());

  sytable.setCaseInsensitive(true);
}


//
// types

SgAdaRangeConstraint&
mkAdaRangeConstraint(SgExpression& range)
{
  SgAdaRangeConstraint& sgnode = mkLocatedNode<SgAdaRangeConstraint>(&range);

  range.set_parent(&sgnode);
  return sgnode;
}

SgAdaDigitsConstraint&
mkAdaDigitsConstraint(SgExpression& digits, SgAdaRangeConstraint* range_opt)
{
  SgAdaDigitsConstraint& sgnode = mkLocatedNode<SgAdaDigitsConstraint>(&digits, range_opt);

  digits.set_parent(&sgnode);
  if (range_opt) range_opt->set_parent(&sgnode);

  return sgnode;
}


namespace
{
  void incorporateConstraintExpressions(SgNode& parent, SgExpressionPtrList& constraints, SgExpressionPtrList&& exprs)
  {
    constraints.swap(exprs);

    for (SgExpression* expr : constraints)
      SG_DEREF(expr).set_parent(&parent);
  }
}

SgAdaIndexConstraint&
mkAdaIndexConstraint(SgExpressionPtrList ranges)
{
  SgAdaIndexConstraint& sgnode = mkLocatedNode<SgAdaIndexConstraint>();

  incorporateConstraintExpressions(sgnode, sgnode.get_indexRanges(), std::move(ranges));
  return sgnode;
}

SgAdaDiscriminantConstraint&
mkAdaDiscriminantConstraint(SgExpressionPtrList discriminants)
{
  SgAdaDiscriminantConstraint& sgnode = mkLocatedNode<SgAdaDiscriminantConstraint>();

  incorporateConstraintExpressions(sgnode, sgnode.get_discriminants(), std::move(discriminants));
  return sgnode;
}


SgAdaSubtype&
mkAdaSubtype(SgType& superty, SgAdaTypeConstraint& constr, bool fromRoot)
{
  SgAdaSubtype& sgnode = mkNonSharedTypeNode<SgAdaSubtype>(&superty, &constr);

  sgnode.set_fromRootType(fromRoot);
  constr.set_parent(&sgnode);
  return sgnode;
}

SgAdaDerivedType&
mkAdaDerivedType(SgType& basetype)
{
  return mkNonSharedTypeNode<SgAdaDerivedType>(&basetype);
}

SgAdaModularType&
mkAdaModularType(SgExpression& modexpr)
{
  SgAdaModularType& sgnode = mkNonSharedTypeNode<SgAdaModularType>(&modexpr);

  modexpr.set_parent(&sgnode);
  return sgnode;
}

/*
SgAdaFloatType&
mkAdaFloatType(SgExpression& digits, SgAdaRangeConstraint* range_opt)
{
  SgAdaFloatType& sgnode = mkNonSharedTypeNode<SgAdaFloatType>(&digits, range_opt);

  digits.set_parent(&sgnode);
  if (range_opt) range_opt->set_parent(&sgnode);

  return sgnode;
}

SgAdaFormalType&
mkAdaFormalType(const std::string& name)
{
  SgAdaFormalType& ty = mkNonSharedTypeNode<SgAdaFormalType>();
  ty.set_type_name(name);
  return ty;
}
*/

SgDeclType&
mkExceptionType(SgExpression& n)
{
  return mkNonSharedTypeNode<SgDeclType>(&n);
}

SgDeclType&
mkAttributeType(SgAdaAttributeExp& n)
{
  return mkNonSharedTypeNode<SgDeclType>(&n);
}


SgTypeDefault&
mkOpaqueType()
{
  return mkTypeNode<SgTypeDefault>();
}

SgTypeTuple&
mkTypeUnion(SgTypePtrList elemtypes)
{
  SgTypeTuple&   sgnode = mkNonSharedTypeNode<SgTypeTuple>();

  sgnode.get_types().swap(elemtypes);
  return sgnode;
}

SgEnumDeclaration&
mkEnumDecl(const std::string& name, SgScopeStatement& scope)
{
  return SG_DEREF(sb::buildNondefiningEnumDeclaration_nfi(name, &scope));
}

SgEnumDeclaration&
mkEnumDefn(const std::string& name, SgScopeStatement& scope)
{
  return SG_DEREF(sb::buildEnumDeclaration_nfi(name, &scope));
}

SgAdaAccessType&
mkAdaAccessType(SgType& base_type)
{
  // \todo PP (01/28/22) this may need to be a shared type node
  SgAdaAccessType& sgnode = mkNonSharedTypeNode<SgAdaAccessType>(&base_type);
  return sgnode;
}

namespace
{

SgFunctionType& mkAdaEntryType(SgFunctionParameterList& lst)
{
  // \todo build entry type
  return SG_DEREF(sb::buildFunctionType(sb::buildVoidType(), &lst));
}

}

SgFunctionType& mkFunctionType(SgType& returnType)
{
  SgFunctionParameterTypeList& paramTypes = mkBareNode<SgFunctionParameterTypeList>();

  return SG_DEREF(sb::buildFunctionType(&returnType, &paramTypes));
}

SgFunctionType& mkAdaFunctionRenamingDeclType(SgType& retty, SgFunctionParameterList& lst)
{
  return SG_DEREF(sb::buildFunctionType(&retty, &lst));
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

SgType& mkConstType(SgType& underType)
{
  return SG_DEREF(sb::buildConstType(&underType));
}

SgType& mkAliasedType(SgType& underType)
{
  return SG_DEREF(sb::buildAliasedType(&underType));
}



//
// Statements

SgExprStatement&
mkExprStatement(SgExpression& expr)
{
  SgExprStatement& sgnode  = SG_DEREF( sb::buildExprStatement_nfi(&expr) );

  markCompilerGenerated(sgnode);
  return sgnode;
}

SgStatement&
mkRaiseStmt(SgExpression& raised)
{
  SgExpression&    raiseop = SG_DEREF( sb::buildThrowOp(&raised, SgThrowOp::throw_expression ) );
  SgExprStatement& sgnode  = mkExprStatement(raiseop);

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
  setSymbolTableCaseSensitivity(sgnode);
  return sgnode;
}

SgWhileStmt&
mkWhileStmt(SgExpression& cond, SgBasicBlock& body)
{
  SgExprStatement& condStmt = mkExprStatement(cond);
  SgWhileStmt&     sgnode = SG_DEREF( sb::buildWhileStmt_nfi(&condStmt, &body) );

  markCompilerGenerated(sgnode);
  setSymbolTableCaseSensitivity(sgnode);
  return sgnode;
}

SgAdaLoopStmt&
mkLoopStmt(SgBasicBlock& body)
{
  SgAdaLoopStmt& sgnode = mkLocatedNode<SgAdaLoopStmt>();

  sg::linkParentChild(sgnode, body, &SgAdaLoopStmt::set_body);
  return sgnode;
}

SgForStatement&
mkForStatement(SgBasicBlock& body)
{
  SgNullStatement& test = mkNullStatement();
  SgForStatement&  sgnode = SG_DEREF( sb::buildForStatement(nullptr, &test, nullptr, &body) );
  //~ SgForStatement&  sgnode = SG_DEREF( sb::buildForStatement_nfi(nullptr, &test, nullptr, &body) );

  markCompilerGenerated(sgnode);
  setSymbolTableCaseSensitivity(sgnode);
  return sgnode;
}


SgIfStmt&
mkIfStmt()
// SgExpression& cond, SgStatement& thenBranch, SgStatement* elseBranch_opt)
{
  SgIfStmt& sgnode = SG_DEREF( sb::buildIfStmt_nfi(nullptr, nullptr, nullptr) );

  markCompilerGenerated(sgnode);
  setSymbolTableCaseSensitivity(sgnode);
  return sgnode;
}


SgImportStatement&
mkWithClause(SgExpressionPtrList imported)
{
  SgImportStatement& sgnode = mkBareNode<SgImportStatement>(&mkFileInfo());

  sgnode.get_import_list().swap(imported);
  markCompilerGenerated(sgnode);
  return sgnode;
}

SgUsingDeclarationStatement&
mkUseClause(SgDeclarationStatement& used)
{
  SgUsingDeclarationStatement& sgnode = mkBareNode<SgUsingDeclarationStatement>(&used, nullptr);

  markCompilerGenerated(sgnode);
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
  SgExprStatement&   selStmt = mkExprStatement(selector);
  SgSwitchStatement& sgnode  = SG_DEREF( sb::buildSwitchStatement_nfi(&selStmt, &body) );

  setSymbolTableCaseSensitivity(sgnode);
  return sgnode;
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
mkLabelStmt(const std::string& label, SgStatement& stmt, SgScopeStatement& scope)
{
  SgLabelStatement& sgnode = SG_DEREF( sb::buildLabelStatement(label, &stmt, &scope) );

  sg::linkParentChild(sgnode, stmt, &SgLabelStatement::set_statement);
  return sgnode;
}

SgNullStatement&
mkNullStatement()
{
  SgNullStatement& sgnode = mkLocatedNode<SgNullStatement>();

  return sgnode;
}

SgEmptyDeclaration&
mkNullDecl()
{
  return SG_DEREF(sb::buildEmptyDeclaration());
}

SgTryStmt&
mkTryStmt(SgBasicBlock& blk)
{
  SgTryStmt& sgnode = SG_DEREF(sb::buildTryStmt(&blk));

  markCompilerGenerated(SG_DEREF(sgnode.get_catch_statement_seq_root()));
  markCompilerGenerated(sgnode);
  return sgnode;
}

SgAdaTerminateStmt&
mkTerminateStmt()
{
  return mkLocatedNode<SgAdaTerminateStmt>();
}


//
// declarations

SgTypedefDeclaration&
mkTypeDecl(const std::string& name, SgType& ty, SgScopeStatement& scope)
{
  SgTypedefDeclaration& sgnode = SG_DEREF( sb::buildTypedefDeclaration(name, &ty, &scope) );

  return sgnode;
}

SgClassDefinition&
mkRecordBody()
{
  SgClassDefinition& sgnode = SG_DEREF( sb::buildClassDefinition_nfi() );

  setSymbolTableCaseSensitivity(sgnode);
  return sgnode;
}


SgClassDeclaration&
mkRecordDecl(SgClassDeclaration& nondef, SgClassDefinition& def, SgScopeStatement& scope)
{
  SgClassDeclaration& sgnode = SG_DEREF( sb::buildNondefiningClassDeclaration_nfi( nondef.get_name(),
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
  return sgnode;
}

SgClassDeclaration&
mkRecordDecl(const std::string& name, SgScopeStatement& scope)
{
  SgClassDeclaration& sgnode = SG_DEREF( sb::buildNondefiningClassDeclaration_nfi( name,
                                                                                   SgClassDeclaration::e_struct,
                                                                                   &scope,
                                                                                   false /* template instance */,
                                                                                   nullptr /* template parameter list */
                                                                                 ));

  sgnode.set_firstNondefiningDeclaration(&sgnode);
  return sgnode;
}

SgClassDeclaration&
mkRecordDecl(const std::string& name, SgClassDefinition& def, SgScopeStatement& scope)
{
  return mkRecordDecl(mkRecordDecl(name, scope), def, scope);
}


SgAdaPackageSpecDecl&
mkAdaPackageSpecDecl(const std::string& name, SgScopeStatement& scope)
{
  SgAdaPackageSpec&     pkgdef = mkScopeStmt<SgAdaPackageSpec>();
  SgAdaPackageSpecDecl& sgnode = mkLocatedNode<SgAdaPackageSpecDecl>(name, nullptr);

  sgnode.set_scope(&scope); // analogous to SgClassDeclaration::set_scope .. really needed?
  sgnode.set_firstNondefiningDeclaration(&sgnode);

  sg::linkParentChild(sgnode, pkgdef, &SgAdaPackageSpecDecl::set_definition);

  // add the symbol to the table
  scope.insert_symbol(name, &mkBareNode<SgAdaPackageSymbol>(&sgnode));
  return sgnode;
}

namespace
{
  SgDeclarationScope&
  mkDeclarationScope(SgScopeStatement& /* not use */)
  {
    SgDeclarationScope& sgnode = mkScopeStmt<SgDeclarationScope>();

    //~ sgnode.set_scope(&outer);
    return sgnode;
  }
}

SgAdaDiscriminatedTypeDecl&
mkAdaDiscriminatedTypeDecl(SgScopeStatement& scope)
{
  SgDeclarationScope&         dclscope = mkDeclarationScope(scope);
  SgAdaDiscriminatedTypeDecl& sgnode   = mkLocatedNode<SgAdaDiscriminatedTypeDecl>(&dclscope);

  dclscope.set_parent(&sgnode);
  return sgnode;
}

SgAdaGenericInstanceDecl&
mkAdaGenericInstanceDecl(const std::string& name, SgDeclarationStatement& gendecl, SgScopeStatement& scope)
{
  ROSE_ASSERT(  isSgAdaGenericDecl(&gendecl)
             || isSgAdaRenamingDecl(&gendecl)
             );

  SgAdaGenericInstanceDecl& sgnode = mkLocatedNode<SgAdaGenericInstanceDecl>(name,&gendecl,nullptr);

  sgnode.set_parent(&scope);
  sgnode.set_firstNondefiningDeclaration(&sgnode);

  return sgnode;
}

SgAdaGenericDecl&
mkAdaGenericDecl(SgScopeStatement& scope)
{
   SgAdaGenericDefn&   defn   = mkLocatedNode<SgAdaGenericDefn>();
   SgAdaGenericDecl&   sgnode = mkLocatedNode<SgAdaGenericDecl>(&defn);

   defn.setCaseInsensitive(true);

   sgnode.set_parent(&scope);
   sgnode.set_firstNondefiningDeclaration(&sgnode);

   sg::linkParentChild(sgnode, defn, &SgAdaGenericDecl::set_definition);

   return sgnode;
}

SgAdaFormalTypeDecl&
mkAdaFormalTypeDecl(const std::string& name, SgScopeStatement& scope)
{
  SgAdaFormalTypeDecl&  sgnode = mkLocatedNode<SgAdaFormalTypeDecl>(SgName(name));

  sgnode.set_parent(&scope);
  sgnode.set_firstNondefiningDeclaration(&sgnode);

  scope.insert_symbol(name, new SgAdaGenericSymbol(&sgnode));

  return sgnode;
}


namespace
{
  template <class SageNode>
  SgAdaRenamingDecl&
  mkAdaRenamingDeclInternal(const std::string& name, SageNode& renamed, SgType* ty, SgScopeStatement& scope)
  {
    if (ty == nullptr) ty = sb::buildVoidType();

    SgSymbol&          origsy = SG_DEREF(renamed.search_for_symbol_from_symbol_table());
    SgAdaRenamingDecl& sgnode = mkLocatedNode<SgAdaRenamingDecl>(name, &origsy, ty);

    sgnode.set_parent(&scope);
    sgnode.set_firstNondefiningDeclaration(&sgnode);
    scope.insert_symbol(name, &mkBareNode<SgAdaRenamingSymbol>(&sgnode));
    return sgnode;
  }
}

SgAdaRenamingDecl&
mkAdaRenamingDecl(const std::string& name, SgDeclarationStatement& dcl, SgType* ty, SgScopeStatement& scope)
{
  return mkAdaRenamingDeclInternal(name, dcl, ty, scope);
}

SgAdaRenamingDecl&
mkAdaRenamingDecl(const std::string& name, SgInitializedName& ini, SgType* ty, SgScopeStatement& scope)
{
  return mkAdaRenamingDeclInternal(name, ini, ty, scope);
}


SgAdaPackageBodyDecl&
mkAdaPackageBodyDecl(SgAdaPackageSpecDecl& specdcl)
{
  SgAdaPackageBody&     pkgbody = mkScopeStmt<SgAdaPackageBody>();
  SgAdaPackageBodyDecl& sgnode  = mkLocatedNode<SgAdaPackageBodyDecl>(specdcl.get_name(), &pkgbody);

  pkgbody.set_parent(&sgnode);
  sgnode.set_firstNondefiningDeclaration(&sgnode);

  SgAdaPackageSpec&     pkgspec = SG_DEREF( specdcl.get_definition() );

  pkgspec.set_body(&pkgbody);
  pkgbody.set_spec(&pkgspec);

  // \todo make sure assertion holds

  ADA_ASSERT(SG_DEREF(specdcl.get_scope()).symbol_exists(specdcl.get_name()));
  //~ if (!scope.symbol_exists(specdcl.get_name()))
    //~ scope.insert_symbol(specdcl.get_name(), &mkBareNode<SgAdaPackageSymbol>(&sgnode));

  return sgnode;
}


namespace
{
  template <class SageAdaConcurrentSymbol, class SageAdaConcurrentDecl, class SageAdaConcurrentSpec>
  SageAdaConcurrentDecl&
  mkAdaConcurrentDeclInternal(const std::string& name, SageAdaConcurrentSpec& spec, SgScopeStatement& scope)
  {
    SageAdaConcurrentDecl& sgnode = mkLocatedNode<SageAdaConcurrentDecl>(name, &spec);

    scope.insert_symbol(name, &mkBareNode<SageAdaConcurrentSymbol>(&sgnode));
    spec.set_parent(&sgnode);
    return sgnode;
  }
}


SgAdaTaskTypeDecl&
mkAdaTaskTypeDecl(const std::string& name, SgAdaTaskSpec& spec, SgScopeStatement& scope)
{
  return mkAdaConcurrentDeclInternal<SgAdaTaskSymbol, SgAdaTaskTypeDecl>(name, spec, scope);
}

SgAdaTaskSpecDecl&
mkAdaTaskSpecDecl(const std::string& name, SgAdaTaskSpec& spec, SgScopeStatement& scope)
{
  return mkAdaConcurrentDeclInternal<SgAdaTaskSymbol, SgAdaTaskSpecDecl>(name, spec, scope);
}

SgAdaProtectedTypeDecl&
mkAdaProtectedTypeDecl(const std::string& name, SgAdaProtectedSpec& spec, SgScopeStatement& scope)
{
  return mkAdaConcurrentDeclInternal<SgAdaProtectedSymbol, SgAdaProtectedTypeDecl>(name, spec, scope);
}

SgAdaProtectedSpecDecl&
mkAdaProtectedSpecDecl(const std::string& name, SgAdaProtectedSpec& spec, SgScopeStatement& scope)
{
  return mkAdaConcurrentDeclInternal<SgAdaProtectedSymbol, SgAdaProtectedSpecDecl>(name, spec, scope);
}

namespace
{
  struct TaskDeclInfoResult
  {
    std::string    name;
    SgAdaTaskSpec* spec;
  };

  struct TaskDeclInfo : sg::DispatchHandler<TaskDeclInfoResult>
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
  TaskDeclInfoResult specinfo = sg::dispatch(TaskDeclInfo{}, &tskdecl);
  SgAdaTaskBodyDecl& sgnode   = mkLocatedNode<SgAdaTaskBodyDecl>(specinfo.name, &tskbody);

  tskbody.set_parent(&sgnode);
  sgnode.set_parent(&scope);

  SgAdaTaskSpec&     tskspec = SG_DEREF( specinfo.spec );

  tskspec.set_body(&tskbody);
  tskbody.set_spec(&tskspec);

  // \todo make sure assertion holds
  // ADA_ASSERT(scope.symbol_exists(specinfo.name));

  if (!scope.symbol_exists(specinfo.name))
    scope.insert_symbol(specinfo.name, &mkBareNode<SgAdaTaskSymbol>(&sgnode));

  return sgnode;
}

namespace
{
  struct ProtectedDeclInfoResult
  {
    std::string         name;
    SgAdaProtectedSpec* spec;
  };

  struct ProtectedDeclInfo : sg::DispatchHandler<ProtectedDeclInfoResult>
  {
    template <class SageProtectedDecl>
    void handleProtectedDecl(SageProtectedDecl& n)
    {
      res.name = n.get_name();
      res.spec = n.get_definition();
    }

    void handle(SgNode& n)                 { SG_UNEXPECTED_NODE(n); }
    void handle(SgAdaProtectedSpecDecl& n) { handleProtectedDecl(n); }
    void handle(SgAdaProtectedTypeDecl& n) { handleProtectedDecl(n); }
  };
} // anonymous namespace

SgAdaProtectedBodyDecl&
mkAdaProtectedBodyDecl(SgDeclarationStatement& tskdecl, SgAdaProtectedBody& tskbody, SgScopeStatement& scope)
{
  //~ SgAdaPackageBody&     pkgbody = SG_DEREF( new SgAdaPackageBody() );
  ProtectedDeclInfoResult specinfo = sg::dispatch(ProtectedDeclInfo{}, &tskdecl);
  SgAdaProtectedBodyDecl& sgnode   = mkLocatedNode<SgAdaProtectedBodyDecl>(specinfo.name, &tskbody);

  tskbody.set_parent(&sgnode);
  sgnode.set_parent(&scope);

  SgAdaProtectedSpec&     tskspec = SG_DEREF( specinfo.spec );

  tskspec.set_body(&tskbody);
  tskbody.set_spec(&tskspec);

  // \todo make sure assertion holds
  // ADA_ASSERT(scope.symbol_exists(specinfo.name));

  if (!scope.symbol_exists(specinfo.name))
    scope.insert_symbol(specinfo.name, &mkBareNode<SgAdaProtectedSymbol>(&sgnode));

  return sgnode;
}


SgAdaTaskSpec&
mkAdaTaskSpec() { return mkScopeStmt<SgAdaTaskSpec>(); }

SgAdaTaskBody&
mkAdaTaskBody() { return mkScopeStmt<SgAdaTaskBody>(); }

SgAdaProtectedSpec&
mkAdaProtectedSpec() { return mkScopeStmt<SgAdaProtectedSpec>(); }

SgAdaProtectedBody&
mkAdaProtectedBody() { return mkScopeStmt<SgAdaProtectedBody>(); }

SgFunctionParameterList&
mkFunctionParameterList()
{
  return mkLocatedNode<SgFunctionParameterList>();
}



namespace
{
  void linkParameterScope(SgFunctionDeclaration& decl, SgFunctionParameterList& lst, SgScopeStatement& parmScope)
  {
    // the sage builder overrides this information, so we reset it
    // \todo needs to be fixed in the sage builder
    for (SgInitializedName* n : lst.get_args())
      SG_DEREF(n).set_scope(&parmScope);

    if (SgFunctionParameterScope* fps = isSgFunctionParameterScope(&parmScope))
    {
      sg::linkParentChild(decl, *fps, &SgFunctionDeclaration::set_functionParameterScope);
      return;
    }

    SgFunctionDefinition* defn = isSgFunctionDefinition(&parmScope);
    ADA_ASSERT(defn);
    sg::linkParentChild(decl, *defn, &SgFunctionDeclaration::set_definition);
  }


  /// \private
  /// helps to create a procedure definition:
  ///   attaches the definition to the declaration and returns the *function body*.
  SgScopeStatement&
  mkProcDef()
  {
    //~ SgFunctionDefinition& sgnode = mkLocatedNode<SgFunctionDefinition>(&dcl, nullptr);
    SgBasicBlock&         body   = mkBasicBlock();
    SgFunctionDefinition& sgnode = mkScopeStmt<SgFunctionDefinition>(&mkFileInfo(), &body);

    body.set_parent(&sgnode);
    //~ sg::linkParentChild(sgnode, body, &SgFunctionDefinition::set_body);
    //~ sg::linkParentChild(dcl, sgnode, &SgFunctionDeclaration::set_definition);
    return sgnode;
  }

  /// \private
  /// helps to create a procedure definition as declaration
  SgScopeStatement&
  //~ mkProcDecl(SgFunctionDeclaration& dcl)
  mkProcDecl()
  {
    SgFunctionParameterScope& sgnode = mkScopeStmt<SgFunctionParameterScope>(&mkFileInfo());

    //~ sg::linkParentChild(dcl, sgnode, &SgFunctionDeclaration::set_functionParameterScope);
    return sgnode;
  }

  SgFunctionDeclaration&
  mkProcedureInternal( const std::string& nm,
                       SgScopeStatement& scope,
                       SgType& retty,
                       std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete,
                       //~ SgScopeStatement& (*scopeMaker) (SgFunctionDeclaration&)
                       SgScopeStatement& (*scopeMaker) ()
                     )
  {
    SgFunctionParameterList& lst       = mkFunctionParameterList();
    //~ SgScopeStatement&        parmScope = scopeMaker(sgnode);
    SgScopeStatement&        parmScope = scopeMaker();

    complete(lst, parmScope);

    SgFunctionDeclaration&   sgnode    = SG_DEREF(sb::buildNondefiningFunctionDeclaration(nm, &retty, &lst, &scope, nullptr));

    ADA_ASSERT(sgnode.get_type() != nullptr);

    linkParameterScope(sgnode, lst, parmScope);

    markCompilerGenerated(lst); // this is overwritten in buildNondefiningFunctionDeclaration
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
  SgSymbol*              baseSy = ndef.search_for_symbol_from_symbol_table();
  SgFunctionSymbol&      funcSy = SG_DEREF(isSgFunctionSymbol(baseSy));

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
  SgFunctionDeclaration& sgnode = mkProcedureDef(ndef, scope, retty, std::move(complete));

  return sgnode;
}

// MS: 12/20/2020 Ada function renaming declaration maker
SgAdaFunctionRenamingDecl&
mkAdaFunctionRenamingDecl( const std::string& name,
                           SgScopeStatement& scope,
                           SgType& retty,
                           std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
                           )
{
  SgAdaFunctionRenamingDecl& sgnode = mkLocatedNode<SgAdaFunctionRenamingDecl>(name, nullptr, nullptr);
  SgFunctionParameterList&   lst    = SG_DEREF(sgnode.get_parameterList());
  SgFunctionParameterScope&  psc    = mkScopeStmt<SgFunctionParameterScope>(&mkFileInfo());
  ADA_ASSERT(sgnode.get_functionParameterScope() == nullptr);

  sg::linkParentChild<SgFunctionDeclaration>(sgnode, psc, &SgFunctionDeclaration::set_functionParameterScope);
  complete(lst, psc);

  SgFunctionType& funty = mkAdaFunctionRenamingDeclType(retty, lst);
  sgnode.set_type(&funty);
  ADA_ASSERT(sgnode.get_parameterList_syntax() == nullptr);

  SgFunctionSymbol *funsy = scope.find_symbol_by_type_of_function<SgFunctionDeclaration>(name, &funty, NULL, NULL);
  ADA_ASSERT(funsy == nullptr);

  funsy = &mkBareNode<SgFunctionSymbol>(&sgnode);
  scope.insert_symbol(name, funsy);
  sgnode.set_scope(&scope);
  sgnode.set_definingDeclaration(&sgnode);
  sgnode.unsetForward();

  markCompilerGenerated(lst);
  return sgnode;
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
  SgFunctionParameterScope& psc    = mkScopeStmt<SgFunctionParameterScope>(&mkFileInfo());

  ADA_ASSERT(sgnode.get_functionParameterScope() == nullptr);
  sg::linkParentChild<SgFunctionDeclaration>(sgnode, psc, &SgFunctionDeclaration::set_functionParameterScope);

  complete(lst, psc);

  SgFunctionType&           funty  = mkAdaEntryType(lst);

  sgnode.set_type(&funty);

  //~ ADA_ASSERT(sgnode.get_parameterList() == nullptr);
  //~ sg::linkParentChild<SgFunctionDeclaration>(sgnode, lst, &SgFunctionDeclaration::set_parameterList);

  // not used
  ADA_ASSERT(sgnode.get_parameterList_syntax() == nullptr);

  //~ SgFunctionSymbol*         funsy  = scope.find_symbol_by_type_of_function<SgAdaEntryDecl>(name, &funty, NULL, NULL);
  SgFunctionSymbol*         funsy  = scope.find_symbol_by_type_of_function<SgFunctionDeclaration>(name, &funty, NULL, NULL);

  ADA_ASSERT(funsy == nullptr);
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
  SgAdaAcceptStmt&          sgnode = mkScopeStmt<SgAdaAcceptStmt>();
  SgFunctionParameterScope& psc    = mkScopeStmt<SgFunctionParameterScope>(&mkFileInfo());
  SgFunctionParameterList&  lst    = mkFunctionParameterList();

  ADA_ASSERT(sgnode.get_parameterScope() == nullptr);
  sg::linkParentChild(sgnode, psc, &SgAdaAcceptStmt::set_parameterScope);

  ADA_ASSERT(sgnode.get_parameterList() == nullptr);
  sg::linkParentChild(sgnode, lst, &SgAdaAcceptStmt::set_parameterList);

  sg::linkParentChild(sgnode, ref, &SgAdaAcceptStmt::set_entry);
  sg::linkParentChild(sgnode, idx, &SgAdaAcceptStmt::set_index);
  return sgnode;
}


SgCatchOptionStmt&
mkExceptionHandler(SgInitializedName& parm, SgBasicBlock& body)
{
  SgCatchOptionStmt&     sgnode = SG_DEREF( sb::buildCatchOptionStmt(nullptr, &body) );

  setSymbolTableCaseSensitivity(sgnode);

  SgVariableDeclaration& exparm = mkVarDecl(parm, sgnode);

  sg::linkParentChild(sgnode, exparm, &SgCatchOptionStmt::set_condition);
  return sgnode;
}

namespace
{
  struct InitMaker : sg::DispatchHandler<SgInitializer*>
  {
      using base = sg::DispatchHandler<SgInitializer*>;

      explicit
      InitMaker(SgType& varty)
      : base(), vartype(&varty)
      {}

      void handle(SgNode& n)        { SG_UNEXPECTED_NODE(n); }
      void handle(SgExpression& n)  { res = &mkLocatedNode<SgAssignInitializer>(&n, vartype); }
      void handle(SgInitializer& n) { res = &n; /* can this happen? */ }
      void handle(SgExprListExp& n) { res = sb::buildAggregateInitializer(&n); }

    private:
      SgType* vartype;
  };

  SgInitializer* mkInitializerAsNeeded(SgType& vartype, SgExpression* n)
  {
    if (n == nullptr)
      return nullptr;

    SgInitializer* res = sg::dispatch(InitMaker(vartype), n);

    ADA_ASSERT(res);
    return res;
  }
}

SgInitializedName&
mkInitializedName(const std::string& varname, SgType& vartype, SgExpression* val)
{
  ADA_ASSERT(! (val && val->isTransformation()));
  SgInitializer*     varinit = mkInitializerAsNeeded(vartype, val);
  SgInitializedName& sgnode  = SG_DEREF( sb::buildInitializedName_nfi(varname, &vartype, varinit) );

  //~ sgnode.set_type(&vartype);
  //~ if (varinit)
    //~ markCompilerGenerated(*varinit);

  markCompilerGenerated(sgnode);
  return sgnode;
}

namespace
{
  /// adds initialized names to a variable declarations and sets declptr and parent nodes
  template <class FwdIterator>
  void setInitializedNamesInDecl(FwdIterator aa, FwdIterator zz, SgVariableDeclaration& dcl)
  {
    SgInitializedNamePtrList& names = dcl.get_variables();

    std::for_each( aa, zz,
                   [&](SgInitializedName* ini) -> void
                   {
                     ADA_ASSERT(ini);
                     ini->set_declptr(&dcl);
                     ini->set_parent(&dcl);
                     names.push_back(ini);
                   }
                 );
  }

  template <class SageVariableDeclaration, class FwdIterator, class... Args>
  SageVariableDeclaration&
  mkVarExceptionDeclInternal(FwdIterator aa, FwdIterator zz, SgScopeStatement& scope, Args&&... args)
  {
    using SageNode = SageVariableDeclaration;

    SageNode& vardcl = mkLocatedNode<SageNode>(&mkFileInfo(), std::forward<Args>(args)...);
    bool      isNullDecl = std::distance(aa, zz) == 0;

    if (!isNullDecl)
    {
      setInitializedNamesInDecl(aa, zz, vardcl);
      si::fixVariableDeclaration(&vardcl, &scope);
    }

    vardcl.set_parent(&scope);
    return vardcl;
  }

  template <class SageVariableDeclaration, class FwdIterator, class... Args>
  SageVariableDeclaration&
  mkVarDeclInternal(FwdIterator aa, FwdIterator zz, SgScopeStatement& scope, Args&&... args)
  {
    using SageNode = SageVariableDeclaration;

    SageNode& vardcl = mkVarExceptionDeclInternal<SageNode>(aa, zz, scope, std::forward<Args>(args)...);

    ADA_ASSERT(vardcl.get_definingDeclaration() == nullptr);
    ADA_ASSERT(vardcl.get_firstNondefiningDeclaration() == nullptr);
    vardcl.set_firstNondefiningDeclaration(&vardcl);

    return vardcl;
  }
} // anonymous namespace

SgVariableDeclaration&
mkParameter( const SgInitializedNamePtrList& parms,
             SgTypeModifier parmmode,
             SgScopeStatement& scope
           )
{
  SgVariableDeclaration&    parmDecl = mkLocatedNode<SgVariableDeclaration>(&mkFileInfo());

  setInitializedNamesInDecl(parms.begin(), parms.end(), parmDecl);

  SgDeclarationModifier&    declMods = parmDecl.get_declarationModifier();

  declMods.get_typeModifier() = parmmode;

  si::fixVariableDeclaration(&parmDecl, &scope);
  parmDecl.set_parent(&scope);

  ADA_ASSERT(parmDecl.get_definingDeclaration() == nullptr);
  ADA_ASSERT(parmDecl.get_firstNondefiningDeclaration() == nullptr);

  parmDecl.set_firstNondefiningDeclaration(&parmDecl);
  return parmDecl;
}

SgVariableDeclaration&
mkVarDecl(const SgInitializedNamePtrList& vars, SgScopeStatement& scope)
{
  return mkVarDeclInternal<SgVariableDeclaration>(vars.begin(), vars.end(), scope);
}

/// creates a variant field with (i.e., a variable with conditions)
SgAdaVariantFieldDecl&
mkAdaVariantFieldDecl(const SgInitializedNamePtrList& vars, SgExprListExp& choices, SgScopeStatement& scope)
{
  SgAdaVariantFieldDecl& sgnode = mkVarDeclInternal<SgAdaVariantFieldDecl>(vars.begin(), vars.end(), scope, &choices);

  choices.set_parent(&sgnode);
  return sgnode;
}

SgAdaVariantFieldDecl&
mkAdaVariantFieldDecl(SgExprListExp& choices, SgScopeStatement& scope)
{
  SgInitializedNamePtrList empty;

  return mkAdaVariantFieldDecl(empty, choices, scope);
}


SgVariableDeclaration&
mkVarDecl(SgInitializedName& var, SgScopeStatement& scope)
{
  SgInitializedName*  alias = &var;
  SgInitializedName** aa    = &alias;

  return mkVarDeclInternal<SgVariableDeclaration>(aa, aa+1, scope);
}

SgVariableDeclaration&
mkExceptionDecl(const SgInitializedNamePtrList& vars, SgScopeStatement& scope)
{
  return mkVarExceptionDeclInternal<SgVariableDeclaration>(vars.begin(), vars.end(), scope);
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
mkAdaRecordRepresentationClause(SgType& record, SgExpression& align)
{
  SgBasicBlock&                    elems  = mkBasicBlock();
  SgAdaRecordRepresentationClause& sgnode = mkLocatedNode<SgAdaRecordRepresentationClause>(&record, &align, &elems);

  align.set_parent(&sgnode);
  elems.set_parent(&sgnode);
  return sgnode;
}

SgAdaEnumRepresentationClause&
mkAdaEnumRepresentationClause(SgType& enumtype, SgExprListExp& initlst)
{
  SgAdaEnumRepresentationClause& sgnode = mkLocatedNode<SgAdaEnumRepresentationClause>(&enumtype, &initlst);

  initlst.set_parent(&sgnode);
  return sgnode;
}


SgAdaLengthClause&
mkAdaLengthClause(SgAdaAttributeExp& attr, SgExpression& size)
{
  SgAdaLengthClause& sgnode = mkLocatedNode<SgAdaLengthClause>(&attr, &size);

  attr.set_parent(&sgnode);
  size.set_parent(&sgnode);
  return sgnode;
}

SgPragmaDeclaration&
mkPragmaDeclaration(const std::string& name, SgExprListExp& args)
{
  SgPragma&            details = mkBareNode<SgPragma>(std::ref(name));
  SgPragmaDeclaration& sgnode  = mkLocatedNode<SgPragmaDeclaration>(&details);

  details.set_parent(&sgnode);
  sg::linkParentChild(details, args, &SgPragma::set_args);

  sgnode.set_definingDeclaration(&sgnode);
  sgnode.set_firstNondefiningDeclaration(&sgnode);

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
  SgAssignInitializer&     ini    = mkLocatedNode<SgAssignInitializer>(&val, val.get_type());
  SgDesignatedInitializer& sgnode = mkLocatedNode<SgDesignatedInitializer>(&components, &ini);

  val.set_parent(&ini);
  components.set_parent(&sgnode);
  ini.set_parent(&sgnode);
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
  SgRangeExp&   sgnode = SG_DEREF(sb::buildRangeExp(&start));
  SgExpression& stride = SG_DEREF(sb::buildIntVal(1));

  sg::linkParentChild(sgnode, stride, &SgRangeExp::set_stride);
  sg::linkParentChild(sgnode, end,    &SgRangeExp::set_end);

  markCompilerGenerated(stride);
  markCompilerGenerated(sgnode);
  return sgnode;
}

SgRangeExp&
mkRangeExp()
{
  SgExpression& start  = mkNullExpression();
  SgExpression& end    = mkNullExpression();

  return mkRangeExp(start, end);
}


namespace
{
  SgConstructorInitializer&
  mkConstructorInitializer(SgExprListExp& args, SgType& ty)
  {
    SgConstructorInitializer& sgnode = SG_DEREF(sb::buildConstructorInitializer_nfi(nullptr, &args, &ty, false, false, false, false));

    markCompilerGenerated(sgnode);
    return sgnode;
  }
}

SgNewExp&
mkNewExp(SgType& ty, SgExprListExp* args_opt)
{
  SgConstructorInitializer* init = args_opt ? &mkConstructorInitializer(*args_opt, ty)
                                            : nullptr;

  return mkLocatedNode<SgNewExp>(&ty, nullptr /*placement*/, init, nullptr, 0 /* no global */, nullptr);
}


SgAdaOthersExp&
mkAdaOthersExp()
{
  return mkLocatedNode<SgAdaOthersExp>();
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
  return mkLocatedNode<SgAdaTaskRefExp>(&task);
}

SgAdaProtectedRefExp&
mkAdaProtectedRefExp(SgAdaProtectedSpecDecl& po)
{
  return mkLocatedNode<SgAdaProtectedRefExp>(&po);
}

SgAdaUnitRefExp&
mkAdaUnitRefExp(SgDeclarationStatement& unit)
{
  return mkLocatedNode<SgAdaUnitRefExp>(&unit);
}

SgAdaRenamingRefExp&
mkAdaRenamingRefExp(SgAdaRenamingDecl& decl)
{
  return mkLocatedNode<SgAdaRenamingRefExp>(&decl);
}


SgCastExp&
mkCastExp(SgExpression& expr, SgType& ty)
{
  return SG_DEREF(sb::buildCastExp_nfi(&expr, &ty, SgCastExp::e_static_cast));
}


SgExpression&
mkQualifiedExp(SgExpression& expr, SgType& ty)
{
  return SG_DEREF(sb::buildCastExp_nfi(&expr, &ty, SgCastExp::e_ada_type_qualification));
}


namespace
{
  SgCommaOpExp* commaOpExpMaker(SgExpression* lhs, SgExpression* rhs)
  {
    ADA_ASSERT(lhs && rhs);

    SgCommaOpExp* sgnode = sb::buildCommaOpExp(lhs, rhs);

    markCompilerGenerated(SG_DEREF(sgnode));
    return sgnode;
  }
}

SgExpression& mkChoiceExpIfNeeded(const SgExpressionPtrList& choices)
{
  ADA_ASSERT(choices.size() > 0);

  return SG_DEREF( std::accumulate( choices.begin()+1, choices.end(),
                                    choices.front(),
                                    commaOpExpMaker
                                  ));
}


SgUnaryOp&
mkForLoopIncrement(bool forward, SgVariableDeclaration& var)
{
  static constexpr SgUnaryOp::Sgop_mode mode = SgUnaryOp::prefix;

  SgVarRefExp& varref = SG_DEREF( sb::buildVarRefExp(&var) );
  SgUnaryOp*   sgnode = forward ? static_cast<SgUnaryOp*>(sb::buildPlusPlusOp(&varref, mode))
                                : sb::buildMinusMinusOp(&varref, mode)
                                ;
  ADA_ASSERT(sgnode);

  markCompilerGenerated(varref);
  markCompilerGenerated(*sgnode);
  return *sgnode;
}


SgExprListExp&
mkExprListExp(const std::vector<SgExpression*>& exprs)
{
  SgExprListExp& sgnode = SG_DEREF(sb::buildExprListExp(exprs));

  markCompilerGenerated(sgnode);
  return sgnode;
}

SgNullExpression&
mkNullExpression()
{
  SgNullExpression& sgnode = SG_DEREF(sb::buildNullExpression());

  markCompilerGenerated(sgnode);
  return sgnode;
}


namespace
{
  struct IntegerValue : sg::DispatchHandler<long long int>
  {
    void handle(const SgNode& n)           { SG_UNEXPECTED_NODE(n); }
    void handle(const SgShortVal& n)       { res = n.get_value(); }
    void handle(const SgIntVal& n)         { res = n.get_value(); }
    void handle(const SgLongIntVal& n)     { res = n.get_value(); }
    void handle(const SgLongLongIntVal& n) { res = n.get_value(); }
  };

  long long int getIntegralValue(SgInitializedName& enumitem)
  {
    SgAssignInitializer& ini = SG_DEREF( isSgAssignInitializer(enumitem.get_initializer()) );

    return sg::dispatch(IntegerValue{}, ini.get_operand());
  }
};

SgExpression&
mkEnumeratorRef(SgEnumDeclaration& enumdecl, SgInitializedName& enumitem)
{
  using rose_rep_t = decltype(std::declval<SgEnumVal>().get_value());

  const long long int enumval = getIntegralValue(enumitem);
  ROSE_ASSERT(  (enumval >= std::numeric_limits<rose_rep_t>::min())
             && (enumval <= std::numeric_limits<rose_rep_t>::max())
             && ("integral value over-/underflow during conversion")
             );

  return SG_DEREF( sb::buildEnumVal_nfi(enumval, &enumdecl, enumitem.get_name()) );
}

SgExpression&
mkEnumeratorRef_repclause(SgEnumDeclaration&, SgInitializedName& enumitem)
{
  return SG_DEREF( sb::buildVarRefExp(&enumitem, nullptr /* not needed */) );
}


SgAdaAttributeExp&
mkAdaAttributeExp(SgExpression& expr, const std::string& ident, SgExprListExp& args)
{
  SgAdaAttributeExp& sgnode = mkLocatedNode<SgAdaAttributeExp>(ident, &expr, &args);
  //~ SG_DEREF(sb::buildTypeTraitBuiltinOperator(ident, { &expr, &args }));

  expr.set_parent(&sgnode);
  args.set_parent(&sgnode);

  markCompilerGenerated(sgnode);
  return sgnode;
}


namespace
{
  SgFunctionParameterTypeList&
  mkFunctionParameterTypeList()
  {
    return mkBareNode<SgFunctionParameterTypeList>();
  }

  SgType&
  convertType(SgType& actual, SgType& orig, SgTypedefType& derv)
  {
    return &orig == &actual ? derv : actual;
  }


  /// replaces the original type of \ref declaredDerivedType with \ref declaredDerivedType in \ref funcTy.
  /// returns \ref funcTy to indicate an error.
  SgFunctionType&
  convertToDerivedType(SgFunctionType& funcTy, SgTypedefType& declaredDerivedType)
  {
    SgDeclarationStatement* baseTypeDecl = si::ada::baseDeclaration(declaredDerivedType.get_base_type());

    if (baseTypeDecl == nullptr)
      return funcTy;

    SgType*              origTypePtr  = si::getDeclaredType(baseTypeDecl);
    SgType&              originalType = SG_DEREF(origTypePtr);
    SgType&              origRetTy    = SG_DEREF(funcTy.get_return_type());
    SgType&              dervRetTy    = convertType(origRetTy, originalType, declaredDerivedType);
    int                  numUpdTypes  = (&dervRetTy != &origRetTy);
    std::vector<SgType*> newTypeList;

    for (SgType* origArgTy : funcTy.get_arguments())
    {
      SgType* newArgTy = &convertType(SG_DEREF(origArgTy), originalType, declaredDerivedType);

      newTypeList.push_back(newArgTy);
      if (newArgTy != origArgTy) ++numUpdTypes;
    }

    // only create new nodes if everything worked
    if (numUpdTypes == 0)
      return funcTy;

    SgFunctionParameterTypeList& paramTyLst  = mkFunctionParameterTypeList();

    // \todo could we just swap the lists?
    for (SgType* argTy : newTypeList)
      paramTyLst.append_argument(argTy);

    return SG_DEREF( sb::buildFunctionType(&dervRetTy, &paramTyLst) );
  }
}


SgAdaInheritedFunctionSymbol&
mkAdaInheritedFunctionSymbol(SgFunctionDeclaration& fn, SgTypedefType& declaredDerivedType, SgScopeStatement& scope)
{
  SgFunctionType& functy = SG_DEREF(fn.get_type());
  SgFunctionType& dervty = convertToDerivedType(functy, declaredDerivedType);

  if (&functy == &dervty)
  {
    // \todo in a first step, just report the errors in the log.
    //       => fix this issues for all ROSE and ACATS tests.
    logError() << "Inherited subroutine w/o type modification: " << fn.get_name()
               << std::endl;
  }

  SgAdaInheritedFunctionSymbol& sgnode = mkBareNode<SgAdaInheritedFunctionSymbol>(&fn, &dervty);

  scope.insert_symbol(fn.get_name(), &sgnode);
  sgnode.set_parent(&scope);
  return sgnode;
}


//
// specialized templates

template <>
SgStringVal& mkValue<SgStringVal>(const char* textrep)
{
  ADA_ASSERT(textrep);

  const char delimiter = *textrep;
  ADA_ASSERT(delimiter == '"' || delimiter == '%');

  SgStringVal& sgnode = mkLocatedNode<SgStringVal>(si::ada::convertStringLiteral(textrep));

  sgnode.set_stringDelimiter(delimiter);
  return sgnode;
}


template <>
long double convAdaLiteral<long double>(const char* img)
{
  return si::ada::convertRealLiteral(img);
}


template <>
char convAdaLiteral<char>(const char* img)
{
  return si::ada::convertCharLiteral(img);
}

namespace
{
  template <class SageIntegralValue>
  SageIntegralValue*
  mkIntegralLiteralIfWithinRange(long long int val, const char* textrep)
  {
    using rose_rep_t = decltype(std::declval<SageIntegralValue>().get_value());

    const bool withinRange = (  (val >= std::numeric_limits<rose_rep_t>::min())
                             && (val <= std::numeric_limits<rose_rep_t>::max())
                             );

    if (!withinRange) return nullptr;

    return &mkLocatedNode<SageIntegralValue>(val, textrep);
  }
}

SgValueExp&
mkAdaIntegerLiteral(const char* textrep)
{
  static constexpr bool MakeSmallest = false;

  ADA_ASSERT(textrep);

  long long int val = si::ada::convertIntegerLiteral(textrep);
  SgValueExp*   res = nullptr;

  MakeSmallest
  //~ || (res = mkIntegralLiteralIfWithinRange<SgShortVal>      (val, textrep))
  || (res = mkIntegralLiteralIfWithinRange<SgIntVal>        (val, textrep))
  || (res = mkIntegralLiteralIfWithinRange<SgLongIntVal>    (val, textrep))
  || (res = mkIntegralLiteralIfWithinRange<SgLongLongIntVal>(val, textrep))
  ;

  if (!res)
  {
    logError() << "Unable to represent " << textrep << " within the bounds of long long int"
               << std::endl;
    ROSE_ABORT();
  }

  return *res;
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
