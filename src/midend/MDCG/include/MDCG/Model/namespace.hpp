
#ifndef __MDCG_MODEL_NAMESPACE_HPP__
#define __MDCG_MODEL_NAMESPACE_HPP__

#include "MDCG/Model/base.hpp"

namespace MDCG {

namespace Model {

template <>
struct node_t<e_model_namespace> {
  namespace_symbol_t * symbol;
};

template <>
struct scope_t<e_model_namespace> {
  namespace_t parent;

  std::vector<variable_t>  variable_children;
  std::vector<function_t>  function_children;
  std::vector<type_t>      type_children;
  std::vector<class_t>     class_children;
  std::vector<namespace_t> namespace_children;
};

}

}

#endif /* __MDCG_MODEL_NAMESPACE_HPP__ */

