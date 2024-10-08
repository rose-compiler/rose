
#include "sage3basic.h"
#include "unparser.h"
#include "rose_config.h"

#define DEBUG__isAssociatedWithCxx11_initializationList 0
#define DEBUG__trimCtorNameQual 0
#define DEBUG__unparseCtorInit 0

static bool isAssociatedWithCxx11_initializationList(SgConstructorInitializer * con_init) {
  bool is_cxx11_initialization_list = false;
#if DEBUG__isAssociatedWithCxx11_initializationList
  printf ("Enter isAssociatedWithCxx11_initializationList()\n");
#endif
  if (con_init != NULL) {
    SgMemberFunctionDeclaration * mfdecl = con_init->get_declaration();
#if DEBUG__isAssociatedWithCxx11_initializationList
    printf ("  mfdecl = %p = %s\n", mfdecl, mfdecl ? mfdecl->class_name().c_str() : "");
#endif
    if (mfdecl != NULL) {
      std::string name = mfdecl->get_name();
#if DEBUG__isAssociatedWithCxx11_initializationList
      printf ("    ->name = %s\n",name.c_str());
#endif
      if (name == "initializer_list") {
        is_cxx11_initialization_list = true;
      }
    }
  }

#if DEBUG__isAssociatedWithCxx11_initializationList
  printf ("  is_cxx11_initialization_list = %s\n", is_cxx11_initialization_list ? "true" : "false");
  printf ("Leave isAssociatedWithCxx11_initializationList()\n");
#endif
  return is_cxx11_initialization_list;
}

//! Fixes generated name-qualification for constructor calls
//! Call to constructor `Nspc::Abc()` is internally qualified as `Nspc::Abc::Abc()`
//! Which produces the name-qualifier `Nspc::Abc::`
//! Older compiler accept that qualification but not modern compilers which simply expect `Nspc::`
//! When needed, this code removes the trailing `::`. In this case, it return `false` to signify that the constructor name does not need to be unparsed separately
static bool trimCtorNameQual(std::string & qualifier) {
#if DEBUG__trimCtorNameQual
  printf("Enter trimTypenameFromCtorNameQual()\n");
  printf("  qualifier = %s\n", qualifier.c_str());
#endif

#ifdef USE_CMAKE
#  ifdef CMAKE_COMPILER_IS_GNUCXX && !( (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4 && BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER >= 5) || (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 4) )
  return true;
#  endif
#  ifdef CMAKE_COMPILER_IS_CLANG && !(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER >= 6)
  return true;
#  endif
#else
#  if BACKEND_CXX_IS_GNU_COMPILER && !( (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4 && BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER >= 5) || (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 4) )
  return true;
#  endif
#  if BACKEND_CXX_IS_CLANG_COMPILER && !(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER >= 6)
  return true;
#  endif
#endif

  size_t size = qualifier.size();
  if (size == 0) return true;
  ROSE_ASSERT(size >= 2);
  qualifier = qualifier.substr(0, size-2);
#if DEBUG__trimCtorNameQual
  printf("  qualifier = %s\n", qualifier.c_str());
#endif
  return false;
}

