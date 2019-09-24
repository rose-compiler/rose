
#include "sage3basic.h"

#include "SgNodeHelper.h"

#include "Typeforge/OpNet/OperandData.hpp"

// #include "sage3basic.h"

#ifndef DEBUG__Typeforge
#define DEBUG__Typeforge 0
#endif
#ifndef DEBUG__Typeforge__OperandData
#define DEBUG__Typeforge__OperandData DEBUG__Typeforge
#endif
#ifndef DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base
#define DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base DEBUG__Typeforge__OperandData
#endif
#ifndef DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__member_access
#  define DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__member_access DEBUG__Typeforge__OperandData
#endif
#ifndef DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base__build_qualname_for_non_named_scopes
#  define DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base__build_qualname_for_non_named_scopes 0
#endif

namespace Typeforge {

OperandData<OperandKind::base>::OperandData(SgLocatedNode * const __lnode, OperandKind const & __opkind) :
  opkind(__opkind),
  lnode(__lnode),
  original_type(nullptr),
  scope(nullptr),
  handle(""),
  filename(lnode->get_endOfConstruct()->get_filenameString()),
  line_start(lnode->get_startOfConstruct()->get_raw_line()),
  column_start(lnode->get_startOfConstruct()->get_raw_col()),
  line_end(lnode->get_endOfConstruct()->get_raw_line()),
  column_end(lnode->get_endOfConstruct()->get_raw_col()),
  can_be_changed(true),
  from_system_files(false),
  casts(),
  edge_labels()
{
#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base
  std::cout << "Typeforge::OperandData<OperandKind::base>::OperandData" << std::endl;
  std::cout << "  lnode = " << lnode  << " (" << ( lnode ? lnode->class_name() : "") << ")" << std::endl;
  std::cout << "    parent = " << lnode->get_parent()  << " (" << ( lnode->get_parent() ? lnode->get_parent()->class_name() : "") << ")" << std::endl;
#endif

  if (filename == "compilerGenerated" || filename == "NULL_FILE") {
    SgDeclarationStatement * declstmt = isSgDeclarationStatement(lnode);
    declstmt = declstmt ? declstmt->get_definingDeclaration() : nullptr;
    if (declstmt != nullptr) {
      filename = declstmt->get_endOfConstruct()->get_filenameString();
      line_start = declstmt->get_startOfConstruct()->get_raw_line();
      column_start = declstmt->get_startOfConstruct()->get_raw_col();
      line_end = declstmt->get_endOfConstruct()->get_raw_line();
      column_end = declstmt->get_endOfConstruct()->get_raw_col();
    }
  }

  // TODO get paths through:
  //  - automated: analyze all positions in graph [eventually]
  //  - cmd-line [good first step]
  //  - environment [I don't like that one much]
  std::vector<std::string> paths{
    "/workspace/pipeline-tests/typeforge-tests/",
    "/opt/rose/vanderbrugge1/typeforge/native/release/install/include/edg/g++_HEADERS/hdrs7/",
    "/opt/rose/vanderbrugge1/typeforge/native/release/install/include/edg/g++_HEADERS/"
  };
  for (auto p : paths) {
    auto i = filename.find(p);
    if (i == 0) {
      filename = filename.substr(p.size());
    }
  }

  // This object is used to unparse type properly
  SgUnparse_Info * uinfo = new SgUnparse_Info();
      uinfo->set_SkipComments();
      uinfo->set_SkipWhitespaces();
      uinfo->set_SkipEnumDefinition();
      uinfo->set_SkipClassDefinition();
      uinfo->set_SkipFunctionDefinition();
      uinfo->set_SkipBasicBlock();
      uinfo->set_isTypeFirstPart(); // <<< prevent proper unparsing of array-type but necessary to avoid some class definitions

  SgExpression * expr = isSgExpression(lnode);
  SgInitializedName * iname = isSgInitializedName(lnode);
  SgVariableDeclaration * vdecl = isSgVariableDeclaration(lnode);
  SgFunctionDeclaration * fdecl = isSgFunctionDeclaration(lnode);
  SgClassDeclaration * xdecl = isSgClassDeclaration(lnode);
  SgNamespaceDeclarationStatement * ndecl = isSgNamespaceDeclarationStatement(lnode);

  if (expr != nullptr) {
    std::ostringstream oss;
      oss << filename << ":" << line_start << ":" << column_start << ":" << line_end << ":" << column_end;
    handle = oss.str();
    original_type = expr->get_type();

    scope = nullptr; // FIXME do we want to prepend the scope of the expression?

  } else if (ndecl != nullptr) {
    original_type = nullptr;
    handle = ndecl->get_qualified_name().getString();
    scope = nullptr;
    ndecl = nullptr;

  } else if (iname != nullptr) {
    SgFunctionParameterList * fplst = isSgFunctionParameterList(iname->get_parent());
    assert(fplst != nullptr);
    fdecl = isSgFunctionDeclaration(fplst->get_parent());
    assert(fdecl != nullptr);

    auto it = std::find(fplst->get_args().begin(), fplst->get_args().end(), iname);
    assert(it != fplst->get_args().end());
    auto pos = it - fplst->get_args().begin();

    std::ostringstream oss;
    oss << pos;
    handle = oss.str();
    original_type = iname->get_type();
    scope = nullptr;

  } else if (xdecl != nullptr) {
    original_type = xdecl->get_type();
    handle = xdecl->get_qualified_name().getString();
    scope = nullptr;
    fdecl = nullptr;

  } else if (fdecl != nullptr) {
    original_type = fdecl->get_type()->get_return_type();
    std::ostringstream oss;
    oss << fdecl->get_qualified_name().getString() << "(";
    for (auto t : fdecl->get_type()->get_argument_list()->get_arguments()) {
      oss << globalUnparseToString(t, uinfo) << ",";
    }
    oss << ")" << handle;
    handle = oss.str();
    scope = nullptr;
    fdecl = nullptr;

  } else if (vdecl != nullptr) {
    iname = SgNodeHelper::getInitializedNameOfVariableDeclaration(vdecl);

    handle = iname->get_name().getString();
    original_type = iname->get_type();
    scope = vdecl->get_scope();
    assert(scope != nullptr);

  } else {
    assert(false);
  }

#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base
  std::cout << "  original_type = " << original_type << " (" << ( original_type ? original_type->class_name()  : "") << ") : " << ( original_type ? original_type->unparseToString()  : "") << std::endl;
  std::cout << "  scope         = " << scope         << " (" << ( scope         ? scope->class_name()          : "") << ")" << std::endl;
#endif

  SgStatement * ascope = isSgScopeStatement(scope);
  assert(scope == nullptr || ascope != nullptr);

  while ( ascope && !isSgFunctionDefinition(ascope) && !isSgClassDefinition(ascope) && !isSgNamespaceDefinitionStatement(ascope) && !isSgGlobal(ascope) ) {

#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base
    std::cout << "  WHILE" << std::endl;
    std::cout << "  -> ascope = " << ascope << " (" << ascope->class_name() << ")" << std::endl;
    std::cout << "       parent = " << ascope->get_parent() << " (" << ascope->get_parent()->class_name() << ")" << std::endl;
    std::cout << "         parent = " << ascope->get_parent()->get_parent() << " (" << ascope->get_parent()->get_parent()->class_name() << ")" << std::endl;
    std::cout << "           parent = " << ascope->get_parent()->get_parent()->get_parent() << " (" << ascope->get_parent()->get_parent()->get_parent()->class_name() << ")" << std::endl;
#endif

    SgStatement * pscope = ascope->get_scope();

#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base
    std::cout << "  -> pscope  = " << pscope << " (" << ( pscope ? pscope->class_name() : "" ) << ")" << std::endl;
    std::cout << "       parent = " << pscope->get_parent() << " (" << pscope->get_parent()->class_name() << ")" << std::endl;
#endif

    if (SgCatchOptionStmt * catch_stmt = isSgCatchOptionStmt(ascope)) {
      pscope = isSgCatchStatementSeq(catch_stmt->get_parent());
      assert(pscope != nullptr);
#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base
      std::cout << "  -> pscope  = " << pscope << " (" << ( pscope ? pscope->class_name() : "" ) << ")" << std::endl;
#endif
    }

    if (SgTryStmt * try_stmt = isSgTryStmt(ascope->get_parent())) {
      pscope = try_stmt;
    }

#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base__build_qualname_for_non_named_scopes
    SgFunctionDefinition * pfdefn = isSgFunctionDefinition(pscope);
    SgClassDefinition * pxdefn = isSgClassDefinition(pscope);
    SgNamespaceDefinitionStatement * pndefn = isSgNamespaceDefinitionStatement(pscope);
    SgGlobal * pglob = isSgGlobal(pscope);
#endif
    std::ostringstream oss;
    if (SgBasicBlock * pbb = isSgBasicBlock(pscope)) {
      auto stmts = pbb->getStatementList();
      auto it = std::find(stmts.begin(), stmts.end(), ascope);
      assert(it != stmts.end());
      auto pos = it - stmts.begin();
      oss << pos << "::";

    } else if (isSgTryStmt(pscope)) {
      oss << "try_body::";

    } else if (SgCatchStatementSeq * catch_stmts_seq = isSgCatchStatementSeq(pscope)) {
      auto stmts = catch_stmts_seq->get_catch_statement_seq();
      auto it = std::find(stmts.begin(), stmts.end(), ascope);
      assert(it != stmts.end());
      auto pos = it - stmts.begin();
      oss << "catch_" << pos << "::";

      pscope = isSgTryStmt(catch_stmts_seq->get_parent());
      assert(pscope != nullptr);

#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base__build_qualname_for_non_named_scopes
    } else if (pfdefn != nullptr) {
      oss << "{}::";
    } else if (pxdefn != nullptr) {
      oss << "##::";
    } else if (pndefn != nullptr) {
      oss << "$$::";
    } else if (pglob != nullptr) {
      oss << "@@::";
    } else {
      oss << "--::";
#endif
    }
    handle = oss.str() + handle;
    ascope = pscope;
  }

  if (SgClassDefinition * xdefn = isSgClassDefinition(ascope)) {
    xdecl = xdefn->get_declaration();
    scope = xdecl;
  } else if (SgFunctionDefinition * fdefn = isSgFunctionDefinition(ascope)) {
    fdecl = fdefn->get_declaration();
    scope = fdecl;
  } else if (SgNamespaceDefinitionStatement * ndefn = isSgNamespaceDefinitionStatement(ascope)) {
    ndecl = ndefn->get_namespaceDeclaration();
    scope = ndecl;
  }

  if (ndecl != nullptr) {
    handle = ndecl->get_qualified_name().getString() + "::" + handle;
  } else if (xdecl != nullptr) {
    handle = xdecl->get_qualified_name().getString() + "::" + handle;
  } else if (fdecl != nullptr) {
    std::ostringstream oss;
    oss << fdecl->get_qualified_name().getString() << "(";
    for (auto t : fdecl->get_type()->get_argument_list()->get_arguments()) {
      oss << globalUnparseToString(t, uinfo) << ",";
    }
    oss << ")::" << handle;
    handle = oss.str();
  }

  bool builtin_decl = handle.find("::__builtin_") == 0 ||
                      handle.find("::__sync_")    == 0 ||
                      handle.find("::__noop")     == 0 ||
                      handle.find("::__atomic")   == 0;


  if (isSgExpression(lnode)) {
    handle = "EXPR[" + handle + "]";
  } else if (isSgNamespaceDeclarationStatement(lnode)) {
    handle = "NAMESPACE[" + handle + "]";
  } else if (isSgInitializedName(lnode)) {
    handle = "PARAMETER[" + handle + "]";
  } else if (isSgClassDeclaration(lnode)) {
    handle = "STRUCT[" + handle + "]";
  } else if (isSgFunctionDeclaration(lnode)) {
    handle = "FUNCTION[" + handle + "]";
  } else if (isSgVariableDeclaration(lnode)) {
    handle = "VARIABLE[" + handle + "]";
  } else {
    assert(false);
  }

#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base
  std::cout << "  handle   = " << handle << std::endl;
#endif

  delete uinfo;

  if (
    builtin_decl || filename == "NULL_FILE" ||
    filename.find("rose_edg_required_macros_and_functions.h") != std::string::npos
  ) {
    from_system_files = true;
  }

  if (filename == "compilerGenerated" || from_system_files) {
    can_be_changed = false;
  }
}

static SgExpression * skipCastOperator(SgExpression * e) {
  assert(e != nullptr);
  while (SgCastExp * ce = isSgCastExp(e)) {
    e = ce->get_operand_i();
    assert(e != nullptr);
  }
  assert(e != nullptr);
  return e;
}

OperandData<OperandKind::variable>::OperandData(SgVariableDeclaration * const vdecl) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)vdecl, OperandKind::variable)
{
  // TODO
}

