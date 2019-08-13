
#include "sage3basic.h"

#include "OperandData.h"

#include "SgNodeHelper.h"

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
  predeccessors(),
  successors(),
  can_be_changed(true),
  from_system_files(false),
  casts()
{
#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base
  std::cout << "Typeforge::OperandData<OperandKind::base>::OperandData" << std::endl;
  std::cout << "  opkind = " << opkind << std::endl;
  std::cout << "  lnode  = " << lnode  << " (" << ( lnode ? lnode->class_name() : "") << ")" << std::endl;
#endif

  if (filename.find("rose_edg_required_macros_and_functions.h") != std::string::npos) {
    from_system_files = true;
  }

  if (from_system_files) {
    can_be_changed = false;
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

  SgScopeStatement * ascope = isSgScopeStatement(scope);
  assert(scope == nullptr || ascope != nullptr);

  while ( ascope && !isSgFunctionDefinition(ascope) && !isSgClassDefinition(ascope) && !isSgNamespaceDefinitionStatement(ascope) && !isSgGlobal(ascope) ) {

#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base
    std::cout << "  -> ascope  = " << ascope << " (" << ascope->class_name() << ")" << std::endl;
#endif

    SgScopeStatement * pscope = ascope->get_scope();

#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base
    std::cout << "  -> pscope  = " << pscope << " (" << ( pscope ? pscope->class_name() : "" ) << ")" << std::endl;
#endif

#if DEBUG__Typeforge__OperandData__CTOR__for__OperandKind__base__build_qualname_for_non_named_scopes
    SgFunctionDefinition * pfdefn = isSgFunctionDefinition(pscope);
    SgClassDefinition * pxdefn = isSgClassDefinition(pscope);
    SgNamespaceDefinitionStatement * pndefn = isSgNamespaceDefinitionStatement(pscope);
    SgGlobal * pglob = isSgGlobal(pscope);
#endif
    SgBasicBlock * pbb = isSgBasicBlock(pscope);
    std::ostringstream oss;
    if (pbb != nullptr) {
      auto stmts = pbb->getStatementList();
      auto it = std::find(stmts.begin(), stmts.end(), ascope);
      assert(it != stmts.end());
      auto pos = it - stmts.begin();
      oss << pos << "::";
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

  // TODO `can_be_changed`
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

OperandData<OperandKind::varref>::OperandData(SgVarRefExp * const vexp) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)vexp, OperandKind::varref)
{
  // TODO
}

OperandData<OperandKind::fref>::OperandData(SgFunctionRefExp * const vexp) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)vexp, OperandKind::fref)
{
  // TODO
}

OperandData<OperandKind::value>::OperandData(SgValueExp * const vexp) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)vexp, OperandKind::value)
{
  // TODO
}

OperandData<OperandKind::arithmetic>::OperandData(SgExpression * const expr) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)expr, OperandKind::arithmetic)
{
  // TODO
}

OperandData<OperandKind::call>::OperandData(SgFunctionCallExp * const fcall) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)fcall, OperandKind::call)
{
  // TODO
}

OperandData<OperandKind::array_access>::OperandData(SgPntrArrRefExp * const arrref) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)arrref, OperandKind::array_access)
{
  // TODO
}

OperandData<OperandKind::address_of>::OperandData(SgAddressOfOp * const addof) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)addof, OperandKind::address_of)
{
  // TODO
}

OperandData<OperandKind::dereference>::OperandData(SgPointerDerefExp * const ptrref) :
  OperandData<OperandKind::base>::OperandData((SgLocatedNode*)ptrref, OperandKind::dereference)
{
  // TODO
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
    case V_SgExpression: { // TODO list handled expression variants
      return new OperandData<OperandKind::arithmetic>((SgExpression*)node);
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
    default: {
      std::cerr << "[buildOperandData] Unknown node variant: " << node << " ( " << node->class_name() << " )" << std::endl;
      return new OperandData<OperandKind::unknown>(node);
    }
  }
  return nullptr;
}

}

