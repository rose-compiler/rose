
#include "sage3basic.h"

#include "MDCG/Model/field.hpp"
#include "MDCG/Model/class.hpp"
#include "MDCG/Model/type.hpp"

#include <cassert>

namespace MDCG {

namespace Model {

Model::class_t node_t<e_model_field>::getBaseClass(std::string field_name, std::string class_name) {
  if (!field_name.empty() && symbol->get_name().getString() != field_name) return NULL;

  if (type == NULL || type->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type) return NULL;

  if (!class_name.empty() && type->node->base_class->node->symbol->get_name().getString() != class_name) return NULL;

  return type->node->base_class;
}

Model::class_t node_t<e_model_field>::getBaseClassForPointerOnClass(std::string field_name, std::string class_name) {
  /// match 'struct class_name * field_name;'

  if (!field_name.empty() && symbol->get_name().getString() != field_name) return NULL;

  MDCG::Model::type_t type_ = type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type) return NULL;
  type_ = type_->node->base_type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type) return NULL;

  Model::class_t res = type_->node->base_class;

  if (!class_name.empty() && res->node->symbol->get_name().getString() != class_name) return NULL;

  return res;
}

Model::class_t node_t<e_model_field>::getBaseClassForArrayOnClass(std::string field_name, std::string class_name) {
  /// match 'struct class_name field_name[size];'

  if (!field_name.empty() && symbol->get_name().getString() != field_name) return NULL;

  MDCG::Model::type_t type_ = type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_array_type) return NULL;
  type_ = type_->node->base_type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type) return NULL;

  Model::class_t res = type_->node->base_class;

  if (!class_name.empty() && res->node->symbol->get_name().getString() != class_name) return NULL;

  return res;
}

Model::class_t node_t<e_model_field>::getBaseClassForTypedefOnPointerOnClass(std::string field_name, std::string class_name) {
  /// match 'typedef struct class_name * typedef_name; typedef_name field_name;'

  if (!field_name.empty() && symbol->get_name().getString() != field_name) return NULL;

  MDCG::Model::type_t type_ = type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_typedef_type) return NULL;
  type_ = type_->node->base_type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type) return NULL;
  type_ = type_->node->base_type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type) return NULL;

  Model::class_t res = type_->node->base_class;

  if (!class_name.empty() && res->node->symbol->get_name().getString() != class_name) return NULL;

  return res;
}


Model::class_t node_t<e_model_field>::getBaseClassForPointerOnTypedefOnPointerOnClass(std::string field_name, std::string class_name) {
  /// match 'typedef struct class_name * typedef_name; typedef_name * field_name;'

  if (!field_name.empty() && symbol->get_name().getString() != field_name) return NULL;

  MDCG::Model::type_t type_ = type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type) return NULL;
  type_ = type_->node->base_type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_typedef_type) return NULL;
  type_ = type_->node->base_type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type) return NULL;
  type_ = type_->node->base_type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type) return NULL;

  Model::class_t res = type_->node->base_class;

  if (!class_name.empty() && res->node->symbol->get_name().getString() != class_name) return NULL;

  return res;
}

Model::class_t node_t<e_model_field>::getBaseClassForArrayOnTypedefOnPointerOnClass(std::string field_name, std::string class_name) {
  /// match 'typedef struct class_name * typedef_name; typedef_name field_name[size];'

  if (!field_name.empty() && symbol->get_name().getString() != field_name) return NULL;

  MDCG::Model::type_t type_ = type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_array_type) return NULL;
  type_ = type_->node->base_type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_typedef_type) return NULL;
  type_ = type_->node->base_type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_pointer_type) return NULL;
  type_ = type_->node->base_type;
  if (type_ == NULL || type_->node->kind != MDCG::Model::node_t<MDCG::Model::e_model_type>::e_class_type) return NULL;

  Model::class_t res = type_->node->base_class;

  if (!class_name.empty() && res->node->symbol->get_name().getString() != class_name) return NULL;

  return res;
}

}

}