OperandData<OperandKind::function>::OperandData(SgFunctionDeclaration * const fdecl) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)fdecl, OperandKind::function)
{
  // TODO fields above
}

OperandData<OperandKind::parameter>::OperandData(SgInitializedName * const iname) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)iname, OperandKind::parameter)
{
  // TODO
}

OperandData<OperandKind::varref>::OperandData(SgVarRefExp * const vref) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)vref, OperandKind::varref)
{
  SgVariableSymbol * vsym = vref->get_symbol();
  assert(vsym != nullptr);

  SgInitializedName * iname = vsym->get_declaration();
  assert(iname != nullptr);

  SgVariableDeclaration * vdecl = isSgVariableDeclaration(iname->get_parent());
  if (vdecl != nullptr) {
    edge_labels.insert(std::pair<std::string, SgLocatedNode *>("decl", vdecl));
  } else {
    edge_labels.insert(std::pair<std::string, SgLocatedNode *>("decl", iname));
  }
}

OperandData<OperandKind::fref>::OperandData(SgFunctionRefExp * const fref) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)fref, OperandKind::fref)
{
  SgFunctionSymbol * fsym = fref->get_symbol_i();
  assert(fsym != nullptr);

  SgDeclarationStatement * declstmt = fsym->get_declaration();
  assert(declstmt != nullptr);

  declstmt = declstmt->get_firstNondefiningDeclaration();
  assert(declstmt != nullptr);

  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("decl", declstmt));
}

