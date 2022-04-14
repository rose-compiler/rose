
#ifndef ROSE_CODEGEN_OBJECT_H
#define ROSE_CODEGEN_OBJECT_H

#include "sage3basic.h" // TODO ROSE header that contains only forward of nodes and VariantT (maybe also template-meta for hierarchy and traversal)

namespace Rose { namespace CodeGen {

enum class Object {
  a_namespace,
  a_class,
  a_typedef,
  a_variable,
  a_function
};

template <Object otag>
struct object_helper;

template <Object otag> using symbol_t      = typename object_helper<otag>::symbol_t;
template <Object otag> using reference_t   = typename object_helper<otag>::ref_t;
template <Object otag> using declaration_t = typename object_helper<otag>::decl_t;

template <Object otag> constexpr auto is_template_symbol_variant = object_helper<otag>::is_template_symbol_variant;

template <>
struct object_helper<Object::a_namespace> {
  using decl_t   = SgNamespaceDeclarationStatement;
  using symbol_t = SgNamespaceSymbol;
  using type_t   = void;

  using ref_t    = void;

  static constexpr bool is_template_symbol_variant(VariantT v) { return v == false; }
};

template <>
struct object_helper<Object::a_class> {
  using decl_t   = SgClassDeclaration;
  using symbol_t = SgClassSymbol;
  using type_t   = SgClassType;

  using ref_t    = type_t;

  static constexpr bool is_template_symbol_variant(VariantT v) { return v == V_SgTemplateClassSymbol; }
};

template <>
struct object_helper<Object::a_typedef> {
  using decl_t   = SgClassDeclaration;
  using symbol_t = SgTypedefSymbol;
  using type_t   = SgTypedefType;

  using ref_t    = type_t;

  static constexpr bool is_template_symbol_variant(VariantT v) { return v == V_SgTemplateTypedefSymbol; }
};

template <>
struct object_helper<Object::a_variable> {
  using decl_t   = SgVariableDeclaration;
  using symbol_t = SgVariableSymbol;
  using type_t   = void;

  using ref_t      = SgVarRefExp;

  static constexpr bool is_template_symbol_variant(VariantT v) { return v == V_SgTemplateVariableSymbol; }
};

template <>
struct object_helper<Object::a_function> {
  using decl_t   = SgFunctionDeclaration;
  using symbol_t = SgFunctionSymbol;
  using type_t   = void;

  using ref_t      = SgFunctionRefExp;

  static constexpr bool is_template_symbol_variant(VariantT v) { return v == V_SgTemplateFunctionSymbol || v == V_SgTemplateMemberFunctionSymbol; }
};

} }

#endif /* ROSE_CODEGEN_OBJECT_H */

