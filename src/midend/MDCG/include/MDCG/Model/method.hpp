
#ifndef __MDCG_MODEL_METHOD_HPP__
#define __MDCG_MODEL_METHOD_HPP__

#include "MDCG/Model/base.hpp"

namespace MDCG {

namespace Model {

template <>
struct node_t<e_model_method> {
  method_symbol_t * symbol;

  type_t return_type;
  std::vector<type_t> args_types;
};

template <>
struct scope_t<e_model_method> {
  class_t parent;
};

}

}

#endif /* __MDCG_MODEL_METHOD_HPP__ */