OperandData<OperandKind::thisref>::OperandData(SgThisExp * const thisexp) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)thisexp, OperandKind::thisref)
{
  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("decl", nullptr));
}

OperandData<OperandKind::fref>::OperandData(SgMemberFunctionRefExp * const mfref) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)mfref, OperandKind::fref)
{
  SgFunctionSymbol * fsym = mfref->get_symbol_i();
  assert(fsym != nullptr);

  SgDeclarationStatement * declstmt = fsym->get_declaration();
  assert(declstmt != nullptr);

  declstmt = declstmt->get_firstNondefiningDeclaration();
  assert(declstmt != nullptr);

  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("decl", declstmt));
}

OperandData<OperandKind::value>::OperandData(SgValueExp * const vexp) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)vexp, OperandKind::value)
{}

OperandData<OperandKind::assign>::OperandData(SgBinaryOp * const bop) :
  OperandData<OperandKind::base>::OperandData(bop, OperandKind::assign)
{
  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("ref", skipCastOperator(bop->get_lhs_operand_i())));
  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("val", skipCastOperator(bop->get_rhs_operand_i())));
}

OperandData<OperandKind::assign>::OperandData(SgAssignInitializer * const assinit) :
  OperandData<OperandKind::base>::OperandData(assinit, OperandKind::assign)
{
  SgInitializedName * iname = isSgInitializedName(assinit->get_parent());
  assert(iname != nullptr);
  SgVariableDeclaration * vdecl = isSgVariableDeclaration(iname->get_parent());
  if (vdecl != nullptr) {
    edge_labels.insert(std::pair<std::string, SgLocatedNode *>("ref", vdecl));
  } else {
    edge_labels.insert(std::pair<std::string, SgLocatedNode *>("ref", iname));
  }
  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("val", skipCastOperator(assinit->get_operand_i())));
}

