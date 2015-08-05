
#ifndef __MDCG_MODEL_FIELD_HPP__
#define __MDCG_MODEL_FIELD_HPP__

#include "MDCG/Model/base.hpp"

namespace MDCG {

namespace Model {

template <>
struct node_t<e_model_field> {
  field_symbol_t * symbol;

  type_t type;
};

template <>
struct scope_t<e_model_field> {
  class_t parent;
};

}

}

#endif /* __MDCG_MODEL_FIELD_HPP__ */

