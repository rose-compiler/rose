#include "sage3basic.h"

#include "Typeforge/OpNet/commons.hpp"

#ifndef DEBUG__Typeforge
#  define DEBUG__Typeforge 0
#endif
#ifndef DEBUG__Typeforge__stripType
#  define DEBUG__Typeforge__stripType DEBUG__Typeforge
#endif

namespace Typeforge {

EdgeKind operator| (const EdgeKind & a, const EdgeKind & b) {
  assert(a != EdgeKind::unknown);
  assert(b != EdgeKind::unknown);

  EdgeKind r = EdgeKind::unknown;

  if (a == b) r = a;
  else if (a == EdgeKind::traversal || a == EdgeKind::value) r = b;
  else if (b == EdgeKind::traversal || b == EdgeKind::value) r = a;
  else if (a == EdgeKind::deref && b == EdgeKind::address) r = EdgeKind::value;
  else if (a == EdgeKind::address && b == EdgeKind::deref) r = EdgeKind::value;

  return r;
}

EdgeKind operator& (const EdgeKind & a, const EdgeKind & b) {
  assert(a != EdgeKind::unknown);
  assert(b != EdgeKind::unknown);

  // incompatible
  assert(a != EdgeKind::deref || b != EdgeKind::address);
  assert(a != EdgeKind::address || b != EdgeKind::deref);

  EdgeKind r = EdgeKind::unknown;

  if (a == b) r = a;
  else if (a == EdgeKind::traversal || a == EdgeKind::value) r = b;
  else if (b == EdgeKind::traversal || b == EdgeKind::value) r = a;

  return r;
}

static SgType * stripType__(SgType * type, bool strip_std_vector) {
  assert(type != NULL);

#if DEBUG__Typeforge__stripType
  std::cout << "Typeforge::stripType" << std::endl;
  std::cout << "  type      = " << type << " ( " << type->class_name() << "): " << type->unparseToString() << "" << std::endl;
#endif

  type = type->stripType(
    SgType::STRIP_ARRAY_TYPE     |
    SgType::STRIP_POINTER_TYPE   |
    SgType::STRIP_MODIFIER_TYPE  |
    SgType::STRIP_REFERENCE_TYPE |
    SgType::STRIP_RVALUE_REFERENCE_TYPE
  );

  SgTypedefType * td_type = isSgTypedefType(type);
  SgClassType * xtype = isSgClassType(type);
  if (td_type != nullptr) {
    type = stripType__(td_type->get_base_type(), strip_std_vector);
  } else if (strip_std_vector && xtype != nullptr) {
    SgDeclarationStatement * decl_stmt = xtype->get_declaration();
    assert(decl_stmt != nullptr);

#if DEBUG__Typeforge__stripType
    std::cout << "  decl_stmt = " << decl_stmt << " ( " << decl_stmt->class_name() << "): " << decl_stmt->unparseToString() << "" << std::endl;
#endif

    SgTemplateInstantiationDecl * ti_decl = isSgTemplateInstantiationDecl(decl_stmt);
    if (ti_decl != nullptr) {
#if DEBUG__Typeforge__stripType
      std::cout << "      ->get_qualified_name() = " << ti_decl->get_qualified_name() << std::endl;
      std::cout << "      ->get_name()           = " << ti_decl->get_name() << std::endl;
#endif

      SgTemplateClassDeclaration * td_decl = ti_decl->get_templateDeclaration();
      assert(td_decl != nullptr);

#if DEBUG__Typeforge__stripType
      std::cout << "  td_decl   = " << td_decl << " ( " << td_decl->class_name() << "): " << td_decl->unparseToString() << "" << std::endl;
      std::cout << "      ->get_qualified_name() = " << td_decl->get_qualified_name() << std::endl;
#endif

      if (td_decl->get_qualified_name() == "::std::vector") {
        auto tpl_args = ti_decl->get_templateArguments();
        assert(tpl_args.size() > 0);

        SgType * tpl_type_arg = tpl_args[0]->get_type();
        assert(tpl_type_arg != nullptr);

        type = stripType__(tpl_type_arg, false);
      }
    }
  }

  return type;
}

bool is_template_with_dependent_return_type(lnode_ptr fnc) {
  if (SgTemplateInstantiationFunctionDecl * ti_fdecl = isSgTemplateInstantiationFunctionDecl(fnc)) {
    // case: call to: template < ... , typename Tx , ... > Tx const & foo(...);
    SgTemplateFunctionDeclaration * t_fdecl = ti_fdecl->get_templateDeclaration();
    assert(t_fdecl != nullptr);
    SgFunctionType * ftype = t_fdecl->get_type();
    assert(ftype != nullptr);
    SgType * r_ftype = ftype->get_return_type();
    assert(r_ftype != nullptr);
    SgNonrealType * nrtype = isSgNonrealType(::Typeforge::stripType__(r_ftype, true));
    if (nrtype != nullptr) {
      SgNonrealDecl * nrdecl = isSgNonrealDecl(nrtype->get_declaration());
      assert(nrdecl != nullptr);
      if (nrdecl->get_is_template_param()) { // TODO check that it is a template parameter from `t_fdecl`
        return true;
      }
    }
  }
  return false;
}

}

