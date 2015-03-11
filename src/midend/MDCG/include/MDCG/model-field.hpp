/** 
 * \file MDCG/include/MDCG/model-field.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MDCG_MODEL_FIELD_HPP__
#define __MDCG_MODEL_FIELD_HPP__

#include "MDCG/model-base.hpp"

namespace MDCG {

namespace Model {

/*!
 * \addtogroup grp_mdcg_model
 * @{
*/

template <>
struct node_t<e_model_field> {
  field_symbol_t * symbol;

  type_t type;
};

template <>
struct scope_t<e_model_field> {
  class_t parent;
};

/** @} */

}

}

#endif /* __MDCG_MODEL_FIELD_HPP__ */

