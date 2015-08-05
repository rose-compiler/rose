
#ifndef __MDCG_MODEL_FUNCTION_HPP__
#define __MDCG_MODEL_FUNCTION_HPP__

#include "MDCG/Model/base.hpp"

namespace MDCG {

namespace Model {

template <>
struct node_t<e_model_function> {
  function_symbol_t * symbol;

  type_t return_type;
  std::vector<type_t> args_types;
};

template <>
struct scope_t<e_model_function> {
  namespace_t parent;
};

}

}

#endif /* __MDCG_MODEL_FUNCTION_HPP__ */

