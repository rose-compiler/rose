/** 
 * \file MDCG/lib/code-generator.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "sage3basic.h"

#include "MDCG/Core/static-initializer.hpp"
#include "MFB/Sage/variable-declaration.hpp"

#include <cassert>

namespace MDCG {

/*!
 * \addtogroup grp_mdcg_static_initializer
 * @{
*/

StaticInitializer::StaticInitializer(MFB::Driver<MFB::Sage> & driver) :
  p_driver(driver)
{}

MFB::Driver<MFB::Sage> & StaticInitializer::getDriver() const {
  return p_driver;
}

SgVariableSymbol * StaticInitializer::instantiateDeclaration(std::string decl_name, size_t file_id, SgType * type, SgInitializer * init) const {

  MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(decl_name, type, init, NULL, file_id, false, true);
  MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = p_driver.build<SgVariableDeclaration>(var_decl_desc);

  return var_decl_res.symbol;
}

Model::class_t StaticInitializer::getBaseClass(Model::field_t field, std::string field_name, std::string class_name) {
  assert(field_name.empty() || field->node->symbol->get_name().getString() == field_name);
  assert(field->node->type != NULL && field->node->type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);
  assert(class_name.empty() || field->node->type->node->base_class->node->symbol->get_name().getString() == class_name);
  return field->node->type->node->base_class;
}

Model::class_t StaticInitializer::getBaseClassForPointerOnClass(Model::field_t field, std::string field_name, std::string class_name) {
  /// match 'struct class_name * field_name;'

  assert(field_name.empty() || field->node->symbol->get_name().getString() == field_name);

  MDCG::Model::type_t type = field->node->type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
  type = type->node->base_type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);

  Model::class_t res = type->node->base_class;

  assert(class_name.empty() || res->node->symbol->get_name().getString() == class_name);

  return res;
}

Model::class_t StaticInitializer::getBaseClassForArrayOnClass(Model::field_t field, std::string field_name, std::string class_name) {
  /// match 'struct class_name field_name[size];'

  assert(field_name.empty() || field->node->symbol->get_name().getString() == field_name);

  MDCG::Model::type_t type = field->node->type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_array_type);
  type = type->node->base_type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);

  Model::class_t res = type->node->base_class;

  assert(class_name.empty() || res->node->symbol->get_name().getString() == class_name);

  return res;
}

Model::class_t StaticInitializer::getBaseClassForTypedefOnPointerOnClass(Model::field_t field, std::string field_name, std::string class_name) {
  /// match 'typedef struct class_name * typedef_name; typedef_name field_name;'

  assert(field_name.empty() || field->node->symbol->get_name().getString() == field_name);

  MDCG::Model::type_t type = field->node->type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_typedef_type);
  type = type->node->base_type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
  type = type->node->base_type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);

  Model::class_t res = type->node->base_class;

  assert(class_name.empty() || res->node->symbol->get_name().getString() == class_name);

  return res;
}


Model::class_t StaticInitializer::getBaseClassForPointerOnTypedefOnPointerOnClass(Model::field_t field, std::string field_name, std::string class_name) {
  /// match 'typedef struct class_name * typedef_name; typedef_name * field_name;'

  assert(field_name.empty() || field->node->symbol->get_name().getString() == field_name);

  MDCG::Model::type_t type = field->node->type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
  type = type->node->base_type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_typedef_type);
  type = type->node->base_type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
  type = type->node->base_type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);

  Model::class_t res = type->node->base_class;

  assert(class_name.empty() || res->node->symbol->get_name().getString() == class_name);

  return res;
}

Model::class_t StaticInitializer::getBaseClassForArrayOnTypedefOnPointerOnClass(Model::field_t field, std::string field_name, std::string class_name) {
  /// match 'typedef struct class_name * typedef_name; typedef_name field_name[size];'

  assert(field_name.empty() || field->node->symbol->get_name().getString() == field_name);

  MDCG::Model::type_t type = field->node->type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_array_type);
  type = type->node->base_type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_typedef_type);
  type = type->node->base_type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type);
  type = type->node->base_type;
  assert(type != NULL && type->node->kind == MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type);

  Model::class_t res = type->node->base_class;

  assert(class_name.empty() || res->node->symbol->get_name().getString() == class_name);

  return res;
}

/** @} */

}