void Unparse_ExprStmt::unparseCtorInit(SgExpression * expr, SgUnparse_Info & info) {
#if DEBUG__unparseCtorInit
  printf ("Enter Unparse_ExprStmt::unparseCtorInit():\n");
  printf ("  info.inAggregateInitializer() = %s \n", info.inAggregateInitializer() ? "true" : "false");
  printf ("  expr = %p = %s\n", expr, expr->class_name().c_str());
#endif

  SgConstructorInitializer* con_init = isSgConstructorInitializer(expr);
  ASSERT_not_null(con_init);
  bool ctor_without_args = con_init->get_args()->get_expressions().empty();
#if DEBUG__unparseCtorInit
  printf ("    ->get_need_name() = %s \n", con_init->get_need_name() ? "true" : "false");
  printf ("    ->get_is_braced_initialized() = %s \n", con_init->get_is_braced_initialized() ? "true" : "false");
  printf ("    ->get_need_parenthesis_after_name() = %s \n", con_init->get_need_parenthesis_after_name() ? "true" : "false");
  printf ("    ->get_is_explicit_cast() = %s \n", con_init->get_is_explicit_cast() ? "true" : "false");
  printf ("    ->get_is_used_in_conditional() = %s \n", con_init->get_is_used_in_conditional() ? "true" : "false");
  printf ("  ctor_without_args = %s \n", ctor_without_args ? "true" : "false");
#endif
  SgMemberFunctionDeclaration * ctor_decl = con_init->get_declaration();
  SgClassDeclaration * ctor_class = con_init->get_class_decl();
  SgType * ctor_type = con_init->get_type();
#if DEBUG__unparseCtorInit
  printf ("  ctor_decl  = %p = %s = %s\n", ctor_decl,  ctor_decl  ? ctor_decl->class_name().c_str()  : "", ctor_decl  ? ctor_decl->get_name().str()  : "");
  printf ("  ctor_class = %p = %s = %s\n", ctor_class, ctor_class ? ctor_class->class_name().c_str() : "", ctor_class ? ctor_class->get_name().str() : "");
  printf ("  ctor_type  = %p = %s\n",      ctor_type,  ctor_type  ? ctor_type->class_name().c_str()  : "", isSgNamedType(ctor_type) ? ((SgNamedType*)ctor_type)->get_name().str() : "");
#endif

  bool use_braces = con_init->get_is_braced_initialized();
  bool print_ctor_name = unp->u_sage->printConstructorName(con_init);
  SgNode * pnode = con_init->get_parent();
  SgNode * ppnode = pnode ? pnode->get_parent() : nullptr;
  SgNode * pppnode = ppnode ? ppnode->get_parent() : nullptr;
  bool is_ctor_within_new = isSgNewExp(pppnode);
  bool arg_of_ctor_or_aggr = isSgConstructorInitializer(ppnode) || isSgAggregateInitializer(ppnode);
  bool nested_ctor_init_without_arg = ctor_without_args && arg_of_ctor_or_aggr;
  bool is_explicit_ctor = ctor_decl ? ctor_decl->get_functionModifier().isExplicit() : false;
  bool iname_use_cpy_syntax = isSgInitializedName(pnode) ? ((SgInitializedName*)pnode)->get_using_assignment_copy_constructor_syntax() : false;
  bool explicit_ctor_with_cpy_syntax = is_explicit_ctor && iname_use_cpy_syntax && !ctor_without_args && !use_braces;
#if DEBUG__unparseCtorInit
  printf ("  use_braces = %s\n", use_braces ? "true" : "false");
  printf ("  pnode = %p = %s\n", pnode, pnode ? pnode->class_name().c_str() : "");
  printf ("  ppnode = %p = %s\n", ppnode, ppnode ? ppnode->class_name().c_str() : "");
  printf ("  pppnode = %p = %s\n", pppnode, pppnode ? pppnode->class_name().c_str() : "");
  printf ("  is_ctor_within_new = %s\n", is_ctor_within_new ? "true" : "false");
  printf ("  nested_ctor_init_without_arg = %s\n", nested_ctor_init_without_arg ? "true" : "false");
  printf ("  is_explicit_ctor = %s\n", is_explicit_ctor ? "true" : "false");
  printf ("  iname_use_cpy_syntax = %s\n", iname_use_cpy_syntax ? "true" : "false");
  printf ("  explicit_ctor_with_cpy_syntax = %s\n", explicit_ctor_with_cpy_syntax ? "true" : "false");
#endif

  bool need_name = con_init->get_need_name() && !is_ctor_within_new && ( nested_ctor_init_without_arg || con_init->get_is_explicit_cast() || explicit_ctor_with_cpy_syntax );
#if DEBUG__unparseCtorInit
  printf ("  print_ctor_name = %s\n", print_ctor_name ? "true" : "false");
  printf ("  need_name       = %s\n", need_name       ? "true" : "false");
#endif
  if (need_name) {
    SgUnparse_Info info_for_typename(info);
    if (ctor_class || ctor_decl) {
      std::string qualifier = con_init->get_qualified_name_prefix().str();
      bool unparse_ctor_name = ctor_decl != nullptr ? trimCtorNameQual(qualifier) : true;
      curprint(qualifier.c_str());

      if (unparse_ctor_name) {
        SgTemplateInstantiationMemberFunctionDecl * tpl_ctor_decl = isSgTemplateInstantiationMemberFunctionDecl(ctor_decl);
        if (ctor_class) {
          info_for_typename.set_reference_node_for_qualification(con_init);
          info_for_typename.set_SkipClassSpecifier();
          unp->u_type->unparseType(ctor_class->get_type(), info_for_typename);
        } else if (tpl_ctor_decl != nullptr && !ctor_decl->get_declarationModifier().isFriend()) {
          unparseTemplateMemberFunctionName(tpl_ctor_decl, info);
        } else {
          curprint(ctor_decl->get_name());
        }
      }
    } else {
      info_for_typename.unset_isWithType();
      info_for_typename.unset_SkipBaseType();
      info_for_typename.set_reference_node_for_qualification(con_init);
      unp->u_type->unparseType(ctor_type, info_for_typename);
    }
  }

  if (con_init->get_is_used_in_conditional()) curprint(" = ");

  SgExprListExp* ctor_args = con_init->get_args();
  ASSERT_not_null(ctor_args);

  SgUnparse_Info info_for_args(info);
  if (isAssociatedWithCxx11_initializationList(con_init)) {
    info_for_args.set_context_for_added_parentheses(true);
    ROSE_ASSERT(ctor_args->get_expressions().size() == 2);
    SgExpression * init_arg = ctor_args->get_expressions()[0];
    ASSERT_not_null(init_arg);
    unparseExpression(init_arg, info_for_args);

  } else {
    if (ctor_args->get_expressions().empty()) use_braces = false; // FIXME not sure why? That should be okay with modern C++

    bool need_paren = need_name || use_braces || is_ctor_within_new || con_init->get_need_parenthesis_after_name(); // FIXME is need_name relevant here?

    if (print_ctor_name) {
      // FIXME looks like this tries to deal with initializer list but we have use_initlist (see next)
      SgExprListExp* expressionList = isSgExprListExp(ctor_args);
      ASSERT_not_null(expressionList);
      if (!ctor_args->get_expressions().empty()) {
        SgInitializer * initializer = isSgInitializer(ctor_args->get_expressions()[0]);
        if (initializer == nullptr) {
          need_paren = true;
        } else if (ctor_args->get_expressions().size() >= 2) {
          need_paren = true;
        }
      }
    }
#if DEBUG__unparseCtorInit
    printf ("  use_braces   = %s \n", use_braces   ? "true" : "false");
    printf ("  need_paren   = %s \n", need_paren   ? "true" : "false");
#endif

    // FIXME should it simply be `info_for_args.set_context_for_added_parentheses(need_paren);`?
    //       and what about `use_braces` vs `con_init->get_is_braced_initialized()`
    info_for_args.set_context_for_added_parentheses( con_init->get_is_braced_initialized() || (con_init->get_is_explicit_cast() && !use_braces) );
    if (need_paren) curprint(use_braces ? "{" : "(");
    unparseExpression(ctor_args, info_for_args);
    if (need_paren) curprint(use_braces ? "}" : ")");
  }

#if DEBUG__unparseCtorInit
  printf ("Leaving Unparse_ExprStmt::unparseCtorInit \n");
#endif
}