OperandData<OperandKind::unary_arithmetic>::OperandData(SgUnaryOp * const uop) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)uop, OperandKind::unary_arithmetic)
{
  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("op", skipCastOperator(uop->get_operand_i())));
}

OperandData<OperandKind::binary_arithmetic>::OperandData(SgBinaryOp * const binop) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)binop, OperandKind::binary_arithmetic)
{
  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("lhs", skipCastOperator(binop->get_lhs_operand_i())));
  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("rhs", skipCastOperator(binop->get_rhs_operand_i())));
}

OperandData<OperandKind::call>::OperandData(SgFunctionCallExp * const fcall) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)fcall, OperandKind::call)
{
  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("fnc", skipCastOperator(fcall->get_function())));
  auto const & args = fcall->get_args()->get_expressions();
  for (size_t i = 0; i < args.size(); ++i) {
    std::ostringstream oss;
    oss << "args[" << i << "]";
    edge_labels.insert(std::pair<std::string, SgLocatedNode *>(oss.str(), skipCastOperator(args[i])));
  }
}

OperandData<OperandKind::array_access>::OperandData(SgPntrArrRefExp * const arrref) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)arrref, OperandKind::array_access)
{
  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("arr", skipCastOperator(arrref->get_lhs_operand_i())));
  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("idx", skipCastOperator(arrref->get_rhs_operand_i())));
}

