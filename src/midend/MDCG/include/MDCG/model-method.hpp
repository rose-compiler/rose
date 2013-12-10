
#ifndef __MDCG_MODEL__HPP__
#define __MDCG_MODEL__HPP__

#include "MDCG/model-base.hpp"

namespace MDCG {

namespace Model {

template <>
struct node_t<e_model_method> {
  method_symbol_t * symbol;

  type_t * return_type;
  std::vector<type_t  *> args_types;
};

template <>
struct scope_t<e_model_method> {
  namespace_t * parent;

  std::vector<variable_t *>  variable_children;
  std::vector<function_t *>  function_children;
  std::vector<type_t *>      type_children;
  std::vector<class_t *>     class_children;
  std::vector<namespace_t *> namespace_children;
};

}

}

#endif /* __MDCG_MODEL__HPP__ */

