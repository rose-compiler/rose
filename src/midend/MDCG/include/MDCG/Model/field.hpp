
#ifndef __MDCG_MODEL_FIELD_HPP__
#define __MDCG_MODEL_FIELD_HPP__

#include "MDCG/Model/base.hpp"

namespace MDCG {

namespace Model {

template <>
struct node_t<e_model_field> {
  field_symbol_t * symbol;

  type_t type;

  /// match 'struct class_name field_name;'
  Model::class_t getBaseClass(std::string field_name, std::string class_name);

  /// match 'struct class_name * field_name;'
  Model::class_t getBaseClassForPointerOnClass(std::string field_name, std::string class_name);

  /// match 'struct class_name field_name[size];'
  Model::class_t getBaseClassForArrayOnClass(std::string field_name, std::string class_name);

  /// match 'typedef struct class_name * typedef_name; typedef_name field_name;'
  Model::class_t getBaseClassForTypedefOnPointerOnClass(std::string field_name, std::string class_name);

  /// match 'typedef struct class_name * typedef_name; typedef_name * field_name;'
  Model::class_t getBaseClassForPointerOnTypedefOnPointerOnClass(std::string field_name, std::string class_name);

  /// match 'typedef struct class_name * typedef_name; typedef_name field_name[size];'
  Model::class_t getBaseClassForArrayOnTypedefOnPointerOnClass(std::string field_name, std::string class_name);
};

template <>
struct scope_t<e_model_field> {
  class_t parent;
};

}

}

#endif /* __MDCG_MODEL_FIELD_HPP__ */

