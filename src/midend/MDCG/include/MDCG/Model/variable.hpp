
#ifndef __MDCG_MODEL_VARIABLE_HPP__
#define __MDCG_MODEL_VARIABLE_HPP__

#include "MDCG/Model/base.hpp"

namespace MDCG {

namespace Model {

template <>
struct node_t<e_model_variable> {
  variable_symbol_t * symbol;

  type_t type;
};

template <>
struct scope_t<e_model_variable> {
  namespace_t parent;
};

}

}

#endif /* __MDCG_MODEL_VARIABLE_HPP__ */