OperandData<OperandKind::address_of>::OperandData(SgAddressOfOp * const addof) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)addof, OperandKind::address_of)
{
  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("obj", skipCastOperator(addof->get_operand_i())));
}

OperandData<OperandKind::dereference>::OperandData(SgPointerDerefExp * const ptrref) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)ptrref, OperandKind::dereference)
{
  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("ref", skipCastOperator(ptrref->get_operand_i())));
}

OperandData<OperandKind::member_access>::OperandData(SgBinaryOp * const bop) :
  OperandData<OperandKind::base>::OperandData((SgBinaryOp*)bop, OperandKind::member_access)
{
  assert(isSgBinaryOp(bop));

#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__member_access
  std::cout << "OperandData<OperandKind::member_access>::OperandData" << std::endl;
  std::cout << "  bop = " << bop << " (" << ( bop ? bop->class_name() : "") << ")" << std::endl;
#endif

  SgExpression * lhs = bop->get_lhs_operand_i();
#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__member_access
  std::cout << " lhs = " << lhs << " (" << ( lhs ? lhs->class_name() : "") << ")" << std::endl;
#endif
  lhs = skipCastOperator(lhs);
#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__member_access
  std::cout << " lhs = " << lhs << " (" << ( lhs ? lhs->class_name() : "") << ")" << std::endl;
#endif
  
  SgExpression * rhs = bop->get_rhs_operand_i();
#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__member_access
  std::cout << " rhs = " << rhs << " (" << ( rhs ? rhs->class_name() : "") << ")" << std::endl;
#endif
  rhs = skipCastOperator(rhs);
#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__member_access
  std::cout << " rhs = " << rhs << " (" << ( rhs ? rhs->class_name() : "") << ")" << std::endl;
#endif

  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("obj", lhs));
  edge_labels.insert(std::pair<std::string, SgLocatedNode *>("fld", rhs));

#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__member_access
  std::cout << " # edge_labels = " << edge_labels.size() << std::endl;
#endif
}

OperandData<OperandKind::unknown>::OperandData(SgLocatedNode * const lnode) :
  OperandData<OperandKind::base>::OperandData(lnode, OperandKind::unknown)
{}

