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
  /// links a first nondefining declaration to a definition and vice versa
  template <class SageSymbol, class SageDeclarationStatement>
  void linkDeclDef(SageSymbol& sym, SageDeclarationStatement& defdcl)
  {
    SageDeclarationStatement& nondef = dynamic_cast<SageDeclarationStatement&>(*sym.get_declaration());

    // defining and first non-defining must differ
    ADA_ASSERT(&nondef != &defdcl);

    nondef.set_definingDeclaration(&defdcl);
    defdcl.set_firstNondefiningDeclaration(&nondef);
  }

  /// \private
  /// links a secondary nondefining declaration to a first nondefining declaration
  void linkDeclDecl(SgFunctionSymbol& funcsy, SgFunctionDeclaration& func)
  {
    SgFunctionDeclaration& sdcl = SG_DEREF(funcsy.get_declaration());
    ADA_ASSERT(&sdcl != &func);
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

namespace
{
  template <class SageFixedPointConstraint, class... Other>
  SageFixedPointConstraint&
  mkFixedPointConstraint(SgExpression& exp, SgAdaTypeConstraint* sub_opt, Other... other)
  {
    SageFixedPointConstraint& sgnode = mkLocatedNode<SageFixedPointConstraint>(&exp, sub_opt, other...);

    exp.set_parent(&sgnode);
    if (sub_opt) sub_opt->set_parent(&sgnode);

    return sgnode;
  }
}

SgAdaDigitsConstraint&
mkAdaDigitsConstraint(SgExpression& digits, SgAdaTypeConstraint* sub_opt)
{
  return mkFixedPointConstraint<SgAdaDigitsConstraint>(digits, sub_opt);
}

SgAdaDeltaConstraint&
mkAdaDeltaConstraint(SgExpression& delta, bool isDecimal, SgAdaTypeConstraint* sub_opt)
{
  return mkFixedPointConstraint<SgAdaDeltaConstraint>(delta, sub_opt, isDecimal);
}

SgAdaNullConstraint&
mkAdaNullConstraint()
{
  return mkLocatedNode<SgAdaNullConstraint>();
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
mkExprAsType(SgExpression& n)
{
  return mkNonSharedTypeNode<SgDeclType>(&n);
}


SgTypeDefault&
mkOpaqueType()
{
  // not in sage builder
  return mkTypeNode<SgTypeDefault>();
}

SgAdaDiscreteType&
mkAdaDiscreteType()
{
  return mkTypeNode<SgAdaDiscreteType>();
}

SgType& mkQualifiedType(SgExpression& qual, SgType& base)
{
  SgTypeExpression& baseexp   = mkTypeExpression(base);
  SgDotExp&         qualified = SG_DEREF(sb::buildDotExp(&qual, &baseexp));
  //~ SgExprListExp     list     = mkExprListExp({&namequal, &baseexp});
  SgDeclType&       sgnode   = mkNonSharedTypeNode<SgDeclType>(&qualified);

  return sgnode;
}


SgTypeVoid&
mkTypeVoid()
{
  return SG_DEREF(sb::buildVoidType());
}

SgTypeUnknown&
mkTypeUnknown()
{
  return mkTypeNode<SgTypeUnknown>();
}

SgTypeUnknown&
mkUnresolvedType(const std::string& n)
{
  SgTypeUnknown& sgnode = mkNonSharedTypeNode<SgTypeUnknown>();

  sgnode.set_has_type_name(true);
  sgnode.set_type_name(n);

  return sgnode;
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
  // \todo PP (28/1/22) this may need to be a shared type node
  SgAdaAccessType& sgnode = mkNonSharedTypeNode<SgAdaAccessType>(&base_type);
  return sgnode;
}

namespace
{

SgFunctionType& mkAdaEntryType(SgType& /* indexType ignored */, SgFunctionParameterList& lst)
{
  // \todo build entry type
  return SG_DEREF(sb::buildFunctionType(&mkTypeVoid(), &lst));
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
  // if this type is changed, the type comparison in si::Ada::isIntegerType
  // needs to be updated.
  return SG_DEREF(sb::buildLongLongType());
}

SgType& mkRealType()
{
  // if this type is changed, the type comparison in si::Ada::isFloatingPointType
  // needs to be updated.
  return SG_DEREF(sb::buildLongDoubleType());
}

SgType& mkFixedType()
{
  // return mkTypeNode<SgTypeFixed>(nullptr, nullptr);
  return SG_DEREF(sb::buildFixedType(nullptr, nullptr));
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
mkIfStmt(bool elseIfPath)
// SgExpression& cond, SgStatement& thenBranch, SgStatement* elseBranch_opt)
{
  SgIfStmt& sgnode = SG_DEREF( sb::buildIfStmt_nfi(nullptr, nullptr, nullptr) );

  markCompilerGenerated(sgnode);
  setSymbolTableCaseSensitivity(sgnode);
  sgnode.set_is_else_if_statement(elseIfPath);
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
  SgUsingDeclarationStatement& sgnode = mkLocatedNode<SgUsingDeclarationStatement>(&used, nullptr);

  sgnode.set_firstNondefiningDeclaration(&sgnode);
  return sgnode;
}


SgAdaExitStmt&
mkAdaExitStmt(SgStatement& loop, SgExpression& cond, bool explicitLoopName)
{
  SgAdaExitStmt& sgnode = mkLocatedNode<SgAdaExitStmt>(&loop, &cond, explicitLoopName);

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

SgAdaSelectStmt&
mkAdaSelectStmt(SgAdaSelectStmt::select_type_enum select_type)
{
  SgAdaSelectStmt& sgnode = mkLocatedNode<SgAdaSelectStmt>();

  sgnode.set_select_type(select_type);
  return sgnode;
}

SgAdaSelectAlternativeStmt&
mkAdaSelectAlternativeStmt(SgExpression& guard, SgBasicBlock& body)
{
  SgAdaSelectAlternativeStmt& sgnode = mkLocatedNode<SgAdaSelectAlternativeStmt>();

  sg::linkParentChild(sgnode, guard, &SgAdaSelectAlternativeStmt::set_guard);
  sg::linkParentChild(sgnode, body, &SgAdaSelectAlternativeStmt::set_body);
  return sgnode;
}



SgAdaDelayStmt&
mkAdaDelayStmt(SgExpression& timeExp, bool relativeTime)
{
  SgAdaDelayStmt& sgnode = mkLocatedNode<SgAdaDelayStmt>(&timeExp, relativeTime);

  sg::linkParentChild(sgnode, timeExp, &SgAdaDelayStmt::set_time);
  return sgnode;
}

namespace
{
  SgProcessControlStatement&
  mkProcessControlStatement(SgExpression& code, SgProcessControlStatement::control_enum kind)
  {
    SgProcessControlStatement& sgnode = mkLocatedNode<SgProcessControlStatement>(&code);

    code.set_parent(&sgnode);
    sgnode.set_control_kind(kind);
    return sgnode;
  }
};

SgProcessControlStatement&
mkAbortStmt(SgExprListExp& abortList)
{
  return mkProcessControlStatement(abortList, SgProcessControlStatement::e_abort);
}

SgProcessControlStatement&
mkRequeueStmt(SgExpression& entryexpr, bool withAbort)
{
  auto kind = withAbort ? SgProcessControlStatement::e_requeue
                        : SgProcessControlStatement::e_requeue_with_abort;

  return mkProcessControlStatement(entryexpr, kind);
};



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
  mkDeclarationScope(SgScopeStatement& /* not used */)
  {
    SgDeclarationScope& sgnode = mkScopeStmt<SgDeclarationScope>();

    //~ sgnode.set_scope(&outer);
    return sgnode;
  }

  SgAdaParameterList&
  mkAdaParameterList(SgScopeStatement& /* not used */)
  {
    SgAdaParameterList& sgnode = mkLocatedNode<SgAdaParameterList>();

    //~ sgnode.set_scope(&outer);
    return sgnode;
  }
}

SgAdaDiscriminatedTypeDecl&
mkAdaDiscriminatedTypeDecl(SgScopeStatement& scope)
{
  SgDeclarationScope&         dclscope = mkDeclarationScope(scope);
  SgAdaParameterList&         params   = mkAdaParameterList(dclscope);
  SgAdaDiscriminatedTypeDecl& sgnode   = mkLocatedNode<SgAdaDiscriminatedTypeDecl>(&dclscope, &params);

  dclscope.set_parent(&sgnode);
  params.set_parent(&sgnode);
  return sgnode;
}

SgAdaGenericInstanceDecl&
mkAdaGenericInstanceDecl(const std::string& name, SgDeclarationStatement& gendecl, SgScopeStatement& scope)
{
  ROSE_ASSERT(  isSgAdaGenericDecl(&gendecl)
             || isSgAdaRenamingDecl(&gendecl)
             );

  //~ SgDeclarationScope&       dclscope = mkDeclarationScope(scope);
  SgScopeStatement&         dclscope = mkBasicBlock();
  SgAdaGenericInstanceDecl& sgnode = mkLocatedNode<SgAdaGenericInstanceDecl>(name,&gendecl,&dclscope);

  sg::linkParentChild(sgnode, dclscope, &SgAdaGenericInstanceDecl::set_instantiatedScope);

  // PP (2/24/22): should this be set_definingDeclaration ?
  //               I think not, since we only extract the spec (possibly the impl for procedures...)
  sgnode.set_firstNondefiningDeclaration(&sgnode);

  // \todo not sure if needs an explicit scope
  //       if not: fix also in nameQualifcationSupport.C, Statement.code, unparseAdaStatement.C
  sgnode.set_scope(&scope);

  scope.insert_symbol(name, &mkBareNode<SgAdaGenericInstanceSymbol>(&sgnode));
  return sgnode;
}

SgAdaFormalPackageDecl&
mkAdaFormalPackageDecl(const std::string& name, SgDeclarationStatement& gendecl, SgExprListExp& args, SgScopeStatement& scope)
{
  ROSE_ASSERT(  isSgAdaGenericDecl(&gendecl)
             || isSgAdaRenamingDecl(&gendecl)
             );

  SgAdaFormalPackageDecl& sgnode = mkLocatedNode<SgAdaFormalPackageDecl>(name, &gendecl, &args);
  //~ SgScopeStatement&       dclscope = mkDeclarationScope(scope);
  SgScopeStatement&       dclscope = mkBasicBlock();

  sg::linkParentChild(sgnode, dclscope, &SgAdaFormalPackageDecl::set_prototypeScope);
  sgnode.set_firstNondefiningDeclaration(&sgnode);
  scope.insert_symbol(name, &mkBareNode<SgAdaFormalPackageSymbol>(&sgnode));
  return sgnode;
}


SgAdaGenericDecl&
mkAdaGenericDecl(const std::string& name, SgScopeStatement& scope)
{
   SgAdaGenericDefn&   defn   = mkScopeStmt<SgAdaGenericDefn>();
   SgAdaGenericDecl&   sgnode = mkLocatedNode<SgAdaGenericDecl>(name, &defn);

   sg::linkParentChild(sgnode, defn, &SgAdaGenericDecl::set_definition);

   sgnode.set_firstNondefiningDeclaration(&sgnode);
   scope.insert_symbol(name, &mkBareNode<SgAdaGenericSymbol>(&sgnode));
   return sgnode;
}

SgAdaFormalTypeDecl&
mkAdaFormalTypeDecl(const std::string& name, SgScopeStatement& scope)
{
  SgAdaFormalTypeDecl&  sgnode = mkLocatedNode<SgAdaFormalTypeDecl>(SgName(name));

  sgnode.set_parent(&scope);
  sgnode.set_firstNondefiningDeclaration(&sgnode);
  scope.insert_symbol(name, &mkBareNode<SgAdaGenericSymbol>(&sgnode));

  return sgnode;
}


SgAdaRenamingDecl&
mkAdaRenamingDecl(const std::string& name, SgExpression& renamed, SgType* ty, SgScopeStatement& scope)
{
  if (ty == nullptr) ty = &mkTypeVoid();

  SgAdaRenamingDecl& sgnode = mkLocatedNode<SgAdaRenamingDecl>(name, &renamed, ty);

  renamed.set_parent(&sgnode);
  sgnode.set_parent(&scope);
  sgnode.set_firstNondefiningDeclaration(&sgnode);
  scope.insert_symbol(name, &mkBareNode<SgAdaRenamingSymbol>(&sgnode));
  return sgnode;
}


namespace
{
  template <class SageAdaBodyDecl>
  void linkBodyDeclDef_opt(SageAdaBodyDecl* nondef, SageAdaBodyDecl& defn)
  {
    // \todo nondef needs to be generated by the caller if not set
    if (nondef)
    {
      nondef->set_definingDeclaration(&defn);
      defn.set_firstNondefiningDeclaration(nondef);
      defn.set_definingDeclaration(&defn);
    }
    else
    {
      defn.set_firstNondefiningDeclaration(&defn);
    }
  }

  template <class SageAdaBodySymbol, class SageAdaBodyDecl>
  void insertBodySymbol_opt(SageAdaBodyDecl& sgnode, SgScopeStatement& scope)
  {
    // \todo is it sufficient that the scope contains the same name already?
    if (!scope.symbol_exists(sgnode.get_name()))
      scope.insert_symbol(sgnode.get_name(), &mkBareNode<SageAdaBodySymbol>(&sgnode));
  }
}


SgAdaPackageBodyDecl&
mkAdaPackageBodyDecl_nondef(SgAdaPackageSpecDecl& specdcl, SgScopeStatement& scope)
{
  SgAdaPackageBodyDecl& sgnode  = mkLocatedNode<SgAdaPackageBodyDecl>(specdcl.get_name(), nullptr);

  sgnode.set_scope(&scope);

  insertBodySymbol_opt<SgAdaPackageSymbol>(sgnode, scope);
  return sgnode;
}

SgAdaPackageBodyDecl&
mkAdaPackageBodyDecl(SgAdaPackageSpecDecl& specdcl, SgAdaPackageBodyDecl* nondef_opt, SgScopeStatement& scope)
{
  SgAdaPackageBody&     pkgbody = mkScopeStmt<SgAdaPackageBody>();
  SgAdaPackageBodyDecl& sgnode  = mkLocatedNode<SgAdaPackageBodyDecl>(specdcl.get_name(), &pkgbody);
  SgAdaPackageSpec&     pkgspec = SG_DEREF( specdcl.get_definition() );

  sgnode.set_scope(&scope);
  pkgbody.set_parent(&sgnode);
  pkgspec.set_body(&pkgbody);
  pkgbody.set_spec(&pkgspec);

  linkBodyDeclDef_opt(nondef_opt, sgnode);
  insertBodySymbol_opt<SgAdaPackageSymbol>(sgnode, scope);
  return sgnode;
}


namespace
{
  template <class SageAdaConcurrentSymbol, class SageAdaConcurrentDecl, class SageAdaConcurrentSpec>
  SageAdaConcurrentDecl&
  mkAdaConcurrentDeclInternal(const std::string& name, SageAdaConcurrentSpec* spec_opt, SgScopeStatement& scope)
  {
    SageAdaConcurrentDecl& sgnode = mkLocatedNode<SageAdaConcurrentDecl>(name, spec_opt);

    scope.insert_symbol(name, &mkBareNode<SageAdaConcurrentSymbol>(&sgnode));

    if (spec_opt)
    {
      spec_opt->set_parent(&sgnode);
      sgnode.set_definingDeclaration(&sgnode);
    }
    else
    {
      sgnode.set_firstNondefiningDeclaration(&sgnode);
    }

    return sgnode;
  }

  template <class SageAdaConcurrentSymbol, class SageAdaConcurrentDecl, class SageAdaConcurrentSpec>
  SageAdaConcurrentDecl&
  mkAdaConcurrentDeclInternal(SageAdaConcurrentDecl& nondef, SageAdaConcurrentSpec& spec, SgScopeStatement& scope)
  {
    std::string              name   = nondef.get_name();
    SageAdaConcurrentDecl& sgnode = mkLocatedNode<SageAdaConcurrentDecl>(name, &spec);
    SgSymbol*                baseSy = nondef.search_for_symbol_from_symbol_table();
    SageAdaConcurrentSymbol& sym    = dynamic_cast<SageAdaConcurrentSymbol&>(*baseSy);

    //~ scope.insert_symbol(name, &mkBareNode<SageAdaConcurrentSymbol>(&sgnode));
    sgnode.set_definingDeclaration(&sgnode);
    linkDeclDef(sym, sgnode);
    spec.set_parent(&sgnode);
    return sgnode;
  }
}


SgAdaTaskTypeDecl&
mkAdaTaskTypeDecl(const std::string& name, SgAdaTaskSpec* spec_opt, SgScopeStatement& scope)
{
  return mkAdaConcurrentDeclInternal<SgAdaTaskSymbol, SgAdaTaskTypeDecl>(name, spec_opt, scope);
}

SgAdaTaskTypeDecl&
mkAdaTaskTypeDecl(SgAdaTaskTypeDecl& nondef, SgAdaTaskSpec& spec, SgScopeStatement& scope)
{
  return mkAdaConcurrentDeclInternal<SgAdaTaskSymbol>(nondef, spec, scope);
}

SgAdaTaskSpecDecl&
mkAdaTaskSpecDecl(const std::string& name, SgAdaTaskSpec& spec, SgScopeStatement& scope)
{
  return mkAdaConcurrentDeclInternal<SgAdaTaskSymbol, SgAdaTaskSpecDecl>(name, &spec, scope);
}

SgAdaProtectedTypeDecl&
mkAdaProtectedTypeDecl(const std::string& name, SgAdaProtectedSpec* spec_opt, SgScopeStatement& scope)
{
  return mkAdaConcurrentDeclInternal<SgAdaProtectedSymbol, SgAdaProtectedTypeDecl>(name, spec_opt, scope);
}

SgAdaProtectedTypeDecl&
mkAdaProtectedTypeDecl(SgAdaProtectedTypeDecl& nondef, SgAdaProtectedSpec& spec, SgScopeStatement& scope)
{
  return mkAdaConcurrentDeclInternal<SgAdaProtectedSymbol>(nondef, spec, scope);
}

SgAdaProtectedSpecDecl&
mkAdaProtectedSpecDecl(const std::string& name, SgAdaProtectedSpec& spec, SgScopeStatement& scope)
{
  return mkAdaConcurrentDeclInternal<SgAdaProtectedSymbol, SgAdaProtectedSpecDecl>(name, &spec, scope);
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
mkAdaTaskBodyDecl_nondef(SgDeclarationStatement& tskdecl, SgScopeStatement& scope)
{
  TaskDeclInfoResult specinfo = sg::dispatch(TaskDeclInfo{}, &tskdecl);
  SgAdaTaskBodyDecl& sgnode   = mkLocatedNode<SgAdaTaskBodyDecl>(specinfo.name, &tskdecl, nullptr);

  sgnode.set_parent(&scope); // needed?

  insertBodySymbol_opt<SgAdaTaskSymbol>(sgnode, scope);
  return sgnode;
}

SgAdaTaskBodyDecl&
mkAdaTaskBodyDecl( SgDeclarationStatement& tskdecl,
                   SgAdaTaskBodyDecl* nondef_opt,
                   SgAdaTaskBody& tskbody,
                   SgScopeStatement& scope
                 )
{
  // \todo what is the relationship beteen nondef and tskbody_opt?
  // ROSE_ASSERT((nondef != nullptr) == (tskbody_opt != nullptr)) ;

  TaskDeclInfoResult specinfo = sg::dispatch(TaskDeclInfo{}, &tskdecl);
  SgAdaTaskBodyDecl& sgnode   = mkLocatedNode<SgAdaTaskBodyDecl>(specinfo.name, &tskdecl, &tskbody);
  SgAdaTaskSpec&     tskspec = SG_DEREF( specinfo.spec );

  sgnode.set_parent(&scope); // \needed

  tskbody.set_parent(&sgnode);
  tskbody.set_spec(&tskspec);
  tskspec.set_body(&tskbody);

  linkBodyDeclDef_opt(nondef_opt, sgnode);
  insertBodySymbol_opt<SgAdaTaskSymbol>(sgnode, scope);
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
mkAdaProtectedBodyDecl_nondef(SgDeclarationStatement& podecl, SgScopeStatement& scope)
{
  // \todo combine with mkAdaTaskBodyDecl
  ProtectedDeclInfoResult specinfo = sg::dispatch(ProtectedDeclInfo{}, &podecl);
  SgAdaProtectedBodyDecl& sgnode   = mkLocatedNode<SgAdaProtectedBodyDecl>(specinfo.name, nullptr);

  sgnode.set_parent(&scope); // needed?

  insertBodySymbol_opt<SgAdaProtectedSymbol>(sgnode, scope);
  return sgnode;
}


SgAdaProtectedBodyDecl&
mkAdaProtectedBodyDecl( SgDeclarationStatement& podecl,
                        SgAdaProtectedBodyDecl* nondef_opt,
                        SgAdaProtectedBody&     pobody,
                        SgScopeStatement&       scope
                      )
{
  // \todo combine with mkAdaTaskBodyDecl
  ProtectedDeclInfoResult specinfo = sg::dispatch(ProtectedDeclInfo{}, &podecl);
  SgAdaProtectedBodyDecl& sgnode   = mkLocatedNode<SgAdaProtectedBodyDecl>(specinfo.name, &pobody);
  SgAdaProtectedSpec&     pospec = SG_DEREF( specinfo.spec );

  sgnode.set_parent(&scope); // needed?

  pobody.set_parent(&sgnode);
  pobody.set_spec(&pospec);
  pospec.set_body(&pobody);

  linkBodyDeclDef_opt(nondef_opt, sgnode);
  insertBodySymbol_opt<SgAdaProtectedSymbol>(sgnode, scope);
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
  void linkParametersToScope(SgFunctionParameterList& lst, SgScopeStatement& parmScope)
  {
    // the sage builder overrides this information, so we reset it
    // \todo needs to be fixed in the sage builder
    for (SgInitializedName* n : lst.get_args())
      SG_DEREF(n).set_scope(&parmScope);
  }

  void linkParameterScope(SgFunctionDeclaration& decl, SgFunctionParameterList& lst, SgScopeStatement& parmScope)
  {
    parmScope.set_parent(&decl);
    linkParametersToScope(lst, parmScope);

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
  /// helps to create a procedure/function definition by creating the definition
  ///   attaches the definition to the declaration and returns the *function body*.
  SgFunctionDefinition&
  mkProcDecl()
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
  /// helps to create a nondefining function declaration by creating a scope for parameters
  SgFunctionParameterScope&
  mkProcDecl_nondef()
  {
    return mkScopeStmt<SgFunctionParameterScope>(&mkFileInfo());
  }

  SgFunctionDeclaration&
  mkProcedureInternal( const std::string& nm,
                       SgScopeStatement& scope,
                       SgType& retty,
                       std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete,
                       std::function<SgScopeStatement&()> scopeMaker
                     )
  {
    //~ std::cerr << nm << "() -> " << &retty << std::endl;

    SgFunctionParameterList& lst       = mkFunctionParameterList();
    SgScopeStatement&        parmScope = scopeMaker();

    // NOTE: the true parent is set in linkParameterScope; the parameter completion needs
    //       to be able to traverse the scope chain until global scope.
    parmScope.set_parent(&scope);
    complete(lst, parmScope);

    SgFunctionDeclaration&   sgnode    = SG_DEREF(sb::buildNondefiningFunctionDeclaration(nm, &retty, &lst, &scope, nullptr));

    ADA_ASSERT(sgnode.get_type() != nullptr);

    linkParameterScope(sgnode, lst, parmScope);

    markCompilerGenerated(lst); // this is overwritten in buildNondefiningFunctionDeclaration
    markCompilerGenerated(sgnode);
    // std::cerr << nm << "'() -> " << sgnode.get_type()->get_return_type() << std::endl;
    return sgnode;
  }
}

SgAdaSubroutineType&
mkAdaSubroutineType( SgType& retty,
                     std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete,
                     SgScopeStatement& scope,
                     bool isProtected
                   )
{
  SgFunctionParameterList&  lst       = mkFunctionParameterList();
  SgFunctionParameterScope& parmScope = mkProcDecl_nondef();

  // NOTE: the true parent is set underneath; the parameter completion needs
  //       to be able to traverse the scope chain until global scope.
  parmScope.set_parent(&scope);
  complete(lst, parmScope);

  // \todo can we make this a shared type?
  SgAdaSubroutineType&      sgnode = mkNonSharedTypeNode<SgAdaSubroutineType>(&lst, &parmScope, &retty, isProtected);

  lst.set_parent(&sgnode);
  // correct? or shall we link to surrounding scope?
  //~ parmScope.set_parent(&sgnode);
  parmScope.set_parent(&scope);

  return sgnode;
}


SgFunctionDeclaration&
mkProcedureDecl_nondef( const std::string& nm,
                        SgScopeStatement& scope,
                        SgType& retty,
                        std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
                      )
{
  return mkProcedureInternal(nm, scope, retty, std::move(complete), mkProcDecl_nondef);
}

SgFunctionDeclaration&
mkProcedureDecl_nondef( SgFunctionDeclaration& ndef,
                        SgScopeStatement& scope,
                        SgType& retty,
                        std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
                      )
{
  SgName                 nm     = ndef.get_name();
  SgFunctionDeclaration& sgnode = mkProcedureInternal(nm, scope, retty, std::move(complete), mkProcDecl_nondef);
  SgSymbol*              baseSy = ndef.search_for_symbol_from_symbol_table();
  SgFunctionSymbol&      funcSy = SG_DEREF(isSgFunctionSymbol(baseSy));

  linkDeclDecl(funcSy, sgnode);

  ADA_ASSERT(sgnode.get_definingDeclaration() == nullptr);
  ADA_ASSERT(sgnode.isForward());
  return sgnode;
}

SgFunctionDeclaration&
mkProcedureDecl( SgFunctionDeclaration& ndef,
                 SgScopeStatement& scope,
                 SgType& retty,
                 std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
               )
{
  SgName                 nm     = ndef.get_name();
  SgFunctionDeclaration& sgnode = mkProcedureInternal(nm, scope, retty, std::move(complete), mkProcDecl);
  SgSymbol*              baseSy = ndef.search_for_symbol_from_symbol_table();
  SgFunctionSymbol&      funcSy = SG_DEREF(isSgFunctionSymbol(baseSy));

  linkDeclDef(funcSy, sgnode);
  sgnode.set_definingDeclaration(&sgnode);
  sgnode.unsetForward();

  return sgnode;
}

SgFunctionDeclaration&
mkProcedureDecl( const std::string& nm,
                 SgScopeStatement& scope,
                 SgType& retty,
                 std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete
               )
{
  SgFunctionDeclaration& ndef   = mkProcedureDecl_nondef(nm, scope, retty, complete);
  SgFunctionDeclaration& sgnode = mkProcedureDecl(ndef, scope, retty, std::move(complete));

  return sgnode;
}

// MS: 12/20/2020 Ada function renaming declaration maker
// PP:  5/22/2022 added nondef_opt to support renaming-as-body declarations
SgAdaFunctionRenamingDecl&
mkAdaFunctionRenamingDecl( const std::string& name,
                           SgScopeStatement& scope,
                           SgType& retty,
                           std::function<void(SgFunctionParameterList&, SgScopeStatement&)> complete,
                           SgAdaFunctionRenamingDecl* nondef_opt
                         )
{
  SgAdaFunctionRenamingDecl& sgnode = mkLocatedNode<SgAdaFunctionRenamingDecl>(name, nullptr, nullptr);
  SgFunctionParameterList&   lst    = SG_DEREF(sgnode.get_parameterList());
  SgFunctionParameterScope&  psc    = mkScopeStmt<SgFunctionParameterScope>(&mkFileInfo());
  ADA_ASSERT(sgnode.get_functionParameterScope() == nullptr);

  sg::linkParentChild<SgFunctionDeclaration>(sgnode, psc, &SgFunctionDeclaration::set_functionParameterScope);

  // NOTE: the true parent is set in linkParameterScope; the parameter completion needs
  //       to be able to traverse the scope chain until global scope.
  sgnode.set_parent(&scope);
  complete(lst, psc);

  SgFunctionType& funty = mkAdaFunctionRenamingDeclType(retty, lst);
  sgnode.set_type(&funty);
  ADA_ASSERT(sgnode.get_parameterList_syntax() == nullptr);

  if (scope.find_symbol_by_type_of_function<SgFunctionDeclaration>(name, &funty, NULL, NULL))
  {
    logWarn() << "found function symbol " << name << " in scope. Type of scope: "
              << typeid(scope).name()
              << std::endl;
    ADA_ASSERT(nondef_opt);
  }
  else
  {
    SgFunctionSymbol& funsy = mkBareNode<SgFunctionSymbol>(&sgnode);
    scope.insert_symbol(name, &funsy);
  }

  if (nondef_opt)
  {
    SgSymbol*         baseSy = nondef_opt->search_for_symbol_from_symbol_table();
    SgFunctionSymbol& funcSy = SG_DEREF(isSgFunctionSymbol(baseSy));

    // demote to a non-defining declaration
    nondef_opt->set_firstNondefiningDeclaration(nondef_opt);
    nondef_opt->setForward();
    linkDeclDef(funcSy, sgnode);
  }

  sgnode.set_scope(&scope);
  sgnode.set_definingDeclaration(&sgnode);
  sgnode.unsetForward();

  markCompilerGenerated(lst);
  return sgnode;
}

namespace
{
  SgAdaEntryDecl&
  mkAdaEntryDeclInternal( const std::string& name,
                          SgScopeStatement& scope,
                          std::function<void(SgFunctionParameterList&, SgScopeStatement&)> completeParams,
                          std::function<SgInitializedName&(SgScopeStatement&)> genIndex,
                          std::function<SgScopeStatement&()> scopeMaker
                        )
  {
    SgAdaEntryDecl&           sgnode = mkLocatedNode<SgAdaEntryDecl>(name, nullptr /* entry type */, nullptr /* definition */);
    SgFunctionParameterList&  lst    = SG_DEREF(sgnode.get_parameterList());
    SgScopeStatement&         psc    = scopeMaker();

    ADA_ASSERT(sgnode.get_functionParameterScope() == nullptr);
    linkParameterScope(sgnode, lst, psc);

    SgInitializedName&        entryIndexVar  = genIndex(psc);

    sgnode.set_entryIndex(&entryIndexVar);
    sgnode.set_scope(&scope);
    sgnode.set_parent(&scope); // NOTE: needs to be set to enable scope chain traversal
    completeParams(lst, psc);

    SgType&                   entryIndexType = SG_DEREF(entryIndexVar.get_type());
    SgFunctionType&           funty          = mkAdaEntryType(entryIndexType, lst);

    sgnode.set_type(&funty);

    // not used
    ADA_ASSERT(sgnode.get_parameterList_syntax() == nullptr);

    SgFunctionSymbol*         funsy  = scope.find_symbol_by_type_of_function<SgFunctionDeclaration>(name, &funty, NULL, NULL);

    ADA_ASSERT(funsy == nullptr);
    funsy = &mkBareNode<SgFunctionSymbol>(&sgnode);

    scope.insert_symbol(name, funsy);
    markCompilerGenerated(lst);
    return sgnode;
  }
}

SgAdaEntryDecl&
mkAdaEntryDecl( const std::string& name,
                SgScopeStatement& scope,
                std::function<void(SgFunctionParameterList&, SgScopeStatement&)> completeParams,
                SgType& entryIndexType
              )
{
  SgType*         entryIdxType = &entryIndexType;
  auto            genIndex = [entryIdxType](SgScopeStatement& scope) -> SgInitializedName&
                             {
                               SgInitializedName&     res = mkInitializedName("", *entryIdxType, nullptr);
                               /*SgVariableDeclaration& var =*/ mkVarDecl(res, scope);

                               return res;
                             };
  SgAdaEntryDecl& sgnode = mkAdaEntryDeclInternal( name, scope, completeParams, genIndex, mkProcDecl_nondef);

  sgnode.set_firstNondefiningDeclaration(&sgnode);

  return sgnode;
}

SgAdaEntryDecl&
mkAdaEntryDefn( SgAdaEntryDecl& ndef,
                SgScopeStatement& scope,
                std::function<void(SgFunctionParameterList&, SgScopeStatement&)> completeParams,
                std::function<SgInitializedName&(SgScopeStatement&)> genIndex
              )
{
  SgAdaEntryDecl&   sgnode = mkAdaEntryDeclInternal(ndef.get_name(), scope, completeParams, genIndex, mkProcDecl);
  SgSymbol*         baseSy = ndef.search_for_symbol_from_symbol_table();
  SgFunctionSymbol& funcSy = SG_DEREF(isSgFunctionSymbol(baseSy));

  sgnode.set_definingDeclaration(&sgnode);
  sgnode.unsetForward();
  linkDeclDef(funcSy, sgnode);

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
  SgVariableDeclaration& sgnode = mkVarExceptionDeclInternal<SgVariableDeclaration>(vars.begin(), vars.end(), scope);

  //~ sgnode.set_firstNondefiningDeclaration(&sgnode); //??
  sgnode.set_definingDeclaration(&sgnode); //??
  return sgnode;
}

SgAdaVariantDecl&
mkAdaVariantDecl(SgExpression& discr)
{
  SgAdaUnscopedBlock& blk = mkLocatedNode<SgAdaUnscopedBlock>();
  SgAdaVariantDecl&   sgnode = mkLocatedNode<SgAdaVariantDecl>(&discr, &blk);

  discr.set_parent(&sgnode);
  blk.set_parent(&sgnode);
  return sgnode;
}

SgAdaVariantWhenStmt&
mkAdaVariantWhenStmt(SgExprListExp& choices)
{
  SgAdaUnscopedBlock&   blk = mkLocatedNode<SgAdaUnscopedBlock>();
  SgAdaVariantWhenStmt& sgnode = mkLocatedNode<SgAdaVariantWhenStmt>(&choices, &blk);

  choices.set_parent(&sgnode);
  blk.set_parent(&sgnode);
  return sgnode;
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

SgAdaRepresentationClause&
mkAdaRepresentationClause(SgType& record, SgExpression& align, bool isAtClause)
{
  SgBasicBlock*              elems  = !isAtClause ?  &mkBasicBlock() : nullptr;
  SgAdaRepresentationClause& sgnode = mkLocatedNode<SgAdaRepresentationClause>(&record, &align, elems);

  if (!isAtClause) elems->set_parent(&sgnode);
  align.set_parent(&sgnode);
  return sgnode;
}

SgAdaEnumRepresentationClause&
mkAdaEnumRepresentationClause(SgType& enumtype, SgExprListExp& initlst)
{
  SgAdaEnumRepresentationClause& sgnode = mkLocatedNode<SgAdaEnumRepresentationClause>(&enumtype, &initlst);

  initlst.set_parent(&sgnode);
  return sgnode;
}


SgAdaAttributeClause&
mkAdaAttributeClause(SgAdaAttributeExp& attr, SgExpression& size)
{
  SgAdaAttributeClause& sgnode = mkLocatedNode<SgAdaAttributeClause>(&attr, &size);

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
mkRecordParent(SgType& n)
{
  if (SgClassType* clsty = isSgClassType(&n))
  {
    SgClassDeclaration& dcl = SG_DEREF(isSgClassDeclaration(clsty->get_declaration()));

    if (!dcl.get_definingDeclaration())
    {
      logWarn() << "no defining declaration for base class: " << dcl.get_name()
                << std::endl;
    }

    return mkBareNode<SgBaseClass>(&dcl, true /* direct base */);
  }

  return mkBareNode<SgExpBaseClass>(nullptr, true /* direct base */, &mkTypeExpression(n));
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


SgAdaAncestorInitializer&
mkAdaAncestorInitializer(SgExpression& par)
{
  SgAdaAncestorInitializer& sgnode = mkLocatedNode<SgAdaAncestorInitializer>(&par);

  par.set_parent(&sgnode);
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

SgConditionalExp&
mkIfExpr()
{
  return SG_DEREF( sb::buildConditionalExp_nfi(nullptr, nullptr, nullptr, nullptr) );
}


SgExpression&
mkAdaBoxExp()
{
  return SG_DEREF(sb::buildVoidVal());
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


SgFunctionCallExp&
mkFunctionCallExp(SgExpression& target, SgExprListExp& arglst, bool usesOperatorSyntax)
{
  SgFunctionCallExp& sgnode = SG_DEREF(sb::buildFunctionCallExp_nfi(&target, &arglst));

  sgnode.set_uses_operator_syntax(usesOperatorSyntax);
  return sgnode;
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

SgExprStatement&
mkForLoopTest(bool forward, SgVariableDeclaration& var)
{
  SgVarRefExp&         varref = SG_DEREF( sb::buildVarRefExp(&var) );
  SgInitializedName&   inivar = SG_DEREF( var.get_variables().front() );
  SgAssignInitializer& iniini = SG_DEREF( isSgAssignInitializer(inivar.get_initializer()) );
  SgExpression&        range  = SG_DEREF( iniini.get_operand() );
  SgExpression&        rngcp  = SG_DEREF( si::deepCopy(&range) );
  SgExpression&        test   = SG_DEREF( sb::buildMembershipOp(&varref, &rngcp) );
  SgExprStatement&     sgnode = mkExprStatement(test);

  markCompilerGenerated(varref);
  markCompilerGenerated(rngcp);
  markCompilerGenerated(test);
  markCompilerGenerated(sgnode);
  return sgnode;
}



SgExprListExp&
mkExprListExp(const std::vector<SgExpression*>& exprs)
{
  SgExprListExp& sgnode = SG_DEREF(sb::buildExprListExp(exprs));

  markCompilerGenerated(sgnode);
  return sgnode;
}


SgTypeExpression&
mkTypeExpression(SgType& ty)
{
  SgTypeExpression& sgnode = SG_DEREF(sb::buildTypeExpression(&ty)) ;

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
  long long int getIntegralValue(SgInitializedName& enumitem)
  {
    SgAssignInitializer& ini = SG_DEREF( isSgAssignInitializer(enumitem.get_initializer()) );

    return si::Ada::staticIntegralValue(ini.get_operand());
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

#if OBSOLETE
SgExpression&
mkEnumeratorRef_repclause(SgEnumDeclaration&, SgInitializedName& enumitem)
{
  return SG_DEREF( sb::buildVarRefExp(&enumitem, nullptr /* not needed */) );
}
#endif /* OBSOLETE */

namespace
{
  SgType& accessTypeAttr(SgExpression& expr, SgExprListExp&)
  {
    return mkAdaAccessType(SG_DEREF(expr.get_type()));
  }

  SgType& integralTypeAttr(SgExpression&, SgExprListExp&)
  {
    return mkIntegralType();
  }

  SgType& realTypeAttr(SgExpression&, SgExprListExp&)
  {
    return mkRealType();
  }

  SgType& unknownTypeAttr(SgExpression&, SgExprListExp&)
  {
    return mkTypeUnknown();
  }

  SgType& voidTypeAttr(SgExpression&, SgExprListExp&)
  {
    return mkTypeVoid();
  }

  SgType& boolTypeAttr(SgExpression&, SgExprListExp&)
  {
    return lookupNode(adaTypes(), AdaIdentifier{"BOOLEAN"});
  }
/*
  SgType& fixedTypeAttr(SgExpression&, SgExprListExp&)
  {
    return mkFixedType();
  }
*/
  SgType& stringTypeAttr(SgExpression&, SgExprListExp&)
  {
    return lookupNode(adaTypes(), AdaIdentifier{"STRING"});
  }

  SgType& wideStringTypeAttr(SgExpression&, SgExprListExp&)
  {
    return lookupNode(adaTypes(), AdaIdentifier{"WIDE_STRING"});
  }

  SgType& wideWideStringTypeAttr(SgExpression&, SgExprListExp&)
  {
    return lookupNode(adaTypes(), AdaIdentifier{"WIDE_WIDE_STRING"});
  }

  SgType& firstLastTypeAttr(SgExpression& obj, SgExprListExp& args)
  {
    SgType* basety = nullptr;

    if (SgTypeExpression* tyexp = isSgTypeExpression(&obj))
      basety = tyexp->get_type();
    else
      basety = obj.get_type();

    si::Ada::FlatArrayType flatty = si::Ada::getArrayTypeInfo(basety);

    if (flatty.first == nullptr)
      return SG_DEREF(basety);

    SgType* resty = nullptr;

    try
    {
      const int dim = si::Ada::firstLastDimension(args);

      resty = SG_DEREF(flatty.second.at(dim-1)).get_type();
    }
    catch (...)
    {
      logWarn() << "unable to fold " << args.get_expressions().at(0)->unparseToString()
                << std::endl;

      resty = &mkTypeUnknown();
    }

    return SG_DEREF(resty);
  }

  SgType& exprTypeAttr(SgExpression& tyrep, SgExprListExp&)
  {
    //~ logError() << " >>> " << tyrep.unparseToString() << " " << typeid(*tyrep.get_type()).name()
                //~ << std::endl;

    return SG_DEREF(tyrep.get_type());
  }

  SgType& argTypeAttr(SgExpression&, SgExprListExp& args)
  {
    SgExpression& exp = SG_DEREF(args.get_expressions().front());

    return SG_DEREF(exp.get_type());
  }

  SgType&
  attributeType(SgExpression& expr, const std::string& ident, SgExprListExp& args)
  {
    using TypeMaker = SgType& (*) (SgExpression& expr, SgExprListExp& args);
    using TypeCalc  = std::map<AdaIdentifier, TypeMaker>;

    static const TypeCalc typecalc = { { "access",               &accessTypeAttr }
                                     , { "address",              &integralTypeAttr }
                                     , { "address_size",         &integralTypeAttr }
                                     , { "adjacent",             &argTypeAttr }
                                     , { "aft",                  &integralTypeAttr }
                                     , { "alignment",            &integralTypeAttr }
                                     //~ , { "base",                 &unknownTypeAttr }
                                     , { "bit",                  &integralTypeAttr }
                                     , { "bit_order",            &voidTypeAttr }
                                     , { "body_version",         &stringTypeAttr }
                                     //~ , { "class",                &unknownTypeAttr }
                                     , { "callable",             &boolTypeAttr }
                                     , { "caller",               &stringTypeAttr } // should be Ada.Task_ID
                                     , { "ceiling",              &argTypeAttr }
                                     , { "component_size",       &integralTypeAttr }
                                     , { "compose",              &argTypeAttr }
                                     , { "constrained",          &boolTypeAttr }
                                     , { "copy_sign",            &argTypeAttr }
                                     , { "count",                &integralTypeAttr }
                                     , { "delta",                &realTypeAttr }
                                     , { "denorm",               &boolTypeAttr }
                                     , { "definite",             &boolTypeAttr }
                                     , { "digits",               &integralTypeAttr }
                                     , { "exponent",             &integralTypeAttr }
                                     //~ , { "external_tag",         &tagTypeAttr }
                                     , { "fraction",             &argTypeAttr }
                                     , { "first",                &firstLastTypeAttr }
                                     , { "first_bit",            &integralTypeAttr }
                                     , { "first_valid",          &exprTypeAttr }
                                     , { "floor",                &argTypeAttr }
                                     , { "fore",                 &integralTypeAttr }
                                     , { "fraction",             &argTypeAttr }
                                     , { "has_access_values",    &boolTypeAttr }
                                     , { "has_discriminants",    &boolTypeAttr }
                                     , { "has_tagged_values",    &boolTypeAttr }
                                     , { "has_same_storage",     &boolTypeAttr }
                                     , { "identity",             &stringTypeAttr } // should be an identity (TASK_ID, EXCEPTION_ID ...)
                                     , { "image",                &stringTypeAttr }
                                     //~ , { "input",                &unknownTypeAttr }   // ???
                                     , { "last",                 &firstLastTypeAttr }
                                     , { "last_valid",           &exprTypeAttr }
                                     , { "last_bit",             &integralTypeAttr }
                                     , { "leading_part",         &argTypeAttr }
                                     , { "length",               &integralTypeAttr }
                                     , { "machine",              &argTypeAttr }
                                     , { "machine_emax",         &integralTypeAttr }
                                     , { "machine_emin",         &integralTypeAttr }
                                     , { "machine_mantissa",     &integralTypeAttr }
                                     , { "machine_radix",        &integralTypeAttr }
                                     , { "machine_rounds",       &boolTypeAttr }
                                     , { "machine_rounding",     &argTypeAttr }
                                     , { "machine_overflows",    &boolTypeAttr }
                                     , { "max",                  &argTypeAttr }
                                     , { "maximum_alignment",    &integralTypeAttr }
                                     , { "min",                  &argTypeAttr }
                                     , { "model",                &argTypeAttr }
                                     , { "model_emin",           &integralTypeAttr }
                                     , { "model_epsilon",        &realTypeAttr }
                                     , { "model_mantissa",       &integralTypeAttr }
                                     , { "model_small",          &realTypeAttr }
                                     , { "modulus",              &integralTypeAttr }
                                     , { "overlaps_storage",     &boolTypeAttr }
                                     , { "pos",                  &integralTypeAttr }
                                     , { "position",             &integralTypeAttr }
                                     //~ , { "pred",                 &argTypeAttr }
                                     , { "pred",                 &exprTypeAttr }   // Type'Pred may have no arguments when it is passed as function
                                     , { "remainder",            &argTypeAttr }
                                     , { "rounding",             &argTypeAttr }
                                     //~ , { "range",                &unknownTypeAttr }
                                     //~ , { "output",               &unknownTypeAttr }   // ???
                                     //~ , { "read",                 &unknownTypeAttr }   // ???
                                     , { "safe_first",           &realTypeAttr }
                                     , { "safe_last",            &realTypeAttr }
                                     , { "scaling",              &argTypeAttr }
                                     , { "scalar_storage_order", &voidTypeAttr }
                                     , { "storage_pool",         &voidTypeAttr }
                                     , { "storage_size",         &integralTypeAttr }
                                     //~ , { "succ",                 &argTypeAttr }
                                     , { "succ",                 &exprTypeAttr }  // Type'Pred may have no arguments when it is passed as function
                                     , { "signed_zeros",         &boolTypeAttr }
                                     , { "size",                 &integralTypeAttr }
                                     , { "storage_unit",         &integralTypeAttr }
                                     //~ , { "tag",                  &tagTypeAttr }
                                     , { "terminated",           &boolTypeAttr }
                                     , { "truncation",           &argTypeAttr }
                                     , { "to_address",           &integralTypeAttr }
                                     , { "unbiased_rounding",    &argTypeAttr }
                                     , { "unchecked_access",     &accessTypeAttr }
                                     , { "unrestricted_access",  &accessTypeAttr }
                                     , { "val",                  &exprTypeAttr }
                                     , { "valid",                &boolTypeAttr }
                                     , { "value",                &exprTypeAttr }
                                     //~ , { "write",                &unknownTypeAttr }
                                     , { "wide_identity",        &wideStringTypeAttr } // should be Ada.Task_ID
                                     , { "wide_value",           &exprTypeAttr } // should be Ada.Task_ID
                                     , { "wide_wide_identity",   &wideWideStringTypeAttr } // should be Ada.Task_ID
                                     , { "wide_wide_value",      &exprTypeAttr } // should be Ada.Task_ID
                                     , { "width",                &integralTypeAttr }
                                     , { "wide_width",           &integralTypeAttr }
                                     , { "wide_wide_width",      &integralTypeAttr }
                                     , { "word_size",            &integralTypeAttr }
                                     };

    auto      pos = typecalc.find(ident);
    TypeMaker bldr = ((pos != typecalc.end()) ? pos->second : nullptr);

    if ((bldr == &argTypeAttr) && (args.get_expressions().size() == 0))
    {
      logError() << ident << " with zero arguments." << std::endl;
      ADA_ASSERT(false);
    }

    if (bldr == nullptr)
    {
      bldr = &unknownTypeAttr;
      logWarn() << "unknown attribute type for '" << ident << std::endl;
    }

    return bldr(expr, args);
  }
}


SgAdaAttributeExp&
mkAdaAttributeExp(SgExpression& expr, const std::string& ident, SgExprListExp& args)
{
  SgType&            attrty = attributeType(expr, ident, args);
  SgAdaAttributeExp& sgnode = mkLocatedNode<SgAdaAttributeExp>(ident, &expr, &args, &attrty);

  expr.set_parent(&sgnode);
  args.set_parent(&sgnode);

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
  convertType(SgType& actual, SgType& orig, SgType& derv)
  {
    return &orig == &actual ? derv : actual;
  }


  /// replaces the original type of \ref declaredDerivedType with \ref declaredDerivedType in \ref funcTy.
  /// returns \ref funcTy to indicate an error.
  SgFunctionType&
  convertToDerivedType(SgFunctionType& funcTy, SgType& derivedType)
  {
    SgDeclarationStatement* baseTypeDecl = si::Ada::baseDeclaration(derivedType);

    if (baseTypeDecl == nullptr)
      return funcTy;

    SgType*              origTypePtr  = si::getDeclaredType(baseTypeDecl);
    SgType&              originalType = SG_DEREF(origTypePtr);
    SgType&              origRetTy    = SG_DEREF(funcTy.get_return_type());
    SgType&              dervRetTy    = convertType(origRetTy, originalType, derivedType);
    int                  numUpdTypes  = (&dervRetTy != &origRetTy);
    std::vector<SgType*> newTypeList;

    for (SgType* origArgTy : funcTy.get_arguments())
    {
      SgType* newArgTy = &convertType(SG_DEREF(origArgTy), originalType, derivedType);

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
mkAdaInheritedFunctionSymbol(SgFunctionDeclaration& fn, SgType& derivedType, SgScopeStatement& scope)
{
  SgFunctionType& functy = SG_DEREF(fn.get_type());
  SgFunctionType& dervty = convertToDerivedType(functy, derivedType);

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

  SgStringVal& sgnode = mkLocatedNode<SgStringVal>(si::Ada::convertStringLiteral(textrep));

  sgnode.set_stringDelimiter(delimiter);
  return sgnode;
}


template <>
long double convAdaLiteral<long double>(const char* img)
{
  return si::Ada::convertRealLiteral(img);
}


template <>
char convAdaLiteral<char>(const char* img)
{
  return si::Ada::convertCharLiteral(img);
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

  long long int val = si::Ada::convertIntegerLiteral(textrep);
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
    ADA_ASSERT(false);
  }

  return *res;
}



//
// builder functions

//~ SgRemOp*
//~ buildRemOp(SgExpression* lhs, SgExpression* rhs)
//~ {
  //~ return &mkLocatedNode<SgRemOp>(lhs, rhs, nullptr);
//~ }

//~ SgAbsOp*
//~ buildAbsOp(SgExpression* op)
//~ {
  //~ return &mkLocatedNode<SgAbsOp>(op, nullptr);
//~ }


}
