#ifndef ROSE_CodeGen_Object_H
#define ROSE_CodeGen_Object_H
#include <RoseFirst.h>

// FIXME it is a problem with MSVC ("cannot access protected member")
//#include "Rose/Traits/Describe.h"
#include "sage3basic.hhh"

namespace Rose { namespace CodeGen {

//! The five kind of objects manipulated by Rose::CodeGen::API and associated  Rose::CodeGen::Factory
enum class Object {
  a_namespace,
  a_class,
  a_typedef,
  a_variable,
  a_function
};

template <Object otag>
struct object_helper;

/**
 * The SgSymbol specialization for an Object kind
 * /tparam otag an Object kind
 */
template <Object otag> using symbol_t      = typename object_helper<otag>::symbol_t;

/**
 * The SgReference specialization for an Object kind
 * /tparam otag an Object kind
 */
template <Object otag> using reference_t   = typename object_helper<otag>::ref_t;

/**
 * The SgDeclarationStatement specialization for an Object kind
 * /tparam otag an Object kind
 */
template <Object otag> using declaration_t = typename object_helper<otag>::decl_t;

/**
 * This function returns whether the node variant is a template symbol for an Object kind
 * /tparam otag an Object kind
 */
template <Object otag> constexpr auto is_template_symbol_variant = object_helper<otag>::is_template_symbol_variant;

/**
 * Pointer to an API member
 * /tparam otag an Object kind
 * /tparam API
 */
template <Object otag, typename API> using symref_t = symbol_t<otag> * API::*;

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
  using decl_t   = SgTypedefDeclaration;
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

#endif