OperandData<OperandKind::base> * OperandData<OperandKind::base>::build(SgLocatedNode * node) {
  switch (node->variantT()) {
    case V_SgVariableDeclaration: {
      return new OperandData<OperandKind::variable>((SgVariableDeclaration*)node);
    }

    case V_SgFunctionDeclaration:
    case V_SgTemplateInstantiationFunctionDecl:
    case V_SgMemberFunctionDeclaration:
    case V_SgTemplateInstantiationMemberFunctionDecl: {  
      return new OperandData<OperandKind::function>((SgFunctionDeclaration*)node);
    }

    case V_SgInitializedName: {
      return new OperandData<OperandKind::parameter>((SgInitializedName*)node);
    }

    case V_SgVarRefExp: {
      return new OperandData<OperandKind::varref>((SgVarRefExp*)node);
    }

    case V_SgFunctionRefExp: {
      return new OperandData<OperandKind::fref>((SgFunctionRefExp*)node);
    }
    case V_SgMemberFunctionRefExp: {
      return new OperandData<OperandKind::fref>((SgMemberFunctionRefExp*)node);
    }

    case V_SgBoolValExp:
    case V_SgChar16Val:
    case V_SgChar32Val:
    case V_SgCharVal:
    case V_SgComplexVal:
    case V_SgDoubleVal:
    case V_SgEnumVal:
    case V_SgFloat128Val:
    case V_SgFloat80Val:
    case V_SgFloatVal:
    case V_SgIntVal:
    case V_SgLongDoubleVal:
    case V_SgLongIntVal:
    case V_SgLongLongIntVal:
    case V_SgNullptrValExp:
    case V_SgShortVal:
    case V_SgStringVal:
    case V_SgUnsignedCharVal:
    case V_SgUnsignedIntVal:
    case V_SgUnsignedLongLongIntVal:
    case V_SgUnsignedLongVal:
    case V_SgUnsignedShortVal:
    case V_SgWcharVal: {
      return new OperandData<OperandKind::value>((SgValueExp*)node);
    }

    case V_SgAssignOp:
    case V_SgPlusAssignOp:
    case V_SgMinusAssignOp:
    case V_SgMultAssignOp:
    case V_SgDivAssignOp:
    case V_SgModAssignOp:
    case V_SgLshiftAssignOp:
    case V_SgRshiftAssignOp:
    case V_SgAndAssignOp:
    case V_SgIorAssignOp:
    case V_SgXorAssignOp: {
      return new OperandData<OperandKind::assign>((SgBinaryOp*)node);
    }

    case V_SgAssignInitializer: {
      return new OperandData<OperandKind::assign>((SgAssignInitializer*)node);
    }

    case V_SgAggregateInitializer:
    case V_SgBracedInitializer:
    case V_SgCompoundInitializer:
    case V_SgConstructorInitializer:
    case V_SgDesignatedInitializer: {
      std::cerr << "[buildOperandData] Unsupported SgInitializer: " << node << " ( " << node->class_name() << " )" << std::endl;
      return new OperandData<OperandKind::unknown>(node);
    }

    case V_SgMinusOp:
    case V_SgPlusPlusOp: {
      return new OperandData<OperandKind::unary_arithmetic>((SgUnaryOp*)node);
    }

    case V_SgAddOp:
    case V_SgLessThanOp: {
      return new OperandData<OperandKind::binary_arithmetic>((SgBinaryOp*)node);
    }

    case V_SgFunctionCallExp: {
      return new OperandData<OperandKind::call>((SgFunctionCallExp*)node);
    }

    case V_SgPntrArrRefExp: {
      return new OperandData<OperandKind::array_access>((SgPntrArrRefExp*)node);
    }

    case V_SgAddressOfOp: {
      return new OperandData<OperandKind::address_of>((SgAddressOfOp*)node);
    }

    case V_SgPointerDerefExp: {
      return new OperandData<OperandKind::dereference>((SgPointerDerefExp*)node);
    }

    case V_SgArrowExp:
    case V_SgDotExp: {
      return new OperandData<OperandKind::member_access>((SgBinaryOp*)node);
    }

    case V_SgThisExp: {
      return new OperandData<OperandKind::thisref>((SgThisExp*)node);
    }

    default: {
      std::cerr << "[buildOperandData] Unknown node variant: " << node << " ( " << node->class_name() << " )" << std::endl;
      return new OperandData<OperandKind::unknown>(node);
    }
  }
  return nullptr;
}

}

