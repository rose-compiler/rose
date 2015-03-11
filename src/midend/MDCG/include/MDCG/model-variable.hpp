/** 
 * \file MDCG/include/MDCG/model-variable.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MDCG_MODEL_VARIABLE_HPP__
#define __MDCG_MODEL_VARIABLE_HPP__

#include "MDCG/model-base.hpp"

namespace MDCG {

namespace Model {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

template <>
struct node_t<e_model_variable> {
  variable_symbol_t * symbol;

  type_t type;
};

template <>
struct scope_t<e_model_variable> {
  namespace_t parent;
};

/** @} */

}

}

#endif /* __MDCG_MODEL_VARIABLE_HPP__ */

