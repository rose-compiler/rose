/** 
 * \file MDCG/include/MDCG/model-method.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MDCG_MODEL_METHOD_HPP__
#define __MDCG_MODEL_METHOD_HPP__

#include "MDCG/Core/model-base.hpp"

namespace MDCG {

namespace Model {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

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

/** @} */

}

}

#endif /* __MDCG_MODEL_METHOD_HPP__ */

